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
#include "userio.h"
#include "tools.h"
#include "tag.h"
#include "calctxt.h"
#include "insert.h"
#include "stdtypes.h"

void fittogrid(struct point *p)
 {
 int i;
 if(!view.gridonoff) return;
 for(i=0;i<3;i++)
  p->x[i]=floor(p->x[i]/view.gridlength+0.5)*view.gridlength; 
 }
 
/* insert a copy of thing t in cube c */
struct node *insertthing(struct node *c,struct node *t)
 {
 struct node *n;
 struct thing *nt;
 int i,j;
 my_assert(c!=NULL);
 nt=changething(NULL,t==NULL ? NULL : t->d.t,t==NULL ? tt1_item : 
  t->d.t->type1,c->d.c);
 if(nt==NULL) { printmsg(TXT_CANTINSERTTHING); return NULL; }
 nt->tagged=NULL;
 if(nt->type1!=4 && nt->type2!=14 && t!=NULL) nt->type2=t->d.t->type2;
 for(j=0;j<3;j++)
  {
  nt->p[0].x[j]=0.0;
  for(i=0;i<8;i++)
   nt->p[0].x[j]+=c->d.c->p[i]->d.p->x[j]/8.0;
  }
 checkmem(n=addnode(&l->things,l->things.size,nt));
 setthingpts(nt); setthingcube(nt);
 return n;
 }
 
struct wall *insertwall(struct node *c,int wallnum,int txt1,int txt2,int dir)
 {
 struct wall *w;
 int j;
 my_assert(c!=NULL);
 if(c->d.c->walls[wallnum]==NULL)
  { checkmem(w=MALLOC(sizeof(struct wall))); }
 else w=c->d.c->walls[wallnum];
 if(view.pdefcube && view.pdefcube->d.c->walls[view.defwall]) 
  *w=*view.pdefcube->d.c->walls[view.defwall];
 else
  { w->texture1=w->texture2=w->txt2_direction=0; }
 if(txt1>=0) w->texture1=txt1;
 if(txt2>=0) w->texture2=txt2;
 if(dir>=0) w->txt2_direction=dir;
 w->no=wallnum; w->locked=0; w->ls=NULL; 
 for(j=0;j<4;j++)
  {
  w->tagged[j]=NULL;
  w->p[j]=c->d.c->p[wallpts[wallnum][j]];
  if(!view.pdefcube || !view.pdefcube->d.c->walls[view.defwall])
   w->corners[j]=stdcorners[j];
  }
 c->d.c->walls[wallnum]=w;
 recalcwall(c->d.c,wallnum);
 if(view.currwall==wallnum && view.pcurrcube!=NULL && 
  view.pcurrcube->no==c->no) 
  view.pcurrwall=w;
 return w;
 }
 
struct node *insertsingledoor(struct node *c,struct node *pd,int wallnum)
 {
 struct door *d;
 struct node *n;
 struct sdoor *sd;
 my_assert(c!=NULL && wallnum>=0 && wallnum<6 && 
  (init.d_ver>=d2_10_sw || (c->d.c->nc[wallnum]!=NULL || 
  c->d.c->walls[wallnum]==NULL)));
 checkmem(d=MALLOC(sizeof(struct door)));
 if(c->d.c->nc[wallnum]!=NULL)
  {
  if(pd && pd->d.d->w)
   { checkmem(insertwall(c,wallnum,pd->d.d->w->texture1,
      pd->d.d->w->texture2,
      pd->d.d->w->txt2_direction)); }
  else
   if(pig.anims[stdncdoor.animtxt]->pig->anim_t2)
    {checkmem(insertwall(c,wallnum,0,pig.anims[stdncdoor.animtxt]->rdlno,0));}
   else
    checkmem(insertwall(c,wallnum,pig.anims[stdncdoor.animtxt]->rdlno,0,0));
  if(pd && pd->d.d->type1!=door1_switchwithwall)
   *d=*pd->d.d;
  else *d=stdncdoor;
  }
 else
  { if(pd && pd->d.d->type1==door1_switchwithwall)
     { *d=*pd->d.d; 
       c->d.c->walls[wallnum]->texture2=pd->d.d->w->texture2; }
    else { *d=stdnonncdoor;c->d.c->walls[wallnum]->texture2=std_switch_t2; } }
 d->cubenum=c->no; d->wallnum=wallnum; d->sd=NULL;
 d->d=NULL; initlist(&d->sdoors);
 checkmem(n=addnode(&l->doors,-1,d));
 if(d->switchtype!=switch_nothing && d->switchtype!=switch_exit &&
  d->switchtype!=switch_secretexit && l->sdoors.size<254)
  {
  my_assert(pd!=NULL);
  checkmem(sd=MALLOC(sizeof(struct sdoor)));
  *sd=*pd->d.d->sd->d.sd; sd->d=n;
  checkmem(d->sd=addnode(&l->sdoors,-1,sd));
  d->sdoor=d->sd->no;
  setsdoortargets(sd);
  }
 else d->sdoor=0xff;
 if(!initdoor(n)) { killnode(&l->doors,n); my_assert(0); }
 c->d.c->d[wallnum]=n;
 return n;
 } 
 
int insertsdoor(struct node *d)
 {
 struct sdoor *sd;
 checkmem(sd=MALLOC(sizeof(struct sdoor)));
 *sd=stdsdoor; sd->d=d;
 if(d->d.d->type1==door1_switchwithwall) sd->flags|=switchflag_once;
 checkmem(d->d.d->sd=addnode(&l->sdoors,l->sdoors.size,sd));
 return 1;
 }
 
