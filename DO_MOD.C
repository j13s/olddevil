/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_mod.c - functions handling an event for modifying existing objects.
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
#include "userio.h"
#include "tools.h"
#include "tag.h"
#include "insert.h"
#include "calctxt.h"
#include "plot.h"
#include "options.h"
#include "do_event.h"
#include "click.h"
#include "do_move.h"
#include "do_mod.h"

void dec_enlargeshrink(int ec) 
 { 
 int cubepnts[9],i;
 struct node *n;
 if(view.currmode!=tt_cube && view.currmode!=tt_wall)
  { printmsg(TXT_NOSHRINKMODE,init.bnames[view.currmode]); return; }
 if(l->tagged[view.currmode].size==0)
  { printmsg(TXT_NOTHINGTAGGED,init.bnames[view.currmode]); return; }
 for(n=l->tagged[view.currmode].head;n->next!=NULL;n=n->next)
  {
  l->levelsaved=0;
  if(view.currmode==tt_cube)
   { cubepnts[0]=8; for(i=0;i<8;i++) cubepnts[i+1]=i; }
  else
   { cubepnts[0]=4; for(i=0;i<4;i++) cubepnts[i+1]=wallpts[n->no%6][i]; }
  growshrink(n->d.n->d.c->p,cubepnts,ec==ec_enlarge);
  }
 plotlevel(); drawopt(in_pnt);
 }

void dec_setexit(int ec)
 { 
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 if(view.pcurrcube->d.c->nc[view.currwall]!=NULL)
  { printmsg(TXT_EXITNONEIGHBOUR); return; }
 l->levelsaved=0;
 l->exitcube=view.pcurrcube; l->exitwall=view.currwall; 
 plotlevel();
 }

void dec_calctxts(int ec) 
 {
 struct node *n;
 int i;
 l->levelsaved=0;
 switch(view.currmode)
  {
  case tt_cube:
   for(n=l->tagged[tt_cube].head;n->next!=NULL;n=n->next)
    for(i=0;i<8;i++)  newcubecorners(n->d.n,i);
   break;
  case tt_wall:
   for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
    if(n->d.n->d.c->walls[n->no%6])
     recalcwall(n->d.n->d.c,n->no%6);
   break;
  case tt_pnt:
   for(n=l->tagged[tt_pnt].head;n->next!=NULL;n=n->next) newcorners(n->d.n);
   break;
  default: printmsg(TXT_NOTXTCALC,init.bnames[view.currmode]);
  }
 plotlevel(); drawopt(in_wall); 
 }

void dec_aligntxts(int ec) 
 { 
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 if(!view.pcurrwall) { printmsg(TXT_NOCURRSIDE); return; }
 if(l->tagged[tt_wall].size==0) 
  { printmsg(TXT_NOTHINGTAGGED,init.bnames[tt_wall]); return; }
 l->levelsaved=0;
 arrangebitmaps(view.pcurrcube,view.pcurrwall);
 plotlevel(); drawopt(in_wall);
 }

void ed_initdatastr(int ds,char *offset,char *str)
 {
 unsigned char *memdata;
 char *pos;
 long offnum;
 if(ds>4) ds+=2; /* Internal and Leveldata */
 memdata=getdata(ds,NULL); offnum=strtol(offset,&pos,0);
 *pos=0; 
 if(!memdata) { strcpy(str,""); return; }
 sprintf(str,"%d",memdata[offnum]);
 } 
 
void ed_changestruct(struct w_button *b)
 { 
 struct w_button **bs=b->data;
 strcpy(bs[1]->d.str->str,"0");
 ed_initdatastr(b->d.ls->selected,bs[1]->d.str->str,bs[2]->d.str->str);
 w_drawbutton(bs[1]); w_drawbutton(bs[2]);
 }
 
void ed_offsetentered(struct w_button *b)
 { 
 struct w_button **bs=b->data;
 ed_initdatastr(bs[0]->d.ls->selected,b->d.str->str,bs[2]->d.str->str);
 w_drawbutton(bs[2]);
 }
 
void ed_dataentered(struct w_button *b)
 { 
 struct w_button **bs=b->data;
 unsigned char *memdata;
 long offnum,data;
 int ds=bs[0]->d.ls->selected;
 if(ds>4) ds+=2; /* Internal and Leveldata */
 memdata=getdata(ds,NULL); offnum=strtol(bs[1]->d.str->str,NULL,0);
 data=strtol(b->d.str->str,NULL,0);
 if(memdata) memdata[offnum]=data&0xff;
 ed_initdatastr(bs[0]->d.ls->selected,bs[1]->d.str->str,b->d.str->str);
 }
 
const char *structnames[7]={ "Cube","Side","Corner","Thing","Door","Switch",
 "Producer" };
void dec_enterdata(int ec)
 {
 struct w_window w,*ow;
 struct w_button *b[3];
 struct w_b_choose whichstruct;
 struct w_b_string offset,data;
 char offsetstr[4],datastr[4];
 struct w_keycode exitkey;
 static int save_struct=0,save_offset=0;
 w.xpos=w.ypos=-1; w.xsize=160; w.ysize=60;
 w.maxxsize=w.maxysize=-1;
 w.shrunk=0; w.title="Enter data"; w.hotkey=0; w.help=NULL;
 w.buttons=wb_close|wb_drag; w.refresh=wr_normal; w.data=NULL;
 w.refresh_routine=NULL; w.close_routine=NULL; w.click_routine=NULL;
 checkmem(ow=w_openwindow(&w));
 whichstruct.d_xsize=-1; whichstruct.num_options=7;
 whichstruct.options=structnames; whichstruct.selected=save_struct;
 whichstruct.select_lroutine=whichstruct.select_rroutine=ed_changestruct;
 checkmem(b[0]=w_addstdbutton(ow,w_b_choose,0,0,w_xwininsize(ow),
  w_ywininsize(ow)/2,"Structure",&whichstruct,1));
 offset.d_xsize=-1; offset.max_length=3; offset.str=offsetstr;
 offset.allowed_char=isdigit; 
 offset.l_char_entered=offset.r_char_entered=NULL;
 offset.l_string_entered=offset.r_string_entered=ed_offsetentered;
 sprintf(offsetstr,"%d",save_offset);
 data=offset; data.str=datastr; 
 data.l_string_entered=data.r_string_entered=ed_dataentered;
 ed_initdatastr(whichstruct.selected,offsetstr,datastr);
 checkmem(b[1]=w_addstdbutton(ow,w_b_string,b[0]->xpos,b[0]->ypos+b[0]->ysize,
  w_xwininsize(ow)/2,w_ywininsize(ow)-b[0]->ypos-b[0]->ysize,"Offset",&offset,
  1));
 checkmem(b[2]=w_addstdbutton(ow,w_b_string,b[1]->xpos+b[1]->xsize,
  b[0]->ypos+b[0]->ysize,w_xwininsize(ow)-b[1]->xpos-b[1]->xsize,
  w_ywininsize(ow)-b[0]->ypos-b[0]->ysize,"Data",&data,1));
 b[0]->data=b[1]->data=b[2]->data=b;
 exitkey.kbstat=0; exitkey.key=27; exitkey.event=-1; exitkey.flags=w_kf_exit;
 w_handleuser(0,NULL,1,&ow,1,&exitkey,NULL);
 save_struct=b[0]->d.ls->selected;
 save_offset=strtol(offsetstr,NULL,0);
 w_closewindow(ow); drawopts(); plotlevel();
 }

