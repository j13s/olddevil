/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    click.c - clicking in the level window
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
#include "tools.h"
#include "plot.h"
#include "in_plot.h"
#include "tag.h"
#include "userio.h"
#include "grfx.h"
#include "options.h"
#include "do_move.h"
#include "click.h"

struct clickhit
 { int x,y,d; struct node *data; int wall; struct point p; };
 
/* x0 is the start of the beam, v the direction. sx,sy are the coords of
   the mouseclick in the window w */ 
void init_beam(struct point *x0,struct point *v,struct w_window *w,
 int sx,int sy)
 {
 int xs,k;
 struct point e0,er[3];
 float x,y;
 xs=w_xwininsize(l->w);
 if(l->whichdisplay) 
  { xs/=2;
    if(sx>=xs) { initcoordsystem(1,&e0,er,x0); sx-=xs; }
    else initcoordsystem(0,&e0,er,x0); }
 else initcoordsystem(0,&e0,er,x0);
 x=(float)sx-xs/2.0;
 y=(float)-sy+w_ywininsize(l->w)/2.0;
 for(k=0;k<3;k++) v->x[k]=e0.x[k]+x*er[0].x[k]+y*er[1].x[k]-x0->x[k];
 }
 
/* return the factor (between 0 and 1) for the point p clicked with the beam
   a,m (|m|=1). If the point is not valid at all, a 10 is returned */
#define T_FACTOR 0.3
#define PHI_FACTOR 0.7
float checkpnt(struct point *a,struct point *m,struct point *p)
 {
 float t,phi;
 int i;
 struct point d;
 for(i=0;i<3;i++) d.x[i]=p->x[i]-a->x[i];
 t=SCALAR(m,&d); /* t=LENGTH(t*m); */
 if(t<view.minclicksodist || t>view.maxvisibility*1.2) return 10.0;
 phi=t/LENGTH(&d);
/* if(phi<view.clickphi) return 10.0;
 t/=view.maxvisibility*1.2; phi=(1-phi)/(1-view.clickphi); */
 t=t*t; phi=phi*phi;
 return t*T_FACTOR+phi*PHI_FACTOR;
 }

/* same as checkpnt but no ifs for returning 10.0 */
#define EP_T_FACTOR 0.2
#define EP_PHI_FACTOR 0.8
float checkeverypnt(struct point *a,struct point *m,struct point *p)
 {
 float t,phi;
 int i;
 struct point d;
 for(i=0;i<3;i++) d.x[i]=p->x[i]-a->x[i];
 t=SCALAR(m,&d); /* t=LENGTH(t*m); */
 if(t<view.minclicksodist) return 10.0;
 phi=t/LENGTH(&d);
/* t/=view.maxvisibility; phi=acos(phi)/acos(view.clickphi);*/
 t=t*t*t; phi=phi*phi;
 return t*EP_T_FACTOR+phi*EP_PHI_FACTOR;
 }
 
#define NUM_CLICKLIST 5
#define PIX_CLICKLIST 2

/* draw little circles around the hits. If slr==-1 it is single persp.,
 slr==0 double left, slr==1 double right */
/* check if point p is in the clicking circle around x,y (window coord. 
 but with 0,0 in the middle of the window) */
struct clickhit *checkclick(struct point *p,int clickradius,
 int x,int y,struct node *data,int wall)
 {
 struct pixel pix;
 struct clickhit *ch;
 float d;
 if(!in_getpixelcoords(p,&pix) || 
  (pix.d>view.maxvisibility*1.1 && wall!=7)) return NULL;
 if((d=(float)(x-pix.x)*(x-pix.x)+(float)(y-pix.y)*(y-pix.y))<=
  clickradius*clickradius)
  { checkmem(ch=MALLOC(sizeof(struct clickhit)));
    ch->x=pix.x; ch->y=pix.y; ch->d=d; ch->p=*p; ch->data=data; 
    ch->wall=wall; }
 else ch=NULL;
 return ch;
 }

#define MAX_CLICKHITS 3
 
