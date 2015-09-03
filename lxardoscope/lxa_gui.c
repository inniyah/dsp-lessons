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
#include "ipushwheel.h"
#include "lxforms.h"
#include "lxa.h"

static FL_PUP_ENTRY fdchoice_droplist_dev_0[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "select",	0,	"",	 FL_PUP_NONE},
    { "select",	0,	"",	 FL_PUP_NONE},
    {0}
};


static FL_PUP_ENTRY fdchoice_droplist_opmode_3[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "Time",	0,	"",	 FL_PUP_NONE},
    { "Add",	0,	"",	 FL_PUP_NONE},
    { "X-Y",	0,	"",	 FL_PUP_NONE},
    {0}
};

FD_lxa *create_form_lxa(void)
{
  FL_OBJECT *obj;
  FD_lxa *fdui = (FD_lxa *) fl_calloc(1, sizeof(*fdui));

  fdui->lxa = fl_bgn_form(FL_NO_BOX, 790, 490);
  obj = fl_add_box(FL_UP_BOX,0,0,790,490,"");
    fl_set_object_color(obj,FL_DARKER_COL1,FL_DARKER_COL1);
  fdui->xyplot_display = obj = fl_add_xyplot(FL_NORMAL_XYPLOT,10,10,435,435,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_xyplot_xtics(obj, 5, 2);
    fl_set_xyplot_xgrid(obj, FL_GRID_MINOR);
    fl_set_xyplot_ygrid(obj, FL_GRID_MINOR);
    fl_set_xyplot_ybounds(obj,-5.5,5.0);	 		
	fl_set_xyplot_fixed_xaxis(obj,"00","00");
	fl_set_xyplot_alphaytics(obj,
		"@-5.0|@-4.0|@-3.0|-@-2.0|@-1.0|0@0.0|@1.0|+@2.0|@3.0|@4.0|@5.0","");      

//
// Channel 1 box  
  obj = fl_add_box(FL_BORDER_BOX,450,10,90,20,"");  // outer label frame
  obj = fl_add_box(FL_BORDER_BOX,451,11,88,18,"");  // inner label frame
  obj = fl_add_box(FL_BORDER_BOX,450,19,330,96,""); // outer work frame
  obj = fl_add_box(FL_BORDER_BOX,451,20,329,95,""); // inner work frame
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,452,12,86,18,"Channel 1"); // this covers lines from work frame
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->check_chan[0] = obj = fl_add_checkbutton(FL_PUSH_BUTTON,460,40,52,20,"ON");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,CB_check_chan1,0);
    fl_set_button(obj, 1);
    fl_set_object_color(obj,FL_MCOL,FL_YELLOW);
  fdui->button_color_chan[0] = obj = fl_add_button(FL_NORMAL_BUTTON,460,65,52,20,"Color");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_color(obj,FL_RED,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,CB_button_color_chan1,0);
  fdui->button_inv_chan[0] = obj = fl_add_checkbutton(FL_PUSH_BUTTON,460,90,52,20,"INV");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);    
    fl_set_object_callback(obj,CB_button_inv_chan1,0);
    fl_set_button(obj, 0);
    fl_set_object_color(obj,FL_MCOL,FL_YELLOW);    
  fdui->counter_Vpos[0] = obj = fl_add_counter(FL_NORMAL_COUNTER,525,40,130,20,"Position: (div)");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,CB_counter_Vpos1,0);
    fl_set_counter_precision(obj, 1);
    fl_set_counter_bounds(obj, -100.00, 100.00);
    fl_set_counter_step(obj, 0.1, 1.0);    
    fl_set_counter_value(obj, 2.0);    
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->counter_Vscale[0] = obj = fl_add_iscaler(FL_FLOAT_INPUT,525,90,130,20,"Scale: (V/div)");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,CB_counter_Vscale1,0);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);	
	fl_set_iscaler_maxchars(obj,5);
