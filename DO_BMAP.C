/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_bmap.c - fit bitmaps to walls.
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
#include "system.h"
#include "tools.h"
#include "grfx.h"
#include "insert.h"
#include "do_event.h"
#include "do_bmap.h"

extern int supervga;
int bmap_movefactor=32;
double bmap_rotangle=M_PI/40;
int olddrawwhat=DW_DEFAULT,oldnumlock;
int oldx1[4],oldy1[4],oldx2[4],oldy2[4];

void bm_line(int x1,int y1,int x2,int y2,int c)
 {
 int rx1,ry1,rx2,ry2;
 /* faster is of course >>5 instead of *64/2048 but I think the compiler will
    see this */
 rx1=(x1*64)/2048+view.fit_xpos;
 rx2=(x2*64)/2048+view.fit_xpos;
 ry1=(y1*64)/2048+view.fit_ypos;
 ry2=(y2*64)/2048+view.fit_ypos;
 sys_clipline(rx1,ry1,rx2,ry2,c,0);
 }

void plotbitmap(struct wall *w)
 {
 int x,y;
 /* XOR-ing the line is cooler, but then you can't see the color of the
    lines anymore. So hope your grfx-card is fast enough. */
 for(y=-view.fit_maxynum/2;y<(view.fit_maxynum+1)/2;y++)
  for(x=-view.fit_maxxnum/2;x<(view.fit_maxxnum+1)/2;x++)
   sys_drawtexture(1,view.fit_xpos+x*64,view.fit_ypos+y*64,0,0,NULL);
 for(x=0;x<4;x++)
  {
/*  bm_line(oldx1[x],oldy1[x],oldx2[x],oldy2[x],
   view.color[x==0 ? HILIGHTCOLORS+3 : x==3 ? HILIGHTCOLORS+4 : WHITE]); */
  oldx1[x]=w->corners[x].xpos; oldy1[x]=w->corners[x].ypos;
  oldx2[x]=w->corners[(x+1)&0x3].xpos; oldy2[x]=w->corners[(x+1)&0x3].ypos; 
  bm_line(oldx1[x],oldy1[x],oldx2[x],oldy2[x],
   view.color[x==0 ? HILIGHTCOLORS+3 : x==3 ? HILIGHTCOLORS+4 : WHITE]);
  }
 }
 
void initclipping(void)
 {
 sys_initclipping(view.fit_xpos-view.fit_maxxnum/2*64,
  view.fit_ypos-view.fit_maxynum/2*64,
  view.fit_xpos+(view.fit_maxxnum+1)/2*64,
  view.fit_ypos+(view.fit_maxynum+1)/2*64);
 }
  
int initbitmapedit(struct wall *w)
 {
 int x,y;
 olddrawwhat=view.drawwhat; 
 if((oldnumlock=sys_getnumlock())==1) sys_numlockonoff();
 view.drawwhat&=~(DW_CUBES|DW_ALLLINES|DW_THINGS|DW_DOORS|DW_GRID); 
 view.drawwhat|=DW_BITMAP;
 changeto256(); 
 for(y=-view.fit_maxynum/2;y<(view.fit_maxynum+1)/2;y++)
  for(x=-view.fit_maxxnum/2;x<(view.fit_maxxnum+1)/2;x++)
   sys_drawtexture(1,view.fit_xpos+x*64,view.fit_ypos+y*64,0,0,NULL);
 for(x=0;x<4;x++)
  { oldx1[x]=oldx2[x]=oldy1[x]=oldy2[x]=0; }
 initclipping();
 plotbitmap(w); 
 return 1;
 }

void movebitmap(int x,int y)
 {
 int i;
 for(i=0;i<4;i++)
  { view.pcurrwall->corners[i].xpos+=x;  
    view.pcurrwall->corners[i].ypos+=y;  }
 }
 
void turnbitmap(double angle)
 {
 int turnx,turny,centerx=0,centery=0,i;
 for(i=0;i<4;i++)
  { centerx+=view.pcurrwall->corners[i].xpos;
    centery+=view.pcurrwall->corners[i].ypos; }
 centerx/=4; centery/=4;
 for(i=0;i<4;i++)
  {
  turnx=view.pcurrwall->corners[i].xpos-centerx;
  turny=view.pcurrwall->corners[i].ypos-centery;
  view.pcurrwall->corners[i].xpos=centerx+cos(angle)*turnx+sin(angle)*turny+
   0.5;
  view.pcurrwall->corners[i].ypos=centery-sin(angle)*turnx+cos(angle)*turny+
   0.5;
  }
 }
 