/* Routines for generating a corridor */ 
/* number of cubes at the start */
#define CORRIDOR_STDLENGTH 10
/* number of parts in recalc_corridor for each cube */
#define CORRIDOR_FINENESS 4
/* maximum number of coefficients needed in recalc_track */
#define COEFF_NUM 4
#define MIN_CUBEDEPTH 1
#define MAX_CUBEDEPTH 100
#define CIRCLE_FACTOR (2/M_PI)

/* insert a new corridorelement starting with cube (first==1 for
 oppwalls[c->front_wall], 0 for c->front_wall) */
void add_correlem(struct corridor *c,struct node *cube,int first)
 {
 int i,j,p,wp;
 struct corridorelement *ce;
 struct node *n,*n2;
 checkmem(ce=MALLOC(sizeof(struct corridorelement)));
 checkmem(addnode(&c->elements,-1,ce));
 checkmem(ce->pnts=MALLOC(sizeof(struct point *)*c->num_pnts));
 checkmem(ce->rel_pnts=MALLOC(sizeof(struct point)*c->num_pnts));
 for(j=0;j<3;j++) ce->center.x[j]=0.0;
 checkmem(ce->cubes=MALLOC(sizeof(struct node *)*c->num_cubes));
 for(i=0,n=cube,p=0;i<c->num_cubes && n->next!=NULL;i++,n=n->next) 
  {
  ce->cubes[i]=n;
  for(wp=0;wp<4;wp++)
   { 
   for(n2=n->d.c->p[wallpts[first ? oppwalls[c->front_wall[i]] :
    c->front_wall[i]][wp]]->d.lp->c.head;
    n2->next!=NULL;n2=n2->next)
    if(n2->d.n->no<n->no) break;
   if(n2->next==NULL)
    { my_assert(p<c->num_pnts);
      ce->pnts[p]=n->d.c->p[wallpts[first ? oppwalls[c->front_wall[i]] :
       c->front_wall[i]][wp]]->d.p;
      for(j=0;j<3;j++) ce->center.x[j]+=ce->pnts[p]->x[j];
      p++; }
   }
  }
 for(j=0;j<3;j++) ce->center.x[j]/=c->num_pnts;
 }

/* make coordsystem for tangential vector m with twist
 using ref_coords as reference coordsystem (last coords). Store
 the result in coords and the new reference coords (same
 as the coords without twist) in ref_coords. */
void init_ce_coords(struct point *m,float twist,struct point *ref_coords,
 struct point *coords)
 {
 int i;
 float l,a,b,q,r,s;
 struct point e[2];
 coords[2]=*m; normalize(&coords[2]); 
 /* construct two arbitrary vectors orthogonal to m (a righthanded 
    coordsystem)*/
 VECTOR(&e[0],&ref_coords[1],&coords[2]);
 l=LENGTH(&e[0]);
 if(l>=0.1) for(i=0;i<3;i++) e[0].x[i]/=l;
 else { VECTOR(&e[0],&ref_coords[0],&coords[2]); normalize(&e[0]); }
 VECTOR(&e[1],&coords[2],&e[0]);
 /* now maximize the function c[0]*rc[0]+c[1]*rc[1] with
    c[0]=r*e[0]+s*e[1], c[1]=-s*e[0]+r*e[1]  and r^2+s^2=1 */
 a=SCALAR(&e[0],&ref_coords[0])+SCALAR(&e[1],&ref_coords[1]);
 b=SCALAR(&e[1],&ref_coords[0])-SCALAR(&e[0],&ref_coords[1]);
 q=sqrt(a*a+b*b);
 if(q>=0.001) { r=a/q; s=b/q; }
 else /* e[0]Xe[1] is (anti)parallel to rc[0]Xrc[1] */
  { r=1.0; s=0.0; }
 for(i=0;i<3;i++)
  { coords[0].x[i]=r*e[0].x[i]+s*e[1].x[i];
    coords[1].x[i]=-s*e[0].x[i]+r*e[1].x[i]; }
 ref_coords[0]=coords[0]; ref_coords[1]=coords[1];
 turnsinglepnt(&coords[2],cos(twist),sin(twist),&coords[0]);
 turnsinglepnt(&coords[2],cos(twist),sin(twist),&coords[1]);
 }
 
/* initialize the relative pnts in the corridorelement ce corridor c.
 Use coords as coordsystem */
void init_ce_relpnts(struct corridor *c,struct corridorelement *ce,
 struct point *coords)
 {
 struct point p;
 int i,j;
 for(i=0;i<c->num_pnts;i++)
  { for(j=0;j<3;j++) p.x[j]=ce->pnts[i]->x[j]-ce->center.x[j];
    INVMATRIXMULT(&ce->rel_pnts[i],coords,&p); }
 }
 
int connect_cube_layer(struct corridor *c,struct node *start)
 {
 struct node *n,*n2;
 int i,j,k;
 w_display_msgs(0);
 for(n=start,i=0;n->next!=NULL && i<c->num_cubes;n=n->next,i++)
  for(j=0;j<6;j++)
   if(c->connected[i*6+j]>=0 && n->d.c->nc[j]==NULL)
    { 
    for(n2=start,k=0;n2->next!=NULL && k<c->connected[i*6+j]/6;n2=n2->next,
     k++);
    if(n2->next && n2->d.c->nc[c->connected[i*6+j]%6]==NULL)
     if(!connectcubes(&c->points,n,j,n2,c->connected[i*6+j]%6)) return 0;
    }
 w_display_msgs(1);
 return 1;
 }
 