struct node *insertdoor(struct node *c,int wallnum)
 {
 struct node *n;
 int i,wall;
 checkmem(n=insertsingledoor(c,view.pcurrdoor,wallnum));
 if(c->d.c->nc[wallnum])
  {
  for(i=0,wall=-1;i<6;i++)
   if(c->d.c->nc[wallnum]->d.c->nc[i]!=NULL)
    if(c->d.c->nc[wallnum]->d.c->nc[i]->no==c->no) {wall=i; break;}
  my_assert(wall!=-1);
  checkmem(n->d.d->d=insertsingledoor(c->d.c->nc[wallnum],
   view.pcurrdoor ? view.pcurrdoor->d.d->d : NULL,wall));
  n->d.d->d->d.d->d=n;
  }
 else my_assert(init.d_ver>=d2_10_sw);
 return n;
 }

void floatpnt(struct node *c,int pn)
 {
 int j;
 struct listpoint *olp=c->d.c->p[pn]->d.lp,*lp;
 struct node *np;
 checkmem(lp=MALLOC(sizeof(struct listpoint)));
 for(np=olp->c.head;np->next!=NULL;np=np->next)
  if(np->d.n->no==c->no) break;
 my_assert(np->next!=NULL);
 freenode(&olp->c,np,NULL);
 lp->p=olp->p;
 initlist(&lp->c); lp->tagged=NULL;
 checkmem(addnode(&lp->c,pn,c));
 checkmem(np=addnode(&l->pts,l->pts.size,lp));
 c->d.c->p[pn]=np; 
 for(j=0;j<3;j++)
  c->d.c->walls[wallno[pn][0][j]]->p[wallno[pn][1][j]]=np;
 }
 
void justdelconnect(struct node *c,int wn)
 {
 struct node *nc;
 int nwn;
 struct wall *w;
 nc=c->d.c->nc[wn];
 my_assert(nc!=NULL && c!=NULL && c->d.c->d[wn]==NULL && 
  c->d.c->walls[wn]==NULL);
 for(nwn=0;nwn<6;nwn++)
  if(nc->d.c->nc[nwn]!=NULL && nc->d.c->nc[nwn]->no==c->no) break; 
 my_assert(nwn<6);
 checkmem(w=insertwall(nc,nwn,-1,-1,-1)); nc->d.c->nc[nwn]=NULL;
 checkmem(w=insertwall(c,wn,-1,-1,-1)); c->d.c->nc[wn]=NULL;
 }
 
void insertpnt(struct node *c,int pn)
 {
 int j;
 struct listpoint *olp=c->d.c->p[pn]->d.lp;
 /* kill all connections */
 if(olp->c.size<2) return;
 for(j=0;j<3;j++)
  if(c->d.c->nc[wallno[pn][0][j]]!=NULL) justdelconnect(c,wallno[pn][0][j]);
 floatpnt(c,pn);
 }
 
/* delete the connection on cube c side wn. If fast==1 float all points
 which are only connected with the cube c after deleting the connection. */
void deleteconnect(struct node *c,int wn,int fast)
 {
 int i,j;
 my_assert(c!=NULL && wn>=0 && wn<6 && c->d.c->nc[wn]!=NULL && 
  c->d.c->d[wn]==NULL);
 justdelconnect(c,wn);
 if(fast)
  {
  for(i=0;i<4;i++)
   {
   for(j=0;j<3;j++) if(c->d.c->nc[wallno[wallpts[wn][i]][0][j]]!=NULL) break;
   if(j==3) floatpnt(c,wallpts[wn][i]);
   }
  }
 }
 
void killtarget(struct sdoor *sd,int n)
 {
 int i,j;
 my_assert(sd!=NULL);
 for(i=0;i<sd->num;i++) 
  if(sd->target[i]->no==n) break;
 if(i==sd->num) { printmsg(TXT_CANTKILLTARGET,n); return; }
 for(j=i;j<sd->num-1;j++) sd->target[j]=sd->target[j+1];
 sd->num--; 
 }
 
void killsdoorlist(struct list *l,int no)
 {
 struct node *n;
 my_assert(l!=NULL);
 for(n=l->head;n->next!=NULL;n=n->next)
  killtarget(n->d.n->d.d->sd->d.sd,no);
 }
 
/* Remove all links to the switch sd in its targets. -> The switch sd
 switches 0 targets after this function was called */
void cleansdoor(struct sdoor *sd)
 {
 int k;
 struct list *tl;
 struct node *n;
 my_assert(sd!=NULL);
 for(k=0;k<sd->num;k++)
  {
  switch(getsdoortype(sd))
   {
   case sdtype_door: tl=&sd->target[k]->d.d->sdoors; break;
   case sdtype_cube: case sdtype_side:
    tl=&sd->target[k]->d.c->sdoors; break;
   default: tl=NULL;
   }
  if(tl)
   if((n=findnode(tl,sd->d->no))==NULL)
    waitmsg(TXT_CLEANNODOORFORSWITCH,sd->d->no);
   else freenode(tl,n,NULL);
  sd->target[k]=NULL;
  }
 sd->num=0;
 }
 
/* Kill the switch n */ 
void deletesdoor(struct node *n)
 {
 my_assert(n!=NULL);
 cleansdoor(n->d.sd); 
 if(n->d.sd->d) n->d.sd->d->d.d->sd=NULL;
 freenode(&l->sdoors,n,free);
 }
 
void deletesingledoor(struct node *n)
 {
 int i;
 my_assert(n!=NULL && n->d.d->c!=NULL);
 n->d.d->c->d.c->d[n->d.d->wallnum]=NULL;
 if(n->d.d->c->d.c->nc[n->d.d->wallnum])
  {
  for(i=0;i<4;i++) untag(tt_edge,n->d.d->c,n->d.d->wallnum,i);
  freewall(l,n->d.d->c->d.c,n->d.d->wallnum);
  if(view.pcurrwall==n->d.d->w) view.pcurrwall=NULL;
  if(view.pdefcube==n->d.d->c && view.defwall==n->d.d->wallnum)
   { view.pdeflevel=NULL; view.pdefcube=NULL; view.defwall=0; }
  if(n->d.d->d) n->d.d->d->d.d->d=NULL;
  }
 else 
  { my_assert(init.d_ver>=d2_10_sw);
    n->d.d->w->texture2=0; /* delete the switch */ }
 if(n->d.d->sd) deletesdoor(n->d.d->sd);
 if(n==view.pcurrdoor) 
  { view.pcurrdoor=view.pcurrdoor->prev->prev ? view.pcurrdoor->prev :
     view.pcurrdoor->next;
    if(view.pcurrdoor->next==NULL) view.pcurrdoor=NULL; }
 killsdoorlist(&n->d.d->sdoors,n->no);
 untag(tt_door,n);
 freenode(&l->doors,n,freedoor);
 }
 
