/*
 *
 * This file is part of the LXARDOSCOPE package.
 *
 * LXARDOSCOPE is an Arduino based oscilloscope for Linux, using the Xforms library.
 *
 * Copyright (C) 2011 Oskar Leuthold
 * 
 * LXARDOSCOPE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LXARDOSCOPE is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with LXARDOSCOPE; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */


#include <forms.h>
#include "lxforms.h"
#include "lxa.h"
#include <termios.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>


//*****************************************************************
// serial port setup adapted from 
// http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html
// 

struct termios oldtio,newtio;

int open_dev()
{
int BAUD;
printf("trying to open device <%s>\n",BASE.device_name);
        BASE.dev_h = open(BASE.device_name, O_RDWR | O_NOCTTY ); 
        if (BASE.dev_h<0) {perror(BASE.device_name); exit(-1); }
printf("success: device open\n");      
        tcgetattr(BASE.dev_h,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));

		if(BASE.baud_rate==115200) BAUD = B115200;
		else {
			printf("only baud rate of 115200 supported\n");
			return 0;
		}
        newtio.c_cflag = BAUD | CRTSCTS | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   	/* inter-character timer unused */
        newtio.c_cc[VMIN]     = 200;   	/* blocking read until 200 chars received */
        								/* however this is ignored; see read command below */
        tcflush(BASE.dev_h, TCIFLUSH);
        tcsetattr(BASE.dev_h,TCSANOW,&newtio);

  return 1;
}


//************************************************************************
/* serial data structure:
   		byte 1  A0 5 most significant bits + 224 (128+64+32)
   		byte 2  A0 5 least significant bits + 96 (64+32)
   		byte 3  A1 5 most significant bits +  96 (64+32)
   		byte 4  A1 5 least significant bits +  96 (64+32)
   for byte 1, legitimate values are between 224 and 255
   for bytes 2, 3 and 4 legitimate values are between 96 and 127
*/


