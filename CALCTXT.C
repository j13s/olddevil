/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    calctxt.c - calculate the texture coordinates/lights
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
#include "plottxt.h"
#include "insert.h"
#include "calctxt.h"

void newcubecorners(struct node *c,int pointnum)
 {
 int i;
 for(i=0;i<3;i++) 
  {
  if(c->d.c->walls[wallno[pointnum][0][i]]!=NULL) 
   newwall_offset(c->d.c,c->d.c->walls[wallno[pointnum][0][i]],
    (wallno[pointnum][1][i]+2)&3,(wallno[pointnum][1][i]+3)&3); 
  }
 }

void calc_txtcoords(struct wall *w,int corner,struct point *new_p,
 short int *new_coords)
 {
 int j,t_ps,t_pe;
 struct point turnaxis,paraxis,p;
 float x,y,t_uv[2],ot_uv[2];
 my_assert(w!=NULL);
 t_pe=(corner+1)&3; t_ps=(corner-1)&3;
 for(j=0;j<3;j++) 
  { turnaxis.x[j]=w->p[t_pe]->d.p->x[j]-w->p[t_ps]->d.p->x[j];
    paraxis.x[j]=w->p[(corner+2)&3]->d.p->x[j]-w->p[corner]->d.p->x[j];
    p.x[j]=new_p->x[j]-w->p[t_ps]->d.p->x[j]; }
 normalize(&turnaxis); x=SCALAR(&turnaxis,&paraxis);
 for(j=0;j<3;j++) paraxis.x[j]-=x*turnaxis.x[j];
 normalize(&paraxis);
 for(j=0;j<2;j++) t_uv[j]=w->corners[t_pe].x[j]-w->corners[t_ps].x[j];
 x=sqrt(t_uv[0]*t_uv[0]+t_uv[1]*t_uv[1]); t_uv[0]/=x; t_uv[1]/=x;
 ot_uv[0]=-t_uv[1]; ot_uv[1]=t_uv[0];
 x=SCALAR(&turnaxis,&p)/640.0; y=SCALAR(&paraxis,&p)/640.0;
 for(j=0;j<2;j++) new_coords[j]=w->corners[t_ps].x[j]+x*t_uv[j]+y*ot_uv[j];
 }
 
