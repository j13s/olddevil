/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_move.c - all functions for moving/rotating.
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
#include "userio.h"
#include "calctxt.h"
#include "insert.h"
#include "plot.h"
#include "click.h"
#include "options.h"
#include "tag.h"
#include "plottxt.h"
#include "opt_txt.h"
#include "plotdata.h"
#include "do_event.h"
#include "do_opts.h"
#include "do_mod.h"
#include "do_move.h"

#define MOUSESTART_HILIGHT 1
#define MOUSEMOVE_HILIGHT 2

void makedirection(int axis,int dir,struct point *e,struct point *r)
 { int i; for(i=0;i<3;i++) r->x[i]=dir*e[axis].x[i]*view.pmovefactor; }
 
void pm_thing(struct list *li,int axis,int dir)
 {
 struct point r;
 struct node *sn,*n;
 struct point coords[3];
 int i,j;
 for(sn=li->head;sn->next!=NULL;sn=sn->next)
  {
  n=sn->d.n;
  if(n->d.t->type1==2 || n->d.t->type1==4 || n->d.t->type1==9 ||
   n->d.t->type1==14 || n->d.t->type1==3)
   {
   for(i=0;i<3;i++)
    for(j=0;j<3;j++)
     coords[i].x[j]=n->d.t->orientation[i*3+j]/65536.0;
   makedirection(axis,dir,coords,&r);
   }
  else
   makedirection(axis,dir,view.e,&r);
  for(i=0;i<3;i++)
   n->d.t->p[0].x[i]+=r.x[i];
  setthingpts(n->d.t); setthingcube(n->d.t);
  }
 }

void move_pnts(struct list *li,int axis,int dir)
 {
 struct point r;
 makedirection(axis,dir,view.e,&r);
 move_pntlist(li,&r);
 }
 
void pm_pnt(struct list *li,int axis,int dir)
 {
 struct list lp;
 struct node *n;
 /* Build a list of points to move with move_pnts */
 initlist(&lp);
 for(n=li->head;n->next!=NULL;n=n->next)
  addnode(&lp,n->no,n->d.n);
 if(lp.size>0) move_pnts(&lp,axis,dir);
 freelist(&lp,NULL);
 }
 
void pm_cube(struct list *li,int axis,int dir)
 {
 struct list lp;
 struct node *n;
 int j;
 /* Build a list of points to move with move_pnts */
 initlist(&lp);
 for(n=li->head;n->next!=NULL;n=n->next)
  for(j=0;j<8;j++)
   /* hope you don't wanna move too many cubes. I think there
      must be a better solution than this "findnode" */
   if(!findnode(&lp,n->d.n->d.c->p[j]->no))
    addnode(&lp,n->d.n->d.c->p[j]->no,n->d.n->d.c->p[j]);
 if(lp.size>0) move_pnts(&lp,axis,dir);
 freelist(&lp,NULL);
 }
 
void pm_wall(struct list *li,int axis,int dir)
 {
 struct list lp;
 struct node *n;
 int j;
 /* Build a list of points to move with move_pnts */
 initlist(&lp);
 for(n=li->head;n->next!=NULL;n=n->next)
  for(j=0;j<4;j++)
   /* hope you don't wanna move too many walls. I think there
      must be a better solution than this "findnode" */
   if(!findnode(&lp,n->d.n->d.c->p[wallpts[n->no%6][j]]->no))
    addnode(&lp,n->d.n->d.c->p[wallpts[n->no%6][j]]->no,
     n->d.n->d.c->p[wallpts[n->no%6][j]]); 
 if(lp.size>0) move_pnts(&lp,axis,dir);
 freelist(&lp,NULL);
 }

void (*pmove[tt_number])(struct list *li,int axis,int dir) =
 { pm_cube,pm_wall,NULL,pm_pnt,pm_thing,NULL };

void pr_thing(struct list *nl,int x,int y,int z,int dir)
 {
 int k,l;
 struct point coords[3];
 struct node *n,*sn;
 for(sn=nl->head;sn->next!=NULL;sn=sn->next)
  {
  n=sn->d.n;
  for(k=0;k<3;k++)
   for(l=0;l<3;l++)
    coords[k].x[l]=n->d.t->orientation[k*3+l]/65536.0;
  turn(coords,coords,x,y,z,dir*view.protangle);
  for(k=0;k<3;k++)
   for(l=0;l<3;l++)
    n->d.t->orientation[k*3+l]=coords[k].x[l]*65536;
  setthingpts(n->d.t);
  }
 }

/* Make offset and coordsystem for turning. Turn around point pnt of
 current wall or if pnt==-1 around the center of the current wall */
void turningcoords(struct point *offset,struct point *coords,int pnt)
 {
 int k,l;
 struct cube *c;
 float x;
 c=view.pcurrcube->d.c;
 if(pnt==-1)
  /* rotate around the center of the current cube. z-axis goes in positive
     direction through the center of the current wall, x-axis&y-axis
     through the center of the walls with the current point */
  {
  for(l=0;l<3;l++) 
   {
   offset->x[l]=0;
   for(k=0;k<8;k++)
    offset->x[l]+=c->p[k]->d.p->x[l]/8.0;
   }
  for(l=0;l<3;l++)
   {
   coords[2].x[l]=-offset->x[l];
   for(k=0;k<4;k++) 
    coords[2].x[l]+=c->p[wallpts[view.currwall][k]]->d.p->x[l]/4.0;
   }
  normalize(&coords[2]);
  for(l=0;l<3;l++)
   coords[0].x[l]=c->p[wallpts[view.currwall][1]]->d.p->x[l]-
    c->p[wallpts[view.currwall][0]]->d.p->x[l];
  x=SCALAR(&coords[0],&coords[2])/LENGTH(&coords[2]);
  for(l=0;l<3;l++)
   coords[0].x[l]-=x*coords[2].x[l];
  normalize(&coords[0]);
  VECTOR(&coords[1],&coords[2],&coords[0]);
  }
 else 
  {
  *offset=*c->p[wallpts[view.currwall][pnt]]->d.p;
  for(l=0;l<3;l++)
   { coords[0].x[l]=c->p[wallpts[view.currwall][(pnt+1)&3]]->d.p->x[l]-
      offset->x[l];
     coords[1].x[l]=c->p[wallpts[view.currwall][(pnt-1)&3]]->d.p->x[l]-
      offset->x[l]; }
  normalize(&coords[0]);
  x=SCALAR(&coords[0],&coords[1])/LENGTH(&coords[0]);
  for(l=0;l<3;l++) coords[1].x[l]-=x*coords[0].x[l];
  normalize(&coords[1]);
  VECTOR(&coords[2],&coords[0],&coords[1]);
  }
 }

void turnpnt(struct point *offset,struct point *coords,struct point *ncoords,
 struct point *pnt)
 {
 struct point tpnt,pc;
 int k;
 for(k=0;k<3;k++)
  tpnt.x[k]=pnt->x[k]-offset->x[k];
 for(k=0;k<3;k++)
  pc.x[k]=SCALAR(&coords[k],&tpnt);
 for(k=0;k<3;k++)
  pnt->x[k]=ceil(pc.x[0]*ncoords[0].x[k]+pc.x[1]*ncoords[1].x[k]+
   pc.x[2]*ncoords[2].x[k]+offset->x[k]);
 }    
     
