/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    insert.c - insert/delete all objects.
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
#include "insert.h"
#include "stdtypes.h"

void fittogrid(struct point *p)
 {
 int i;
 if(!view.gridonoff) return;
 for(i=0;i<3;i++)
  p->x[i]=floor(p->x[i]/view.gridlength+0.5)*view.gridlength; 
 }
 
struct node *insertthing(struct node *c,int t1)
 {
 struct node *n;
 struct thing *t;
 int i,j;
 t=changething(NULL,t1,view.pcurrcube->d.c);
 for(j=0;j<3;j++)
  {
  t->p[0].x[j]=0.0;
  for(i=0;i<8;i++)
   t->p[0].x[j]+=c->d.c->p[i]->d.p->x[j]/8.0;
  }
 setthingpts(t);
 if((n=addnode(&l->things,l->things.size,t))==NULL)
  { printmsg("Can't add node to thing list."); free(t); return NULL; }
 return n;
 }
 
struct wall *insertwall(struct node *c,int wallnum,int txt2)
 {
 struct wall *w;
 int j;
 if(c->d.c->walls[wallnum]!=NULL)
  { printmsg("There's already a wall."); return NULL; }
 if((w=malloc(sizeof(struct wall)))==NULL)
  { printmsg("No mem for wall."); return NULL; }
 w->texture2=txt2; w->texture1=0;
 w->no=wallnum;
 for(j=0;j<4;j++)
  {
  w->p[j]=c->d.c->p[wallpts[wallnum][j]];
  w->corners[j]=stdcorners[j];
  }
 c->d.c->walls[wallnum]=w;
 for(j=0;j<4;j++) newwallpnt(c->d.c,w,w->p[j]->d.p);
 if(view.currwall==wallnum && view.pcurrcube->no==c->no) 
  view.pcurrwall=w;
 return w;
 }
 
struct node *insertsingledoor(struct node *c,int wallnum,int animtxt,
 int walltxt)
 {
 struct door *d;
 struct node *n;
 if(c->d.c->nc[wallnum]==NULL)
  { printmsg("No neighbour."); return NULL; }
 if(insertwall(c,wallnum,walltxt)==NULL) return NULL;
 if((d=malloc(sizeof(struct door)))==NULL)
  { printmsg("No mem for door.\n"); return NULL; }
 *d=stddoor;
 d->cubenum=c->no; d->wallnum=wallnum;
 d->d=NULL;
 d->sd=NULL;
 if((n=addnode(&l->doors,l->doors.size,d))==NULL)
  { printmsg("No mem for node.\n"); free(d); d->c->d.c->walls[wallnum]=NULL;
    free(d->w); return NULL; }
 initlist(&d->sdoors);
 initdoor(n);
 c->d.c->d[wallnum]=n;
 return n;
 } 
 
int insertsdoor(struct node *d)
 {
 struct sdoor *sd;
 if((sd=malloc(sizeof(struct sdoor)))==NULL)
  { printmsg("No mem for switch."); return 0; }
 *sd=stdsdoor;
 sd->d=d;
 if((d->d.d->sd=addnode(&l->sdoors,l->sdoors.size,sd))==NULL)
  { printmsg("Can't insert node for switch."); free(sd); return 0;}
 return 1;
 }
 
struct node *insertdoor(struct node *c,int wallnum,int animtxt,int walltxt)
 {
 struct node *n;
 int i,wall;
 if((n=insertsingledoor(c,wallnum,animtxt,walltxt))==NULL) return NULL;
 for(i=0,wall=-1;i<6;i++)
  if(c->d.c->nc[wallnum]->d.c->nc[i]!=NULL)
   if(c->d.c->nc[wallnum]->d.c->nc[i]->no==c->no) {wall=i; break;}
 if(wall==-1)
  { printmsg("Can't find neighbours."); deletedoor(n); return NULL; }
 if((n->d.d->d=insertsingledoor(c->d.c->nc[wallnum],wall,animtxt,walltxt))==
  NULL) { deletedoor(n); return NULL; } 
 n->d.d->d->d.d->d=n;
 return n;
 }
 