/* Add a new layer of cubes to corridor c. */
int new_cube_layer(struct corridor *c)
 {
 struct node *fcube,*n;
 int i;
 my_assert(c->elements.tail->next!=NULL);
 checkmem(fcube=insertcube(&c->cubes,&c->points,
  c->elements.tail->d.ce->cubes[0],c->front_wall[0],c->depth));
 for(i=1;i<c->num_cubes;i++)
  checkmem(insertcube(&c->cubes,&c->points,c->elements.tail->d.ce->cubes[i],
   c->front_wall[i],c->depth));
 if(!connect_cube_layer(c,fcube))
  {
  for(n=fcube;n->next!=NULL;n=n->next) deletecube(&c->cubes,&c->points,n);
  return 0;
  }
 add_correlem(c,fcube,0);
 return 1;
 }
  
/* recalculate the corridor element n in corridor c. New coords are a,
  coords. prev_coords are the coords of the previous corridor element. */
struct node *recalc_correlem(struct corridor *c,struct node *n,
 struct point *a,struct point *coords,struct point *prev_coords)
 {
 int i,j;
 if(n->next==NULL)
  { if(!new_cube_layer(c)) return NULL; n=c->elements.tail;
    init_ce_relpnts(c,n->d.ce,prev_coords); }
 n->d.ce->center=*a;
 for(i=0;i<c->num_pnts;i++)
  { MATRIXMULT(n->d.ce->pnts[i],coords,&n->d.ce->rel_pnts[i]);
    for(j=0;j<3;j++) n->d.ce->pnts[i]->x[j]+=n->d.ce->center.x[j]; }
 return n->next;
 }

void kill_corr_elem(struct corridor *c,struct node *n)
 { 
 int i,j;
 FREE(n->d.ce->pnts);
 for(i=0;i<c->num_cubes;i++) 
  {
  /* to avoid a recalculation of some textures coords which will cause
     a crash if the cube was an illegal one */
  for(j=0;j<6;j++)
   if(n->d.ce->cubes[i]->d.c->walls[j])
    n->d.ce->cubes[i]->d.c->walls[j]->locked=1; 
  deletecube(&c->cubes,&c->points,n->d.ce->cubes[i]);
  }
 FREE(n->d.ce->cubes); FREE(n->d.ce); killnode(&c->elements,n);
 } 
   
void corr_get_pnt(struct point *ci,float t,struct point *p)
 { int i;
   for(i=0;i<3;i++) 
    p->x[i]=ci[0].x[i]+ci[1].x[i]*t+ci[2].x[i]*t*t+ci[3].x[i]*t*t*t; }

void corr_get_gradient(struct point *ci,float t,struct point *p)
 { int i;
   for(i=0;i<3;i++) p->x[i]=ci[1].x[i]+2*ci[2].x[i]*t+3*ci[3].x[i]*t*t; }

void corr_init_coeff(struct point *ci,struct track *st,struct track *et)
 {
 int i;
 ci[0]=st->x; 
 /* This is the solution for a polynom of 3rd grade through two pts */
 for(i=0;i<3;i++)
  { 
  ci[1].x[i]=st->coords[2].x[i]*st->l_c2; 
  ci[2].x[i]=3*et->x.x[i]-3*st->x.x[i]-2*st->coords[2].x[i]*st->l_c2-
   et->coords[2].x[i]*et->l_c2;
  ci[3].x[i]=-2*et->x.x[i]+2*st->x.x[i]+st->coords[2].x[i]*st->l_c2+
   et->coords[2].x[i]*et->l_c2;
  }
 }

/* return the twistvalue for the track from predecessor of t to t. ref_coords
 are the coords[0] and [1] of the corridor element of t calculating
 out of the predecessor without twist */
float corr_get_twist(struct track *t,struct point *ref_coords)
 {
 struct point coords[3],n;
 float twist;
 coords[0]=ref_coords[0]; coords[1]=ref_coords[1];
 init_ce_coords(&t->coords[2],0.0,coords,coords);
 VECTOR(&n,&coords[0],&t->coords[0]);
 if(fabs(twist=SCALAR(&n,&t->coords[2]))<0.0001) twist=0;
 else 
  twist=twist<0 ? -acos(SCALAR(&t->coords[0],&coords[0])) :
   +acos(SCALAR(&t->coords[0],&coords[0]));
 while(twist-t->old_twist<-M_PI) twist+=2*M_PI;
 while(twist-t->old_twist>M_PI) twist-=2*M_PI;
 t->old_twist=twist;
 return twist;
 }
 
/* recalculate the complete track from st to et starting with 
 corridorelement start. if the result is not correct, return a NULL
 (the old coords are not restored), otherwise a pointer to the last corridor 
 element concerning to this track. */
struct node *recalc_track(struct corridor *c,struct track *st,
 struct track *et,struct node *start)
 {
 struct point ci[COEFF_NUM],dx1,ref_coords[2],*cp,old_coords[3];
 int i,num_parts,count;
 float s,last_s,t,dt,d_twist,twist,overall_twist;
 struct node *n,*tn;
 struct list curve;
 for(i=0;i<3;i++) dx1.x[i]=et->x.x[i]-st->x.x[i];
 if(LENGTH(&dx1)<2*c->depth) return 0;
 corr_init_coeff(ci,st,et);
 /* OK. Now we divide the curve in oldlength/depth*CORR_FINENESS parts 
   and step through it */
 num_parts=et->length/c->depth*CORRIDOR_FINENESS+0.5;
 if(num_parts<CORRIDOR_FINENESS*2) return 0;
 dt=1.0/num_parts;
 /* First make the coordsystems untwisted */
 initlist(&curve);
 ref_coords[0]=st->coords[0]; ref_coords[1]=st->coords[1];
 /* t is the parameter of the curve, s is the length */
 for(t=0.0,s=last_s=0.0,count=0,n=NULL;t<=1.0;t+=dt)
  { 
  corr_get_gradient(ci,t,&dx1);
  s+=sqrt(SCALAR(&dx1,&dx1))*dt;
  count++;
  if(s-last_s>=c->depth)
   { checkmem(cp=MALLOC(sizeof(struct point)*4));
     checkmem(n=addnode(&curve,-1,cp));
     init_ce_coords(&dx1,0.0,ref_coords,&cp[1]);
     corr_get_pnt(ci,t,&cp[0]); n->no=0;
     if(count<CORRIDOR_FINENESS) { dt/=2; n->no=1; }
     else if(count>CORRIDOR_FINENESS*2) { dt*=2; n->no=-1; }
     last_s=s; count=0; }
  }
 if(n==NULL) return NULL;
 et->length=s;
 if(s-last_s<c->depth/2) 
  { n=n->prev; FREE(n->next->d.p); killnode(&curve,n->next); }
 /* OK, now calculate the corresponding twisted cubes */
 overall_twist=corr_get_twist(et,&n->d.p[1]);
 ref_coords[0]=st->coords[0]; ref_coords[1]=st->coords[1];
 for(i=0;i<3;i++) old_coords[i]=st->coords[i];
/* n=recalc_correlem(c,start,&st->x,st->coords,old_coords); */
 n=start->next;
 d_twist=overall_twist/(curve.size+1);
 for(tn=curve.head->next,twist=0.0;tn!=NULL;tn=tn->next,twist+=d_twist)
  {
  turn(&tn->prev->d.p[1],&tn->prev->d.p[1],0,1,2,twist);
  if((n=recalc_correlem(c,n,&tn->prev->d.p[0],&tn->prev->d.p[1],
   old_coords))==NULL) { start=NULL; break; }
  for(i=0;i<3;i++) old_coords[i]=tn->prev->d.p[1+i];
  if(tn->prev->no>0) d_twist/=2; else if(tn->prev->no<0) d_twist*=2;
  FREE(tn->prev->d.p); killnode(&curve,tn->prev);
  }
 if(start==NULL || (n=recalc_correlem(c,n,&et->x,et->coords,old_coords))==NULL)
  return NULL;
 for(tn=start;tn!=n && tn->next!=NULL;tn=tn->next)
  for(i=0;i<c->num_cubes;i++)
   if(!testcube(tn->d.ce->cubes[i],0)) return NULL; 
 return n->prev;
 }

