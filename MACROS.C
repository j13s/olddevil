/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    macros.c - build, save and insert macros.
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
#include "savetool.h"
#include "insert.h"
#include "readlvl.h"
#include "tag.h"
#include "macros.h"

/* make a coordsystem naxis out of cube c in the following way:
   x-axis: vector from point pnt to point (pnt+1)&0x3 of wall.
   y-axis: the part of the vector from point pnt to point (pnt-1)&0x3 of wall
    which goes orthogonal to x-axis.
   z-axis: vector product of y and x-axis. (left-handed)
   if inout=1 x and y axis are exchanged.
   all three vectors are normed. */
void getcubecoords(struct cube *c,int wall,int pnt,struct point *naxis,
 int inout)
 {
 int j;
 struct point *c0=&naxis[inout],*c1=&naxis[inout^1];
 float f;
 for(j=0;j<3;j++)
  {
  c0->x[j]=c->p[wallpts[wall][(pnt+1)&3]]->d.p->x[j]-
   c->p[wallpts[wall][pnt]]->d.p->x[j];
  c1->x[j]=c->p[wallpts[wall][(pnt-1)&3]]->d.p->x[j]-
   c->p[wallpts[wall][pnt]]->d.p->x[j];
  }
 normalize(c0); normalize(c1);
 /* making yaxis orthogonal to xaxis: */
 f=SCALAR(c0,c1);
 for(j=0;j<3;j++)
  naxis[1].x[j]=naxis[1].x[j]-f*naxis[0].x[j];
 /* now we have two normed vectors, get the third one: */
 normalize(&naxis[1]); 
 VECTOR(&naxis[2],&naxis[0],&naxis[1])
 }

void copy_lightsource(struct leveldata *insertinto,struct node **cubes,
 int cubessize,struct lightsource *sls,int checktagged)
 {
 struct lightsource *ls;
 struct ls_effect *lse;
 struct node *sn,*nls;
 checkmem(ls=MALLOC(sizeof(struct lightsource)));
 my_assert(sls->cube!=NULL && sls->cube->no>=0 &&
  sls->cube->no<cubessize && cubes[sls->cube->no]!=NULL);
 ls->cube=cubes[sls->cube->no];
 ls->w=sls->w; ls->fl=NULL;
 my_assert(ls->cube!=NULL && ls->cube->d.c->walls[(int)ls->w]!=NULL);
 initlist(&ls->effects);
 checkmem(nls=addnode(&insertinto->lightsources,-1,ls));
 ls->cube->d.c->walls[(int)ls->w]->ls=nls;
 if(sls->fl!=NULL)
  {
  checkmem(ls->fl=MALLOC(sizeof(struct flickering_light)));
  *ls->fl=*sls->fl; ls->fl->ls=nls; 
  }
 for(sn=sls->effects.head;sn->next!=NULL;sn=sn->next)
  if(!checktagged || sn->d.lse->cube->d.c->tagged)
   {
   checkmem(lse=MALLOC(sizeof(struct ls_effect)));
   *lse=*sn->d.lse;
   my_assert(sn->d.lse->cube->no>=0 && sn->d.lse->cube->no<cubessize &&
    cubes[sn->d.lse->cube->no]!=NULL);
   lse->cube=cubes[sn->d.lse->cube->no];
   checkmem(addnode(&ls->effects,-1,lse));
   if(ls->fl!=NULL)
    checkmem(addnode(&lse->cube->d.c->fl_lights,-1,ls->fl));
   }
 }

/* copies all in l tagged cubes with doors and all in tl
   tagged things to a macro */
