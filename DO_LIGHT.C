/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_light.c - functions for automatic illuminating
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
#include "tag.h"
#include "options.h"
#include "plot.h"
#include "initio.h"
#include "do_event.h"
#include "do_light.h"
#include "lac_cfg.h"

extern int init_test;

#define ILLUM_SUBGRIDSIZE 4 /* 4*4 checkpoints for each lightsource */
#define MAXLSCOORD 2048 /* Max. Coord for a lightsource. The lightsources
 at the edge of the side are there */
#define MINFACTOR 0.001
#define ZERO 0.0
/* This is the max. distance where all the light is transmitted */
#define MINWAY 65536.0
/* Some pre-calculated values (calculated at the start of dec_mineillum)
   sqr_qw_mw=(quarterway-minway)^2
   qw_2mw=quarterway-2*minway */
float sqr_qw_mw,qw_2mw;
int maxdepth; /* =(view.quarterway/(10*65536.0))^2+4.5, approx. value */

/* set the light on c,wd,cd from the lightsource at spos with brightness 
 light. return a 0 if the transferred light is nearly 0, 1 otherwise */
int setcornerlight(struct point *spos,unsigned int light,
 struct node *n,int wd,int cd,unsigned short *add)
 {
 int j;
 float s,l;
 struct point d1,*epos=n->d.c->p[wallpts[wd][cd]]->d.p;
 unsigned long tmp;
 for(j=0;j<3;j++) d1.x[j]=epos->x[j]-spos->x[j];
 l=sqrt(SCALAR(&d1,&d1));
 s=(l<=MINWAY ? 1.0 : sqr_qw_mw/((l+qw_2mw)*(l+qw_2mw)));
 if(s<=MINFACTOR) return 0;
 if(n->d.c->walls[wd]!=NULL)
  add[wd*4+cd]=(tmp=add[wd*4+cd]+(unsigned long)(s*light))>theMaxLight ?
   theMaxLight :tmp; 
 return 1;
 }

/* check if the vector starting at p1 to p1+m runs through the
 triangle 0,r,s. t=r X s. */
int checkcolltriangle(struct point *p1,struct point *m,struct point *r,
 struct point *s,struct point *t)
 {
 float d,x1,x2;
 struct point h;
 if(init_test&8)
  fprintf(errf,
   "check trg: p1 %g %g %g m %g %g %g\n r %g %g %g s %g %g %g t %g %g %g\n",
   p1->x[0]/65536.0,p1->x[1]/65536.0,p1->x[2]/65536.0,
   m->x[0]/65536.0,m->x[1]/65536.0,m->x[2]/65536.0,
   r->x[0]/65536.0,r->x[1]/65536.0,r->x[2]/65536.0,
   s->x[0]/65536.0,s->x[1]/65536.0,s->x[2]/65536.0,
   t->x[0]/65536.0/65536.0,t->x[1]/65536.0/65536.0,t->x[2]/65536.0/65536.0);
 d=SCALAR(m,t);
 if(fabs(d)<=ZERO) /* the line runs parallel to the triangle */
  return fabs(SCALAR(t,p1))<=ZERO ? 1 : 0;
 else /* well, at least the line is not parallel to the plane of the wall */
  {
  d=1/d;
  x1=-SCALAR(p1,t)*d;
  if(x1<-ZERO || x1>1.0+ZERO) return 0;
  VECTOR(&h,p1,m);
  x1=-SCALAR(&h,s)*d;
  if(x1<-ZERO || x1>1.0+ZERO) return 0;
  x2=SCALAR(&h,r)*d;
  if(x2<-ZERO || x1+x2>1.0+ZERO) return 0;
  }
 return 1;
 }
 
/* procedure to check if the line from p1 to c->p[p] goes through all sides
 in arrays nc, nc_w. Number of sides: num_s. */
int checkforline(struct point *p1,struct point *p2,int num_s,
 struct node **nc,int *nc_w)
 {
 int side,i;
 struct point ea,r,s,t,m,a;
 /* m=p2-p1 */
 for(i=0;i<3;i++) m.x[i]=p2->x[i]-p1->x[i];
 if(init_test&8) fprintf(errf,"Checking p1 %g %g %g p2 %g %g %g num_s %d\n",
  p1->x[0]/65536.0,p1->x[1]/65536.0,p1->x[2]/65536.0,
  p2->x[0]/65536.0,p2->x[1]/65536.0,p2->x[2]/65536.0,num_s);
 for(side=num_s-1;side>=0;side--)
  {
  if(init_test&8)
   fprintf(errf,"side=%d nc %d nc_w %d\n",side,nc[side]->no,nc_w[side]);
  for(i=0;i<3;i++) 
   { ea.x[i]=nc[side]->d.c->p[wallpts[nc_w[side]][0]]->d.p->x[i];
     r.x[i]=nc[side]->d.c->p[wallpts[nc_w[side]][1]]->d.p->x[i]-ea.x[i];
     s.x[i]=nc[side]->d.c->p[wallpts[nc_w[side]][3]]->d.p->x[i]-ea.x[i]; 
     a.x[i]=p1->x[i]-ea.x[i]; }
  VECTOR(&t,&r,&s);
  if(!checkcolltriangle(&a,&m,&r,&s,&t))
   { /* missed the triangle at all */
   for(i=0;i<3;i++) 
    { ea.x[i]=nc[side]->d.c->p[wallpts[nc_w[side]][2]]->d.p->x[i];
      r.x[i]=nc[side]->d.c->p[wallpts[nc_w[side]][3]]->d.p->x[i]-ea.x[i];
      s.x[i]=nc[side]->d.c->p[wallpts[nc_w[side]][1]]->d.p->x[i]-ea.x[i]; 
      a.x[i]=p1->x[i]-ea.x[i]; }
   VECTOR(&t,&r,&s);
   if(!checkcolltriangle(&a,&m,&r,&s,&t)) return 0;
   else if(init_test&8) fprintf(errf,"2. checkcolltriangle true\n");
   }
  else if(init_test&8) fprintf(errf,"1. checkcolltriangle true\n");
  }
 return 1;
 }

