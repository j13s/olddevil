/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    wi_keys.c - The handling routines for using keys&buttons
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
#include "wi_buts.h"
#include "wi_keys.h"
    
#define L_BUTKEY 0xd
#define R_BUTKEY 0xd
#define R_KBSTAT 1

int wi_bk_press(struct ws_event *ws,struct wi_button *b)
 {
 int delay=1,oldkey,oldkbstat;
 clock_t t1,t2;
 my_assert(b->b.d.d!=NULL)
 oldkey=ws->key; oldkbstat=ws->kbstat;
 wi_drawbutton(b,1);
 t1=clock();
 do
  {
  ws_getevent(ws,0);
  t2=clock();
  if(b->b.type==w_b_press &&
   (t2-t1)*100>=CLOCKS_PER_SEC*(delay ? b->b.d.p->delay : b->b.d.p->repeat) &&
   (delay || b->b.d.p->repeat>=0))
   {
   if(ws->key==L_BUTKEY && (ws->kbstat&R_KBSTAT)!=R_KBSTAT && 
    b->b.d.p->l_pressed_routine)
    b->b.d.p->l_pressed_routine(&b->b);
   else if(b->b.d.p->r_pressed_routine) b->b.d.p->r_pressed_routine(&b->b);
   t1=t2; delay=0;
   }
  }
 while(ws->key==oldkey);
 if(b->b.type==w_b_switch) b->b.d.s->on^=1;
 wi_drawbutton(b,0);    
 if(b->b.type==w_b_press)
  {
  if(oldkey==L_BUTKEY && (oldkbstat&R_KBSTAT)!=R_KBSTAT &&
   b->b.d.p->l_routine) b->b.d.p->l_routine(&b->b);
  else if(oldkey==R_BUTKEY && b->b.d.p->r_routine) 
   b->b.d.p->r_routine(&b->b);
  }
 else if(b->b.type==w_b_switch)
  {
  if(oldkey==L_BUTKEY && (oldkbstat&R_KBSTAT)!=R_KBSTAT &&
   b->b.d.s->l_routine) b->b.d.s->l_routine(&b->b);
  else if(oldkey==R_BUTKEY && b->b.d.s->r_routine) 
   b->b.d.s->r_routine(&b->b);
  }
 return 1;
 }
 
int wi_bk_choose(struct ws_event *ws,struct wi_button *b)
 {
 int i,oldi,xp,yp,xs,ys,end=0,cxs,collength,oldx,oldy;
 my_assert(b->b.d.d!=NULL)
 wi_drawbutton(b,1);
 wi_choosecoords(b,&xp,&yp,&xs,&ys,&collength,&cxs);
 oldi=-1; oldx=oldy=0; i=b->b.d.ls->selected; cxs-=4; xp+=2; yp+=2;
 do
  {
  if(i>=0 && i<b->b.d.ls->num_options && i!=oldi)
   {
   if(oldi>=0)
    {
    ws_drawfilledbox(oldx,oldy,cxs,shapes.titlebar_height+2,
     notes.colindex[cv_buttonin],0);
    ws_drawtext(oldx+3,oldy+2,cxs-2,b->b.d.ls->options[oldi],
     notes.colindex[cv_textbg],notes.colindex[cv_buttonin]); 
    ws_drawtext(oldx+2,oldy+1,cxs-2,b->b.d.ls->options[oldi],
     notes.colindex[cv_textfg],-1); 
    }
   oldi=i; oldx=xp+(i/collength)*cxs; 
   oldy=yp+(i%collength)*shapes.titlebar_height;
   ws_drawframedbox(oldx,oldy,cxs-1,shapes.titlebar_height+1,1,
    notes.colindex[cv_buttonrb],notes.colindex[cv_buttonlt],
    notes.colindex[cv_buttonin]);
   ws_drawtext(oldx+3,oldy+2,cxs-2,b->b.d.ls->options[i],
    notes.colindex[cv_textfg],notes.colindex[cv_buttonin]); 
   ws_drawtext(oldx+2,oldy+1,cxs-2,b->b.d.ls->options[i],
    notes.colindex[cv_textbg],-1); 
   }
  ws_getevent(ws,1);
  if(ws->flags==ws_f_keypress)
   switch(ws->key)
    {
    case 0x248: i=i<=0 ? b->b.d.ls->num_options-1 : i-1; break; /* up */
    case 0x250: i=i>=b->b.d.ls->num_options-1 ? 0 : i+1; break; /* down */
    case 0x1b: end=1; break; /* esc */
    default:
     if(ws->key==L_BUTKEY && (ws->kbstat&R_KBSTAT)!=R_KBSTAT) end=2;
     else if(ws->key==R_BUTKEY) end=3;
    }
  else end=1;
  }
 while(!end);
 if(end>1 && i>=0 && i<b->b.d.ls->num_options) b->b.d.ls->selected=i;
 wi_drawbutton(b,0);
 if(end>1 && i>=0 && i<b->b.d.ls->num_options)
  {
  if(end==2 && b->b.d.ls->select_lroutine) b->b.d.ls->select_lroutine(&b->b);
  else if(end==3 && b->b.d.ls->select_rroutine) 
   b->b.d.ls->select_rroutine(&b->b);
  wi_drawbutton(b,0);
  return 1;
  } 
 else return 0;
 }
 
