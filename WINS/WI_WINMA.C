/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_winman.c - Handling of windows and screens -- internal routines
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

/* Some tools. These are only internal routines need for
   window-handling. You don't need to replace them in a GUI-based
   Operating System. */

#include "wins_int.h"
#include "w_init.h"
#include "w_tools.h"
#include "w_draw.h"
#include "wi_buts.h"
#include "wi_winma.h"

/* Test if windows w1 and w2 overlap each other. Return a 1 if
   there is an overlap. Return a 0 if there is no overlap. */
int wi_winoverlap(struct wi_window *w1,struct wi_window *w2)
 {
 my_assert(w1!=NULL && w2!=NULL);
 return !(w1->w.xpos+w1->w.xsize<=w2->w.xpos || 
  w2->w.xpos+w2->w.xsize<=w1->w.xpos || w1->w.ypos+w1->w.ysize<=w2->w.ypos || 
  w2->w.ypos+w2->w.ysize<=w1->w.ypos);
 }

/* Test if windows w1 and w2 overlap each other. Return a 1 if
   there is an overlap. Return a 0 if there is no overlap. The coordinates
   of the overlap are saved in where */
int wi_whereoverlap(struct wi_window *w1,struct wi_window *w2,
 struct bounds *where)
 {
 my_assert(w1!=NULL && w2!=NULL);
 if(!wi_winoverlap(w1,w2)) return 0;
 where->xp=w1->w.xpos<w2->w.xpos ? w2->w.xpos : w1->w.xpos;
 where->yp=w1->w.ypos<w2->w.ypos ? w2->w.ypos : w1->w.ypos;
 where->xs=(w1->w.xpos+w1->w.xsize<w2->w.xpos+w2->w.xsize ? 
  w1->w.xpos+w1->w.xsize : w2->w.xpos+w2->w.xsize) - where->xp;
 where->ys=(w1->w.ypos+w1->w.ysize<w2->w.ypos+w2->w.ysize ? 
  w1->w.ypos+w1->w.ysize : w2->w.ypos+w2->w.ysize) - where->yp;
 return 1;
 }
 
/* Draw the window wi in the foreground and save the background. */
void wi_drawwin(struct wi_window *wi)
 {
 int titlebar,x;
 struct node *n;
 my_assert(wi!=NULL);
 titlebar=((wi->w.buttons&(wb_drag|wb_size|wb_close))!=0||wi->w.title!=NULL);
 if(wi->w.shrunk && !titlebar) wi->w.shrunk=0;
 wi->win_contents=
  ((!wi->w.shrunk && wi->w.ysize>=shapes.titlebar_height*2) || !titlebar);
 if(wi->bg) ws_freebitmap(wi->bg);
 checkmem(wi->bg=ws_savebitmap(NULL,wi->w.xpos,wi->w.ypos,wi->w.xsize,
  wi->w.ysize));
 if(wi->win_contents)
  {
  ws_drawbox(w_xwinincoord(&wi->w,-1),w_ywinincoord(&wi->w,-1),
   w_xwininsize(&wi->w)+2,w_ywininsize(&wi->w)+2,
   notes.colindex[wi==notes.cur_win ? cv_curwin : cv_winedge],0);
  ws_drawfilledbox(w_xwinincoord(&wi->w,0),w_ywinincoord(&wi->w,0),
   w_xwininsize(&wi->w),w_ywininsize(&wi->w),notes.colindex[cv_winfill],0); 
  }
 for(n=wi->buttonlist.head;n->next!=NULL && n->d.w_b->sys_button;n=n->next)
  wi_drawbutton(n->d.w_b,0);
 if(titlebar)
  {
  if(!(wi->w.buttons&(wb_drag|wb_size)))
   {
   if(!(wi->w.buttons&(wb_close|wb_shrink)))
    ws_drawfilledbox(x=wi->w.xpos,wi->w.ypos,wi->w.xsize,
     shapes.titlebar_height,
     notes.colindex[notes.cur_win==wi ? cv_curwin : cv_buttonin],0);
   else
    ws_drawfilledbox(x=wi->w.xpos+shapes.closebutton_width,wi->w.ypos,
     wi->w.xsize-shapes.closebutton_width,shapes.titlebar_height,
     notes.colindex[notes.cur_win==wi ? cv_curwin : cv_buttonin],0);
   ws_drawtext(x+4,wi->w.ypos+2,wi->w.xpos+wi->w.xsize-x-6,
    wi->w.title,notes.colindex[cv_textbg],-1);
   ws_drawtext(x+3,wi->w.ypos+1,wi->w.xpos+wi->w.xsize-x-6,
    wi->w.title,notes.colindex[cv_textfg],-1);
   }
  }
 wi->drawn=1;
 if(wi->win_contents)
  switch(wi->w.refresh)
   {
   case wr_routine: wi->w.refresh_routine(&wi->w,wi->w.data); break;
   case wr_normal:
    if(wi->in!=NULL)
     {
     ws_copybitmap(NULL,w_xwinincoord(&wi->w,0),w_ywinincoord(&wi->w,0),
      wi->in,0,0,w_xwininsize(&wi->w)>wi->in->xsize ? wi->in->xsize :
      w_xwininsize(&wi->w),w_ywininsize(&wi->w)>wi->in->ysize ? 
      wi->in->xsize : w_ywininsize(&wi->w),1);
     ws_freebitmap(wi->in); wi->in=NULL;
     }
    break;
   default:
    fprintf(errf,"Unknown refresh-type in drawwin: %d\n",wi->w.refresh);
   }
 }

void wi_undrawwin(struct wi_window *wi)
 {
 if(!wi->drawn) return;
 my_assert(wi!=NULL && wi->bg!=NULL);
 if(wi->w.refresh!=wr_routine)
  { 
  if(wi->in) { ws_freebitmap(wi->in); wi->in=NULL; }
  checkmem(wi->in=ws_savebitmap(NULL,w_xwinincoord(&wi->w,0),
   w_ywinincoord(&wi->w,0),w_xwininsize(&wi->w),w_ywininsize(&wi->w)));
  }
 ws_restorebitmap(wi->bg); wi->bg=NULL;
 wi->drawn=0;
 }