void deleteconnect(struct node *c,int wn)
 {
 struct node *nc;
 int j,nwn;
 struct listpoint *lp;
 struct wall *w;
 if((nc=c->d.c->nc[wn])==NULL) 
  { printmsg("No neighbour cube."); return; }
 if(c->d.c->d[wn]!=NULL)
  { printmsg("First kill the wall."); return; }
 for(nwn=0;nwn<6;nwn++)
  if(nc->d.c->nc[nwn]->no==c->no) break; 
 if(nwn==6) 
  { printmsg("Can't find corresponding side in neighbour cube."); return; }
 if((w=insertwall(nc,nwn,0))==NULL) return;
 nc->d.c->nc[nwn]=NULL;
 if((w=insertwall(c,wn,0))==NULL) return;
 c->d.c->nc[wn]=NULL;
 /* now double points */
 for(j=0;j<4;j++)
  {
  if((lp=malloc(sizeof(struct listpoint)))==NULL)
   { printmsg("No mem for listpoint."); return; }
  for(nc=w->p[j]->d.lp->c.head;nc->next!=NULL;nc=nc->next)
   if(nc->d.n->no==c->no) break;
  if(nc->next==NULL)
   { printmsg("Can't find cube in pointlist."); return; }
  freenode(&w->p[j]->d.lp->c,nc,NULL);
  lp->p=w->p[j]->d.lp->p;
  initlist(&lp->c);
  if(addnode(&lp->c,wallpts[wn][j],c)==NULL)
   { printmsg("Can't add node to cubelist in point."); return; }
  if((nc=addnode(&l->pts,l->pts.size,lp))==NULL)
   { printmsg("Can't add point to pointlist."); return; }
  w->p[j]=nc;
  c->d.c->p[wallpts[wn][j]]=nc;
  }
 }
 
void killtarget(struct sdoor *sd,int n)
 {
 int i,j;
 for(i=0;i<sd->num;i++) 
  if(sd->target[i]->no==n) break;
 if(i==sd->num)
  { printmsg("Can't kill target %d",n); return; }
 for(j=i;j<sd->num-1;j++) sd->target[j]=sd->target[j+1];
 sd->num--; 
 }
 
void killsdoorlist(struct list *l,int no)
 {
 struct node *n;
 for(n=l->head;n->next!=NULL;n=n->next)
  killtarget(n->d.n->d.d->sd->d.sd,no);
 }
 
void cleansdoor(struct sdoor *sd)
 {
 int k;
 struct list *tl;
 struct node *n;
 for(k=0;k<sd->num;k++)
  {
  if(getsdoortype(sd)==sdtype_door) tl=&sd->target[k]->d.d->sdoors;
  else tl=&sd->target[k]->d.c->sdoors;
  if((n=findnode(tl,sd->d->no))==NULL)
   { fprintf(errf,"Can't find corresponding door %d to sdoor.",sd->d->no);
     my_exit(); }
  freenode(tl,n,NULL);
  }
 sd->num=0;
 }
 
void deletesdoor(struct node *n)
 {
 cleansdoor(n->d.sd);
 freenode(&l->sdoors,n,free);
 }
 
void deletesingledoor(struct node *n)
 {
 n->d.d->c->d.c->d[n->d.d->wallnum]=NULL;
 free(n->d.d->c->d.c->walls[n->d.d->wallnum]);
 n->d.d->c->d.c->walls[n->d.d->wallnum]=NULL;
 if(view.pcurrwall==n->d.d->w) view.pcurrwall=NULL;
 free(n->d.d->w);
 if(n->d.d->d) n->d.d->d->d.d->d=NULL;
 if(n->d.d->sd) cleansdoor(n->d.d->sd->d.sd);
 view.oldpcurrdoor=NULL;
 if(n->no==view.pcurrdoor->no) 
  { view.pcurrdoor=view.pcurrdoor->prev->prev ? view.pcurrdoor->prev :
     view.pcurrdoor->next;
    if(view.pcurrdoor->next==NULL) view.pcurrdoor=NULL; }
 killsdoorlist(&n->d.d->sdoors,n->no);
 freenode(&l->doors,n,freedoor);
 }
 
void deletedoor(struct node *n)
 {
 struct node *d2=n->d.d->d;
 if(d2!=NULL) deletesingledoor(d2);
 deletesingledoor(n);
 }
 
