/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    wi_buts.c - Drawing buttons -- internal routines
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

#include "wins_int.h"
#include "w_init.h"
#include "w_draw.h"
#include "w_tools.h"
#include "wi_buts.h"

/* give the coordinates for the choosebox of button b (in xp,yp,xs,ys).
 return the number of columns, the number of entries in a column
 (collength) and the pixelwidth of a column (colwidth) */
int wi_choosecoords(struct wi_button *b,int *xp,int *yp,int *xs,int *ys,
 int *collength,int *colwidth)
 {
 const char **s,**options;
 int columns=1,num_options,d_xsize,one_ys;
 if(b->b.type==w_b_tag)
  { num_options=b->b.d.tag->num_options;
    options=b->b.d.tag->options;
    d_xsize=0; one_ys=shapes.titlebar_height+2; }
 else
  { num_options=b->b.d.ls->num_options;
    options=b->b.d.ls->options;
    d_xsize=b->b.d.ls->d_xsize; one_ys=shapes.titlebar_height; }
 for(s=options,*xs=b->b.xsize-d_xsize;s<&options[num_options];s++)
  if(*xs<ws_pixstrlen(*s)+10) *xs=ws_pixstrlen(*s)+10;
 *ys=num_options*one_ys+10;
 *xp=w_xwinincoord(b->b.w,b->b.xpos)+d_xsize+2;
 *yp=w_ywinincoord(b->b.w,b->b.ypos)+b->b.ysize+1;
 while((*ys+columns-1)/columns>=notes.winspace.ys-10) columns++;
 *colwidth=*xs; *collength=(num_options+columns-1)/columns;
 *xs=*xs*columns; *ys=*collength*one_ys+10;
 if(*xs>notes.winspace.xs) *xs=notes.winspace.xs;
 if(*yp+*ys>=notes.winspace.yp+notes.winspace.ys)
  { *yp=notes.winspace.yp+notes.winspace.ys-*ys-1;
    *xp+=b->b.xsize-d_xsize; 
    if(*xp+*xs>=notes.winspace.xp+notes.winspace.xs)
     *xp-=b->b.xsize+2+*xs; }
 else if(*xp+*xs>=notes.winspace.xp+notes.winspace.xs) 
  *xp=notes.winspace.xp+notes.winspace.xs-*xs-1;
 return columns;
 }
 
/* Draw button b. Return 0 if not successful, 1 otherwise. 
   highlighted=1. highlight button
   special for tag button:
    highlighted=1 highlight tag button itself
    highlighted=2 draw list for tag button and highlight tag button
    highlighted=3 current button */
