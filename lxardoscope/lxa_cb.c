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
#include "xforms-1.0.93sp1_ext/ipushwheel.h"
#include "xforms-1.0.93sp1_ext/iscaler.h"
#include "lxforms.h"
#include "lxa.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>

extern FD_lxa *lxa;
extern FD_about *about;
extern  int alphasort();
extern struct termios oldtio;
void set_lxa_time_scale();
int xyplot_display_handler(FL_OBJECT *,int,FL_Coord,FL_Coord,int,void *);
void prepare_xyplot(FL_OBJECT *xyplot);
void find_tty_devices();
void select_device(char device_name);
pthread_t thread;
int  iret;

void refresh_mouse_info(float wx, float wy)  {
	char cad[16];
	int xind, done;
	float val;
	BASE.wx=wx; 
	if(BASE.wx<datx[0][0]||BASE.wx>datx[0][BASE.disp_actual-1]) {
		fl_set_object_label(lxa->text_mouse_T,"*  ");
		fl_set_object_label(lxa->text_mouse_V[0],"*  ");
		fl_set_object_label(lxa->text_mouse_V[1],"*  ");
	}		
	else {
		switch (BASE.op_mode) {
   			case OPERATION_MODE_NORMAL: {
  				if(HOR.T_scale<10.0) sprintf(cad,"%.3f",wx);
  				else if (HOR.T_scale<100.0) sprintf(cad,"%.2f",wx);
  				else if (HOR.T_scale<1000.0) sprintf(cad,"%.1f",wx);
  				else sprintf(cad,"%.0f",wx);
  				fl_set_object_label(lxa->text_mouse_T,cad);
				xind=0;
				done=0;
				while (done==0) {
					xind++;
	   				if(datx[0][xind]>=BASE.wx) done=1;
					if(xind>BASE.disp_actual-1) done=1;
				}
				val=(BASE.wx-datx[0][xind-1])/(datx[0][xind]-datx[0][xind-1]);
				sprintf(cad,"%.2f",(daty[0][xind-1]-CHAN.V_pos[0]
					+(daty[0][xind]-daty[0][xind-1])*val)*CHAN.V_scale[0]);
  				fl_set_object_label(lxa->text_mouse_V[0],cad);
				xind=0;
				done=0;
				while (done==0) {
					xind++;
	   				if(datx[1][xind]>=BASE.wx) done=1;
					if(xind>BASE.disp_actual-1) done=1;
				}
				val=(BASE.wx-datx[1][xind-1])/(datx[1][xind]-datx[1][xind-1]);
				sprintf(cad,"%.2f",(daty[1][xind-1]-CHAN.V_pos[1]
					+(daty[1][xind]-daty[1][xind-1])*val)*CHAN.V_scale[1]);
  				fl_set_object_label(lxa->text_mouse_V[1],cad);
				break;
			}
			case OPERATION_MODE_ADD: {
  				if(HOR.T_scale<10.0) sprintf(cad,"%.3f",wx);
  				else if (HOR.T_scale<100.0) sprintf(cad,"%.2f",wx);
  				else if (HOR.T_scale<1000.0) sprintf(cad,"%.1f",wx);
  				else sprintf(cad,"%.0f",wx);
  				fl_set_object_label(lxa->text_mouse_T,cad);
				xind=0;
				done=0;
				while (done==0) {
					xind++;
	   				if(datx[0][xind]>=BASE.wx) done=1;
					if(xind>BASE.disp_actual-1) done=1;
				}
				val=(BASE.wx-datx[0][xind-1])/(datx[0][xind]-datx[0][xind-1]);
				sprintf(cad,"%.2f",(daty[0][xind-1]-CHAN.V_pos[0]+
					(daty[0][xind]-daty[0][xind-1])*val)*CHAN.V_scale[0]);
  				fl_set_object_label(lxa->text_mouse_V[0],cad);
				fl_set_object_label(lxa->text_mouse_V[1],"*  ");
				break;
			}
			case OPERATION_MODE_XY: {
				fl_set_object_label(lxa->text_mouse_V[0],"*  ");
				fl_set_object_label(lxa->text_mouse_V[1],"*  ");
				fl_set_object_label(lxa->text_mouse_T,"*  ");
				break;
			}
  		}
	}
}

