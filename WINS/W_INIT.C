/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_init.c - Initroutines
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
#include "wi_winma.h"
#include "wi_menu.h"
#include "wi_keys.h"
#include "wi_buts.h"
#include "w_draw.h"
#include "w_init.h"

FILE *errf;
struct shapeparameters shapes = { 16,16,16,2 };
struct notebook notes;

/* searches :<m> in File f. Returns number behind the marker and -1 if
   marker was not found. */
int w_findmarker(FILE *f,const char *m)
 {
 char puffer[255],newmarker[200];
 fseek(f,0,SEEK_SET); /* not the smartest solution */
 newmarker[0]=':';
 strcpy(&newmarker[1],m);
 newmarker[strlen(m)+1]=0;
 do
  {
  if(fgets(puffer,255,f)==NULL)
   { printf("Can't find marker :%s\n",m); return -1; }
  }
 while(strncmp(newmarker,puffer,strlen(newmarker)));
 return atoi(&puffer[strlen(newmarker)+1]);
 }

void w_skipline(FILE *f)
 {
 char puffer[255];
 if(fgets(puffer,255,f)==NULL)
  { printf("Can't skip line.\n"); exit(1); }
 if(strlen(puffer)>80)
  { printf("w_skipline: line too long\n"); exit(1); }
 }
 
/* This routine is called to initialize the Window system. You must therefore
   init the grfx & all variables you need. 
   xres,yres and colors are the parameters of the grfx-mode.
   palette are the colors.
   If fontname==NULL, a default font is used.
   Returns zero if not successful.
   If it's not succesful you must remain in Textmode. 
   If successful return a 1 */
int w_initwins(int xres,int yres,int colors,const char *fontname)
 {
 initlist(&notes.windows);
 initlist(&notes.menu);
 notes.print_msg=1;
 notes.xres=xres; notes.yres=yres; notes.colors=colors;
 notes.winspace.xp=notes.winspace.yp=0;
 notes.winspace.xs=xres; notes.winspace.ys=yres; 
 notes.cur_win=NULL; notes.cur_but=NULL; notes.titlebar_text=NULL;
 if((errf=fopen("devil.err","w"))==NULL) errf=stdout;
 if(!ws_initgrfx(xres,yres,colors,fontname)) return 0;
 ws_setcolor(0,0,0,0); 
 notes.colindex[cv_bg]=w_makecolor(0,0,0);
 notes.colindex[cv_winfill]=w_makecolor(0,0,0);
 notes.colindex[cv_textbg]=w_makecolor(0,0,0);
 ws_setcolor(1,150,150,150);
 notes.colindex[cv_winedge]=w_makecolor(150,150,150);
 notes.colindex[cv_buttonin]=w_makecolor(150,150,150);
 ws_setcolor(2,255,255,255);
 notes.colindex[cv_windrag]=w_makecolor(255,255,255);
 ws_setcolor(3,200,200,200);
 notes.colindex[cv_buttonlt]=w_makecolor(200,200,200);
 ws_setcolor(4,100,100,100);
 notes.colindex[cv_buttonrb]=w_makecolor(100,100,100);
 ws_setcolor(5,120,120,120);
 notes.colindex[cv_buttonpressed]=w_makecolor(120,120,120);
 ws_setcolor(6,130,130,130);
 notes.colindex[cv_switchon]=w_makecolor(130,130,130);
 ws_setcolor(7,255,255,255);
 notes.colindex[cv_textfg]=w_makecolor(255,255,255);
 ws_setcolor(8,255,0,0);
 notes.colindex[cv_buttonmarker]=w_makecolor(255,0,0);
 ws_setcolor(9,200,150,50);
 notes.colindex[cv_curwin]=w_makecolor(200,150,50);
 ws_setcolor(10,250,190,65);
 notes.colindex[cv_curwinlt]=w_makecolor(250,190,65);
 ws_setcolor(11,160,120,40);
 notes.colindex[cv_curwinrb]=w_makecolor(160,120,40);
 notes.initialized=1;
 ws_resetmousecolors();
 return 1;
 }

void drawmenubar(void)
 {
 struct node *n;
 char buffer[256];
 ws_drawframedbox(0,0,notes.xres,shapes.titlebar_height+
  shapes.width_menuframe*2+1,shapes.width_menuframe,
  notes.colindex[cv_buttonlt],notes.colindex[cv_buttonrb],
  notes.colindex[cv_buttonin]);
 for(n=notes.menu.head;n->next!=NULL;n=n->next) wi_drawmenutxt(n->d.w_m,0);
 if((notes.cur_win!=NULL && notes.cur_win->w.title!=NULL) ||
  notes.titlebar_text!=NULL)  
  {
  if(notes.titlebar_text)
   { strcpy(buffer,notes.titlebar_text); strcat(buffer,"  / "); }
  else buffer[0]=0;
  strcat(buffer,notes.cur_win->w.title);
  ws_drawtext(notes.xres-ws_pixstrlen(buffer)-shapes.titlebar_height/4,2,
   ws_pixstrlen(buffer),buffer,notes.colindex[cv_textbg],
   notes.colindex[cv_buttonin]);
  ws_drawtext(notes.xres-ws_pixstrlen(buffer)-shapes.titlebar_height/4+1,3,
   ws_pixstrlen(buffer),buffer,notes.colindex[cv_textfg],-1);
  }
 }
 
void set_titlebar_text(char const *txt)
 {
 if(notes.titlebar_text) FREE(notes.titlebar_text);
 checkmem(notes.titlebar_text=MALLOC(strlen(txt)+1));
 strcpy(notes.titlebar_text,txt);
 drawmenubar();
 }