int wi_drawbutton(struct wi_button *b,int highlighted)
 {
 struct node *n;
 const char **s;
 char buffer[1024];
 int xp,yp,xs,ys,i,x,y,columns,colwidth,collength;
 my_assert(b!=NULL && b->b.w!=NULL);
 if(!b->sys_button && (b->b.xpos<0 || b->b.ypos<0 || b->b.xsize<0 || 
  b->b.ysize<0 || b->b.xpos+b->b.xsize>w_xwininsize(b->b.w) || 
  b->b.ypos+b->b.ysize>w_ywininsize(b->b.w)))
  { fprintf(errf,"Button not in window: %d %d %d %d / %d %d\n",
     b->b.xpos,b->b.ypos,b->b.xsize,b->b.ysize,w_xwininsize(b->b.w),
     w_ywininsize(b->b.w)); return 0;
     /* button is not in the window and no system button */ }
 if(!b->drawn) return 1;
 if(!b->activated) highlighted=0;
 if(notes.cur_but && highlighted==0 && notes.cur_but->d.w_b==b) highlighted=3;
 switch(b->b.type)
  {
  case w_b_press: case w_b_drag: 
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos),
    w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,1,
    notes.colindex[highlighted==1 ? cv_buttonrb : 
     (b->sys_button && (struct wi_window *)b->b.w==notes.cur_win ?
      cv_curwinlt : cv_buttonlt)],
    notes.colindex[highlighted==1 ? cv_buttonlt : 
     (b->sys_button && (struct wi_window *)b->b.w==notes.cur_win ?
      cv_curwinrb : cv_buttonrb)],
    notes.colindex[highlighted==1 ? cv_buttonpressed : 
     (b->sys_button && (struct wi_window *)b->b.w==notes.cur_win ? 
      cv_curwin : cv_buttonin)]);
   if(highlighted==3)
    ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
     w_ywinincoord(b->b.w,b->b.ypos)+1,b->b.xsize-2,b->b.ysize-2,
     notes.colindex[cv_buttonmarker],0);
   break;
  case w_b_switch:
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos),
    w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,1,
    notes.colindex[cv_buttonlt],notes.colindex[cv_buttonrb],
    notes.colindex[highlighted==1 ? cv_buttonpressed : cv_buttonin]);
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos)+2,
    w_ywinincoord(b->b.w,b->b.ypos)+2,b->b.xsize-4,b->b.ysize-4,1,
    notes.colindex[b->b.d.s->on ? cv_buttonrb : cv_buttonlt],
    notes.colindex[b->b.d.s->on ? cv_buttonlt : cv_buttonrb],
    notes.colindex[highlighted==1 ? cv_buttonpressed : (b->b.d.s->on ?
     cv_switchon : cv_buttonin)]);   
   if(highlighted==3)
    ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
    w_ywinincoord(b->b.w,b->b.ypos)+1,b->b.xsize-2,b->b.ysize-2,
    notes.colindex[cv_buttonmarker],0);
   break;
  case w_b_choose:
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos),
    w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,1,
    notes.colindex[cv_buttonlt],notes.colindex[cv_buttonrb],
    notes.colindex[cv_buttonin]);
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos)+1+b->b.d.ls->d_xsize,
    w_ywinincoord(b->b.w,b->b.ypos)+2,b->b.xsize-4-b->b.d.ls->d_xsize,
    b->b.ysize-4,1,notes.colindex[highlighted==1 ? cv_buttonrb : cv_buttonlt],
    notes.colindex[highlighted==1 ? cv_buttonlt : cv_buttonrb],
    notes.colindex[cv_buttonin]);   
   if(b->b.d.ls->selected>=0)
    {
    ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+4+b->b.d.ls->d_xsize,
     w_ywinincoord(b->b.w,b->b.ypos)+4,b->b.xsize-b->b.d.ls->d_xsize-6,
     b->b.d.ls->options[b->b.d.ls->selected],
     notes.colindex[highlighted==1 ? cv_textfg : cv_textbg],
     notes.colindex[cv_buttonin]);
    ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+3+b->b.d.ls->d_xsize,
     w_ywinincoord(b->b.w,b->b.ypos)+3,b->b.xsize-b->b.d.ls->d_xsize-6,
     b->b.d.ls->options[b->b.d.ls->selected],
     notes.colindex[highlighted==1 ? cv_textbg : cv_textfg],-1);
    }
   if(highlighted==1)
    {
    columns=wi_choosecoords(b,&xp,&yp,&xs,&ys,&collength,&colwidth);
    checkmem(b->bm=ws_savebitmap(NULL,xp,yp,xs,ys));
    ws_drawframedbox(xp,yp,xs,ys,2,notes.colindex[cv_buttonlt],
     notes.colindex[cv_buttonrb],notes.colindex[cv_buttonin]);
    for(s=b->b.d.ls->options,x=0,y=-1;
     s<&b->b.d.ls->options[b->b.d.ls->num_options];s++)
     {
     if(++y>=collength) { y=0; x+=colwidth; }
     ws_drawtext(xp+x+5,yp+y*shapes.titlebar_height+4,colwidth-7,*s,
      notes.colindex[cv_textbg],notes.colindex[cv_buttonin]); 
     ws_drawtext(xp+x+4,yp+y*shapes.titlebar_height+3,colwidth-6,*s,
      notes.colindex[cv_textfg],-1); 
     }
    }
   else
    {
    if(b->bm!=NULL)
     { ws_restorebitmap(b->bm); b->bm=NULL; }
    if(highlighted==3)
     ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
     w_ywinincoord(b->b.w,b->b.ypos)+1,b->b.xsize-2,b->b.ysize-2,
     notes.colindex[cv_buttonmarker],0);
    }
   break;
  case w_b_tag:
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos),
    w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,1,
    notes.colindex[cv_buttonlt],notes.colindex[cv_buttonrb],
    notes.colindex[cv_buttonin]);
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos)+2,
    w_ywinincoord(b->b.w,b->b.ypos)+2,b->b.xsize-4,b->b.ysize-4,1,
    notes.colindex[highlighted==1 ? cv_buttonrb : cv_buttonlt],
    notes.colindex[highlighted==1 ? cv_buttonlt : cv_buttonrb],
    notes.colindex[cv_buttonin]);   
   if(highlighted==2)
    {
    columns=wi_choosecoords(b,&xp,&yp,&xs,&ys,&collength,&colwidth);
    if(b->bm==NULL) checkmem(b->bm=ws_savebitmap(NULL,xp,yp,xs,ys));
    ws_drawframedbox(xp,yp,xs,ys,2,notes.colindex[cv_buttonlt],
     notes.colindex[cv_buttonrb],notes.colindex[cv_buttonin]);
    for(i=0,x=0,y=-1;i<b->b.d.tag->num_options;i++)
     {
     if(++y>=collength) { y=0; x+=colwidth; }
     if(b->b.d.tag->tagged[i])
      ws_drawframedbox(xp+x+1,yp+y*(shapes.titlebar_height+2)+3,colwidth-4,
       shapes.titlebar_height+2,1,notes.colindex[cv_buttonrb],
       notes.colindex[cv_buttonlt],notes.colindex[cv_buttonin]);
     else
      ws_drawfilledbox(xp+x+1,yp+y*(shapes.titlebar_height+2)+3,colwidth-4,
       shapes.titlebar_height+2,notes.colindex[cv_buttonin],0);
     ws_drawtext(xp+x+5,yp+y*(shapes.titlebar_height+2)+5,
      colwidth-7,b->b.d.tag->options[i],
      notes.colindex[b->b.d.tag->tagged[i] ? cv_textfg :
      cv_textbg],notes.colindex[cv_buttonin]); 
     ws_drawtext(xp+x+4,yp+y*(shapes.titlebar_height+2)+4,colwidth-6,
      b->b.d.tag->options[i],notes.colindex[b->b.d.tag->tagged[i] ? 
      cv_textbg : cv_textfg],-1); 
     }
    }
   else 
    {
    if(b->bm!=NULL)
     { ws_restorebitmap(b->bm); b->bm=NULL; }
    if(highlighted==3)
     ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
     w_ywinincoord(b->b.w,b->b.ypos)+1,b->b.xsize-2,b->b.ysize-2,
     notes.colindex[cv_buttonmarker],0);
    } 
   break;
  case w_b_string:
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos),
    w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,1,
    notes.colindex[highlighted==1 ? cv_buttonrb : cv_buttonlt],
    notes.colindex[highlighted==1 ? cv_buttonlt : cv_buttonrb],
    notes.colindex[highlighted==1 ? cv_buttonpressed : cv_buttonin]);
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos)+1+b->b.d.str->d_xsize,
    w_ywinincoord(b->b.w,b->b.ypos)+2,b->b.xsize-4-b->b.d.str->d_xsize,
    b->b.ysize-4,1,notes.colindex[highlighted==1 ? cv_buttonrb : cv_buttonlt],
    notes.colindex[highlighted==1 ? cv_buttonlt : cv_buttonrb],
    notes.colindex[cv_bg]);   
   if(strlen(&b->b.d.str->str[b->b.d.str->offset])>b->b.d.str->length)
    { strncpy(buffer,&b->b.d.str->str[b->b.d.str->offset],b->b.d.str->length);
      buffer[b->b.d.str->length]=0; }
   else strcpy(buffer,&b->b.d.str->str[b->b.d.str->offset]);
   ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+4+b->b.d.str->d_xsize,
    w_ywinincoord(b->b.w,b->b.ypos)+4,b->b.xsize-6,
    buffer,notes.colindex[cv_textfg],notes.colindex[cv_bg]);
   if(highlighted==3)
    ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
    w_ywinincoord(b->b.w,b->b.ypos)+1,b->b.xsize-2,b->b.ysize-2,
    notes.colindex[cv_buttonmarker],0);
   break;
  case w_b_list:
   ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos),
    w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,1,
    notes.colindex[cv_buttonlt],notes.colindex[cv_buttonrb],
    notes.colindex[cv_buttonin]);
   for(i=0;i<=(b->b.ysize-4)/(shapes.titlebar_height+2) && 
    i<b->b.d.sls->no_strings-b->b.d.sls->start_no;i++)
    {
    xp=((b->b.d.sls->sel_arr[i+b->b.d.sls->start_no]&1)!=0);
    if(xp)
     ws_drawframedbox(w_xwinincoord(b->b.w,b->b.xpos)+2,
      w_ywinincoord(b->b.w,b->b.ypos)+(shapes.titlebar_height+2)*i+1,
      b->b.xsize-4,shapes.titlebar_height+1,1,notes.colindex[cv_buttonrb],
      notes.colindex[cv_buttonlt],notes.colindex[cv_buttonin]);
    if(notes.cur_but && highlighted==1 && notes.cur_but->d.w_b==b &&
     (b->b.d.sls->sel_arr[i+b->b.d.sls->start_no]&2)!=0)
     ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
      w_ywinincoord(b->b.w,b->b.ypos)+(shapes.titlebar_height+2)*i+1,
      b->b.xsize-2,shapes.titlebar_height+1,
      notes.colindex[cv_buttonmarker],0);
    ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+4,
     w_ywinincoord(b->b.w,b->b.ypos)+(shapes.titlebar_height+2)*i+3,
     b->b.xsize-6,b->b.d.sls->strings[i+b->b.d.sls->start_no],
     notes.colindex[xp ? cv_textfg : cv_textbg],notes.colindex[cv_buttonin]);
    ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+3,
     w_ywinincoord(b->b.w,b->b.ypos)+(shapes.titlebar_height+2)*i+2,
     b->b.xsize-6,b->b.d.sls->strings[i+b->b.d.sls->start_no],
     notes.colindex[!xp ? cv_textfg : cv_textbg],-1);
    }
   if(highlighted==3)
    ws_drawbox(w_xwinincoord(b->b.w,b->b.xpos)+1,
    w_ywinincoord(b->b.w,b->b.ypos)+1,b->b.xsize-2,b->b.ysize-2,
    notes.colindex[cv_buttonmarker],0);
   break;
  default: return 0;
  }
 for(n=highlighted==1 ? b->b.a_inscription.head : b->b.inscription.head;
  n->next!=NULL;n=n->next)
  switch(n->d.w_d->type)
   {
   case w_bdt_txt:
    ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+n->d.w_d->x+1,
     w_ywinincoord(b->b.w,b->b.ypos)+n->d.w_d->y+1,b->b.xsize,
     n->d.w_d->contents.txt,
     notes.colindex[highlighted==1 ? cv_textfg : cv_textbg],-1);
    ws_drawtext(w_xwinincoord(b->b.w,b->b.xpos)+n->d.w_d->x,
     w_ywinincoord(b->b.w,b->b.ypos)+n->d.w_d->y,b->b.xsize,
     n->d.w_d->contents.txt,
     notes.colindex[highlighted==1 ? cv_textbg : cv_textfg],-1);
    break;
   case w_bdt_image: 
    ws_copybitmap(NULL,w_xwinincoord(b->b.w,b->b.xpos)+n->d.w_d->x,
     w_ywinincoord(b->b.w,b->b.ypos)+n->d.w_d->y,n->d.w_d->contents.bm,
     0,0,n->d.w_d->contents.bm->xsize,n->d.w_d->contents.bm->ysize,
     n->d.w_d->image_with_bg);
    break;
   }
 if(!b->activated)
  {
  ws_drawpatternedbox(w_xwinincoord(b->b.w,b->b.xpos),
   w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,
   notes.colindex[cv_buttonin]);
  /*
  ws_drawline(w_xwinincoord(b->b.w,b->b.xpos),w_ywinincoord(b->b.w,b->b.ypos),
   w_xwinincoord(b->b.w,b->b.xpos+b->b.xsize),
   w_ywinincoord(b->b.w,b->b.ypos+b->b.ysize),notes.colindex[cv_bg],0);
  ws_drawline(w_xwinincoord(b->b.w,b->b.xpos+b->b.xsize),
   w_ywinincoord(b->b.w,b->b.ypos),w_xwinincoord(b->b.w,b->b.xpos),
   w_ywinincoord(b->b.w,b->b.ypos+b->b.ysize),notes.colindex[cv_bg],0);
  */
  }
 return 1;
 }