void deletecube(struct node *n)
 {
 int k,m,w;
 struct cube *nc,*c=n->d.c;
 struct listpoint *lp;
 struct node *wall,*cn;
 if(n->no==view.pcurrcube->no) 
  { 
  view.pcurrcube=view.pcurrcube->prev->prev ? view.pcurrcube->prev :
   view.pcurrcube->next;
  if(!view.pcurrcube->next)
   { printmsg("Can't kill last cube."); view.pcurrcube=view.pcurrcube->prev;
     return; } 
  }
 for(k=0;k<6;k++)
  if(c->nc[k]!=NULL)
   {
   nc=c->nc[k]->d.c;
   for(m=0,w=-1;m<6;m++)
    if(nc->nc[m]!=NULL && nc->nc[m]->no==n->no) { w=m; break; }
   if(w==-1)
    { fprintf(errf,"Can't find neighbour.\n"); my_exit(); }
   nc->nc[w]=NULL;
   insertwall(c->nc[k],w,0);
   }
  else
   if((wall=findnode(&view.tagged[tt_wall],n->no*6+k))!=NULL)
    freenode(&view.tagged[tt_wall],wall,NULL);
 if(view.oldpcurrcube->no==n->no) view.oldpcurrcube=NULL;
 for(k=0;k<8;k++)
  {
  lp=c->p[k]->d.lp;
  for(cn=lp->c.head;cn->next!=NULL;cn=cn->next)
   if(cn->d.n->no==n->no) break;
  if(cn->next==NULL)
   { fprintf(errf,"deletecube: Can't find cube in point (%d)\n",lp->c.size); 
     my_exit(); }
  freenode(&lp->c,cn,NULL);
  if(lp->c.size<=0) freenode(&l->pts,c->p[k],freelistpnt);
  }
 if(view.exitcube->no==n->no) view.exitcube=NULL;
 if(c->cp!=NULL) freenode(&l->producers,c->cp,free);
 killsdoorlist(&c->sdoors,n->no);
 for(wall=view.tagged[tt_pnt].head;wall->next!=NULL;wall=wall->next)
  if(wall->no/24==n->no) freenode(&view.tagged[tt_pnt],wall,NULL);
 freenode(&l->cubes,n,freecube);
 view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
 }

/* replace point oldp with point newp in all cubes of oldp
   starting at node sn (if NULL starting at head).
   returns 0 if there are more than eight cubes for one point,
   1 otherwise. */
int changepnt(struct node *oldp,struct node *newp,struct node *sn)
 {
 struct node *n,*cn;
 int pn;
 int i;
 if(oldp==newp) return 1; /* points are the same */
 /* change all old points in new points. */
 for(cn=(sn==NULL) ? oldp->d.lp->c.head : sn;cn->next!=NULL;cn=cn->next)
  {
  n=cn->d.n; pn=cn->no;
  n->d.c->p[pn]=newp;
  if(addnode(&newp->d.lp->c,cn->no,n)==NULL) return 0;
  for(i=0;i<3;i++)
   if(n->d.c->walls[wallno[pn][0][i]])
    n->d.c->walls[wallno[pn][0][i]]->p[wallno[pn][1][i]]=newp; 
  }
 for(cn=(sn==NULL) ? oldp->d.lp->c.head : sn;cn->next!=NULL;cn=cn->next)
  freenode(&oldp->d.lp->c,cn,NULL);
 return 1;
 }
 
/* connect cube c1, wall w1 with cube c2, wall w2. fitting wall w2.
   both walls must exist.
   returns 0 if connection is not possible, otherwise 1 */
int connect(struct node *nc1,int w1,struct node *nc2,int w2)
 {
 struct wall *pw2;
 struct cube *c1,*c2;
 int i,j,k,cp[4];
 struct node *pnts[4][2],*on[4]; /* the four changed points [0]:old [1]:new */
 struct point *p1,p2;
 double dist;
 if(nc1->no==nc2->no)
  { printmsg("Can't connect cube with itself."); return 0; }
 c1=nc1->d.c; c2=nc2->d.c;
 pw2=c2->walls[w2];
 if(pw2==NULL || c1->walls[w1]==NULL)
  { fprintf(errf,"Cube has no wall: %d %d %p / %d %d %p\n",nc1->no,w1,
    c1->walls[w1],nc2->no,w2,c2->walls[w2]);
    my_exit(); }
 /* search nearest points  */
 for(j=0;j<4;j++)
  {
  p1=c1->walls[w1]->p[j]->d.p;
  for(i=0,cp[j]=-1,dist=1.0e7;i<4;i++)
   {
   for(k=0;k<3;k++)
    p2.x[k]=pw2->p[i]->d.p->x[k]-p1->x[k];
   if(dist>LENGTH(&p2)) { dist=LENGTH(&p2); cp[j]=i; }
   }
  if(cp[j]==-1)
   { printmsg("Cube 2 too far away."); return 0; }
  for(i=0;i<j;i++)
   if(cp[i]==cp[j])
    { printmsg("Connection ambiguous."); return 0; }
  }
 /* copy the new four points on the old four points of the current wall and
   free the new four */
 for(j=0;j<4;j++)
  {
  pnts[j][0]=c2->p[wallpts[w2][cp[j]]];/*old pointer for freenode*/
  pnts[j][1]=c1->p[wallpts[w1][j]]; /* new pointer */
  }
 for(k=0,i=-1;k<4 && i==-1;k++)
  {
  on[k]=pnts[k][1]->d.lp->c.tail;
  if(changepnt(pnts[k][0],pnts[k][1],NULL)==0) i=k;
  }
  /* test if connection is correct */
 for(k=0;k<4 && i==-1;k++)
  if(testpnt(pnts[k][1])==0) i=3;
 if(i!=-1)
  {
  for(k=0;k<=i;k++)
   changepnt(pnts[k][1],pnts[k][0],on[k]);
  return 0;
  }
 /* connection is correct: let's kill all old things */
 c2->nc[w2]=nc1; free(c2->walls[w2]); c2->walls[w2]=NULL;
 c1->nc[w1]=nc2; free(c1->walls[w1]); c1->walls[w1]=NULL;
 /* free old points and calculate new textures */
 for(j=0;j<4;j++)
  {
  if(pnts[j][0]->no!=pnts[j][1]->no)
   {
   freenode(&l->pts,pnts[j][0],free);  
   newcorners(pnts[j][1]); 
   }
  }
 if(view.exitcube->no==nc1->no || view.exitcube->no==nc2->no)
  view.exitcube=NULL;
 printmsg("Connected cube %d,%d with cube %d,%d",nc1->no,w1,nc2->no,w2);
 return 1;
 }