int wi_bk_tag(struct ws_event *ws,struct wi_button *b)
 {
 int pos,leftright,xp,yp,xs,ys,num_selected,end=0,cols,collength,cxs;
 my_assert(b->b.d.d!=NULL)
 leftright=(ws->key==L_BUTKEY && (ws->kbstat&R_KBSTAT)!=R_KBSTAT);
 wi_drawbutton(b,1);
 for(pos=0,num_selected=0;pos<b->b.d.tag->num_options;pos++)
  if(b->b.d.tag->tagged[pos]) num_selected++; 
 cols=wi_choosecoords(b,&xp,&yp,&xs,&ys,&collength,&cxs);
 wi_drawbutton(b,2); pos=0;
 do
  {
  ws_drawbox(xp+(pos/collength)*cxs+1,yp+(pos%collength)*
   (shapes.titlebar_height+2)+2,cxs-3,
   shapes.titlebar_height+3,notes.colindex[cv_buttonmarker],0);  
  ws_getevent(ws,1);
  if(ws->flags==ws_f_keypress)
   switch(ws->key)
    {
    case 0x248: pos=pos<=0 ? b->b.d.tag->num_options-1 : pos-1; break; /*up*/
    case 0x250: pos=pos>=b->b.d.tag->num_options-1 ? 0 : pos+1; break;/*down*/
    case 0x1b: end=1; break; /* esc */
    default:
     if(ws->key==L_BUTKEY || ws->key==R_BUTKEY)
      if((num_selected+=(b->b.d.tag->tagged[pos]^=1) ? 1 : -1)>
       b->b.d.tag->max_options && b->b.d.tag->max_options>0)
       { b->b.d.tag->tagged[pos]=0; num_selected--; }
     break;
    }
  else end=1;
  wi_drawbutton(b,2);
  }
 while(!end);
 wi_drawbutton(b,0); 
 if(leftright && b->b.d.tag->tag_lroutine) b->b.d.tag->tag_lroutine(&b->b);
 if(!leftright && b->b.d.tag->tag_rroutine) b->b.d.tag->tag_rroutine(&b->b);
 return 1;
 }
 
int wi_bk_string(struct ws_event *ws,struct wi_button *b)
 {
 int leftright;
 my_assert(b->b.d.d!=NULL)
 leftright=(ws->key==L_BUTKEY && (ws->kbstat&R_KBSTAT)!=R_KBSTAT);
 wi_drawbutton(b,1);
 return wi_handlestringbutton(ws,b,0,leftright);
 }

int wi_bk_list(struct ws_event *ws,struct wi_button *b)
 {
 int end=0,pos,leftright,i,num_sel;
 my_assert(ws!=NULL && b!=NULL);
 if(b->b.d.sls->no_strings<=0) return 0;
 leftright=(ws->key==L_BUTKEY && (ws->kbstat&R_KBSTAT)!=R_KBSTAT);
 for(i=0,num_sel=0;i<b->b.d.sls->no_strings;i++)
  num_sel+=b->b.d.sls->sel_arr[i];
 pos=0;
 do
  {
  b->b.d.sls->sel_arr[pos]|=2;
  wi_drawbutton(b,1);
  b->b.d.sls->sel_arr[pos]^=2;
  ws_getevent(ws,1);
  if(ws->flags==ws_f_keypress)
   {
   switch(ws->key)
    {
    case 0x1b: end=2; break;
    case 0xd: end=1; break;
    case 0x20: 
     b->b.d.sls->sel_arr[pos]^=1;
     if(b->b.d.sls->sel_arr[pos] && ++num_sel>b->b.d.sls->max_selected)
      for(i=0;i<b->b.d.sls->no_strings;i++)
       if(i!=pos && b->b.d.sls->sel_arr[i])
        { b->b.d.sls->sel_arr[i]=0; num_sel--; }
     break;
    case 0x247: pos=0; b->b.d.sls->start_no=0; break;/*home*/
    case 0x24f: pos=b->b.d.sls->no_strings-1; break; /* end*/
    case 0x248: --pos; break; /* up */
    case 0x249: pos-=(b->b.d.sls->no_rows-1)/2;break;/*pdown*/
    case 0x250: ++pos; break; /* down */
    case 0x251: pos+=(b->b.d.sls->no_rows+1)/2; break;/*pup*/
    default:
     if(isprint(ws->key)) 
      {
      for(i=0;i<b->b.d.sls->no_strings;i++)
       if(toupper(ws->key)<=toupper(b->b.d.sls->strings[i][0])) break;
      pos=i;
      }
    }
   fprintf(errf,"1 pos=%d start_no=%d\n",pos,b->b.d.sls->start_no);
   if(pos<0) pos=0;
   else if(pos>=b->b.d.sls->no_strings)
    pos=b->b.d.sls->no_strings-1; 
   if(pos<b->b.d.sls->start_no) 
    b->b.d.sls->start_no=pos;
   else if(pos>=b->b.d.sls->start_no+b->b.d.sls->no_rows) 
    if((b->b.d.sls->start_no=pos-b->b.d.sls->no_rows+1)<0)
     b->b.d.sls->start_no=0;
   fprintf(errf,"2 pos=%d start_no=%d\n",pos,b->b.d.sls->start_no);
   }
  else end=2;
  }
 while(!end);
 wi_drawbutton(b,0);
 if(end==1)
  if(leftright && b->b.d.sls->l_string_selected)
   b->b.d.sls->l_string_selected(&b->b);
  else if(!leftright && b->b.d.sls->r_string_selected)
   b->b.d.sls->r_string_selected(&b->b);
 return end==2 ? 0 : 1;
 }
 
/* Handle the buttons. return a 0 if the procedure was aborted and 1
   if the user completed the use of the button (whatever that means). */
int (*wi_butkeyhandling[w_b_number])(struct ws_event *ws,struct wi_button *b)
 = { wi_bk_press,NULL,wi_bk_press,wi_bk_choose,wi_bk_string,wi_bk_tag,
     wi_bk_list };