void wi_changecurbutton(struct node *n)
 {
 struct node *on=notes.cur_but;
 if(on==n) return;
 notes.cur_but=n;
 my_assert(n==NULL || on==NULL || n->d.w_b->b.w==on->d.w_b->b.w);
 if(on)
  { w_refreshstart(on->d.w_b->b.w);
    wi_drawbutton(on->d.w_b,0); 
    if(n) wi_drawbutton(n->d.w_b,0); 
    w_refreshend(on->d.w_b->b.w); }
 else if(n)
  { w_refreshstart(n->d.w_b->b.w);
    wi_drawbutton(n->d.w_b,0);
    w_refreshend(n->d.w_b->b.w); }
 }
 
void wi_undrawbutton(struct wi_button *b)
 {
 ws_drawfilledbox(w_xwinincoord(b->b.w,b->b.xpos),
  w_ywinincoord(b->b.w,b->b.ypos),b->b.xsize,b->b.ysize,
  notes.colindex[cv_winfill],0);
 }
 
/* Button handling routines */
int wi_b_press(struct ws_event *ws,struct wi_button *b)
 {
 int x,y,delay=1;
 clock_t t1,t2;
 my_assert(b->b.d.d!=NULL)
 if(ws->buttons==ws_bt_none) return 0;
 wi_drawbutton(b,1);
 t1=clock();
 do
  {
  ws_getevent(ws,0);
  x=w_xscreencoord(b->b.w,ws->x); y=w_yscreencoord(b->b.w,ws->y);
  t2=clock();
  if(b->b.type==w_b_press && (((b->b.d.p->l_pressed_routine!=NULL && 
   ws->buttons==ws_bt_left) || (b->b.d.p->r_pressed_routine!=NULL && 
   ws->buttons==ws_bt_right)) && 
   (t2-t1)*100>=CLOCKS_PER_SEC*(delay ? b->b.d.p->delay : b->b.d.p->repeat) &&
   (delay || b->b.d.p->repeat>=0)))
   {
   if(ws->buttons==ws_bt_left) b->b.d.p->l_pressed_routine(&b->b);
   else b->b.d.p->r_pressed_routine(&b->b);
   t1=t2; delay=0;
   }
  }
 while(x>b->b.xpos && x<b->b.xpos+b->b.xsize-1 && y>b->b.ypos && 
  y<b->b.ypos+b->b.ysize-1 && ws->buttons!=ws_bt_none);
 if(ws->buttons!=ws_bt_none) { wi_drawbutton(b,0); return 0; }
 if(b->b.type==w_b_switch) b->b.d.s->on^=1;
 wi_drawbutton(b,0);    
 if(b->b.type==w_b_press)
  {
  if(ws->flags==ws_f_lbutton&&b->b.d.p->l_routine) b->b.d.p->l_routine(&b->b);
  else if(ws->flags==ws_f_rbutton && b->b.d.p->r_routine) 
   b->b.d.p->r_routine(&b->b);
  }
 else if(b->b.type==w_b_switch)
  {
  if(ws->flags==ws_f_lbutton&&b->b.d.s->l_routine) b->b.d.s->l_routine(&b->b);
  else if(ws->flags==ws_f_rbutton && b->b.d.s->r_routine) 
   b->b.d.s->r_routine(&b->b);
  }
 return 1;
 }
 