int recalc_corridor(struct corridor *c)
 {
 struct node *tn,*cn=c->elements.head,*kn;
 struct point d;
 int i;
 my_assert(c->tracking.size>1); /* at least two points */
 for(tn=c->tracking.head->next;tn->next!=NULL;tn=tn->next)
  {
  for(i=0;i<3;i++) d.x[i]=tn->d.ct->x.x[i]-tn->prev->d.ct->x.x[i];
  if(LENGTH(&d)<3*c->depth) return 0;
  if((cn=recalc_track(c,tn->prev->d.ct,tn->d.ct,cn))==NULL) 
   return 0;
  }
 for(kn=cn->next->next;kn!=NULL;kn=kn->next) kill_corr_elem(c,kn->prev);
 return 1;
 }
 
int movecorr(struct point *add,union move_params *params)
 { 
 struct corridor *c;
 struct track *t,*mt,*ct[5],oldt[5];
 struct point d,r;
 float f;
 int i,ok=0;
 c=params->c.c;
 t=params->c.t;
 if(t->fixed==-1) return 0;
 plotcorridor(c);
 switch(c->b_stdform->d.ls->selected)
  {
  case 0: 
   oldt[0]=*t;
   for(i=0;i<3;i++) t->x.x[i]+=add->x[i]; 
   ok=recalc_corridor(c);
   if(!ok) { *t=oldt[0]; recalc_corridor(c); }
   break;
  case 1: 
   mt=t->modify_too;
   oldt[0]=*t; oldt[1]=*mt;
   for(i=0;i<3;i++)
    { t->x.x[i]+=add->x[i]; d.x[i]=t->x.x[i]-mt->x.x[i]; }
   t->l_c2=mt->l_c2=LENGTH(&d);
   for(i=0;i<3;i++) t->coords[2].x[i]=mt->coords[2].x[i]=d.x[i]/t->l_c2;
   init_ce_coords(&t->coords[2],0.0,&t->coords[0],t->coords);
   init_ce_coords(&mt->coords[2],0.0,&mt->coords[0],mt->coords);
   ok=recalc_corridor(c);
   if(!ok) { *t=oldt[0]; *mt=oldt[1]; recalc_corridor(c); }
   break;
  case 2: 
   ct[0]=t;
   for(i=1;i<5;i++) ct[i]=ct[i-1]->modify_too;
   for(i=0;i<5;i++) oldt[i]=*ct[i];
   f=SCALAR(add,&t->coords[2]); 
   for(i=0;i<3;i++) r.x[i]=add->x[i]-f*t->coords[2].x[i]; 
   if(ct[4]->fixed) /* ->t=mt1 */
    {
    ct[1]->l_c2=ct[3]->l_c2=ct[4]->l_c2=ct[4]->l_c2+CIRCLE_FACTOR*
     (SCALAR(&r,&t->coords[1])<0 ? -LENGTH(&r) : LENGTH(&r));
    for(i=0;i<3;i++) { ct[0]->x.x[i]+=r.x[i]; ct[2]->x.x[i]-=r.x[i]; }
    }
   else /* ->t=mt2 */
    {
    ct[1]->l_c2=ct[4]->l_c2=ct[4]->l_c2+CIRCLE_FACTOR*
     (SCALAR(&r,&t->coords[1])<0 ? -LENGTH(&r) : LENGTH(&r));
    for(i=0;i<3;i++)
     { ct[0]->x.x[i]+=r.x[i];
       ct[1]->x.x[i]+=r.x[i]/2.0; ct[4]->x.x[i]+=r.x[i]/2.0; }
    }
   ok=recalc_corridor(c);
   if(!ok) { for(i=0;i<5;i++) *ct[i]=oldt[i]; recalc_corridor(c); }
   break;
  }
 plotcorridor(c);
 return ok;
 }
 
int turncorr(struct point *t_axis,float fx,float fy,int with_left,
 union move_params *params)
 {
 struct corridor *c;
 struct point oldx,coords[3];
 int ok,i,j,k;
 struct track *t;
 c=params->c.c; t=params->c.t;
 if(c->b_stdform->d.ls->selected!=0) return 0;
 plotcorridor(c);
 oldx=t->x; for(i=0;i<3;i++) coords[i]=t->coords[i];
 fx=fx>=M_PI ? fx-2*M_PI : (fx<=-M_PI ? fx+2*M_PI : fx);
 fy=fy>=M_PI ? fy-2*M_PI : (fy<=-M_PI ? fy+2*M_PI : fy);
 if(!with_left && fabs(fx)<fabs(fy)) 
  { t->l_c2+=fy/M_PI*c->depth;                         
    if(t->l_c2<c->depth) t->l_c2=c->depth; }
 else 
  {
  if(!with_left) { i=0; j=1; k=2; }
  else if(fabs(fx)>fabs(fy)) { i=2; j=0; k=1; } else { i=1; j=2; k=0; }
  turn(coords,t->coords,i,j,k,k==0 ? fy : fx);
  }
 ok=recalc_corridor(c);
 if(!ok)
  { t->x=oldx; for(i=0;i<3;i++) t->coords[i]=coords[i]; recalc_corridor(c); }
 plotcorridor(c);
 return ok;
 }

/* Changes the cube from the lists lc1,lp1 to lc2,lp2. All points
 must be unique and new. No doors allowed. */