/* callbacks for lxa GUI */

void CB_logotipo(FL_OBJECT *ob, long data) {
   fl_show_form(about->about,FL_PLACE_CENTERFREE,FL_FULLBORDER,"About...");
}

void CB_button_run(FL_OBJECT *ob, long data)
{
  int b=fl_get_button(ob);   
  const char *undef="undefined";
  if(strcmp(BASE.device_name,undef)==0) {
  	printf("Please select a valid device, then press START\n"); 
  	BASE.run=0;
  	fl_set_button(lxa->button_run,0);
  	fl_set_object_lcol(ob,FL_GREEN);
  	fl_set_object_label(ob,"START");
  }
  else if (b==1) {
     BASE.run=1;
     fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);
     fl_set_object_lcol(ob,FL_RED);
     fl_set_object_label(ob,"STOP");
	 BASE.rchunk=0;
     BASE.cchunk=0;
	 BASE.tchunk=0;
	 BASE.threadenb=1;
     fl_deactivate_object(lxa->record);	
     fl_deactivate_object(lxa->droplist_dev);
	 iret=pthread_create( &thread, NULL, *lx_run,NULL);
	 if(iret==1)      		printf("Thread creation failed: %d\n", iret);
// enable mouse handling on xyplot
	 fl_set_object_posthandler(lxa->xyplot_display,xyplot_display_handler);
  }
  else  if(BASE.run==1) {
     BASE.run=2;
     fl_set_object_lcol(ob,FL_GREEN);
     fl_set_object_label(ob,"START");
	 fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);
	 BASE.threadenb=0;
	 fl_activate_object(lxa->record);
     fl_activate_object(lxa->droplist_dev);
   }
}

void CB_timer_refresh(FL_OBJECT *ob, long data) {
  	if(BASE.ifile) {
		plot_file(lxa->xyplot_display,3);
		fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);
		return;
	} 
    switch (HOR.trig_enb)
    {
     case TRIG_OFF:       
         fl_set_object_color(lxa->indicator_triggered,FL_INACTIVE,FL_INACTIVE);
      break;
     case TRIG_ON: 
          fl_set_object_color(lxa->indicator_triggered,FL_RED,FL_RED);
     break;
    }

  if (BASE.run==1) { 
 	fl_freeze_form(lxa->lxa);
 	pro_display(lxa->xyplot_display,0);
	if(BASE.rchunk>BASE.orchunk) {
			if(BASE.pulse==3) fl_set_object_color(lxa->button_pulse,FL_ORCHID,FL_PALEGREEN);
			else if(BASE.pulse>=6) {
				fl_set_object_color(lxa->button_pulse,FL_PALEGREEN,FL_ORCHID);
				BASE.pulse=0;
			}
			BASE.pulse++;
	}
	if(BASE.gndcal==1) {
		fl_set_object_color(lxa->button_gndcal,FL_MCOL,FL_GREEN);
		fl_set_button(lxa->button_gndcal, 0);
	}
 	fl_unfreeze_form(lxa->lxa);
    fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);
  }
  if(BASE.run==2) {
	fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);
	pro_display(lxa->xyplot_display,1);
  }
}


int xyplot_display_handler(FL_OBJECT *ob,int event,FL_Coord mx,FL_Coord my,int key,void *xev) {
float wx,wy;
switch(event) {
    case FL_PUSH:
           fl_xyplot_s2w(ob,mx,my,&wx,&wy);
           refresh_mouse_info(wx,wy);
            break;
  }
 return 0;
}