void w_newpalette(unsigned char *palette)
 {
 int i;
 struct node *n,*nb;
 if(!notes.initialized) return;
 for(i=0;i<256;i++) ws_setcolor(i,palette[i*3],palette[i*3+1],palette[i*3+2]);
 ws_resetmousecolors();
 notes.colindex[cv_bg]=w_makecolor(0,0,0);
 notes.colindex[cv_winfill]=w_makecolor(0,0,0);
 notes.colindex[cv_textbg]=w_makecolor(0,0,0);
 notes.colindex[cv_winedge]=w_makecolor(150,150,150);
 notes.colindex[cv_buttonin]=w_makecolor(150,150,150);
 notes.colindex[cv_windrag]=w_makecolor(255,255,255);
 notes.colindex[cv_buttonlt]=w_makecolor(200,200,200);
 notes.colindex[cv_buttonrb]=w_makecolor(100,100,100);
 notes.colindex[cv_buttonpressed]=w_makecolor(120,120,120);
 notes.colindex[cv_switchon]=w_makecolor(130,130,130);
 notes.colindex[cv_textfg]=w_makecolor(255,255,255);
 notes.colindex[cv_buttonmarker]=w_makecolor(255,0,0);
 notes.colindex[cv_curwin]=w_makecolor(200,150,50);
 notes.colindex[cv_curwinlt]=w_makecolor(250,190,65);
 notes.colindex[cv_curwinrb]=w_makecolor(160,120,40);
 for(n=notes.windows.head;n->next!=NULL;n=n->next)
  wi_undrawwin(n->d.w_w);
 ws_drawfilledbox(0,0,notes.xres,notes.yres,0,0); 
 drawmenubar(); 
 for(n=notes.windows.tail;n->prev!=NULL;n=n->prev)
  { 
  wi_drawwin(n->d.w_w); 
  for(nb=n->d.w_w->buttonlist.head;nb->next!=NULL;nb=nb->next)
   if(!nb->d.w_b->sys_button)
    if(!n->d.w_w->w.shrunk && nb->d.w_b->drawn) wi_drawbutton(nb->d.w_b,0);
  }
 } 

void wi_fixwincoords(struct w_window *w)
 {
 if(w->xpos<notes.winspace.xp) w->xpos=notes.winspace.xp;
 if(w->ypos<notes.winspace.yp) w->ypos=notes.winspace.yp;
 if(w->xsize>notes.winspace.xs) w->xsize=notes.winspace.xs;
 if(w->ysize>notes.winspace.ys) w->ysize=notes.winspace.ys;
 }
 
void wi_closeroutine(struct w_button *b)
 { if(b->w->close_routine!=NULL) b->w->close_routine(b->w,b->w->data); }
void wi_shrinkroutine(struct w_button *b)
 { w_shrinkwin(b->w); }
static int ox,oy,dx,dy;
void wi_drag_press(struct w_button *b,int x,int y)
 { 
 if(!(b->w->buttons&wb_drag)) return;
 ws_erasemouse(); ws_mousewarp(b->w->xpos,b->w->ypos);
 ws_drawbox(b->w->xpos,b->w->ypos,b->w->xsize,b->w->ysize,
  notes.colindex[cv_windrag],1); 
 ox=b->w->xpos; oy=b->w->ypos; dx=x-b->w->xpos; dy=y-b->w->ypos;
 }
void wi_drag_drag(struct w_button *b,int x,int y)
 {
 if(x==ox && y==oy) return;
 ws_drawbox(ox,oy,b->w->xsize,b->w->ysize,notes.colindex[cv_windrag],1);
 if(x<notes.winspace.xp) x=notes.winspace.xp;
 else if(x>=notes.winspace.xp+notes.winspace.xs-b->w->xsize)
  x=notes.winspace.xp+notes.winspace.xs-b->w->xsize;
 if(y<notes.winspace.yp) y=notes.winspace.yp;
 else if(y>=notes.winspace.yp+notes.winspace.ys-b->w->ysize)
  y=notes.winspace.yp+notes.winspace.ys-b->w->ysize;
 ws_mousewarp(x,y);
 ws_drawbox(x,y,b->w->xsize,b->w->ysize,notes.colindex[cv_windrag],1);
 ox=x; oy=y;
 }
void wi_drag_rel(struct w_button *b,int x,int y)
 {
 ws_drawbox(ox,oy,b->w->xsize,b->w->ysize,notes.colindex[cv_windrag],1);
 if(ox!=b->w->xpos || oy!=b->w->ypos)
  {
  wi_undrawwin((struct wi_window *)b->w);
  b->w->xpos=ox; b->w->ypos=oy; wi_fixwincoords(b->w);
  wi_drawwin((struct wi_window *)b->w);
  }
 ws_mousewarp(b->w->xpos+dx,b->w->ypos+dy);
 ws_displaymouse();
 }   
void wi_resize_press(struct w_button *b,int x,int y)
 {
 if(!(b->w->buttons&wb_size)) return;
 ws_mousewarp(b->w->xpos+b->w->xsize,b->w->ypos);
 ws_erasemouse();
 ws_drawbox(b->w->xpos,b->w->ypos,b->w->xsize,b->w->ysize,
  notes.colindex[cv_windrag],1); 
 ox=b->w->xpos+b->w->xsize; oy=b->w->ypos; 
 dx=x-b->w->xpos-b->w->xsize; dy=y-b->w->ypos;
 }