void change_cube_list(struct node *n,struct list *lc1,struct list *lp1,
 struct list *lc2,struct list *lp2)
 {
 int i;
 unlistnode(lc1,n);
 listnode_tail(lc2,n);
 for(i=0;i<8;i++) 
  { unlistnode(lp1,n->d.c->p[i]); listnode_tail(lp2,n->d.c->p[i]); }
 }
 
void make_start_cubes(struct corridor *c,struct node *cube,int wall,
 struct point *a,struct point *nv)
 {
 struct node *n;
 int i,j,nbwall;
 struct point t;
 float f;
 struct wall w;
 my_assert(cube!=NULL && wall>=0 && wall<6);
 if(cube->d.c->nc[wall]!=NULL || cube->d.c->walls[wall]==NULL ||
  !testtag(tt_wall,cube,wall)) return;
 for(i=0;i<4;i++)
  { for(j=0;j<3;j++) t.x[j]=cube->d.c->p[wallpts[wall][i]]->d.p->x[j]-a->x[j];
    f=SCALAR(&t,nv);
    if(f!=0.0) 
     for(j=0;j<3;j++) cube->d.c->p[wallpts[wall][i]]->d.p->x[j]-=f*nv->x[j]; }
 my_assert(c->front_wall=REALLOC(c->front_wall,(++c->num_cubes)*sizeof(int)));
 c->front_wall[c->num_cubes-1]=wall;
 untag(tt_wall,cube,wall); tag(tt_cube,cube);
 w=*cube->d.c->walls[wall];
 checkmem(n=insertcube(NULL,NULL,cube,wall,c->depth));
 for(i=0;i<4;i++) insertpnt(n,wallpts[oppwalls[wall]][i]);
 cube->d.c->walls[wall]->texture1=w.texture1;
 cube->d.c->walls[wall]->texture2=w.texture2;
 for(i=0;i<4;i++) cube->d.c->walls[wall]->corners[i]=w.corners[i];
 change_cube_list(n,&l->cubes,&l->pts,&c->cubes,&c->points);  
 for(i=0;i<4;i++)
  if(cube->d.c->nc[nb_sides[wall][i]] &&
   (nbwall=findnbwalltoline(cube,cube->d.c->nc[nb_sides[wall][i]]->d.c,
   wall,i,(i+1)&3))!=-1)
   make_start_cubes(c,cube->d.c->nc[nb_sides[wall][i]],nbwall,a,nv);
 }

void init_connected(struct corridor *c)
 {
 int i,cubeno,nbwall;
 struct node *nc,*cube;
 my_assert(c->connected=MALLOC(c->num_cubes*6*sizeof(int)));
 sortlist(&l->tagged[tt_cube],0);
 my_assert(l->tagged[tt_cube].size==c->num_cubes);
 for(cube=l->tagged[tt_cube].head,cubeno=0;cube->next!=NULL && 
  cubeno<c->num_cubes;cubeno++,cube=cube->next)
  {
  for(i=0;i<6;i++)
   {
   c->connected[cubeno*6+i]=-1;
   if((nc=cube->d.n->d.c->nc[i])!=NULL && testtag(tt_cube,nc)) 
    {
    for(nbwall=0;nbwall<6;nbwall++) if(nc->d.c->nc[nbwall]==cube->d.n) break;
    my_assert(nbwall<6);
    c->connected[cubeno*6+i]=nc->d.c->tagged->no*6+nbwall; }
    }
  }
 }
 
int make_start_corridor(struct corridor *c)
 {
 int i;
 struct point a,m1,m2,nv;
 struct cube *cube=view.pcurrcube->d.c;
 untagall(tt_cube);
 a=*cube->p[wallpts[view.currwall][view.curredge]]->d.p;
 for(i=0;i<3;i++)
  {
  m1.x[i]=cube->p[wallpts[view.currwall][(view.curredge+1)&3]]->d.p->x[i]-
   a.x[i];
  m2.x[i]=cube->p[wallpts[view.currwall][(view.curredge-1)&3]]->d.p->x[i]-
   a.x[i]; 
  }
 normalize(&m1); normalize(&m2); VECTOR(&nv,&m1,&m2);
 make_start_cubes(c,view.pcurrcube,view.currwall,&a,&nv);
 init_connected(c);
 untagall(tt_cube);
 if(c->num_cubes==0) return 0;
 connect_cube_layer(c,c->cubes.head); 
 c->num_pnts=c->points.size/2;
 add_correlem(c,c->cubes.head,1); add_correlem(c,c->cubes.head,0);
 for(i=1;i<CORRIDOR_STDLENGTH;i++) new_cube_layer(c);
 return 1;
 }
 
void delete_corridor(struct corridor *c)
 {
 struct node *n;
 FREE(c->connected); FREE(c->front_wall);
 freelist(&c->cubes,freecube);
 freelist(&c->points,freelistpnt);
 for(n=c->elements.head->next;n!=NULL;n=n->next)
  { FREE(n->prev->d.ce->pnts); FREE(n->prev->d.ce->rel_pnts);
    FREE(n->prev->d.ce->cubes);  FREE(n->prev->d.ce); FREE(n->prev); }
 freelist(&c->tracking,free);
 FREE(c);
 }
 
void finish_corridor(struct corridor *c)
 {
 struct node *n,*np,**first_layer;
 int i;
 sortlist(&l->cubes,0); sortlist(&l->pts,0);
 checkmem(first_layer=MALLOC(sizeof(struct node *)*c->num_cubes));
 for(i=0,n=c->cubes.head;i<c->num_cubes;i++,n=n->next)
  { my_assert(n->next!=NULL); first_layer[i]=n; }
 for(n=c->cubes.head->next;n!=NULL;n=n->next)
  { unlistnode(&c->cubes,np=n->prev); listnode_tail(&l->cubes,np); }
 for(n=c->points.head->next;n!=NULL;n=n->next)
  { unlistnode(&c->points,np=n->prev); listnode_tail(&l->pts,np); }
 for(i=0;i<c->num_cubes;i++)
  connectsides(first_layer[i],oppwalls[c->front_wall[i]]);
 for(n=first_layer[0];n->next!=NULL;n=n->next)
  for(i=0;i<6;i++) if(n->d.c->walls[i]!=NULL) recalcwall(n->d.c,i);
 FREE(first_layer);
 delete_corridor(c);
 }