/* make click point for door */
void makedoorpnt(struct door *d)
 {
 int j;
 struct point diff1,diff2,cp,*p[4];
 for(j=0;j<4;j++) p[j]=d->w->p[j]->d.p;
 for(j=0;j<3;j++)
  { diff1.x[j]=p[2]->x[j]-p[0]->x[j];
    diff2.x[j]=p[3]->x[j]-p[1]->x[j]; }
 VECTOR(&d->p,&diff1,&diff2);
 normalize(&d->p);
 for(j=0;j<3;j++) 
  { cp.x[j]=(p[0]->x[j]+p[1]->x[j]+p[2]->x[j]+p[3]->x[j])/4;
    d->p.x[j]=cp.x[j]+d->p.x[j]*view.dsize; }
 }
 
/* init of all pointers&point in a door. cubenum and wallnum must be set,
   sdoors must be inited */
void initdoor(struct node *n)
 {
 int i;
 struct door *d=n->d.d;
 struct sdoor *sd;
 struct node *sdn;
 if((d->c=findnode(&l->cubes,d->cubenum))==NULL)
  { fprintf(errf,"Can't find cube %lu for door.\n",d->cubenum); my_exit(); }
 if(d->wallnum>5)
  { fprintf(errf,"Wrong wall number %lu (cube=%lu) in door.\n",d->wallnum,
     d->cubenum); my_exit(); }
 d->w=d->c->d.c->walls[d->wallnum];
 if(d->w==NULL)
  { fprintf(errf,"Door: wall number %lu (cube=%lu) == NULL.\n",d->wallnum,
     d->cubenum); my_exit(); }
 if(d->c->d.c->nc[d->wallnum]==NULL)
  { fprintf(errf,"Warning: Door cube has no neighbour: %lu %lu\n",d->cubenum,
     d->wallnum);  }
 d->d=NULL;
 d->edoor=0;
 for(i=0;i<6;i++)
  if(d->c->d.c->nc[d->wallnum] && d->c->d.c->nc[d->wallnum]->d.c->nc[i]==d->c)
   { d->d=d->c->d.c->nc[d->wallnum]->d.c->d[i]; break; }
 if(d->sdoor!=0xff)
  {
  if((d->sd=findnode(&l->sdoors,d->sdoor))==NULL)
   { fprintf(errf,"Can't find sdoor %d for door.\n",(int)d->sdoor);
     my_exit(); }
  sd=d->sd->d.sd;
  sd->d=n;
  for(i=0;i<sd->num;i++)
   {
   if((sdn=findnode(&l->cubes,sd->cubes[i]))==NULL)
    {fprintf(errf,"Can't find cube %hd for sdoor.\n",sd->cubes[i]);
     my_exit();}
   if(getsdoortype(sd)==sdtype_door)
    {
    if(sd->walls[i]>5) 
     { fprintf(errf,"Illegal wall number %d for sdoor %d.\n",sd->walls[i],
        d->sd->no); my_exit(); }
    if((sd->target[i]=sdn->d.c->d[sd->walls[i]])==NULL)
     { fprintf(errf,"No door on cube %d wall %d for sdoor %d.\n",
        sd->cubes[i],sd->walls[i],d->sd->no); my_exit(); }
    addnode(&sd->target[i]->d.d->sdoors,n->no,n);
    }
   else
    {
    sd->target[i]=sdn;
    addnode(&sd->target[i]->d.c->sdoors,n->no,n);
    }
   }
  }
 else d->sd=NULL;
 makedoorpnt(d);
 }
 
/* init of all pointers in a cube. the fields c->pts,c->nextcubes,c->doors
   must be set. */
