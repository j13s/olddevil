/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    plot.c - all vector drawing routines.
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

#define DEBUG 0

#include "structs.h"
#include "userio.h"
#include "tools.h"
#include "calctxt.h"
#include "insert.h"
#include "do_move.h"
#include "do_mod.h"
#include "grfx.h"
#include "readtxt.h"
#include "tag.h"
#include "macros.h"
#include "plotdata.h"
#include "plotsys.h"
#include "plottxt.h"
#include "plot.h"
#define COLORNUM2(d,min) (((d)>=view.maxvisibility) ? (min) : \
   (d<=0.0) ? GRAYSCALE : ((int)((GRAYSCALE-1)*(1-(d)/view.maxvisibility))+1))
#define WCOLORNUM(d,h) view.color[(h)<=0 ? COLORNUM2(d,(h)==-1 ? BLACK : 1) :\
 ((h)==256 ? BLACK : HILIGHTCOLORS+(h)-1)]
#define TCOLORNUM(c,h) view.color[(h)<=0 ? \
 ((h)==0 ? (c)+THINGCOLORS : BLACK) : HILIGHTCOLORS+(h)-1]
#define ANALYZE_HILIGHT(h) (h<0 ? -h : (h==256 ? -1 : (h==257 ? 0 : h)))
int partcolor(struct door *d,float dist,int h)
 {
 int i,key;
 if(h<0 && dist>view.maxvisibility) return BLACK;
 if(d->tagged) return HILIGHTCOLORS+2;
 switch(d->type1)
  {
  case door1_onlytxt: case door1_shootthrough:
   return COLORNUM2(dist,h==0 ? 1 : BLACK); 
  case door1_blow: /* blow door */
   return DOORCOLORS;
  case door1_normal: /* normal door */
   for(i=0,key=d->key;key!=0;i++,key=key>>1);
   return i+DOORCOLORS;
  default:
   return WHITE;
  }
 }
#define PCOLORNUM(dist,door,h) view.color[(h)<=0 ? partcolor(door,dist,h) : \
 ((h)==256 ? BLACK : HILIGHTCOLORS+(h)-1)]

#include "in_plot.h"

/* Maximum/minimum viewangle. The minimum view angle will be used for
 the smaller axis if the quotient between larger and smaller axis is
 smaller than tan(MAX_VIEWANGLE)/tan(MIN_VIEWANGLE). The max. value
 will be used for the larger axis if the quotient is greater. */
#define MAX_VIEWANGLE (M_PI/3.0) 
#define MIN_VIEWANGLE (M_PI/5.0)

struct point e0,er[3]; /* e0 and e[3] for current view */
float xviewphi=0.5,yviewphi=0.5,z_dist; /* viewphi is the cos(view angle) */
struct point x0,m0; /* x0 viewpoint, m0 line viewpoint-center of screen */
int max_xcoord,max_ycoord; /* (scr_xysize-1)/2 */
int scr_xsize,scr_ysize;

float FABS(float x) { return x<0.0 ? -x : x; }

void copytoscreen(void) 
 { psys_copytoscreen(w_xwinincoord(l->w,0),w_ywinincoord(l->w,0),
    w_xwinincoord(l->w,0),w_ywinincoord(l->w,0),w_xwininsize(l->w),
    w_ywininsize(l->w)); }

void initcoordsystem(int lr,struct point *e0,struct point *er,
 struct point *x0)
 {
 int i;
 er[0]=lr>0 ? view.e[2] : view.e[0];
 er[1]=view.e[1];
 if(lr>0) for(i=0;i<3;i++) er[2].x[i]=-view.e[0].x[i];
 else er[2]=view.e[2];
 if(l->whichdisplay && lr>=0)
  for(i=0;i<3;i++) e0->x[i]=view.e0.x[i]-er[2].x[i]*view.distcenter;
 else *e0=view.e0;
 /* Position of viewpoint: */
 for(i=0;i<3;i++)
  /* from the middle of the screen to behind the screen: */
  x0->x[i]=e0->x[i]-er[2].x[i]*z_dist;
 }
 
void makeview(int lr)
 {
 int i,max_axis,min_axis;
 float max_angle,min_angle;
 my_assert(lr==-1 || lr==0 || lr==1);
 if(lr==-1)
  { scr_xsize=init.xres; scr_ysize=init.yres; }
 else
  { scr_xsize=(view.whichdisplay ? w_xwininsize(l->w)/2 : w_xwininsize(l->w));
    scr_ysize=w_ywininsize(l->w); }
 max_xcoord=scr_xsize/2; max_ycoord=scr_ysize/2;
 scr_xsize=max_xcoord*2+1; scr_ysize=max_ycoord*2+1;
 max_axis = max_xcoord>=max_ycoord ?
  (min_axis=max_ycoord,max_xcoord) : (min_axis=max_xcoord,max_ycoord);
 if(max_axis*tan(MIN_VIEWANGLE)>=min_axis*tan(MAX_VIEWANGLE))
  {max_angle=MAX_VIEWANGLE; min_angle=atan(tan(max_angle)*min_axis/max_axis);}
 else  
  {min_angle=MIN_VIEWANGLE; max_angle=atan(tan(min_angle)*max_axis/min_axis);}
 xviewphi= max_xcoord>=max_ycoord ? (yviewphi=cos(min_angle),cos(max_angle)) :
  (yviewphi=cos(max_angle),cos(min_angle));
 z_dist=max_axis/tan(max_angle);
 initcoordsystem(lr,&e0,er,&x0);
 for(i=0;i<3;i++) m0.x[i]=e0.x[i]-x0.x[i];
 }

/* return the coords of point p on the screen plane in pix. Return a
 1 if the point is visible, a zero otherwise. */
int in_getpixelcoords(struct point *p,struct pixel *pix)
 {
 struct point d,x,y;
 float fx,fy;
 int i;
 for(i=0;i<3;i++) d.x[i]=p->x[i]-x0.x[i];
 pix->d=SCALAR(&d,&er[2]); fx=SCALAR(&d,&er[0]); fy=SCALAR(&d,&er[1]);
 if(pix->d<z_dist) return 0; /* point behind me */
 for(i=0;i<3;i++)
  { x.x[i]=d.x[i]-fy*er[1].x[i]; y.x[i]=d.x[i]-fx*er[0].x[i]; }
 if(SCALAR(&x,&er[2])<xviewphi || SCALAR(&y,&er[2])<yviewphi) return 0;
 pix->x=fx*z_dist/pix->d+0.5; pix->y=fy*z_dist/pix->d+0.5;
 return 1;
 }