// adapted from http://www.opensource.apple.com/source/distcc/distcc-20/src/timeval.c
// modified to leave structures tx and ty intact 
int timeval_subtract (struct timeval *tresult,
                      struct timeval *tx,
                      struct timeval *ty)
     { 
       long int ty_tv_usec=ty->tv_usec;
       long int ty_tv_sec=ty->tv_sec;
       //Perform the carry for the later subtraction by updating y.
       if (tx->tv_usec < ty->tv_usec) {
         int nsec = (ty->tv_usec - tx->tv_usec) / 1000000 + 1;
          ty_tv_usec = ty->tv_usec - 1000000 * nsec;
         ty_tv_sec = ty->tv_sec + nsec;
       }
       if (tx->tv_usec - ty->tv_usec > 1000000) {
         int nsec = (tx->tv_usec - ty->tv_usec) / 1000000;
         ty_tv_usec = ty->tv_usec + 1000000 * nsec;
         ty_tv_sec = ty->tv_sec - nsec;
       }
     
       // Compute the time remaining to wait. tv_usec is certainly positive. 
       tresult->tv_sec = tx->tv_sec - ty_tv_sec;
       tresult->tv_usec = tx->tv_usec - ty_tv_usec;
       // Return 1 if result is negative. 
       return tx->tv_sec < ty->tv_sec;
     }


//**********************************

void CB_counter_Vpos1(FL_OBJECT *ob, long dummy) {
  CHAN.V_pos[0]=fl_get_counter_value(ob);
}

double range(double val, int button) {
int k,nbvals,done;
float vals1[]= 
{ 	0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009,
	0.0010,0.0012,0.0014,0.0016,0.0018,0.0020,0.0022,0.0024,0.0026,0.0030,
	0.0032,0.0034,0.0036,0.0038,0.0040,0.0042,0.0044,0.0046,0.0048,0.0050,
	0.0055,0.0060,0.0065,0.0070,0.0075,0.0080,0.0085,0.0090,0.0095,
	0.01,0.011,0.012,0.013,0.014,0.015,0.016,0.017,0.018,0.019,
	0.02,0.022,0.024,0.026,0.028,0.03,0.032,0.034,0.036,0.038,
	0.04,0.042,0.044,0.046,0.048,0.05,0.055,0.06,0.065,0.07, 
	0.075,0.08,0.085,0.090,0.095,
	0.1,0.105,0.11,0.115,0.12,0.125,0.13,0.135,0.14,0.145,0.15,0.155,0.16,
	0.165,0.17,0.175,0.18,0.185,0.19,0.195,
	0.2,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,
	0.30,0.32,0.34,0.36,0.38,0.40,0.42,0.44,0.46,0.48,
	0.5,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.90,0.95,
	1.0,1.05,1.1,1.15,1.2,1.25,1.3,1.35,1.4,1.45,1.5,1.55,1.6,1.65,1.7,
	1.75,1.8,1.85,1.9,1.95,
	2,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,3.0,3.1,3.2,3.3,3.4,3.5,
	3.6,3.7,3.8,3.9,4,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,5,
	5.2,5.4,5.6,5.8,6,6.2,6.4,6.6,6.8,7,7.2,7.4,7.6,7.8,8,
	8.2,8.4,8.6,8.8,9,9.2,9.4,9.6,9.8,10,
	10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,15.5,16,16.5,17,17.5,18,18.5,
	19,19.5,20,21,22,23,24,25,26,27,28,29,30,32, 34,36,38,40,42,44,46,48,50,
	55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,
	155,160,165,170,175,180,185,190,195,200,210,220,230,240,250,260,270,280,
	290,300, 320,340,360,380,400,420, 440, 460, 480,500,550,600,650,700,750,
	800,850,900,950,1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,1500,
	1550,1600,1650,1700,1750,1800,1850,1900,1950,2000, 2100, 2200,2300,2400,
	2500,2600,2700,2800,2900,3000,3200,3400,3600,3800,4000,4200,4400, 4600,
	4800,5000,5500,6000};
float vals2[]= 
{ 	0.0002,0.0005,0.0010,0.0020,0.0050,0.01,0.02,0.05,0.1,0.2,0.5,1,
	2,5,10,20,50,100,200,500,1000,2000,5000,6000};
if(button>0) val=val+0.0001;
else val=val-0.0001;
if(button==-1 || button==1) {
	nbvals=sizeof(vals1)/sizeof *(vals1);
	done=0;
	k=0;
	while(!done) {
		if(val>vals1[k]) k++;
		else done=1;
		if(k>=nbvals) done=1;
	}
	if(button==-1 && k>1) val=vals1[k-1];
	else if(button==1 && k<nbvals) val=vals1[k];
	else val=0; 
}
if(button==-2 || button==2) {
	nbvals=sizeof(vals2)/sizeof *(vals2);
	done=0;
	k=0;
	while(!done) {
		if(val>vals2[k]) k++;
		else done=1;
		if(k>=nbvals) done=1;
	}
	if(button==-2 && k>1) val=vals2[k-1];
	else if(button==2 && k<nbvals) val=vals2[k];
	else val=0; 
}
return(val);
}