void wi_resize_size(struct w_button *b,int x,int y)
 {
 if(x==ox && y==oy) return;
 ws_drawbox(b->w->xpos,oy,ox-b->w->xpos,b->w->ypos+b->w->ysize-oy,
  notes.colindex[cv_windrag],1);
 if(b->w->maxxsize>0)
  if(x-b->w->xpos>b->w->maxxsize) x=b->w->xpos+b->w->maxxsize;
  else if(x-b->w->xpos<-b->w->maxxsize) x=b->w->xpos-b->w->maxxsize;
 if(b->w->maxysize>0)
  if(b->w->ypos+b->w->ysize-y>b->w->maxysize)
   y=b->w->ypos+b->w->ysize-b->w->maxysize;
  else if(y-b->w->ypos-b->w->ysize>b->w->maxysize)
   y=b->w->maxysize+b->w->ypos+b->w->ysize;
 if(x<=notes.winspace.xp) x=notes.winspace.xp+1;
 else if(x>=notes.winspace.xp+notes.winspace.xs)
  x=notes.winspace.xp+notes.winspace.xs-1;
 if(y<notes.winspace.yp) y=notes.winspace.yp;
 else if(y>=notes.winspace.yp+notes.winspace.ys)
  y=notes.winspace.yp+notes.winspace.ys-1;
 ws_drawbox(b->w->xpos,y,x-b->w->xpos,b->w->ypos+b->w->ysize-y,
  notes.colindex[cv_windrag],1);
 ox=x; oy=y; ws_mousewarp(x,y);
 }
/* resizes the window of button b which is the dragbutton. In the window 
   structure are the new values for xsize,ysize */
void w_resizewin(struct w_button *b)
 {
 struct node *n;
 my_assert(b!=NULL && b->w!=NULL);
 wi_fixwincoords(b->w);
 if(b->w->ysize<shapes.titlebar_height)
  b->w->ysize=shapes.titlebar_height;
 if(b->w->xsize<ws_pixstrlen(b->w->title)+shapes.closebutton_width+
  shapes.sizebutton_width+4)
  b->w->xsize=ws_pixstrlen(b->w->title)+shapes.closebutton_width+
   shapes.sizebutton_width+4;
 if(b->w->ypos+b->w->ysize>notes.winspace.yp+notes.winspace.ys)
  b->w->ypos=notes.winspace.yp+notes.winspace.ys-b->w->ysize;
 if(b->w->xpos+b->w->xsize>notes.winspace.xp+notes.winspace.xs)
  b->w->xpos=notes.winspace.xp+notes.winspace.xs-b->w->xsize;
 if(b->w->ypos<notes.winspace.yp) b->w->ypos=notes.winspace.yp;
 if(b->w->xpos<notes.winspace.xp) b->w->xpos=notes.winspace.xp;
 ((struct wi_window *)(b->w))->oysize=b->w->ysize;
 b->xsize=b->w->xsize;
 if(b->w->buttons&(wb_close|wb_shrink)) b->xsize-=shapes.closebutton_width;
 if((n=b->inscription.head)!=NULL)
  n->d.w_d->x=(b->xsize-ws_pixstrlen(n->d.w_d->contents.txt))/2-1;
 if((n=b->a_inscription.head)!=NULL)
  n->d.w_d->x=(b->xsize-ws_pixstrlen(n->d.w_d->contents.txt))/2-1;
 }
void wi_resize_rel(struct w_button *b,int x,int y)
 {
 int p;
 ws_drawbox(b->w->xpos,oy,ox-b->w->xpos,b->w->ypos+b->w->ysize-oy,
  notes.colindex[cv_windrag],1);
 wi_undrawwin((struct wi_window *)b->w);
 if(x>b->w->xpos) { b->w->xsize=x-b->w->xpos; }
 else { b->w->xpos=x; b->w->xsize=b->w->xpos-x; }
 if(y<b->w->ypos+b->w->ysize) 
  { b->w->ysize=b->w->ypos+b->w->ysize-y; b->w->ypos=y; }
 else 
  { p=b->w->ysize; b->w->ysize=y-b->w->ypos-b->w->ysize; 
    b->w->ypos=b->w->ypos+p; }
 w_resizewin(b);
 wi_drawwin((struct wi_window *)b->w);
 ws_mousewarp(b->w->xpos+b->w->xsize+dx,b->w->ypos+dy);
 ws_displaymouse();
 } 
 
/* Opens a window with the parameters from w (which is not the same pointer
   as the returned value!). The enums are defined in wins.h.
   Open window means to initialize the window structure
   and bring the window on the current screen. If this is not possible,
   the routine returns NULL. If maxxsize<xsize && maxxsize>0 then
   maxxsize=xsize. Same for maxysize. title==NULL is possible. 
   If xpos<0 or ypos<0 this position will be centered. */