//fdui->counter_Vscale[0] = obj = fl_add_scrollbar(FL_FLOAT_INPUT,525,90,130,20,"Scale: (V/div)");
//fl_set_object_callback(obj,CB_counter_Vscale1,0);
// Results for channel 1
  obj = fl_add_box(FL_BORDER_BOX,670,20,110,95,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_text(FL_NORMAL_TEXT,677,22,42,18,"Max:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,40,42,18,"Min:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,58,42,18,"PP:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,74,42,21,"Avg:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,94,42,18,"RMS:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->text_max_chan[0] = obj = fl_add_text(FL_NORMAL_TEXT,719,22,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fdui->text_min_chan[0] = obj = fl_add_text(FL_NORMAL_TEXT,719,40,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->text_pp_chan[0] = obj = fl_add_text(FL_NORMAL_TEXT,719,58,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->text_avg_chan[0] = obj = fl_add_text(FL_NORMAL_TEXT,719,76,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->text_rms_chan[0] = obj = fl_add_text(FL_NORMAL_TEXT,719,94,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);

//
// Channel 2 box  
  obj = fl_add_box(FL_BORDER_BOX,450,120,90,20,"");
  obj = fl_add_box(FL_BORDER_BOX,451,121,88,18,"");
  obj = fl_add_box(FL_BORDER_BOX,450,129,330,96,"");
  obj = fl_add_box(FL_BORDER_BOX,451,130,329,95,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,452,122,86,18,"Channel 2");
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->check_chan[1] = obj = fl_add_checkbutton(FL_PUSH_BUTTON,460,150,52,20,"ON");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,CB_check_chan2,0);
    fl_set_button(obj, 1);
    fl_set_object_color(obj,FL_MCOL,FL_YELLOW);
   fdui->button_color_chan[1] = obj = fl_add_button(FL_NORMAL_BUTTON,460,175,52,20,"Color");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_color(obj,FL_GREEN,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,CB_button_color_chan2,0);
  fdui->button_inv_chan[1] = obj = fl_add_checkbutton(FL_PUSH_BUTTON,460,200,52,20,"INV");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,CB_button_inv_chan2,0);
    fl_set_button(obj, 0);
    fl_set_object_color(obj,FL_MCOL,FL_YELLOW);
 fdui->counter_Vpos[1] = obj = fl_add_counter(FL_NORMAL_COUNTER,525,150,130,20,"Position: (div)");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,CB_counter_Vpos2,0);
    fl_set_counter_precision(obj, 1);
    fl_set_counter_bounds(obj, -100.00, 100.00);
    fl_set_counter_step(obj, 0.1, 1.0);
    fl_set_counter_value(obj, -2.0);    
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->counter_Vscale[1] = obj = fl_add_iscaler(FL_FLOAT_INPUT,525,200,130,20,"Scale: (V/div)");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,CB_counter_Vscale2,0);    
	fl_set_iscaler_maxchars(obj,5);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);

