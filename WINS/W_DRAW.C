/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_draw.c - Drawing routines
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
#include "wi_winma.h"
#include "w_tools.h"
#include "w_init.h"
#include "w_draw.h"

void w_clearwindow(struct w_window *w)
 {
 struct wi_window *wi=(struct wi_window *)w;
 my_assert(wi && wi->in_refresh);
 if(wi->in) ws_clearbitmap(wi->in,notes.colindex[cv_winfill]);
 if(wi->win_contents)
  ws_drawfilledbox(w_xwinincoord(w,0),w_ywinincoord(w,0),w_xwininsize(w),
   w_ywininsize(w),notes.colindex[cv_bg],0);
 }
 
void w_drawline(struct w_window *w,int x1,int y1,int x2,int y2,int col,
 int xor)
 {
 my_assert(w);
 if(((struct wi_window *)w)->win_contents)
  ws_drawline(w_xwinincoord(w,x1),w_ywinincoord(w,y1),w_xwinincoord(w,x2),
   w_ywinincoord(w,y2),col,xor);
 }
 
void w_drawclipline(struct w_window *w,int x1,int y1,int x2,int y2,int col,
 int xor)
 {
 my_assert(w);
 ws_setclipping(w_xwinincoord(w,0),w_ywinincoord(w,0),
  w_xwinincoord(w,w_xwininsize(w)-1),w_ywinincoord(w,w_ywininsize(w)-1));
 if(((struct wi_window *)w)->win_contents)
  ws_drawline(w_xwinincoord(w,x1),w_ywinincoord(w,y1),w_xwinincoord(w,x2),
   w_ywinincoord(w,y2),col,xor);
 ws_setclipping(-1,-1,-1,-1);
 }
 
void w_drawbitmap(struct w_window *w,int xp,int yp,int xs,int ys,
 struct ws_bitmap *bm,int withbg)
 { 
 my_assert(w!=NULL && bm!=NULL);
 if(w->shrunk) return;
 if(xp+xs>w_xwininsize(w)) xs=w_xwininsize(w)-xp;
 if(yp+ys>w_ywininsize(w)) ys=w_ywininsize(w)-yp;
 ws_copybitmap(NULL,w_xwinincoord(w,xp),w_ywinincoord(w,yp),bm,0,0,xs,ys,
  withbg); 
 }

/* Draw text with standard colors (like a button) to w,x,y. Max. pixel width
   of text is width. */
void w_drawouttext(struct w_window *w,int x,int y,int width,char *txt)
 {
 ws_drawfilledbox(w_xwinincoord(w,x),w_ywinincoord(w,y),width,
  w_titlebarheight(),notes.colindex[cv_buttonin],0);
 ws_drawtext(w_xwinincoord(w,x)+2,w_ywinincoord(w,y)+2,width,txt,
  notes.colindex[cv_textbg],-1);
 ws_drawtext(w_xwinincoord(w,x)+1,w_ywinincoord(w,y)+1,width,txt,
  notes.colindex[cv_textfg],-1);
 }

void w_drawbuttonbox(struct w_window *w,int x,int y,int xs,int ys)
 { ws_drawfilledbox(w_xwinincoord(w,x),w_ywinincoord(w,y),xs,ys,
    notes.colindex[cv_buttonin],0); }
    
/* replace the part on the screen with the corresponding contents of the
 bg-bitmap of the window and vice versa */
static void wi_swapbg(struct wi_window *w,struct bounds *b)
 {
 struct ws_bitmap *bm;
 my_assert(w->bg!=NULL);
 checkmem(bm=ws_savebitmap(NULL,b->xp,b->yp,b->xs,b->ys));
 ws_copybitmap(NULL,b->xp,b->yp,w->bg,b->xp-w->w.xpos,
  b->yp-w->w.ypos,b->xs,b->ys,1);
 ws_copybitmap(w->bg,b->xp-w->w.xpos,b->yp-w->w.ypos,bm,0,0,b->xs,b->ys,1);
 ws_freebitmap(bm);
 }
   