struct w_window *w_openwindow(struct w_window *w)
 {
 struct wi_window *wi;
 struct wi_button *b;
 struct w_b_press *wi_p_closebutton;
 struct w_b_drag *wi_d_sizebutton;
 int x;
 my_assert(w!=NULL);
 my_assert(!(w->refresh==wr_routine && w->refresh_routine==NULL));
 my_assert(w->xsize>0 && w->ysize>0 && w->xsize<=notes.winspace.xs
  && w->ysize<=notes.winspace.ys);
 if(w->xpos+w->xsize>=notes.winspace.xs) w->xpos=notes.winspace.xs-w->xsize-1;
 if(w->ypos+w->ysize>=notes.winspace.ys) w->ypos=notes.winspace.ys-w->ysize-1;
 checkmem(wi=MALLOC(sizeof(struct wi_window)));
 wi->w=*w; 
 if(wi->w.xpos<0) wi->w.xpos=(notes.winspace.xs-wi->w.xsize)/2;
 if(wi->w.ypos<0) wi->w.ypos=(notes.winspace.ys-wi->w.ysize)/2;
 wi->w.xpos+=notes.winspace.xp; wi->w.ypos+=notes.winspace.yp;
 if(wi->w.maxxsize<wi->w.xsize&&wi->w.maxxsize>0) wi->w.maxxsize=wi->w.xsize;
 if(wi->w.maxysize<wi->w.ysize&&wi->w.maxysize>0) wi->w.maxysize=wi->w.ysize;
 if(w->title) 
  { checkmem(wi->w.title=MALLOC(strlen(w->title)+1));
    strcpy((char *)wi->w.title,w->title); }
 else w->title=NULL;
 wi->bg=wi->in=NULL; wi->drawn=0; wi->in_refresh=0;
 initlist(&wi->buttonlist);
 checkmem(wi->n=addheadnode(&notes.windows,-1,wi));
 wi->w.no=wi->n->no;
 wi->oysize = wi->iysize = wi->w.ysize;
 wi->ixsize = wi->w.xsize;
 wi->ixpos = wi->w.xpos; wi->iypos = wi->w.ypos;
 if(wi->w.shrunk) wi->w.ysize=shapes.titlebar_height;
 if(wi->w.buttons&(wb_close|wb_shrink))
  {
  checkmem(wi_p_closebutton=MALLOC(sizeof(struct w_b_press)));
  wi_p_closebutton->l_pressed_routine=
   wi_p_closebutton->r_pressed_routine=NULL;
  wi_p_closebutton->l_routine=wi->w.buttons&wb_close ? wi_closeroutine : NULL;
  wi_p_closebutton->r_routine=wi->w.buttons&wb_shrink ? wi_shrinkroutine:NULL;
  checkmem(b=(struct wi_button *)w_addstdbutton(&wi->w,w_b_press,
   w_xscreencoord(&wi->w,wi->w.xpos),w_yscreencoord(&wi->w,wi->w.ypos),
   shapes.closebutton_width,shapes.titlebar_height,NULL,wi_p_closebutton,0))
  b->sys_button=1; b->drawn=1; /* otherwise it is not drawn */
  b->b.data=(void *)0;
  x=b->b.xsize;
  }
 else x=0;
 if(wi->w.buttons&(wb_drag|wb_size))
  {
  checkmem(wi_d_sizebutton=MALLOC(sizeof(struct w_b_drag)));
  wi_d_sizebutton->l_pressroutine=wi->w.buttons&wb_drag ? wi_drag_press:NULL;
  wi_d_sizebutton->l_dragroutine=wi->w.buttons&wb_drag ? wi_drag_drag : NULL;
  wi_d_sizebutton->l_relroutine=wi->w.buttons&wb_drag ? wi_drag_rel : NULL;
  wi_d_sizebutton->r_pressroutine=wi->w.buttons&wb_size ?wi_resize_press:NULL;
  wi_d_sizebutton->r_dragroutine=wi->w.buttons&wb_size ? wi_resize_size:NULL;
  wi_d_sizebutton->r_relroutine=wi->w.buttons&wb_size ? wi_resize_rel : NULL; 
  checkmem(b=(struct wi_button *)w_addstdbutton(&wi->w,w_b_drag,
   w_xscreencoord(&wi->w,wi->w.xpos)+x,w_yscreencoord(&wi->w,wi->w.ypos),
   wi->w.xsize-x,shapes.titlebar_height,wi->w.title,wi_d_sizebutton,0))
  b->sys_button=1; b->drawn=1; /* otherwise it is not drawn */
  b->b.data=(void *)1;
  }
 wi_drawwin(wi);
 return &wi->w;
 }
 
/* if a button is created before the window is made, this routine
   can put it into the window. */
void w_buttoninwin(struct w_window *w,struct w_button *b,int draw)
 {
 struct wi_button *bi=(struct wi_button *)b;
 my_assert(b!=NULL && w!=NULL && b->w==NULL);
 b->w=w; bi->drawn=draw;
 if(draw) my_assert(wi_drawbutton(bi,0));
 checkmem(bi->n=addnode(&((struct wi_window *)bi->b.w)->buttonlist,-1,bi));
 bi->b.no=bi->n->no;
 }
 
void w_dontchangebutton(struct w_button *b)
 { struct wi_button *bi=(struct wi_button *)b; bi->tobechanged=0; }
 
struct w_button *w_addbutton(struct w_button *b,int draw)
 {
 struct wi_button *bi;
 my_assert(b!=NULL && b->d.d!=NULL && (b->w!=NULL || !draw));
 checkmem(bi=MALLOC(sizeof(struct wi_button)));
 bi->b=*b;
 copylisthead(&bi->b.inscription,&b->inscription);
 copylisthead(&bi->b.a_inscription,&b->a_inscription);
 bi->sys_button=0; bi->activated=1; bi->bm=NULL; bi->drawn=draw;
 bi->tobechanged=1;
 if(b->w)
  {
  if(draw && !wi_drawbutton(bi,0)) return NULL;
  checkmem(bi->n=addnode(&((struct wi_window *)bi->b.w)->buttonlist,-1,bi));
  bi->b.no=bi->n->no;
  }
 else bi->b.no=-1;
 return &bi->b;
 }
 
/* Draws a standard button with text. data is the substructure of the button. 
   Returns pointer to button-structure if successful, 0 otherwise. */