void close_corr_win(struct corridor *c)
 {
 w_deletebutton(c->b_ok); w_deletebutton(c->b_cancel);
 w_deletebutton(c->b_ins); w_deletebutton(c->b_del);
 w_deletebutton(c->b_setend);
 c->b_ok=c->b_cancel=c->b_ins=c->b_del=c->b_setend=NULL;
 if(view.whichdisplay)
  { corr_win2_xpos=w_xwinspacecoord(c->win->xpos);
    corr_win2_ypos=w_ywinspacecoord(c->win->ypos); }
 else
  { corr_win1_xpos=w_xwinspacecoord(c->win->xpos);
    corr_win1_ypos=w_ywinspacecoord(c->win->ypos); }
 w_closewindow(c->win); c->win=NULL;
 l->cur_corr=NULL; plotlevel(); 
 }

void closerout_corr_win(struct w_window *w,void *d)
 { 
 if(yesnomsg(TXT_FINISHCORRIDOR)) finish_corridor(l->cur_corr);
 else if(yesnomsg(TXT_DELETECORRIDOR)) delete_corridor(l->cur_corr);
 else return;
 close_corr_win(l->cur_corr);
 }
 
void corr_ok(struct w_button *b)
 { if(!yesnomsg(TXT_FINISHCORRIDOR)) return;
   finish_corridor(l->cur_corr); close_corr_win(l->cur_corr); }
 
void corr_cancel(struct w_button *b)
 { if(!yesnomsg(TXT_DELETECORRIDOR)) return;
   delete_corridor(l->cur_corr); close_corr_win(l->cur_corr); }

/* insdel=1: insert a new trackpnt,
   insdel=-1: delete one trackpnt */
void change_num_tp(int insdel)
 {
 struct track *new_tp,*st,*et;
 struct point ci[COEFF_NUM],ref_coords[2],dx,coords[3];
 struct node *n,*last;
 int num_parts;
 float t,length,s,last_s,dt;
 struct list new_track,old_track;
 initlist(&new_track);
 checkmem(new_tp=MALLOC(sizeof(struct track)));
 *new_tp=*l->cur_corr->tracking.head->d.ct;
 checkmem(last=addnode(&new_track,-1,new_tp));
 for(length=0.0,n=l->cur_corr->tracking.head->next;n->next!=NULL;n=n->next)
  length+=n->d.ct->length;
 for(s=last_s=0.0,n=l->cur_corr->tracking.head->next;n->next!=NULL;n=n->next)
  {
  st=n->prev->d.ct; et=n->d.ct;
  num_parts=et->length/l->cur_corr->depth*CORRIDOR_FINENESS+0.5;
  dt=1.0/num_parts; 
  corr_init_coeff(ci,st,et);
  ref_coords[0]=st->coords[0]; ref_coords[1]=st->coords[1];
  for(t=0.0;t<=1.0;t+=dt)
   {
   corr_get_gradient(ci,t,&dx);
   s+=sqrt(SCALAR(&dx,&dx))*dt;
   /* this must be done before the if because ref_coords are changed */
   init_ce_coords(&dx,0.0,ref_coords,coords);
   if(s-last_s>=length/(l->cur_corr->tracking.size-1+insdel)) 
    {
    checkmem(new_tp=MALLOC(sizeof(struct track)));
    checkmem(addnode(&new_track,-1,new_tp));
    new_tp->fixed=0; new_tp->length=s; new_tp->l_c2=LENGTH(&dx);
    new_tp->old_twist=et->old_twist*(s-st->length)/et->length;
    corr_get_pnt(ci,t,&new_tp->x);
    turn(coords,new_tp->coords,0,1,2,new_tp->old_twist);
    last_s=s;
    }
   }
  }
 if(new_track.size<=(l->cur_corr->tracking.size-1+insdel)) 
  { checkmem(new_tp=MALLOC(sizeof(struct track)));
    checkmem(addnode(&new_track,-1,new_tp));
    new_tp->fixed=0; }
 *new_track.tail->d.ct=*l->cur_corr->tracking.tail->d.ct;
 copylisthead(&old_track,&l->cur_corr->tracking);
 copylisthead(&l->cur_corr->tracking,&new_track);
 if(!recalc_corridor(l->cur_corr))
  { printmsg(TXT_CORRWEIRD); 
    copylisthead(&new_track,&l->cur_corr->tracking);
    copylisthead(&l->cur_corr->tracking,&old_track);
    recalc_corridor(l->cur_corr); freelist(&new_track,free); }
 else freelist(&old_track,free);
 plotlevel();
 }

void corr_ins_tp(struct w_button *b) { change_num_tp(1); } 
void corr_del_tp(struct w_button *b) 
 { if(l->cur_corr->tracking.size>2) change_num_tp(-1); } 

void corr_setend(struct w_button *b)
 {
 int i,j;
 struct track *end,oldend;
 struct point e1,e2;
 struct cube *c=view.pcurrcube->d.c;
 float f;
 my_assert(l!=NULL && l->cur_corr!=NULL);
 plotcorridor(l->cur_corr);
 end=l->cur_corr->tracking.tail->d.ct; end->old_twist=0.0;
 oldend=*end;
 for(j=0;j<3;j++)
  {
  end->x.x[j]=0.0; 
  for(i=0;i<4;i++) end->x.x[j]+=c->p[wallpts[view.currwall][i]]->d.p->x[j];
  end->x.x[j]/=4;
  e1.x[j]=c->p[wallpts[view.currwall][(view.curredge+1)&3]]->d.p->x[j]-
   c->p[wallpts[view.currwall][view.curredge]]->d.p->x[j];
  e2.x[j]=c->p[wallpts[view.currwall][(view.curredge-1)&3]]->d.p->x[j]-
   c->p[wallpts[view.currwall][view.curredge]]->d.p->x[j];
  }
 normalize(&e1); normalize(&e2); f=SCALAR(&e1,&e2);
 end->coords[0]=e1;
 for(i=0;i<3;i++) end->coords[1].x[i]=e2.x[i]-f*e1.x[i];
 normalize(&end->coords[1]);
 VECTOR(&end->coords[2],&end->coords[0],&end->coords[1]); 
 i=recalc_corridor(l->cur_corr);
 if(!i)
  { *end=oldend; recalc_corridor(l->cur_corr); printmsg(TXT_CORRWEIRD); }
 plotcorridor(l->cur_corr);
 }

