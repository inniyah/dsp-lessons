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

#include "xforms-1.0.93sp1_ext/forms.h"
#include "lxforms.h"
#include "lxa.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

short vbuf[VBUF_SIZE];



void fillxy() {
	int chan[2], cont,ch;
	short val;
	float mult[2];
    val=2.0;
	chan[0]=HOR.trig_chan;
	chan[1]=HOR.trig_chan==0?1:0;
	for(ch=0;ch<2;ch++) {
		mult[ch]=BASE.reference/1024.0;
		if (CHAN.inv[ch]) mult[ch]*=-1;
		if (CHAN.on[chan[ch]]==0) mult[ch]=0;
	}
	for (cont=0;cont<BASE.disp_size;cont++) {
     	datx[0][cont]=cont*1000.0/BASE.sampling_rate;
     	datx[1][cont]=(cont+0.5)*1000.0/BASE.sampling_rate;
    }
	if(BASE.op_mode==1) {				
		if(DEFAULT_ADD_MODE==0) {
        	for (cont=0;cont<BASE.disp_actual;cont++) {
            	daty[0][cont]=
					vbuf[BASE.vstart+2*cont  ]*mult[0]+
					vbuf[BASE.vstart+2*cont+1]*mult[1];
			}
		}
		else {
 			for (cont=0;cont<BASE.disp_size;cont++) {
     			datx[0][cont]=(cont+0.25)*1000.0/BASE.sampling_rate;
    		}
			if(BASE.vstart>0) {
				for (cont=0;cont<BASE.disp_actual-1;cont++) {
					daty[0][cont]=0.25*((3.0*vbuf[BASE.vstart+2*cont]+vbuf[BASE.vstart+2*cont+2])*mult[0]
						   +(3.0*vbuf[BASE.vstart+2*cont+1]+vbuf[BASE.vstart+2*cont-1])*mult[1]);				
				}
			}
			else {
			// first data point is special
				daty[0][0]=0.25*((3.0*vbuf[BASE.vstart]+vbuf[BASE.vstart+2])*mult[0]
						   +4.0*vbuf[BASE.vstart+1]*mult[1]);
				for (cont=1;cont<BASE.disp_actual-1;cont++) {
					daty[0][cont]=0.25*((3.0*vbuf[BASE.vstart+2*cont]+vbuf[BASE.vstart+2*cont+2])*mult[0]
						   +(3.0*vbuf[BASE.vstart+2*cont+1]+vbuf[BASE.vstart+2*cont-1])*mult[1]);				
				}
			}
		}
		if(BASE.disp_actual<BASE.disp_size) {
			for (cont=BASE.disp_actual;cont<BASE.disp_size;cont++) {
				daty[0][cont]=0;
				}
			}
    }
	else {
			for (cont=0;cont<BASE.disp_actual;cont++) {
			daty[0][cont]=vbuf[BASE.vstart+2*cont ]*mult[0];
			daty[1][cont]=vbuf[BASE.vstart+2*cont+1]*mult[1];
			}
			if(BASE.disp_actual<BASE.disp_size) {
				for (cont=BASE.disp_actual;cont<BASE.disp_size;cont++) {
				daty[0][cont]=0;
				daty[1][cont]=0;
				}
			}
	}
}

// fills vbuf for increasing time
int fillvbufp(int vshift) {
	int ifsize=2*BASE.ifsize;  
//	if(BASE.ifile==1) {
		if(BASE.ifdatapos+vshift<ifsize) { 
			BASE.ifdatapos=BASE.ifdatapos+vshift;
			int start=BASE.ifdatapos;
			fseek(BASE.ihandle,2*start,SEEK_SET);
			int stop=start+VBUF_SIZE;
			if(stop>ifsize) stop=ifsize;
			if(fread(&vbuf,stop-start,2,BASE.ihandle)==0) {
				printf("error reading file (1)\n");
				fclose(BASE.ihandle);
				BASE.ifile=2;
				return -1;
			}
			else { 
				BASE.vcount=stop-start;
				BASE.vstart=0;
				BASE.ifvb_first=start;
				BASE.ifvb_last=stop;
				BASE.disp_max=(BASE.vcount-BASE.vstart)/2;
			}
		}
		else printf("end of file reached\n");
	return 0;
}
	