struct w_button *w_addstdbutton(struct w_window *w,enum but_types type,
 int xpos,int ypos,int xsize,int ysize,const char *txt,void *data,int draw)
 {
 struct w_button *b,*nb;
 struct w_description *d;
 int x,y,l;
 if(ysize>0 && ysize<shapes.titlebar_height && txt!=NULL) return NULL;
 checkmem(b=MALLOC(sizeof(struct w_button)));
 b->xpos=xpos; b->ypos=ypos; b->xsize=xsize; b->ysize=ysize; b->w=w;
 b->type=type; b->helptxt=NULL;
 initlist(&b->inscription); initlist(&b->a_inscription);
 b->d.d=data;
 switch(type)
  {
  case w_b_press: case w_b_drag: 
   if(b->xsize<0)
    b->xsize=txt!=NULL ? ws_pixstrlen(txt)+4 : shapes.closebutton_width;
   if(b->ysize<0) b->ysize=shapes.titlebar_height; 
   break;
  case w_b_switch: case w_b_tag:
   if(b->xsize<0)
    b->xsize=txt!=NULL ? ws_pixstrlen(txt)+6 : shapes.closebutton_width;
   if(b->ysize<0) b->ysize=shapes.titlebar_height+4; 
   break;
  case w_b_choose: 
   b->d.ls->d_xsize=txt==NULL ? 2 : ws_pixstrlen(txt)+4; 
   if(b->xsize<0)
    {
    for(x=0;x<b->d.ls->num_options;x++)
     if(b->xsize<ws_pixstrlen(b->d.ls->options[x]))
      b->xsize=ws_pixstrlen(b->d.ls->options[x]);
    b->xsize+=b->d.ls->d_xsize+10;
    }
   if(b->ysize<0) b->ysize=shapes.titlebar_height+5;
   break;
  case w_b_string:
   b->d.str->d_xsize=txt==NULL ? 2 : ws_pixstrlen(txt)+4;
   b->d.str->offset=0; 
   if(b->xsize<0)
    b->xsize=b->d.str->max_length*ws_pixstrlen("M")+b->d.str->d_xsize+10;
   if(b->ysize<0) b->ysize=shapes.titlebar_height+5;
   b->d.str->length=(b->xsize-b->d.str->d_xsize-10)/ws_pixstrlen("M");
   break;
  case w_b_list:
   if(b->xsize<0)
    for(x=0;x<b->d.sls->no_strings;x++)
     if(b->xsize<ws_pixstrlen(b->d.sls->strings[x]))
      b->xsize=ws_pixstrlen(b->d.sls->strings[x]);
   if(b->ysize<0) b->ysize=(shapes.titlebar_height+2)*5+4;
   break;
  default: return NULL;
  }
 if(txt!=NULL)
  {
  switch(type)
   {
   case w_b_press: case w_b_drag: case w_b_switch: case w_b_tag:
    if(ws_pixstrlen(txt)+4<=b->xsize)
     { x=(b->xsize-ws_pixstrlen(txt))/2-1; l=strlen(txt); }
    else
     { x=2; l=ws_charstrlen(b->xsize-4); }
    y=(b->ysize-shapes.titlebar_height)/2; 
    break;
   case w_b_choose:
    if(ws_pixstrlen(txt)+4<=b->d.ls->d_xsize)
     { x=(b->d.ls->d_xsize-ws_pixstrlen(txt))/2; l=strlen(txt); }
    else
     { x=3; l=ws_charstrlen(b->d.ls->d_xsize-4); }
    y=(b->ysize-shapes.titlebar_height)/2;
    break;
   case w_b_string:
    if(ws_pixstrlen(txt)+4<=b->d.str->d_xsize)
     { x=(b->d.str->d_xsize-ws_pixstrlen(txt))/2; l=strlen(txt); }
    else
     { x=3; l=b->d.str->d_xsize<4 ? 0 : ws_charstrlen(b->d.str->d_xsize-4); }
    y=(b->ysize-shapes.titlebar_height)/2;
    break;
   default: return NULL;
   }
  if(l>0)
   {
   checkmem(d=MALLOC(sizeof(struct w_description)));
   checkmem(d->contents.txt=MALLOC(l+1));
   strncpy(d->contents.txt,txt,l); d->contents.txt[l]=0;
   d->type=w_bdt_txt; d->x=x; d->y=y;
   checkmem(addnode(&b->inscription,-1,d));
   checkmem(addnode(&b->a_inscription,-1,d));
   }
  }
 nb=w_addbutton(b,draw);
 free(b);
 return nb;
 }
 
/* Draws a button with an image. data is the substructure of
   the button. bm is the pointer to the normal image. abm the pointer to the
   image for the activated button. The color 0 in the images is replaced
   with the correct background color for the button. The button is as large
   as the image plus the edge of the button if the xsize is set to -1.
   If the button is a string button ysize may be set to -1 to get the
   stdsize for stringbuttons. If the buttons is not a string button and
   xsize!=-1 then ysize must be set as well.
   Therefore both images must be of equal size. The position is the position 
   of the left upper edge of the button (not the image).
   Returns pointer to button-structure if successful, 0 otherwise. */