/* returns the coords of point p in pix. lr==0 for left display, lr==1
   for right display */
int getpixelcoords(int lr,struct point *p,struct pixel *pix)
 { makeview(lr); return in_getpixelcoords(p,pix); }
 
/* calculates point of intersection of two lines m1*t1+b1, m2*t2+b2:
    a=b2-b1, e=m1, d=m2 */
int calchit(float *r,float ax,float ay,float ex,float ey,
 float dx,float dy)
 { 
 float D=ey*dx-ex*dy,s;
 if(D==0.0) 
  { *r=0; return 0; } /* no hit at all, lines are parallel */ \
 *r=(ay*dx-ax*dy)/D; s=(ex*ay-ey*ax)/D; 
 return (*r>=0.0 && *r<=1.0) && (s>=0.0 && s<=1.0); 
 }

/* get the (clipped) screen coords for line sp->ep (lr==0 on left,==1 on right  display) and return them in spix,epix. If the line is not visible, return
 a 0,otherwise a 1. if checkdist==1 then check if the line is too
 far away */
int getscreencoords(int lr,struct point *sp,struct point *ep,
 struct pixel *spix,struct pixel *epix,int checkdist)
 {
 struct point e_d,s_d,d,p;
 float f1,f2,s_s[3],e_s[3],l2_e,l2_s;
 struct point_2d sp2d,ep2d;
 int i;
 /* clip the line against the viewplane */
 for(i=0;i<3;i++)
  { e_d.x[i]=ep->x[i]-x0.x[i]; s_d.x[i]=sp->x[i]-x0.x[i]; 
    d.x[i]=ep->x[i]-sp->x[i]; }
 f1=SCALAR(&s_d,&er[2]); f2=SCALAR(&e_d,&er[2]);
 if(f1<z_dist && f2<z_dist) return 0; /* point behind me */
 if(checkdist && f1>view.maxvisibility && f2>view.maxvisibility) return 0;
  /* point too far away */
 if(f1>=z_dist && f2<z_dist)
  { s_s[2]=f2; e_s[2]=f1; p=e_d; e_d=s_d; s_d=p; }
 else { s_s[2]=f1; e_s[2]=f2; }
 s_s[0]=SCALAR(&s_d,&er[0]); e_s[0]=SCALAR(&e_d,&er[0]);
 s_s[1]=SCALAR(&s_d,&er[1]); e_s[1]=SCALAR(&e_d,&er[1]);
 l2_s=SCALAR(&s_d,&s_d); l2_e=SCALAR(&e_d,&e_d);
 if(((s_s[0]>0.0 && e_s[0]>0.0) || (s_s[0]<0.0 && e_s[0]<0.0)) &&
  s_s[0]*s_s[0]>l2_s*xviewphi && e_s[0]*e_s[0]>l2_e*xviewphi) return 0;
 if(((s_s[1]>0.0 && e_s[1]>0.0) || (s_s[1]<0.0 && e_s[1]<0.0)) &&
  s_s[1]*s_s[1]>l2_e*yviewphi && e_s[1]*e_s[1]>l2_e*yviewphi) return 0;
 if(s_s[2]<z_dist && e_s[2]>=z_dist)
  { /* clip against viewplane */
  f1=(z_dist-s_s[2])/(e_s[2]-s_s[2]);
  sp2d.x[0]=s_s[0]+(e_s[0]-s_s[0])*f1;
  sp2d.x[1]=s_s[1]+(e_s[1]-s_s[1])*f1;
  spix->d=z_dist;
  }
 else /* no clipping against viewplane */
  { sp2d.x[0]=z_dist*s_s[0]/s_s[2];
    sp2d.x[1]=z_dist*s_s[1]/s_s[2];
    spix->d=s_s[2]; }
 ep2d.x[0]=z_dist*e_s[0]/e_s[2];
 ep2d.x[1]=z_dist*e_s[1]/e_s[2];
 epix->d=e_s[2];
 /* clip against screen borders: start point */
 if(FABS(sp2d.x[0])>max_xcoord) 
  {
  if(ep2d.x[0]==sp2d.x[0]) return 0;
  if(sp2d.x[0]>0.0) f1=(sp2d.x[0]-max_xcoord)/(sp2d.x[0]-ep2d.x[0]);
  else f1=(-max_xcoord-sp2d.x[0])/(ep2d.x[0]-sp2d.x[0]); 
  if(f1<0.0 || f1>1.0) return 0;
  sp2d.x[0]=sp2d.x[0]+(ep2d.x[0]-sp2d.x[0])*f1; 
  sp2d.x[1]=sp2d.x[1]+(ep2d.x[1]-sp2d.x[1])*f1;
  }
 if(FABS(sp2d.x[1])>max_ycoord) 
  {
  if(ep2d.x[1]==sp2d.x[1]) return 0;
  if(sp2d.x[1]>0.0) f1=(sp2d.x[1]-max_ycoord)/(sp2d.x[1]-ep2d.x[1]);
  else f1=(-max_ycoord-sp2d.x[1])/(ep2d.x[1]-sp2d.x[1]);
  if(f1<0.0 || f1>1.0) return 0;
  sp2d.x[0]=sp2d.x[0]+(ep2d.x[0]-sp2d.x[0])*f1; 
  sp2d.x[1]=sp2d.x[1]+(ep2d.x[1]-sp2d.x[1])*f1;
  }
 /* end point */
 if(FABS(ep2d.x[0])>max_xcoord) 
  {
  if(ep2d.x[0]==sp2d.x[0]) return 0;
  if(ep2d.x[0]>0.0) f1=(ep2d.x[0]-max_xcoord)/(ep2d.x[0]-sp2d.x[0]);
  else f1=(-max_xcoord-ep2d.x[0])/(sp2d.x[0]-ep2d.x[0]); 
  if(f1<0.0 || f1>1.0) return 0;
  ep2d.x[0]=ep2d.x[0]-(ep2d.x[0]-sp2d.x[0])*f1; 
  ep2d.x[1]=ep2d.x[1]-(ep2d.x[1]-sp2d.x[1])*f1;
  }
 if(FABS(ep2d.x[1])>max_ycoord) 
  {
  if(ep2d.x[1]==sp2d.x[1]) return 0;
  if(ep2d.x[1]>0.0) f1=(ep2d.x[1]-max_ycoord)/(ep2d.x[1]-sp2d.x[1]);
  else f1=(-max_ycoord-ep2d.x[1])/(sp2d.x[1]-ep2d.x[1]);
  if(f1<0.0 || f1>1.0) return 0;
  ep2d.x[0]=ep2d.x[0]-(ep2d.x[0]-sp2d.x[0])*f1; 
  ep2d.x[1]=ep2d.x[1]-(ep2d.x[1]-sp2d.x[1])*f1;
  }
 spix->x=sp2d.x[0]+max_xcoord+0.5;
 spix->y=-sp2d.x[1]+max_ycoord+0.5;
 epix->x=ep2d.x[0]+max_xcoord+0.5;
 epix->y=-ep2d.x[1]+max_ycoord+0.5;
 if(lr) { epix->x+=scr_xsize; spix->x+=scr_xsize; }
 return 1;
 }
 
