/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    macros.c - build, save and insert macros.
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
#include "savetool.h"
#include "insert.h"
#include "event.h"
#include "macros.h"

/* if m is a coordsystem in MATRIXMULT the vectors are in the columns */
#define MATRIXMULT(e,m,v) { int i;\
 for(i=0;i<3;i++) \
  ((e)->x[i])=((m)[0].x[i]*(v)->x[0]+(m)[1].x[i]*(v)->x[1]+ \
   (m)[2].x[i]*(v)->x[2]); }
/* and in INVMATRIXMULT in the rows */
#define INVMATRIXMULT(e,m,v) { int i;\
 for(i=0;i<3;i++) \
  ((e)->x[i])=((m)[i].x[0]*(v)->x[0]+(m)[i].x[1]*(v)->x[1]+ \
   (m)[i].x[2]*(v)->x[2]); }
   
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
 double f;
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
 VECTOR(&naxis[2],&naxis[1],&naxis[0])
 }
 
int readdescription(char **ltxt)
 {
 if((*ltxt=malloc(strlen("BLABLABLA")+1))==NULL)
  return 0;
 strcpy(*ltxt,"BLABLABLA");
 return 1;
 }
 
/* copies all in l tagged cubes with doors and all in tl
   tagged things to a macro */
struct macro *buildmacro(struct list *l,struct list *tl)
 {
 struct node *n,*tn,*sn,*nn;
 struct macro *m;
 struct cube *c;
 struct producer *cp;
 struct listpoint *lp;
 struct door *d;
 struct sdoor *sd;
 struct thing *t;
 int i,j;
 struct point coords[3],*offset,p;
 if((m=malloc(sizeof(struct macro)))==NULL)
  { printmsg("No mem for macro."); return 0; }
 initlist(&m->cubes); initlist(&m->pts); initlist(&m->doors);
 initlist(&m->things); initlist(&m->sdoors); initlist(&m->producers);
 m->wallno=view.currwall;
 /* make coordsystem */
 offset=view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
 getcubecoords(view.pcurrcube->d.c,view.currwall,view.currpnt,coords,0);
 /* first make lists */
 for(tn=l->head;tn->next!=NULL;tn=tn->next)
  {
  n=tn->d.n;
  if((c=calloc(sizeof(struct cube),1))==NULL)
   { printmsg("No mem for cube."); freemacro(m); return NULL; }
  nn=addnode(&m->cubes,n->no,c);
  *c=*n->d.c;
  if(c->cp!=NULL)
   {
   if((cp=malloc(sizeof(struct producer)))==NULL)
    { printmsg("No mem for producer."); return NULL; }
   *cp=*c->cp->d.cp;
   if((sn=addnode(&m->producers,c->cp->no,cp))==NULL)
    { printmsg("Can't add node for producer."); return NULL; }
   c->cp=sn;
   cp->c=nn;
   }
  for(j=0;j<8;j++)
   {
   if((sn=findnode(&m->pts,n->d.c->p[j]->no))==NULL)
    {
    if((lp=malloc(sizeof(struct listpoint)))==NULL)
     { printmsg("No mem for point."); freemacro(m); return NULL; }
    for(i=0;i<3;i++) p.x[i]=n->d.c->p[j]->d.lp->p.x[i]-offset->x[i];
    INVMATRIXMULT(&lp->p,coords,&p);
    initlist(&lp->c);
    if((sn=addnode(&m->pts,n->d.c->p[j]->no,lp))==NULL)
     { printmsg("Can't create point node."); freemacro(m); return NULL; }
    }
   else
    lp=sn->d.lp;
   c->p[j]=sn;
   addnode(&lp->c,j,nn);
   }
  for(j=0;j<6;j++)
   {
   if(n->d.c->walls[j]!=NULL)
    {
    if((c->walls[j]=malloc(sizeof(struct wall)))==NULL)
     { printmsg("No mem for wall."); freemacro(m); return NULL; } 
    *c->walls[j]=*n->d.c->walls[j];
    for(i=0;i<4;i++)
     c->walls[j]->p[i]=c->p[wallpts[j][i]];
    }
   if(n->d.c->d[j]!=NULL)
    {
    if((d=malloc(sizeof(struct door)))==NULL)
     { printmsg("No mem for door."); freemacro(m); return NULL; }
    *d=*n->d.c->d[j]->d.d;
    d->c=nn; d->w=c->walls[j];
    if((sn=addnode(&m->doors,n->d.c->d[j]->no,d))==NULL)
     { printmsg("Can't create door node."); freemacro(m); return NULL; }
    c->d[j]=sn;
    if(d->sd!=NULL)
     {
     if((sd=malloc(sizeof(struct sdoor)))==NULL)
      { printmsg("No mem for switch."); freemacro(m); return NULL; }
     *sd=*d->sd->d.sd;
     if((d->sd=addnode(&m->sdoors,d->sd->no,sd))==NULL)
      { printmsg("Can't add node for switch."); freemacro(m); return NULL; }
     sd->d=sn;
     }
    }
   }
  }
 /* copy the things */
 for(tn=tl->head;tn->next!=NULL;tn=tn->next)
  {
  j=getsize(ds_thing,tn->d.n->d.t)+(unsigned char *)&tn->d.n->d.t->type1-
   (unsigned char *)tn->d.n->d.t;
  if((t=malloc(j))==NULL)
   { printmsg("No mem for thing."); return 0; }
  memcpy(t,tn->d.n->d.t,j);
  for(i=0;i<3;i++) p.x[i]=t->p[0].x[i]-offset->x[i];
  INVMATRIXMULT(&t->p[0],coords,&p);
  if(addheadnode(&m->things,tn->d.n->no,t)==NULL)
   { printmsg("Can't add thing."); return 0; }
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
      { freenode(&m->doors,n->d.c->d[j],free); n->d.c->d[j]=NULL; 
        n->d.c->walls[j]->texture2=0; }
     if(n->d.c->walls[j]==NULL) n->d.c->walls[j]=insertwall(n,j,0);
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
 /* making the numbers */
 for(n=m->cubes.head;n->next!=NULL;n=n->next)
  {
  for(j=0;j<8;j++) n->d.c->pts[j]=n->d.c->p[j]->no;
  for(j=0;j<6;j++)
   {
   n->d.c->doors[j]=(n->d.c->d[j]==NULL) ? 0xff : n->d.c->d[j]->no;
   if(n->d.c->d[j]!=NULL)
    {
    n->d.c->d[j]->d.d->cubenum=n->no;
    n->d.c->d[j]->d.d->wallnum=j;
    }
   n->d.c->nextcubes[j]=(n->d.c->nc[j]==NULL) ? 0xffff : n->d.c->nc[j]->no;
   } 
  if(n->d.c->cp) 
   { n->d.c->cp->d.cp->cubenum=n->no; n->d.c->prodnum=n->d.c->cp->no; }
  else n->d.c->prodnum=0xff;
  }
 for(n=m->sdoors.head;n->next!=NULL;n=n->next)
  for(j=0;j<n->d.sd->num;j++)
   {
   n->d.sd->d->d.d->sdoor=n->no;
   if(getsdoortype(n->d.sd)==sdtype_door)
    { n->d.sd->cubes[j]=n->d.sd->target[j]->d.d->cubenum;
      n->d.sd->walls[j]=n->d.sd->target[j]->d.d->wallnum; }
   else
    { n->d.sd->cubes[j]=n->d.sd->target[j]->no;
      n->d.sd->walls[j]=0; }
   }
 m->shorttxt=m->longtxt=NULL;
 m->groupno=-1; /* not saved yet */
 m->filename=NULL; /* no corresponding file */
 return m;
 }

int savemacro(FILE *f,struct macro *m)
 {
 struct node *exit;
 int ok;
 /* numerate the list new: */
 if(fwrite("MACRO",1,5,f)!=5)
  { printmsg("Can't save header."); return 0; }
 if(fwrite(&m->groupno,sizeof(int),1,f)!=1)
  { printmsg("Can't save macro group number."); return 0; }
 if(!fwritestring(f,m->shorttxt))
  { printmsg("Can't save short string."); return 0; }
 if(fwrite(&m->wallno,sizeof(int),1,f)!=1)
  { printmsg("Can't save macro wall number."); return 0; }
 if(!fwritestring(f,m->longtxt))
  { printmsg("Can't save short string."); return 0; }
 exit=view.exitcube; view.exitcube=NULL;
 ok=savelist(f,&m->pts,savepoint,1) && 
  savelist(f,&m->cubes,savecube,1) && savelist(f,&m->doors,savedoor,1) && 
  savelist(f,&m->things,savething,1) && savelist(f,&m->sdoors,savesdoor,1) &&
  savelist(f,&m->producers,saveproducer,1);
 view.exitcube=exit;
 return ok;
 }

int readmacro(struct macro *m)
 {
 FILE *f;
 int ok;
 char buffer[strlen(view.macropath)+strlen(m->filename)+1];
 strcpy(buffer,view.macropath); strcat(buffer,m->filename);
 if((f=fopen(buffer,"rb"))==NULL)
  { printmsg("Can't open macro file %s.",buffer); return 0; }
 free(m->shorttxt);
 if(!readmacroheader(f,m))
  { fclose(f); printmsg("Can't read macroheader."); return 0; }
 if(fread(&m->wallno,sizeof(int),1,f)!=1)
  { fprintf(errf,"Can't read wall number in macrofile %s\n",buffer); 
    fclose(f); return 0; }
 if((m->longtxt=freadstring(f))==NULL)
  { fprintf(errf,"Can't read longtxt for macro %s\n",buffer);return 0; }
 ok=readlist(f,&m->pts,readpnt,-1) && readlist(f,&m->cubes,readcube,-1) &&
  readlist(f,&m->doors,readdoor,-1) && readlist(f,&m->things,readthing,-1) &&
  readlist(f,&m->sdoors,readsdoor,-1) && 
  readlist(f,&m->producers,readproducer,-1);
 fclose(f);
 if(!ok)
  printmsg("Can't read macro data.");
 return ok;
 }
   
int insertmacro(struct macro *m,int connectnow)
 {
 struct point naxis[3],*offset;
 struct listpoint *lp;
 struct thing *t;
 int i,j;
 struct cube *c;
 struct node *n;
 /* the translation */
 offset=view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
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
 getcubecoords(view.pcurrcube->d.c,view.currwall,view.currpnt,naxis,1);
 /* now we have the three basis vectors of the macro-coord-system
    in the normal coord-system that means we have the 
    transformation matrix */
 sortlist(&l->cubes,m->cubes.size); sortlist(&l->pts,m->pts.size);
 sortlist(&l->things,m->things.size); sortlist(&l->doors,m->doors.size);
 /* inserting the points */
 for(n=m->pts.head;n->next!=NULL;n=n->next)
  {
  if((lp=malloc(sizeof(struct listpoint)))==NULL)
   { printmsg("No mem to create point."); return 0; }
  /* rotating points */
  MATRIXMULT(&lp->p,naxis,&n->d.lp->p);
  /* and move them */
  for(i=0;i<3;i++) lp->p.x[i]+=offset->x[i];
  fittogrid(&lp->p);
  initlist(&lp->c);
  if(!addheadnode(&l->pts,n->no,lp))
   { printmsg("Can't add point.\n"); return 0; } 
  }
 /* the things */
 for(n=m->things.head;n->next!=NULL;n=n->next)
  {
  j=getsize(ds_thing,n->d.t)+(unsigned char *)&n->d.t->type1-
   (unsigned char *)n->d.t;
  if((t=malloc(j))==NULL)
   { printmsg("No mem for thing."); return 0; }
  memcpy(t,n->d.t,j);
  MATRIXMULT(&t->p[0],naxis,&n->d.t->p[0]);
  for(i=0;i<3;i++) t->p[0].x[i]+=offset->x[i];
  setthingpts(t);
  if(addheadnode(&l->things,n->no,t)==NULL)
   { printmsg("Can't add thing."); return 0; }
  }
 /* inserting the doors, sdoors and producers */
 if(!copylist(&l->doors,&m->doors,sizeof(struct door)))
  { printmsg("Can't copy doors."); return 0; }
 if(!copylist(&l->sdoors,&m->sdoors,sizeof(struct sdoor)))
  { printmsg("Can't copy switches."); return 0; }
 if(!copylist(&l->producers,&m->producers,sizeof(struct producer)))
  { printmsg("Can't copy producers."); return 0; }
 /* now insert all cubes */
 for(n=m->cubes.tail;n->prev!=NULL;n=n->prev)
  {
  if((c=malloc(sizeof(struct cube)))==NULL)
   { printmsg("No mem for inserting cube.\n"); return 0; }
  *c=*n->d.c;
  for(j=0;j<6;j++)
   {
   if(n->d.c->walls[j]!=NULL)
    {
    if((c->walls[j]=malloc(sizeof(struct wall)))==NULL)
     { printmsg("No mem for new walls."); return 0; }
    *c->walls[j]=*n->d.c->walls[j];
    }
   }
  addheadnode(&l->cubes,n->no,c);
  }
 for(n=l->cubes.head;n->no<m->cubes.size;n=n->next)
  initcube(n);
 for(n=l->doors.head;n->no<m->doors.size;n=n->next)
  initlist(&n->d.d->sdoors);
 for(n=l->doors.head;n->no<m->doors.size;n=n->next)
  initdoor(n);
 /* now connect the current cube with the cube 0 of the macro */
 if(connectnow)
  if(!connect(l->cubes.head,m->wallno,view.pcurrcube,view.currwall))
   return 0; 
 return 1;
 } 
 