void *lx_run(void *arg)
        {
		printf("data acquisition thread running with pid= %d tid=%ld\n",getpid(),(long int)syscall(224));
        int res,slot,start;
        unsigned char bufn[256];
		unsigned char bufj[4]={0,0,0,0};
		int n=0,k=0,j=0;  
		int rchar;   // characters read
		int inchunk; // counts number of input chunks
        int nbchar=0;
		short ch[2];
		int valid;
		float calcount=0,calval1=0,calval2=0;
		int minval[2]={0,0}, maxval[2]={0,0};	 
        struct timeval tres;
        struct timeval sta;
        struct timeval fin;
		slot=BASE.rchunk%RBUF_NB_CHUNKS;
		start=slot*RBUF_CHUNK_SIZE;
		BASE.pulse=0;
		BASE.orchunk=0;  //  old rchunk value, for pulse display
		if (open_dev()==0) {
			return 0;
		}
		// flushing helps for startup
		tcflush(BASE.dev_h, TCIFLUSH);
		gettimeofday(&sta, NULL);
        res = read(BASE.dev_h,bufn,256);
		rchar=res;
		inchunk=1;
		while (BASE.threadenb) {
			if(bufn[n]>223) j=0;
			bufj[j]=bufn[n];
			j++;
			n++;
			if(n>=res) {
				res = read(BASE.dev_h,bufn,256);
				nbchar=nbchar+res;
				n=0;
			}
			if(j==4) { 
				valid=1;
				ch[0]=0;
				ch[1]=0;
				ch[0]=(bufj[0]-224)*32;
				if(bufj[1]>95 && bufj[1]<128) ch[0]=ch[0]+bufj[1]-96;
				else {
					printf("error: byte [%d]=%d #2; samples ignored\n",n,bufj[1]);
					valid=0;
				}
				if(bufj[2]>95 && bufj[2]<128) ch[1]=(bufj[2]-96)*32;
				else {
					printf("error: byte [%d]=%d #3; samples ignored\n",n,bufj[2]);
					valid=0;
				}
				if(bufj[3]>95 && bufj[3]<128) ch[1]=ch[1]+bufj[3]-96;
				else {
					printf("error: byte [%d]=%d #4; samples ignored\n",n,bufj[3]);
					valid=0;
				}
				if(valid) {
					if(ch[0]==0) minval[0]++;
					if(ch[1]==0) minval[1]++;
					if(ch[0]==1023) maxval[0]++;
					if(ch[1]==1023) maxval[1]++;
					rbuf[start+k]=(short)(ch[0]-CHAN.offset[0]);
					rbuf[start+k+1]=(short)(ch[1]-CHAN.offset[1]);
					if(BASE.gndcal) {
						calval1=calval1+(float)ch[0];
						calval2=calval2+(float)ch[1];
						calcount++;
					}
					k=k+2;

				if(BASE.record==1) {
					int e;
   					if((e=write(BASE.rhandle,&ch,4))<0) {
      					fprintf(stderr, "write failed %s\n", strerror(errno));
      					close(BASE.rhandle);
      					exit(1);
    				}
				}
				if(k>=RBUF_CHUNK_SIZE) {
					k=0;
					BASE.orchunk=BASE.rchunk;
					BASE.rchunk++;
					slot=BASE.rchunk%RBUF_NB_CHUNKS;
					start=slot*RBUF_CHUNK_SIZE;

					if(minval[0]>0) CHAN.minvalue[0]=1;
					if(maxval[0]>0) CHAN.maxvalue[0]=1;
					if(minval[1]>0) CHAN.minvalue[1]=1;
					if(maxval[1]>0) CHAN.maxvalue[1]=1;
					minval[0]=0; minval[1]=0; maxval[0]=0; maxval[1]=0;

					if(BASE.rchunk%40==20) {
						gettimeofday(&fin, NULL);
    					timeval_subtract(&tres,&fin,&sta);
						double tspan=(double)tres.tv_sec+1e-6*(double)tres.tv_usec;
// update the effective rate:
						BASE.charspersec=nbchar/tspan;
// charspersec=4*sampling_rate
// with some moving average:
						BASE.sampling_rate=(4.0*BASE.sampling_rate+BASE.charspersec)/8.0;
//						set_lxa_time_scale();
						if(BASE.sampling_rate<BASE.baud_rate/50) {
							printf("serial port communication too slow\n");
							printf("  receiving %d characters per second\n",(int)BASE.charspersec);
							printf("  expecting at least %d characters per second\n",
										(int)(rint(BASE.baud_rate/12.5)));
						}
						if(BASE.report_timing==1) {
		printf("***** elapsed time=%2.3f, %d bytes received, %d bytes per second, sampling rate= %d\n",
							tspan,nbchar,(int)rint(BASE.charspersec),(int)rint(BASE.sampling_rate));
							}
						gettimeofday(&sta, NULL);
						nbchar=0;
					}//
					if(BASE.gndcal>1) {
						if(BASE.rchunk%10==5) {
							CHAN.offset[0]=calval1/calcount;
							CHAN.offset[1]=calval2/calcount;
							if(BASE.gndcal>1) printf("          offset for channel A0 = %3.1f, for A1 = %3.1f\n",
								CHAN.offset[0],CHAN.offset[1]); 
							BASE.gndcal++;
							if(BASE.gndcal>4) {
								BASE.gndcal=1;
								calval1=0;
								calval2=0;
								calcount=0;
							}
						}
					} //if(BASE.gndcal)
			    }//	if(k>=RBUF_CHUNK_SIZE)	
			} //if(valid)
		} // if(j>3)
		else if(j>1000) {
			printf("no valid data found in 1000 samples\n");
			j=10;
		}
	} //while (BASE.threadenb)


tcsetattr(BASE.dev_h,TCSANOW,&oldtio);
return 0;
}