struct w_button *w_addimagebutton(struct w_window *w,enum but_types type,
 int xpos,int ypos,int xsize,int ysize,struct ws_bitmap *bm,
 struct ws_bitmap *abm,void *data,int with_bg,int draw)
 {
 struct w_button *b,*nb;
 struct w_description *d1,*d2;
 if(abm==NULL || bm==NULL || abm->xsize!=bm->xsize || abm->ysize!=bm->ysize) 
  return NULL;
 checkmem(b=MALLOC(sizeof(struct w_button)));
 b->xpos=xpos; b->ypos=ypos; b->w=w;
 if(xsize<0)
  { b->xsize=bm->xsize+(type==w_b_switch ? 6 : 2);
    b->ysize=bm->ysize+(type==w_b_switch ? 6 : 2); }
 else
  { b->xsize=xsize; b->ysize=ysize<0 ? shapes.titlebar_height+5 : ysize; }
 b->type=type; b->helptxt=NULL;
 initlist(&b->inscription); initlist(&b->a_inscription);
 b->d.d=data;
 checkmem(d1=MALLOC(sizeof(struct w_description)));
 d1->contents.bm=bm;
 d1->type=w_bdt_image; d1->image_with_bg=with_bg;
 d1->y=d1->x=type==w_b_switch ? 3 : 1;
 if(type==w_b_string) 
  { d1->y=(b->ysize-bm->ysize)/2; b->d.str->d_xsize=bm->xsize+2;
    b->d.str->offset=0; 
    b->d.str->length=(b->xsize-b->d.str->d_xsize-10)/ws_pixstrlen("M"); }
 checkmem(addnode(&b->inscription,-1,d1));
 checkmem(d2=MALLOC(sizeof(struct w_description)));
 *d2=*d1; d2->contents.bm=abm;
 checkmem(addnode(&b->a_inscription,-1,d2));
 nb=w_addbutton(b,draw);
 free(b);
 return nb;
 }

void w_deactivatebutton(struct w_button *b)
 {
 struct wi_button *bi=(struct wi_button *)b;
 bi->activated=0;
 if(bi->drawn) wi_drawbutton(bi,0);
 }
 
void w_activatebutton(struct w_button *b)
 {
 struct wi_button *bi=(struct wi_button *)b;
 bi->activated=1;
 if(bi->drawn) wi_drawbutton(bi,0);
 }
 
void w_drawbutton(struct w_button *b)
 {
 struct wi_button *bi=(struct wi_button *)b;
 bi->drawn=1; wi_drawbutton(bi,0);
 }
 
void w_undrawbutton(struct w_button *b)
 {
 struct wi_button *bi=(struct wi_button *)b;
 bi->drawn=0; wi_undrawbutton(bi);
 }

void w_setdrawbutton(struct w_button *b,int drawn)
 { ((struct wi_button *)b)->drawn=0; }
 
/* Delete button b (this routine doesn't undraw the button, the button is
   already undrawn) */
void w_deletebutton(struct w_button *b)
 {
 struct wi_button *bi=(struct wi_button *)b;
 struct node *n;
 my_assert(bi!=NULL);
 if(bi->n!=NULL) killnode(&((struct wi_window *)b->w)->buttonlist,bi->n);
 for(n=b->inscription.head->next;n!=NULL;n=n->next)
  killnode(&b->inscription,n->prev);
 for(n=b->a_inscription.head->next;n!=NULL;n=n->next)
  killnode(&b->a_inscription,n->prev);
 FREE(bi);
 }
 
/* Close window w and free the structure. */
void w_closewindow(struct w_window *w)
 {
 struct wi_window *wi=(struct wi_window *)w;
 struct node *n;
 if(w==NULL) return;
 if(notes.cur_but && notes.cur_but->d.w_b->b.w==w) notes.cur_but=NULL;
 if(notes.cur_win && &notes.cur_win->w==w) notes.cur_win=NULL;
 w_wintofront(w);
 wi_undrawwin(wi);
 for(n=wi->buttonlist.head->next;n!=NULL;n=n->next)
  {
  if(n->prev->d.w_b->sys_button) FREE(n->prev->d.w_b->b.d.d);
  w_deletebutton(&n->prev->d.w_b->b);
  }
 if(wi->n) killnode(&notes.windows,wi->n);
 if(wi->in) ws_freebitmap(wi->in);
 if(wi->bg) ws_freebitmap(wi->bg);
 free(wi);
 }
 
void w_closewins(void)
 {
 struct node *n;
 if(notes.initialized)
  {
  for(n=notes.windows.head->next;n!=NULL;n=n->next)
   w_closewindow((struct w_window *)n->prev->d.w_w); 
  killlist(&notes.windows);
  fclose(errf);
  }
 ws_textmode();
 }

/* recursive function for reading menu-data. See w_initmenu for details.
   xoffset and yoffset are the starting point of the current menue. */