int wi_b_drag(struct ws_event *ws,struct wi_button *b)
 {
 void (*p1)(struct w_button *,int,int),(*p2)(struct w_button *,int,int),
  (*p3)(struct w_button *,int,int);
 int ox,oy;
 my_assert(b->b.d.d!=NULL && b->b.w!=NULL);
 if(!((ws->flags==ws_f_lbutton && ws->buttons==ws_bt_left) ||
  (ws->flags==ws_f_rbutton && ws->buttons==ws_bt_right))) return 0;
 wi_drawbutton(b,1);
 if(ws->flags==ws_f_lbutton) 
  { p1=b->b.d.dr->l_pressroutine; p2=b->b.d.dr->l_dragroutine;
    p3=b->b.d.dr->l_relroutine; }
 else
  { p1=b->b.d.dr->r_pressroutine; p2=b->b.d.dr->r_dragroutine;
    p3=b->b.d.dr->r_relroutine; }
 ox=ws->x; oy=ws->y;    
 if(p1!=NULL) p1(&b->b,ws->x,ws->y);
 do
  {
  ws_getevent(ws,0);
  if(p2!=NULL && (ox!=ws->x || oy!=ws->y)) 
   { p2(&b->b,ws->x,ws->y); ox=ws->x;oy=ws->y; }
  }
 while(ws->buttons!=ws_bt_none);
 if(p3!=NULL) p3(&b->b,ws->x,ws->y);
 wi_drawbutton(b,0);
 return 1;
 }
 