int illum_checkwall(struct cube *c,int w)
 {
 if(c->d[w]==NULL) return 1;
 switch(c->d[w]->d.d->type1)
  {
  case door1_blow: case door1_normal: case door1_switchwithwall:
   return 0; break;
  case door1_onlytxt: case door1_shootthrough: 
   return (c->walls[w]->texture1<pig.num_rdltxts && 
   pig.rdl_txts[c->walls[w]->texture1].pig!=NULL &&
   pig.rdl_txts[c->walls[w]->texture1].txtlistno[txt1_wall]>=0 &&
   pig.rdl_txts[c->walls[w]->texture1].txtlistno[txt1_normal]<0) ||
   (c->walls[w]->texture2<pig.num_rdltxts && c->walls[w]->texture2!=0 &&
   pig.rdl_txts[c->walls[w]->texture2].pig!=NULL &&
   pig.rdl_txts[c->walls[w]->texture2].txtlistno[txt2_wall]>=0 &&
   pig.rdl_txts[c->walls[w]->texture2].txtlistno[txt2_normal]<0);
   break;
  case door1_onlyswitch: return 1; break;
  }
 return 0;
 }
 
/* init the list effects for the lightsources at lsp with light "light". 
 The lightsource is in the cube cube_n, depth=0, 
 nc and nc_w are two arrays of the size MAXDEPTH, depth=0. effects is an
 empty list */
void calclseffects(struct point *lsp,unsigned int light,struct node *cube_n,
 int depth,struct node **nc,int *nc_w,struct list *effects)
 { 
 int w,j,vis,onevis,wd,cd;
 unsigned short add[24];
 struct point endp;
 struct node *n;
 struct cube *c=cube_n->d.c;
 struct ls_effect *lse;
 for(w=0;w<6;w++)
  if(c->nc[w]!=NULL)
   {
   /* check: don't run in circles */
   for(j=depth;j>=0;j--) if(nc[j]==c->nc[w]) break;
   if(j>-1) continue;
   /* check: is this cube already check from this lightsource? */
   if(init_test&4)
    {
    fprintf(errf,"ILLUM: Checking cube %d from cube %d wall %d over ",
     c->nc[w]->no,cube_n->no,w);
    for(j=0;j<depth;j++) fprintf(errf," %d,%d",nc[j]->no,nc_w[j]);
    fprintf(errf,"\n");
    }
   for(n=effects->head,lse=NULL;n->next!=NULL;n=n->next)
    if(c->nc[w]==n->d.lse->cube) { lse=n->d.lse; break; }
   /* ok, a new cube. check if there's a wall which is in the way... */
   if(c->d[w]!=NULL)
    if(!illum_checkwall(c,w)) continue;
   /* no wall in the way. OK, no we have to check the neighbour cube
    c->nc[w]. The light must pass the side w of the cube w: */
   nc[depth]=cube_n; nc_w[depth]=w;
   /* now check all eight points from the neighbour cube */
   if(init_test&4) fprintf(errf,"Checking points...");
   for(wd=0,onevis=0;wd<6;wd++)
    for(cd=0;cd<4;cd++)
     {
     add[wd*4+cd]=0;
     for(j=0;j<3;j++)
      endp.x[j]=(c->nc[w]->d.c->p[wallpts[wd][(cd+1)&3]]->d.p->x[j]+
       c->nc[w]->d.c->p[wallpts[wd][(cd-1)&3]]->d.p->x[j])*0.01+
       c->nc[w]->d.c->p[wallpts[wd][cd]]->d.p->x[j]*0.98;
     /* check if the light can go from point p to point c->nc[w],p by
      passing all sides in the arrays nc, nc_w */
     vis=(c->nc[w]->d.c->nc[wd]!=cube_n &&
      checkforline(lsp,&endp,depth+1,nc,nc_w));
     if(vis && (!lse || lse->add_light[w*4+cd]==0)) 
      onevis|=setcornerlight(lsp,light,c->nc[w],wd,cd,add);
     if(init_test&4) fprintf(errf," %d:%d:%d:%d\n",wd,cd,vis,add[wd*4+cd]);
     }
   if(init_test&4) fprintf(errf," %d\n",onevis);
   /* not one of the points is visible */
   if(!onevis) continue; /* check next neighbour */
   /* now make the corresponding lightsource effect */
   if(!lse)
    { checkmem(lse=MALLOC(sizeof(struct ls_effect)));
      lse->cube=c->nc[w]; 
      for(j=0;j<24;j++) { lse->add_light[j]=0; lse->smoothed[j]=0; }
      checkmem(addnode(effects,-1,lse)); }
   for(j=0;j<24;j++) /* only change if add!=0 for corners that are
    illuminated along other ways */
    if(add[j]!=0) lse->add_light[j]=add[j];
   /* OK. All points of the neighbour cube to wall w are checked and
      properly illuminated. At least one point was illuminated
      with more than insignificant light. So go one step
      further, i.e. make the same procedure with the neighbour cube */
   if(depth<maxdepth-1)
    calclseffects(lsp,light,c->nc[w],depth+1,nc,nc_w,effects);
   }  /* end of for(w=0..5) if(walls[w]!=NULL) for each neighbour of cube c */
 }
 