void in_plot3dline(int lr,struct point *sp,struct point *ep,int color,
 int xor,int checkdist)
 {
 struct pixel spix,epix;
 if(getscreencoords(lr,sp,ep,&spix,&epix,checkdist))
  plotline(spix.x,spix.y,epix.x,epix.y,color,xor);
 }

/* draw a line from sp to ep with color. if xor==1 xored. if checkdist==1
 draw the line only if it's within maxvisibility */
void plot3dline(struct point *sp,struct point *ep,int color,int xor,
 int checkdist)
 { makeview(0); in_plot3dline(0,sp,ep,color,xor,checkdist);
   if(view.whichdisplay)
    { makeview(1); in_plot3dline(1,sp,ep,color,xor,checkdist); } }
 
/* highlight==0: normal wallcolor 
   highlight>0: view.color[HILIGHTCOLORS+highlight-1]
   highlight<0: same as highlight>0 with xor.
   highlight==256: black
   highlight==257: normal wallcolor or black if too far away*/ 
void in_plotmarker(int w,struct point *p,int hilight)
 {
 struct pixel spix,epix;
 struct point dp[8];
 int i;
 makemarker(p,dp);
 for(i=0;i<8;i+=2) 
  if(getscreencoords(w,&dp[i],&dp[i+1],&spix,&epix,hilight==0||hilight==257))
   plotline(spix.x,spix.y,epix.x,epix.y,
    WCOLORNUM((epix.d+spix.d)/2.0,ANALYZE_HILIGHT(hilight)),hilight<0);
  else if(hilight==257)
   if(getscreencoords(w,&dp[i],&dp[i+1],&spix,&epix,0))
    plotline(spix.x,spix.y,epix.x,epix.y,
     WCOLORNUM((epix.d+spix.d)/2.0,ANALYZE_HILIGHT(256)),0);
 }

void plotmarker(struct point *p,int hilight) 
 { makeview(0); in_plotmarker(0,p,hilight);
   if(view.whichdisplay) { makeview(1); in_plotmarker(1,p,hilight); } }
   
/* hilight==0: normal
   hilight>0: view.color[HILIGHTCOLORS+hilight-1]
   hilight==-1: undraw (same as normal but unconditional)
   hilight==256: black 
   drawswitched==1: draw objects switched with this door
   drawswitched==-1: undraw objects switched with this door
   drawswitched==0: only draw the door */
void in_plotdoor(int w,struct node *n,int hilight,int drawswitched,int xor)
 {
 struct door *d=n->d.d;
 struct pixel spix,epix;
 struct node *sdn;
 int j;
 for(j=0;j<4;j++)
  if(getscreencoords(w,&d->p,d->w->p[j]->d.p,&spix,&epix,hilight==0))
   plotline(spix.x,spix.y,epix.x,epix.y,
    PCOLORNUM((epix.d+spix.d)/2.0,d,hilight),xor);
 if(d->sd!=NULL && drawswitched)
  {
  switch(getsdoortype(d->sd->d.sd))
   {
   case sdtype_door:
    for(j=0;j<d->sd->d.sd->num;j++) 
     in_plotdoor(w,d->sd->d.sd->target[j],drawswitched>0 ? 2 : -1,0,0);
    break;
   case sdtype_cube:
    for(j=0;j<d->sd->d.sd->num;j++) 
     in_plotcube(w,d->sd->d.sd->target[j],drawswitched>0 ? 2 : -1,0,0,1,0);
    break;
   case sdtype_side:
    for(j=0;j<d->sd->d.sd->num;j++) 
     in_plotwall(w,d->sd->d.sd->target[j]->d.c,d->sd->d.sd->walls[j],
      drawswitched>0 ? 1 : -1,0);
    break;    
   default: break;
   }
  }
 if(drawswitched)
  for(sdn=d->sdoors.head;sdn->next!=NULL;sdn=sdn->next)
   in_plotdoor(w,sdn->d.n,drawswitched>0 ? 4 : -1,0,0); 
 }

void plotdoor(struct node *n,int hilight,int xor) 
 { makeview(0); in_plotdoor(0,n,hilight,0,xor);
   if(view.whichdisplay) { makeview(1); in_plotdoor(1,n,hilight,0,xor); } }

/* hilight==0: normal
   hilight>0: view.color[HILIGHTCOLORS+hilight-1] 
   hilight==-1: undraw (same as normal but draw unconditional)
   hilight==-2: undraw with mincolor==1
   hilight==256: black
   withdoors==1: draw doors that switch this cube
   withdoors==-1: undraw doors ....
   withdoors==0: only draw cube */