int wi_readmenu(FILE *mfile,int no,void (**action)(int),int max_anr,
 int xoffset,int yoffset,int mainline,struct list *l,int level)
 {
 int i,lastpos=0;
 char buffer[1024],helptxt[1024],*pos;
 struct wi_menu *m;
 struct node *n;
 for(i=0;i<no;i++)
  {
  checkmem(m=MALLOC(sizeof(struct wi_menu)));
  if(fscanf(mfile," %i%*[^{]%1023[^}]%*[^{]%1023[^}]%*[^\n]",&m->actionnr,
   buffer,helptxt)!=3) 
   { fprintf(errf,"Can't read menu-data %d/%d/%d.\n",i,no,level); return 0; }
  if((pos=strchr(buffer,'>'))==NULL)
   { fprintf(errf,"Can't find hotkey in %s.\n",buffer); return 0; }
  checkmem(m->txt=MALLOC(strlen(buffer)+(!mainline && m->actionnr<0 ?
   1 : -1)));
  checkmem(m->help=MALLOC(strlen(helptxt)));
  *pos=0;
  strcpy(m->txt,buffer+1); strcat(m->txt,pos+1); strcpy(m->help,helptxt+1);
  m->hotkey=level==0 ? toupper(*(pos+1)) : *(pos+1);
  m->hklinex=ws_pixstrlen(buffer+1);
  *(pos+2)=0;
  m->hklinexs=ws_pixstrlen(pos+1)-1;
  m->hkliney=shapes.titlebar_height-3;
  initlist(&m->submenus); m->bm=NULL;
  checkmem(addnode(l,-1,m));
  m->level=level;
  if(mainline)
   {
   m->x1=xoffset+lastpos; m->y1=yoffset; 
   m->xs=ws_pixstrlen(m->txt)+shapes.titlebar_height/2;
   m->ys=shapes.titlebar_height;
   lastpos+=m->xs;
   }
  else
   { 
   if(m->actionnr<0) strcat(m->txt," >");
   m->ys=shapes.titlebar_height; m->x1=xoffset; 
   m->y1=yoffset+i*m->ys+2*shapes.width_menuframe; 
   if(m->y1+m->ys>notes.yres)
    { fprintf(errf,"Submenu too large: txt=%s\n",m->txt); return 0; }
   if(ws_pixstrlen(m->txt)+shapes.titlebar_height/2>lastpos) 
    lastpos=ws_pixstrlen(m->txt)+shapes.titlebar_height/2;
   }
  if(m->actionnr>=0)
   {
   if(m->actionnr>max_anr) 
    {fprintf(errf,"Action number too large: %d > %d\n",m->actionnr,max_anr);
     return 0;}
   m->action=action[m->actionnr];
   }
  else
   {
   if(!wi_readmenu(mfile,-m->actionnr,action,max_anr,mainline ? m->x1 :
    m->x1+ws_pixstrlen(m->txt)+1,mainline ? m->y1+shapes.titlebar_height+
    shapes.width_menuframe+1 : m->y1,0,&m->submenus,level+1)) 
    { fprintf(errf,"Can't read submenu for %d\n",i); return 0; }
   m->action=NULL; m->actionnr=-1;
   }
  }
 if(!mainline) for(n=l->head;n->next!=NULL;n=n->next) n->d.w_m->xs=lastpos+2;
 return 1;
 }
 
void wi_adjustmenus(struct list *l,int xoffset)
 {
 struct node *n;
 for(n=l->head;n->next!=NULL;n=n->next)
  { n->d.w_m->x1=xoffset; 
    wi_adjustmenus(&n->d.w_m->submenus,xoffset+n->d.w_m->xs+
     2*shapes.width_menuframe+1); }
 }

/* This routine draws the menuline at the top of the screen. Returns 1
   if successful, 0 otherwise. mfile is a file with the
   list of menupoints, action is a functionarray with max_anr+1 entries. 
   The format of the file is the following:
   :MENU <number of entries in menuline>
    <no> {<Name>}
    where 
     no: if >0 then number of actionfunction
         if <0 then number of submenupoints
	 is read with %i, therefore normal C-integer-constant.
     name: name of menupoint. > marks the Hotkey of this menupoint */
int w_initmenu(FILE *mfile,void (**action)(int),int max_anr)
 {
 struct node *n;
 int no;
 my_assert(mfile!=NULL);
 no=w_findmarker(mfile,"MENU");
 if(no<0) return 0;
 if(!wi_readmenu(mfile,no,action,max_anr,shapes.width_menuframe*2,
  shapes.width_menuframe,1,&notes.menu,0)) return 0;
 for(n=notes.menu.head;n->next!=NULL;n=n->next)
  wi_adjustmenus(&n->d.w_m->submenus,n->d.w_m->x1);
 drawmenubar();
 notes.winspace.yp+=shapes.titlebar_height+shapes.width_menuframe*2;
 notes.winspace.ys-=shapes.titlebar_height+shapes.width_menuframe*2;
 return 1;
 }

void w_setcolors(int *palette)
 {
 int i;
 for(i=0;i<256;i++)
  ws_setcolor(i,palette[i*3],palette[i*3+1],palette[i*3+2]);
 }

/* Creates a bitmap xsize*ysize and draws the lines into it. The lines
   are an array of n lines which consist of a color (1 int) and 2 coords
   (4 ints) */ 
struct ws_bitmap *w_createlinesbm(int xsize,int ysize,int n,int *lines)
 {
 struct ws_bitmap *bm;
 int i;
 my_assert(lines);
 checkmem(bm=ws_createbitmap(xsize,ysize,NULL));
 for(i=0;i<n;i++)
  ws_bmdrawline(bm,lines[i*5+1],lines[i*5+2],lines[i*5+3],lines[i*5+4],
   lines[i*5],0);
 return bm;
 }

void w_shrinkwin(struct w_window *w)
 {
 my_assert(w!=NULL);
 w_wintofront(w);
 wi_undrawwin((struct wi_window *)w);
 w->shrunk^=1;
 if(w->shrunk && notes.cur_but && notes.cur_but->d.w_b->b.w==w)
  notes.cur_but=NULL;
 w->ysize = w->shrunk ? shapes.titlebar_height : 
  ((struct wi_window *)w)->oysize;
 if(w->ypos+w->ysize>notes.winspace.yp+notes.winspace.ys)
  w->ypos=notes.winspace.yp+notes.winspace.ys-w->ysize;
 wi_drawwin((struct wi_window *)w);
 }