void deletedoor(struct node *n)
 {
 struct node *d2;
 my_assert(n!=NULL);
 d2=n->d.d->d;
 if(d2!=NULL) deletesingledoor(d2);
 deletesingledoor(n);
 }

void deletelistpnt(struct list *pts,struct node *nlp)
 {
 untag(tt_pnt,nlp);
 if(nlp==view.pcurrpnt)
  view.pcurrpnt=nlp->prev->prev ? nlp->prev : nlp->next;
 my_assert(nlp->next!=NULL);
 freenode(pts,nlp,freelistpnt);
 }

/* Delete all lightsources referencing the cube */
void delete_ref_ls(struct node* c)
{
    struct node* p;
    struct node* pe;
    struct node* nlse;
    struct lightsource* ls;
    int flag;
    int w,co;
    long overflow;

    for(p=l->lightsources.head; p->next!=NULL; p=p->next)
    {
        if(p->d.ls)
        {
            ls = p->d.ls;
            flag = 0;

            for(pe=ls->effects.head; pe!=NULL; pe=pe->next)
            {
                if(pe->d.lse!=NULL)
                {
                    if(pe->d.lse->cube == c)
                        flag = 1;
                }
            }
            if(flag)
            {
               for(nlse=ls->effects.head->next;nlse!=NULL;nlse=nlse->next)
               {
                   for(w=0;w<6;w++)
                   {
                       if(nlse->prev->d.lse->cube->d.c->walls[w])
                       {
                           for(co=0;co<4;co++)
                           {
                              overflow=nlse->prev->d.lse->cube->d.c->walls[w]->corners[co].light-
                                 nlse->prev->d.lse->add_light[w*4+co];
                              nlse->prev->d.lse->cube->d.c->walls[w]->corners[co].light=
                                 overflow<view.illum_minvalue ? view.illum_minvalue : overflow;
                           }
                       }
                   }
                   freenode(&ls->effects,nlse->prev,free);
               }
            }
        }
    }
}

/* deletes cube n. if cubes==NULL || pts==NULL the cube is deleted
 in the current level */
void deletecube(struct list *pcubes,struct list *ppts,struct node *n)
 {
 int k,m,w;
 struct cube *nc,*c;
 struct listpoint *lp;
 struct node *cn;
 struct list *cubes=pcubes,*pts=ppts;
 my_assert(n!=NULL);
 c=n->d.c;
 if(cubes==NULL || pts==NULL)
  {
  if(l->exitcube && l->exitcube->no==n->no) l->exitcube=NULL;
  untag(tt_cube,n);
  cubes=&l->cubes; pts=&l->pts;
  if(n->no==view.pcurrcube->no)
   { 
   for(k=0;k<6;k++) if(c->nc[k]!=NULL) break;
   if(k==6)
    view.pcurrcube=view.pcurrcube->prev->prev ? view.pcurrcube->prev :
     view.pcurrcube->next;
   else
    { view.pcurrcube=c->nc[k];
      for(m=0;m<6;m++) 
       if(view.pcurrcube->d.c->nc[m]==n)
        { view.currwall=m; break; } }
   my_assert(view.pcurrcube->next && view.pcurrcube->prev);
   }
  if(n==view.pdefcube)
   { view.pdeflevel=NULL; view.pdefcube=NULL; view.defwall=0; }
  view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
  do
   { for(k=0;k<8;k++)
      if(view.pcurrcube->d.c->p[k]==view.pcurrpnt) break;
     if(k<8) 
      { view.pcurrpnt=view.pcurrpnt->next;
        if(view.pcurrpnt->next==NULL) view.pcurrpnt=l->pts.head; } }
  while(k<8);
  }
 delete_ref_ls(n);
 for(k=0;k<6;k++)
  {
  if(c->walls[k]!=NULL && c->walls[k]->ls!=NULL)
   { my_assert(pcubes==NULL && ppts==NULL);
   freenode(&l->lightsources,c->walls[k]->ls,freelightsource); }
  for(m=0;m<4;m++) untag(tt_edge,n,k,m);
  untag(tt_wall,n,k);
  if(c->nc[k]!=NULL)
   {
   nc=c->nc[k]->d.c;
   for(m=0,w=-1;m<6;m++)
    if(nc->nc[m]!=NULL && nc->nc[m]->no==n->no) { w=m; break; }
   my_assert(w!=-1 && c->nc[k]->d.c->walls[w]==NULL);
   nc->nc[w]=NULL;
   insertwall(c->nc[k],w,-1,-1,-1);
   }
  }
 for(k=0;k<8;k++)
  {
  lp=c->p[k]->d.lp;
  for(cn=lp->c.head->next;cn!=NULL;cn=cn->next)
   if(cn->prev->d.n->no==n->no) freenode(&lp->c,cn->prev,NULL);
  if(lp->c.size<=0) deletelistpnt(pts,c->p[k]);
  }
 if(c->cp!=NULL) freenode(&l->producers,c->cp,free);
 killsdoorlist(&c->sdoors,n->no);
 for(cn=c->things.head;cn->next!=NULL;cn=cn->next)
  cn->d.t->nc=NULL;
 freelist(&c->things,NULL);
 freelist(&c->fl_lights,NULL);
 freenode(cubes,n,freecube);
 }

/* replace point oldp with point newp in all cubes of oldp
   starting at node sn (if NULL starting at head). */