void initcube(struct node *n)
 {
 int j,k;
 struct listpoint *lp;
 struct cube *c=n->d.c;
 for(j=0;j<8;j++)
  {
  if((c->p[j]=findnode(&l->pts,(int)c->pts[j]))==NULL)
   { fprintf(errf,"Can't find cubepoint %d.\n",(int)c->pts[j]); my_exit(); }
  lp=c->p[j]->d.lp;
  addnode(&lp->c,j,n);
  }
 for(j=0;j<6;j++)
  {
  if(c->nextcubes[j]==0xffff)
   c->nc[j]=NULL;
  else
   if((c->nc[j]=findnode(&l->cubes,(int)c->nextcubes[j]))==NULL)
    { fprintf(errf,"Can't find neighbour cube %d.\n",(int)c->nextcubes[j]);
      my_exit(); }
  if(c->doors[j]==0xff)
   c->d[j]=NULL;
  else
   if((c->d[j]=findnode(&l->doors,(int)c->doors[j]))==NULL)
    { fprintf(errf,"Can't find door %d.\n",(int)c->doors[j]); my_exit(); }
  if(c->walls[j]!=NULL)
   for(k=0;k<4;k++)
    c->walls[j]->p[k]=c->p[wallpts[j][k]];
  }
 initlist(&c->sdoors);
 if(c->type==4 && c->prodnum>=0) /* producer */
  {
  if((c->cp=findnode(&l->producers,c->prodnum))==NULL)
   { fprintf(errf,"Can't find producer %d for cube %d.\n",
      (int)c->prodnum,n->no); my_exit(); } 
  }
 else c->cp=NULL;
 }

void newcubecorners(struct cube *c,int pointnum)
 {
 int i;
 for(i=0;i<3;i++)
  newwallpnt(c,c->walls[wallno[pointnum][0][i]],c->p[pointnum]->d.p);
 }

void newwall_offset(struct cube *c,struct wall *w,int op0,int op1)
 {
 struct point p[2],p1,p2,op,turnaxis;
 int j,p1xoffs,p1yoffs,op2,op3,centerx,centery;
 struct pixel ex,ey;
 double x,lx,y;
/*                        0 1 2 3 corner
  wall 0 (left side):   7 6 2 3
  wall 1 (ceiling):     0 4 7 3 
  wall 2 (right side):  0 1 5 4
  wall 3 (floor):       2 6 5 1
  wall 4 (front):       4 5 6 7
  wall 5 (back):        3 2 1 0 */
 if(w==NULL) return;
 if(op1<op0) { j=op0; op0=op1; op1=j; }
 op2=(op1+1)&3; op3=(op0-1)&3;
 /* save the old orientation of the texture */
 p1xoffs=w->corners[op0].xpos; p1yoffs=w->corners[op0].ypos;
 ex.x=w->corners[op1].xpos-p1xoffs;
 ex.y=w->corners[op1].ypos-p1yoffs;
 lx=sqrt(ex.x*ex.x+ex.y*ex.y);
 if(lx==0)
  { ex.x=1; ex.y=0; lx=1; }
 ex.x/=lx; ex.y/=lx;
 /* that was the first norm vector, now the one orthogonal */
 ey.y=ex.x; ey.x=-ex.y;
 /* the first point stays in (p1offsx,p1offsy),
    the second point is in the old direction but perhaps in another
    length */
 for(j=0;j<3;j++)
  p[0].x[j]=w->p[op1]->d.p->x[j]-w->p[op0]->d.p->x[j];
 w->corners[op1].xpos=(short int)(ex.x*LENGTH(&p[0])/640.0)+p1xoffs;
 w->corners[op1].ypos=(short int)(ex.y*LENGTH(&p[0])/640.0)+p1yoffs;
 /* OK. now the last point must be divided in one who goes
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
 w->corners[op3].xpos=(short int)((ey.x*LENGTH(&p[1])+ex.x*x)/640.0)+p1xoffs;
 w->corners[op3].ypos=(short int)((ey.y*LENGTH(&p[1])+ex.y*x)/640.0)+p1yoffs;
 /* now the last point: */
 /* NEW */
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
 /* END NEW 
 for(j=0;j<3;j++)
  p1.x[j]=w->p[op2]->d.p->x[j]-w->p[op0]->d.p->x[j]; */
 normalize(&p[1]);
 x=SCALAR(&p1,&p[0]);
 y=SCALAR(&p1,&p[1]);
 w->corners[op2].xpos=(short int)((ex.x*x+ey.x*y)/640.0)+p1xoffs;
 w->corners[op2].ypos=(short int)((ex.y*x+ey.y*y)/640.0)+p1yoffs; 
 centerx=centery=0;
 for(j=0;j<4;j++)
  { centerx+=w->corners[j].xpos; centery+=w->corners[j].ypos; }
 for(j=0;j<4;j++)
  { w->corners[j].xpos-=(centerx/4/2048)*2048;
    w->corners[j].ypos-=(centery/4/2048)*2048; }
 for(j=0;j<4;j++)
  if(c->d[w->no]!=NULL)
   makedoorpnt(c->d[w->no]->d.d);
 }
 