void CB_counter_Vscale1(FL_OBJECT *ob, long dummy) {
  	int digit;
	double val;	
	char str[ 64 ];
  	int button=fl_get_iscaler_button(ob);
	if(button==0){
		val=atof(fl_get_iscaler_input(ob));
		if(strcmp(fl_get_object_label(ob),"Scale: (mV/div)")==0) val=val/1000;
	}
	else val=range(CHAN.V_scale[0],button);
  	if(val==0) val=CHAN.V_scale[0];
  	if(val<0.002) val=0.002;
  	if(val>10) val=10;
	CHAN.V_scale[0]=val;
  	if(val>1.9999) { digit=1; fl_set_object_label(ob,"Scale: (V/div)");}
  	else if(val>0.20001) { digit=2; fl_set_object_label(ob,"Scale: (V/div)");}
  	else if(val>0.00999) { digit=0; val=1000*val; fl_set_object_label(ob,"Scale: (mV/div)");}
  	else { digit=1; val=1000*val; fl_set_object_label(ob,"Scale: (mV/div)");}
	sprintf(str, "%.*f", digit, val);
	if(strlen(str)==1) sprintf(str, "    %.*f", digit, val);
	else if(strlen(str)==2) sprintf(str, "   %.*f", digit, val);
	else if(strlen(str)==3) sprintf(str, "  %.*f", digit, val);
	else if(strlen(str)==4) sprintf(str, " %.*f", digit, val);
	else if(strlen(str)==5) sprintf(str, " %.*f", digit, val);
	fl_set_iscaler_input(ob,str);
}	
		

void CB_check_chan1(FL_OBJECT *ob, long dummy) {
  CHAN.on[0]=fl_get_button(ob);
}

void CB_button_color_chan1(FL_OBJECT *ob, long dummy) {
  CHAN.color[0]=fl_show_colormap(CHAN.color[0]);
  fl_set_object_color(ob,CHAN.color[0],FL_COL1);
}

void CB_button_inv_chan1(FL_OBJECT *ob, long dummy) {
  CHAN.inv[0]=fl_get_button(ob);
}


void CB_counter_Vpos2(FL_OBJECT *ob, long dummy) {
  CHAN.V_pos[1]=fl_get_counter_value(ob);
}

void CB_counter_Vscale2(FL_OBJECT *ob, long dummy) {
  	int digit;
	double val;	
	char str[ 64 ];
  	int button=fl_get_iscaler_button(ob);
	if(button==0){
		val=atof(fl_get_iscaler_input(ob));
		if(strcmp(fl_get_object_label(ob),"Scale: (mV/div)")==0) val=val/1000;
	}
	else val=range(CHAN.V_scale[1],button);
  	if(val==0) val=CHAN.V_scale[1];
  	if(val<0.002) val=0.002;
  	if(val>10) val=10;
	CHAN.V_scale[1]=val;
  	if(val>1.9999) { digit=1; fl_set_object_label(ob,"Scale: (V/div)");}
  	else if(val>0.20001) { digit=2; fl_set_object_label(ob,"Scale: (V/div)");}
  	else if(val>0.00999) { digit=0; val=1000*val; fl_set_object_label(ob,"Scale: (mV/div)");}
  	else { digit=1; val=1000*val; fl_set_object_label(ob,"Scale: (mV/div)");}
	sprintf(str, "%.*f", digit, val);
	if(strlen(str)==1) sprintf(str, "    %.*f", digit, val);
	else if(strlen(str)==2) sprintf(str, "   %.*f", digit, val);
	else if(strlen(str)==3) sprintf(str, "  %.*f", digit, val);
	else if(strlen(str)==4) sprintf(str, " %.*f", digit, val);
	else if(strlen(str)==5) sprintf(str, " %.*f", digit, val);
	fl_set_iscaler_input(ob,str);
}	