// Channel 2 results
  obj = fl_add_box(FL_BORDER_BOX,670,130,110,95,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_text(FL_NORMAL_TEXT,677,133,42,18,"Max:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,151,42,18,"Min:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,169,42,18,"PP:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,186,42,21,"Avg:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,677,205,42,18,"RMS:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->text_max_chan[1] = obj = fl_add_text(FL_NORMAL_TEXT,719,133,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);    
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->text_min_chan[1] = obj = fl_add_text(FL_NORMAL_TEXT,719,151,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->text_pp_chan[1] = obj = fl_add_text(FL_NORMAL_TEXT,719,169,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->text_avg_chan[1] = obj = fl_add_text(FL_NORMAL_TEXT,719,187,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->text_rms_chan[1] = obj = fl_add_text(FL_NORMAL_TEXT,719,205,52,18,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);

  fdui->trigger_Group = fl_bgn_group();
// Time base  
  obj = fl_add_box(FL_BORDER_BOX,450,230,90,20,""); 
  obj = fl_add_box(FL_BORDER_BOX,451,231,88,18,""); 
  obj = fl_add_box(FL_BORDER_BOX,450,239,238,161,""); 
  obj = fl_add_box(FL_BORDER_BOX,451,240,237,160,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,452,232,86,18,"Timebase");
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->counter_Tscale = obj = fl_add_iscaler(0,557,250,125,20,"Full Scale (ms)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,CB_counter_Tscale,0);
//    fl_set_scaler_precision(obj, 1);
//    fl_set_scaler_bounds(obj, -6, 5000.00);
//    fl_set_scaler_value(obj, 10.0);	
	fl_set_iscaler_maxchars(obj,5);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE); 

// Trigger box
  obj = fl_add_box(FL_BORDER_BOX,451,278,237,122,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_DARKER_COL1);
  obj = fl_add_box(FL_BORDER_BOX,451,279,237,121,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_DARKER_COL1);
   obj = fl_add_box(FL_FLAT_BOX,460,271,60,20,"Trigger");
     fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
     fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
     fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->check_trigger = obj = fl_add_checkbutton(FL_PUSH_BUTTON,523,286,50,18,"ON");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,CB_check_trigger,0);
    fl_set_button(obj, 0);    
    fl_set_object_color(obj,FL_MCOL,FL_YELLOW);
  fdui->trig_level = obj = fl_add_ipushwheel(3,625,286,60,48,"Level\n(V)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,CB_trig_level,0);
    fl_set_ipushwheel_bounds(obj, -10.00, 10.00);
    fl_set_ipushwheel_step(obj, 0.01);      
	fl_set_ipushwheel_precision(obj,2);
    fl_set_ipushwheel_color(obj,FL_MCOL,FL_INACTIVE);
	fl_set_ipushwheel_maxchars(obj,7);
//	fl_set_ipushwheel_nbchars(obj,9);
    fl_set_ipushwheel_value(obj, 0.0);  
  fdui->trig_channel = obj = fl_add_button(FL_PUSH_BUTTON,523,304,50,18,"1");
    fl_set_object_bw(obj,1);
    fl_set_object_lalign(obj,FL_ALIGN_INSIDE);
    fl_set_object_callback(obj,CB_trig_channel,0);
    fl_set_object_color(obj,FL_MCOL,FL_MCOL);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	obj = fl_add_box(FL_NO_BOX,458,304,65,18,"Channel");
  	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->trig_mode = obj = fl_add_button(FL_PUSH_BUTTON,523,322,50,18,"UP");
    fl_set_object_bw(obj,1);
    fl_set_object_lalign(obj,FL_ALIGN_INSIDE);    
    fl_set_object_callback(obj,CB_trig_mode,0);
    fl_set_choice(obj,0);    
    fl_set_object_color(obj,FL_MCOL,FL_MCOL);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	obj = fl_add_box(FL_NO_BOX,458,322,65,20,"Edge");
  	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->counter_Hyst = obj = fl_add_counter(FL_NORMAL_COUNTER,567,346,115,20,"Hysteresis (V)");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,CB_counter_hyst,0);
    fl_set_counter_precision(obj, 2);
    fl_set_counter_bounds(obj, 0.0, 5.0);
    fl_set_counter_step(obj, 0.01,0.1);
	fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);    
	fl_set_counter_value(obj, 0.1); 
   fdui->counter_Tpos = obj = fl_add_counter(FL_NORMAL_COUNTER,567,372,115,20,"DisplayShift (ms)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,CB_counter_Tpos,0);
    fl_set_counter_precision(obj, 0);
	fl_set_counter_step(obj,1,10);
    fl_set_counter_bounds(obj, -500.0, 500.0);    
	fl_set_object_color(obj,FL_MCOL,FL_INACTIVE); 
// trigger flag
   obj = fl_add_box(FL_BORDER_BOX,450,399,90,22,"");  
   obj = fl_add_box(FL_BORDER_BOX,451,399,89,21,"");  
   obj = fl_add_box(FL_FLAT_BOX,452,400,87,19,"Triggered");   
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
fdui->indicator_triggered = obj = fl_add_roundbutton(FL_NORMAL_BUTTON,450,398,20,20,"");
    fl_set_object_color(obj,FL_INACTIVE,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_deactivate_object(obj);
//
// the following two out of sequence, but are part of this group
// record
   obj = fl_add_box(FL_BORDER_BOX,695,352,85,24,"");  
   obj = fl_add_box(FL_BORDER_BOX,696,353,84,23,"");  
   obj = fl_add_box(FL_FLAT_BOX,697,354,82,19,"Record");   
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
fdui->record = obj = fl_add_roundbutton(FL_PUSH_BUTTON,695,354,20,20,"");
    fl_set_object_color(obj,FL_INACTIVE,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_callback(obj,CB_record,0);
//
fdui->button_pulse = obj = fl_add_roundbutton(FL_PUSH_BUTTON,520,420,20,20,"");
    fl_set_object_color(obj,FL_INACTIVE,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,CB_button_pulse,0);
	fl_set_button(obj, 1);
fl_end_group();
//
  fdui->filehandling_Group = fl_bgn_group();
  obj = fl_add_box(FL_BORDER_BOX,450,230,90,20,""); 
  obj = fl_add_box(FL_BORDER_BOX,451,231,88,18,""); 
  obj = fl_add_box(FL_BORDER_BOX,450,239,238,161,""); 
  obj = fl_add_box(FL_BORDER_BOX,451,240,237,160,"");
  fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,452,232,86,18,"Timebase");
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

  fdui->f_counter_Tscale = obj = fl_add_iscaler(0,530,262,130,20,"Full Scale (ms)");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,CB_counter_Tscale,0);
//    fl_set_iscaler_precision(obj, 1);
//    fl_set_iscaler_bounds(obj, -6, 5000.00);
//    fl_set_iscaler_value(obj, DEFAULT_TIME_SCALE);
	fl_set_iscaler_maxchars(obj,5);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE); 
//
//  fdui->counter_Vscale[1] = obj = fl_add_iscaler(FL_FLOAT_INPUT,525,200,130,20,"Scale: (V/div)");
//    fl_set_object_lalign(obj,FL_ALIGN_TOP);
//    fl_set_object_callback(obj,CB_counter_Vscale2,0);    
//	fl_set_iscaler_maxchars(obj,5);
//    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
//  fdui->counter_Pos = obj = fl_add_scaler(0,460,295,200,20,"");
//    fl_set_object_lalign(obj,FL_ALIGN_TOP);
//    fl_set_object_callback(obj,CB_position,0);
//    fl_set_scaler_precision(obj, 10);
//    fl_set_scaler_bounds(obj, 0, 1000000.00);
//    fl_set_scaler_value(obj,0);
//    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE); 
fdui->f_counter_Pos = obj = fl_add_iscaler(FL_FLOAT_INPUT,460,295,200,20,"");
fl_set_object_lalign(obj,FL_ALIGN_TOP);
fl_set_object_callback(obj,CB_f_position,0);
fl_set_object_color(obj,FL_MCOL,FL_INACTIVE); 

  obj = fl_add_box(FL_NO_BOX,460,317,110,18,"File Position (ms)");
  	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
//  fdui->input_Pos = obj = fl_add_input(FL_FLOAT_INPUT,500,295,120,20,"");
//	fl_set_object_callback(obj,CB_input_Pos,0);
//    fl_set_input(obj,"        0.00        ");
//	fl_set_input_cursorpos(obj,20,1);
//	fl_set_input_color(obj,FL_BLACK,FL_BLACK);
  fdui->f_trig_channel = obj = fl_add_button(FL_PUSH_BUTTON,460,343,50,18,"1");
    fl_set_object_bw(obj,1);
    fl_set_object_lalign(obj,FL_ALIGN_INSIDE);
    fl_set_object_callback(obj,CB_trig_channel,0);
    fl_set_object_color(obj,FL_MCOL,FL_MCOL);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_box(FL_NO_BOX,457,361,110,36,"Vertical Channel\nin XY mode");
  	fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->f_Rate = obj = fl_add_ipushwheel(4,582,328,80,48,"");
    fl_set_object_lalign(obj,FL_ALIGN_BOTTOM);
    fl_set_object_callback(obj,CB_f_rate,0);
    fl_set_ipushwheel_bounds(obj, 1000, 5000);
    fl_set_ipushwheel_step(obj, 1.0);    
	fl_set_ipushwheel_precision(obj,0);
    fl_set_ipushwheel_color(obj,FL_MCOL,FL_INACTIVE);
	fl_set_ipushwheel_maxchars(obj,9);
//	fl_set_ipushwheel_nbchars(obj,9);
    fl_set_ipushwheel_value(obj, 3000);    
  obj = fl_add_box(FL_NO_BOX,557,377,110,20,"Sampling Rate");
  	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
fl_end_group();
//
// Display mode box
  obj = fl_add_box(FL_BORDER_BOX,450,430,68,30,"");  
  obj = fl_add_box(FL_BORDER_BOX,451,431,66,29,"");  
  obj = fl_add_box(FL_BORDER_BOX,450,444,90,36,""); 
  obj = fl_add_box(FL_BORDER_BOX,451,445,89,35,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,452,432,64,19,"Mode");    
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

 fdui->droplist_opmode = obj = fl_add_choice(FL_DROPLIST_CHOICE,460,455,70,20,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_callback(obj,CB_droplist_opmode,0);
    fl_set_choice_entries(obj, fdchoice_droplist_opmode_3);
    fl_set_choice(obj,1);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);

//
// Signal Values box
  obj = fl_add_box(FL_BORDER_BOX,695,239,67,30,"");  
  obj = fl_add_box(FL_BORDER_BOX,696,240,65,28,"");  
  obj = fl_add_box(FL_BORDER_BOX,695,268,85,76,""); 
  obj = fl_add_box(FL_BORDER_BOX,696,269,84,75,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,697,241,63,34,"Signal\nValues");   
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,695,279,40,20,"T");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,695,299,40,20,"V1");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,695,319,40,20,"V2");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->text_mouse_T = obj = fl_add_text(FL_NORMAL_TEXT,725,279,50,20,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fdui->text_mouse_V[0] = obj = fl_add_text(FL_NORMAL_TEXT,725,299,50,20,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->text_mouse_V[1] = obj = fl_add_text(FL_NORMAL_TEXT,725,319,50,20,"*  ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);

//
// Arduino and serial port
  obj = fl_add_box(FL_BORDER_BOX,695,393,85,20,"");  // for label
  obj = fl_add_box(FL_BORDER_BOX,696,394,83,18,"");  // double line
  obj = fl_add_box(FL_BORDER_BOX,545,406,235,75,""); // for double line, outer
  obj = fl_add_box(FL_BORDER_BOX,546,407,233,74,"");
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
  obj = fl_add_box(FL_FLAT_BOX,697,395,81,18,"Arduino"); // label covers lines    
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_color(obj,FL_LIGHTER_COL1,FL_LIGHTER_COL1);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

  fdui->droplist_dev = obj = fl_add_choice(FL_DROPLIST_CHOICE,665,425,110,20,"Device");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,CB_droplist_dev,0);
    fl_set_choice_entries(obj, fdchoice_droplist_dev_0);
    fl_set_choice(obj,1);
    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);  
	obj = fl_add_text(FL_NORMAL_TEXT,547,405,75,22,"Reference");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_SMALL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->reference = obj = fl_add_ipushwheel(3,552,426,60,48,"");
    fl_set_object_callback(obj,CB_reference,0);
    fl_set_ipushwheel_bounds(obj, 0.1, 10.00);
    fl_set_ipushwheel_step(obj, 0.01);    
	fl_set_ipushwheel_precision(obj,2);
    fl_set_ipushwheel_color(obj,FL_MCOL,FL_INACTIVE);
	fl_set_ipushwheel_maxchars(obj,7);
//	fl_set_ipushwheel_nbchars(obj,7);
    fl_set_ipushwheel_value(obj, DEFAULT_REFERENCE);
//    fl_set_object_color(obj,FL_MCOL,FL_INACTIVE);
  fdui->report_timing = obj = fl_add_checkbutton(FL_PUSH_BUTTON,755,456,20,18,"Timing");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);     
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,CB_report_timing,0);
    fl_set_button(obj, 0);    
    fl_set_object_color(obj,FL_MCOL,FL_YELLOW);
	fl_deactivate_object(obj);
  fdui->button_run = obj = fl_add_button(FL_PUSH_BUTTON,330,450,100,30,"START");
    fl_set_object_lcolor(obj,FL_CHARTREUSE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_button(obj, 0);
    fl_set_object_callback(obj,CB_button_run,0);
	fl_deactivate_object(obj);
  fdui->logotipo = obj = fl_add_button(FL_NORMAL_BUTTON,15,450,110,30,"lxardoscope");
    fl_set_object_lcolor(obj,FL_CHARTREUSE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,CB_logotipo,0);
  fdui->timer_refresh = obj = fl_add_timer(FL_HIDDEN_TIMER,190,350,80,30,"sampling");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_callback(obj,CB_timer_refresh,0);
  fdui->button_gndcal = obj = fl_add_checkbutton(FL_RADIO_BUTTON,675,456,20,18,"GNDcal");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_bw(obj,2);     
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_button(obj, 0);    
    fl_set_object_color(obj,FL_MCOL,FL_RED);
    fl_set_object_callback(obj,CB_button_gndcal,0);
	fl_deactivate_object(obj);
  fl_end_form();

  fdui->lxa->fdui = fdui;

  return fdui;
}

FD_about *create_form_about(void)
{
  FL_OBJECT *obj;
  FD_about *fdui = (FD_about *) fl_calloc(1, sizeof(*fdui));

  fdui->about = fl_bgn_form(FL_NO_BOX, 380, 250);
  obj = fl_add_box(FL_UP_BOX,0,0,380,250,"");
  fdui->button_about_OK = obj = fl_add_button(FL_NORMAL_BUTTON,150,215,80,25,"close");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,CB_button_about_OK,0);
  obj = fl_add_text(FL_NORMAL_TEXT,120,10,160,40,"lxardoscope 0.85");
    fl_set_object_lcolor(obj,FL_CHARTREUSE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_SHADOW_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,40,55,300,60,"Created by Nick\n (aka Oskar Leuthold)\n in 2011.");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,40,120,300,20,"Thanks to:");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,40,140,300,20,"J. Fernando Moyano for XOSKOPE,");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,40,160,300,20,"Dr. Zhao for XFORMS library, and");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,15,180,350,20,"Tim Witham for xoscope.");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  fl_end_form();

  fdui->about->fdui = fdui;

  return fdui;
}