void newwallpnt(struct cube *c,struct wall *w,struct point *np)
 { newwall_offset(c,w,0,1); }
 
/* test if one of the cubes depending on point np is not convex or weird.
   return 1 if all cubes are alright, 0 otherwise */
int testpnt(struct node *np)
 {
 unsigned short int newpnum=0,j,l1n,l2n,l3n;
 struct point l1,l2,l3,e;
 struct node *nc;
 /* test if two points are equal */
 for(nc=np->d.lp->c.head;nc->next!=NULL;nc=nc->next)
  for(l1n=0;l1n<7;l1n++)
   for(l2n=l1n+1;l2n<8;l2n++)
    {
    for(l3n=0;l3n<3 &&
     nc->d.n->d.c->p[l1n]->d.p->x[l3n]==nc->d.n->d.c->p[l2n]->d.p->x[l3n];
     l3n++);
    if(l3n==3) { printmsg("Pnt %d and %d in Cube %d are equal.",
     l1n,l2n,nc->d.n->no); return 0; }
    } 
 /* Test if all cubes convex and all walls are not weird */
 for(nc=np->d.lp->c.head;nc->next!=NULL;nc=nc->next)
  {
  for(newpnum=0;newpnum<8;newpnum++)
   {
   for(j=0;j<3;j++)
    {
    if(newpnum<4)
     { l1n=newpnum+4; l2n=(newpnum+1)&0x3;l3n=(newpnum-1)&0x3; }
    else
     { l1n=newpnum-4; l3n=(newpnum==7) ? 4 : newpnum+1;
       l2n=(newpnum==4) ? 7 : newpnum-1; }
    l1.x[j]=nc->d.n->d.c->p[l1n]->d.p->x[j] - 
     nc->d.n->d.c->p[newpnum]->d.p->x[j]; /* that looks great */
    l2.x[j]=nc->d.n->d.c->p[l2n]->d.p->x[j] - 
     nc->d.n->d.c->p[newpnum]->d.p->x[j];
    l3.x[j]=nc->d.n->d.c->p[l3n]->d.p->x[j] - 
     nc->d.n->d.c->p[newpnum]->d.p->x[j];
    }
   normalize(&l1); normalize(&l2); normalize(&l3);
   /* test if l1,l2,l3 are a right-handed system */
   VECTOR(&e,&l3,&l2);
   if(SCALAR(&e,&l1)<=view.mincorner)
    { printmsg("Cube %d at point %d not convex",nc->d.n->no,newpnum); 
      return 0; }
     /* this was a nearly left-handed system */
   }
  /* test weird walls */
  for(j=0;j<6;j++)
   {
   for(l1n=0;l1n<3;l1n++)
    l1.x[l1n]=nc->d.n->d.c->p[wallpts[j][3]]->d.p->x[l1n]-
     nc->d.n->d.c->p[wallpts[j][2]]->d.p->x[l1n];
   for(l1n=0;l1n<3;l1n++)
    l2.x[l1n]=nc->d.n->d.c->p[wallpts[j][0]]->d.p->x[l1n]-
     nc->d.n->d.c->p[wallpts[j][1]]->d.p->x[l1n];
   if(fabs(SCALAR(&l1,&l2))<=view.minweirdwall*LENGTH(&l1)*LENGTH(&l2))
    {
    for(l1n=0;l1n<3;l1n++)
     l1.x[l1n]=nc->d.n->d.c->p[wallpts[j][3]]->d.p->x[l1n]-
      nc->d.n->d.c->p[wallpts[j][0]]->d.p->x[l1n];
    for(l1n=0;l1n<3;l1n++)
     l2.x[l1n]=nc->d.n->d.c->p[wallpts[j][2]]->d.p->x[l1n]-
      nc->d.n->d.c->p[wallpts[j][1]]->d.p->x[l1n];
    /* weird wall? */
    if(fabs(SCALAR(&l1,&l2))<=view.minweirdwall*LENGTH(&l1)*LENGTH(&l2)) 
     { printmsg("Wall %d of cube %d weird.",j,nc->d.n->no); return 0; }
     /* weird wall! */
    }
   }
  }
 return 1;
 }