void corr_form(struct w_button *b)
 { 
 struct corridor *c=b->data;
 struct node *n;
 struct track *st,*et,*mt1,*mt2,*mt3;
 int i,j;
 switch(b->d.ls->selected)
  {
  case 0: /* user defined */ 
   w_activatebutton(c->b_ins); w_drawbutton(c->b_ins);
   w_activatebutton(c->b_del); w_drawbutton(c->b_del);
   w_activatebutton(c->b_setend); w_drawbutton(c->b_setend);
   for(n=c->tracking.head->next;n->next!=NULL;n=n->next) n->d.ct->fixed=0;
   st=c->tracking.head->d.ct; et=c->tracking.tail->d.ct;
   st->fixed=-1;
   if(st->x.x[0]==et->x.x[0] && st->x.x[1]==et->x.x[1] &&
    st->x.x[2]==et->x.x[2]) 
    for(i=0;i<3;i++) et->x.x[i]-=et->coords[2].x[i]*MIN_CUBEDEPTH*65536.0;
   break;
  case 1: /* straight */
   w_undrawbutton(c->b_ins); w_deactivatebutton(c->b_ins);
   w_undrawbutton(c->b_del); w_deactivatebutton(c->b_del);
   w_undrawbutton(c->b_setend); w_deactivatebutton(c->b_setend);
   if(c->tracking.size>2)
    for(n=c->tracking.head->next->next;n->next!=NULL;n=n->next)
     { FREE(n->prev->d.ct); killnode(&c->tracking,n->prev); }
   st=c->tracking.head->d.ct; et=c->tracking.tail->d.ct;
   for(i=0;i<3;i++) st->coords[i]=et->coords[i]=c->e[i];
   st->l_c2=et->l_c2=et->length=CORRIDOR_STDLENGTH*c->depth;
   st->old_twist=et->old_twist=0.0; et->modify_too=st;
   for(i=0;i<3;i++) et->x.x[i]=st->x.x[i]+et->length*st->coords[2].x[i];
   st->fixed=1; et->fixed=0;
   break;
  case 2: /* circle */
   w_undrawbutton(c->b_ins); w_deactivatebutton(c->b_ins);
   w_undrawbutton(c->b_del); w_deactivatebutton(c->b_del);
   w_undrawbutton(c->b_setend); w_deactivatebutton(c->b_setend);
   if(c->tracking.size>2)
    for(n=c->tracking.head->next;n->next!=NULL;n=n->next)
     { FREE(n->prev->d.ct); killnode(&c->tracking,n->prev); }
   for(i=0;i<3;i++)
    { checkmem(st=MALLOC(sizeof(struct track)));
      checkmem(addnode(&c->tracking,-1,st)); }
   st=c->tracking.head->d.ct; et=c->tracking.tail->d.ct;
   mt1=c->tracking.head->next->d.ct; mt3=c->tracking.tail->prev->d.ct;
   mt2=c->tracking.head->next->next->d.ct;
   st->old_twist=et->old_twist=mt1->old_twist=mt2->old_twist=
    mt3->old_twist=0.0;
   st->fixed=et->fixed=mt3->fixed=1; mt1->fixed=mt2->fixed=0;
   st->l_c2=mt1->l_c2=mt2->l_c2=mt3->l_c2=et->l_c2=
    c->depth*CORRIDOR_STDLENGTH;
   st->length=0.0; mt1->length=M_PI/2*st->l_c2;
   mt2->length=mt1->length*2; mt3->length=mt1->length*3;
   et->length=mt1->length*4;
   mt1->modify_too=mt2; mt2->modify_too=mt3; mt3->modify_too=et;
   et->modify_too=st; st->modify_too=mt1;
   et->x=st->x;
   for(i=0;i<3;i++) 
    { 
    for(j=0;j<3;j++) et->coords[j].x[i]=st->coords[j].x[i];
    mt1->coords[0].x[i]=mt2->coords[0].x[i]=mt3->coords[0].x[i]=
     st->coords[0].x[i];
    mt1->coords[1].x[i]=st->coords[2].x[i];
    mt1->coords[2].x[i]=-st->coords[1].x[i];
    mt2->coords[1].x[i]=-st->coords[1].x[i];
    mt2->coords[2].x[i]=-st->coords[2].x[i];
    mt3->coords[1].x[i]=-st->coords[2].x[i];
    mt3->coords[2].x[i]=st->coords[1].x[i];
    mt1->x.x[i]=st->x.x[i]+
     (-st->coords[1].x[i]+st->coords[2].x[i])*st->l_c2*CIRCLE_FACTOR;
    mt2->x.x[i]=st->x.x[i]-st->coords[1].x[i]*st->l_c2*2*CIRCLE_FACTOR;
    mt3->x.x[i]=st->x.x[i]+
     (-st->coords[1].x[i]-st->coords[2].x[i])*st->l_c2*CIRCLE_FACTOR;
    }
   break;
  }
 recalc_corridor(c); plotlevel();
 }

void corr_depth(struct w_button *b)
 {
 float new_d;
 struct corridor *c=b->data;
 if(sscanf(b->d.str->str,"%g",&new_d)==1 && new_d>=MIN_CUBEDEPTH &&
  new_d<=MAX_CUBEDEPTH) c->depth=new_d*65536.0;
 sprintf(b->d.str->str,"%10.2f",c->depth/65536.0);
 w_drawbutton(b); recalc_corridor(b->data); plotlevel();
 }
 
int corr_win1_xpos=-1,corr_win1_ypos=-1; 
int corr_win2_xpos=-1,corr_win2_ypos=-1; 
struct w_window init_corr_win={ 0,400,100,150,107,-1,-1,0,NULL,0,NULL,
 wb_drag|wb_close|wb_shrink,wr_normal,NULL,NULL,closerout_corr_win,NULL };
