/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_winman.c - Handling of windows and screens - official routines
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
#include "w_tools.h"
#include "wi_keys.h"
#include "wi_buts.h"
#include "wi_menu.h"
#include "wi_winma.h"
#include "w_init.h"
#include "w_draw.h"
#include "w_event.h"

struct w_window *msg_win;
int msg_win_x,msg_win_y; /*Position of the mouse when the window was opened*/

struct wi_button *wi_checkforbutton(struct wi_window *wi,int x,int y)
 {
 struct node *n;
 struct wi_button *bi;
 /* Test if a button was hit */
 for(n=wi->buttonlist.head,bi=NULL;n->next!=NULL;n=n->next)
  if(n->d.w_b->drawn && x>n->d.w_b->b.xpos && 
   x<n->d.w_b->b.xpos+n->d.w_b->b.xsize-1 && y>n->d.w_b->b.ypos && 
   y<n->d.w_b->b.ypos+n->d.w_b->b.ysize-1 && 
   n->d.w_b->b.xpos+n->d.w_b->b.xsize-1<wi->w.xsize &&
   n->d.w_b->b.ypos+n->d.w_b->b.ysize-1<wi->w.ysize)
    { bi=n->d.w_b; break; } 
 if(bi!=NULL && bi->activated) 
  { my_assert(bi->b.type<w_b_number); return bi; }
 return NULL;
 }
 
void w_killcurrentmessage(void)
 { if(msg_win) { w_closewindow(msg_win); msg_win=NULL; } }

void (*handleuser_permrout)(void)=NULL;

void w_setpermanentroutine(void (*permrout)(void))
 { handleuser_permrout=permrout; }

/* Wait for the user doing something and call the corresponding procedures.
   This routine quits when one of the buttons in the endbuttons array is hit
   or when a key with the flag 'exit' is pressed.
   It returns the position of the button (lower 16bit)/key (higher 16bit) in
   the array. 
   It returns -1 if all windows are closed that are in the array wins. 
   If endbutton==NULL (num_endbuts==0) and wins==NULL (num_wins==0) this 
   routine has a loop in it, so it never returns. Then you must end the 
   program via an exit in one of the called procedures. 
   Only clicks in the windows out of the array wins are accepted. If 
   wins==NULL (num_wins==0) then every click is accepted */