struct leveldata *buildmacro(struct leveldata *ld)
 {
 struct list *l=&ld->tagged[tt_cube],*tl=&ld->tagged[tt_thing];
 struct node *n,*tn,*sn,*nn;
 struct leveldata *m;
 struct cube *c;
 struct producer *cp;
 struct listpoint *lp;
 struct door *d;
 struct sdoor *sd;
 struct thing *t;
 struct node **cubes;
 int i,j;
 sortlist(&ld->cubes,0);
 checkmem(cubes=MALLOC(sizeof(struct node *)*ld->cubes.size));
 for(i=0;i<ld->cubes.size;i++) cubes[i]=NULL;
 checkmem(m=emptylevel());
 /* first make lists */
 for(tn=l->head;tn->next!=NULL;tn=tn->next)
  {
  n=tn->d.n;
  checkmem(c=MALLOC(sizeof(struct cube)));
  checkmem(nn=addnode(&m->cubes,n->no,c));
  my_assert(n->no>=0 && n->no<ld->cubes.size);
  cubes[n->no]=nn;
  *c=*n->d.c;
  c->tagged=NULL; initlist(&c->things);
  for(j=0;j<6;j++) c->tagged_walls[j]=NULL;
  if(c->cp!=NULL)
   {
   checkmem(cp=MALLOC(sizeof(struct producer)));
   *cp=*c->cp->d.cp;
   checkmem(sn=addnode(&m->producers,c->cp->no,cp));
   c->cp=sn;
   cp->c=nn;
   }
  for(j=0;j<8;j++)
   {
   if((sn=findnode(&m->pts,n->d.c->p[j]->no))==NULL)
    {
    checkmem(lp=MALLOC(sizeof(struct listpoint)));
    lp->p=n->d.c->p[j]->d.lp->p;
    initlist(&lp->c); lp->tagged=NULL;
    checkmem(sn=addnode(&m->pts,n->d.c->p[j]->no,lp));
    }
   else
    lp=sn->d.lp;
   c->p[j]=sn;
/*   checkmem(addnode(&lp->c,j,nn)); this is wrong here because it is done
 in initlevel */
   }
  for(j=0;j<6;j++)
   {
   if(n->d.c->walls[j]!=NULL)
    {
    checkmem(c->walls[j]=MALLOC(sizeof(struct wall)));
    *c->walls[j]=*n->d.c->walls[j];
    for(i=0;i<4;i++) 
     { c->walls[j]->p[i]=c->p[wallpts[j][i]];
       c->walls[j]->tagged[i]=NULL; }
    c->walls[j]->ls=NULL;
    }
   if(n->d.c->d[j]!=NULL)
    {
    checkmem(d=MALLOC(sizeof(struct door)));
    *d=*n->d.c->d[j]->d.d;
    d->tagged=NULL; d->old_txt1=-1;
    d->c=nn; d->w=c->walls[j];
    checkmem(sn=addnode(&m->doors,n->d.c->d[j]->no,d));
    c->d[j]=sn; d->sdoor=0xff; initlist(&d->sdoors);
    if(d->sd!=NULL)
     {
     checkmem(sd=MALLOC(sizeof(struct sdoor)));
     *sd=*d->sd->d.sd;
     checkmem(d->sd=addnode(&m->sdoors,d->sd->no,sd));
     sd->d=sn; d->sdoor=d->sd->no;
     } 
    }
   }
  }
 /* copy the things */
 for(tn=tl->head;tn->next!=NULL;tn=tn->next)
  {
  j=getsize(ds_thing,tn->d.n->d.t)+(unsigned char *)&tn->d.n->d.t->type1-
   (unsigned char *)tn->d.n->d.t;
  checkmem(t=MALLOC(j));
  memcpy(t,tn->d.n->d.t,j);
  t->tagged=NULL; t->cube=t->nc ? t->nc->no : -1; t->nc=NULL;
  checkmem(addheadnode(&m->things,tn->d.n->no,t));
  }
 /* ok! now make the neighbour cubes pointer */
 for(n=m->cubes.head;n->next!=NULL;n=n->next)
  {
  for(j=0;j<6;j++)
   {
   if(n->d.c->nc[j]!=NULL)
    {
    if((n->d.c->nc[j]=findnode(&m->cubes,n->d.c->nc[j]->no))==NULL)
     {
     if(n->d.c->d[j]!=NULL)
      { if(n->d.c->d[j]->d.d->sd) 
         freenode(&m->sdoors,n->d.c->d[j]->d.d->sd,free);
        freenode(&m->doors,n->d.c->d[j],free); n->d.c->d[j]=NULL; 
        n->d.c->walls[j]->texture2=0; }
     if(n->d.c->walls[j]==NULL)
      {
      n->d.c->walls[j]=insertwall(n,j,-1,-1,-1);
      for(i=0;i<4;i++) n->d.c->walls[j]->corners[i].light=view.illum_minvalue;
      }
     n->d.c->nc[j]=NULL;
     }
    }
   }
  } 
 /* and make the sdoors */
 for(n=m->sdoors.head;n->next!=NULL;n=n->next)
  {
  for(j=0,i=0;j<n->d.sd->num;j++)
   if((n->d.sd->target[i]=findnode(getsdoortype(n->d.sd)==sdtype_door ?
    &m->doors : &m->cubes,n->d.sd->target[j]->no))!=NULL) i++; 
  n->d.sd->num=i;
  }
 sortlist(&m->cubes,0); /* so we get no higher numbers than size */
 sortlist(&m->things,0); sortlist(&m->doors,0); sortlist(&m->pts,0); 
 sortlist(&m->sdoors,0); sortlist(&m->producers,0);
 sortlist(&m->lightsources,0);
 /* making the numbers */
 for(n=m->cubes.head;n->next!=NULL;n=n->next)
  {
  for(j=0;j<8;j++) n->d.c->pts[j]=n->d.c->p[j]->no;
  for(j=0;j<6;j++)
   {
   n->d.c->doors[j]=(n->d.c->d[j]==NULL) ? 0xff : n->d.c->d[j]->no;
   if(n->d.c->d[j]!=NULL)
    { n->d.c->d[j]->d.d->cubenum=n->no;
      n->d.c->d[j]->d.d->wallnum=j; }
   n->d.c->nextcubes[j]=(n->d.c->nc[j]==NULL) ? 0xffff : n->d.c->nc[j]->no;
   } 
  if(n->d.c->cp) 
   { n->d.c->cp->d.cp->cubenum=n->no; n->d.c->prodnum=n->d.c->cp->no; }
  else n->d.c->prodnum=-1;
  }
 for(n=m->sdoors.head;n->next!=NULL;n=n->next)
  {
  n->d.sd->d->d.d->sdoor=n->no;
  for(j=0;j<n->d.sd->num;j++)
   {
   if(getsdoortype(n->d.sd)==sdtype_door)
    { n->d.sd->cubes[j]=n->d.sd->target[j]->d.d->cubenum;
      n->d.sd->walls[j]=n->d.sd->target[j]->d.d->wallnum; }
   else
    n->d.sd->cubes[j]=n->d.sd->target[j]->no; 
   }
  }
 checkmem(m->fullname=MALLOC(strlen(TXT_DEFAULTMACRONAME)+1));
 sprintf(m->fullname,TXT_DEFAULTMACRONAME,view.b_levels->num_options);
 m->filename=NULL; 
 m->exitcube=(m->cubes.size>0) ? m->cubes.head : NULL;
 m->exitwall=view.currwall;
 m->currwall=m->curredge=0;
 initlevel(m);
 /* copy the lightsources */
 for(tn=ld->lightsources.head;tn->next!=NULL;tn=tn->next)
  if(tn->d.ls->cube->d.c->tagged!=NULL)
   copy_lightsource(m,cubes,ld->cubes.size,tn->d.ls,1);
 m->levelsaved=0;
 for(i=0;i<3;i++)
  {
  m->e0.x[i]=view.e0.x[i];
  for(j=0;j<3;j++) m->e[j].x[i]=view.e[j].x[i];
  }
 FREE(cubes);
 return m;
 }
 