int changepnt(struct node *oldp,struct node *newp,struct node *sn)
 {
 struct node *n,*cn;
 int pn;
 int i;
 if(oldp==newp) return 1; /* points are the same */
 /* change all old points in new points. */
 for(cn=(sn==NULL) ? oldp->d.lp->c.head : sn->next;cn->next!=NULL;cn=cn->next)
  {
  n=cn->d.n; pn=cn->no;
  n->d.c->p[pn]=newp;
  if(addnode(&newp->d.lp->c,cn->no,n)==NULL) return 0;
  for(i=0;i<3;i++)
   {
   if(n->d.c->walls[wallno[pn][0][i]])
    n->d.c->walls[wallno[pn][0][i]]->p[wallno[pn][1][i]]=newp;
   n->d.c->recalc_polygons[wallno[pn][0][i]]=1;
   }
  }
 for(cn=((sn==NULL) ? oldp->d.lp->c.head : sn->next)->next;cn!=NULL;
     cn=cn->next)
  freenode(&oldp->d.lp->c,cn->prev,NULL);
 return 1;
 }
 
/* connect cube c1, wall w1 with cube c2, wall w2. fitting wall w2.
   both walls must exist. If pts==NULL delete in current level.
   returns 0 if connection is not possible, otherwise 1 */