/* Calculate the effect from the lightsource light on side c,wall */
void calcillumwall(struct node *c,int wall,int *light)
 {
 int i,j,x,y,x2,y2,minx,maxx,miny,maxy,e1x,e1y,e2x,e2y,e3x,e3y,e4x,e4y,d1,d2,
  px,py,dp1,dp2=0,*nc_w,gx,gy,ox,oy,sub_in_sum,
  sub_in1[ILLUM_SUBGRIDSIZE*ILLUM_SUBGRIDSIZE],
  sub_in2[ILLUM_SUBGRIDSIZE*ILLUM_SUBGRIDSIZE];
 struct wall *w=c->d.c->walls[wall];
 struct node **nc,*n,*ne;
 struct point center,e1,e2,e3,e4;
 float sub_a[ILLUM_SUBGRIDSIZE*ILLUM_SUBGRIDSIZE],
  sub_b[ILLUM_SUBGRIDSIZE*ILLUM_SUBGRIDSIZE],a,b;
 struct point lsp;
 struct lightsource *ls;
 struct list effects,illum_cubes;
 struct ls_effect *lse;
 unsigned long tmp;
 if(init_test&4) 
  fprintf(errf,"Calculating effects of light source %d,%d\n",c->no,wall);
 checkmem(nc_w=MALLOC(sizeof(int)*maxdepth));
 checkmem(nc=MALLOC(sizeof(struct node *)*maxdepth));
 minx=miny=65536; maxx=maxy=-65536;
 for(i=0;i<4;i++)
  { if(minx>w->corners[i].x[0]) minx=w->corners[i].x[0];
    if(maxx<w->corners[i].x[0]) maxx=w->corners[i].x[0];
    if(miny>w->corners[i].x[1]) miny=w->corners[i].x[1];
    if(maxy<w->corners[i].x[1]) maxy=w->corners[i].x[1]; }
 minx=((minx<0 ? minx-2047 : minx)/2048)*2048;
 miny=((miny<0 ? miny-2047 : miny)/2048)*2048;
 maxx=((maxx<0 ? maxx-2047 : maxx)/2048)*2048; 
 maxy=((maxy<0 ? maxy-2048 : maxy)/2048)*2048;
 e1x=w->corners[1].x[0]-w->corners[0].x[0];
 e1y=w->corners[1].x[1]-w->corners[0].x[1];
 e2x=w->corners[3].x[0]-w->corners[0].x[0];
 e2y=w->corners[3].x[1]-w->corners[0].x[1];
 e3x=w->corners[1].x[0]-w->corners[2].x[0];
 e3y=w->corners[1].x[1]-w->corners[2].x[1];
 e4x=w->corners[3].x[0]-w->corners[2].x[0];
 e4y=w->corners[3].x[1]-w->corners[2].x[1];
 d1=e1x*e2y-e1y*e2x; d2=e3x*e4y-e3y*e4x;
 if(d1==0 || d2==0) 
  { waitmsg(TXT_CANTCALCLIGHT,c->no,wall); return; }
 for(i=0;i<3;i++) 
  { center.x[i]=0.0;
    for(x=0;x<8;x++) center.x[i]+=c->d.c->p[x]->d.p->x[i];
    center.x[i]/=8;
    e1.x[i]=w->p[1]->d.p->x[i]-w->p[0]->d.p->x[i];
    e2.x[i]=w->p[3]->d.p->x[i]-w->p[0]->d.p->x[i];
    e3.x[i]=w->p[1]->d.p->x[i]-w->p[2]->d.p->x[i];
    e4.x[i]=w->p[3]->d.p->x[i]-w->p[2]->d.p->x[i]; }
 initlist(&effects);
 checkmem(lse=MALLOC(sizeof(struct ls_effect)));
 checkmem(addnode(&effects,-1,lse));
 for(i=0;i<24;i++) { lse->add_light[i]=0; lse->smoothed[i]=0; }
 lse->cube=c; 
 if(init_test&4)
  fprintf(errf,"cube %d wall %d: %hd,%hd %hd,%hd %hd,%hd %hd,%hd\n",
   c->no,wall,w->corners[0].x[0],w->corners[0].x[1],
   w->corners[1].x[0],w->corners[1].x[1],
   w->corners[2].x[0],w->corners[2].x[1],
   w->corners[3].x[0],w->corners[3].x[1]);
 for(gx=minx;gx<=maxx;gx+=2048)
 for(gy=miny;gy<=maxy;gy+=2048)
  for(x=0;x<ILLUM_GRIDSIZE;x++)
  for(y=0;y<ILLUM_GRIDSIZE;y++)
   {
   sub_in_sum=0;
   for(x2=0;x2<ILLUM_SUBGRIDSIZE;x2++)
   for(y2=0;y2<ILLUM_SUBGRIDSIZE;y2++)
    {
    a=b=0.0;
    ox=gx+x*MAXLSCOORD/ILLUM_GRIDSIZE+ 
     (2*x2+1)*(MAXLSCOORD/ILLUM_GRIDSIZE)/ILLUM_SUBGRIDSIZE/2;
    oy=gy+y*MAXLSCOORD/ILLUM_GRIDSIZE+
     (2*y2+1)*(MAXLSCOORD/ILLUM_GRIDSIZE)/ILLUM_SUBGRIDSIZE/2;
    px=ox-w->corners[0].x[0];
    py=oy-w->corners[0].x[1];
    dp1=px*e2y-py*e2x;
    a=(float)dp1/d1; 
    /* a is the factor before e1 in the linear combination of p, b -> e2,
     so a*e1+b*e2=p */
    sub_in1[y2*ILLUM_SUBGRIDSIZE+x2]=sub_in2[y2*ILLUM_SUBGRIDSIZE+x2]=0;
    if(a>=0.0 && a<=1.0)
     { dp2=e1x*py-e1y*px; b=(float)dp2/d1;
       sub_in_sum+=sub_in1[y2*ILLUM_SUBGRIDSIZE+x2]=(b>=0.0 && a+b<=1.0); }
    if(!sub_in1[y2*ILLUM_SUBGRIDSIZE+x2])
     {
     px=ox-w->corners[2].x[0];
     py=oy-w->corners[2].x[1];
     dp1=px*e4y-py*e4x; a=(float)dp1/d2;
     if(a>=0.0 && a<=1.0)
      { dp2=e3x*py-e3y*px; b=(float)dp2/d2; 
        sub_in_sum+=sub_in2[y2*ILLUM_SUBGRIDSIZE+x2]=(b>=0.0 && a+b<=1.0); }
     }
    sub_a[y2*ILLUM_SUBGRIDSIZE+x2]=a; sub_b[y2*ILLUM_SUBGRIDSIZE+x2]=b;
    }
   if(init_test&4)
    fprintf(errf,"Cube %d wall %d x %d,%d y %d,%d sub_in_sum %d\n",
     c->no,wall,gx,x,gy,y,sub_in_sum);
   if(sub_in_sum>ILLUM_SUBGRIDSIZE) 
    /* if it is only one row it is neglected */
    /* this point of the grid lies in the inside of the side */
    {
    for(i=0;i<3;i++) lsp.x[i]=0.0;
    for(x2=0;x2<ILLUM_SUBGRIDSIZE;x2++)
    for(y2=0;y2<ILLUM_SUBGRIDSIZE;y2++)
     if(sub_in1[j=y2*ILLUM_SUBGRIDSIZE+x2] || sub_in2[j])
      {
      if(sub_in1[j])
       for(i=0;i<3;i++)
        lsp.x[i]+=w->p[0]->d.p->x[i]+sub_a[j]*e1.x[i]+sub_b[j]*e2.x[i];
      else
       for(i=0;i<3;i++)
        lsp.x[i]+=w->p[2]->d.p->x[i]+sub_a[j]*e3.x[i]+sub_b[j]*e4.x[i];
      }
    /* move the lightsource a bit into the middle of the cube, away from
       the wall, so we don't get trouble with flat walls */
    for(i=0;i<3;i++)
     lsp.x[i]=lsp.x[i]/sub_in_sum*0.99+center.x[i]*0.01;
    a=sub_in_sum>=ILLUM_SUBGRIDSIZE*(ILLUM_SUBGRIDSIZE-1) ? 1.0 :
     (float)(sub_in_sum+ILLUM_SUBGRIDSIZE)/
     (ILLUM_SUBGRIDSIZE*ILLUM_SUBGRIDSIZE);
    if(init_test&4)
     fprintf(errf,"-->a %g pos %g %g %g light %d\n",
      a,lsp.x[0]/65536.0,lsp.x[1]/65536.0,
      lsp.x[2]/65536.0,(int)(light[y*ILLUM_GRIDSIZE+x]*a));
    /* a is between 1/2 and 1 */
    for(i=0;i<6;i++)
     if(c->d.c->walls[i])
      for(j=0;j<4;j++)
       setcornerlight(&lsp,(unsigned int)(light[y*ILLUM_GRIDSIZE+x]*a),c,i,j,
        lse->add_light);
    initlist(&illum_cubes);
    calclseffects(&lsp,(unsigned int)(light[y*ILLUM_GRIDSIZE+x]*a),c,0,nc,
     nc_w,&illum_cubes);
    for(n=illum_cubes.head->next;n!=NULL;n=n->next)
     {
     for(ne=effects.head;ne->next!=NULL;ne=ne->next)
      if(ne->d.lse->cube==n->prev->d.lse->cube) break;
     if(ne->next)
      { for(i=0;i<24;i++) 
         ne->d.lse->add_light[i]=
	  (tmp=ne->d.lse->add_light[i]+
           n->prev->d.lse->add_light[i])>theMaxLight ? theMaxLight : tmp;
        ne= n->prev; FREE(ne->d.lse); unlistnode(&illum_cubes, ne); FREE(ne); }
     else
     {
        ne = n->prev;
        unlistnode(&illum_cubes, ne);
        listnode_tail(&effects, ne);
      }
     }
    }
   }
 checkmem(ls=MALLOC(sizeof(struct lightsource)));
 copylisthead(&ls->effects,&effects);
 ls->cube=c; ls->w=wall; ls->fl=NULL;
 my_assert(c->d.c->walls[wall]!=NULL)
 checkmem(c->d.c->walls[wall]->ls=addnode(&l->lightsources,-1,ls));
 if(init_test&4)
  {
  fprintf(errf,"Light from wall %d %d\n",ls->cube->no,ls->w);
  for(n=ls->effects.head;n->next!=NULL;n=n->next)
   {
   fprintf(errf," Cube %d:",n->d.lse->cube->no);
   for(i=0;i<24;i++) 
    if(n->d.lse->add_light[i]!=0) fprintf(errf," %d,%d:%d",i/4,i%4,
     n->d.lse->add_light[i]);
   fprintf(errf,"\n");
   }
  }
 FREE(nc); FREE(nc_w);
 }
 