void in_plotcube(int w,struct node *n,int hilight,int withdoors,int xor,
 int withalllines,int withlockedsides)
 {
 struct pixel spix,epix;
 int j,next,testdist;
 struct cube *c=n->d.c;
 struct node *sdn;
 testdist=(hilight==0);
 if((hilight==0||hilight==-1) && c->type!=cube_normal)
  {
  withalllines=1;
  switch(c->type)
   {
   case cube_fuel: hilight=5; break;
   case cube_reactor: hilight=4; break;
   case cube_producer: hilight=2; break;
   case cube_blueflag: hilight=8; break;
   case cube_redflag: hilight=7; break;
   }
  }
 if(hilight==1 && testtag(tt_cube,n)) hilight=6;
 if(withdoors)
  for(sdn=c->sdoors.head;sdn->next!=NULL;sdn=sdn->next)
   in_plotdoor(w,sdn->d.n,withdoors>0 ? 4 : -1,0,0);
 next=0xffff;
 if(!withalllines)
  {
  for(j=0;j<6;j++)
   if(c->nc[j]!=NULL && c->nc[j]->no<n->no)
    switch(j)
     {
     case 0 : next&=~(0x4|0x40|0x400|0x800); break;
     case 1 : next&=~(0x8|0x80|0x100|0x800); break;
     case 2 : next&=~(0x1|0x10|0x100|0x200); break;
     case 3 : next&=~(0x2|0x20|0x200|0x400); break;
     case 4 : next&=~(0x10|0x20|0x40|0x80); break;
     case 5 : next&=~(0x1|0x2|0x4|0x8); break;
     }  
  if(next==0) return;
  }
 for(j=0;j<4;j++) 
  {
  if((next&(1<<j))!=0 && 
   getscreencoords(w,c->p[j]->d.p,c->p[j==3?0:j+1]->d.p,&spix,&epix,
    testdist))
   plotline(spix.x,spix.y,epix.x,epix.y,
    WCOLORNUM((spix.d+epix.d)/2,hilight),xor);
  if((next&(0x10<<j))!=0 &&
   getscreencoords(w,c->p[j+4]->d.p,c->p[j==3?4:j+5]->d.p,&spix,&epix,
    testdist))
   plotline(spix.x,spix.y,epix.x,epix.y,
    WCOLORNUM((spix.d+epix.d)/2,hilight),xor);
  if((next&(0x100<<j))!=0 &&
   getscreencoords(w,c->p[j]->d.p,c->p[j+4]->d.p,&spix,&epix,testdist))
   plotline(spix.x,spix.y,epix.x,epix.y,
    WCOLORNUM((spix.d+epix.d)/2,hilight),xor);
  }
 if(withlockedsides)
  for(j=0;j<6;j++)
   if(n->d.c->walls[j]!=NULL && n->d.c->walls[j]->locked)
    in_plottagwall(w,n->d.c,j,n->d.c->tagged_walls[j]!=NULL ? 4 : 2,xor); 
 }

void plotcube(struct node *n,int hilight,int xor,int withalllines,
 int withlockedsides) 
 { makeview(0); in_plotcube(0,n,hilight,0,xor,withalllines,withlockedsides);
   if(view.whichdisplay)
    { makeview(1); in_plotcube(1,n,hilight,0,xor,withalllines,
       withlockedsides); } }

/* highlight==0: normal thingcolor 
   highlight>0: view.color[HILIGHTCOLORS+highlight-1]
   highlight<0: same as highlight>0 with xor.
   highlight==256: black
   highlight==257: normal thingcolor or black if too far away */ 
void in_plotthing(int w,struct thing *t,int hilight)
 {
 int i;
 struct pixel spix,epix;
 if(t->tagged && hilight==0) hilight=3;
 for(i=1;i<(t->type1==7 ? 9 : 11);i+=2) 
  if(getscreencoords(w,&t->p[i],&t->p[i+1],&spix,&epix,
   hilight==0||hilight==257))
   plotline(spix.x,spix.y,epix.x,epix.y,
    TCOLORNUM(t->color,ANALYZE_HILIGHT(hilight)),hilight<0);
  else if(hilight==257)
   if(getscreencoords(w,&t->p[i],&t->p[i+1],&spix,&epix,0))
    plotline(spix.x,spix.y,epix.x,epix.y,
      TCOLORNUM(t->color,ANALYZE_HILIGHT(256)),0);
 }

void plotthing(struct thing *t,int hilight)
 { makeview(0); in_plotthing(0,t,hilight);
   if(view.whichdisplay) { makeview(1); in_plotthing(1,t,hilight); } }

/* hilight==0: normal
   hilight>0: view.color[HILIGHTCOLORS+hilight-1] 
   hilight==2: current side
   hilight==-1: undraw (same as normal but draw unconditional)
   hilight==256: black */
void in_plotwall(int w,struct cube *c,int wno,int hilight,int xor)
 {
 struct pixel spix,epix;
 int j;
 for(j=0;j<4;j++)
  if(getscreencoords(w,c->p[wallpts[wno][j]]->d.p,
   c->p[wallpts[wno][(j+1)&3]]->d.p,&spix,&epix,hilight==0))
   plotline(spix.x,spix.y,epix.x,epix.y,
    WCOLORNUM((spix.d+epix.d)/2.0,
    ((j==0||j==3) && hilight==2) ? (j==0 ? 4 : 5) : hilight),xor);
 }  
   
void plotwall(struct cube *c,int wno,int hilight,int xor)
 { makeview(0); in_plotwall(0,c,wno,hilight,xor);
   if(view.whichdisplay) { makeview(1); in_plotwall(1,c,wno,hilight,xor); } }