void CB_check_chan2(FL_OBJECT *ob, long dummy) {
  CHAN.on[1]=fl_get_button(ob);
}

void CB_button_color_chan2(FL_OBJECT *ob, long dummy) {
  CHAN.color[1]=fl_show_colormap(CHAN.color[1]);
  fl_set_object_color(ob,CHAN.color[1],FL_COL1);
}

void CB_button_inv_chan2(FL_OBJECT *ob, long dummy) {
  CHAN.inv[1]=fl_get_button(ob);
}

void CB_counter_Tscale(FL_OBJECT *ob, long dummy)
{
  	int digit;
	double val;	
	char str[ 64 ];
  	int button=fl_get_iscaler_button(ob);
	if(button==0){
		val=atof(fl_get_iscaler_input(ob));
	}
	else val=range(HOR.T_scale,button);
  	if(val==0) val=HOR.T_scale;
  	if(val<2.0) val=2.0;
  	if(val>5000) val=5000;
	HOR.T_scale=val;
  	if(val>19.999) digit=0;
  	else if(val>1.999) digit=1; 
  	else digit=2;
	sprintf(str, "%.*f", digit, val);
	if(strlen(str)==1) sprintf(str, "    %.*f", digit, val);
	else if(strlen(str)==2) sprintf(str, "   %.*f", digit, val);
	else if(strlen(str)==3) sprintf(str, "  %.*f", digit, val);
	else if(strlen(str)==4) sprintf(str, " %.*f", digit, val);
	else if(strlen(str)==5) sprintf(str, " %.*f", digit, val);
	fl_set_iscaler_input(ob,str);
    if(BASE.ifile) plot_file(lxa->xyplot_display,4);
}

void CB_counter_Tpos(FL_OBJECT *ob, long dummy) {
  HOR.T_pos=fl_get_counter_value(ob);
}


void CB_trig_level(FL_OBJECT *ob, long dummy) {
  double val=fl_get_ipushwheel_value(ob);
  HOR.trig_level=val;
}