#define MAX_CORNERNB 20
/* this is cos(phi), phi=max. angle to smooth the edges */
#define MAX_SMOOTHANGLE 0.87 

struct smoothcorner
 {
 struct node *cube;
 int w[3],c[3],smooth[3];
 struct ls_effect *lse;
 };
int index;

/* store the part of vv which is orthogonal to vt in ve. vt must be 
   normalized */
void ortho(struct point *ve,struct point *vv,struct point *vt)
 {
 int i;
 float x;
 x=SCALAR(vv,vt);
 for(i=0;i<3;i++) ve->x[i]=vv->x[i]-vt->x[i]*x;
 }
 
/* get all corners which have the same point as ncube,w,c and the cos(angle)
 between the normalvector from the wall ncube,w and the normalvector from the
 new wall should be greater than MAX_SMOOTHANGLE */
int rec_getnbcorners(struct smoothcorner *sm_corners,int smc_length,
 struct node *nc,int w,int c,struct point *cmp)
 {
 int i,sc,c2,w2,pno,p2,new_smcl;
 struct point d1,d2,nv;
 my_assert(nc!=NULL && w>=0 && w<6 && c>=0 && c<4 && cmp!=NULL && 
  smc_length<MAX_CORNERNB);
 pno=wallpts[w][c];
 /* set this cube in the array because it is checked */
 sm_corners[smc_length].cube=nc; new_smcl=smc_length+1;
 /* run through all three corners */
 for(sc=0;sc<3;sc++)
  {
  sm_corners[smc_length].w[sc]=w2=wallno[pno][0][sc]; 
  sm_corners[smc_length].c[sc]=c2=wallno[pno][1][sc];
  sm_corners[smc_length].smooth[sc]=0;
  if(nc->d.c->walls[w2])
   {
   for(i=0;i<3;i++)
    { d1.x[i]=nc->d.c->walls[w2]->p[(c2+1)&3]->d.p->x[i]-
       nc->d.c->p[pno]->d.p->x[i];
      d2.x[i]=nc->d.c->walls[w2]->p[(c2-1)&3]->d.p->x[i]-
       nc->d.c->p[pno]->d.p->x[i]; }
   VECTOR(&nv,&d1,&d2); normalize(&nv);
   sm_corners[smc_length].smooth[sc]=(SCALAR(&nv,cmp)>=MAX_SMOOTHANGLE);
   }
  if(nc->d.c->nc[w2] && illum_checkwall(nc->d.c,w2))
   {
   for(i=0;i<smc_length;i++) 
   if(nc->d.c->nc[w2]==sm_corners[i].cube) break;
   if(i==smc_length)
    { /* new cube */
    for(p2=0;p2<8;p2++) if(nc->d.c->nc[w2]->d.c->p[p2]==nc->d.c->p[pno])break;
    if(p2<8) 
     {
     for(i=0;i<3;i++) if(nc->d.c->nc[w2]->d.c->nc[wallno[p2][0][i]]==nc)break;
     if(i<3)
      new_smcl=rec_getnbcorners(sm_corners,new_smcl,nc->d.c->nc[w2],
       wallno[p2][0][i],wallno[p2][1][i],cmp);
     else if(init_test&4) fprintf(errf,
      "ERROR rec_getnbcorners: nc=%d nc[w2]=%d w2=%d pno=%d p2=%d i=%d\n",
      nc->no,nc->d.c->nc[w2]->no,w2,pno,p2,i);
     }
    else if(init_test&4) fprintf(errf,
     "ERROR rec_getnbcorners: nc=%d nc[w2]=%d w2=%d pno=%d p2=%d\n",
     nc->no,nc->d.c->nc[w2]->no,w2,pno,p2);
    }
   }
  }
 return new_smcl;
 }
 