int wi_b_choose(struct ws_event *ws,struct wi_button *b)
 {
 int i,oldi,xp,yp,xs,ys,cxs,collength,columns,oldx,oldy;
 my_assert(b->b.d.d!=NULL)
 if(ws->buttons==ws_bt_none) return 0;
 wi_drawbutton(b,1);
 columns=wi_choosecoords(b,&xp,&yp,&xs,&ys,&collength,&cxs);
 oldi=-1; oldx=oldy=0; i=b->b.d.ls->selected; xs-=4; xp+=2; yp+=2;
 ws->x=xp+1; /* so the highlighted item is drawn */
 do
  {
  if(i>=0 && i<b->b.d.ls->num_options && i!=oldi && ws->x>xp && ws->x<xp+xs)
   {
   if(oldi>=0)
    {
    ws_drawfilledbox(oldx,oldy,cxs-5,shapes.titlebar_height+2,
     notes.colindex[cv_buttonin],0);
    ws_drawtext(oldx+3,oldy+2,cxs-8,b->b.d.ls->options[oldi],
     notes.colindex[cv_textbg],notes.colindex[cv_buttonin]); 
    ws_drawtext(oldx+2,oldy+1,cxs-7,b->b.d.ls->options[oldi],
     notes.colindex[cv_textfg],-1); 
    }
   oldi=i; oldx=xp+(i/collength)*cxs; 
   oldy=yp+(i%collength)*shapes.titlebar_height;
   ws_drawframedbox(oldx,oldy,cxs-5,shapes.titlebar_height+1,1,
    notes.colindex[cv_buttonrb],notes.colindex[cv_buttonlt],
    notes.colindex[cv_buttonin]);
   ws_drawtext(oldx+3,oldy+2,cxs-8,b->b.d.ls->options[i],
    notes.colindex[cv_textfg],notes.colindex[cv_buttonin]); 
   ws_drawtext(oldx+2,oldy+1,cxs-7,b->b.d.ls->options[i],
    notes.colindex[cv_textbg],-1); 
   }
  ws_getevent(ws,0);
  i=(ws->y>=yp+3)&&(ws->x>=xp+3) ? 
   ((ws->x-xp-3)/cxs)*collength+(ws->y-yp-3)/shapes.titlebar_height : -1;
  }
 while(ws->buttons!=ws_bt_none);
 if(ws->x>xp && ws->x<xp+xs && i>=0 && i<b->b.d.ls->num_options)
  b->b.d.ls->selected=i;
 wi_drawbutton(b,0);
 if(i>=0 && i<b->b.d.ls->num_options)
  {
  if(ws->flags==ws_f_lbutton && b->b.d.ls->select_lroutine) 
   b->b.d.ls->select_lroutine(&b->b);
  else if(ws->flags==ws_f_rbutton && b->b.d.ls->select_rroutine) 
   b->b.d.ls->select_rroutine(&b->b);
  wi_drawbutton(b,0);
  return 1;
  } 
 else return 0;
 }
 