int w_handleuser(int num_endbuts,struct w_button **endbuttons,int num_wins,
 struct w_window **wins,int num_keycodes,struct w_keycode *ec_keys,
 void (**keyhandling)(int evcode))
 {
 struct ws_event ws;
 struct w_event wev,*we;
 struct node *n;
 int event,end=-2,i,winnum,winclosed,numclosedwins=0;
 struct wi_window *wi=NULL;
 struct wi_button *bi=NULL;
 we=&wev;
 if(wins) w_butwin(wins[0]);
 do
  {
  if(handleuser_permrout) handleuser_permrout();
  event=ws_getevent(&ws,0);
  for(i=0;i<notes.num_kbstatcursors;i++)
   if((ws.kbstat&notes.kbstatcursors[i].bitmask)==
    notes.kbstatcursors[i].kbstat)
     { if(notes.current_cursor!=i)
        { ws_changecursor(notes.kbstatcursors[i].cursor);
	  notes.current_cursor=i; }
       break; }
  if(i==notes.num_kbstatcursors && notes.current_cursor>=0)
   { ws_changecursor(NULL); notes.current_cursor=-1; }
  if(msg_win)
   {
   if(ws.buttons!=ws_bt_none && ws.x>=msg_win->xpos && 
    ws.x<msg_win->xpos+msg_win->xsize && ws.y>=msg_win->ypos &&
    ws.y<msg_win->ypos+msg_win->ysize) continue;
   i=(!event || (ws.x>=msg_win->xpos && ws.x<msg_win->xpos+msg_win->xsize &&
    ws.y>=msg_win->ypos && ws.y<msg_win->ypos+msg_win->ysize));
   if(msg_win_x<ws.x-20 || msg_win_y<ws.y-20 || msg_win_x>ws.x+20 ||
    msg_win_y>ws.y+20 || (event && (ws.flags==ws_f_keypress || 
     ws.buttons!=ws_bt_none)))
    { w_closewindow(msg_win); msg_win=NULL; }
   if(i) continue;
   }
  if(event && ws.flags==ws_f_keypress)
   { 
   if((ws.kbstat&ws_ks_alt)!=0 && isalnum(ws.key) && num_wins==0)
    { wi_handlemenu(&ws); continue; }
   if(notes.cur_but!=NULL && (ws.key==0x248 || ws.key==0x250 || ws.key==0xd))
    {
    n=notes.cur_but;
    switch(ws.key)
     {
     case 0x248: /* up */
      do
       { n=n->prev;
         if(n->prev==NULL) n=*(&n->prev+1); /*this is the tail of the list*/ }
      while(n!=notes.cur_but && (!n->d.w_b->drawn || !n->d.w_b->activated ||
       n->d.w_b->sys_button || !n->d.w_b->tobechanged || 
       wi_butkeyhandling[n->d.w_b->b.type]==NULL));
      break;
     case 0x250: /* down */
      do
       { n=n->next;
         if(n->next==NULL) n=*(&n->next-1); /*this is the head of the list*/ }
      while(n!=notes.cur_but && (!n->d.w_b->drawn || !n->d.w_b->activated ||
       n->d.w_b->sys_button || !n->d.w_b->tobechanged || 
       wi_butkeyhandling[n->d.w_b->b.type]==NULL));
      break;
     case 0xd: /* return */
      if(wi_butkeyhandling[n->d.w_b->b.type]!=NULL)
       { n->d.w_b->b.event=ws;
         if(wi_butkeyhandling[n->d.w_b->b.type](&ws,n->d.w_b) && endbuttons) 
          for(i=0;i<num_endbuts;i++)
           if(n->d.w_b==(struct wi_button *)endbuttons[i]) { end=i; break; } }
      break;
     }
    wi_changecurbutton(n);
    continue;
    }
   if((ws.kbstat&ws_ks_ctrl)!=0 && isalnum(ws.key))
    if(num_wins==0)
     { for(n=notes.windows.head;n->next!=NULL;n=n->next)
        if(ws.key==n->d.w_w->w.hotkey) { w_butwin(&n->d.w_w->w); break; } }
    else
     for(i=0;i<num_wins;i++)
      if(ws.key==wins[i]->hotkey) { w_butwin(wins[i]); break; }
   if(num_keycodes>0)
    for(i=0;i<num_keycodes;i++) 
     {
     if(ec_keys[i].key==ws.key &&
      (ec_keys[i].kbstat&ws.kbstat)==ec_keys[i].kbstat)
       { 
       if(ec_keys[i].event>=0) 
        keyhandling[ec_keys[i].event](ec_keys[i].event); 
       if(ec_keys[i].flags&w_kf_exit) end=i<<16;
       break; 
       }
     }
   continue;
   }
  we->w=NULL; we->x=ws.x; we->y=ws.y; we->ws=&ws; we->w=NULL;
  if(num_wins==0 && ws.y<shapes.titlebar_height+2*shapes.width_menuframe && 
   event)
   wi_handlemenu(&ws);
  else 
   {
   for(n=notes.windows.head;n->next!=NULL;n=n->next)
    if(ws.x>n->d.w_w->w.xpos && ws.x<n->d.w_w->w.xpos+n->d.w_w->w.xsize-1 &&
     ws.y>n->d.w_w->w.ypos && ws.y<n->d.w_w->w.ypos+n->d.w_w->w.ysize-1)
     { wi=n->d.w_w; we->w=&wi->w; we->x=w_xscreencoord(we->w,ws.x);
       we->y=w_yscreencoord(we->w,ws.y); break; }
   if(we->w==NULL) continue;
   if(wins)
    {
    for(winnum=0;winnum<num_wins;winnum++) if(we->w==wins[winnum]) break;
    if(winnum>=num_wins) continue;
    }
   else winnum=-1;
   if(event)
    { 
    /* Get window in the foreground */
    w_wintofront(we->w);
    if(we->w->click_routine) we->w->click_routine(we->w,we->w->data,we);
    /* Test if a button was hit */
    if((bi=wi_checkforbutton(wi,we->x,we->y))!=NULL)
     {
     if(ws.kbstat&ws_ks_alt)
      {if(bi->b.helptxt) w_okcancel(bi->b.helptxt,NULL,NULL,NULL); continue;}
     bi->b.event=ws;      
     /* if window is closed, all buttons are deleted, so this must be done
        before the window is closed. */
     winclosed=(bi->sys_button && !bi->b.data && ws.flags==ws_f_lbutton &&
      (wi->w.buttons&wb_close)!=0);  
     if(bi->tobechanged && wi_buttonhandling[bi->b.type]!=NULL &&
      wi_buttonhandling[bi->b.type](&ws,bi))
      {
      if(endbuttons) 
       for(i=0;i<num_endbuts;i++)
        if(bi==(struct wi_button *)endbuttons[i]) { end=i; break; }
      if(wins && winclosed && ++numclosedwins==num_wins) end=-1; 
      }
     }
    } /* end: if(event) */
   } /* end: no menu */
  }
 while(end<-1);
 return end;
 }

/* turn on/off all messages without buttons that are displayed via w_okcancel.
   If on==1 display the msgs, if 0 then display them not. */