void newwall_offset(struct cube *c,struct wall *w,int op0,int op1)
 {
 struct point p[2],p1,p2,op,turnaxis;
 int j,p1xoffs,p1yoffs,op2,op3,centerx,centery;
 float x,lx,y,ex_x,ex_y,ey_x,ey_y;
/*                      0 1 2 3 corner
  wall 0 (left side):   7 6 2 3
  wall 1 (ceiling):     0 4 7 3 
  wall 2 (right side):  0 1 5 4
  wall 3 (floor):       2 6 5 1
  wall 4 (front):       4 5 6 7
  wall 5 (back):        3 2 1 0 */
 if(c==NULL || w==NULL) return;
 if(w->locked) return;
 if((op0==0&&op1==3)||(op0==3&&op1==0)) { op1=0; op0=3; }
 else if(op1<op0) { j=op0; op0=op1; op1=j; }
 op2=(op1+1)&3; op3=(op0-1)&3;
 /* save the old orientation of the texture */
 p1xoffs=w->corners[op0].x[0]; p1yoffs=w->corners[op0].x[1];
 ex_x=w->corners[op1].x[0]-p1xoffs;
 ex_y=w->corners[op1].x[1]-p1yoffs;
 lx=sqrt(ex_x*ex_x+ex_y*ex_y);
 if(lx==0) { ex_x=1; ex_y=0; lx=1; }
 ex_x/=lx; ex_y/=lx;
 /* that was the first norm vector, now the one orthogonal */
 ey_y=ex_x; ey_x=-ex_y;
 /* the first point stays in (p1offsx,p1offsy),
    the second point is in the old direction but perhaps in another
    length */
 for(j=0;j<3;j++)
  p[0].x[j]=w->p[op1]->d.p->x[j]-w->p[op0]->d.p->x[j];
 w->corners[op1].x[0]=(short int)(ex_x*LENGTH(&p[0])/640.0)+p1xoffs;
 w->corners[op1].x[1]=(short int)(ex_y*LENGTH(&p[0])/640.0)+p1yoffs;
 /* OK. now the third vector must be divided in one who goes
    parallel to p[op0]->p[op1] and one who's orthogonal */
 for(j=0;j<3;j++)
  p1.x[j]=w->p[op3]->d.p->x[j]-w->p[op0]->d.p->x[j];
 normalize(&p[0]);
 x=SCALAR(&p[0],&p1);
 for(j=0;j<3;j++)
  p[1].x[j]=p1.x[j]-x*p[0].x[j];
 /* in p[1] is now the part of p1 that's not in px direction and
    x the length of p[1] in p[0] direction.
    p[0] direction is in the plane ex, the orthognal direction ey,
    so we get the point in the plane with */
 w->corners[op3].x[0]=(short int)((ey_x*LENGTH(&p[1])+ex_x*x)/640.0)+p1xoffs;
 w->corners[op3].x[1]=(short int)((ey_y*LENGTH(&p[1])+ex_y*x)/640.0)+p1yoffs;
 /* now the last point: */
 /* turn last point into the plane of the three others: */
 /* get the coordsystem of the turn axis */
 for(j=0;j<3;j++) turnaxis.x[j]=w->p[op3]->d.p->x[j]-w->p[op1]->d.p->x[j];
 normalize(&turnaxis);
 for(j=0;j<3;j++)
  {
  op.x[j]=(w->p[op3]->d.p->x[j]+w->p[op1]->d.p->x[j])/2.0;
  p1.x[j]=w->p[op2]->d.p->x[j]-op.x[j];
  }
 x=SCALAR(&p1,&turnaxis);
 y=sqrt(LENGTH(&p1)*LENGTH(&p1)-x*x);
 /* now x,y are the coordinates of the point parallel/orthogonal to the
    turnaxis */
 /* now make the coordsystem of the other three points */
 for(j=0;j<3;j++) p2.x[j]=w->p[op0]->d.p->x[j]-op.x[j];
 lx=SCALAR(&p2,&turnaxis);
 for(j=0;j<3;j++) p2.x[j]-=turnaxis.x[j]*lx;
 normalize(&p2);
 /* -p2.x[j] because the point lies on the other side of the turnaxis */
 for(j=0;j<3;j++) 
  p1.x[j]=op.x[j]+turnaxis.x[j]*x-p2.x[j]*y-w->p[op0]->d.p->x[j];
 /* Ok, in p1 is now the point turned into the plane of the three other 
    points. */
 normalize(&p[1]);
 x=SCALAR(&p1,&p[0]);
 y=SCALAR(&p1,&p[1]);
 w->corners[op2].x[0]=(short int)((ex_x*x+ey_x*y)/640.0)+p1xoffs;
 w->corners[op2].x[1]=(short int)((ex_y*x+ey_y*y)/640.0)+p1yoffs; 
 centerx=centery=0;
 for(j=0;j<4;j++)
  { centerx+=w->corners[j].x[0]; centery+=w->corners[j].x[1]; }
 for(j=0;j<4;j++)
  { w->corners[j].x[0]-=(centerx/4/2048)*2048;
    w->corners[j].x[1]-=(centery/4/2048)*2048; }
 if(c->d[w->no]!=NULL) makedoorpnt(c->d[w->no]->d.d);
 c->recalc_polygons[w->no]=1;
 }
 
void recalcwall(struct cube *c,int wallno)
 { 
 my_assert(c!=NULL && wallno>=0 && wallno<6);
 if(c->walls[wallno]!=NULL) newwall_offset(c,c->walls[wallno],0,1); 
 }
 
void newcorners(struct node *np)
 {
 struct node *nc;
 for(nc=np->d.lp->c.head;nc->next!=NULL;nc=nc->next)
  newcubecorners(nc->d.n,nc->no); 
 }

/* gives no of wall with neighbourcube to points wp1, wp2 on wall wno */
int findwallnotoline(int wno,int wp1,int wp2)
 {
 int cn1,i,j;
 for(i=0,cn1=-1;i<3;i++)
  if((cn1=wallno[wallpts[wno][wp1]][0][i])!=wno)
   for(j=0;j<3;j++)
    if(cn1==wallno[wallpts[wno][wp2]][0][j]) 
     return cn1;
 waitmsg(TXT_CANTFINDARRCUBE);
 return 0;
 }
 