int getnbcorners(struct smoothcorner *nbcorners,struct node *c,
 int w,int p)
 {
 int i;
 struct point d1,d2,cmp;
 for(i=0;i<3;i++)
  { d1.x[i]=c->d.c->walls[w]->p[(p+1)&3]->d.p->x[i]-
     c->d.c->walls[w]->p[p]->d.p->x[i];
    d2.x[i]=c->d.c->walls[w]->p[(p-1)&3]->d.p->x[i]-
     c->d.c->walls[w]->p[p]->d.p->x[i]; }
 VECTOR(&cmp,&d1,&d2); normalize(&cmp);
 return rec_getnbcorners(nbcorners,0,c,w,p,&cmp);
 }

void smoothlight(void)
 {
 struct node *nls,*nlse,*nlse2;
 int progress,last_progress=-1,num_nbs,w,p,sum_light,num_light,i,j;
 struct list new_effects;
 struct ls_effect *lse;
 struct smoothcorner nbcorners[MAX_CORNERNB];
 for(nls=l->lightsources.head;nls->next!=NULL;nls=nls->next)
  {
  if((progress=(nls->no*100/l->lightsources.size)/5*5)!=last_progress)
   { printmsg(TXT_SMOOTHINGLIGHT,progress); last_progress=progress; }
  initlist(&new_effects);
  for(nlse=nls->d.ls->effects.head;nlse->next!=NULL;nlse=nlse->next)
   for(w=0;w<6;w++)
    for(p=0;p<4;p++)
     if(!nlse->d.lse->smoothed[w*4+p] && nlse->d.lse->add_light[w*4+p])
      {
      if(init_test&4)
       fprintf(errf,"Smoothing cube %d,wall %d,corner %d (light %d)\n",
        nlse->d.lse->cube->no,w,p,nlse->d.lse->add_light[w*4+p]);
      num_nbs=getnbcorners(nbcorners,nlse->d.lse->cube,w,p);
      for(i=0,sum_light=0,num_light=0;i<num_nbs;i++)
       {
       if(init_test&4)
        fprintf(errf,
         "%d. NB_corner: cube %d w %d,%d,%d c %d,%d,%d s %d,%d,%d\n",i,
         nbcorners[i].cube->no,nbcorners[i].w[0],nbcorners[i].w[1],
         nbcorners[i].w[2],nbcorners[i].c[0],nbcorners[i].c[1],
         nbcorners[i].c[2],nbcorners[i].smooth[0],nbcorners[i].smooth[1],
         nbcorners[i].smooth[2]);
       nbcorners[i].lse=NULL;
       if(nbcorners[i].cube==nlse->d.lse->cube)
        nbcorners[i].lse=nlse->d.lse;
       else for(nlse2=nls->d.ls->effects.head;nlse2->next!=NULL;
        nlse2=nlse2->next)
        if(nlse2->d.lse->cube==nbcorners[i].cube)
         nbcorners[i].lse=nlse2->d.lse;
       if(nbcorners[i].lse==NULL)
        {
        checkmem(lse=MALLOC(sizeof(struct ls_effect)));
        checkmem(addnode(&new_effects,-1,lse));
        lse->cube=nbcorners[i].cube;
        for(j=0;j<24;j++) { lse->add_light[j]=0; lse->smoothed[j]=0; }
        nbcorners[i].lse=lse;
        }
       for(j=0;j<3;j++)
        if(nbcorners[i].smooth[j] &&
	 nbcorners[i].cube->d.c->walls[nbcorners[i].w[j]]) 
         { sum_light+=nbcorners[i].lse->add_light[nbcorners[i].w[j]*4+
            nbcorners[i].c[j]]; num_light++; }
       }
      if(num_light>1)
       {
       sum_light/=num_light;
       for(i=0;i<num_nbs;i++)
        if(nbcorners[i].lse)
         for(j=0;j<3;j++) 
          if(nbcorners[i].smooth[j] &&
  	   nbcorners[i].cube->d.c->walls[nbcorners[i].w[j]]) 
    	    {
	    nbcorners[i].lse->smoothed[nbcorners[i].w[j]*4+
	     nbcorners[i].c[j]]=1;
            if(nbcorners[i].cube->d.c->walls[nbcorners[i].w[j]])
 	     {
 	     if(init_test&4)
 	      fprintf(errf,"Smoothing with cube %d(==%d) wall %d corner %d\n",
	       nbcorners[i].lse->cube->no,nbcorners[i].cube->no,
	       nbcorners[i].w[j],nbcorners[i].c[j]);
	     nbcorners[i].lse->add_light[nbcorners[i].w[j]*4+
              nbcorners[i].c[j]]=sum_light;
	     }
	    }
       }
      else if(init_test&4) fprintf(errf,"No Smoothing.\n");
      }
  if(new_effects.head->next)
   {
   new_effects.head->prev=nls->d.ls->effects.tail;
   new_effects.tail->next=nls->d.ls->effects.tail->next;
   nls->d.ls->effects.tail->next=new_effects.head;
   nls->d.ls->effects.tail=new_effects.tail;
   nls->d.ls->effects.size += new_effects.size;
   }
  }
 }
 
