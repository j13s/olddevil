/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_opts.c - procedures for handling the b_prev_no_next buttons in the
     option menu
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
    
#include "structs.h"
#include "userio.h"
#include "tools.h"
#include "insert.h"
#include "options.h"
#include "plot.h"
#include "tag.h"
#include "macros.h"
#include "readlvl.h"
#include "do_light.h"
#include "do_event.h"
#include "do_opts.h"

static void wi_changeobject(struct w_button *b,int dowhat)
 {
 enum infos what;
 struct node **n=NULL,*sn;
 struct list *ls=NULL;
 my_assert(b!=NULL);
 if(l==NULL) return;
 what=(enum infos)b->data;
 switch(what)
  {
  case in_cube: n=&view.pcurrcube; ls=&l->cubes; break;
  case in_thing: n=&view.pcurrthing;ls=&l->things; break;
  case in_door: n=&view.pcurrdoor; ls=&l->doors; break;
  case in_pnt: n=&view.pcurrpnt; ls=&l->pts; break;
  case in_wall: 
   if(view.pcurrcube!=NULL) 
    {
    switch(dowhat)
     {
     case -4: view.currwall=0; break;
     case -3: view.currwall--; break;
     case -2: view.currwall++; break;
     case -1: view.currwall=5; break;
     default: view.currwall=dowhat;
     }
    if(view.currwall>5) view.currwall=0;
    else if(view.currwall<0) view.currwall=5;
    sprintf(b->d.str->str,"%.4d",view.currwall);
    }
   else sprintf(b->d.str->str,"----");
   break;
  case in_edge: 
   if(view.pcurrcube!=NULL) 
    {
    switch(dowhat)
     {
     case -4: view.curredge=0; break;
     case -3: view.curredge--; break;
     case -2: view.curredge++; break;
     case -1: view.curredge=3; break;
     default: view.curredge=dowhat;
     }
    if(view.curredge>3) view.curredge=0;
    else if(view.curredge<0) view.curredge=3;
    sprintf(b->d.str->str,"%.4d",view.curredge);
    n=&view.pcurrpnt; ls=&l->pts;
    drawopt(in_edge); what=in_pnt;
    dowhat=view.pcurrcube->d.c->p[wallpts[view.currwall][view.curredge]]->no;
    }
   else sprintf(b->d.str->str,"----");
   break;
  default: waitmsg("Unknown infotype in changeobject %d",what); return;
  }
 if(n!=NULL && *n!=NULL && ls!=NULL)
  {
  switch(dowhat)
   {
   case -4: *n=ls->head; break;
   case -3: *n=(*n)->prev->prev!=NULL ? (*n)->prev : ls->tail; break;
   case -2: *n=(*n)->next->next!=NULL ? (*n)->next : ls->head; break;
   case -1: *n=ls->tail; break;
   default: if((sn=findnode(ls,dowhat))!=NULL) *n=sn;
   }
  if((*n)->no>9999) sprintf(b->d.str->str,"****");
  else sprintf(b->d.str->str,"%.4u",(*n)->no);
  }
 else if(n!=NULL && ls!=NULL) sprintf(b->d.str->str,"----");
 view.pcurrwall=view.pcurrcube==NULL ? NULL :
  view.pcurrcube->d.c->walls[view.currwall];
 switch(what)
  {
  case in_cube: drawopt(in_cube);
  case in_wall: drawopt(in_wall);
  case in_edge: drawopt(in_edge); break;
  case in_pnt: drawopt(in_pnt); break;
  case in_thing: drawopt(in_thing); break;
  case in_door: drawopt(in_door); break;
  default: waitmsg("Unknown infotype in changeobject %d",what); return;
  }
 plotcurrent();
 }
void b_firstobj(struct w_button *b)
 { wi_changeobject((struct w_button *)b->data,-4); }
void b_prevobj(struct w_button *b)
 { wi_changeobject((struct w_button *)b->data,-3); }
void b_nextobj(struct w_button *b)
 { wi_changeobject((struct w_button *)b->data,-2); }
void b_lastobj(struct w_button *b)
 { wi_changeobject((struct w_button *)b->data,-1); }
void b_numobj(struct w_button *b)
 { wi_changeobject(b,atoi(b->d.str->str)); }
 
void b_tagoneobject(struct w_button *b) { tagobject((enum infos)b->data); }
void b_tagallobjects(struct w_button *b) {tagallobjects((enum infos)b->data);}