void CB_droplist_dev(FL_OBJECT *ob, long dummy) {
  char test[20];
  char undef[]="undefined";
  char tfile[]="file";
  char nfile[]="(null)";
  const char *fname;
  int success=0;
  BASE.sampling_rate=DEFAULT_SAMPLING_RATE;
  strcpy(test,fl_get_choice_text(ob));
  if(strcmp(test,tfile)==0) {		// user wants to display file content
     fl_set_fselector_placement(FL_PLACE_FREE); 
     fl_disable_fselector_cache(1);
	 fname=fl_show_fselector("Select Rawfile",  ".", "*.raw",0);
	 if(fname==NULL) success=0;
     else if (strlen(fname)==0) success=0;
	 else if (strcmp(nfile,fname)==0) success=0;
     else {
	 	BASE.ihandle=fopen(fname,"r");
		if (BASE.ihandle==0) {
		printf("Cannot open %s for display\n",BASE.ifname);
		BASE.ifile=0;
		success=0;
		}
		else success=1;
	 }
	 if(success) {
		BASE.ifname=fname;
		struct stat filestatus;
		stat(BASE.ifname , &filestatus );
		BASE.ifsize=(int)filestatus.st_size/4;
		printf("file name= >%s<\n",BASE.ifname);
		int fsize=(int)filestatus.st_size;
		if(fsize/(4*BASE.sampling_rate)<10000) 
		printf("file size is %d bytes, (%d samples for each channel, %.2f seconds)\n",
			fsize,BASE.ifsize,fsize/(4*BASE.sampling_rate));	
		else
		printf("file size is %d bytes, (%d samples for each channel, %.3e seconds)\n",
			fsize,BASE.ifsize,fsize/(4*BASE.sampling_rate));					
	    strcpy(BASE.device_name,tfile);
	    BASE.ifile=1;
		fl_show_object(lxa->f_counter_Pos);
 		fl_hide_object(lxa->button_run);
		BASE.run=0;
  		fl_set_button(lxa->button_run,0);
	  	fl_set_object_lcol(lxa->button_run,FL_GREEN);
  		fl_set_object_label(lxa->button_run,"START");
 		fl_hide_object(lxa->button_run);		
		fl_set_object_color(lxa->record,FL_INACTIVE,FL_COL1);
		fl_deactivate_object(lxa->record);
		fl_deactivate_object(lxa->button_gndcal);
		fl_deactivate_object(lxa->report_timing);
		if(BASE.record) close(BASE.rhandle);
   		BASE.record=0;
		fl_hide_object(lxa->trigger_Group);
		fl_show_object(lxa->filehandling_Group);		
		HOR.trig_chan=0;
		fl_set_object_label(lxa->f_trig_channel, "1");
// enable mouse handling on xyplot
		fl_set_timer(lxa->timer_refresh,DEFAULT_REFRESH_TIME);
	 	fl_set_object_posthandler(lxa->xyplot_display,xyplot_display_handler);
		fl_delete_xyplot_overlay(lxa->xyplot_display,4);
		fl_delete_xyplot_overlay(lxa->xyplot_display,5);
   		plot_file(lxa->xyplot_display,5);
		}

	else {
		fl_set_choice_text(ob,undef);
		printf("please select a file, or a sound card\n");
	}

   }
   else if(strcmp(test,undef)!=0) {   // one of the dsp entries was selected
       strcpy(BASE.device_name,test);
	   if (strcmp(undef,fl_get_choice_item_text(ob,1))==0) {
           if (open_dev()==0) {
                printf("can not initialize device < %s >, please select a different one\n",test); 
                fl_set_object_color(lxa->droplist_dev,FL_DARKORANGE,FL_INACTIVE); 			
           }
           else {
           		fl_delete_choice(ob,1);
		   		success=1;
 
          		fl_set_object_color(lxa->droplist_dev,FL_MCOL,FL_INACTIVE);
		   		BASE.ifile=0;
 		   		fl_show_object(lxa->button_run);
		   		fl_activate_object(lxa->button_run);
 		   		fl_hide_object(lxa->f_counter_Pos);	
           }
       }
       else {
          if (open_dev()==0) {
                printf("can not initialize device < %s >, please select a different one\n",test);
				fl_set_object_color(lxa->droplist_dev,FL_DARKORANGE,FL_INACTIVE); 	
          }
          else success=1;
		}
	    if(success) {
	        fl_set_object_color(lxa->droplist_dev,FL_MCOL,FL_INACTIVE);
		  	BASE.ifile=0; 
		  	fl_show_object(lxa->button_run);
		  	fl_activate_object(lxa->button_run);
			fl_show_object(lxa->trigger_Group);
			fl_hide_object(lxa->filehandling_Group);
			fl_activate_object(lxa->button_gndcal);
			fl_activate_object(lxa->report_timing);		
		}
   }
   else printf("please make a valid choice\n");
}