void in_plottagwall(int w,struct cube *c,int wallno,int hilight,int xor)
 {
 struct point p1,p2,p3,p4,p5,p6,d1,d2,d3;
 int i,j;
 for(i=0;i<3;i++)
  {
  p1.x[i]=d1.x[i]=0;
  for(j=0;j<4;j++)
   {
   p1.x[i]+=c->p[wallpts[wallno][j]]->d.p->x[i];
   d1.x[i]+=c->p[wallpts[oppwalls[wallno]][j]]->d.p->x[i]-
    c->p[wallpts[wallno][j]]->d.p->x[i];
   }
  p1.x[i]/=4.0;
  d2.x[i]=c->p[wallpts[wallno][0]]->d.p->x[i]-
   c->p[wallpts[wallno][1]]->d.p->x[i]+
   c->p[wallpts[wallno][3]]->d.p->x[i]-
   c->p[wallpts[wallno][2]]->d.p->x[i];
  d3.x[i]=c->p[wallpts[wallno][0]]->d.p->x[i]-
   c->p[wallpts[wallno][3]]->d.p->x[i]+
   c->p[wallpts[wallno][1]]->d.p->x[i]-
   c->p[wallpts[wallno][2]]->d.p->x[i];
  }
 normalize(&d1); normalize(&d2); normalize(&d3);
 for(i=0;i<3;i++)
  {
  p2.x[i]=p1.x[i]+d2.x[i]*view.tsize;
  p3.x[i]=p1.x[i]-d2.x[i]*view.tsize;
  p4.x[i]=p1.x[i]+d3.x[i]*view.tsize;
  p5.x[i]=p1.x[i]-d3.x[i]*view.tsize;
  p6.x[i]=p1.x[i]+d1.x[i]*view.tsize;
  }
 in_plot3dline(w,&p1,&p6,WCOLORNUM(0.0,hilight),xor,0);
 in_plot3dline(w,&p2,&p3,WCOLORNUM(0.0,hilight),xor,0);
 in_plot3dline(w,&p4,&p5,WCOLORNUM(0.0,hilight),xor,0);
 }
 
void plottagwall(struct cube *c,int wno,int hilight,int xor)
 { makeview(0); in_plottagwall(0,c,wno,hilight,xor);
   if(view.whichdisplay)
    { makeview(1); in_plottagwall(1,c,wno,hilight,xor); } }

struct node *oldpcurrcube,*oldpcurrthing,*oldpcurrdoor,*oldpcurrpnt;
int oldcurrwall,oldcurredge,killoldmacro;

void in_plotfirstmacrocube(int w)
 {
 struct point naxis[3],*offset,*eoffset,eaxis[3],hp1,hp2,*op[8];
 static struct point np[8];
 static int marker[6];
 int i,j;
 if(killoldmacro)
  {
  struct cube c;
  struct node n,pnode[8];
  n.d.c=&c;
  for(j=0;j<8;j++)
   { c.p[j]=&pnode[j]; pnode[j].d.p=&np[j]; }
  in_plotcube(w,&n,2,0,1,1,0);
  for(j=0;j<6;j++)
   if(marker[j]) in_plottagwall(w,&c,j,2,1);
  killoldmacro=0;
  }
 if(view.pcurrmacro==NULL || view.pcurrmacro->exitcube==NULL ||
  view.pcurrmacro->exitwall<0 ||
  view.pcurrmacro->exitcube->d.c->nc[view.pcurrmacro->exitwall]!=NULL
  || view.pcurrcube->d.c->nc[view.currwall]!=NULL) return;
 offset=view.pcurrcube->d.c->p[wallpts[view.currwall][view.curredge]]->d.p;
 eoffset=view.pcurrmacro->exitcube->d.c->p[
  wallpts[view.pcurrmacro->exitwall][0]]->d.p;
 getcubecoords(view.pcurrmacro->exitcube->d.c,view.pcurrmacro->exitwall,0,
  eaxis,0);
 getcubecoords(view.pcurrcube->d.c,view.currwall,view.curredge,naxis,1);
 for(j=0;j<8;j++)
  {
  for(i=0;i<3;i++)
   hp1.x[i]=view.pcurrmacro->exitcube->d.c->p[j]->d.p->x[i]-eoffset->x[i];
  INVMATRIXMULT(&hp2,eaxis,&hp1);
  MATRIXMULT(&np[j],naxis,&hp2);
  /* and move them */
  for(i=0;i<3;i++) np[j].x[i]+=offset->x[i];
  fittogrid(&np[j]);
  op[j]=view.pcurrmacro->exitcube->d.c->p[j]->d.p;
  view.pcurrmacro->exitcube->d.c->p[j]->d.p=&np[j];
  }
 in_plotcube(w,view.pcurrmacro->exitcube,2,0,1,1,0);
 for(j=0;j<6;j++)
  if((marker[j]=(view.pcurrmacro->exitcube->d.c->nc[j]!=NULL))!=0)
   in_plottagwall(w,view.pcurrmacro->exitcube->d.c,j,2,1);
 for(j=0;j<8;j++) view.pcurrmacro->exitcube->d.c->p[j]->d.p=op[j];
 killoldmacro=1;
 }

void in_plotcurrent(int w)
 {
 if(!view.pcurrcube || !l) return;
 if(oldpcurrdoor) in_plotdoor(w,oldpcurrdoor,-1,-1,0);
 if(oldpcurrthing) in_plotthing(w,oldpcurrthing->d.t,
  testtag(tt_thing,oldpcurrthing) ? 3 : 257);
 if(oldpcurrpnt) 
  in_plotmarker(w,oldpcurrpnt->d.p,testtag(tt_pnt,oldpcurrpnt) ? 3 : -1);
 if(oldpcurrcube) 
  { if(oldcurredge>=0) in_plotpnt(w,oldpcurrcube,oldcurrwall,oldcurredge,
     testtag(tt_edge,oldpcurrcube,oldcurrwall,oldcurredge) ? 3 : -1);
    in_plotcube(w,oldpcurrcube,testtag(tt_cube,oldpcurrcube) ? 3 : -1,
     1,0,1,1); }
 else
  if(oldcurredge>=0) 
   in_plotpnt(w,view.pcurrcube,oldcurrwall,oldcurredge,
    testtag(tt_edge,view.pcurrcube,oldcurrwall,oldcurredge) ? 3 : -1);
 if(view.pcurrthing) in_plotthing(w,view.pcurrthing->d.t,
  testtag(tt_thing,view.pcurrthing) ? 6 : 1);
 if(view.pcurrdoor) in_plotdoor(w,view.pcurrdoor,
  testtag(tt_door,view.pcurrdoor) ? 6 : 1,1,0);
 if(view.pcurrpnt) in_plotmarker(w,view.pcurrpnt->d.p,
  testtag(tt_pnt,view.pcurrpnt) ? 6 : -1);
 in_plotcube(w,view.pcurrcube,1,1,0,1,1);
 in_plotwall(w,view.pcurrcube->d.c,view.currwall,2,0);
 in_plotpnt(w,view.pcurrcube,view.currwall,view.curredge,
  testtag(tt_edge,view.pcurrcube,view.currwall,view.curredge) ? 6 : -1);
 if((view.drawwhat&DW_GROUPPREVIEW)!=0 && view.pcurrmacro!=NULL &&
  view.pcurrmacro!=l)
  in_plotfirstmacrocube(w);
 }