/* Bring window wi to front. */
void w_wintofront(struct w_window *w)
 {
 struct wi_window *wi=(struct wi_window *)w;
 struct node *n;
 struct bounds b;
 struct ws_bitmap *bm;
 struct list l;
 my_assert(wi!=NULL && wi->n!=NULL);
 if(wi->in_refresh) w_refreshend(&wi->w);
 if(wi->n->prev->prev==NULL) return;
 if(wi->drawn)
  {
  initlist(&l);
  for(n=notes.windows.head;n->next!=NULL && n!=wi->n;n=n->next)
   if(n->d.w_w->drawn && wi_whereoverlap(n->d.w_w,wi,&b))
    {
    checkmem(bm=ws_savebitmap(NULL,b.xp,b.yp,b.xs,b.ys));
    bm->w=n->d.w_w;
    checkmem(addnode(&l,-1,bm));
    ws_copybitmap(NULL,b.xp,b.yp,n->d.w_w->bg,b.xp-n->d.w_w->w.xpos,
     b.yp-n->d.w_w->w.ypos,b.xs,b.ys,1);     
    }
  for(n=l.tail->prev;n!=NULL;n=n->prev)
   {
   bm=n->next->d.w_bm;
   ws_copybitmap(bm->w->bg,bm->xpos-bm->w->w.xpos,bm->ypos-bm->w->w.ypos,
    wi->bg,bm->xpos-w->xpos,bm->ypos-w->ypos,bm->xsize,bm->ysize,1);
   ws_copybitmap(wi->bg,bm->xpos-w->xpos,bm->ypos-w->ypos,bm,0,0,bm->xsize,
    bm->ysize,1);
   ws_freebitmap(bm);
   FREE(n->next);
   }
  }
 unlistnode(&notes.windows,wi->n); 
 wi->n->prev=notes.windows.head->prev;
 wi->n->next=notes.windows.head;
 wi->n->prev->next=wi->n;
 wi->n->next->prev=wi->n;
 notes.windows.size++;
 }

/* the user wants to refresh window w. So we bring it in front. If
  you called this function you must call refreshend when you are ready
  with refreshing */
void w_refreshstart(struct w_window *w)
 { 
 struct wi_window *wi=(struct wi_window *)w;
 struct node *n;
 struct bounds b;
 my_assert(w!=NULL && wi->n!=NULL);
 if(wi->in_refresh) return;
 wi->in_refresh=1;
 ws_erasemouse();
 for(n=notes.windows.head;n->next!=NULL && n!=wi->n;n=n->next)
  if(n->d.w_w->drawn && wi_whereoverlap(wi,n->d.w_w,&b))
   wi_swapbg(n->d.w_w,&b);
 }
 
void w_refreshend(struct w_window *w)
 {
 struct wi_window *wi=(struct wi_window *)w;
 struct node *n;
 struct bounds b;
 my_assert(w!=NULL && wi->n!=NULL);
 if(!wi->in_refresh) return;
 wi->in_refresh=0; 
 for(n=wi->n->prev;n->prev!=NULL;n=n->prev)
  if(n->d.w_w->drawn && wi_whereoverlap(wi,n->d.w_w,&b))
   wi_swapbg(n->d.w_w,&b);
 ws_displaymouse();
 }

/* redraw w */
void w_refreshwin(struct w_window *w)
 { 
 struct wi_window *wi=(struct wi_window *)w;
 my_assert(w!=NULL);
 w_refreshstart(w);
 if(wi->drawn) wi_undrawwin(wi);
 wi_drawwin(wi);
 w_refreshend(w); 
 }
 
void w_changewintitle(struct w_window *w,char *s)
 {
 char *t;
 struct node *n;
 struct wi_window *wi=(struct wi_window *)w;
 struct wi_button *b;
 my_assert(w!=NULL && s!=NULL);
 t=(char *)w->title; FREE(t);  /* the const is needed for init the win,
  then the string is copied */
 checkmem(t=MALLOC(strlen(s)+1));
 strcpy(t,s); w->title=t;
 for(n=wi->buttonlist.head,b=NULL;n->next!=NULL && n->d.w_b->sys_button;
  n=n->next)
  if((int)n->d.w_b->b.data==1) { b=n->d.w_b; break; }
 if(b!=NULL) 
  {
  if(b->b.inscription.head->next)
   {
   FREE(b->b.inscription.head->d.w_d->contents.txt);
   checkmem(b->b.inscription.head->d.w_d->contents.txt=MALLOC(strlen(t)+1));
   strcpy(b->b.inscription.head->d.w_d->contents.txt,t);
   }
  if(b->b.a_inscription.head->next)
   {
   FREE(b->b.a_inscription.head->d.w_d->contents.txt);
   checkmem(b->b.a_inscription.head->d.w_d->contents.txt=MALLOC(strlen(t)+1));
   strcpy(b->b.a_inscription.head->d.w_d->contents.txt,t);
   }
  w_resizewin(&b->b); 
  w_refreshwin(w); 
  }
 }

int w_inrefresh(struct w_window *w)
 { return ((struct wi_window *)w)->in_refresh; }

void w_changetitlebartext(char const *s)
 {
 
 }