void stretchbitmap(double factor)
 {
 int stretchx[4],stretchy[4],centerx=0,centery=0,i;
 for(i=0;i<4;i++)
  { centerx+=view.pcurrwall->corners[i].xpos;
    centery+=view.pcurrwall->corners[i].ypos; }
 centerx/=4; centery/=4;
 for(i=0;i<4;i++)
  {
  stretchx[i]=view.pcurrwall->corners[i].xpos-centerx;
  stretchy[i]=view.pcurrwall->corners[i].ypos-centery;
  if((int)stretchx[i]*factor==0 || (int)stretchy[i]*factor==0) return;
  }
 for(i=0;i<4;i++)
  {
  view.pcurrwall->corners[i].xpos=centerx+stretchx[i]*factor;
  view.pcurrwall->corners[i].ypos=centery+stretchy[i]*factor;
  }
 }
 
void bm_quit(void)
 {
 struct node *n;
 struct cube *c;
 int j;
 view.drawwhat=olddrawwhat; 
 if(oldnumlock && !sys_getnumlock()) sys_numlockonoff();
 changeto16();
 for(n=view.tagged[tt_wall].head;n->next!=NULL;n=n->next)
  {
  c=n->d.n->d.c;
  if(c->walls[n->no%6])
   for(j=0;j<4;j++)
    { c->walls[n->no%6]->corners[j].xpos=view.pcurrwall->corners[j].xpos;
      c->walls[n->no%6]->corners[j].ypos=view.pcurrwall->corners[j].ypos;}
  }
 }

void bm_printmsg(char *txt,...)
 {
 va_list args;
 va_start(args,txt);
 sys_killclipping();
 vprintmsg(txt,args);
 initclipping();
 va_end(args);
 }
 
int do_bitmap(enum evcodes ec)
 {
 int i,xcenter=0,ycenter=0;
 if(!view.pcurrwall) return 1;
 switch(ec)
  {
  case ec_quit: bm_quit(); break;
  case ec_fitbitmap: bm_quit(); break;
  case ec_incrgridsize:
   for(i=0;i<4;i++) 
    { view.pcurrwall->corners[i].xpos=stdcorners[i].xpos;
      view.pcurrwall->corners[i].ypos=stdcorners[i].ypos; }
   bm_printmsg("Bitmap fitted to bitmap."); 
   break;
  case ec_decrgridsize:
   for(i=0;i<4;i++) 
    newwallpnt(view.pcurrcube->d.c,view.pcurrwall,view.pcurrwall->p[i]->d.p);
   bm_printmsg("Bitmap fitted to side."); 
   break;
  case ec_pmoveforw: stretchbitmap(1+bmap_movefactor/1024.0); break;
  case ec_pmoveback: stretchbitmap(1.0/(1+bmap_movefactor/1024.0)); break;
  case ec_pmoveright: movebitmap(bmap_movefactor,0); break;
  case ec_pmoveleft: movebitmap(-bmap_movefactor,0); break;
  case ec_pmoveup: movebitmap(0,-bmap_movefactor); break;
  case ec_pmovedown: movebitmap(0,bmap_movefactor); break;
  case ec_pturnright: turnbitmap(bmap_rotangle); break;
  case ec_pturnleft: turnbitmap(-bmap_rotangle); break;
  case ec_incpmovefactor: 
   bmap_movefactor*=2; if(bmap_movefactor==2048) bmap_movefactor=1024;
   bm_printmsg("Movefactor now %4.1f pixels.",bmap_movefactor*64.0/2048);
   break;
  case ec_decpmovefactor: 
   bmap_movefactor/=2; if(bmap_movefactor==0) bmap_movefactor=1;
   bm_printmsg("Movefactor now %4.1f pixels.",bmap_movefactor*64.0/2048);
   break;
  case ec_incprotangle: bmap_rotangle*=2; 
   bm_printmsg("Rotangle is now %5.1f degrees.",bmap_rotangle*180/M_PI); 
   break;
  case ec_decprotangle: bmap_rotangle/=2; 
   bm_printmsg("Rotangle is now %5.1f degrees.",bmap_rotangle*180/M_PI); 
   break;
  default: break;
  }
 /* Search best position for corners (center near the origin) */
 for(i=0;i<4;i++)
  { xcenter+=view.pcurrwall->corners[i].xpos;
    ycenter+=view.pcurrwall->corners[i].ypos; }
 xcenter=(xcenter/4/2048)*2048;
 ycenter=(ycenter/4/2048)*2048;
 movebitmap(-xcenter,-ycenter);
 return 6;
 }
 