int read_lightsources(void)
 {
 FILE *f;
 int i,j,num,rdlnum,light;
 if((f=fopen(init.lightname,"r"))==NULL) 
  { waitmsg(TXT_CANTOPENLSFILE,init.lightname); return 0; }
 if(!findmarker(f,"LightSources",&num)) 
  { fclose(f); waitmsg(TXT_NOTALSFILE,init.lightname); return 0; }
 for(i=0;i<num;i++)
  {
  my_assert(fscanf(f,"%d",&rdlnum)==1);
  my_assert(rdlnum<pig.num_rdltxts);
  for(j=0;j<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE;j++)
   {
   my_assert(fscanf(f,"%d",&light)==1);
   pig.rdl_txts[rdlnum].my_light[j]=(light*32767+49)/100*
    view.illum_brightness;
   }
  } 
 fclose(f);
 return 1;
 }
 
void calccornerlight(int withsmooth)
 {
 struct node *ntc,*nlse;
 int ldrawn,per,w,c,i,j,x,y,light[ILLUM_GRIDSIZE*ILLUM_GRIDSIZE],overall;
 clock_t time1;
 struct ws_event ws;
 struct flickering_light fl;
 sortlist(&l->cubes,0);
 /* read the lightsource file */
 if(!read_lightsources()) return;
 /* Set all lights to the default values */
 time1=clock();
 sqr_qw_mw=(view.illum_quarterway-MINWAY)*(view.illum_quarterway-MINWAY);
 qw_2mw=view.illum_quarterway-2*MINWAY;
 maxdepth=(view.illum_quarterway/(10*65536.0))*
  (view.illum_quarterway/(10*65536.0))+4.5;
 /* reset the tagged edges */
 for(ntc=l->tagged[tt_cube].head,per=0,ldrawn=-10;ntc->next!=NULL;
  ntc=ntc->next,per++)
  {
  if((clock()-time1)/CLOCKS_PER_SEC>ldrawn)
   {
   ws_getevent(&ws,0);
   printmsg(TXT_CALCLIGHT,ntc->d.n->no,
    (per*100)/(float)l->tagged[tt_cube].size,(clock()-time1)/
    (float)CLOCKS_PER_SEC,init.lightname);
   ldrawn=(clock()-time1)/CLOCKS_PER_SEC;
   if(ws.key==27) break;
   }
  for(w=0;w<6;w++)
   if(ntc->d.n->d.c->walls[w]!=NULL)
    {
    fl.delay=0;
    if(ntc->d.n->d.c->walls[w]->ls)
     { /* an old lightsource. Delete it but keep the fl */
     if(ntc->d.n->d.c->walls[w]->ls->d.ls->fl)
      fl=*ntc->d.n->d.c->walls[w]->ls->d.ls->fl;
     freenode(&l->lightsources,ntc->d.n->d.c->walls[w]->ls,freelightsource);
     }
    ntc->d.n->d.c->walls[w]->ls=NULL;
    overall=0;
    if(ntc->d.n->d.c->walls[w]->texture1<pig.num_rdltxts &&
     pig.rdl_txts[ntc->d.n->d.c->walls[w]->texture1].pig!=NULL)
     for(i=0;i<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE;i++)
      overall+=(light[i]=
       pig.rdl_txts[ntc->d.n->d.c->walls[w]->texture1].my_light[i]);
    if(ntc->d.n->d.c->walls[w]->texture2<pig.num_rdltxts &&
     pig.rdl_txts[ntc->d.n->d.c->walls[w]->texture2].pig!=NULL &&
     ntc->d.n->d.c->walls[w]->texture2!=0)
     for(i=0;i<ILLUM_GRIDSIZE;i++)
      for(j=0;j<ILLUM_GRIDSIZE;j++)
       {
       switch(ntc->d.n->d.c->walls[w]->txt2_direction)
        {
        case 0: y=i; x=j; break;
        case 1: y=ILLUM_GRIDSIZE-1-j; x=i; break;
        case 2: y=ILLUM_GRIDSIZE-1-i; x=ILLUM_GRIDSIZE-1-j; break;
        case 3: y=j; x=ILLUM_GRIDSIZE-1-i; break;
        default: y=i; x=j; break;
        }
       overall+=(light[y*ILLUM_GRIDSIZE+x]+=
        pig.rdl_txts[ntc->d.n->d.c->walls[w]->texture2].my_light[
         i*ILLUM_GRIDSIZE+j]);
       }
    if(overall>0)
     {
     calcillumwall(ntc->d.n,w,light);
     if(fl.delay>0 && ntc->d.n->d.c->walls[w]->ls)
      {
      checkmem(ntc->d.n->d.c->walls[w]->ls->d.ls->fl=MALLOC(
       sizeof(struct flickering_light)));
      *ntc->d.n->d.c->walls[w]->ls->d.ls->fl=fl;
      ntc->d.n->d.c->walls[w]->ls->d.ls->fl->ls=ntc->d.n->d.c->walls[w]->ls;
      }
     }
    }
  }
 l->levelillum=1;
 if(withsmooth) smoothlight();
 /* OK, now set the lights & init the light lists */
 for(ntc=l->tagged[tt_cube].head;ntc->next!=NULL;ntc=ntc->next)
  for(w=0;w<6;w++) if(ntc->d.n->d.c->walls[w]!=NULL)
    for(c=0;c<4;c++) ntc->d.n->d.c->walls[w]->corners[c].light=
		      view.illum_minvalue;
 for(ntc=l->cubes.head;ntc->next!=NULL;ntc=ntc->next)
  freelist(&ntc->d.c->fl_lights,NULL);
 for(ntc=l->lightsources.head;ntc->next!=NULL;ntc=ntc->next)
  {
  if(init_test&4)
   fprintf(errf,"Light from cube %d wall %d\n",ntc->d.ls->cube->no,
    ntc->d.ls->w);
  for(nlse=ntc->d.ls->effects.head;nlse->next!=NULL;nlse=nlse->next)
   for(i=0;i<6;i++)
    if(nlse->d.lse->cube->d.c->walls[i])
     for(j=0;j<4;j++)
      if((overall=nlse->d.lse->add_light[i*4+j])!=0)
       {
       if(init_test&4) fprintf(errf," %d,%d,%d:%d",nlse->d.lse->cube->no,
        i,j,overall);
       if(ntc->d.ls->cube->d.c->tagged)
	{
	overall+=nlse->d.lse->cube->d.c->walls[i]->corners[j].light;
	nlse->d.lse->cube->d.c->walls[i]->corners[j].light=
         overall>theMaxLight ? theMaxLight : overall;
	}
       if(ntc->d.ls->fl!=NULL)
	checkmem(addnode(&nlse->d.lse->cube->d.c->fl_lights,-1,ntc->d.ls->fl));
       }
  }
 }
 