void b_l_insert(struct w_button *b) 
 { 
 int oldmode;
 oldmode=view.currmode; view.currmode=(enum infos)b->data;
 do_event[ec_insert](ec_insert);
 view.currmode=oldmode;
 }
void b_r_insert(struct w_button *b) 
 { 
 int oldmode;
 oldmode=view.currmode; view.currmode=(enum infos)b->data;
 do_event[ec_insertfast](ec_insertfast);
 view.currmode=oldmode;
 }
void b_l_delete(struct w_button *b) 
 { 
 int oldmode;
 oldmode=view.currmode; view.currmode=(enum infos)b->data;
 do_event[ec_delete](ec_delete);
 view.currmode=oldmode;
 }
void b_r_delete(struct w_button *b) 
 { 
 int oldmode;
 oldmode=view.currmode; view.currmode=(enum infos)b->data;
 do_event[ec_deletefast](ec_deletefast);
 view.currmode=oldmode;
 }
 
void b_l_defaultobject(struct w_button *b)
 {
 if(view.pcurrcube && view.pcurrcube->d.c->d[view.currwall])
  { printmsg(TXT_DOORNOTDEFWALL); drawopt(in_wall); return; }
 if(view.pcurrcube && !view.pcurrcube->d.c->walls[view.currwall])
  { printmsg(TXT_EMPTYNOTDEFWALL); drawopt(in_wall); return; }
 view.pdefcube=view.pcurrcube; view.defwall=view.currwall; 
 view.pdeflevel=l->n;
 drawopt(in_wall);
 }
 
void b_r_defaultobject(struct w_button *b)
 { drawopt((enum infos)b->data); } /* to let the button pop out */

void b_l_lockwall(struct w_button *b) 
 {
 if(!view.pcurrcube || !view.pcurrcube->d.c->walls[view.currwall])
  { printmsg(TXT_EMPTYWALLNOTLOCK); drawopt(in_wall); return; }
 view.pcurrcube->d.c->walls[view.currwall]->locked^=1;
 plotlevel();
 }