void CB_record(FL_OBJECT *ob, long dummy) {
	int rec=fl_get_button(ob);
	if(rec==1) {
   		fl_set_fselector_placement(FL_PLACE_FREE); 
		fl_disable_fselector_cache(1);
   		BASE.rfname=fl_show_fselector("Save raw data as:",  0, "*.raw",0);
   		printf("file name=%s\n",BASE.rfname);
   		int fr;
   		if((fr=open(BASE.rfname,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU))==-1)
      		{
         	printf("Cannot open %s\n",BASE.rfname);
         	return;
      	}
    	fl_set_object_color(lxa->record,FL_GREEN,FL_GREEN);
   		BASE.record=1;
		BASE.rhandle=fr;
     }
     else {
		close(BASE.rhandle);
		fl_set_object_color(lxa->record,FL_INACTIVE,FL_COL1);
   		BASE.record=0;
	 }
}

void CB_droplist_opmode(FL_OBJECT *ob, long dummy) {
  	BASE.op_mode=fl_get_choice(ob)-1;
	fl_set_object_label(lxa->text_mouse_V[0],"*  ");
	fl_set_object_label(lxa->text_mouse_V[1],"*  ");
	fl_set_object_label(lxa->text_mouse_T,"*  ");
	switch (BASE.op_mode) {
   		case OPERATION_MODE_NORMAL: {
		fl_activate_object(lxa->counter_Vpos[1]);
		fl_set_object_color(lxa->counter_Vpos[1],FL_MCOL,FL_INACTIVE);
		fl_set_object_lcol(lxa->counter_Vpos[1],FL_BLACK);
		fl_activate_object(lxa->counter_Vscale[1]);
		fl_set_object_color(lxa->counter_Vscale[1],FL_MCOL,FL_INACTIVE);		
		fl_set_object_lcol(lxa->counter_Vscale[1],FL_BLACK);
		fl_activate_object(lxa->check_chan[0]);
		fl_activate_object(lxa->check_chan[1]);
		fl_activate_object(lxa->trig_channel);
		fl_set_object_color(lxa->trig_channel,FL_MCOL,FL_INACTIVE);		
		fl_set_object_lcol(lxa->trig_channel,FL_BLACK);
		break;
		}
		case OPERATION_MODE_ADD: {
		fl_deactivate_object(lxa->counter_Vpos[1]);
		fl_set_object_color(lxa->counter_Vpos[1],FL_LIGHTER_COL1,FL_LIGHTER_COL1);
		fl_set_object_lcol(lxa->counter_Vpos[1],FL_DARKER_COL1);
		fl_deactivate_object(lxa->counter_Vscale[1]);
		fl_set_object_color(lxa->counter_Vscale[1],FL_LIGHTER_COL1,FL_LIGHTER_COL1);		
		fl_set_object_lcol(lxa->counter_Vscale[1],FL_DARKER_COL1);
    	fl_set_choice(lxa->trig_channel,0);
		fl_deactivate_object(lxa->trig_channel);
		fl_set_object_color(lxa->trig_channel,FL_LIGHTER_COL1,FL_LIGHTER_COL1);		
		fl_set_object_lcol(lxa->trig_channel,FL_DARKER_COL1);		
		HOR.trig_chan=0;
		fl_set_button(lxa->trig_channel,0);
		fl_set_object_label(lxa->trig_channel, "1");
		CHAN.V_pos[0]=0;
		fl_set_counter_value(lxa->counter_Vpos[0],0);
		break;
		}
		case OPERATION_MODE_XY: {
		fl_activate_object(lxa->counter_Vpos[1]);
		fl_set_object_color(lxa->counter_Vpos[1],FL_MCOL,FL_INACTIVE);
		fl_set_object_lcol(lxa->counter_Vpos[1],FL_BLACK);
		fl_activate_object(lxa->counter_Vscale[1]);
		fl_set_object_color(lxa->counter_Vscale[1],FL_MCOL,FL_INACTIVE);		
		fl_set_object_lcol(lxa->counter_Vscale[1],FL_BLACK);
		fl_activate_object(lxa->trig_channel);
		fl_set_object_color(lxa->trig_channel,FL_MCOL,FL_INACTIVE);		
		fl_set_object_lcol(lxa->trig_channel,FL_BLACK);
		break;
		}
	}
// xforms sometimes forgets to show the correct grid ...
    fl_set_xyplot_xtics(lxa->xyplot_display, 5, 2);
    fl_set_xyplot_xgrid(lxa->xyplot_display, FL_GRID_MINOR);
}