/* get the wall in neighbour cube nc which has the two points wp1,wp2
   in wall w in cube c common with w. */
int findnbwalltoline(struct node *c,struct cube *nc,
 int w,int wp1,int wp2)
 {
 int i,j;
 for(i=0;i<6;i++)
  if(nc->walls[i]!=NULL && (nc->nc[i]==NULL || nc->nc[i]->no!=c->no))
   for(j=0;j<4;j++)
    if(nc->walls[i]->p[j]->no==c->d.c->p[wallpts[w][wp1]]->no && 
     (nc->walls[i]->p[(j+1)&0x3]->no==c->d.c->p[wallpts[w][wp2]]->no ||
      nc->walls[i]->p[(j-1)&0x3]->no==c->d.c->p[wallpts[w][wp2]]->no))
     { return i; break; }
 return -1;
 }
 
void arrangebitmaps(struct node *start_c,struct wall *start_w)
 {
 int cn1,wp1,wp2,w2p1,w2p2,i;
 struct wall *w,*w1;
 struct node *nw,*nc,*c,*run;
 struct node **save;
 struct list walls;
 my_assert(start_c!=NULL && start_w!=NULL);
 /* I must do it with a list otherwise go32 crashes. perhaps stack overflow?*/
 initlist(&walls);
 checkmem(save=(struct node **)MALLOC(sizeof(struct node *)*2));
 save[0]=start_c; save[1]=(struct node *)start_w;
 checkmem(addnode(&walls,start_c->no,save));
 for(run=walls.head;run->next!=NULL;run=run->next)
  {
  c=*((struct node **)run->d.d); w1=*((struct wall **)run->d.d+1);
  for(wp1=0;wp1<4;wp1++)
   {
   wp2=(wp1+1)&3;
   /* first get the wall which shares wp1 and wp2 */
   cn1=nb_sides[w1->no][wp1];
   if((nc=c->d.c->nc[cn1])==NULL) nc=c; 
   else
    { /* the next wall lies in the neighbour cube */
    /* now find the right wall */
    if((cn1=findnbwalltoline(c,nc->d.c,w1->no,wp1,wp2))==-1) continue;
    }
   if((w=nc->d.c->walls[cn1])==NULL) continue;
   /* OK, w should be one wall next to w1, now look, if it's tagged */
   if((nw=nc->d.c->tagged_walls[w->no])==NULL) continue;
   untag(tt_wall,nw->d.n,w->no); /* untag wall */
   /* Search the shared points */
   for(i=0,w2p1=w2p2=-1;i<4;i++)
    if(w->p[i]->no==w1->p[wp1]->no) w2p1=i;
    else if(w->p[i]->no==w1->p[wp2]->no) w2p2=i;
   my_assert(w2p1>=0 && w2p2>=0);
   w->corners[w2p1].x[0]=w1->corners[wp1].x[0];
   w->corners[w2p1].x[1]=w1->corners[wp1].x[1];
   w->corners[w2p2].x[0]=w1->corners[wp2].x[0];
   w->corners[w2p2].x[1]=w1->corners[wp2].x[1];
   newwall_offset(nc->d.c,w,w2p1,w2p2);
   checkmem(save=MALLOC(sizeof(struct node *)*2));
   save[0]=nc; save[1]=(struct node *)w;
   checkmem(addnode(&walls,nc->no,save));
   }
  freenode(&walls,run,free);
  }
 freelist(&walls,free);
 }
 
 /* find a path beginning with the last cube in list dest with cubes out
   of the tag list and save this path in dest in the right
   order. returns 0 if path is ambiguous, 1 if a path was found. */
int findpath(struct list *dest)
 {
 int s;
 struct node *cc,*nc,*snc;
 cc=dest->tail->d.n;
 for(s=0,nc=NULL;s<6;s++)
  if(cc->d.c->nc[s]!=NULL && (snc=cc->d.c->nc[s]->d.c->tagged)!=NULL) 
   if(nc!=NULL) {printmsg(TXT_AMBIGUOUSPATH,cc->no,snc->no,nc->no);return 0;}
   else nc=snc;
 if(nc==NULL) return 1; /* path ended here */
 untag(tt_cube,nc->d.n);
 checkmem(addnode(dest,nc->d.n->no,nc->d.n));
 return findpath(dest);
 }