void plotcurrent(void)
 { 
 w_refreshstart(l->w); makeview(0); in_plotcurrent(0);
 if(view.whichdisplay) { makeview(1); in_plotcurrent(1); }
 copytoscreen();
 w_refreshend(l->w);
 oldpcurrthing=view.pcurrthing; oldpcurrdoor=view.pcurrdoor;
 oldpcurrcube=view.pcurrcube;  oldpcurrpnt=view.pcurrpnt;
 oldcurredge=view.curredge; oldcurrwall=view.currwall;
 }
 
/* plot coordinate axis at view.e0 */
void in_plotcoordaxis(int lr)
 {
 struct point sp,ep;
 int i;
 float size;
 if((view.coord_axis&(1<<l->whichdisplay))==0 || view.distcenter==0.0) return;
 size = 5.0*view.tsize*view.distcenter/6553600.0;
 sp=view.e0;
 if(!l->whichdisplay) for(i=0;i<3;i++) sp.x[i]+=er[2].x[i]*view.distcenter;
 ep=sp; ep.x[0]+=size; in_plot3dline(lr,&sp,&ep,TCOLORNUM(0,1),0,0);
 ep=sp; ep.x[1]+=size; in_plot3dline(lr,&sp,&ep,TCOLORNUM(0,2),0,0);
 ep=sp; ep.x[2]+=size; in_plot3dline(lr,&sp,&ep,TCOLORNUM(0,3),0,0);
 }
 
/* highlight==0: normal wallcolor 
   highlight>0: view.color[HILIGHTCOLORS+highlight-1]
   highlight<0: same as highlight>0 with xor.
   highlight==256: black
   highlight==257: normal wallcolor or black if too far away*/ 
void in_plotpnt(int w,struct node *c,int wn,int pn,int hilight)
 {
 struct point p1,p2;
 int i;
 for(i=0;i<3;i++)
  {
  p1.x[i]=c->d.c->p[wallpts[wn][(pn+1)&0x3]]->d.p->x[i]-
   c->d.c->p[wallpts[wn][pn]]->d.p->x[i];
  p2.x[i]=c->d.c->p[wallpts[wn][(pn-1)&0x3]]->d.p->x[i]-
   c->d.c->p[wallpts[wn][pn]]->d.p->x[i];
  }
 normalize(&p1); normalize(&p2);
 for(i=0;i<3;i++) p1.x[i]+=p2.x[i];
 normalize(&p1);
 for(i=0;i<3;i++)
  p1.x[i]=c->d.c->p[wallpts[wn][pn]]->d.p->x[i]+p1.x[i]*view.tsize*3;
 in_plot3dline(w,c->d.c->p[wallpts[wn][pn]]->d.p,&p1,
  WCOLORNUM(0.0,ANALYZE_HILIGHT(hilight)),hilight<0,hilight==0);
 }

void plotpnt(struct node *n,int w,int c,int hilight) 
 { makeview(0); in_plotpnt(0,n,w,c,hilight);
   if(view.whichdisplay) { makeview(1); in_plotpnt(1,n,w,c,hilight); } }
   
/* plot current level l. */
void plotlevel(void) { struct lightsource *ls; cont_plotlevel(&ls); }
unsigned long cont_plotlevel(struct lightsource **ls)
 {
 struct node *n;
 int lr,i;
 struct point d;
 long dt=-1;
 if(l==NULL || l->w==NULL || l->w->shrunk) return 0;
 w_refreshstart(l->w); 
 /* kill oldpicture */
 clearlevelwin();
 oldpcurrcube=oldpcurrthing=oldpcurrdoor=oldpcurrpnt=NULL;
 oldcurrwall=oldcurredge=-1; killoldmacro=0;
 for(lr=0;lr<=l->whichdisplay;lr++)
  {
  makeview(lr);
  if(lr)
   { plotline(w_xwininsize(l->w)/2,0,w_xwininsize(l->w)/2,
      w_ywininsize(l->w)-1,view.color[WHITE],0);
     plotline(w_xwininsize(l->w)/2-1,0,w_xwininsize(l->w)/2-1,
      w_ywininsize(l->w)-1,view.color[WHITE],0); }
  if(!l->whichdisplay && view.render>1 && l->inside)
   dt=render_level(lr,l,l->rendercube!=NULL ? l->rendercube :
    view.pcurrcube,view.drawwhat,0); 
  else
   {
   if((view.drawwhat&DW_CUBES)!=0 || (!l->inside && view.render>1))
    if((view.drawwhat&DW_ALLLINES)==0)
     for(n=l->lines.head;n->next!=NULL;n=n->next)
      {
      for(i=0;i<3;i++)
       d.x[i]=(n->d.l->s->d.p->x[i]+n->d.l->e->d.p->x[i])/2.0-x0.x[i];
      in_plot3dline(lr,n->d.l->s->d.p,n->d.l->e->d.p,
       WCOLORNUM(LENGTH(&d),n->d.l->color),0,1);
      }
    else
     { for(n=l->cubes.head;n->next!=NULL;n=n->next)
        if(n->d.c->tagged==NULL) in_plotcube(lr,n,0,0,0,0,1); }
   if(view.render>=1)
    dt=render_level(lr,l,view.pcurrcube,0,view.render==1?1:0);
   if((view.drawwhat&DW_THINGS)!=0)
    for(n=l->things.head;n->next!=NULL;n=n->next)
     if(!n->d.t->tagged) in_plotthing(lr,n->d.t,0);
   if((view.drawwhat&DW_DOORS)!=0)
    for(n=l->doors.head;n->next!=NULL;n=n->next)
     if(!n->d.d->tagged) in_plotdoor(lr,n,0,0,0);
   if(l->exitcube)
    { in_plottagwall(lr,l->exitcube->d.c,l->exitwall,2,0);
      in_plotmarker(lr,l->exitcube->d.c->p[wallpts[l->exitwall][0]]->d.p,2); }
   if(l->cur_corr) in_plotcorridor(lr,l->cur_corr);
   }   
  /* now plot tagged things */
  for(n=l->cubes.head;n->next!=NULL;n=n->next)
   if(n->d.c->tagged!=NULL) in_plotcube(lr,n,3,0,0,1,1); 
  if((view.drawwhat&DW_XTAGGED)!=0)
   for(n=l->tagged[tt_cube].head;n->next!=NULL;n=n->next)
    for(i=0;i<6;i++) in_plottagwall(lr,n->d.n->d.c,i,3,0);
  for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
   in_plottagwall(lr,n->d.n->d.c,n->no%6,n->d.n->d.c->walls[n->no%6]!=NULL && 
    n->d.n->d.c->walls[n->no%6]->locked ? 4 : 3,0);
  for(n=l->tagged[tt_pnt].head;n->next!=NULL;n=n->next)
   in_plotmarker(lr,n->d.n->d.p,3);
  for(n=l->tagged[tt_thing].head;n->next!=NULL;n=n->next)
   in_plotthing(lr,n->d.n->d.t,0);
  for(n=l->tagged[tt_door].head;n->next!=NULL;n=n->next)
   in_plotdoor(lr,n->d.n,3,0,0);
  for(n=l->tagged[tt_edge].head;n->next!=NULL;n=n->next)
   in_plotpnt(lr,n->d.n,(n->no%24)/4,(n->no%24)%4,3);
  in_plotcurrent(lr);  
  in_plotcoordaxis(lr);
  }
 copytoscreen();
 oldpcurrthing=view.pcurrthing; oldpcurrdoor=view.pcurrdoor;
 oldpcurrcube=view.pcurrcube; oldpcurrpnt=view.pcurrpnt;
 oldcurredge=view.curredge; oldcurrwall=view.currwall;
 w_refreshend(l->w);
 for(n=l->lightsources.head,*ls=NULL;n->next!=NULL;n=n->next)
  if(n->d.ls->fl && n->d.ls->fl->calculated)
   {
   n->d.ls->fl->calculated=0;
   if(dt>=0 && dt>n->d.ls->fl->delay && (*ls==NULL ||
    n->d.ls->fl->delay>(*ls)->fl->delay))
    *ls=n->d.ls;
   }
 return *ls==NULL ? 0 : dt;
 }
 
