/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_tools.c - Some tools needed by everyone
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
#include "w_tools.h"

/* returns maximum x-size of a window */
int w_xmaxwinsize(void) { return notes.winspace.xs; }

/* returns maximum y-size of a window */
int w_ymaxwinsize(void) { return notes.winspace.ys; }

/* returns max y-size of freespace of window w */
int w_ymaxwininsize(struct w_window *w)
 {
 if(w->shrunk) return 0;
 return ((w->buttons&(wb_drag|wb_size|wb_close))!=0||w->title!=NULL)
  ? w_ymaxwinsize()-shapes.titlebar_height-1 : w_ymaxwinsize()-2; 
 }

/* returns x-size of freespace of window */
int w_xmaxwininsize(struct w_window *w)
 {
 if(w->shrunk) return 0;
 return w_xmaxwinsize()-2;
 }

/* returns x-size of freespace of window */
int w_xwininsize(struct w_window *w)
 {
 if(w->shrunk) return 0;
 return w->xsize-2;
 }

/* returns y-size of freespace of window */
int w_ywininsize(struct w_window *w)
 {
 if(w->shrunk) return 0;
 return ((w->buttons&(wb_drag|wb_size|wb_close))!=0||w->title!=NULL)
  ? w->ysize-shapes.titlebar_height-1 : w->ysize-2; 
 }

/* returns the x-coord on the screen when x is the coordinate in
 the free space from the window. x must lie in the free-space. It is 
 not checked if the coordinate which is returned makes sense! */
int w_xwinincoord(struct w_window *w,int x)
 { return w->xpos+x+1; }

/* returns the y-coord on the screen when y is the coordinate in
 the free space from the window. y must lie in the free-space. It is 
 not checked if the coordinate which is returned makes sense! */
int w_ywinincoord(struct w_window *w,int y)
 { return ((w->buttons&(wb_drag|wb_size|wb_close))!=0 || w->title!=NULL)
    ? w->ypos+y+shapes.titlebar_height : w->ypos+y+1; }

/* This is used to determine the size of the window when it is opened, so w is
   the init-structure of the window and xs,ys are the needed size in
   the window. The result is saved in w->xsize,w->ysize */
void w_winoutsize(struct w_window *w,int xs,int ys)
 {
 w->xsize=xs+2;
 w->ysize=ys+(((w->buttons&(wb_drag|wb_size|wb_close))!=0||w->title!=NULL)
  ? shapes.titlebar_height+1 : 2); 
 }

/* returns the x-coord in the window when x is the coordinate on the screen */
int w_xscreencoord(struct w_window *w,int x)
 { return x-w->xpos-1; }

/* returns the y-coord in the window when x is the coordinate on the screen */
int w_yscreencoord(struct w_window *w,int y)
 { return ((w->buttons&(wb_drag|wb_size|wb_close))!=0 || w->title!=NULL)
    ? y-w->ypos-shapes.titlebar_height : y-w->ypos-1; }

/* Copy the string str. */
char *w_copystr(const char *str)
 {
 char *s;
 if(str==NULL) return NULL;
 checkmem(s=MALLOC(strlen(str)+1));
 strcpy(s,str);
 return s;
 }
 
/* Search the color index which has the 'nearest' color to r,g,b */
int w_makecolor(int r,int g,int b)
 {
 int i,n,dist,mdist=255+255+255+1,rn,bn,gn;
 for(n=0,i=0;n<256;n++)
  {
  ws_getcolor(n,&rn,&gn,&bn);
  dist=abs(r-rn)+abs(g-gn)+abs(b-bn);
  if(dist<mdist)
   { i=n; if((mdist=dist)==0) break; }
  }
 return i;
 }

int w_titlebarheight(void) { return shapes.titlebar_height; }

int w_xwinspacecoord(int x) { return x-notes.winspace.xp; }

int w_ywinspacecoord(int y) { return y-notes.winspace.yp; }

int w_xscrspacecoord(int x) { return x+notes.winspace.xp; }

int w_yscrspacecoord(int y) { return y+notes.winspace.yp; }