void setinnercubelight(void)
 {
 struct node *ntc;
 int overall,i,w,c;
 for(ntc=l->tagged[tt_cube].head->next;ntc!=NULL;ntc=ntc->next)
  {
  overall=0; i=0;
  for(w=0;w<6;w++)
   if(ntc->prev->d.n->d.c->walls[w]!=NULL)
    { i+=4;
      for(c=0;c<4;c++) 
       overall+=ntc->prev->d.n->d.c->walls[w]->corners[c].light; }
  if(i>0)
   { ntc->prev->d.n->d.c->light=overall/i; untag(tt_cube,ntc->prev->d.n); }
  }
 c=0; /* just that we run in no noend-loop, not really needed. */
 while(l->tagged[tt_cube].head->next!=NULL && c++<5)
  for(ntc=l->tagged[tt_cube].head->next;ntc!=NULL;ntc=ntc->next)
   {
   overall=0; i=0;
   for(w=0;w<6;w++) 
    if(ntc->prev->d.n->d.c->nc[w] && !ntc->prev->d.n->d.c->nc[w]->d.c->tagged)
     { overall+=ntc->prev->d.n->d.c->nc[w]->d.c->light; i++; }
   if(i>0) 
    { ntc->prev->d.n->d.c->light=overall/i; untag(tt_cube,ntc->prev->d.n); }
   } 
 }
 