void w_resizeinwin(struct w_window *w,int x,int y)
 {
 struct wi_window *wi=(struct wi_window *)w;
 struct node *n;
 struct w_button *b;
 my_assert(w!=NULL && (w->buttons&(wb_drag|wb_size))!=0);
 w_wintofront(&wi->w);
 wi_undrawwin(wi);
 w->shrunk=0;
 w_winoutsize(w,x,y);
 for(n=wi->buttonlist.head,b=NULL;n->next!=NULL && n->d.w_b->sys_button;
  n=n->next)
  if((int)n->d.w_b->b.data==1) { b=&n->d.w_b->b; break; }
 my_assert(b!=NULL); w_resizewin(b);
 wi_drawwin(wi);
 }

/* Make window w the current window. NULL is allowed */
void w_curwin(struct w_window *w) 
 {
 struct w_window *ow;
 ow = notes.cur_win!=NULL ? &notes.cur_win->w : NULL;
 notes.cur_win=(struct wi_window *)w;
 drawmenubar(); 
 if(ow) w_refreshwin(ow);
 if(w) w_refreshwin(w);
 }

/* test if win w is the current window */
int w_isthiscurwin(struct w_window *w)
 { return w==&notes.cur_win->w; }
 
/* Set the window with the current button */
void w_butwin(struct w_window *w)
 {
 struct wi_window *wi=(struct wi_window *)w;
 struct node *n;
 wi_changecurbutton(NULL); /* turn the old button off */
 if(wi==NULL || wi->buttonlist.size<=0) n=NULL;
 else
  {
  w_wintofront(w); if(w->shrunk) w_shrinkwin(w); 
  n=wi->buttonlist.head;
  while(n->next!=NULL && (!n->d.w_b->drawn || !n->d.w_b->activated ||
   n->d.w_b->sys_button || wi_butkeyhandling[n->d.w_b->b.type]==NULL))
   n=n->next;
  if(n->next==NULL) n=NULL;
  }
 wi_changecurbutton(n);
 }

/* save position & size of windows with names for shutdown */
int w_savecfg(FILE *f)
 {
 struct node *n;
 int i;
 for(n=notes.windows.head,i=0;n->next!=NULL;n=n->next)
  if(n->d.w_w->w.title) i++;
 fprintf(f,"%d\n",i);
 for(n=notes.windows.tail;n->prev!=NULL;n=n->prev)
  if(n->d.w_w->w.title)
   if(fprintf(f,"%s\n%d %d %d %d %d %d\n",n->d.w_w->w.title,n->d.w_w->w.xpos,
    n->d.w_w->w.ypos,n->d.w_w->w.xsize,n->d.w_w->w.ysize,n->d.w_w->oysize,
    n->d.w_w->w.shrunk)<0) return 0;
 return 1;
 }
 
/* restore position & size of windows with names (which must be open). */
void w_readcfg(FILE *f)
 {
 struct node *n,*n2;
 int i,j,xp,yp,xs,ys,s,oys;
 char buffer[256];
 struct wi_button *b;
 if(fscanf(f,"\n%d\n",&i)!=1) return;
 for(j=0;j<i;j++)
  {
  if(!fgets(buffer,255,f)) return;
  buffer[255]=0; buffer[strlen(buffer)-1]=0; /* eliminate the \n */
  for(n=notes.windows.tail;n->prev!=NULL;n=n->prev)
   if(n->d.w_w->w.title && strcmp(n->d.w_w->w.title,buffer)==0) break;
  if(fscanf(f," %d%d%d%d%d%d\n",&xp,&yp,&xs,&ys,&oys,&s)!=6) return;
  if(n->prev)
   {
   w_wintofront(&n->d.w_w->w);
   wi_undrawwin(n->d.w_w);
   n->d.w_w->w.xpos=xp; n->d.w_w->w.ypos=yp; 
   n->d.w_w->w.xsize=xs; n->d.w_w->w.ysize=ys; 
   n->d.w_w->w.shrunk=s; n->d.w_w->oysize=oys;
   for(n2=n->d.w_w->buttonlist.head,b=NULL;n2->next!=NULL && 
    n2->d.w_b->sys_button;n2=n2->next)
    if((int)n2->d.w_b->b.data==1) { b=n2->d.w_b; break; }
   if(b!=NULL) w_resizewin(&b->b); 
   n->d.w_w->oysize=oys; /* is overwritten in resizewin */
   wi_drawwin(n->d.w_w);
   }
  }
 }

/* cursor is activated if  event.kbstat&bitmask==kbstat. w=width, h=height,
 xo,yo=hot spot, colortable maps colors from data to normal colors. */
void w_initkbstatcursor(char *data,int w,int h,int xo,int yo,
 long *colortable,enum kbstattypes kbstat,enum kbstattypes bitmask)
 {
 int i;
 struct kbstatcursor *kbsc;
 my_assert((kbstat<2<<MAX_KBSTAT) && data!=NULL);
 for(i=0,kbsc=NULL;i<notes.num_kbstatcursors;i++)
  if(notes.kbstatcursors[i].kbstat==kbstat && 
   notes.kbstatcursors[i].bitmask==bitmask) kbsc=&notes.kbstatcursors[i];
 if(!kbsc)
  { checkmem(notes.kbstatcursors=REALLOC(notes.kbstatcursors,
     ++notes.num_kbstatcursors*sizeof(struct kbstatcursor)));
    kbsc=&notes.kbstatcursors[notes.num_kbstatcursors-1];
    kbsc->kbstat=kbstat; kbsc->bitmask=bitmask; }
 else ws_killcursor(kbsc->cursor);
 checkmem(kbsc->cursor=ws_initcursor(data,w,h,xo,yo,colortable));
 }
