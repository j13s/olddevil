/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_bmap.c - fit bitmaps to walls.
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de)

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
#include "tools.h"
#include "insert.h"
#include "do_event.h"
#include "do_bmap.h"

int bmap_movefactor=32;
double bmap_rotangle=M_PI/20;
int olddrawwhat=DW_DEFAULT;
int oldx1[4],oldy1[4],oldx2[4],oldy2[4];

void plotbitmap(struct wall *w)
 {
 int x;
 GrSetContext(view.txt_window);
 GrSetUserWindow(-view.txt_maxxnum*2048,-view.txt_maxynum*2048,
  view.txt_maxxnum*2048,view.txt_maxynum*2048);
 for(x=0;x<4;x++)
  {
  GrUsrLine(oldx1[x],oldy1[x],oldx2[x],oldy2[x],
   view.color[(x==0) || (x==3) ? HILIGHTCOLORS+3 : WHITE]|GrXOR);
  oldx1[x]=w->corners[x].xpos; oldy1[x]=w->corners[x].ypos;
  oldx2[x]=w->corners[(x+1)&0x3].xpos; oldy2[x]=w->corners[(x+1)&0x3].ypos;
  GrUsrLine(oldx1[x],oldy1[x],oldx2[x],oldy2[x],
   view.color[(x==0) || (x==3) ? HILIGHTCOLORS+3 : WHITE]|GrXOR);
  }
 GrSetContext(NULL);
 }
 
int initbitmapedit(struct wall *w)
 {
 int x,y;
 olddrawwhat=view.drawwhat; 
 view.drawwhat&=~(DW_CUBES|DW_ALLLINES|DW_THINGS|DW_DOORS); 
 view.drawwhat|=DW_BITMAP;
 GrSetContext(view.txt_window);
 GrSetUserWindow(-view.txt_maxxnum*2048,-view.txt_maxynum*2048,
  view.txt_maxxnum*2048,view.txt_maxynum*2048);
 for(y=0;y<view.txt_maxynum;y++)
  for(x=0;x<view.txt_maxxnum;x++)
   {
   GrBitBlt(NULL,GrSizeX()/2+x*64,GrSizeY()/2+y*64,view.txt_complete,0,0,63,
    63,GrWRITE);
   GrBitBlt(NULL,GrSizeX()/2-(x+1)*64,GrSizeY()/2+y*64,
    view.txt_complete,0,0,63,63,GrWRITE);
   GrBitBlt(NULL,GrSizeX()/2-(x+1)*64,GrSizeY()/2-(y+1)*64,
    view.txt_complete,0,0,63,63,GrWRITE);
   GrBitBlt(NULL,GrSizeX()/2+x*64,GrSizeY()/2-(y+1)*64,
    view.txt_complete,0,0,63,63,GrWRITE);
   }
 GrSetContext(NULL);
 for(x=0;x<4;x++)
  { oldx1[x]=oldx2[x]=oldy1[x]=oldy2[x]=0; }
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
  view.pcurrwall->corners[i].xpos=centerx+cos(angle)*turnx+sin(angle)*turny;
  view.pcurrwall->corners[i].ypos=centery-sin(angle)*turnx+cos(angle)*turny;
  }
 }
 
int do_bitmap(enum evcodes ec)
 {
 int i,xcenter=0,ycenter=0;
 if(!view.pcurrwall) return 1;
 switch(ec)
  {
  case ec_quit: view.drawwhat=olddrawwhat; break;
  case ec_fitbitmap: view.drawwhat=olddrawwhat; break;
  case ec_incrgridsize:
   for(i=0;i<4;i++) view.pcurrwall->corners[i]=stdcorners[i];
    printmsg("Bitmap fitted to bitmap."); 
   break;
  case ec_decrgridsize:
   for(i=0;i<4;i++) 
    newwallpnt(view.pcurrcube->d.c,view.pcurrwall,view.pcurrwall->p[i]->d.p);
   printmsg("Bitmap fitted to side."); 
   break;
  case ec_pmoveright: movebitmap(bmap_movefactor,0); break;
  case ec_pmoveleft: movebitmap(-bmap_movefactor,0); break;
  case ec_pmoveup: movebitmap(0,-bmap_movefactor); break;
  case ec_pmovedown: movebitmap(0,bmap_movefactor); break;
  case ec_pturnright: turnbitmap(bmap_rotangle); break;
  case ec_pturnleft: turnbitmap(-bmap_rotangle); break;
  case ec_incpmovefactor: case ec_incmovefactor:
   bmap_movefactor*=2; if(bmap_movefactor==2048) bmap_movefactor=1024;
   break;
  case ec_decpmovefactor: case ec_decmovefactor:
   bmap_movefactor/=2; if(bmap_movefactor==0) bmap_movefactor=1;
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
 
 