// fills vbuf for decreasing time
int fillvbufn(int vshift) {
	int ifsize=2*BASE.ifsize;  
	int start,stop;
	if(BASE.ifdatapos>0) {
	 	if(vshift<BASE.ifdatapos) { 
			start=BASE.ifdatapos=BASE.ifdatapos-vshift;
			fseek(BASE.ihandle,2*start,SEEK_SET);
		}
		else {
			start=BASE.ifdatapos=0;
//    		printf("partial left shift\n");	
			rewind(BASE.ihandle);
		}	
		stop=start+VBUF_SIZE;
		if(stop>ifsize) stop=ifsize;	
		if(fread(&vbuf,stop-start,2,BASE.ihandle)==0) {
			printf("error reading file (1)\n");
			fclose(BASE.ihandle);
			BASE.ifile=2;
			return -1;
		}
		else { 
			BASE.vcount=stop-start;
			BASE.vstart=0;
			BASE.ifvb_first=start;
			BASE.ifvb_last=stop;
			BASE.disp_max=(BASE.vcount-BASE.vstart)/2;
		}
	}
	else printf("beginning of file reached\n");
	return 0;
}

// fills vbuf
int fillvbuf() {
	int ifsize=2*BASE.ifsize;  
	if(BASE.ifdatapos<ifsize) { 
		int start=BASE.ifdatapos;
		fseek(BASE.ihandle,2*start,SEEK_SET);
		int stop=start+VBUF_SIZE;
		if(stop>ifsize) stop=ifsize;
		if(fread(&vbuf,stop-start,2,BASE.ihandle)==0) {
			printf("error reading file (1)\n");
			fclose(BASE.ihandle);
			BASE.ifile=2;
			return -1;
		}
		else { 
			BASE.vcount=stop-start;
			BASE.vstart=0;
			BASE.ifvb_first=start;
			BASE.ifvb_last=stop;
			BASE.disp_max=(BASE.vcount-BASE.vstart)/2;
		}
	}
	else printf("end of file reached\n");
	return 0;
}
//************************************************************************
//
// reads and displays the content of a raw file
//
int plot_file(FL_OBJECT *xyplot, int button) {
  BASE.disp_size=HOR.T_scale*BASE.sampling_rate/1000+1;
  float mdx[6]={0,0,0,0,0,BASE.disp_size},mdy[6]={0,0,5,-5.5,0,0};
  int channel[2]; 
  int chan,ch,cont;
  char cad[16];
  channel[0]=HOR.trig_chan; 
  channel[1]=HOR.trig_chan==0?1:0;        // not trig channel
  chan=channel[0];


int vshift;
int ifsize=2*BASE.ifsize;     // number of samples 

//////

if(button==0) printf("no function implemented for this action\n");
else if(button==5) { // initial readin:
	BASE.vstart=0;
	BASE.vcount=0;
	BASE.ifdatapos=0;
	BASE.ifvb_first=0;	
//	sprintf(disp,"       %.*f",2,500*(BASE.ifdatapos/BASE.sampling_rate));
//	fl_set_input(lxa->input_Pos,disp);
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));
	BASE.disp_max=100;

	BASE.ihandle=fopen(BASE.ifname,"r");   
// reading enough data to display 5 seconds; thus we do not have to worry about TimeScale.
	if(ifsize<VBUF_SIZE) {
		if(fread(&vbuf,ifsize,2,BASE.ihandle)==0) {
			printf("error reading file (1)\n");
			fclose(BASE.ihandle);
			BASE.ifile=2;
			return 0;
		}
		else { 
//			fclose(BASE.ihandle);
//			BASE.ifile=2;
			BASE.vcount=ifsize;
			BASE.ifvb_last=ifsize;
			BASE.disp_max=2*BASE.vcount;
		}
	}
	else {
		if(fread(&vbuf,VBUF_SIZE,2,BASE.ihandle)==0) {
			printf("error reading file (2)\n");
			fclose(BASE.ihandle);
			BASE.ifile=2;
			return 0;
		}
		else { 
			BASE.ifile=1;
			BASE.vcount=VBUF_SIZE;
			BASE.ifvb_last=VBUF_SIZE;
			BASE.disp_max=VBUF_SIZE;
		}			
	}	
//////
	BASE.disp_actual=BASE.disp_size;
	if(BASE.disp_actual>BASE.disp_max) BASE.disp_actual=BASE.disp_max; 
	fillxy();
}
else if(button==1) {
	vshift=2*(BASE.disp_size/10);
	if(vshift<1) vshift=1;
	if(fillvbufp(vshift)<0) return 0;
	BASE.disp_actual=BASE.disp_size;
	if(BASE.disp_actual>BASE.disp_max) BASE.disp_actual=BASE.disp_max;
	fillxy();
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));
	}
else if(button==2) {
	vshift=16*(BASE.disp_size/10);
	if(vshift<5) vshift=5;	
	if(fillvbufp(vshift)<0) return 0;
	BASE.disp_actual=BASE.disp_size;
	if(BASE.disp_actual>BASE.disp_max) BASE.disp_actual=BASE.disp_max;
	fillxy();
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));
	}	