void dec_setcornerlight(int ec)
 {
 clock_t time1;
 if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
 if(l->tagged[tt_cube].size==0) { printmsg(TXT_NOCUBETAGGED); return; }
 time1=clock();
 calccornerlight(ec==ec_mineillumsmooth);
 l->levelsaved=0;
 drawopts(); plotlevel();
 printmsg(TXT_ENDSETCORNERLIGHT,(clock()-time1)/(float)CLOCKS_PER_SEC);
 }
 
void dec_setinnercubelight(int ec)
 {
 if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
 if(l->tagged[tt_cube].size==0) { printmsg(TXT_NOCUBETAGGED); return; }
 setinnercubelight();
 l->levelsaved=0;
 drawopts(); plotlevel();
 printmsg(TXT_ENDSETINNERCUBELIGHT);
 }
 
void dec_mineillum(int ec)
 {
 clock_t time1;
 if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
 if(l->tagged[tt_cube].size==0) { printmsg(TXT_NOCUBETAGGED); return; }
 time1=clock();
 calccornerlight(isAlwaysSmoothing); setinnercubelight();
 l->levelsaved=0; l->levelillum=1;
 drawopts(); plotlevel();
 printmsg(TXT_ENDCALCLIGHT,(clock()-time1)/(float)CLOCKS_PER_SEC);
 }

void dec_setlsfile(int ec)
 {
 char *old_ls_name,*new_ls_name,*path,ext[4];
 old_ls_name=strrchr(init.lightname,'/');
 if(old_ls_name!=NULL) 
  { checkmem(path=MALLOC(old_ls_name-init.lightname+1));
    strncpy(path,init.lightname,old_ls_name-init.lightname);
    path[old_ls_name-init.lightname]=0; old_ls_name++; }
 else 
  { checkmem(path=MALLOC(strlen(init.cfgpath)+1));
    strcpy(path,init.cfgpath); old_ls_name=init.lightname; }
 strcpy(ext,init.d_ver>=d2_10_sw ? "ls2" : "ls1");
 if((new_ls_name=getfilename(&path,old_ls_name,ext,TXT_SETLSFILE,0))==NULL)
  return;
 FREE(init.lightname);
 checkmem(init.lightname=MALLOC(strlen(new_ls_name)+1));
 strcpy(init.lightname,new_ls_name);
 printmsg(TXT_LSFILENAME,init.lightname);
 }

void start_adjustlight(struct wall *wall)
 {
 struct node *n,*nl;
 int w,c,sum;
 my_assert(l!=NULL && wall!=NULL && wall->ls!=NULL);
 for(nl=l->lightsources.head;nl->next!=NULL;nl=nl->next)
  for(n=nl->d.ls->effects.head;n->next!=NULL;n=n->next)
   {
   my_assert(n->d.lse->cube!=NULL);
   for(w=0;w<6;w++) if(n->d.lse->cube->d.c->walls[w]) for(c=0;c<4;c++)
    if((sum=-n->d.lse->add_light[w*4+c])!=0)
     {
     sum+=n->d.lse->cube->d.c->walls[w]->corners[c].light;
     n->d.lse->cube->d.c->walls[w]->corners[c].light=sum<0 ? 0 : sum;
     }
   }
 for(n=wall->ls->d.ls->effects.head;n->next!=NULL;n=n->next)
  {
  my_assert(n->d.lse->cube!=NULL);
  for(w=0;w<6;w++) if(n->d.lse->cube->d.c->walls[w]) for(c=0;c<4;c++)
   if((sum=n->d.lse->add_light[w*4+c])!=0)
    {
    sum+=n->d.lse->cube->d.c->walls[w]->corners[c].light;
    n->d.lse->cube->d.c->walls[w]->corners[c].light=sum>theMaxLight ?
     theMaxLight : sum;
    }
  }
 }

void end_adjustlight(struct wall *wall,int save)
 {
 struct node *n,*nl;
 struct ls_effect *lse;
 int w,c,sum;
 my_assert(l!=NULL && wall!=NULL && wall->ls!=NULL);
 if(save)
  {
  freelist(&wall->ls->d.ls->effects,free);
  for(n=l->cubes.head;n->next!=NULL;n=n->next)
   {
   lse=NULL;
   for(w=0;w<6;w++) if(n->d.c->walls[w]) for(c=0;c<4;c++)
    if(n->d.c->walls[w]->corners[c].light) 
     {
     if(!lse)
      {
      checkmem(lse=MALLOC(sizeof(struct ls_effect)));
      checkmem(addnode(&wall->ls->d.ls->effects,-1,lse));
      }
     lse->cube=n;
     lse->add_light[w*4+c]=n->d.c->walls[w]->corners[c].light;
     lse->smoothed[w*4+c]=0;
     }
   }
  }
 for(nl=l->lightsources.head;nl->next!=NULL;nl=nl->next)
  for(n=nl->d.ls->effects.head;n->next!=NULL;n=n->next)
   {
   my_assert(n->d.lse->cube!=NULL);
   for(w=0;w<6;w++) if(n->d.lse->cube->d.c->walls[w]) for(c=0;c<4;c++)
    if((sum=n->d.lse->add_light[w*4+c])!=0)
     {
     sum+=n->d.lse->cube->d.c->walls[w]->corners[c].light;
     n->d.lse->cube->d.c->walls[w]->corners[c].light=sum>theMaxLight ?
      theMaxLight : sum;
     }
   }
 }