int wi_b_tag(struct ws_event *ws,struct wi_button *b)
 {
 int pos,oldpos,leftright,xp,yp,xs,ys,num_selected,cxs,collength,cols;
 my_assert(b->b.d.d!=NULL)
 if(ws->buttons==ws_bt_none) return 0;
 leftright=(ws->buttons==ws_bt_left);
 wi_drawbutton(b,1);
 do
  {
  ws_getevent(ws,0);
  xp=w_xscreencoord(b->b.w,ws->x); yp=w_yscreencoord(b->b.w,ws->y);
  }
 while(xp>b->b.xpos && xp<b->b.xpos+b->b.xsize-1 && yp>b->b.ypos && 
  yp<b->b.ypos+b->b.ysize-1 && ws->buttons!=ws_bt_none);
 if(ws->buttons!=ws_bt_none) 
  { wi_drawbutton(b,0); return 0; }
 for(pos=0,num_selected=0;pos<b->b.d.tag->num_options;pos++)
  if(b->b.d.tag->tagged[pos]) num_selected++; 
 cols=wi_choosecoords(b,&xp,&yp,&xs,&ys,&collength,&cxs);
 wi_drawbutton(b,2);
 oldpos=-1;
 do
  {
  ws_getevent(ws,0);
  pos=(ws->y>=yp+3)&&(ws->x>=xp+3) ? 
   ((ws->x-xp-3)/cxs)*collength+(ws->y-yp-3)/(shapes.titlebar_height+2) : -1;
  if((ws->buttons==ws_bt_left || ws->buttons==ws_bt_right) && 
   (oldpos!=pos || ws->flags!=ws_f_none) && ws->x>=xp && ws->x<xp+xs)  
   {
   if(oldpos>=0&&oldpos<b->b.d.tag->num_options&&oldpos!=pos) 
    num_selected+=(b->b.d.tag->tagged[oldpos]^=1) ? 1 : -1;
   if(pos>=0 && pos<b->b.d.tag->num_options) 
    if((num_selected+=(b->b.d.tag->tagged[pos]^=1) ? 1 : -1)>
     b->b.d.tag->max_options && b->b.d.tag->max_options>0)
     { b->b.d.tag->tagged[pos]=0; num_selected--; }
   wi_drawbutton(b,2);
   }   
  if(ws->buttons==ws_bt_none && (ws->flags==ws_f_lbutton || 
   ws->flags==ws_f_rbutton)) oldpos=-1;
  else oldpos=pos;      
  }
 while(ws->flags==ws_f_none || (ws->x>=xp && ws->x<xp+xs && ws->y>=yp &&
  ws->y<yp+ys));
 wi_drawbutton(b,0); 
 if(leftright && b->b.d.tag->tag_lroutine) b->b.d.tag->tag_lroutine(&b->b);
 if(!leftright && b->b.d.tag->tag_rroutine) b->b.d.tag->tag_rroutine(&b->b);
 return 1;
 }
 