/* #include "plotfill.c" */

void addline(struct node *c,int s,int e)
 {
 struct node *sp,*ep;
 struct line *li;
 struct node *n;
 if(c->d.c->p[s]<c->d.c->p[e]) { sp=c->d.c->p[s]; ep=c->d.c->p[e]; }
 else { sp=c->d.c->p[e]; ep=c->d.c->p[s]; }
 for(n=l->lines.tail;n->prev!=NULL;n=n->prev)
  if(n->d.l->s->no==sp->no && n->d.l->e->no==ep->no) return; 
 checkmem(li=malloc(sizeof(struct line)));
 addnode(&l->lines,c->no,li); 
 li->s=sp; li->e=ep; li->color=0;
 }
 
void initdescmap(void)
 {
 struct node *n;
 int i,j,k,a,cn1,wno;
 struct wall *w;
 struct point t,c0,c1,c2;
 float x1,x2;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 freelist(&l->lines,free);
 for(n=l->cubes.head;n->next!=NULL;n=n->next)
  for(i=0;i<6;i++)
   if(n->d.c->walls[i]!=NULL)
    {
    for(j=0;j<4;j++)
     {
/*     cn1=findnbcubetoline(i,j,(j+1)&3); */
     cn1=nb_sides[i][j];
     if(n->d.c->nc[cn1]==NULL) addline(n,wallpts[i][j],wallpts[i][(j+1)&3]);
     else
      { /* ok, wall is there, neighbour cube is also there. Let's have a
          look if there's a wall in the neighbourcube */
      wno=findnbwalltoline(n,n->d.c->nc[cn1]->d.c,i,j,(j+1)&3);
      if(wno==-1 || (w=n->d.c->nc[cn1]->d.c->walls[wno])==NULL)
       { addline(n,wallpts[i][j],wallpts[i][(j+1)&3]); continue; }
      /* Calculate angle between walls */
      for(k=0;k<3;k++)
       { 
       c1.x[k]=0; c2.x[k]=0;
       for(a=0;a<4;a++)
        {
	c1.x[k]+=n->d.c->walls[i]->p[a]->d.p->x[k]; 
        c2.x[k]+=w->p[a]->d.p->x[k]; 
	}
       c0.x[k]=(n->d.c->walls[i]->p[j]->d.p->x[k]+
        n->d.c->walls[i]->p[(j+1)&3]->d.p->x[k])/2.0;
       t.x[k]=(n->d.c->walls[i]->p[j]->d.p->x[k]-
        n->d.c->walls[i]->p[(j+1)&3]->d.p->x[k]);
       c1.x[k]=c1.x[k]/4.0-c0.x[k]; c2.x[k]=c2.x[k]/4.0-c0.x[k];
       }
      /* make both vectors orthgonal to c0 */
      normalize(&t); x1=SCALAR(&c1,&t); x2=SCALAR(&c2,&t);
      for(k=0;k<3;k++)
       {
       c1.x[k]-=x1*t.x[k];
       c2.x[k]-=x2*t.x[k];
       }
      normalize(&c1); normalize(&c2);
      if(SCALAR(&c1,&c2)>view.mapangle) 
       addline(n,wallpts[i][j],wallpts[i][(j+1)&3]);
      }
     }
    }
 }

void in_plotcorridor(int lr,struct corridor *c)
 {
 struct node *n;
 struct point *plot_pnts=NULL,p[11];
 int i,num_pts=0;
 float size;
 for(n=c->cubes.head;n->next!=NULL;n=n->next) in_plotcube(lr,n,2,0,1,0,0);
 for(n=c->tracking.head;n->next!=NULL;n=n->next)
  if(n->d.ct->fixed<=0)
   { 
   size=n->d.ct->l_c2/10;
   if(size<view.tsize*2) size=view.tsize*2;
   else if(size>view.tsize*20) size=view.tsize*20;
   switch(c->b_stdform->d.ls->selected)
    {
    case 0: make_o_marker(&n->d.ct->x,n->d.ct->coords,size,p); 
     num_pts=10; plot_pnts=&p[1]; break;
    case 1: case 2: makemarker(&n->d.ct->x,plot_pnts=p); num_pts=8; break;
    }
   for(i=0;i<num_pts;i+=2) 
    in_plot3dline(lr,&plot_pnts[i],&plot_pnts[i+1],
     view.color[HILIGHTCOLORS+4],1,0);
   }
 }
 