void newcorners(struct node *np)
 {
 struct node *nc;
 for(nc=np->d.lp->c.head;nc->next!=NULL;nc=nc->next)
  newcubecorners(nc->d.n->d.c,nc->no); 
 }

/* gives wall with neighbourcube to points wp1, wp2 on wall w1 */
int findalignwalltoline(struct wall *w1,int wp1,int wp2)
 {
 int cn1,i,j;
 for(i=0,cn1=-1;i<3;i++)
  if((cn1=wallno[wallpts[w1->no][wp1]][0][i])!=w1->no)
   for(j=0;j<3;j++)
    if(cn1==wallno[wallpts[w1->no][wp2]][0][j]) 
     return cn1;
 fprintf(errf,"Can't find arrange cube??\n"); my_exit();
 return 0;
 }
 
void arrangebitmaps(struct node *start_c,struct wall *start_w,struct list *ws)
 {
 int cn1,wp1,wp2,w2p1,w2p2,i,j;
 struct wall *w,*w1;
 struct node *nw,*nc,*c,*run;
 struct node **save;
 struct list walls;
 if(start_w==NULL) 
  { printmsg("No wall there."); return; }
 /* I must do it with a list otherwise go32 crashes. perhaps stack overflow?*/
 initlist(&walls);
 if((save=(struct node **)malloc(sizeof(struct node *)*2))==NULL)
  { printmsg("Sorry, not enough mem."); freelist(&walls,free); return; }
 save[0]=start_c; save[1]=(struct node *)start_w;
 if(!addnode(&walls,start_c->no,save))
  { printmsg("Sorry, not enough mem."); freelist(&walls,free); return; }   
 for(run=walls.head;run->next!=NULL;run=run->next)
  {
  c=*((struct node **)run->d.d); w1=*((struct wall **)run->d.d+1);
  for(wp1=0;wp1<4;wp1++)
   {
   wp2=(wp1+1)&3;
   /* first get the cube of every line in the wall which is equal and
      not the cube corresponding to w1 (cause there's no neighbour, 
      there's a side). */
   cn1=findalignwalltoline(w1,wp1,wp2);
   if((nc=c->d.c->nc[cn1])==NULL) continue;
   /* now find the right wall */
   for(i=0,w=NULL;i<6 && w==NULL;i++)
    if(nc->d.c->walls[i]!=NULL && (nc->d.c->nc[i]==NULL || 
     nc->d.c->nc[i]->no!=c->no))
     for(j=0;j<4;j++)
      if(nc->d.c->walls[i]->p[j]->no==w1->p[wp1]->no && 
       (nc->d.c->walls[i]->p[(j+1)&0x3]->no==w1->p[wp2]->no ||
        nc->d.c->walls[i]->p[(j-1)&0x3]->no==w1->p[wp2]->no))
       { w=nc->d.c->walls[i]; break; }
   if(w==NULL) continue;
   /* OK, w should be one wall next to w1, now look, if it's tagged */
   if((nw=findnode(ws,nc->no*6+w->no))==NULL) continue;
   freenode(ws,nw,NULL); /* untag wall */
   /* Search the shared points */
   for(i=0,w2p1=w2p2=-1;i<4;i++)
    if(w->p[i]->no==w1->p[wp1]->no) w2p1=i;
    else if(w->p[i]->no==w1->p[wp2]->no) w2p2=i;
   if(w2p1<0 || w2p2<0) 
    { printmsg("Arrange bitmaps: couldn't find shared points.????");
      continue; }
   w->corners[w2p1].xpos=w1->corners[wp1].xpos;
   w->corners[w2p1].ypos=w1->corners[wp1].ypos;
   w->corners[w2p2].xpos=w1->corners[wp2].xpos;
   w->corners[w2p2].ypos=w1->corners[wp2].ypos;
   newwall_offset(nc->d.c,w,w2p1,w2p2);
   if((save=malloc(sizeof(struct node *)*2))==NULL)
    { printmsg("Sorry, not enough mem."); freelist(&walls,free); return; }
   save[0]=nc; save[1]=(struct node *)w;
   if(!addnode(&walls,nc->no,save))
    { printmsg("Sorry, not enough mem."); freelist(&walls,free); return; }   
   }
  freenode(&walls,run,free);
  }
 freelist(&walls,free);
 }
 