int wi_b_string(struct ws_event *ws,struct wi_button *b)
 {
 int x,y,p,leftright;
 my_assert(b->b.d.d!=NULL)
 if(ws->buttons==ws_bt_none) return 0;
 leftright=(ws->buttons==ws_bt_left);
 wi_drawbutton(b,1);
 do
  {
  ws_getevent(ws,0);
  x=w_xscreencoord(b->b.w,ws->x); y=w_yscreencoord(b->b.w,ws->y);
  }
 while(x>b->b.xpos && x<b->b.xpos+b->b.xsize-1 && y>b->b.ypos && 
  y<b->b.ypos+b->b.ysize-1 && ws->buttons!=ws_bt_none);
 if(ws->buttons!=ws_bt_none) { wi_drawbutton(b,0); return 0; }
 p=ws_charstrlen(x-b->b.xpos-b->b.d.str->d_xsize)-1;
 return wi_handlestringbutton(ws,b,p,leftright);
 }

int wi_b_list(struct ws_event *ws,struct wi_button *b)
 {
 int i,pos,leftright,num_sel;
 if(ws->buttons==ws_bt_none) return 0;
 leftright=(ws->buttons==ws_bt_left);
 for(pos=0,num_sel=0;pos<b->b.d.sls->no_strings;pos++)
  num_sel+=b->b.d.sls->sel_arr[pos];
 do
  ws_getevent(ws,1);
 while(ws->buttons!=ws_bt_none);
 if(ws->x>=w_xwinincoord(b->b.w,b->b.xpos) && ws->x<
  w_xwinincoord(b->b.w,b->b.xpos+b->b.xsize)) 
  {
  pos=(w_yscreencoord(b->b.w,ws->y)-b->b.ypos-4);
  pos=pos<0 ? -1 : pos/(shapes.titlebar_height+2);
  if(pos>=0 && pos<b->b.d.sls->no_rows && 
   pos+b->b.d.sls->start_no<b->b.d.sls->no_strings)
   { if((b->b.d.sls->sel_arr[b->b.d.sls->start_no+pos]^=1)!=0 &&
      b->b.d.sls->max_selected<++num_sel)
      for(i=0;i<b->b.d.sls->no_strings;i++)
       if(i!=b->b.d.sls->start_no+pos && b->b.d.sls->sel_arr[i])
        { b->b.d.sls->sel_arr[i]=0; num_sel--; }
     wi_drawbutton(b,0); }
  }
 if(leftright && b->b.d.sls->l_string_selected)
  b->b.d.sls->l_string_selected(&b->b);
 else if(!leftright && b->b.d.sls->r_string_selected)
  b->b.d.sls->r_string_selected(&b->b);
 return 1;
 }
 