int connectcubes(struct list *pts,struct node *nc1,int w1,struct node *nc2,
 int w2)
 {
 struct wall *pw2;
 struct cube *c1,*c2;
 int i,j,k,cp[4];
 struct node *pnts[4][2],*on[4]; /* the four changed points [0]:old [1]:new */
 struct point *p1,p2;
 float dist;
 my_assert(nc1!=NULL && nc2!=NULL && w1>=0 && w1<6 && w2>=0 && w2<6);
 c1=nc1->d.c; c2=nc2->d.c;
 pw2=c2->walls[w2];
 my_assert(pw2!=NULL && c1->walls[w1]!=NULL);
 if(nc1->no==nc2->no) { printmsg(TXT_CONNCUBEITSELF,nc1->no); return 0; }
 if(c1->d[w1]!=NULL || c2->d[w2]!=NULL)
  { printmsg(TXT_CONNWALLINWAY,nc1->no,w1,nc2->no,w2); return 0; }
 /* search nearest points  */
 for(j=0;j<4;j++)
  {
  p1=c1->walls[w1]->p[j]->d.p;
  for(i=0,cp[j]=-1,dist=view.maxuserconnect;i<4;i++)
   {
   for(k=0;k<3;k++)
    p2.x[k]=pw2->p[i]->d.p->x[k]-p1->x[k];
   if(dist>LENGTH(&p2)) { dist=LENGTH(&p2); cp[j]=i; }
   }
  if(cp[j]==-1)
   { printmsg(TXT_CONNCUBETOOFARAWAY,nc1->no,w1,nc2->no,w2); return 0; }
  for(i=0;i<j;i++)
   if(cp[i]==cp[j])
    { printmsg(TXT_CONNAMBIGUOUS,nc1->no,w1,nc2->no,w2); return 0; }
  }
 /* copy the new four points on the old four points of the current wall and
   free the new four */
 for(j=0;j<4;j++)
  {
  my_assert(cp[j]>=0 && cp[j]<4);
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
 for(j=0;j<4;j++)
  { untag(tt_edge,nc1,w1,j); untag(tt_edge,nc2,w2,j); }
 c2->nc[w2]=nc1; freewall(pts==NULL ? l : NULL,c2,w2);
 c1->nc[w1]=nc2; freewall(pts==NULL ? l : NULL,c1,w1);
 /* free old points and calculate new textures  */
 for(j=0;j<4;j++)
  if(pnts[j][0]->no!=pnts[j][1]->no)
   {
   if(view.pcurrpnt==pnts[j][0]) view.pcurrpnt=pnts[j][1];
   my_assert(pnts[j][0]->d.lp->c.size==0)
   deletelistpnt(pts==NULL ? &l->pts : pts,pnts[j][0]);
   newcorners(pnts[j][1]);  
   }
 if((view.pdefcube==nc1 && view.defwall==w1) ||
  (view.pdefcube==nc2 && view.defwall==w2))
  { view.pdeflevel=NULL; view.pdefcube=NULL; view.defwall=0; }
 if(pts==NULL)
  if(l->exitcube!=NULL && ((l->exitcube->no==nc1->no && l->exitwall==w1) || 
   (l->exitcube->no==nc2->no && l->exitwall==w2))) l->exitcube=NULL;
 view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
 printmsg(TXT_CUBESCONNECTED,nc1->no,w1,nc2->no,w2);
 return 1;
 }

/* make click point for door */
void makedoorpnt(struct door *d)
 {
 int j;
 struct point diff1,diff2,cp,*p[4];
 my_assert(d!=NULL);
 if(d->w==NULL) return;
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
int initdoor(struct node *n)
 {
 int i,j;
 struct door *d;
 struct sdoor *sd;
 struct node *sdn;
 my_assert(n!=NULL && n->d.d!=NULL);
 d=n->d.d;
 if((d->c=findnode(&l->cubes,d->cubenum))==NULL)
  { waitmsg(TXT_NOCUBEFORDOOR,d->cubenum); return 0; }
 if(d->wallnum>5)
  { waitmsg(TXT_WRONGWALLINDOOR,d->wallnum,d->cubenum); return 0; }
 d->w=d->c->d.c->walls[d->wallnum];
 if(d->w==NULL)
  { waitmsg(TXT_NOWALLFORDOOR,d->cubenum,d->wallnum); return 0; }
 if(init.d_ver<d2_10_sw && d->c->d.c->nc[d->wallnum]==NULL)
  waitmsg(TXT_NONCFORDOOR,d->cubenum,d->wallnum); 
 d->d=NULL; d->edoor=0; d->switchtype=switch_nothing;
 d->tagged=NULL;
 d->old_txt1=d->animtxt>=pig.num_anims || pig.anims[d->animtxt]==NULL || 
  pig.anims[d->animtxt]->pig==NULL || !pig.anims[d->animtxt]->pig->anim_t2 ?
  -1 : d->w->texture1;
 for(i=0;i<6;i++)
  if(d->c->d.c->nc[d->wallnum] && d->c->d.c->nc[d->wallnum]->d.c->nc[i]==d->c)
   { d->d=d->c->d.c->nc[d->wallnum]->d.c->d[i]; break; }
 if(d->sdoor!=0xff)
  {
  if((d->sd=findnode(&l->sdoors,d->sdoor))==NULL)
   { waitmsg(TXT_NOSWITCHFORDOOR,(int)d->sdoor,n->no);
     d->sdoor=0xff; d->sd=NULL; }
  else
   {
   sd=d->sd->d.sd;
   d->switchtype=sd->type;
   sd->d=n;
   for(i=0;i<sd->num;i++)
    {
    if((sdn=findnode(&l->cubes,sd->cubes[i]))==NULL)
     {
     waitmsg(TXT_NOCUBEFORSWITCH,sd->cubes[i]);
     sd->num--;
     for(j=i;j<sd->num;j++) 
      { sd->cubes[j]=sd->cubes[j+1]; sd->walls[j]=sd->walls[j+1]; }
     continue;
     }
    switch(getsdoortype(sd))
     {
     case sdtype_door:
      if(sd->walls[i]>5) 
       { 
       waitmsg(TXT_WRONGWALLFORSWITCH,sd->walls[i],d->sd->no);        
       sd->num--;
       for(j=i;j<sd->num;j++) 
        { sd->cubes[j]=sd->cubes[j+1]; sd->walls[j]=sd->walls[j+1]; }
       continue;
       }
      if((sd->target[i]=sdn->d.c->d[sd->walls[i]])==NULL)
       { 
       waitmsg(TXT_NODOORFORSWITCH,sd->cubes[i],sd->walls[i],d->sd->no);
       sd->num--;
       for(j=i;j<sd->num;j++) 
        { sd->cubes[j]=sd->cubes[j+1]; sd->walls[j]=sd->walls[j+1]; }
       continue;
       }
      addnode(&sd->target[i]->d.d->sdoors,n->no,n);
      break;
     case sdtype_cube: case sdtype_side:
      sd->target[i]=sdn;
      addnode(&sd->target[i]->d.c->sdoors,n->no,n);
      break;
     default: break;
     }
    }
   }
  }
 else d->sd=NULL;
 makedoorpnt(d);
 return 1;
 }

/* init of all pointers in a cube. the fields c->pts,c->nextcubes,c->doors
   must be set. */
int initcube(struct node *n)
 {
 int j,k;
 struct listpoint *lp;
 struct cube *c;
 my_assert(n!=NULL && l!=NULL);
 c=n->d.c;
 initlist(&c->sdoors); initlist(&c->things); initlist(&c->fl_lights);
 for(j=0;j<8;j++)
  {
  if((c->p[j]=findnode(&l->pts,(int)c->pts[j]))==NULL)
   { return yesnomsg(TXT_NOCUBEPNT,(int)c->pts[j]) ? -1 : 0; }
  lp=c->p[j]->d.lp;
  checkmem(addnode(&lp->c,j,n));
  }
 for(j=0;j<6;j++)
  {
  c->polygons[j*2]=NULL; c->polygons[j*2+1]=NULL; c->recalc_polygons[j]=1;
  c->tagged_walls[j]=NULL; 
  if(c->nextcubes[j]>=0x7fff)
   { c->nc[j]=NULL; 
     /*for exit:*/ if(c->walls[j]==NULL) insertwall(n,j,-1,-1,-1); }
  else
   if((c->nc[j]=findnode(&l->cubes,(int)c->nextcubes[j]))==NULL)
    { if(yesnomsg(TXT_NONCFORCUBE,(int)c->nextcubes[j],n->no)) return -1;
      c->walls[j]=NULL; c->nc[j]=NULL; c->nextcubes[j]=0xffff; 
      insertwall(n,j,-1,-1,-1); }
  if(c->doors[j]==0xff)
   c->d[j]=NULL;
  else
   if((c->d[j]=findnode(&l->doors,(int)c->doors[j]))==NULL)
    { if(yesnomsg(TXT_NODOORFORCUBE,(int)c->doors[j],n->no)) return -1;
      c->d[j]=NULL; c->doors[j]=0xff; }
  for(k=0;k<4;k++)
   if(c->walls[j]!=NULL)
    { c->walls[j]->p[k]=c->p[wallpts[j][k]];
      c->walls[j]->tagged[k]=NULL; }
  }
 c->tagged=NULL;
 if(c->type==4 && c->prodnum>=0) /* producer */
  {
  if((c->cp=findnode(&l->producers,c->prodnum))==NULL)
   { if(yesnomsg(TXT_NOPRODFORCUBE,(int)c->prodnum,n->no)) return -1;
     c->prodnum=0; }
  else c->cp->d.cp->c=n;
  }
 else c->cp=NULL;
 return 1;
 }

/* test if the cubes is convex or weird.
   return 1 if cube is alright, 0 otherwise */
int testcube(struct node *nc,int withmsg)
 {
 unsigned short int newpnum=0,j,l1n,l2n,l3n;
 struct point l1,l2,l3,e;
 float ll1;
 struct node *nt;
 /* test if two points are equal */
 for(l1n=0;l1n<6;l1n++)
  for(l2n=0;l2n<4;l2n++)
   {
   for(l3n=0;l3n<3;l3n++)
    l1.x[l3n]=nc->d.c->p[wallpts[l1n][l2n]]->d.p->x[l3n]-
     nc->d.c->p[wallpts[l1n][(l2n+1)&3]]->d.p->x[l3n]; 
   ll1=LENGTH(&l1);
   if(ll1<=640.0 || ll1>32700*640.0)
    { if(withmsg) printmsg(TXT_TPLINETOOLONG,l2n,(l2n+1)&3,l1n,nc->no);
      return 0; }
   } 
 for(nt=nc->d.c->things.head;nt->next!=NULL;nt=nt->next) 
  setthingcube(nt->d.t);
 if(!view.warn_convex) return 1;
 /* Test if all cubes convex and all walls are not weird */
 for(newpnum=0;newpnum<8;newpnum++)
  {
  for(j=0;j<3;j++)
   {
   if(newpnum<4)
    { l1n=newpnum+4; l2n=(newpnum+1)&0x3;l3n=(newpnum-1)&0x3; }
   else
    { l1n=newpnum-4; l3n=(newpnum==7) ? 4 : newpnum+1;
      l2n=(newpnum==4) ? 7 : newpnum-1; }
   l1.x[j]=nc->d.c->p[l1n]->d.p->x[j] - nc->d.c->p[newpnum]->d.p->x[j];
   l2.x[j]=nc->d.c->p[l2n]->d.p->x[j] - nc->d.c->p[newpnum]->d.p->x[j];
   l3.x[j]=nc->d.c->p[l3n]->d.p->x[j] - nc->d.c->p[newpnum]->d.p->x[j];
   }
  normalize(&l1); normalize(&l2); normalize(&l3);
  /* test if l1,l2,l3 are a right-handed system */
  VECTOR(&e,&l3,&l2);
  if(SCALAR(&e,&l1)<=view.mincorner)
   { if(withmsg) printmsg(TXT_TPCUBENOTCONVEX,nc->no,newpnum); return 0; }
     /* this was a nearly left-handed system */
  }
 /* test weird walls */
 for(j=0;j<6;j++)
  {
  for(l1n=0;l1n<3;l1n++)
   l1.x[l1n]=nc->d.c->p[wallpts[j][3]]->d.p->x[l1n]-
    nc->d.c->p[wallpts[j][2]]->d.p->x[l1n];
  for(l1n=0;l1n<3;l1n++)
   l2.x[l1n]=nc->d.c->p[wallpts[j][0]]->d.p->x[l1n]-
    nc->d.c->p[wallpts[j][1]]->d.p->x[l1n];
  if(fabs(SCALAR(&l1,&l2))<=view.minweirdwall*LENGTH(&l1)*LENGTH(&l2))
   {
   for(l1n=0;l1n<3;l1n++)
    l1.x[l1n]=nc->d.c->p[wallpts[j][3]]->d.p->x[l1n]-
     nc->d.c->p[wallpts[j][0]]->d.p->x[l1n];
   for(l1n=0;l1n<3;l1n++)
    l2.x[l1n]=nc->d.c->p[wallpts[j][2]]->d.p->x[l1n]-
     nc->d.c->p[wallpts[j][1]]->d.p->x[l1n];
   /* weird wall? */
   if(fabs(SCALAR(&l1,&l2))<=view.minweirdwall*LENGTH(&l1)*LENGTH(&l2)) 
    { if(withmsg) printmsg(TXT_TPWEIRDWALL,j,nc->no); return 0; }
      /* weird wall! */
   }
  }
 return 1;
 }
 
/* test if one of the cubes depending on point np is not convex or weird.
   return 1 if all cubes are alright, 0 otherwise */
int testpnt(struct node *np)
 {
 struct node *nc;
 for(nc=np->d.lp->c.head;nc->next!=NULL;nc=nc->next) 
  if(!testcube(nc->d.n,1)) return 0;
 return 1;
 }

/* n=cube node. i=wallnum. Searches a side which is near the side
   n,i and connects it with n,i. Returns 0 if connectcubes is not successful,
   1 is returned if the cubes are connected. -1 is returned if no
   corresponding side is found. */
int connectsides(struct node *n,int i)
 {
 struct node *sn,*cn;
 int j,k,m,wallnum,found=-1;
 struct point d;
 float min_d,nd;
 my_assert(i>=0 && i<6 && n!=NULL && n->d.c->nc[i]==NULL);
 for(cn=l->cubes.head,sn=NULL,min_d=view.maxconndist*4.0;cn->next!=NULL;
  cn=cn->next)
  {
  if(cn==n) continue;
  for(wallnum=0;wallnum<6;wallnum++)
   if(!cn->d.c->nc[wallnum])
    {
    for(m=0;m<4;m++)
     {
      for(j=0,nd=0.0;j<4;j++) 
      {
      for(k=0;k<3;k++) d.x[k]=cn->d.c->p[wallpts[wallnum][3-j]]->d.p->x[k]-
       n->d.c->p[wallpts[i][(j+m)&3]]->d.p->x[k];
      nd+=LENGTH(&d);
      if(nd>=min_d) break;
      }
     if(nd<min_d) { sn=cn; found=wallnum; min_d=nd; }
     }
    }
  }
 return (sn!=NULL ? connectcubes(NULL,sn,found,n,i) : -1);
 }

int move_pntlist(struct list *l,struct point *r)
 {
 struct point *save,*p;
 struct node *n,*sn;
 int i;
 if((save=MALLOC(sizeof(struct point)*l->size))==NULL)
  { printmsg(TXT_NOMEMFOROLDCOORDS); return 1; }
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
 FREE(save);
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
  {
  for(i=0;i<3;i++)
   ps[pntnos[j+1]]->d.p->x[i]=(ps[pntnos[j+1]]->d.p->x[i]-center.x[i])*
    (groworshrink ? view.distscala : 1/view.distscala)+center.x[i];
  fittogrid(ps[pntnos[j+1]]->d.p);
  }
 for(j=0,ok=1;j<pntnos[0];j++)
  if(!testpnt(ps[pntnos[j+1]])) { ok=0;break; }
 if(!ok) for(j=0;j<pntnos[0];j++) *ps[pntnos[j+1]]->d.p=oldpnts[j];
 else for(j=0;j<pntnos[0];j++) newcorners(ps[pntnos[j+1]]);
 }

/* make stdcube at the origin */ 
struct node *newcube(struct leveldata *ld)
 {
 struct cube *nc;
 struct listpoint *lp;
 struct node *nnc,*nlp;
 int i,j;
 struct leveldata *ol;
 checkmem(nc=MALLOC(sizeof(struct cube)));
 /* do the init stuff for the cube */
 nc->type=0; nc->prodnum=-1; nc->value=0; initlist(&nc->sdoors);
 nc->cp=NULL; nc->tagged=NULL; initlist(&nc->things); 
 for(j=0;j<6;j++) 
  { nc->nc[j]=NULL; nc->d[j]=NULL; nc->tagged_walls[j]=NULL;
    nc->nextcubes[j]=0xffff; nc->doors[j]=0xff; }
 checkmem(nnc=addnode(&ld->cubes,-1,nc));
 for(j=0;j<8;j++) 
  {
  checkmem(lp=MALLOC(sizeof(struct listpoint)));
  checkmem(nlp=addnode(&ld->pts,-1,lp)); initlist(&lp->c); lp->tagged=NULL;
/*  checkmem(addnode(&lp->c,j,nnc)); is done by initcube */
  nc->p[j]=nlp; lp->p=stdcubepts[j]; nc->pts[j]=nc->p[j]->no;
  }
 /* and now make the walls */
 for(i=0;i<6;i++)
  {
  checkmem(nc->walls[i]=MALLOC(sizeof(struct wall)));
  nc->walls[i]->texture1=nc->walls[i]->texture2=0;
  nc->walls[i]->txt2_direction=0;
  for(j=0;j<4;j++) 
   { nc->walls[i]->corners[j]=stdcorners[j];
     nc->walls[i]->p[j]=nc->p[wallpts[i][j]];
     nc->walls[i]->tagged[j]=NULL; }
  nc->walls[i]->no=i; nc->walls[i]->locked=0; nc->walls[i]->ls=NULL;
  }
 ol=l; l=ld; initcube(nnc); l=ol;
 return nnc;
 }

void delflickeringlight(struct lightsource *ls)
 {
 struct node *ne,*nfl;
 if(ls->fl==NULL) return;
 for(ne=ls->effects.head;ne->next!=NULL;ne=ne->next)
  for(nfl=ne->d.lse->cube->d.c->fl_lights.head->next;nfl!=NULL;nfl=nfl->next)
   if(nfl->prev->d.fl==ls->fl)
    freenode(&ne->d.lse->cube->d.c->fl_lights,nfl->prev,NULL);
 FREE(ls->fl); ls->fl=NULL;
 }

void freewall(struct leveldata *ld,struct cube *c,int w)
 {
 my_assert(c->walls[w]!=NULL && c->nc[w]!=NULL);
 if(c->walls[w]->ls)
  {
  if(ld)
      freenode(&ld->lightsources,c->walls[w]->ls,freelightsource);
  else
      freenode(&l->lightsources,c->walls[w]->ls,freelightsource);
  }
 FREE(c->walls[w]); c->walls[w]=NULL;
 }

/* insert a cube at c,wallnum. If cubes and pts==NULL, insert the cube
 in the current level otherwise insert it in the lists cubes and pts.
 If depth<0 use an reasonable length otherwise use depth. */
struct node *insertcube(struct list *cubes,struct list *pts,struct node *c,
 int wallnum,float depth)
 {
 struct cube *nc;
 struct node *nnc,*nlp;
 struct listpoint *lp;
 struct wall *w;
 struct point d,n1,n2,e1,e2;
 float length;
 int i,j;
 my_assert(c!=NULL && wallnum>=0 && wallnum<6 && l!=NULL);
 if(cubes==NULL || pts==NULL) { cubes=&l->cubes; pts=&l->pts; }
 checkmem(nc=MALLOC(sizeof(struct cube)));
 /* do the init stuff for the cube */
 nc->type=0; nc->prodnum=-1; nc->value=0; initlist(&nc->sdoors);
 nc->cp=NULL; nc->tagged=NULL; initlist(&nc->things); initlist(&nc->fl_lights);
 for(j=0;j<6;j++) 
  { nc->nc[j]=NULL; nc->d[j]=NULL; nc->tagged_walls[j]=NULL;
    nc->polygons[j*2]=nc->polygons[j*2+1]=NULL;
    nc->recalc_polygons[j]=1; }
 checkmem(nnc=addnode(cubes,-1,nc));
 /* Calculate the 'depth'-vector for the new cube */
 if(depth<=0.0)
  { for(j=0;j<3;j++) 
     for(i=0,d.x[j]=0;i<4;i++)
      d.x[j]+=(c->d.c->p[wallpts[wallnum][i]]->d.p->x[j]-
       c->d.c->p[wallpts[oppwalls[wallnum]][3-i]]->d.p->x[j])/4.0;
    depth=LENGTH(&d); }
 /* Calculate the normalvector on the side */
 for(i=0;i<3;i++)
  { e1.x[i]=c->d.c->p[wallpts[wallnum][3]]->d.p->x[i]-
     c->d.c->p[wallpts[wallnum][0]]->d.p->x[i];
    e2.x[i]=c->d.c->p[wallpts[wallnum][1]]->d.p->x[i]-
     c->d.c->p[wallpts[wallnum][0]]->d.p->x[i]; }
 VECTOR(&n1,&e1,&e2); length=depth/LENGTH(&n1);
 for(j=0;j<3;j++) n1.x[j]*=length;
 for(i=0;i<3;i++)
  { e1.x[i]=c->d.c->p[wallpts[wallnum][1]]->d.p->x[i]-
     c->d.c->p[wallpts[wallnum][2]]->d.p->x[i];
    e2.x[i]=c->d.c->p[wallpts[wallnum][3]]->d.p->x[i]-
     c->d.c->p[wallpts[wallnum][2]]->d.p->x[i]; }
 VECTOR(&n2,&e1,&e2); length=depth/LENGTH(&n2);
 for(j=0;j<3;j++) n2.x[j]*=length;
 for(j=0;j<3;j++) d.x[j]=(n1.x[j]+n2.x[j])/2;
 /* Assign the points. The new cube is the same way numbered as the
    old is. */
 for(i=0;i<4;i++)
  {
  nc->p[wallpts[oppwalls[wallnum]][3-i]]=c->d.c->p[wallpts[wallnum][i]];
  checkmem(addnode(&c->d.c->p[wallpts[wallnum][i]]->d.lp->c,
   wallpts[oppwalls[wallnum]][3-i],nnc));
  checkmem(lp=MALLOC(sizeof(struct listpoint)));
  checkmem(nlp=addnode(pts,-1,lp));
  initlist(&lp->c); lp->tagged=NULL;
  checkmem(addnode(&lp->c,wallpts[wallnum][i],nnc));
  nc->p[wallpts[wallnum][i]]=nlp;
  for(j=0;j<3;j++)
   lp->p.x[j]=nc->p[wallpts[oppwalls[wallnum]][3-i]]->d.p->x[j]+d.x[j];
  }
 /* and now make the walls */
 for(i=0;i<6;i++)
  {
  if(i==oppwalls[wallnum]) { nc->walls[i]=NULL; continue; }
  checkmem(nc->walls[i]=MALLOC(sizeof(struct wall)));
  w=c->d.c->walls[i]!=NULL && c->d.c->d[i]==NULL ? 
   c->d.c->walls[i] : (view.pdefcube==NULL ? NULL : 
   view.pdefcube->d.c->walls[view.defwall]);
  nc->walls[i]->texture1=w==NULL ? 0 : w->texture1;
  nc->walls[i]->texture2=w==NULL ? 0 : w->texture2;
  nc->walls[i]->txt2_direction=w==NULL ? 0 : w->txt2_direction;
  for(j=0;j<4;j++) 
   { nc->walls[i]->corners[j]=w==NULL ? stdcorners[j] : w->corners[j];
     nc->walls[i]->p[j]=nc->p[wallpts[i][j]];
     nc->walls[i]->tagged[j]=NULL; }
  nc->walls[i]->no=i; nc->walls[i]->locked=0; nc->walls[i]->ls=NULL;
  recalcwall(nc,i);  
  }
 /* and know kill the wall in the other cube */
 c->d.c->nc[wallnum]=nnc; nc->nc[oppwalls[wallnum]]=c;
 freewall(pts==NULL && cubes==NULL ? l : NULL,c->d.c,wallnum);
 if(cubes==NULL && pts==NULL)
  {
  l->levelsaved=l->levelillum=0;
  if(view.pdefcube==c && view.defwall==wallnum)
   { view.pdeflevel=NULL; view.pdefcube=NULL; view.defwall=0; }
  if(l->exitcube!=NULL && l->exitcube->no==c->no && l->exitwall==wallnum)
   l->exitcube=NULL;
  view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
  }
 return nnc;
 }
 
void makesidestdshape(struct cube *cube,int w)
 {
 struct point *p[4],c,x,y,d[4],np1[4],np2[4],op[4];
 int i,pn;
 float f,phi,a,b;
 my_assert(cube!=NULL);
 for(pn=0;pn<4;pn++) 
  { p[pn]=cube->p[wallpts[w][pn]]->d.p; op[pn]=*p[pn]; }
 for(i=0;i<3;i++)
  { c.x[i]=0.0; for(pn=0;pn<4;pn++) c.x[i]+=p[pn]->x[i]/4.0;
    x.x[i]=p[2]->x[i]-p[1]->x[i]+p[3]->x[i]-p[0]->x[i];
    y.x[i]=p[1]->x[i]-p[0]->x[i]+p[2]->x[i]-p[3]->x[i]; }
 normalize(&x); f=SCALAR(&x,&y);
 for(i=0;i<3;i++) y.x[i]-=f*x.x[i];
 normalize(&y);
 for(i=0;i<3;i++) for(pn=0;pn<4;pn++) d[pn].x[i]=p[pn]->x[i]-c.x[i]; 
 /* using stdcubepts[4+p] because they are numbered in the right way:
    corner 0=point 4 1=5 2=6 3=7 */
 for(pn=0,a=0.0;pn<4;pn++) a+=stdcubepts[4+pn].x[0]*SCALAR(&d[pn],&x)+
  stdcubepts[4+pn].x[1]*SCALAR(&d[pn],&y);
 if(fabs(a)>0.01)
  {
  for(pn=0,b=0.0;pn<4;pn++) b+=stdcubepts[4+pn].x[0]*SCALAR(&d[pn],&y)-
   stdcubepts[4+pn].x[1]*SCALAR(&d[pn],&x);
  phi=atan(b/a);
  }
 else phi=M_PI/2;
 /* now we have two solutions: phi and phi+M_PI. Check which one is the
    one with the largest distance between the four points and which one
    the one with the smallest distance */
 for(pn=0,a=b=0.0;pn<4;pn++)
  {
  for(i=0;i<3;i++)
   { 
   np1[pn].x[i]=d[pn].x[i]
    -(stdcubepts[4+pn].x[0]*cos(phi)-stdcubepts[4+pn].x[1]*sin(phi))*x.x[i]
    -(stdcubepts[4+pn].x[0]*sin(phi)+stdcubepts[4+pn].x[1]*cos(phi))*y.x[i];
   np2[pn].x[i]=d[pn].x[i]
    +(stdcubepts[4+pn].x[0]*cos(phi)-stdcubepts[4+pn].x[1]*sin(phi))*x.x[i]
    +(stdcubepts[4+pn].x[0]*sin(phi)+stdcubepts[4+pn].x[1]*cos(phi))*y.x[i]; 
   }
  a+=SCALAR(&np1[pn],&np1[pn]); b+=SCALAR(&np2[pn],&np2[pn]);
  }
 for(pn=0;pn<4;pn++)
  { for(i=0;i<3;i++)
     p[pn]->x[i]-=a<b ? np1[pn].x[i] : np2[pn].x[i];
    if(!testpnt(cube->p[wallpts[w][pn]])) break; }
 if(pn!=4) for(pn=0;pn<4;pn++) *p[pn]=op[pn];
 else for(pn=0;pn<4;pn++) newcorners(cube->p[wallpts[w][pn]]);
 } 