void add_clickhit(struct clickhit **nearest,struct clickhit *ch)
 {
 int i,j;
 for(i=0;i<MAX_CLICKHITS;i++)
  if(!nearest[i] || nearest[i]->d>ch->d)
   { if(nearest[MAX_CLICKHITS-1]) FREE(nearest[MAX_CLICKHITS-1]);
     for(j=MAX_CLICKHITS-1;j>i;j--) nearest[j]=nearest[j-1]; nearest[i]=ch;
     break; }
 }
 
void run_thingloop(struct clickhit **hits,int clickradius,int x,int y)
 {
 struct node *n;
 struct clickhit *ch;
 for(n=l->things.head;n->next!=NULL;n=n->next)
  if((ch=checkclick(&n->d.t->p[0],clickradius,x,y,n,-1))!=NULL)
   add_clickhit(hits,ch);
 }
 
void run_doorloop(struct clickhit **hits,int clickradius,int x,int y)
 {
 struct node *n;
 struct clickhit *ch;
 for(n=l->doors.head;n->next!=NULL;n=n->next)
  if((ch=checkclick(&n->d.d->p,clickradius,x,y,n,-1))!=NULL)
   add_clickhit(hits,ch);
 }
 
void run_pntloop(struct clickhit **hits,int clickradius,int x,int y)
 {
 struct node *n;
 struct clickhit *ch;
 for(n=l->pts.head;n->next!=NULL;n=n->next)
  if((ch=checkclick(n->d.p,clickradius,x,y,n,-1))!=NULL)
   add_clickhit(hits,ch);
 }
 
void run_wallloop(struct clickhit **hits,int clickradius,int x,int y)
 {
 struct node *n;
 struct clickhit *ch;
 int w,i,j;
 struct point p,d1,d2,nv;
 for(n=l->cubes.head;n->next!=NULL;n=n->next)
  for(w=0;w<6;w++)
   {
   for(i=0;i<3;i++)
    { p.x[i]=0.0;
      for(j=0;j<4;j++) p.x[i]+=n->d.c->p[wallpts[w][j]]->d.p->x[i]/4.0; }
   if(n->d.c->nc[w]) 
    {
    for(i=0;i<3;i++)
     { d1.x[i]=n->d.c->p[wallpts[w][2]]->d.p->x[i]-
        n->d.c->p[wallpts[w][0]]->d.p->x[i];
       d2.x[i]=n->d.c->p[wallpts[w][3]]->d.p->x[i]-
        n->d.c->p[wallpts[w][1]]->d.p->x[i]; }
    VECTOR(&nv,&d1,&d2); normalize(&nv);
    for(i=0;i<3;i++) p.x[i]+=nv.x[i]*view.dsize;
    }
   if((ch=checkclick(&p,clickradius,x,y,n,w))!=NULL)
    add_clickhit(hits,ch);
   }
 }

void run_edgeloop(struct clickhit **hits,int clickradius,int x,int y)
 {
 struct node *n;
 struct clickhit *ch;
 int w,e,i,j;
 struct point p,c;
 for(n=l->cubes.head;n->next!=NULL;n=n->next)
  for(w=0;w<6;w++) if(!n->d.c->nc[w] || n->d.c->nc[w]->no>n->no) 
   {
   for(i=0;i<3;i++)
    { c.x[i]=0.0;
      for(j=0;j<4;j++) c.x[i]+=n->d.c->p[wallpts[w][j]]->d.p->x[i]/4.0; }
   for(e=0;e<4;e++)
    { 
    for(i=0;i<3;i++)
     p.x[i]=c.x[i]*0.4+n->d.c->p[wallpts[w][e]]->d.p->x[i]*0.6;
    if((ch=checkclick(&p,clickradius,x,y,n,w*4+e+100))!=NULL)
     add_clickhit(hits,ch); 
    }
   }
 }

void run_cubeloop(struct clickhit **hits,int clickradius,int x,int y)
 {
 struct node *n;
 struct clickhit *ch;
 int i,j;
 struct point p;
 for(n=l->cubes.head;n->next!=NULL;n=n->next)
  {
  for(i=0;i<3;i++)
   { p.x[i]=0.0;
     for(j=0;j<8;j++) p.x[i]+=n->d.c->p[j]->d.p->x[i]/8.0; }
  if((ch=checkclick(&p,clickradius,x,y,n,6))!=NULL)
   add_clickhit(hits,ch);
  }
 }

