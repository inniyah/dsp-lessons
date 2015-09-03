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
#include "iscaler.h"
#include "lxforms.h"
#include "lxa.h"
#include <sys/dir.h>
#include <stdlib.h>


#ifndef _REENTRANT
#error ACK! You need to compile with _REENTRANT defined since this uses threads
#endif

// see lxa.h for an explanation of these sizes
float datx[2][DATXY_SIZE];	
float daty[2][DATXY_SIZE];
float tbuf[2][TBUF_SIZE];   	
signed short rbuf[RBUF_SIZE];	


FD_lxa *lxa=NULL;
FD_about *about=NULL;

base_info BASE;	
horizontal_info	HOR;
channel_info CHAN;

int at_close_routine(FL_FORM *frm, void *data);

//void set_lxa_time_scale() {
//   BASE.disp_size=HOR.T_scale*BASE.sampling_rate/1000+1;   
//   fl_set_scaler_value(lxa->counter_Tscale,HOR.T_scale);
//}

void find_tty_devices()
{
        int i=1;
		int done=0;
        DIR *pDIR;
        struct dirent *entry;
        pDIR=opendir("/dev");
        if(pDIR!=NULL) {
                while (done==0) {
					entry = readdir(pDIR);
					if(entry!=NULL) {
               			if (strncmp(entry->d_name, "ttyS", 4)== 0) {
                    		strcpy(BASE.devices[i],"/dev/");
							strcat(BASE.devices[i],entry->d_name);
							i++;
               			}
               			else if (strncmp(entry->d_name, "ttyACM", 6)== 0) {
                    		strcpy(BASE.devices[i],"/dev/");
							strcat(BASE.devices[i],entry->d_name);
						i++;
                		}
               			else if (strncmp(entry->d_name, "ttyUSB", 6)== 0) {
                    		strcpy(BASE.devices[i],"/dev/");
							strcat(BASE.devices[i],entry->d_name);
						i++;
                		}
						if(i>9) done=1;
                	}
					else done=1;
                }
                closedir(pDIR);
                sprintf(BASE.devices[0],"%d",i-1);
        }
}


int init_lxa(char *device_name) {  
   	const char *undef="undefined";
   	const char *tfile="file";
	const char *tty = "/dev/tty";
   	int select,n,ch;
	char str[20];
   	find_tty_devices();
   	fl_clear_choice(lxa->droplist_dev);
   	strcpy(BASE.device_name,device_name);
	BASE.baud_rate=DEFAULT_BAUD_RATE;
  	if(strcmp(device_name,undef)==0) {     // no external input
   		fl_addto_choice(lxa->droplist_dev,undef);
   		fl_set_object_color(lxa->droplist_dev,FL_DARKORANGE,FL_INACTIVE);
   	}
	else if(strlen(device_name)<9 || strlen(device_name)>16 || strncmp(device_name,tty,8)!=0) {
       	printf("Error: device name %s is not valid\n",device_name); 
      	printf("       expecting string beginning with /dev/tty\n");
       	printf("please select device on lxa Graphical User Interface\n\n");
        select=fl_addto_choice(lxa->droplist_dev,undef);
        fl_set_object_color(lxa->droplist_dev,FL_DARKORANGE,FL_INACTIVE);
	}
   	else {
        if (open_dev()==0) {
   	        printf("can not initialize device %s\n",device_name);
            select=fl_addto_choice(lxa->droplist_dev,undef);
            fl_set_object_color(lxa->droplist_dev,FL_DARKORANGE,FL_INACTIVE);
        }
        else {
            fl_addto_choice(lxa->droplist_dev,device_name);
            fl_set_object_color(lxa->droplist_dev,FL_MCOL,FL_INACTIVE);
     		fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);  
			fl_activate_object(lxa->button_run);
			fl_show_object(lxa->button_run);
			fl_activate_object(lxa->button_gndcal);
			fl_activate_object(lxa->report_timing);		
	 		BASE.rchunk=0;
     		BASE.cchunk=0;
	 		BASE.threadenb=1;
			BASE.run=1;
     		fl_deactivate_object(lxa->record);	
        }
   	}
   	int nbdev=atoi(BASE.devices[0]);
   	for(n=1;n<=nbdev;n++) {
         if(strcmp(device_name,BASE.devices[n])!=0)
         fl_addto_choice(lxa->droplist_dev,BASE.devices[n]);  
		 } 
   	fl_addto_choice(lxa->droplist_dev,tfile); 
	BASE.reference= DEFAULT_REFERENCE;
	BASE.sampling_rate=DEFAULT_SAMPLING_RATE;
	BASE.op_mode=OPERATION_MODE_NORMAL;
	HOR.trig_chan=DEFAULT_TRIG_CHANNEL;
	HOR.T_pos=DEFAULT_TIME_OFFSET;
	HOR.T_scale=DEFAULT_TIME_SCALE;
	HOR.trig_level=DEFAULT_TRIG_LEVEL;
	HOR.trig_mode=ASC_TRIG;
	HOR.trig_enb=TRIG_OFF;
	HOR.hyst=DEFAULT_HYST;
	BASE.delta=0;
   	BASE.trig=0;
   	BASE.record=0;
   	BASE.ifile=0;
   	BASE.report_timing=0;  
	BASE.disp_actual=0;
	BASE.nbtexts=0;
	CHAN.color[0]=FL_RED;
	CHAN.color[1]=FL_GREEN;
	CHAN.V_pos[0]=2.0;
	CHAN.V_pos[1]=-2.0;
	for(ch=0;ch<2;ch++) {
		CHAN.on[ch]=1;
		CHAN.V_scale[ch]=DEFAULT_V_SCALE;
		CHAN.inv[ch]=0;
		CHAN.minvalue[ch]=0;
		CHAN.maxvalue[ch]=0;
	}
//   	set_lxa_time_scale();
//	set_counter_Tscale(lxa->counter_Tscale);
	sprintf(str, " %.*f", 1,DEFAULT_TIME_SCALE);
	fl_set_iscaler_input(lxa->counter_Tscale,str);	
	fl_set_iscaler_input(lxa->f_counter_Tscale,str);	
	sprintf(str, "   %.*f", 1,DEFAULT_V_SCALE);
	fl_set_iscaler_input(lxa->counter_Vscale[0],str);
	fl_set_iscaler_input(lxa->counter_Vscale[1],str);
   	printf("** initial sampling rate set to %d\n",(int)BASE.sampling_rate);
	fl_show_object(lxa->trigger_Group);
	fl_hide_object(lxa->filehandling_Group);
   return 1;
 }


int main(int argc, char *argv[])
{
    FD_lxa *fd_lxa;
    FD_about *fd_about;
    fl_initialize(&argc,argv,"lxardoscope",0,0);
    fd_lxa = create_form_lxa();
    fd_about = create_form_about();
    about=fd_about;
    lxa=fd_lxa;
	if (argc > 1) init_lxa(argv[1]);
	else init_lxa("undefined");
// show the GUI; its title is defined here
   fl_show_form(fd_lxa->lxa,FL_PLACE_CENTERFREE,FL_FULLBORDER,"lxardoscope_0.9");
   fl_set_atclose(CB_at_close,NULL);
   fl_do_forms();
   return 0;
}