int connectcubes(struct list *taggedcubes)
 {
 struct node *n,*np,*sn,*cn,*ncp[4];
 int i,j,k,ok,wallnum;
 struct point d;
 struct list fndcubes;
 if(view.currmode!=tt_cube)
  { printmsg("Only in cubemode allowed."); return 1; }
 initlist(&fndcubes);
 for(n=taggedcubes->head;n->next!=NULL;n=n->next)
  for(i=0;i<6;i++)
   if(n->d.n->d.c->nc[i]==NULL)
    {
    freelist(&fndcubes,NULL);
    for(j=0;j<4;j++)
     {
     sn=n->d.n->d.c->walls[i]->p[j];
     for(np=l->pts.head,ok=0;np->next!=NULL;np=np->next)
      {
      for(k=0;k<3;k++) d.x[k]=np->d.p->x[k]-sn->d.p->x[k];
      if(LENGTH(&d)<=view.maxconndist) 
       { 
       for(cn=np->d.lp->c.head;cn->next!=NULL;cn=cn->next)
        {
        if((j==0 || findnode(&fndcubes,cn->d.n->no*4+j-1)) &&
	 cn->d.n->no!=n->no)
         { if(!addnode(&fndcubes,cn->d.n->no*4+j,cn))
	    { printmsg("Sorry, not enough mem."); freelist(&fndcubes,NULL);
	      return 1; }
	   ok=1; }
	}
       }
      }
     if(!ok) break; /* no pnt found for this pnt of the wall->next wall */
     }
    if(!ok) continue; /* next wall */
    /* all four points have an near other pnt */
    for(cn=fndcubes.head,sn=NULL;cn->next!=NULL;cn=cn->next)
     if(cn->no%4==3) { sn=cn; break; }
    if(sn==NULL) continue;  /* no cube between the four points */
    /* find the other three entries */
    ncp[3]=sn;
    for(j=0;j<3;j++)
     if((ncp[j]=findnode(&fndcubes,(ncp[3]->no/4)*4+j))==NULL)
      { printmsg("Can't find lower three points???"); freelist(&fndcubes,
        NULL); return 1; }
    /* ncp is a node -> the node from list c from lp ->
       the cubenode -> cube */
    /* find the wallnumber */
    for(wallnum=0;wallnum<6;wallnum++)
     {
     for(j=0;j<4;j++)
      {
      for(k=0;k<4;k++) if(wallpts[wallnum][j]==ncp[k]->d.n->no) break;
      if(k==4) break;
      }
     if(j==4) break;
     }
    if(wallnum==6)
     { printmsg("Can't get wallnum???"); freelist(&fndcubes,NULL);return 1; }
    /* test if found cube is connected */
    if(ncp[0]->d.n->d.n->d.c->nc[wallnum]!=NULL)
     { printmsg("Cube %d has already a neighbour on wall %d",
        ncp[0]->d.n->d.n->no,wallnum); continue; }
    /* connect the two cubes */
    connect(ncp[0]->d.n->d.n,wallnum,n->d.n,i);
    } /* if no neighbourcube */
 return 6;
 }

int move_pntlist(struct list *l,struct point *r)
 {
 struct point *save,*p;
 struct node *n,*sn;
 int i;
 if((save=malloc(sizeof(struct point)*l->size))==NULL)
  { printmsg("No mem for saving old coords."); return 1; }
 for(sn=l->head,p=save;sn->next!=NULL;sn=sn->next,p++)
  {
  n=sn->d.n;
  *p=*n->d.p;
  for(i=0;i<3;i++)
   n->d.p->x[i]+=r->x[i];
  fittogrid(n->d.p);
  }
 for(sn=l->head,i=0;sn->next!=NULL;sn=sn->next)
  if(!testpnt(sn->d.n)) { i=1;break;}
 if(i)
  {
  for(n=l->head,p=save;p-save<l->size;n=n->next,p++)
   *n->d.n->d.p=*p; 
  return 1;
  }
 else
  for(n=l->head;n->next!=NULL;n=n->next) newcorners(n->d.n);
 free(save);
 return 6;
 }

void growshrink(struct node **ps,int *pntnos,int groworshrink)
 {
 int i,j,ok;
 struct point center,oldpnts[8];
 for(i=0;i<3;i++)
  {
  center.x[i]=0;
  for(j=0;j<pntnos[0];j++) center.x[i]+=ps[pntnos[j+1]]->d.p->x[i];
  center.x[i]/=pntnos[0];
  }
 for(j=0;j<pntnos[0];j++) oldpnts[j]=*ps[pntnos[j+1]]->d.p; 
 for(j=0;j<pntnos[0];j++)
  for(i=0;i<3;i++)
   ps[pntnos[j+1]]->d.p->x[i]=(ps[pntnos[j+1]]->d.p->x[i]-center.x[i])*
    (groworshrink ? view.distscala : 1/view.distscala)+center.x[i];
 for(j=0,ok=1;j<pntnos[0];j++)
  if(!testpnt(ps[pntnos[j+1]])) { ok=0;break; }
 if(!ok) for(j=0;j<pntnos[0];j++) *ps[pntnos[j+1]]->d.p=oldpnts[j];
 }