#define NUM_STDFORMS 3
const char *stdforms_txt[NUM_STDFORMS]={ TXT_CUSTOM,TXT_STRAIGHT,TXT_CIRCLE };
void init_corr_window(struct corridor *c,const char *levname)
 {
 static struct w_b_press ok,cancel,ins,del,setend;
 static struct w_b_string depth;
 static struct w_b_choose stdform;
 char *win_tit;
 int xs;
 checkmem(win_tit=MALLOC(strlen(TXT_CORRWIN)+strlen(levname)+1));
 strcpy(win_tit,TXT_CORRWIN); strcat(win_tit,levname);
 init_corr_win.title=win_tit;
 if(view.whichdisplay) 
  if(corr_win2_xpos<0)
   { init_corr_win.xpos=0; 
     init_corr_win.ypos=w_ymaxwinsize()-init_corr_win.ysize; }
  else
   { init_corr_win.xpos=corr_win2_xpos; init_corr_win.ypos=corr_win2_ypos; }
 else
  if(corr_win1_xpos<0)
   { init_corr_win.xpos=w_xmaxwinsize()-init_corr_win.xsize;
     init_corr_win.ypos=0; }
  else
   { init_corr_win.xpos=corr_win1_xpos; init_corr_win.ypos=corr_win1_ypos; }
 checkmem(c->win=w_openwindow(&init_corr_win));
 xs=w_xwininsize(c->win);
 ok.delay=0; ok.repeat=-1; ok.l_pressed_routine=ok.r_pressed_routine=NULL;
 ok.l_routine=ok.r_routine=corr_ok;
 checkmem(c->b_ok=w_addstdbutton(c->win,w_b_press,0,0,xs/2,-1,TXT_OK,
  &ok,1));
 cancel=ok; cancel.l_routine=cancel.r_routine=corr_cancel;
 checkmem(c->b_cancel=w_addstdbutton(c->win,w_b_press,xs/2,0,xs-xs/2,-1,
  TXT_CANCEL,&cancel,1));
 stdform.d_xsize=-1; stdform.num_options=NUM_STDFORMS;
 stdform.options=stdforms_txt; stdform.selected=0;
 stdform.select_lroutine=stdform.select_rroutine=corr_form;
 checkmem(c->b_stdform=w_addstdbutton(c->win,w_b_choose,0,c->b_ok->ysize,
  xs,-1,TXT_STDFORM,&stdform,1));
 depth.d_xsize=-1; depth.max_length=20;
 checkmem(depth.str=MALLOC(depth.max_length+1));
 sprintf(depth.str,"%10.2f",c->depth/65536.0);
 depth.allowed_char=isfloat;
 depth.l_char_entered=depth.r_char_entered=NULL;
 depth.l_string_entered=depth.r_string_entered=corr_depth;
 checkmem(c->b_depth=w_addstdbutton(c->win,w_b_string,0,
  c->b_stdform->ypos+c->b_stdform->ysize,xs,-1,TXT_CORRCUBEDEPTH,&depth,1));
 ins=ok; ins.l_routine=ins.r_routine=corr_ins_tp;
 checkmem(c->b_ins=w_addstdbutton(c->win,w_b_press,0,
  c->b_depth->ypos+c->b_depth->ysize,xs/2,-1,TXT_INSERT,&ins,1));
 del=ok; del.l_routine=del.r_routine=corr_del_tp;
 checkmem(c->b_del=w_addstdbutton(c->win,w_b_press,xs/2,
  c->b_ins->ypos,xs-xs/2,-1,TXT_DELETE,&del,1));
 setend=ok; setend.l_routine=setend.r_routine=corr_setend;
 checkmem(c->b_setend=w_addstdbutton(c->win,w_b_press,0,
  c->b_del->ypos+c->b_del->ysize,xs,-1,TXT_CORRSETEND,&setend,1));
 c->b_depth->data=c; c->b_stdform->data=c;
/* w_resizeinwin(c->win,w_xwininsize(c->win),
  c->b_setend->ypos+c->b_setend->ysize);
 fprintf(errf,"ys:%d\n",c->win->ysize); */
 }
 
void dec_makecorridor(int ec)
 {
 int i,w;
 struct cube *cube;
 struct corridor *c;
 struct track *st,*et;
 struct node *n; 
 struct point ref_coords[2];
 if(!l || !view.pcurrcube) { printmsg(TXT_NOLEVEL); return; }
 if(view.render>1) { view.render=1; plotlevel(); drawopt(in_internal); }
 if(l->cur_corr!=NULL) { w_wintofront(l->cur_corr->win); return; }
 cube=view.pcurrcube->d.c; w=view.currwall; 
 if(cube->nc[w]) { printmsg(TXT_CUBETAGGEDON); return; }
 untagall(tt_cube); tag(tt_wall,view.pcurrcube,view.currwall);
 checkmem(c=MALLOC(sizeof(struct corridor)));
 initlist(&c->elements); initlist(&c->cubes); initlist(&c->points);
 c->connected=NULL; c->num_pnts=0; c->num_cubes=0; c->front_wall=NULL;
 c->startcube=view.pcurrcube; c->startwall=view.currwall;
 c->depth=20.0*65536.0;
 if(!make_start_corridor(c)) { FREE(c); return; }
 initlist(&c->tracking);
 checkmem(st=MALLOC(sizeof(struct track)));
 checkmem(et=MALLOC(sizeof(struct track)));
 st->fixed=-1; st->x=c->elements.head->d.ce->center; 
 et->fixed=0; et->x=c->elements.tail->d.ce->center;
 addnode(&c->tracking,-1,st); addnode(&c->tracking,-1,et);
 for(i=0;i<3;i++)
  { st->coords[2].x[i]=et->x.x[i]-st->x.x[i];
    st->coords[0].x[i]=c->elements.head->d.ce->pnts[1]->x[i]-
     c->elements.head->d.ce->pnts[0]->x[i]; }
 et->length=LENGTH(&st->coords[2]); 
 normalize(&st->coords[2]); et->coords[2]=st->coords[2]; st->length=0.0;
 st->l_c2=et->l_c2=et->length; normalize(&st->coords[0]);
 VECTOR(&st->coords[1],&st->coords[2],&st->coords[0]);
 for(i=0;i<3;i++) { c->e[i]=et->coords[i]=st->coords[i]; }
 ref_coords[0]=st->coords[0]; ref_coords[1]=st->coords[1];
 st->old_twist=et->old_twist=0.0;
 for(n=c->elements.head;n->next!=NULL;n=n->next) 
  init_ce_relpnts(c,n->d.ce,st->coords);
 init_corr_window(c,l->fullname); recalc_corridor(c);    
 l->cur_corr=c; c->ld=l; plotlevel();
 }

void dec_makeedgecoplanar(int ec)
 {
 struct node *n;
 struct point a,r,s,d,*p[4];
 float fr,fs;
 int i,j;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 for(n=l->tagged[tt_edge].head->next;n!=NULL;n=n->next)
  {
  for(i=0;i<4;i++)
   if(i!=(n->prev->no%24)%4 && testtag(tt_edge,n->prev->d.n,
    (n->prev->no%24)/4,i))
    break;
  if(i==4)
   { /* only one edge is tagged on this side. move it into the plane of the
        three others. */
   for(j=0;j<4;j++)
    p[j]=n->prev->d.n->d.c->p[
     wallpts[(n->prev->no%24)/4][(((n->prev->no%24)%4)+j)&3]]->d.p;
   a=*p[2];
   for(j=0;j<3;j++)
    { r.x[j]=p[3]->x[j]-a.x[j]; s.x[j]=p[1]->x[j]-a.x[j];
      d.x[j]=p[0]->x[j]-a.x[j]; }
   normalize(&r); normalize(&s);
   fr=SCALAR(&r,&d); fs=SCALAR(&s,&d);
   for(j=0;j<3;j++) p[0]->x[j]=fr*r.x[j]+fs*s.x[j]+a.x[j];
   untag(tt_edge,n->prev->d.n,(n->no%24)/4,(n->no%24)%4);
   recalcwall(n->prev->d.n->d.c,(n->no%24)/4);
   }
  }
 plotlevel(); drawopts();
 }