void plotcorridor(struct corridor *c)
 { 
 my_assert(c!=NULL);
 w_refreshstart(c->ld->w); makeview(0); in_plotcorridor(0,c);
 if(view.whichdisplay) { makeview(1); in_plotcorridor(1,c); }
 copytoscreen();
 w_refreshend(c->ld->w);
 }


void dec_frames(int ec)
 {
 struct ws_bitmap *scr;
 clock_t t1;
 int n1,n2;
 ws_erasemouse();
 checkmem(scr=ws_savebitmap(NULL,0,0,init.xres,init.yres));
 ws_drawfilledbox(0,0,init.xres,init.yres,0,0);
 makeview(-1); 
 render_level(0,l,view.pcurrcube,0,0); 
 psys_copytoscreen(0,0,0,0,init.xres,init.yres); 
 n1=n2=0; t1=clock();
 while((clock()-t1)<CLOCKS_PER_SEC*10) 
  { render_level(0,l,view.pcurrcube,0,0); n1++; }
/* t1=clock();
 while((clock()-t1)<CLOCKS_PER_SEC*10) 
  { render_level(0,view.pcurrcube,0,0); 
    sys_copytoscreen(0,0,drawbuffer); n2++; } */
 ws_restorebitmap(scr);
 ws_displaymouse();
 waitmsg("Frames per sec: without BitBlt %g with BitBlt %g\n",
  n1/10.0,n2/10.0);
 }

#define SAFE_DIST 131072.0

/* this function must be in this file because it uses the polygonstructure.
 cur_e0 = the offset of the left window in the moment and when the function
  returns this is the offset where the window is after we were moved.
 wanted_e0 = the offset of the left window where it should be after this
  routine were executed (if we are not hitting a wall) */
void move_render(struct point *wanted_e0,struct point *cur_e0)
 {
 struct point m,a,b,c,d,n,o,new_m;
 int i,w,j,pp,min_w=-1;
 float det,r,s,t,min_t=1.0;
 struct polygon *p;
 if(!l->inside || !l->rendercube) 
  { *cur_e0=*wanted_e0;
    if(view.render>1) init_rendercube(); 
    return; }
 for(i=0;i<3;i++) m.x[i]=wanted_e0->x[i]-cur_e0->x[i];
 new_m=m;
 for(w=0;w<6;w++) for(j=0;j<2;j++) 
 if((p=l->rendercube->d.c->polygons[w*2+j])!=NULL)
  {
  for(i=0;i<3;i++) 
   { a.x[i]=p->pnts[0].p_3d->x[i];
     c.x[i]=p->pnts[1].p_3d->x[i]-a.x[i];
     d.x[i]=cur_e0->x[i]-a.x[i]; }
  VECTOR(&o,&d,&m);
  for(pp=2;pp<p->num_pnts;pp++)
   {
   b=c; for(i=0;i<3;i++) c.x[i]=p->pnts[pp].p_3d->x[i]-a.x[i];
   VECTOR(&n,&b,&c); det=-SCALAR(&n,&m);
   if(det>0.0)
    {
    det=1.0/det; r=SCALAR(&o,&c)*det;
    if(r<0.0 || r>1.0) continue;
    s=-SCALAR(&o,&b)*det;
    if(s<0.0 || r+s>1.0) continue;
    t=SCALAR(&n,&d)*det;
    /* if this a wall, we should be a bit larger than zero radius */
    if(t<0.0) continue;
    if(l->rendercube->d.c->nc[w]==NULL && view.render==3)
     if(fabs(t)<LENGTH(&n)*det*SAFE_DIST) t=-LENGTH(&n)*det*SAFE_DIST;
     else t-=LENGTH(&n)*det*SAFE_DIST; 
    if(t>min_t) continue;
    /* this is the next collision */
    min_t=t; min_w=w;
    if(l->rendercube->d.c->nc[w]==NULL)
     /* in the next step we only want to use the part of m*(1.0-t) parallel
        to the wall */
     for(i=0;i<3;i++) 
      new_m.x[i]=(m.x[i]-n.x[i]*SCALAR(&n,&m)/SCALAR(&n,&n))*(1.0-t); 
    }
   }
  }
 if(min_w>=0) /* there is a collision with a wall during this step */
  {
  if(l->rendercube->d.c->nc[min_w]!=NULL) /* a neighbour */
   { for(i=0;i<3;i++) cur_e0->x[i]+=m.x[i]*min_t;
     l->rendercube=l->rendercube->d.c->nc[min_w];
     move_render(wanted_e0,cur_e0); /*start again from the new cube*/}
  else if(view.render==3) /* no neighbour and we should stop */
   { 
   for(i=0;i<3;i++) 
    { cur_e0->x[i]+=m.x[i]*min_t;
      wanted_e0->x[i]=cur_e0->x[i]+new_m.x[i]; }
   /* and start again from the new position in the same cube in a new 
      direction. But only if the remaining step is large enough to be
      no rubbish */  
   if(LENGTH(&new_m)>65536.0) move_render(wanted_e0,cur_e0);
   } /* no neighbour and we can move as free as a (klingon war)bird */
  else { *cur_e0=*wanted_e0; l->inside=0; }
  }
 else /* no collision with a wall, just finish the step */
  *cur_e0=*wanted_e0;
 }

void move_user(struct point *new_e0)
 {
 struct point cur_pos,new_pos;
 int i;
 if(view.render<2) { view.e0=*new_e0; return; }
 for(i=0;i<3;i++) if(new_e0->x[i]!=view.e0.x[i]) break;
 if(i==3) return;
 new_pos=*new_e0; cur_pos=view.e0;
 move_render(&new_pos,&cur_pos); view.e0=cur_pos;
 }