#define MINI_CLICKRADIUS (clickradius/4)
static int xoffset,yoffset,xsize,ysize,lr,clickradius; 

void plotclickmarker(int lr,struct clickhit *ch,int color,int xor)
 {
 int i;
 struct pixel spix,epix;
 ws_drawcircle(ch->x,ch->y,MINI_CLICKRADIUS,view.color[color],xor);
 if(ch->wall==6)  /* cube */
  {
  for(i=0;i<8;i++)
   if(getscreencoords(lr,&ch->p,ch->data->d.c->p[i]->d.p,&spix,&epix,0))
    ws_drawline(w_xwinincoord(l->w,spix.x),w_ywinincoord(l->w,spix.y),
     w_xwinincoord(l->w,epix.x),w_ywinincoord(l->w,epix.y),
     view.color[color],xor);
  }
 else if(ch->wall>=0 && ch->wall<6) /* wall */
  {
  for(i=0;i<4;i++)
   if(getscreencoords(lr,&ch->p,ch->data->d.c->p[wallpts[ch->wall][i]]->d.p,
    &spix,&epix,0))
    ws_drawline(w_xwinincoord(l->w,spix.x),w_ywinincoord(l->w,spix.y),
     w_xwinincoord(l->w,epix.x),w_ywinincoord(l->w,epix.y),
     view.color[color],xor);
  }
 else if(ch->wall>=100) /* edge */
  for(i=0;i<4;i++) if(i!=(((ch->wall-100)%4+2)&3))
   if(getscreencoords(lr,&ch->p,
    ch->data->d.c->p[wallpts[(ch->wall-100)/4][i]]->d.p,&spix,&epix,0))
    ws_drawline(w_xwinincoord(l->w,spix.x),w_ywinincoord(l->w,spix.y),
     w_xwinincoord(l->w,epix.x),w_ywinincoord(l->w,epix.y),
     view.color[color],xor);
 }
 
void (*run_loop[tt_number])(struct clickhit **hits,int cr,int x,
 int y)={ run_cubeloop,run_wallloop,run_edgeloop,run_pntloop,run_thingloop,
  run_doorloop };
 
struct track *check_for_corr(struct w_window *w,int sx,int sy)
 {
 float min_f,f;
 struct point a,m;
 struct node *t,*min_t;
 init_beam(&a,&m,w,sx,sy); normalize(&m);
 min_f=10.0; min_t=NULL;
 if(l->cur_corr!=NULL)
  { for(t=l->cur_corr->tracking.head;t->next!=NULL;t=t->next)
     if(t->d.ct->fixed<=0 && (f=checkeverypnt(&a,&m,&t->d.ct->x))<min_f)
      { min_f=f; min_t=t; } }
 return min_t==NULL ? NULL : min_t->d.ct;
 }

void scan_setclipping(struct w_window *w,int onoff)
 { 
 if(!onoff) ws_setclipping(-1,-1,-1,-1);
 else
  if(!l->whichdisplay)
   ws_setclipping(w_xwinincoord(w,0),w_ywinincoord(w,0),
    w_xwinincoord(w,w_xwininsize(w)),w_ywinincoord(w,w_ywininsize(w))); 
  else if(lr)
   ws_setclipping(w_xwinincoord(w,w_xwininsize(w)/2),w_ywinincoord(w,0),
    w_xwinincoord(w,w_xwininsize(w)),w_ywinincoord(w,w_ywininsize(w))); 
  else
   ws_setclipping(w_xwinincoord(w,0),w_ywinincoord(w,0),
    w_xwinincoord(w,w_xwininsize(w)/2),w_ywinincoord(w,w_ywininsize(w))); 
 }
 
void start_scansequence(struct w_window *w,int wx)
 {
 yoffset=w_ywininsize(w)/2; ysize=w_ywininsize(w);
 if(!l->whichdisplay)
  { xoffset=w_xwininsize(w)/2; xsize=w_xwininsize(w); makeview(lr=0); }
 else 
  {
  xsize=w_xwininsize(w)/2; 
  if(wx>w_xwininsize(w)/2) 
   { xoffset=3*w_xwininsize(w)/4; makeview(lr=1); }
  else
   { xoffset=w_xwininsize(w)/4; makeview(lr=0); }
  }
 clickradius=(w_xwininsize(w)+w_ywininsize(w))/2.0/20.0;
 if(clickradius<view.clickradius) clickradius=view.clickradius;
 }
 