void b_r_lockwall(struct w_button *b)
 {
 struct node *n;
 int i;
 if(!view.pcurrcube || !view.pcurrcube->d.c->walls[view.currwall])
  { printmsg(TXT_EMPTYWALLNOTLOCK); drawopt(in_wall); return; }
 view.pcurrcube->d.c->walls[view.currwall]->locked^=1;
 for(i=1,n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
  if(n->d.n->d.c->walls[n->no%6]!=NULL)
   { n->d.n->d.c->walls[n->no%6]->locked=
      view.pcurrcube->d.c->walls[view.currwall]->locked;
     i++; }
 if(view.pcurrcube->d.c->walls[view.currwall]->locked)
  printmsg(TXT_LOCKEDTAGGEDSIDES,i);
 else printmsg(TXT_UNLOCKEDTAGGEDSIDES,i);
 plotlevel();
 }

void b_selectmacro(struct w_button *b)
 {
 struct node *n;
 view.pcurrmacro=NULL;
 if(b->d.ls->selected>0 && b->d.ls->selected<b->d.ls->num_options)
  for(n=view.levels.head;n->next!=NULL;n=n->next)
   if(!strcmp(n->d.lev->fullname,b->d.ls->options[b->d.ls->selected]))
    { view.pcurrmacro=n->d.lev; break; }
 if(!view.pcurrmacro) b->d.ls->selected=0;
 drawopt(in_cube); plotlevel();
 }

struct node *adjust_light_nc=NULL;
int adjust_light_wall=-1;

void b_adjustlight(struct w_button *b,int save)
 {
 if(b->d.s->on)
  {
  if(adjust_light_nc!=NULL)
   { waitmsg(TXT_DOUBLEADJLIGHT,adjust_light_nc->no,adjust_light_wall);
     b->d.s->on=0; w_drawbutton(b); return; }
  if(view.pcurrwall==NULL || view.pcurrwall->ls==NULL) return;
  adjust_light_nc=view.pcurrcube; adjust_light_wall=view.currwall;
  start_adjustlight(view.pcurrcube->d.c->walls[view.currwall]);
  }
 else
  {
  if(adjust_light_nc==NULL) return;
  if(!save && !yesnomsg(TXT_CANCELADJLIGHT)) return;
  end_adjustlight(adjust_light_nc->d.c->walls[adjust_light_wall],save);
  adjust_light_nc=NULL;
  }
 plotlevel(); drawopts();
 }
void b_l_adjustlight(struct w_button *b) { b_adjustlight(b,1); }
void b_r_adjustlight(struct w_button *b) { b_adjustlight(b,0); }

void b_refreshtagno(enum infos what)
 {
 if(what>=tt_number) return;
 if(l==NULL) sprintf(b_optwins[what][7]->d.str->str,"-----");
 else 
  if(l->tagged[what].size>99999)
   sprintf(b_optwins[what][7]->d.str->str,"*****");
  else
   sprintf(b_optwins[what][7]->d.str->str,"%.5d",l->tagged[what].size);
 }

struct w_b_press b_prev = { 25,5,b_prevobj,NULL,b_prevobj,b_firstobj };
struct w_b_press b_next = { 25,5,b_nextobj,NULL,b_nextobj,b_lastobj };
int init_prev_no_next(enum infos what)
 {
 struct w_b_string *b_no,*b_tagcount;
 struct w_b_switch *b_tag,*b_default,*b_locked,*b_adjustlight;
 struct w_b_press *b_ins,*b_del;
 int y;
 checkmem(b_no=MALLOC(sizeof(struct w_b_string)));
 b_no->max_length=4;
 checkmem(b_no->str=MALLOC(b_no->max_length+1));
 b_no->str[0]=0; b_no->allowed_char=isdigit;
 b_no->l_char_entered=b_no->r_char_entered=NULL;
 b_no->l_string_entered=b_no->r_string_entered=b_numobj;
 checkmem(b_tagcount=MALLOC(sizeof(struct w_b_string)));
 b_tagcount->max_length=5;
 checkmem(b_tagcount->str=MALLOC(b_tagcount->max_length+1));
 b_tagcount->str[0]=0; b_tagcount->allowed_char=isdigit;
 b_tagcount->l_char_entered=b_tagcount->r_char_entered=NULL;
 b_tagcount->l_string_entered=b_tagcount->r_string_entered=NULL;
 checkmem(b_tag=MALLOC(sizeof(struct w_b_switch)));
 b_tag->on=0;
 b_tag->l_routine=b_tagoneobject;
 b_tag->r_routine=b_tagallobjects;
 checkmem(b_ins=MALLOC(sizeof(struct w_b_press)));
 checkmem(b_del=MALLOC(sizeof(struct w_b_press)));
 b_ins->delay=b_del->delay=0; b_ins->repeat=b_del->repeat=-1;
 b_ins->l_pressed_routine=b_ins->r_pressed_routine=b_del->l_pressed_routine=
  b_del->r_pressed_routine=NULL;
 b_ins->l_routine=b_l_insert; b_ins->r_routine=b_r_insert;
 b_del->l_routine=b_l_delete; b_del->r_routine=b_r_delete;
 checkmem(b_optwins[what][1]=w_addstdbutton(optionwins[what],w_b_string,
  w_xwininsize(optionwins[what])/8,0,w_xwininsize(optionwins[what])-
  (w_xwininsize(optionwins[what])/4)*3,-1,NULL,b_no,0));
 checkmem(b_optwins[what][0]=w_addstdbutton(optionwins[what],w_b_press,
  0,0,w_xwininsize(optionwins[what])/8,b_optwins[what][1]->ysize,
  "<",&b_prev,0));
 checkmem(b_optwins[what][2]=w_addstdbutton(optionwins[what],w_b_press,
  b_optwins[what][1]->xpos+b_optwins[what][1]->xsize,0,
  w_xwininsize(optionwins[what])/8,b_optwins[what][1]->ysize,">",
  &b_next,0));
 checkmem(b_optwins[what][3]=w_addstdbutton(optionwins[what],w_b_switch,
  b_optwins[what][2]->xpos+b_optwins[what][2]->xsize,0,
  w_xwininsize(optionwins[what])*7/32,b_optwins[what][1]->ysize,TXT_TAG,
  b_tag,0));
 checkmem(b_optwins[what][7]=w_addstdbutton(optionwins[what],w_b_string,
  b_optwins[what][3]->xpos+b_optwins[what][3]->xsize,0,
  w_xwininsize(optionwins[what])-b_optwins[what][3]->xpos-
  b_optwins[what][3]->xsize,b_optwins[what][1]->ysize,NULL,b_tagcount,0));
 w_dontchangebutton(b_optwins[what][7]);
 if(what!=in_pnt) 
  { checkmem(b_optwins[what][4]=w_addstdbutton(optionwins[what],w_b_press,
     0,b_optwins[what][2]->ypos+b_optwins[what][2]->ysize,
     w_xwininsize(optionwins[what])/(what==in_edge ? 1 : 2),-1,TXT_INSERT,
     b_ins,0));
    if(what!=in_edge)
     { checkmem(b_optwins[what][5]=w_addstdbutton(optionwins[what],w_b_press,
        w_xwininsize(optionwins[what])/2,b_optwins[what][2]->ypos+
        b_optwins[what][2]->ysize,w_xwininsize(optionwins[what])-
        w_xwininsize(optionwins[what])/2,-1,TXT_DELETE,b_del,0)); }
    else b_optwins[what][5]=b_optwins[what][4]; /* see below */ }
 else 
  { b_optwins[what][5]=b_optwins[what][4]=b_optwins[what][3]; }
    /* NULL would be true,but difficult to handle */
 b_optwins[what][2]->data=b_optwins[what][0]->data=
  b_optwins[what][1];
 b_optwins[what][1]->data=b_optwins[what][3]->data=
  b_optwins[what][4]->data=b_optwins[what][5]->data=(void *)what;
 switch(what)
  {
  case in_cube:
   checkmem(view.b_macros=MALLOC(sizeof(struct w_b_choose)));
   view.b_macros->num_options=1;
   checkmem(view.b_macros->options=MALLOC(sizeof(char *)));
   view.b_macros->options[0]=TXT_NONE;
   view.b_macros->selected=0;
   view.b_macros->select_lroutine=view.b_macros->select_rroutine=
    b_selectmacro;
   checkmem(b_optwins[what][6]=w_addstdbutton(optionwins[what],w_b_choose,
    0,b_optwins[what][5]->ypos+b_optwins[what][5]->ysize,
    w_xwininsize(optionwins[what]),-1,TXT_MACRO,view.b_macros,0));
   y=b_optwins[what][6]->ypos+b_optwins[what][6]->ysize;
   b_optwins[what][9]=NULL; b_optwins[what][8]=NULL; 
   break;   
  case in_wall:
   checkmem(b_default=MALLOC(sizeof(struct w_b_switch)));
   b_default->on=0;
   b_default->l_routine=b_l_defaultobject;
   b_default->r_routine=b_r_defaultobject;
   checkmem(b_locked=MALLOC(sizeof(struct w_b_switch)));
   b_locked->on=0;
   b_locked->l_routine=b_l_lockwall;
   b_locked->r_routine=b_r_lockwall;
   checkmem(b_adjustlight=MALLOC(sizeof(struct w_b_switch)));
   b_adjustlight->on=0;
   b_adjustlight->l_routine=b_l_adjustlight;
   b_adjustlight->r_routine=b_r_adjustlight;
   checkmem(b_optwins[what][6]=w_addstdbutton(optionwins[what],w_b_switch,
    0,b_optwins[what][5]->ypos+b_optwins[what][5]->ysize,
    w_xwininsize(optionwins[what])/2,-1,TXT_DEFAULT,b_default,0));
   checkmem(b_optwins[what][8]=w_addstdbutton(optionwins[what],w_b_switch,
    w_xwininsize(optionwins[what])/2,b_optwins[what][5]->ypos+
    b_optwins[what][5]->ysize,(w_xwininsize(optionwins[what])+1)/2,-1,
    TXT_LOCKED,b_locked,0));
   b_optwins[what][6]->data=b_optwins[what][8]->data=(void *)what;
   if(init.d_ver>=d2_10_sw)
    {
    checkmem(b_optwins[what][9]=w_addstdbutton(optionwins[what],w_b_switch,
     0,b_optwins[what][6]->ypos+b_optwins[what][6]->ysize,
     w_xwininsize(optionwins[what]),-1,TXT_ADJUSTLIGHT,b_adjustlight,0));
    y=b_optwins[what][9]->ypos+b_optwins[what][9]->ysize;
    }
   else
    { y=b_optwins[what][8]->ypos+b_optwins[what][8]->ysize;
    b_optwins[what][9]=NULL; }
   break;
  default:
   b_optwins[what][6]=NULL; b_optwins[what][8]=NULL; b_optwins[what][9]=NULL;
   y=b_optwins[what][5]->ypos+b_optwins[what][5]->ysize; 
  }
 return y;
 }

void nextobject(enum tagtypes tt)
 { wi_changeobject((struct w_button *)b_optwins[tt][2]->data,-2); }
void prevobject(enum tagtypes tt)
 { wi_changeobject((struct w_button *)b_optwins[tt][0]->data,-3); }
void resetsideedge(void)
 { wi_changeobject((struct w_button *)b_optwins[tt_wall][0]->data,-4);
   wi_changeobject((struct w_button *)b_optwins[tt_edge][0]->data,-4); }