/* Handle the buttons. return a 0 if the procedure was aborted and 1
   if the user completed the use of the button (whatever that means). */
int (*wi_buttonhandling[w_b_number])(struct ws_event *ws,struct wi_button *b)
 = { wi_b_press,wi_b_drag,wi_b_press,wi_b_choose,wi_b_string,wi_b_tag,
     wi_b_list };

int wi_handlestringbutton(struct ws_event *ws,struct wi_button *b,int p,
 int leftright)
 {
 int end=0,change=0;
 char buffer[256],*oldstr;
 memset(buffer,256,0);
 strncpy(buffer,b->b.d.str->str,254);
 oldstr=b->b.d.str->str; b->b.d.str->str=buffer;
 if(p<0) p=0;
 else if(p>strlen(buffer)) p=strlen(buffer);
 do
  {
  ws_drawline(w_xwinincoord(b->b.w,b->b.xpos)+b->b.d.str->d_xsize+
   (p-b->b.d.str->offset)*ws_pixstrlen("M")+3,w_ywinincoord(b->b.w,b->b.ypos)+
   shapes.titlebar_height,w_xwinincoord(b->b.w,b->b.xpos)+
   b->b.d.str->d_xsize+(p-b->b.d.str->offset+1)*ws_pixstrlen("M")+2,
   w_ywinincoord(b->b.w,b->b.ypos)+shapes.titlebar_height,
   notes.colindex[cv_textfg],1);
  ws_getevent(ws,1);
  if(ws->flags==ws_f_keypress)
   {
   switch(ws->key)
    {
    case 0xd: change=end=1; 
     strcpy(oldstr,buffer); break;
    case 0x1b: end=1; break;
    case 0x253: /* del */
     if(ws->kbstat&ws_ks_shift)
      { buffer[0]=0; p=0; }
     else
      if(p<strlen(buffer)) memmove(&buffer[p],&buffer[p+1],strlen(buffer)-p);
     break;
    case 0x8: /* backspace */
     if(p>0)
      { memmove(&buffer[p-1],&buffer[p],strlen(buffer)-p+1);
        if(--p<b->b.d.str->offset) b->b.d.str->offset=p; }
     break;
    case 0x24b:  /* cursor left */
     if(p>0) if(--p<b->b.d.str->offset) b->b.d.str->offset=p;
     break;
    case 0x24d: /* cursor right */
     if(p<strlen(buffer))
      if(++p>=b->b.d.str->offset+b->b.d.str->length) 
       b->b.d.str->offset=p-b->b.d.str->length+1;
     break;
    case 0x247: p=0; b->b.d.str->offset=0; break; /* home */
    case 0x24f: p=strlen(buffer); b->b.d.str->offset=p-b->b.d.str->length+1;
     break; /* end */
    default:
     if(isprint(ws->key) && (b->b.d.str->allowed_char==NULL ||
      b->b.d.str->allowed_char(ws->key)) && p<b->b.d.str->max_length)
      {
      if(buffer[p]==0) buffer[p+1]=0;
      buffer[p++]=ws->key;
      if(p>=b->b.d.str->offset+b->b.d.str->length) 
       b->b.d.str->offset=p-b->b.d.str->length+1;
      }
    }
   if(leftright && b->b.d.str->l_char_entered) 
    b->b.d.str->l_char_entered(&b->b);
   else if(!leftright && b->b.d.str->r_char_entered) 
    b->b.d.str->r_char_entered(&b->b);
   }
  else end=1;
  wi_drawbutton(b,1);
  }
 while(!end);
 b->b.d.str->str=oldstr;
 wi_drawbutton(b,0);
 if(change)
  {
  if(leftright)
   { if(b->b.d.str->l_string_entered) b->b.d.str->l_string_entered(&b->b); }
  else 
   { if(b->b.d.str->r_string_entered) b->b.d.str->r_string_entered(&b->b); }
  wi_drawbutton(b,0); 
  }
 return 1;
 }