void w_display_msgs(int on)
 { w_killcurrentmessage(); notes.print_msg=(on!=0); }

/* Create a OK/Cancel requester. Question is in txt, text for OK button in ok,
 text for cancel in cancel. Return 1 if ok, 0 if not ok. In txt are upto 30
 '\n' for newlines allowed. cancel==NULL is allowed->only the one button
 is displayed with the ok text. if ok==NULL && cancel==NULL no button is
 displayed and the window is closed when the mouse is moved or the mouse 
 button is pressed or the next window is opened. Of course the return-value
 is then useless.
 help is the online helpstring...(perhaps I will do something like this) */
int w_okcancel(const char *txt,const char *ok,const char *cancel,
 const char *help)
 {
 struct w_window win,*w;
 struct ws_event ws;
 char *pos[30];
 const char *start;
 int numlines,bxsize,i;
 struct w_b_press b_press;
 struct w_button *b_arr[2];
 my_assert(txt!=NULL && (cancel==NULL || ok!=NULL));
 if(!notes.print_msg && ok==NULL && cancel==NULL) return 0;
 bxsize=ws_pixstrlen(ws_pixstrlen(ok)>ws_pixstrlen(cancel) ? ok : cancel)+10;
 i=0; numlines=-1; win.xsize=20+2*bxsize;
 while(++numlines<30 && i<strlen(txt))
  {
  start=strchr(txt+i,'\n');
  if(start==NULL) start=txt+strlen(txt);
  checkmem(pos[numlines]=MALLOC(start-txt-i+1));
  strncpy(pos[numlines],txt+i,start-txt-i);
  pos[numlines][start-txt-i]=0;
  i=start-txt+1;
  if(win.xsize<ws_pixstrlen(pos[numlines])+10) 
   win.xsize=ws_pixstrlen(pos[numlines])+10;
  }
 win.maxxsize=win.xsize;
 win.xpos=(notes.xres-win.xsize)/2;
 win.maxysize=win.ysize=10+(ok!=NULL ? numlines+2 : numlines)*
  shapes.titlebar_height;
 win.ypos=(notes.yres-win.ysize)/1.9; /* this looks better then /2 */
 if(ok==NULL && cancel==NULL)
  {
  ws_getevent(&ws,0);
  ws.x=w_xwinspacecoord(ws.x); ws.y=w_ywinspacecoord(ws.y);
  if(ws.x>=win.xpos-10 && ws.x<=win.xpos+win.xsize+10 &&
   ws.y>=win.ypos-10 && ws.y<=win.ypos+win.ysize+10)
   { win.xpos=ws.x+2; win.ypos=ws.y+2; }
  }
 win.shrunk=0; win.title=NULL; win.help=help;
 win.buttons=0; win.refresh=wr_normal; win.data=NULL;
 win.refresh_routine=NULL; win.click_routine=NULL; win.close_routine=NULL;
 if(msg_win!=NULL) { w_closewindow(msg_win); msg_win=NULL; }
 checkmem(w=w_openwindow(&win));
 for(i=0;i<numlines && pos[i]!=NULL;i++)
  {
  ws_drawtext(w_xwinincoord(w,(w_xwininsize(w)-ws_pixstrlen(pos[i]))/2),
   w_ywinincoord(w,5+i*shapes.titlebar_height),w->xsize,pos[i],
   notes.colindex[cv_textfg],-1);
  FREE(pos[i]);
  }
 if(ok!=NULL)
  {
  b_press.delay=b_press.repeat=0;
  b_press.l_pressed_routine=b_press.r_pressed_routine=b_press.l_routine=
   b_press.r_routine=NULL;
  checkmem(b_arr[0]=w_addstdbutton(w,w_b_press,cancel!=NULL ? 5 :
   (w_xwininsize(w)-ws_pixstrlen(ok)+4)/2,
   w_ywininsize(w)-shapes.titlebar_height-10,bxsize,-1,ok,&b_press,1));
  if(cancel!=NULL)
   checkmem(b_arr[1]=w_addstdbutton(w,w_b_press,w_xwininsize(w)-bxsize-5,
    w_ywininsize(w)-shapes.titlebar_height-10,bxsize,
    -1,cancel,&b_press,1));
  i=w_handleuser(cancel!=NULL ? 2 : 1,b_arr,1,&w,0,NULL,NULL);
  w_closewindow(w); 
  }
 else { msg_win=w; ws_getevent(&ws,0); msg_win_x=ws.x; msg_win_y=ws.y; }
 return i==0;
 }

void w_errormessage(const char *txt,...)
 { 
 char buffer[1024];
 va_list args;
 va_start(args,txt);
 vsprintf(buffer,txt,args);
 w_okcancel(buffer,"Continue",NULL,NULL);
 va_end(args);
 }