void CB_check_trigger(FL_OBJECT *ob, long dummy) { 
  HOR.trig_enb=(TRIG_ENB)(fl_get_button(ob));
}

void CB_trig_mode(FL_OBJECT *ob, long dummy) { 
     if((TRIG_TYPE)HOR.trig_mode==1) {
     HOR.trig_mode=(TRIG_TYPE)0;
     fl_set_object_label(ob,"UP");
     }
     else {
     HOR.trig_mode=(TRIG_TYPE)1;
     fl_set_object_label(ob, "DN");
     }
}

void CB_trig_channel(FL_OBJECT *ob, long dummy)  { 
	int val=fl_get_button(ob);
	if(val) {
		HOR.trig_chan=1;
		fl_set_object_label(ob,"2");
	}
	else {
		HOR.trig_chan=0;
		fl_set_object_label(ob, "1");
	}
}

void CB_reference(FL_OBJECT *ob, long dummy)  {
  double val=fl_get_ipushwheel_value(ob);
  BASE.reference=val;
}

void CB_report_timing(FL_OBJECT *ob, long dummy)  { 
  BASE.report_timing=fl_get_button(ob);
}

void CB_button_about_OK(FL_OBJECT *ob, long dummy) {
  fl_hide_form(about->about);
}


// position counter
void CB_f_position(FL_OBJECT *ob, long dummy) {
	float val;
	int button = fl_get_iscaler_button(ob);
	if(button==0){
		val=atof(fl_get_iscaler_input(ob));
		BASE.input_pos=val;
 		plot_file(lxa->xyplot_display,4);    
	}
	else 
 	  	plot_file(lxa->xyplot_display,button);     
}

int CB_at_close(FL_FORM *lxa,void *dummy)  { 
if(BASE.record) close(BASE.rhandle);
if(BASE.ifile==1)  fclose(BASE.ihandle);
return FL_OK;
}

void CB_button_gndcal(FL_OBJECT *ob, long dummy)  {
	BASE.gndcal = 2;
fl_set_object_color(lxa->button_gndcal,FL_MCOL,FL_RED);
}

void CB_button_pulse(FL_OBJECT *ob, long dummy)  {
printf("this button is for output only\n");
}

void CB_counter_hyst(FL_OBJECT *ob, long dummy)  {  
  HOR.hyst = fl_get_counter_value(ob);
}

void CB_f_rate(FL_OBJECT *ob, long dummy)  {
BASE.sampling_rate = fl_get_ipushwheel_value(ob);   	
plot_file(lxa->xyplot_display,4);
}

// input field
void CB_input_Pos(FL_OBJECT *ob, long dummy)  {
BASE.input_pos=atof(fl_get_iscaler_input(ob));
plot_file(lxa->xyplot_display,4);    
}

void set_f_counter_Pos(float val) {
	char str[30];
	char pstr[30];
	int nn;
	sprintf(str, "%.*f", 2, val);
	BASE.input_pos=atof(str);
	int len=strlen(str);
	if(len>6){
		for(nn=len+1;nn>=len-6;nn--) str[nn+1]=str[nn];
			str[len-6]=',';
	}
	if(len>9){
		for(nn=len+1;nn>=len-9;nn--) str[nn+1]=str[nn];
			str[len-9]=',';
	}
	if(strlen(str)<5)       strcpy(pstr,"           ");
	else if(strlen(str)<7)  strcpy(pstr,"          ");
	else if(strlen(str)<9)  strcpy(pstr,"         ");
	else if(strlen(str)<11) strcpy(pstr,"        ");
	else if(strlen(str)<13) strcpy(pstr,"       ");
	else                    strcpy(pstr,"      ");
	strcat(pstr,str);
	fl_set_iscaler_input(lxa->f_counter_Pos,pstr);
}