else if(button==-1) {
	vshift=2*(BASE.disp_size/10);
	if(vshift<1) vshift=1;
	if(fillvbufn(vshift)<0) return 0;
	BASE.disp_actual=BASE.disp_size;
	if(BASE.disp_actual>BASE.disp_max) BASE.disp_actual=BASE.disp_max;
	fillxy();
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));
	}
else if(button==-2) {
	vshift=16*(BASE.disp_size/10);
	if(vshift<5) vshift=5;
	if(fillvbufn(vshift)<0) return 0;
	BASE.disp_actual=BASE.disp_size;
	if(BASE.disp_actual>BASE.disp_max) BASE.disp_actual=BASE.disp_max;
	fillxy();
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));	
	}	
else if(button==3) {	// timer issued refresh
		fillxy();
}
else if(button==4) {  // TimeScale or InputPos or SamplingRate change
	int val=BASE.input_pos*BASE.sampling_rate/500;
	if(val<ifsize && val>=0) {
		BASE.ifdatapos=val;
		if(BASE.ifile==1) { if(fillvbuf()<0) return 0; }
		BASE.disp_actual=BASE.disp_size;
		if(BASE.disp_actual>BASE.disp_max) BASE.disp_actual=BASE.disp_max;
		fillxy();
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));	
	}
	else {
		printf("value outside file size - resetting to previous value\n");
	set_f_counter_Pos(500*(BASE.ifdatapos/BASE.sampling_rate));	
	}		
}
else {
	printf("error: not implemented functionality\n");
	return 0;
}
// calculate and update channel information
float s;
int nbch=2;	
int ds=BASE.disp_actual;
if(BASE.op_mode==1) {
	nbch=1;
	fl_set_object_label(lxa->text_min_chan[1],"*");
	fl_set_object_label(lxa->text_max_chan[1],"*");
	fl_set_object_label(lxa->text_pp_chan[1],"*");
	fl_set_object_label(lxa->text_avg_chan[1],"*");
	fl_set_object_label(lxa->text_rms_chan[1],"*");	
}
for (ch=0; ch<nbch;ch++) {
   	CHAN.max[ch]=-1000000;
   	CHAN.min[ch]=1000000;
   	CHAN.avg[ch]=0;
   	for (cont=0; cont<ds; cont++) {
      	s=daty[ch][cont];
      	if (s<CHAN.min[ch]) CHAN.min[ch]=s;
      	if (s>CHAN.max[ch]) CHAN.max[ch]=s;
	  	CHAN.avg[ch]=CHAN.avg[ch]+s;
    }
    CHAN.pp[ch]=CHAN.max[ch]-CHAN.min[ch];
    CHAN.avg[ch]=CHAN.avg[ch]/ds;

    double rms=rmsq(0,daty[ch][0],daty[ch][1],daty[ch][2]);
	int ds=BASE.disp_actual;
	for (cont=2; cont<=ds; cont++) {
		rms=rms+rmsq(1,daty[ch][cont-2],daty[ch][cont-1],daty[ch][cont]);
	}	
	rms=rms+rmsq(0,daty[ch][ds],daty[ch][ds-1],daty[ch][ds-2]);
	if(rms>0) CHAN.rms[ch]=sqrt(0.5*rms/ds);
	else CHAN.rms[ch]=0;	

  	sprintf(cad,"%.3f",CHAN.min[ch]);
  	fl_set_object_label(lxa->text_min_chan[ch],cad);
  	sprintf(cad,"%.3f",CHAN.max[ch]);
  	fl_set_object_label(lxa->text_max_chan[ch],cad);
  	sprintf(cad,"%.3f",CHAN.pp[ch]);
  	fl_set_object_label(lxa->text_pp_chan[ch],cad);
  	sprintf(cad,"%.3f",CHAN.avg[ch]);
  	fl_set_object_label(lxa->text_avg_chan[ch],cad);
 	sprintf(cad,"%.3f",CHAN.rms[ch]);
  	fl_set_object_label(lxa->text_rms_chan[ch],cad);
}
// delete all texts
	int txt;
	if(BASE.nbtexts>0) {
		for(txt=0;txt<BASE.nbtexts;txt++) {
				fl_delete_xyplot_text(xyplot,BASE.texts[txt]);
		}
		BASE.nbtexts=0;
	}