struct clickhit *newpos_scansequence(struct w_window *w,int wx,int wy,
 void (*run_loop)(struct clickhit **nearest,int cr,int x,int y),
 struct clickhit **nearest)
 {
 int i;
 struct node *t;
 struct clickhit *ch;
 for(i=0;i<MAX_CLICKHITS;i++) nearest[i]=NULL;
 if(l->cur_corr!=NULL)
  { for(t=l->cur_corr->tracking.head;t->next!=NULL;t=t->next)
     if(t->d.ct->fixed<=0 && (ch=checkclick(&t->d.ct->x,
      clickradius,wx-xoffset,yoffset-wy,t,7))!=NULL)
      add_clickhit(nearest,ch); }
 run_loop(nearest,clickradius,wx-xoffset,yoffset-wy);
 for(i=0;i<MAX_CLICKHITS && nearest[i]!=NULL;i++)
  {
  nearest[i]->y=w_ywinincoord(l->w,yoffset-nearest[i]->y);
  nearest[i]->x=w_xwinincoord(l->w,xoffset+nearest[i]->x);
  }
 return nearest[0];
 }
 
void click_in_level(struct w_window *w,struct w_event *we)
 {
 struct clickhit *first,*nearest[MAX_CLICKHITS];
 struct node *ctrl_pressed;
 struct ws_event ws;
 int i,right_pressed=0,t;
 int x=we->ws->x,y=we->ws->y,wx=we->x,wy=we->y;
 /* First check if this should be a tagging with the mousebox */
 if(we->ws->kbstat&ws_ks_ctrl)
  { mousetagbox(l,we,(we->ws->buttons&ws_bt_left)==ws_bt_left);   
    plotlevel(); drawopt(view.currmode); return; }
 if(we->ws->flags==ws_f_rbutton && we->ws->buttons==ws_bt_right)
  { moveyou_mouse(w,we->x,we->y); plotlevel(); drawopt(view.currmode); 
    return; }
 if(we->ws->buttons!=ws_bt_left) return;
 /* OK, left button is pressed. Now run the scan sequence. Moving the
  white circle and hilighting all objects of the current mode under the
  circle until the button is released again */
 my_assert(view.currmode<tt_number);
 start_scansequence(w,we->x); nearest[0]=NULL; ctrl_pressed=NULL;
 scan_setclipping(w,1); t=0; ws=*we->ws;
 do
  {
  first=newpos_scansequence(w,wx,wy,run_loop[view.currmode],nearest);
  if(t==0 && (ws.kbstat&ws_ks_ctrl)!=0)
   ctrl_pressed=nearest[0] ? nearest[0]->data : NULL;
  if((ws.kbstat&ws_ks_ctrl)==0 && t==1 && nearest[0] && 
   nearest[0]->data==ctrl_pressed && nearest[0]->wall!=7 /* no corr */)
   {
   ctrl_pressed=NULL;
   switch(view.currmode)
    {
    case tt_cube: case tt_thing: case tt_door: case tt_pnt:
     switch_tag(view.currmode,nearest[0]->data); break;
    case tt_wall: switch_tag(tt_wall,nearest[0]->data,nearest[0]->wall);break;
    case tt_edge: switch_tag(tt_edge,nearest[0]->data,
     (nearest[0]->wall-100)/4,(nearest[0]->wall-100)%4); break;
    default: my_assert(0);
    }    
   scan_setclipping(w,0); plotlevel(); drawopt(view.currmode);
   scan_setclipping(w,1);
   }
  t=((ws.kbstat&ws_ks_ctrl)!=0);
  for(i=0;i<MAX_CLICKHITS && nearest[i]!=NULL;i++)
   plotclickmarker(lr,nearest[i],i==0 ? HILIGHTCOLORS : HILIGHTCOLORS+3,1);
  ws_drawcircle(x,y,clickradius,view.color[t ? HILIGHTCOLORS+2 : WHITE],1);
  do ws_getevent(&ws,0); while(ws.x==x && ws.y==y && ws.flags==ws_f_none
   && t==((ws.kbstat&ws_ks_ctrl)!=0));
  ws_drawcircle(x,y,clickradius,view.color[t ? HILIGHTCOLORS+2 : WHITE],1);
  for(i=0;i<MAX_CLICKHITS && nearest[i]!=NULL;i++)
   plotclickmarker(lr,nearest[i],i==0 ? HILIGHTCOLORS : HILIGHTCOLORS+3,1);
  if((ws.buttons&ws_bt_right)!=0 && nearest[0]!=NULL) /* move or rotate */
   {
   right_pressed=1;
   scan_setclipping(w,0);
   if(nearest[0]->wall==7)
    movecorr_mouse(w,wx,wy,nearest[0]->data->d.ct);
   else
    moveobj_mouse(((ws.kbstat&ws_ks_ctrl)!=0 ? 1 : 0)|
     ((ws.kbstat&ws_ks_alt)!=0 ? 2 : 0),w_xscreencoord(w,ws.x),
     w_yscreencoord(w,ws.y),nearest[0]->data,nearest[0]->wall);
   plotlevel();
   if(nearest[0]->wall!=7)
    if(view.currmode==tt_thing) drawopt(in_thing);
    else if(view.currmode==tt_cube || view.currmode==tt_wall || 
     view.currmode==tt_pnt)
     { drawopt(in_wall); drawopt(in_edge); drawopt(in_pnt); }
   return;
   }
  if(ws.buttons!=ws_bt_none)
   {
   wx=w_xscreencoord(w,ws.x); wy=w_yscreencoord(w,ws.y);
   if(wx<(lr ? xsize : 0)) wx=(lr ? xsize : 0); 
   else if(wx>(lr ? xsize*2 : xsize)) wx=(lr ? xsize*2 : xsize)-1;
   if(wy<0) wy=0; else if(wy>ysize) wy=ysize-1;
   x=w_xwinincoord(w,wx); y=w_ywinincoord(w,wy);
   if(x!=ws.x || y!=ws.y) ws_mousewarp(x,y);
   }
  }
 while(ws.buttons!=ws_bt_none);
 scan_setclipping(w,0);
 if(nearest[0]==NULL || nearest[0]->wall==7) return;
 /* OK. The user has released the button. If the right button wasn't pressed
  then click the current thing */
 switch(view.currmode)
  {
  case tt_cube:
   view.pcurrcube=nearest[0]->data; view.currwall=0; view.curredge=0; 
   printmsg(TXT_CLICKEDCUBE,view.pcurrcube->no);
   drawopt(in_wall); drawopt(in_edge);
   break;
  case tt_wall: 
   view.pcurrcube=nearest[0]->data; view.currwall=nearest[0]->wall;
   view.curredge=0; 
   printmsg(TXT_CLICKEDWALL,view.pcurrcube->no,view.currwall);
   drawopt(in_edge);
   break;
  case tt_edge: 
   view.pcurrcube=nearest[0]->data; view.currwall=(nearest[0]->wall-100)/4;
   view.curredge=(nearest[0]->wall-100)%4;
   printmsg(TXT_CLICKEDEDGE,view.pcurrcube->no,view.currwall,view.curredge);
   break;
  case tt_pnt: 
   view.pcurrpnt=nearest[0]->data;
   printmsg(TXT_CLICKEDPNT,view.pcurrpnt->no);
   break;
  case tt_thing:
   view.pcurrthing=nearest[0]->data; 
   printmsg(TXT_CLICKEDTHING,view.pcurrthing->no);
   break;
  case tt_door:
   view.pcurrdoor=nearest[0]->data;
   printmsg(TXT_CLICKEDDOOR,view.pcurrdoor->no);
   break;
  default: my_assert(0);
  } 
 view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
 for(i=0;i<MAX_CLICKHITS-1 && nearest[i]!=NULL;i++)
  FREE(nearest[i]);
 drawopt(view.currmode); plotlevel();
 }
