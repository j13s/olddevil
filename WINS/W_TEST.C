/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_test.c - This is just a test if wins works as it should
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program (file COPYING); if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "wins.h"
#include "w_init.h"
#include "w_event.h"

extern FILE *errf;

enum evcodes { ec_credits,ec_changenumlock,ec_clickcube,ec_clickthing,
 ec_togdrawthings,ec_togdrawcubes,ec_togdrawlines,ec_menucubes,ec_menuint,
 ec_menudoors,ec_menuthings,ec_nextthing,ec_prevthing,ec_nextcube,
 ec_prevcube,ec_nextwall,ec_prevwall,ec_tag,
 ec_togdrawdata,ec_pturnleft,ec_pturnright,ec_pturnup,ec_pturndown,
 ec_incvisibility,ec_decvisibility,ec_inczoom,ec_deczoom,ec_incmovefactor,
 ec_decmovefactor,ec_moveforward,ec_movebackward,ec_moveleft,
 ec_moveright,ec_moveup,ec_movedown,ec_bankleft,ec_turnup,ec_bankright,
 ec_turnright,ec_turndown,ec_turnleft,ec_incrotangle,
 ec_decrotangle,ec_nextdoor,ec_prevdoor,ec_menuwalls,
 ec_menucorners,ec_loadlevel,ec_savelevel,ec_clickdoor,
 ec_togdrawdoors,ec_tagall,ec_beam,ec_help,ec_makemacro,ec_savemacro,
 ec_choosemacro,ec_fastquit,ec_pmovedown,ec_pmoveup,ec_pmoveright,
 ec_pmoveleft,ec_pmoveback,ec_pmoveforw,ec_nextpnt,ec_prevpnt,ec_fitbitmap,
 ec_insert,ec_chval_without_se,ec_pbankleft,ec_pbankright,
 ec_scrollup,ec_scrolldown,ec_changedata,ec_incpmovefactor,
 ec_decpmovefactor,ec_delete,ec_gridonoff,ec_decrgridsize,
 ec_incrgridsize,ec_changevalue,ec_mmoveup,ec_mmovedown,
 ec_connectcubes,ec_choosecube,ec_choosething,ec_choosedoor,
 ec_chval_without_tag,ec_chval_without_all,ec_fastinsert,ec_setexit,
 ec_deleteall,ec_gowall,ec_quit,ec_connectbitmaps,ec_togdrawgrid,
 ec_incprotangle,ec_decprotangle,ec_aligntox,ec_aligntoy,ec_aligntoz,
 ec_aligntonegx,ec_aligntonegy,ec_aligntonegz,ec_tagwholepnt,
 ec_insscaled,ec_insfastscaled,ec_sidecube,ec_grow,ec_shrink,ec_calctxts,
 ec_togglebox,ec_toggleoldbox,ec_toggleboxmode,ec_tagwithmouse,
 ec_lighshading,ec_usecubetag,ec_usesidetag,ec_playlevel,ec_togdrawxtagged,
 ec_plotfilledwall,ec_hogman,ec_deletespecial,ec_statistics,ec_num_of_codes };
 
void (*do_event[ec_num_of_codes])(struct w_event *,int);

void dec_quit(struct w_event *w,int ec)
 { w->w_flags=w_f_end; }
 
void main(void)
 {
 struct w_window *w1,*w2,iw1={0,100,100,200,200,200,200,0,"BLA",
  "Dies ist ein Helptext der automatisch formatiert wird",
  wb_drag|wb_size|wb_close,wr_save,NULL,NULL,NULL};
  char *opt[5] = { "Switch","Only texture","Shoot through","Normal","Door" };
 struct w_b_choose b_choose={ 0,5,opt,NULL,NULL };
 struct w_b_string b_string={ 0,10,"1234567890",isdigit,NULL };
 struct w_event we;
 FILE *mf;
 do_event[ec_quit]=dec_quit;
 if(!w_initwins(640,480,256,"wins.fnt"))
  { printf("Can't initialize Wins.\n"); exit(5); }
 if((mf=fopen("wins.ini","r"))==NULL)
  { fprintf(errf,"No ini-file.\n"); exit(5); }
 if(!w_initmenu(mf,do_event,ec_num_of_codes))
  { fprintf(errf,"Can't init menu.\n"); exit(5); }
 if((w1=w_openwindow(&iw1))==NULL)
  { fprintf(errf,"Can't open window 1\n"); exit(5); }
 if(w_addstdbutton(w1,w_b_choose,10,10,-1,-1,"Test",&b_choose,1)==NULL)
  { fprintf(errf,"Can't open button 1\n"); exit(5); }
 iw1.xpos=150; iw1.ypos=150;
 if((w2=w_openwindow(&iw1))==NULL)
  { fprintf(errf,"Can't open window 2\n"); exit(5); }
 if(w_addstdbutton(w2,w_b_string,10,10,-1,-1,"Test2",&b_string,1)==NULL)
  { fprintf(errf,"Can't open button 2\n"); exit(5); }
 w_startwins();
 w_closewins();
 }