/* Insert macro m in current level. Return 1 if successful, -1 if
 macro was inserted but not connected and 0 if not successful */
int insertmacro(struct leveldata *m,int connectnow,float scaling)
 {
 struct point naxis[3],*offset,*eoffset,eaxis[3],hp1,hp2;
 struct listpoint *lp;
 struct thing *t;
 int i,j;
 struct cube *c;
 struct node *n,**cubes;
 struct node *connect_cube = NULL;
 my_assert(m!=NULL && l!=NULL);
 if(view.pcurrcube->d.c->nc[view.currwall]!=NULL)
  { printmsg(TXT_CUBETAGGEDON); return 0; }
 if(view.pcurrwall==NULL) { printmsg(TXT_NOCURRSIDE); return 0; }
 if(m->exitcube==NULL || m->exitcube->d.c->nc[m->exitwall]!=NULL)
  { printmsg(TXT_NOCONNECTSIDE,m->fullname); return 0; }
 if(((l->cubes.size<=MAX_DESCENT_CUBES && 
  l->cubes.size+view.pcurrmacro->cubes.size>MAX_DESCENT_CUBES) || 
  (l->pts.size<=MAX_DESCENT_VERTICES &&
   l->pts.size+view.pcurrmacro->pts.size>MAX_DESCENT_VERTICES) ||
  (l->doors.size<=MAX_DESCENT_WALLS &&
   l->doors.size+view.pcurrmacro->doors.size>MAX_DESCENT_WALLS) ||
  (l->things.size<=MAX_DESCENT_OBJECTS &&
   l->things.size+view.pcurrmacro->things.size>MAX_DESCENT_OBJECTS)) &&
  !yesnomsg(TXT_TOOMANYITEMS))
  return 0;
 untagall(tt_cube); untagall(tt_door); untagall(tt_thing);
 /* the translation */
 offset=view.pcurrcube->d.c->p[wallpts[view.currwall][view.curredge]]->d.p;
 eoffset=m->exitcube->d.c->p[wallpts[m->exitwall][0]]->d.p;
 /* ok: insert macro in plot:
    adding on the current wall, point 0 on current point.
    macros are always saved as this point in the origin and
    the line point 0->point 1 in the connecting wall on the
    x-axis and the part of the line point 0->point 2 which is
    orthogonal to the x-axis as the y-axis.
    the x-axis will then be layed on the line from the current point
    to the lower point and the y-axis on the line to the higher point,
    so that x and y-axis are exchanged...
    everything clear? */
 /* getting the transformation matrix */
 getcubecoords(m->exitcube->d.c,m->exitwall,0,eaxis,0);
 getcubecoords(view.pcurrcube->d.c,view.currwall,view.curredge,naxis,1);
 /* now we have the three basis vectors of the macro-coord-system
    in the normal coord-system that means we have the 
    transformation matrix */
 sortlist(&m->cubes,0); sortlist(&m->pts,0); sortlist(&m->doors,0);
 sortlist(&m->things,0); sortlist(&m->producers,0); sortlist(&m->sdoors,0);
 sortlist(&m->lightsources,0);
 sortlist(&l->cubes,m->cubes.size); sortlist(&l->pts,m->pts.size);
 sortlist(&l->things,m->things.size); sortlist(&l->doors,m->doors.size);
 sortlist(&l->sdoors,m->sdoors.size); 
 sortlist(&l->producers,m->producers.size);
 sortlist(&l->lightsources,m->lightsources.size);
 /* inserting the points */
 for(n=m->pts.head;n->next!=NULL;n=n->next)
  {
  checkmem(lp=MALLOC(sizeof(struct listpoint)));
  /* rotating points */
  for(i=0;i<3;i++) hp1.x[i]=n->d.lp->p.x[i]-eoffset->x[i];
  INVMATRIXMULT(&hp2,eaxis,&hp1);
  MATRIXMULT(&lp->p,naxis,&hp2);
  /* and move&scale them */
  for(i=0;i<3;i++) lp->p.x[i]=offset->x[i]+lp->p.x[i]*scaling;
  fittogrid(&lp->p);
  initlist(&lp->c); lp->tagged=NULL;
  checkmem(addheadnode(&l->pts,n->no,lp));
  }
 /* the things */
 for(n=m->things.head;n->next!=NULL;n=n->next)
  {
  j=getsize(ds_thing,n->d.t)+(unsigned char *)&n->d.t->type1-
   (unsigned char *)n->d.t;
  checkmem(t=MALLOC(j));
  memcpy(t,n->d.t,j); t->tagged=NULL;
  for(i=0;i<3;i++) hp1.x[i]=n->d.t->p[0].x[i]-eoffset->x[i];
  INVMATRIXMULT(&hp2,eaxis,&hp1);
  MATRIXMULT(&t->p[0],naxis,&hp2);
  for(i=0;i<3;i++) t->p[0].x[i]=offset->x[i]+t->p[0].x[i]*scaling;
  /* Now turn the orientation of the thing */
  for(j=0;j<3;j++)
   {
   for(i=0;i<3;i++) hp1.x[i]=n->d.t->orientation[j*3+i];
   INVMATRIXMULT(&hp2,eaxis,&hp1);
   MATRIXMULT(&hp1,naxis,&hp2);
   for(i=0;i<3;i++) n->d.t->orientation[j*3+i]=hp1.x[i];
   }
  setthingpts(t);  t->nc=NULL; setthingcube(t);
  checkmem(addheadnode(&l->things,n->no,t));
  }
 /* inserting the doors, sdoors and producers and flickering lights */
 checkmem(copylist(&l->doors,&m->doors,sizeof(struct door)));
 for(n=m->doors.head;n->next!=NULL;n=n->next) n->d.d->tagged=NULL;
 checkmem(copylist(&l->sdoors,&m->sdoors,sizeof(struct sdoor)));
 checkmem(copylist(&l->producers,&m->producers,sizeof(struct producer)));
 /* now insert all cubes */
 for(n=m->cubes.tail;n->prev!=NULL;n=n->prev)
  {
  checkmem(c=MALLOC(sizeof(struct cube)));
  *c=*n->d.c; c->tagged=NULL;
  for(j=0;j<8;j++) c->pts[j]=c->p[j]->no;
  for(j=0;j<6;j++)
   {
   c->nextcubes[j]=c->nc[j]!=NULL ? c->nc[j]->no : 0xffff;
   c->doors[j]=c->d[j]!=NULL ? c->d[j]->no : 0xff;
   if(c->cp) { c->prodnum=c->cp->no; c->cp->d.cp->cubenum=n->no; }
   else c->prodnum=-1;
   if(n->d.c->walls[j]!=NULL)
    {
    checkmem(c->walls[j]=MALLOC(sizeof(struct wall)));
    *c->walls[j]=*n->d.c->walls[j]; c->walls[j]->ls=NULL;
    }
   }
  checkmem(addheadnode(&l->cubes,n->no,c));
  if(n == m->exitcube)
      connect_cube = l->cubes.head;
  }
 for(n=l->cubes.head;n->no<m->cubes.size;n=n->next)
  if(!initcube(n))
   { n=n->next; freenode(&l->cubes,n->prev,freecube); }
 for(n=l->things.head;n->no<m->things.size;n=n->next)
  setthingcube(n->d.t);
 for(n=l->doors.head;n->no<m->doors.size;n=n->next)
  { freelist(&n->d.d->sdoors,NULL); initlist(&n->d.d->sdoors); }
 for(n=l->doors.head;n->no<m->doors.size;n=n->next)
  {
  n->d.d->cubenum=n->d.d->c->no;
  if(n->d.d->sd)
   {
   n->d.d->sdoor=n->d.d->sd->no;
   for(i=0;i<n->d.d->sd->d.sd->num;i++)
    if(getsdoortype(n->d.d->sd->d.sd)==sdtype_door)
     { n->d.d->sd->d.sd->cubes[i]=n->d.d->sd->d.sd->target[i]->d.d->c->no;
       n->d.d->sd->d.sd->walls[i]=n->d.d->sd->d.sd->target[i]->d.d->wallnum; }
    else n->d.d->sd->d.sd->cubes[i]=n->d.d->sd->d.sd->target[i]->no; 
   }
  else n->d.d->sdoor=0xff;
  if(!initdoor(n))
   { n=n->next; freenode(&l->doors,n->prev,freedoor); }
  }
 /* copy the lightsources */
 checkmem(cubes=MALLOC(sizeof(struct node *)*m->cubes.size));
 for(i=0,n=l->cubes.head;i<m->cubes.size;n=n->next,i++)
  { my_assert(n->next!=NULL); cubes[i]=n; }
 for(n=m->lightsources.head;n->next!=NULL;n=n->next)
  copy_lightsource(l,cubes,m->cubes.size,n->d.ls,0);
 /* now connect the current cube with the cube 0 of the macro */
 if(connectnow)
 {
    my_assert(connect_cube!=NULL);
    if(!connectcubes(NULL,view.pcurrcube,view.currwall,connect_cube,
        m->exitwall))
   return -1;
 }
 return 1;
 } 
 