switch (BASE.op_mode) {
   case OPERATION_MODE_NORMAL: {
       mdx[0]=datx[0][0];
	   mdx[5]=datx[0][BASE.disp_size-1];
	   if(BASE.disp_actual<BASE.disp_size) 
			mdx[1]=mdx[2]=mdx[3]=mdx[4]=datx[0][BASE.disp_actual];
	   fl_set_xyplot_xbounds(xyplot,mdx[0],mdx[5]);
	   if(BASE.ifdatapos>0) 
			sprintf(BASE.tdisp,"time (ms)    (add %.*f)",2,500*(BASE.ifdatapos/BASE.sampling_rate));
	   else
			sprintf(BASE.tdisp,"time (ms)");
	   BASE.nbtexts=1;
	   BASE.texts[0]=BASE.tdisp;
	   fl_add_xyplot_text(xyplot,0.5*mdx[5],-5.5,BASE.tdisp,FL_ALIGN_CENTER,FL_WHITE);
	   fl_set_xyplot_data(xyplot,mdx,mdy,6,"","","");
       fl_add_xyplot_overlay(xyplot,1,mdx,mdy,6,FL_YELLOW);
	   for (ch=0; ch<2;ch++) {
			chan=channel[ch];	   
            for (cont=0; cont<BASE.disp_actual; cont++) {
	           daty[chan][cont]=CHAN.V_pos[chan]+daty[chan][cont]/CHAN.V_scale[chan];
	   	    }
       		if (CHAN.on[chan]) {
           		fl_add_xyplot_overlay(xyplot,chan+2,datx[chan],daty[chan],BASE.disp_actual,CHAN.color[chan]);
           	 }
       		else fl_delete_xyplot_overlay(xyplot,chan+2);
		}
       break;
     }
     case OPERATION_MODE_ADD: {
         mdx[0]=datx[0][0];
         mdx[5]=datx[0][BASE.disp_size-1];
	   	 if(BASE.disp_actual<BASE.disp_size) 
			mdx[1]=mdx[2]=mdx[3]=mdx[4]=datx[0][BASE.disp_actual];
         fl_set_xyplot_xbounds(xyplot,mdx[0],mdx[5]);
	   	 if(BASE.ifdatapos>0) 
			sprintf(BASE.tdisp,"time (ms)    (add %.*f)",2,500*(BASE.ifdatapos/BASE.sampling_rate));
	     else
			sprintf(BASE.tdisp,"time (ms)");
	   BASE.nbtexts=1;
	   BASE.texts[0]=BASE.tdisp;
	   fl_add_xyplot_text(xyplot,0.5*mdx[5],-5.5,BASE.tdisp,FL_ALIGN_CENTER,FL_WHITE);
	   fl_set_xyplot_data(xyplot,mdx,mdy,6,"","","");
       fl_add_xyplot_overlay(xyplot,1,mdx,mdy,6,FL_YELLOW);
         for (cont=0;cont<BASE.disp_actual;cont++) {
             daty[0][cont]=CHAN.V_pos[0]+daty[0][cont]/CHAN.V_scale[0];
         }
         fl_add_xyplot_overlay(xyplot,2,datx[0],daty[0],BASE.disp_actual-1,CHAN.color[0]);
         fl_delete_xyplot_overlay(xyplot,3);
         break;
     }
     case OPERATION_MODE_XY: {
        chan=channel[0];
        mdx[0]=-5.0;
        mdx[5]=5.0;
        fl_set_xyplot_xbounds(xyplot,-5.0,5.0);
	    fl_set_xyplot_alphaxtics(xyplot,
			"@-5.0|@-4.0|@-3.0|-@-2.0|@-1.0|0@0.0|@1.0|+@2.0|@3.0|@4.0|@5.0","");
		CH[0]="Channel 1";
		CH[1]="Channel 2";
		BASE.nbtexts=4;
		BASE.texts[0]=uparrow;
		BASE.texts[1]=dnarrow;
		BASE.texts[2]=CH[0];
		BASE.texts[3]=CH[1];
		fl_add_xyplot_text(xyplot,-4.8,-5.45,uparrow,FL_ALIGN_RIGHT,FL_WHITE);
		fl_add_xyplot_text(xyplot,2.4,-5.45,dnarrow,FL_ALIGN_RIGHT,FL_WHITE);
		fl_add_xyplot_text(xyplot,-4.3,-5.45,CH[channel[0]],FL_ALIGN_RIGHT,FL_WHITE);
		fl_add_xyplot_text(xyplot,2.9,-5.45,CH[channel[1]],FL_ALIGN_RIGHT,FL_WHITE);
		fl_set_xyplot_data(xyplot,mdx,mdy,6,"","","");
        fl_add_xyplot_overlay(xyplot,1,mdx,mdy,6,FL_YELLOW); 	   
		for (ch=0; ch<2;ch++) {
			chan=channel[ch];	   
            for (cont=0; cont<BASE.disp_actual; cont++) {
	           daty[chan][cont]=CHAN.V_pos[chan]+daty[chan][cont]/CHAN.V_scale[chan];
	   	    }
		 }
         fl_add_xyplot_overlay(xyplot,2,daty[channel[1]],daty[channel[0]],BASE.disp_actual,CHAN.color[channel[0]]);
         fl_delete_xyplot_overlay(xyplot,3);
         break;
     }
}   // switch
  return 1;
}