void pr_cube(struct list *nl,int x,int y,int z,int dir)
 {
 struct point offset,coords[3],ncoords[3],*save,*p;
 struct node *n,*cn;
 struct list lp;
 int j,k,currcubeins=0;
 if(view.gridonoff && view.warn_gridrot && !yesnomsg(TXT_GRIDROT))
  return;
 initlist(&lp);
 turningcoords(&offset,coords,x==1 ? view.curredge : -1);
 /* add current cube to tag list */
 if(x==0 || x==1) /* banking */
  { if((currcubeins=((cn=findnode(nl,view.pcurrcube->no))==NULL))==1)
     cn=addnode(nl,view.pcurrcube->no,view.pcurrcube); }
 else /* left/right */
  cn=NULL; 
 /* turn coordsystem */
 turn(coords,ncoords,0,1,2,dir*view.protangle);
 checkmem(save=MALLOC(sizeof(struct point)*nl->size*8));
 for(n=nl->head;n->next!=NULL;n=n->next)
  for(j=0;j<8;j++)
   if(!findnode(&lp,n->d.n->d.c->p[j]->no))
    addnode(&lp,n->d.n->d.c->p[j]->no,n->d.n->d.c->p[j]->d.p);
 for(n=lp.head,p=save;n->next!=NULL;n=n->next,p++)
  { *p=*n->d.p;
    turnpnt(&offset,coords,ncoords,n->d.p); }
 for(n=lp.head,k=1;n->next!=NULL;n=n->next)
  if((k=testpnt(n))==0) break;
 if(!k)
  for(p=save,n=lp.head;n->next!=NULL;n=n->next,p++)
   *n->d.p=*p;
 else
  for(n=lp.head;n->next!=NULL;n=n->next)
   newcorners(n);
 FREE(save);
 freelist(&lp,NULL);
 if(cn!=NULL && currcubeins) /* for empty tagged list */
  freenode(nl,cn,NULL);
 }

int findsideindirection(int axis,int dir,int with_neighbour)
 {
 float max_v=0.0;
 int i,w,best_w=-1;
 struct point d1,d2,n;
 for(w=0;w<6;w++)
  {
  for(i=0;i<3;i++) d1.x[i]=view.pcurrcube->d.c->p[wallpts[w][2]]->d.p->x[i]-
		    view.pcurrcube->d.c->p[wallpts[w][0]]->d.p->x[i];
  for(i=0;i<3;i++) d2.x[i]=view.pcurrcube->d.c->p[wallpts[w][3]]->d.p->x[i]-
		    view.pcurrcube->d.c->p[wallpts[w][1]]->d.p->x[i];
  VECTOR(&n,&d1,&d2); normalize(&n);
  if(SCALAR(&n,&view.e[axis])*dir>max_v && (!with_neighbour ||
   view.pcurrcube->d.c->nc[w]!=NULL))
   { max_v=SCALAR(&n,&view.e[axis])*dir; best_w=w; }
  }
 return best_w;
 }

void b_move(int axis,int dir)
 {
 int i;
 struct point newpos;
 if(l==NULL || view.pcurrcube==NULL) { printmsg(TXT_NOLEVEL); return; }
 switch(view.movemode)
  {
  case mt_you:
   for(i=0;i<3;i++) 
    newpos.x[i]=view.e0.x[i]+dir*view.movefactor*view.e[axis].x[i];
   move_user(&newpos);
   break;
  case mt_obj:
   /* move the current cube,wall,point,thing */
   if(view.currmode!=tt_cube && view.currmode!=tt_wall && 
    view.currmode!=tt_pnt && view.currmode!=tt_thing)
    { printmsg(TXT_NOMOVINGMODE); return; }
   l->levelsaved=0; l->levelillum=(view.currmode==tt_thing);
   if(!view.pcurrcube) return;
   pmove[view.currmode](&l->tagged[view.currmode],axis,dir);
   break;
  case mt_current:
   i=findsideindirection(axis,-dir,1);
   if(i<0) return;
   if(view.pcurrcube->d.c->nc[i]!=NULL)
    {
    view.currwall=i;
    for(i=0;i<6;i++)
     if(view.pcurrcube->d.c->nc[view.currwall]->d.c->nc[i]==view.pcurrcube)
      break;
    my_assert(i<6); 
    view.pcurrcube=view.pcurrcube->d.c->nc[view.currwall];
    view.currwall=i;
    view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
    plotcurrent(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
    }
   break;
  case mt_texture: fb_move_texture(axis,dir); return;
  default: printmsg("Movemode %d not implemented",view.movemode);
  }
 if(view.movemode!=mt_current) plotlevel();
 if(view.movemode==mt_obj) drawopt(in_pnt);
 }

void b_turn(int x,int y,int z,int dir)
 {
 struct point e0,x0,er[3];
 int i;
 if(l==NULL || view.pcurrcube==NULL) { printmsg(TXT_NOLEVEL); return; }
 switch(view.movemode)
  {
  case mt_you:
   initcoordsystem(0,&e0,er,&x0); 
   turn(view.e,view.e,x,y,z,view.rotangle*dir); 
   if(view.whichdisplay)
    for(i=0;i<3;i++) e0.x[i]+=view.e[2].x[i]*view.distcenter;
   move_user(&e0);
   break;
  case mt_obj:
   l->levelsaved=0;
   switch(view.currmode)
    {
    case tt_thing: pr_thing(&l->tagged[tt_thing],x,y,z,dir); break;
    case tt_cube: pr_cube(&l->tagged[tt_cube],x,y,z,dir); 
     l->levelillum=0; break;
    default: printmsg(TXT_NOTURNOBJECT); 
    }
   break;
  case mt_current:
   i=findsideindirection(x==0 ? 2 : (x==1 ? 1 : 0),
     x==2 ? -dir : dir,0);
   if(i<0) return;
   view.currwall=i; view.pcurrwall=view.pcurrcube->d.c->walls[i];
   plotcurrent(); drawopt(in_wall); drawopt(in_edge);
   break;
  case mt_texture: fb_turn_texture(x,y,z,dir); return;
  default: printmsg("Turnmode %d not implemented",view.movemode);
  }
 if(view.movemode!=mt_current) plotlevel();
 if(view.movemode==mt_obj) drawopt(in_pnt);
 }

void dec_move0(int ec) { b_move(2,-1); }
void dec_move1(int ec) { b_move(1,1); }
void dec_move2(int ec) { b_move(2,1); }
void dec_move3(int ec) { b_move(0,-1); }
void dec_move4(int ec) { b_move(1,-1); }
void dec_move5(int ec) { b_move(0,1); }
void dec_turn0(int ec) { b_turn(0,1,2,1); }
void dec_turn1(int ec) { b_turn(1,2,0,view.flip_y_axis ? 1 : -1); }
void dec_turn2(int ec) { b_turn(0,1,2,-1); }
void dec_turn3(int ec) { b_turn(2,0,1,-1); }
void dec_turn4(int ec) { b_turn(1,2,0,view.flip_y_axis ? -1 : 1); }
void dec_turn5(int ec) { b_turn(2,0,1,1); }

void dec_beam(int ec)
 {
 int i,j;
 struct point p;
 switch(view.currmode)
  {
  case tt_pnt: if(!view.pcurrcube) { printmsg(TXT_NOCURRPNT); return; }
   p=*view.pcurrpnt->d.p;
   break;
  case tt_cube: if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
   for(j=0;j<3;j++)
    { p.x[j]=0;
      for(i=0;i<8;i++) p.x[j]+=view.pcurrcube->d.c->p[i]->d.p->x[j]/8; }
   break;
  case tt_wall: if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
   for(j=0;j<3;j++)
    { 
    p.x[j]=0;
    for(i=0;i<4;i++) 
     p.x[j]+=view.pcurrcube->d.c->p[wallpts[view.currwall][i]]->d.p->x[j]/4;
    }
   break;
  case tt_thing: if(!view.pcurrthing) { printmsg(TXT_NOCURRTHING); return; }
   p=view.pcurrthing->d.t->p[0]; break;
  case tt_door: if(!view.pcurrdoor) { printmsg(TXT_NOCURRWALL); return; }
   p=view.pcurrdoor->d.d->p; break;
  default: my_assert(0);
  }
 view.e0=p;
 for(i=0;i<3;i++) view.e0.x[i]-=view.tsize*4*view.e[2].x[i];
 init_rendercube();
 plotlevel();
 }
 
void dec_gowall(int ec)
 {
 if(!view.pcurrdoor) { printmsg(TXT_NOCURRWALL); return; }
 view.pcurrcube=view.pcurrdoor->d.d->c; 
 view.currwall=view.pcurrdoor->d.d->w->no; 
 plotcurrent(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
 }  
 
void dec_sidecube(int ec)
 {
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 if(!view.pcurrcube->d.c->nc[view.currwall])
  { printmsg(TXT_SIDECUBENONEIGHBOUR); return; }
 view.pcurrcube=view.pcurrcube->d.c->nc[view.currwall]; 
 view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
 plotcurrent(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
 }

struct point align_xaxis[3]={{{0.0,0.0,-1.0}},{{0.0,1.0,0.0}},
 {{1.0,0.0,0.0}}};
struct point align_yaxis[3]={{{1.0,0.0,0.0}},{{0.0,0.0,-1.0}},
 {{0.0,1.0,0.0}} };
struct point align_zaxis[3]={{{1.0,0.0,0.0}},{{0.0,1.0,0.0}},
 {{0.0,0.0,1.0}} };
struct point align_nxaxis[3]={{{0.0,0.0,1.0}},{{0.0,1.0,0.0}},
 {{-1.0,0.0,0.0}} };
struct point align_nyaxis[3]={{{1.0,0.0,0.0}},{{0.0,0.0,1.0}},
 {{0.0,-1.0,0.0}} };
struct point align_nzaxis[3]={{{-1.0,0.0,0.0}},{{0.0,1.0,0.0}},
 {{0.0,0.0,-1.0}} };
void dec_aligntoaxis(int ec)
 {
 int j;
 switch(ec)
  {
  case ec_posxaxis: for(j=0;j<3;j++) view.e[j]=align_xaxis[j]; break;
  case ec_negxaxis: for(j=0;j<3;j++) view.e[j]=align_nxaxis[j]; break;
  case ec_posyaxis: for(j=0;j<3;j++) view.e[j]=align_yaxis[j]; break;
  case ec_negyaxis: for(j=0;j<3;j++) view.e[j]=align_nyaxis[j]; break;
  case ec_poszaxis: for(j=0;j<3;j++) view.e[j]=align_zaxis[j]; break;
  case ec_negzaxis: for(j=0;j<3;j++) view.e[j]=align_nzaxis[j]; break;
  default: my_assert(0);
  }
 plotlevel();
 }

void dec_prevcube(int ec) { prevobject(tt_cube); }
void dec_nextcube(int ec) { nextobject(tt_cube); }
void dec_prevside(int ec) { prevobject(tt_wall); }
void dec_nextside(int ec) { nextobject(tt_wall); }
void dec_prevpnt(int ec) { prevobject(tt_pnt); }
void dec_nextpnt(int ec) { nextobject(tt_pnt); }
void dec_prevthing(int ec) { prevobject(tt_thing); }
void dec_nextthing(int ec) { nextobject(tt_thing); }
void dec_prevwall(int ec) { prevobject(tt_door); }
void dec_nextwall(int ec) { nextobject(tt_door); }
void dec_prevedge(int ec) { prevobject(tt_edge); }
void dec_nextedge(int ec) { nextobject(tt_edge); }
void dec_resetsideedge(int ec) { resetsideedge(); }

void plotpntcubelist(struct list *lp,int hilight,int xor,int start)
 {
 struct node *n;
 int w;
 for(n=lp->head;n->next!=NULL;n=n->next) 
  { 
  if(view.drawwhat&DW_CUBES) plotcube(n->d.n,hilight,xor,0,start);
  for(w=0;w<6;w++) 
   {
   if(n->d.n->d.c->tagged_walls[w] && start)
    plottagwall(n->d.n->d.c,w,256,0);
   if(n->d.n->d.c->d[w]) 
    { makedoorpnt(n->d.n->d.c->d[w]->d.d);
      if(view.drawwhat&DW_DOORS) plotdoor(n->d.n->d.c->d[w],hilight,xor); } 
   }
  copytoscreen();
  }    
 }

void turnsinglepnt(struct point *dir_axis,float cosang,float sinang,
 struct point *s)
 {
 float x,l;
 int i;
 struct point vp,vs,vs2,e1,e2,axis;
 axis=*dir_axis; normalize(&axis);
 x=SCALAR(&axis,s);
 for(i=0;i<3;i++) { vp.x[i]=axis.x[i]*x; vs.x[i]=s->x[i]-vp.x[i]; }
 if((l=LENGTH(&vs))!=0)
  { e1=vs; for(i=0;i<3;i++) e1.x[i]/=l; VECTOR(&e2,&axis,&e1);
    for(i=0;i<3;i++) vs2.x[i]=cosang*l*e1.x[i]+sinang*l*e2.x[i]; }
 else vs2=vs;
 for(i=0;i<3;i++) s->x[i]=vp.x[i]+vs2.x[i];
 }
 
int movething(struct point *add,union move_params *params)
 {
 int withtagged=params->t.withtagged,i;
 struct node *n,*nc=params->t.nc;
 if(!view.render) plotthing(nc->d.t,-1);
 for(i=0;i<3;i++) nc->d.t->p[0].x[i]+=add->x[i];
 setthingpts(nc->d.t); setthingcube(nc->d.t); 
 if(!view.render) plotthing(nc->d.t,-1); 
 if(withtagged)
  for(n=l->tagged[tt_thing].head;n->next!=NULL;n=n->next)
   { if(n->d.n==nc) continue;
     if(!view.render) plotthing(n->d.n->d.t,-1); 
     for(i=0;i<3;i++) n->d.n->d.t->p[0].x[i]+=add->x[i];
     setthingpts(n->d.n->d.t); setthingcube(nc->d.t); 
     if(!view.render) plotthing(n->d.n->d.t,-1); }
 if(view.render) plotlevel(); else copytoscreen();
 return 1;
 }

int turnthing(struct point *t,float fx,float fy,int with_left,
 union move_params *params)
 { 
 int withtagged=params->t.withtagged;
 struct node *n,*nc=params->t.nc;
 int i,j;
 struct point coords[3];
 if(!view.render) plotthing(nc->d.t,-1);
 for(i=0;i<3;i++) 
  for(j=0;j<3;j++) coords[i].x[j]=nc->d.t->orientation[i*3+j]/65536.0;
 if(!with_left) turn(coords,coords,0,1,2,fx);
 else if(fabs(fx)>fabs(fy)) turn(coords,coords,2,0,1,fx);
 else turn(coords,coords,1,2,0,fy);
 for(i=0;i<3;i++) 
  for(j=0;j<3;j++) nc->d.t->orientation[i*3+j]=coords[i].x[j]*65536.0;
 setthingpts(nc->d.t);
 if(!view.render) plotthing(nc->d.t,-1);
 if(withtagged)
  for(n=l->tagged[tt_thing].head;n->next!=NULL;n=n->next)
   { if(nc==n->d.n) continue;
     if(!view.render) plotthing(n->d.n->d.t,-1); 
     for(i=0;i<3;i++) 
      for(j=0;j<3;j++) coords[i].x[j]=n->d.n->d.t->orientation[i*3+j]/65536.0;
     if(!with_left) turn(coords,coords,0,1,2,fx);
     else if(fabs(fx)>fabs(fy)) turn(coords,coords,2,0,1,fx);
     else turn(coords,coords,1,2,0,fy);
     for(i=0;i<3;i++) 
      for(j=0;j<3;j++) n->d.n->d.t->orientation[i*3+j]=coords[i].x[j]*65536.0;
     setthingpts(n->d.n->d.t); 
     if(!view.render) plotthing(n->d.n->d.t,-1); }
 if(view.render) plotlevel(); else copytoscreen();
 return 1;
 }
 
struct point *start_move_pnts(union move_params *params,
 struct list **pnt_list,struct list **side_list,struct list **cube_list,
 struct point **ref_pnt,struct node **cube,int *wall)
 {
 struct point *opa;
 l->levelillum=0;
 *pnt_list=params->p.pnt_list;
 *side_list=params->p.side_list;
 *cube_list=params->p.cube_list;
 *ref_pnt=params->p.ref_pnt;
 *cube=params->p.cube; *wall=params->p.wall;
 if(view.render) plotlevel();
 else plotpntcubelist(*cube_list,MOUSEMOVE_HILIGHT,1,0);
 checkmem(opa=MALLOC(sizeof(struct point)*(*pnt_list)->size));
 w_killcurrentmessage();
 return opa;
 }
 
int movepnt(struct point *add,union move_params *params)
 { 
 struct point *opa,*run,*ref_pnt,p;
 struct list *pnt_list,*cube_list,*side_list;
 int i,j,ok=1,wall;
 short int uv[4][2];
 struct node *n,*nc;
 opa=start_move_pnts(params,&pnt_list,&side_list,&cube_list,&ref_pnt,
  &nc,&wall);
 /* check if all pnts are valid after they're moved */
 for(n=pnt_list->head,run=opa;n->next!=NULL;n=n->next,run++)
  { *run=*n->d.p; for(i=0;i<3;i++) n->d.p->x[i]+=add->x[i]; 
    fittogrid(n->d.p); }
 for(n=pnt_list->head;n->next!=NULL;n=n->next)
  if(!testpnt(n)) { ok=0; break; }
 /* restore old coords */
 for(n=pnt_list->head,run=opa;n->next!=NULL;n=n->next,run++) *n->d.p=*run;
 if(ok)
  {
  /* calculate the new texture coords */
  for(n=side_list->head;n->next!=NULL;n=n->next) 
   {
   if(n->d.mw->cube->d.c->walls[n->d.mw->wall]!=NULL &&
    !n->d.mw->cube->d.c->walls[n->d.mw->wall]->locked)
    {
    for(j=0;j<4;j++)
     if(n->d.mw->move_pnts&(1<<j))
      {
      for(i=0;i<3;i++) 
       p.x[i]=n->d.mw->cube->d.c->walls[n->d.mw->wall]->p[j]->d.p->x[i]+
        add->x[i];
      calc_txtcoords(n->d.mw->cube->d.c->walls[n->d.mw->wall],j,&p,uv[j]);
      }
    for(j=0;j<4;j++) 
     if(n->d.mw->move_pnts&(1<<j))
      { n->d.mw->cube->d.c->walls[n->d.mw->wall]->corners[j].x[0]=uv[j][0];
        n->d.mw->cube->d.c->walls[n->d.mw->wall]->corners[j].x[1]=uv[j][1]; }
    }
   n->d.mw->cube->d.c->recalc_polygons[n->d.mw->wall]=1;
   }
  /* change the space coords */
  for(n=pnt_list->head;n->next!=NULL;n=n->next) 
   for(i=0;i<3;i++) n->d.p->x[i]+=add->x[i];
  /* is this is side/cube movement reinit the moved side */
  if(nc!=NULL) 
   if(wall>=0) nc->d.c->recalc_polygons[wall]=1;
   else for(i=0;i<6;i++) nc->d.c->recalc_polygons[i]=1;
  for(n=side_list->head;n->next!=NULL;n=n->next) 
   if(n->d.mw->cube->d.c->d[n->no%6]!=NULL) 
    makedoorpnt(n->d.mw->cube->d.c->d[n->no%6]->d.d);
  if(ref_pnt)
   for(i=0;i<3;i++) 
    { ref_pnt->x[i]=0.0;
      if(wall<0) for(j=0;j<8;j++) ref_pnt->x[i]+=nc->d.c->p[j]->d.p->x[i]/8.0;
      else for(j=0;j<4;j++) 
       ref_pnt->x[i]+=nc->d.c->p[wallpts[wall][j]]->d.p->x[i]/4.0; }
  }
 FREE(opa);
 if(view.render) plotlevel();
 else plotpntcubelist(cube_list,MOUSEMOVE_HILIGHT,1,0);
 return ok;
 }
 
/* turn is the axis, length of turn the angle */ 
int rotatepnt(struct point *turn,float fx,float fy,int with_left,
 union move_params *params)
 { 
 struct point *opa,*run,*ref_pnt,p,*center_of_rot;
 struct list *pnt_list,*cube_list,*side_list;
 int i,j,ok=1,wall;
 short int uv[4][2];
 struct node *n,*nc;
 float angle,cosang,sinang;
 opa=start_move_pnts(params,&pnt_list,&side_list,&cube_list,&ref_pnt,
  &nc,&wall);
 angle=LENGTH(turn); normalize(turn);
 cosang=cos(angle); sinang=sin(angle);
 /* check if all pnts are valid after they're moved */
 center_of_rot=ref_pnt==NULL ? pnt_list->head->d.p : ref_pnt;
 for(n=pnt_list->head,run=opa;n->next!=NULL;n=n->next,run++)
  { *run=*n->d.p;
    for(i=0;i<3;i++) p.x[i]=n->d.p->x[i]-center_of_rot->x[i];
    turnsinglepnt(turn,cosang,sinang,&p);
    for(i=0;i<3;i++) n->d.p->x[i]=center_of_rot->x[i]+p.x[i];
    fittogrid(n->d.p); }
 for(n=pnt_list->head;n->next!=NULL;n=n->next)
  if(!testpnt(n)) { ok=0; break; }
 /* restore old coords */
 for(n=pnt_list->head,run=opa;n->next!=NULL;n=n->next,run++) *n->d.p=*run;
 if(ok)
  {
  /* calculate the new texture coords */
  for(n=side_list->head;n->next!=NULL;n=n->next) 
   {
   if(n->d.mw->cube->d.c->walls[n->d.mw->wall]!=NULL &&
    !n->d.mw->cube->d.c->walls[n->d.mw->wall]->locked)
    {
    for(j=0;j<4;j++)
     if(n->d.mw->move_pnts&(1<<j))
      {
      for(i=0;i<3;i++) 
       p.x[i]=n->d.mw->cube->d.c->walls[n->d.mw->wall]->p[j]->d.p->x[i]-
        center_of_rot->x[i];
      turnsinglepnt(turn,cosang,sinang,&p);
      for(i=0;i<3;i++) p.x[i]+=center_of_rot->x[i];
      calc_txtcoords(n->d.mw->cube->d.c->walls[n->d.mw->wall],j,&p,uv[j]);
      }
    for(j=0;j<4;j++) 
     if(n->d.mw->move_pnts&(1<<j) && 
      n->d.mw->cube->d.c->walls[n->d.mw->wall]!=NULL)
      { n->d.mw->cube->d.c->walls[n->d.mw->wall]->corners[j].x[0]=uv[j][0];
        n->d.mw->cube->d.c->walls[n->d.mw->wall]->corners[j].x[1]=uv[j][1]; }
    }
   n->d.mw->cube->d.c->recalc_polygons[n->d.mw->wall]=1;
   }
  /* change the space coords */
  for(n=pnt_list->head;n->next!=NULL;n=n->next) 
   { for(i=0;i<3;i++) p.x[i]=n->d.p->x[i]-center_of_rot->x[i];
     turnsinglepnt(turn,cosang,sinang,&p);
     for(i=0;i<3;i++) n->d.p->x[i]=center_of_rot->x[i]+p.x[i]; }
  /* is this is side/cube movement reinit the moved side */
  if(nc!=NULL) 
   if(wall>=0) nc->d.c->recalc_polygons[wall]=1;
   else for(i=0;i<6;i++) nc->d.c->recalc_polygons[i]=1;
  for(n=side_list->head;n->next!=NULL;n=n->next) 
   if(n->d.mw->cube->d.c->d[n->no%6]!=NULL) 
    makedoorpnt(n->d.mw->cube->d.c->d[n->no%6]->d.d);
  }
 FREE(opa);
 if(view.render) plotlevel();
 else plotpntcubelist(cube_list,MOUSEMOVE_HILIGHT,1,0);
 return ok;
 }

#define ABS(x) ((x)<0 ? -(x) : (x))
/* w is the window in which the button was pressed, wx,wy the coords,
 moveobj is the routine to change the coords of the object(s) and draw them 
 and orig_p is a pointer to the reference coords (which must be changed 
 during the moveobj routine) */
void moving_with_mouse(struct w_window *w,int wx,int wy,
 int (*moveobj)(struct point *add,union move_params *params),
 int (*turnobj)(struct point *turn,float fx,float fy,int with_left,
  union move_params *params),struct point *orig_p,union move_params *params)
 {
 struct ws_event ws;
 struct point a,r,er[3],e0,x0,np;
 int x,y,ox,oy,i,lr,xs,oldkbstat=-1;
 float fx,fy;
 struct pixel start;
 ws_erasemouse();
 xs=view.whichdisplay ? w_xwininsize(w)/2 : w_xwininsize(w);
 lr=(wx>=xs); makeview(lr);
 initcoordsystem(lr,&e0,er,&x0);
 getpixelcoords(lr,orig_p,&start);
 ws_mousewarp(ox=w_xwinincoord(w,start.x+(lr ? xs+(xs-1)/2 : (xs-1)/2)), 
  oy=w_ywinincoord(w,(w_ywininsize(w)-1)/2-start.y)); 
 do
  {
  ws_getevent(&ws,0);
  if(ws.flags!=ws_f_none || oldkbstat!=ws.kbstat)
   { getpixelcoords(lr,orig_p,&start); oldkbstat=ws.kbstat;
     ws_mousewarp(ox=w_xwinincoord(w,start.x+(lr ? xs+(xs-1)/2 : (xs-1)/2)), 
      oy=w_ywinincoord(w,(w_ywininsize(w)-1)/2-start.y)); continue; }
  if(ws.x==ox && ws.y==oy) continue;
  if((ws.x-ox)*32>xs) ws.x=ox+xs/32;
  else if((ox-ws.x)*32>xs) ws.x=ox-xs/32;
  if((ws.y-oy)*32>w_ywininsize(w)) ws.y=oy+w_ywininsize(w)/32;
  else if((oy-ws.y)*32>w_ywininsize(w)) ws.y=oy-w_ywininsize(w)/32;
  x=w_xscreencoord(w,ws.x); y=w_yscreencoord(w,ws.y);
  ox=w_xscreencoord(w,ox); oy=w_yscreencoord(w,oy);
  if((ws.kbstat&ws_ks_shift)==0 && ((ws.buttons&ws_bt_left)!=0
   || ABS(x-ox)>ABS(y-oy))) /* rotate */
   {
   if(x<(lr ? xs : 0)) x=(lr ? 2*xs : xs)-1;
   else if(x>=(lr ? xs*2 : xs)) x=(lr ? xs : 0);
   if(y<0) y=w_ywininsize(w)-1;
   else if(y>=w_ywininsize(w)) y=0;
   fx=(float)(x-ox)/xs*2*M_PI; 
   fy=(float)(y-oy)/w_ywininsize(w)*2*M_PI;
   if((ws.buttons&ws_bt_left)==0) 
    for(i=0;i<3;i++) np.x[i]=-fx*er[2].x[i];
   else 
    for(i=0;i<3;i++) np.x[i]=-er[1].x[i]*fx-er[0].x[i]*fy;
   if(turnobj && !turnobj(&np,fx,fy,(ws.buttons&ws_bt_left)!=0,params))
    { x=ox; y=oy; }
   }
  else
   {
   if((ws.kbstat&ws_ks_shift)!=0)
    { if(x<(lr ? xs : 0)) x=lr ? xs : 0;
      else if(x>=(lr ? xs*2 : xs)) x=(lr ? xs*2 : xs)-1;
      if(y<0) y=0;
      else if(y>=w_ywininsize(w)) y=w_ywininsize(w)-1;
      init_beam(&a,&r,w,x,y); normalize(&r);
      for(i=0;i<3;i++) np.x[i]=orig_p->x[i]-a.x[i];
      fx=SCALAR(&np,&er[2])/SCALAR(&er[2],&r);
      for(i=0;i<3;i++) np.x[i]=a.x[i]+r.x[i]*fx-orig_p->x[i]; }
   else
    { fy=(float)(y-oy)/w_ywininsize(w)*view.pmovefactor*20.0;
      for(i=0;i<3;i++) np.x[i]=-fy*er[2].x[i];
      x=ox; y=oy; }
   if(!moveobj(&np,params)) { x=ox; y=oy; }
   }
  ws_mousewarp(ox=w_xwinincoord(w,x),oy=w_ywinincoord(w,y));
  }
 while(ws.buttons!=ws_bt_none);
 ws_displaymouse();
 }

void addpnttomovelist(struct list *pntpl,struct list *cubepl,struct node *np)
 {
 struct node *n2;
 int i,j;
 if(!findnode(pntpl,np->no))
  {
  checkmem(addnode(pntpl,np->no,np->d.lp));
  for(n2=np->d.lp->c.head;n2->next!=NULL;n2=n2->next)
   {
   if(!findnode(cubepl,n2->d.n->no)) 
    checkmem(addnode(cubepl,n2->d.n->no,n2->d.n)); 
   for(j=0;j<3;j++)
    if(n2->d.n->d.c->walls[wallno[n2->no][0][j]] &&
     n2->d.n->d.c->walls[wallno[n2->no][0][j]]->locked)
     for(i=0;i<4;i++)
      if(i!=wallno[n2->no][1][j])
       addpnttomovelist(pntpl,cubepl,
        n2->d.n->d.c->p[wallpts[wallno[n2->no][0][j]][i]]);
   }
  }
 }

struct movewall *addsidetomovelist(struct list *sidepl,struct node *cube,
 int wall,int move_pnts)
 {
 struct movewall *mw;
 checkmem(mw=MALLOC(sizeof(struct movewall)));
 mw->cube=cube; mw->wall=wall; mw->move_pnts=move_pnts;
 checkmem(addnode(sidepl,cube->no*6+wall,mw));
 return mw;
 }
 
void makesidemovelist(struct list *pntpl,struct list *sidepl)
 {
 struct node *n,*n2,*ns;
 struct movewall *mw;
 int j,w,c;
 initlist(sidepl);
 for(n=pntpl->head;n->next!=NULL;n=n->next)
  for(n2=n->d.lp->c.head;n2->next!=NULL;n2=n2->next)
   for(j=0;j<3;j++)
    {
    w=wallno[n2->no][0][j]; c=wallno[n2->no][1][j];
    if((ns=findnode(sidepl,n2->d.n->no*6+w))==NULL)
     mw=addsidetomovelist(sidepl,n2->d.n,w,0);
    else mw=ns->d.mw;
    mw->move_pnts|=1<<c;
    }
 }
 
void redrawcubes(struct list *pntpl,struct list *cubepl)
 {
 struct node *n;
 int w,c;
 plotcurrent(); 
 plotmarker(view.pcurrpnt->d.p,testtag(tt_pnt,view.pcurrpnt) ? -6 : -1);
 plotpnt(view.pcurrcube,view.currwall,view.curredge,testtag(tt_edge,
  view.pcurrcube,view.currwall,view.curredge) ? -6 : -1);     
 for(n=pntpl->head;n->next!=NULL;n=n->next)
  if(testtag(tt_pnt,n) && n!=view.pcurrpnt) plotmarker(n->d.p,-3);
 for(n=cubepl->head;n->next!=NULL;n=n->next)
  for(w=0;w<6;w++) if(n->d.n->d.c->walls[w]) for(c=0;c<4;c++)
   if(testtag(tt_edge,n->d.n,w,c) && (n->d.n!=view.pcurrcube ||
    w!=view.currwall || c!=view.curredge)) plotpnt(n->d.n,w,c,-3);
 plotpntcubelist(cubepl,view.draw_orig_lines ? 5 : 256,0,1); 
 plotpntcubelist(cubepl,MOUSEMOVE_HILIGHT,1,0);
 copytoscreen();
 }

void rotate_texture(struct corner *cs,float angle)
 {
 float centerx,centery,dx,dy;
 int i;
 centerx=centery=0.0;
 for(i=0;i<4;i++) { centerx+=cs[i].x[0]; centery+=cs[i].x[1]; }
 centerx/=4.0; centery/=4.0;
 for(i=0;i<4;i++)
  { dx=cs[i].x[0]-centerx; dy=cs[i].x[1]-centery;
    cs[i].x[0]=centerx+cos(angle)*dx+sin(angle)*dy+0.5;
    cs[i].x[1]=centery-sin(angle)*dx+cos(angle)*dy+0.5; }
 }
 
void move_texture(struct corner *cs,int dx,int dy)
 {
 int i,cx,cy;
 cx=cy=0;
 for(i=0;i<4;i++)
  { cs[i].x[0]+=dx; cs[i].x[1]+=dy; cx+=cs[i].x[0]; cy+=cs[i].x[1]; }
 for(i=0;i<4;i++)
  { cs[i].x[0]-=(cx/2048/4)*2048; cs[i].x[1]-=(cy/2048/4)*2048; }
 }
 
void stretch_texture(struct corner *cs,int sx,int sy)
 {
 float fx,fy,cx,cy,dx,dy;
 int i;
 fx=sx==0 ? 1.0 : (sx<0 ? 1/view.bm_stretchfactor : view.bm_stretchfactor);
 fy=sy==0 ? 1.0 : (sy<0 ? 1/view.bm_stretchfactor : view.bm_stretchfactor);
 cx=cy=0;
 for(i=0;i<4;i++) { cx+=cs[i].x[0]; cy+=cs[i].x[1]; }
 cx/=4.0; cy/=4.0;
 for(i=0;i<4;i++)
  { dx=cs[i].x[0]-cx; dy=cs[i].x[1]-cy;
    cs[i].x[0]=cx+dx*fx+0.5; cs[i].x[1]=cy+dy*fy+0.5; }
 }
 
/* rotate x,y around angle and store the result in dx,dy */
void txt_recalcdxdy(int x,int y,float angle,int *dx,int *dy)
 { *dx=cos(angle)*x+sin(angle)*y;
   *dy=-sin(angle)*x+cos(angle)*y; }
 
/* move texture-coords in cs, if cs!=NULL.
 If cs==NULL, then change the coords for side cube,wall.
 If withtagged==1 changed all tagged walls, too.
 Each time the coords are changed refresh_func is called.
 First click was at screen coords sx,sy.
 angle is the angle between the x-axis of the user and the u-axis of
 the texture */
void move_texture_with_mouse(int sx,int sy,struct corner *cs,
 struct node *cube,int wall,int withtagged,float angle,
 void (*refresh_func)(void))
 {
 struct ws_event ws;
 int ox,oy,xs,ys,dx,dy;
 struct node *n;
 my_assert(cs!=NULL || cube!=NULL);
 xs=init.xres; ys=init.yres; ox=sx; oy=sy;
 if(cs==NULL && cube->d.c->walls[wall]!=NULL)
  cs=cube->d.c->walls[wall]->corners;
 ws_erasemouse();
 do
  {
  ws_getevent(&ws,0);
  if(ws.x==ox && ws.y==oy) continue;
  if((ws.kbstat&ws_ks_shift)==0 && ((ws.buttons&ws_bt_left)!=0
   || ABS(ws.x-ox)>ABS(ws.y-oy))) /* rotate */
   {
   if(ws.x!=ox) ws.y=oy;
   if((ws.buttons&ws_bt_left)==0) 
    {
    rotate_texture(cs,(float)(ws.x-ox)/xs*M_PI);
    for(n=l->tagged[tt_wall].head;withtagged && n->next!=NULL;n=n->next)
     if((n->d.n!=cube || n->no%6!=wall) && n->d.n->d.c->walls[n->no%6])
      rotate_texture(n->d.n->d.c->walls[n->no%6]->corners,
       (float)(ws.x-ox)/xs*M_PI);
    }
   else
    {
    stretch_texture(cs,ws.x-ox,ws.y-oy);
    for(n=l->tagged[tt_wall].head;withtagged && n->next!=NULL;n=n->next)
     if((n->d.n!=cube || n->no%6!=wall) && n->d.n->d.c->walls[n->no%6])
      stretch_texture(n->d.n->d.c->walls[n->no%6]->corners,ws.x-ox,ws.y-oy);
    } 
   }
  else /* move */
   if((ws.kbstat&ws_ks_shift)!=0) /* left, right, up, down */
    {
    txt_recalcdxdy(ws.x-ox,ws.y-oy,angle,&dx,&dy);
    move_texture(cs,(int)((float)dx/xs*2*2048),
     (int)((float)dy/ys*2*2048));
    for(n=l->tagged[tt_wall].head;withtagged && n->next!=NULL;n=n->next)
     if((n->d.n!=cube || n->no%6!=wall) && n->d.n->d.c->walls[n->no%6])
      move_texture(n->d.n->d.c->walls[n->no%6]->corners,
       (int)((float)dx/xs*2*2048),(int)((float)dy/ys*2*2048));
    }
   else /* forward, backward */
    {
    stretch_texture(cs,ws.y-oy,ws.y-oy);
    for(n=l->tagged[tt_wall].head;withtagged && n->next!=NULL;n=n->next)
     if((n->d.n!=cube || n->no%6!=wall) && n->d.n->d.c->walls[n->no%6])
      stretch_texture(n->d.n->d.c->walls[n->no%6]->corners,ws.y-oy,ws.y-oy);
    }
  if(cube) cube->d.c->recalc_polygons[wall]=1;
  for(n=l->tagged[tt_wall].head;withtagged && n->next!=NULL;n=n->next)
   if(n->d.n->d.c->walls[n->no%6])
    n->d.n->d.c->recalc_polygons[n->no%6]=1;    
  ws_mousewarp(ox,oy);
  refresh_func();
  }
 while(ws.buttons!=ws_bt_none);
 ws_displaymouse();
 }

float txt_calcangle(struct wall *w)
 {
 float a,b;
 struct corner *c;
 struct point d1,d2,n;
 int i;
 my_assert(w!=NULL); c=w->corners;
 for(i=0;i<3;i++)
  { d1.x[i]=w->p[2]->d.p->x[i]-w->p[0]->d.p->x[i];
    d2.x[i]=w->p[3]->d.p->x[i]-w->p[1]->d.p->x[i]; }
 normalize(&d1); normalize(&d2);
 VECTOR(&n,&d1,&d2); 
 if(SCALAR(&n,&view.e[2])>=0.5) return 4*M_PI; /* 0.5=cos(MAX_VIEWANGLE) */
 /* Calculate angle between x-axis of screen and u-axis of texture */
 a=acos((c[1].x[0]-c[0].x[0])/
  sqrt((c[1].x[0]-c[0].x[0])*(c[1].x[0]-c[0].x[0])+
   (c[1].x[1]-c[0].x[1])*(c[1].x[1]-c[0].x[1])));
 if(c[1].x[1]-c[0].x[1]<0) a=-a;
 for(i=0;i<3;i++) d1.x[i]=w->p[1]->d.p->x[i]-w->p[0]->d.p->x[i];
 b=acos(SCALAR(&view.e[0],&d1)/LENGTH(&d1));
 if(SCALAR(&view.e[1],&d1)>0) b=-b;
 return a+b;
 }
 
void moveobj_mouse(int withtagged,int wx,int wy,struct node *nc,int wall)
 { 
 struct node *np,*n;
 struct list *cubepl,*sidepl,*pntpl,cubel,sidel,pntl;
 unsigned char *lockedsides=NULL,curlocked[6];
 int i,j,c;
 float angle;
 struct point p;
 union move_params params;
 if(nc==NULL || l==NULL) return;
 initlist(&cubel); initlist(&sidel); initlist(&pntl); 
 cubepl=&cubel; pntpl=&pntl; sidepl=&sidel;
 /* basic initialization for pntlists */
 params.p.pnt_list=pntpl; params.p.cube_list=cubepl;
 params.p.side_list=sidepl; params.p.ref_pnt=&p; params.p.cube=nc;
 switch(view.currmode)
  {
  case tt_cube: 
   for(i=0;i<8;i++) addpnttomovelist(pntpl,cubepl,nc->d.c->p[i]);   
   for(j=0;j<3;j++)
    { p.x[j]=0.0; for(i=0;i<8;i++) p.x[j]+=nc->d.c->p[i]->d.p->x[j]/8.0; }
   fittogrid(&p);
   if(withtagged&1)
    for(n=l->tagged[tt_cube].head,c=0;n->next!=NULL;n=n->next,c++)
     for(i=0;i<8;i++) addpnttomovelist(pntpl,cubepl,n->d.n->d.c->p[i]);
   makesidemovelist(pntpl,sidepl);
   redrawcubes(pntpl,cubepl);
   for(i=0;i<6;i++)
    if(nc->d.c->walls[i])
     { curlocked[i]=nc->d.c->walls[i]->locked; nc->d.c->walls[i]->locked=1; }
   if(withtagged&1)
    {
    checkmem(lockedsides=MALLOC(l->tagged[tt_cube].size*6));
    for(n=l->tagged[tt_cube].head,c=0;n->next!=NULL;n=n->next,c++)
     for(i=0;i<6;i++) if(n->d.n->d.c->walls[i]) 
      { lockedsides[c*6+i]=n->d.n->d.c->walls[i]->locked;
        n->d.n->d.c->walls[i]->locked=1; }
    }
   params.p.wall=-1;
   moving_with_mouse(l->w,wx,wy,movepnt,rotatepnt,&p,&params); 
   for(n=cubepl->head;n->next!=NULL;n=n->next)
    for(i=0;i<6;i++) recalcwall(n->d.n->d.c,i);
   killlist(cubepl); killlist(pntpl);
   if(withtagged&1)
    { for(n=l->tagged[tt_cube].head,c=0;n->next!=NULL;n=n->next,c++) 
       for(i=0;i<6;i++) if(n->d.n->d.c->walls[i])
        n->d.n->d.c->walls[i]->locked=lockedsides[c*6+i];
      FREE(lockedsides); }
   for(i=0;i<6;i++)
    if(nc->d.c->walls[i]) nc->d.c->walls[i]->locked=curlocked[i];
   drawopt(in_edge); drawopt(in_wall); plotlevel();
   break;
  case tt_wall:
   if((withtagged&2)==0)
    { /* move points of wall */
    for(j=0;j<3;j++)
     { p.x[j]=0.0; 
       for(i=0;i<4;i++) p.x[j]+=nc->d.c->p[wallpts[wall][i]]->d.p->x[j]/4.0; }
    fittogrid(&p);
    for(i=0;i<4;i++) 
     addpnttomovelist(pntpl,cubepl,nc->d.c->p[wallpts[wall][i]]);
    if(withtagged&1)
     for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
      for(i=0;i<4;i++)
       addpnttomovelist(pntpl,cubepl,n->d.n->d.c->p[wallpts[n->no%6][i]]);
    makesidemovelist(pntpl,sidepl);
    redrawcubes(pntpl,cubepl);
    params.p.wall=wall;
    moving_with_mouse(l->w,wx,wy,movepnt,
     rotatepnt,&p,&params);
    for(n=cubepl->head;n->next!=NULL;n=n->next)
     { if(n->d.n==nc || testtag(tt_cube,n->d.n)) continue;
       for(i=0;i<6;i++) recalcwall(n->d.n->d.c,i); }
    killlist(cubepl); killlist(pntpl);
    drawopt(in_edge);
    }
   else /* move texture of wall */
    if(nc->d.c->walls[wall])
     {
     angle=txt_calcangle(nc->d.c->walls[wall]);
     fprintf(errf,"angle=%g\n",angle/M_PI);
     if(angle>2*M_PI) { printmsg(TXT_CANTSEETXTWALL); return; }
     move_texture_with_mouse(w_xwinincoord(l->w,wx),w_ywinincoord(l->w,wy),
      NULL,nc,wall,withtagged&1,angle,plotlevel);
     }
   drawopt(in_wall); plotlevel();
   break;
  case tt_pnt: 
   np=nc;
   addpnttomovelist(pntpl,cubepl,np);
   if(withtagged&1)
    for(n=l->tagged[tt_pnt].head;n->next!=NULL;n=n->next)
     addpnttomovelist(pntpl,cubepl,n->d.n);
   makesidemovelist(pntpl,sidepl);
   redrawcubes(pntpl,cubepl); 
   params.p.ref_pnt=NULL; params.p.cube=NULL; params.p.wall=-1;
   moving_with_mouse(l->w,wx,wy,movepnt,rotatepnt,np->d.p,&params);
   for(n=pntpl->head;n->next!=NULL;n=n->next) newcorners(n);
   killlist(cubepl); killlist(pntpl);
   drawopt(in_edge); drawopt(in_wall); plotlevel();
   break;
  case tt_thing: 
   plotcurrent(); 
   /* kill the not-xored object and draw a xored instead */
   plotthing(nc->d.t,256); plotthing(nc->d.t,-1);
   if(withtagged&1) 
    for(n=l->tagged[tt_thing].head;n->next!=NULL;n=n->next)
     { plotthing(n->d.n->d.t,256); plotthing(n->d.n->d.t,-1); } 
   params.t.withtagged=withtagged&1; params.t.nc=nc;
   moving_with_mouse(l->w,wx,wy,movething,turnthing,&nc->d.t->p[0],&params);
   drawopt(in_thing); 
   break;
  default: printmsg(TXT_NOMOVINGMODE); return;
  }
 l->levelsaved=0;
 }

void movecorr_mouse(struct w_window *w,int wx,int wy,struct track *t)
 { union move_params params;
   params.c.t=t; params.c.c=l->cur_corr;
   moving_with_mouse(w,wx,wy,movecorr,turncorr,&t->x,&params); }
 
void moveyou_mouse(struct w_window *w,int wx,int wy)
 {
 struct ws_event ws;
 int xs,lr,ox,oy,sx,sy,i;
 struct point e0,er[3],x0,np,new_e0;
 float l,fx,fy;
 unsigned long dt;
 struct lightsource *ls;
 clock_t t1,t2;
 xs=view.whichdisplay ? w_xwininsize(w)/2 : w_xwininsize(w);
 lr=(wx>=xs);
 initcoordsystem(lr,&e0,er,&x0);
 sx=w_xwinincoord(w,wx); sy=w_ywinincoord(w,wy);
 ox=w_xmaxwinsize()/2; oy=w_ymaxwinsize()/2;
 ws_erasemouse();
 ws_mousewarp(ox,oy);
/* do ws_getevent(&ws,1); while(ws.buttons&ws_bt_right); */
 render_enablelights();
 t1=t2=clock();
 do
  {
  if((dt=cont_plotlevel(&ls))>0 && view.blinkinglightson &&
    view.warn_frameratetoosmall) break;
  else dt=0;
  if((ws.kbstat&ws_ks_ctrl)==0) t1=clock();
  ws_getevent(&ws,0);
  if(ws.flags==ws_f_keypress)
   {
   for(i=0;i<view.num_keycodes;i++)
    {
    if(view.ec_keycodes[i].key==ws.key &&
     (view.ec_keycodes[i].kbstat&ws.kbstat)==view.ec_keycodes[i].kbstat)
     if(view.ec_keycodes[i].event>=0) 
      { do_event[view.ec_keycodes[i].event](view.ec_keycodes[i].event); 
        ws.flags=ws_f_none; break; }
    }
   }
  if(ws.x==ox && ws.y==oy && (ws.kbstat&ws_ks_ctrl)==0) continue;
  if((ws.x-ox)*8>xs) ws.x=ox+xs/8;
  else if((ox-ws.x)*8>xs) ws.x=ox-xs/8;
  if((ws.y-oy)*8>w_ywininsize(w)) ws.y=oy+w_ywininsize(w)/8;
  else if((oy-ws.y)*8>w_ywininsize(w)) ws.y=oy-w_ywininsize(w)/8;
  ws_mousewarp(ox,oy);
  fx=(float)(ws.x-ox)/xs;
  fy=(float)(ws.y-oy)/w_ywininsize(w);
  if((ws.kbstat&ws_ks_ctrl)!=0)
   { t2=clock();
     for(i=0;i<3;i++) new_e0.x[i]=view.e0.x[i]+
      5.0*view.movefactor*(t2-t1)/(float)CLOCKS_PER_SEC*er[2].x[i];
     t1=t2; }
  else new_e0=view.e0;
  if((ws.kbstat&ws_ks_shift)!=0 && (ws.buttons&ws_bt_left)!=0)
   {
   fx*=-2*M_PI*((view.mouse_flipaxis&2)!=0 ? -1 : 1);
   fy*=-2*M_PI*((view.mouse_flipaxis&1)!=0 ? -1 : 1);
   for(i=0;i<3;i++) np.x[i]=er[1].x[i]*fx+er[0].x[i]*fy;
   l=LENGTH(&np);
   if(l!=0)
    { for(i=0;i<3;i++) np.x[i]/=l;
      for(i=0;i<3;i++) turnsinglepnt(&np,cos(l),sin(l),&view.e[i]); }
   }
  else if((ws.kbstat&ws_ks_shift)!=0)
   {
   fx*=view.movefactor*20.0; fy*=-view.movefactor*20.0;
   for(i=0;i<3;i++) new_e0.x[i]+=fx*er[0].x[i]+fy*er[1].x[i];
   }
  else if((ws.buttons&ws_bt_left)==0)
   {
   if(fabs(fx)>fabs(fy)) fy=0.0; else fx=0.0;
   fx*=-2*M_PI*((view.mouse_flipaxis&4)!=0 ? -1 : 1);
   fy*=-view.movefactor*20.0; 
   for(i=0;i<3;i++) new_e0.x[i]+=fy*er[2].x[i];
   for(i=0;i<3;i++) turnsinglepnt(&er[2],cos(fx),sin(fx),&view.e[i]);
   } 
  else
   {
   fx*=2*M_PI*((view.mouse_flipaxis&2)!=0 ? -1 : 1);
   fy*=2*M_PI*((view.mouse_flipaxis&1)!=0 ? -1 : 1);
   for(i=0;i<3;i++) np.x[i]=er[1].x[i]*fx+er[0].x[i]*fy;
   l=LENGTH(&np);
   if(l!=0)
    { for(i=0;i<3;i++)
       { np.x[i]/=l; 
         if(view.whichdisplay) 
	  new_e0.x[i]-=(lr?-view.e[0].x[i]:view.e[2].x[i])*view.distcenter; }
      for(i=0;i<3;i++) turnsinglepnt(&np,cos(l),sin(l),&view.e[i]);
      if(view.whichdisplay)
       for(i=0;i<3;i++) 
        new_e0.x[i]+=(lr?-view.e[0].x[i]:view.e[2].x[i])*view.distcenter; }
   }
  for(i=0;i<3;i++) normalize(&view.e[i]);
  move_user(&new_e0);
  initcoordsystem(lr,&e0,er,&x0); 
  }
 while(!(ws.flags==ws_f_keypress || (ws.flags==ws_f_rbutton &&
  (ws.buttons&ws_bt_right)==0)));
 render_disablelights(); plotlevel();
 ws_mousewarp(sx,sy);
 ws_displaymouse();
 if(dt>0 && yesnomsg(TXT_FRAMERATETOOSMALL,
  dt/65536.0*view.timescale,ls->cube->no,(int)ls->w,
  ls->fl->delay/65536.0,view.timescale*dt/ls->fl->delay*2.0))
  { view.timescale*=(float)dt/ls->fl->delay*2.0; drawopts(); }
 }

extern float xviewphi,yviewphi;
void dec_savepos(int ec)
 {
 int num_pos=ec-ec_savepos_0,i;
 struct list *pts;
 struct node *n;
 struct point d1,d2;
 float x,y,z,min_z,max_x,max_y,min_x,min_y,scotansq;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 my_assert(num_pos>=0 && num_pos<NUM_SAVED_POS);
 l->saved_pos[num_pos].e0=view.e0;
 l->saved_pos[num_pos].e[0]=view.e[0];
 l->saved_pos[num_pos].e[1]=view.e[1];
 l->saved_pos[num_pos].e[2]=view.e[2];
 l->saved_pos[num_pos].max_vis=view.maxvisibility;
 l->saved_pos[num_pos].distcenter=view.distcenter;
 if(num_pos<10) printmsg(TXT_POSSAVED,num_pos);
 else /* make overview */
  { 
  if(num_pos==11) /* of level */ pts=&l->pts;
  else
   if(l->cur_corr) pts=&l->cur_corr->points;
   else { printmsg(TXT_NOCORR); return; }
  for(n=pts->head,max_x=max_y=-FLT_MAX,min_x=min_y=min_z=FLT_MAX;
   n->next!=NULL;n=n->next)
   {
   for(i=0;i<3;i++) d1.x[i]=n->d.p->x[i]-view.e0.x[i];
   x=SCALAR(&d1,&view.e[2]);
   for(i=0;i<3;i++) d2.x[i]=d1.x[i]-x*view.e[2].x[i];
   /* d2 is the the shortest possible vector between the line e0+t*e2
      and the point n->d.p (and therefore orthogonal to e2) */
   x=SCALAR(&d2,&view.e[0]); y=SCALAR(&d2,&view.e[1]);
   if(x>max_x) max_x=x; if(x<min_x) min_x=x;
   if(y>max_y) max_y=y; if(y<min_y) min_y=y;
   }
  for(i=0;i<3;i++)
   view.e0.x[i]+=view.e[0].x[i]*(max_x+min_x)/2.0+
    view.e[1].x[i]*(max_y+min_y)/2.0;
  scotansq=xviewphi<yviewphi ? xviewphi : yviewphi;
  scotansq=scotansq*scotansq/(1.0-scotansq*scotansq);
  for(n=pts->head,min_z=FLT_MAX;n->next!=NULL;n=n->next)
   {
   for(i=0;i<3;i++) d1.x[i]=n->d.p->x[i]-view.e0.x[i];
   z=SCALAR(&d1,&view.e[2]); x=SCALAR(&d1,&d1);
   z-=sqrt((x-z*z)*scotansq);
   if(z<min_z) min_z=z;
   }
  for(i=0;i<3;i++) view.e0.x[i]+=(num_pos==11 ? min_z : min_z*1.2)*
   view.e[2].x[i]; 
  for(n=pts->head,z=min_z=0.0;n->next!=NULL;n=n->next)
   { for(i=0;i<3;i++) d1.x[i]=n->d.p->x[i]-view.e0.x[i];
     if(LENGTH(&d1)>min_z) min_z=LENGTH(&d1);
     z+=SCALAR(&d1,&view.e[2]); }
  view.maxvisibility=min_z*2;
  if(l->whichdisplay) 
   { view.distcenter=z/pts->size;
     for(i=0;i<3;i++) view.e0.x[i]+=view.distcenter*view.e[2].x[i]; }
  plotlevel();
  }
 }
 
void dec_gotopos(int ec)
 {
 int num_pos=ec-ec_gotopos_0;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 my_assert(num_pos>=0 && num_pos<NUM_SAVED_POS);
 view.e0=l->saved_pos[num_pos].e0;
 view.e[0]=l->saved_pos[num_pos].e[0];
 view.e[1]=l->saved_pos[num_pos].e[1];
 view.e[2]=l->saved_pos[num_pos].e[2];
 view.maxvisibility=l->saved_pos[num_pos].max_vis;
 view.distcenter=l->saved_pos[num_pos].distcenter;
 init_rendercube(); plotlevel(); 
 }

void init_rendercube(void)
 {
 struct point e0,er[3],x0;
 struct node *c;
 if(!l) return;
 initcoordsystem(0,&e0,er,&x0);
 c=findpntcube(&l->cubes,&e0);
 if(c!=NULL) { l->rendercube=c; l->inside=1; }
 else l->inside=0;
 }

