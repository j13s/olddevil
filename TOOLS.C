/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    tools.c - all functions which have no better place
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
#include "grfx.h"
#include "plot.h"
#include "tag.h"
#include "insert.h"
#include "tools.h"

char *makepath(const char *path,const char *fname)
 {
 char *lp;
 checkmem(lp=MALLOC(strlen(fname)+strlen(path)+2));
 strcpy(lp,path); strcat(lp,"/"); strcat(lp,fname);
 return lp;
 }

void makesecretstart(struct leveldata *ld,struct node *secretwall,
 struct node *cube)
 {
 int i;
 struct node *n;
 my_assert(secretwall!=NULL);
 my_assert(n=insertthing(cube,NULL));
 n->d.t=changething(n->d.t,NULL,tt1_secretstart,cube->d.c);
 for(i=0;i<9;i++) n->d.t->orientation[i]=ld->secret_orient[i];
 setthingpts(n->d.t); setthingcube(n->d.t); ld->secretstart=n; 
 }
  
/* in t is the old structure (NULL is possible), newtype is the new type1 of
   the thing and c is for the orientation */
struct thing *changething(struct thing *t,struct thing *clone,int newtype,
 struct cube *c)
 {
 struct thing *t2;
 struct point coords[3];
 int j,i,starts[11];
 size_t size=0;
 void *data=NULL;
 struct node *n;
 if(t!=NULL && newtype==t->type1) return t;
 switch(newtype)
  {
  case tt1_robot: size=init.d_ver>=d2_10_sw ? sizeof(struct D2_robot) :
    sizeof(struct D1_robot); 
   data=init.d_ver>=d2_10_sw ? (void *)&D2_stdrobot : (void *)&D1_stdrobot;
   break;
  case tt1_hostage: size=sizeof(struct item); data=&stdhostage; break;
  case tt1_secretstart: /* secret start */
   size=sizeof(struct start); 
   if(l->secretstart) waitmsg(TXT_ONLYONESECRETSTART,l->secretstart->no);
   for(n=l->sdoors.head;n->next!=NULL;n=n->next)
    if(n->d.sd->type==switch_secretexit) break;
   if(n->next==NULL) waitmsg(TXT_NOSECRETDOORFORSTART);
   data=&stdsecretstart; 
   break;
  case tt1_dmstart: case tt1_coopstart: 
   size=sizeof(struct start); 
   data=newtype==tt1_dmstart ? &stdstart : &stdcoopstart; 
   if(init.d_ver>=d2_10_sw && newtype==tt1_coopstart)
    {
    for(n=l->things.head,j=0;n->next!=NULL;n=n->next)
     if(n->d.t->type1==newtype) j++;
    ((struct thing *)data)->type2=0;
    if(j>=4) waitmsg(TXT_TOOMANYSTARTS);
    }
   else
    {
    for(i=0;i<11;i++) starts[i]=0;
    for(n=l->things.head;n->next!=NULL;n=n->next)
     if(n->d.t->type1==newtype && n->d.t->type2<11)
      starts[n->d.t->type2]=1;
    for(i=(newtype==4 ? 0 : 8),j=0;i<(newtype==4 ? 8 : 11) && !j;i++)
     if(starts[i]==0) 
      { j=1; ((struct thing *)data)->type2=newtype==4 ? i : 0;}
    if(!j) waitmsg(TXT_TOOMANYSTARTS);
    }
   break;
  case tt1_mine: size=sizeof(struct mine); data=&stdmine; break;
  case tt1_item: size=sizeof(struct item); data=&stditem; break;
  case tt1_reactor: size=sizeof(struct reactor); data=&stdreactor; break;
  default: fprintf(errf,"Unknown thingtype: %d\n",newtype); my_assert(0);
  }
 if(clone!=NULL) data=clone;
 checkmem(t2=MALLOC(size));
 memcpy(t2,data,size);
 if(t!=NULL) 
  { t2->p[0]=t->p[0]; t2->nc=t->nc; t2->type1=newtype; t2->tagged=t->tagged; }
 else t2->nc=NULL;
 if(t!=NULL) /* orientation */
  for(j=0;j<9;j++) t2->orientation[j]=t->orientation[j];
 else 
  {
  for(j=0;j<3;j++)
   {
   coords[0].x[j]=c->p[wallpts[view.currwall][(view.curredge-1)&3]]->d.p->x[j]
    -c->p[wallpts[view.currwall][view.curredge]]->d.p->x[j];
   coords[2].x[j]=c->p[wallpts[view.currwall][(view.curredge+1)&3]]->d.p->x[j]
    -c->p[wallpts[view.currwall][view.curredge]]->d.p->x[j];
   }
  normalize(&coords[0]);
  for(j=0;j<3;j++)
   coords[2].x[j]-=SCALAR(&coords[0],&coords[2])*coords[0].x[j];
  normalize(&coords[2]);
  VECTOR(&coords[1],&coords[2],&coords[0]);
  for(j=0;j<3;j++)
   for(i=0;i<3;i++)
    t2->orientation[j*3+i]=coords[j].x[i]*65536;
  }
 if(t) 
  {
  if(t->nc)
   { for(n=t->nc->d.c->things.head;n->next!=NULL;n=n->next)
      if(n->d.t==t) break;
     if(n->next) n->d.t=t2; }
  FREE(t);
  }
 setthingpts(t2); setthingcube(t2); 
 return t2;
 }
 
int testvector(struct cube *c,int p0,int p1,int p2,int p3,
 struct point *p)
 { 
 int i;
 struct point econtra,eco[3],h;
 for(i=0;i<3;i++)
  { 
  eco[0].x[i]=c->p[p1]->d.p->x[i]-c->p[p0]->d.p->x[i];
  eco[1].x[i]=c->p[p2]->d.p->x[i]-c->p[p0]->d.p->x[i];
  eco[2].x[i]=c->p[p3]->d.p->x[i]-c->p[p0]->d.p->x[i];
  h.x[i]=p->x[i]-c->p[p0]->d.p->x[i];
  }
 VECTOR(&econtra,&eco[2],&eco[1]);
 if(SCALAR(&econtra,&h)<0.0) return 0;
 VECTOR(&econtra,&eco[0],&eco[2]);
 if(SCALAR(&econtra,&h)<0.0) return 0;
 VECTOR(&econtra,&eco[1],&eco[0]);
 if(SCALAR(&econtra,&h)<0.0) return 0;
 return 1;
 }
 
/* check if pnt p is in cube c */
int checkpntcube(struct node *nc,struct point *p)
 { return (testvector(nc->d.c,0,1,3,4,p) && testvector(nc->d.c,6,2,5,7,p)); }

/* find cube around position p. if there's no cube, return NULL */
struct node *findpntcube(struct list *cl,struct point *p)
 {
 struct node *n;
 /* this is certainly not the fastest method doing this */
 for(n=cl->head;n->next!=NULL;n=n->next)
  if(checkpntcube(n,p)) return n;
 return NULL;
 } 

void makemarker(struct point *mp,struct point *np)
 {
 int j;
 for(j=0;j<8;j++)
  { np[j].x[0]=mp->x[0]; np[j].x[1]=mp->x[1]; np[j].x[2]=mp->x[2]; }
 np[0].x[0]-=view.tsize;np[0].x[1]-=view.tsize;np[0].x[2]-=view.tsize;
 np[1].x[0]+=view.tsize;np[1].x[1]+=view.tsize;np[1].x[2]+=view.tsize;
 np[2].x[0]-=view.tsize;np[2].x[1]+=view.tsize;np[2].x[2]+=view.tsize;
 np[3].x[0]+=view.tsize;np[3].x[1]-=view.tsize;np[3].x[2]-=view.tsize;
 np[4].x[0]+=view.tsize;np[4].x[1]-=view.tsize;np[4].x[2]+=view.tsize;
 np[5].x[0]-=view.tsize;np[5].x[1]+=view.tsize;np[5].x[2]-=view.tsize;
 np[6].x[0]+=view.tsize;np[6].x[1]+=view.tsize;np[6].x[2]-=view.tsize;
 np[7].x[0]-=view.tsize;np[7].x[1]-=view.tsize;np[7].x[2]+=view.tsize;
 }
 
/* make a marker at pos offset with orientation coords (order: right, top
 front) and size size. The coords for the lines are saved in pnts. */ 
void make_o_marker(struct point *offset,struct point *coords,float size,
 struct point *pnts)
 {
 int j;
 for(j=0;j<11;j++) pnts[j]=*offset;
 for(j=0;j<3;j++)
  { pnts[2].x[j]+=coords[2].x[j]*size;
    pnts[3].x[j]+=coords[2].x[j]*size;
    pnts[9].x[j]+=coords[2].x[j]*size; }
 for(j=0;j<3;j++)
  { pnts[6].x[j]+=coords[1].x[j]*size/3;
    pnts[7].x[j]+=coords[1].x[j]*size/3;
    pnts[10].x[j]+=coords[1].x[j]*size/3; }
 for(j=0;j<3;j++)
  { pnts[1].x[j]-=coords[0].x[j]*size/3;
    pnts[8].x[j]-=coords[0].x[j]*size/3;
    pnts[4].x[j]+=coords[0].x[j]*size/3;
    pnts[5].x[j]+=coords[0].x[j]*size/3; }
 }
 
/* set/change the cube for the thing t if necessary. */
void setthingcube(struct thing *t)
 {
 struct node *n;
 int w;
 /* check if the thing is still in the same cube */
 if(t->nc && checkpntcube(t->nc,&t->p[0])) return;
 /* if the thing was in a cube, remove it from the list */
 if(t->nc)
  {
  for(n=t->nc->d.c->things.head;n->next!=NULL;n=n->next)
   if(n->d.t==t) break;
  if(n->next!=NULL)
   {
   unlistnode(&t->nc->d.c->things,n);
   /* check the neighbours */
   for(w=0;w<6;w++)
    if(t->nc->d.c->nc[w] && checkpntcube(t->nc->d.c->nc[w],&t->p[0])) 
     break;
   t->nc=(w<6 ? t->nc->d.c->nc[w] : NULL);
   }
  else t->nc=NULL;
  }
 else n=NULL;
 /* still no cube found? then brute force: Check all cubes */
 if(!t->nc) t->nc=findpntcube(&l->cubes,&t->p[0]);
 /* if the thing is in a new cube add it to the list */
 if(t->nc) 
  if(n) listnode_tail(&t->nc->d.c->things,n);
  else addnode(&t->nc->d.c->things,-1,t);
 }
 
void setthingpts(struct thing *t)
 {
 struct point p,coords[3];
 int i,j;
 float x;
 switch(t->type1)
  {
  case 7: /* item */
   for(j=9;j<11;j++)
    { t->p[j].x[0]=t->p[0].x[0]; t->p[j].x[1]=t->p[0].x[1]; 
      t->p[j].x[2]=t->p[0].x[2]; }
   makemarker(&t->p[0],&t->p[1]);
   break;
  case 3: /* hostage */
   for(j=1;j<11;j++) t->p[j]=t->p[0];
   for(j=0;j<3;j++) p.x[j]=t->orientation[j+3];
   x=t->size/LENGTH(&p);
   for(j=0;j<3;j++)
    { t->p[1].x[j]+=p.x[j]*x*3/4;
      t->p[4].x[j]-=p.x[j]*x;
      t->p[6].x[j]-=p.x[j]*x;
      t->p[7].x[j]+=p.x[j]*x/2;
      t->p[8].x[j]+=p.x[j]*x;
      t->p[9].x[j]+=p.x[j]*x/2;
      t->p[10].x[j]+=p.x[j]*x; }
   for(j=0;j<3;j++) p.x[j]=t->orientation[j];
   x=t->size/LENGTH(&p);
   for(j=0;j<3;j++)
    { t->p[4].x[j]-=p.x[j]*x/3;
      t->p[6].x[j]+=p.x[j]*x/3;
      t->p[8].x[j]-=p.x[j]*x/2;
      t->p[10].x[j]+=p.x[j]*x/2; }
   for(j=0;j<3;j++) p.x[j]=t->orientation[j+6];
   x=t->size/LENGTH(&p);
   for(j=0;j<3;j++)
    { t->p[8].x[j]+=p.x[j]*x/5;
      t->p[10].x[j]+=p.x[j]*x/5; }
   break;
  case 9: /* reactor 
   for(j=1;j<11;j++) t->p[j]=t->p[0];
   for(j=0;j<3;j++) p.x[j]=t->orientation[j+6];
   x=t->size/LENGTH(&p);
   for(j=0;j<3;j++)
    { t->p[1].x[j]-=p.x[j]*x;
      t->p[3].x[j]+=p.x[j]*x;
      t->p[5].x[j]+=p.x[j]*x;
      t->p[7].x[j]-=p.x[j]*x; }
   for(j=0;j<3;j++) p.x[j]=t->orientation[j+3];
   for(j=0;j<3;j++)
    { t->p[1].x[j]-=p.x[j]*x;
      t->p[2].x[j]+=p.x[j]*x;
      t->p[3].x[j]-=p.x[j]*x;
      t->p[4].x[j]+=p.x[j]*x;
      t->p[5].x[j]-=p.x[j]*x;
      t->p[6].x[j]+=p.x[j]*x;
      t->p[7].x[j]-=p.x[j]*x;
      t->p[8].x[j]+=p.x[j]*x;
      t->p[10].x[j]+=p.x[j]*x; }
   for(j=0;j<3;j++) p.x[j]=t->orientation[j];
   for(j=0;j<3;j++)
    { t->p[1].x[j]-=p.x[j]*x;
      t->p[3].x[j]-=p.x[j]*x;
      t->p[5].x[j]+=p.x[j]*x;
      t->p[7].x[j]+=p.x[j]*x; }
   break; */
  default:
   for(i=0;i<3;i++)
    for(j=0;j<3;j++) coords[i].x[j]=t->orientation[i*3+j]/65536.0;
   make_o_marker(t->p,coords,t->size,t->p);
  }
 }
 
/* set the value view.illum_brightness and changes all lightsources for
 the textures in the right manner */
void set_illum_brightness(float dno)
 {
 int j,k;
 for(k=0;k<pig.num_rdltxts;k++)
  for(j=0;j<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE;j++)
   if(pig.rdl_txts[k].my_light[j])
    pig.rdl_txts[k].my_light[j]*=dno/view.illum_brightness;
 view.illum_brightness=dno;
 }

/* turns the coordsystem es to ee.
   i,j give the axis to turn, k is the axis to turn around, i,j,k are
   0,1,2; 1,2,0 or 2,0,1 */
void turn(struct point *es,struct point *ee,int i,int j,int k,float angle)
 {
 struct point ne1,ne2;
 int l;
 for(l=0;l<3;l++)
  {
  ne1.x[l]=cos(angle)*es[i].x[l]+sin(angle)*es[j].x[l];
  ne2.x[l]=-sin(angle)*es[i].x[l]+cos(angle)*es[j].x[l];
  }
 ee[i]=ne1; ee[j]=ne2;
 VECTOR(&ee[k],&ee[i],&ee[j]);
 normalize(&ee[j]);
 normalize(&ee[i]);
 normalize(&ee[k]);
 }
 
/* must be a function because of l (changes in the calculation) 
 prototype in structs.h */
void normalize(struct point *p)
 {
 float l=LENGTH(p);
 if(l!=0.0) { p->x[0]/=l; p->x[1]/=l; p->x[2]/=l; }
 }
 
/* gives the current node corresponding to 'it' */
struct node *getnode(enum datastructs it)
 {
 switch(it)
  {
  case ds_wall: case ds_corner: case ds_producer:
  case ds_cube: case ds_flickeringlight: return view.pcurrcube; break;
  case ds_point: return view.pcurrpnt; break;
  case ds_sdoor:
  case ds_door: return view.pcurrdoor; break;
  case ds_thing: return view.pcurrthing; break;
  case ds_internal: 
  case ds_leveldata: return NULL;
  default:
   fprintf(errf,"getdata: value of info not known: %d\n",it); my_exit(); 
  }
 return NULL;
 }
 
/* gives data for infoitem it. only uses pcurrcube, pcurrdoor, currwall,
 curredge and  pcurrthing (because of changevalue) */
unsigned char *vl_getdata(enum datastructs it,struct node *n,va_list args)
 {
 void *data=NULL;
 int a,b;
 switch(it)
  {
  case ds_cube: 
   if(n==NULL) n=view.pcurrcube;
   if(n!=NULL) data=n->d.c; break;
  case ds_wall:
   if(n==NULL) { n=view.pcurrcube; a=view.currwall; }
   else a=va_arg(args,int);
   if(n!=NULL) data=n->d.c->walls[a]; break;
  case ds_door: 
   if(n==NULL) n=view.pcurrdoor;
   if(n!=NULL) data=n->d.d;
   break;
  case ds_corner: 
   if(n==NULL) { n=view.pcurrcube; a=view.currwall; b=view.curredge; }
   else { a=va_arg(args,int); b=va_arg(args,int); }
   if(n!=NULL && n->d.c->walls[a]!=NULL) data=&n->d.c->walls[a]->corners[b];
   break;
  case ds_thing: 
   if(n==NULL) n=view.pcurrthing;
   if(n!=NULL) data=&n->d.t->type1; break;
  case ds_internal: data=&view; break;
  case ds_leveldata: data=l; break;
  case ds_sdoor:
   if(n==NULL) n=view.pcurrdoor;
   if(n!=NULL && n->d.d->sd!=NULL) data=n->d.d->sd->d.sd;
   break;
  case ds_producer:
   if(n==NULL) n=view.pcurrcube;
   if(n!=NULL && n->d.c->cp!=NULL) data=n->d.c->cp->d.cp; 
   break;
  case ds_point:
   if(n==NULL) n=view.pcurrpnt; 
   if(n!=NULL) data=n->d.p;
   break;
  case ds_flickeringlight:
   if(n==NULL) { n=view.pcurrcube; a=view.currwall; }
   else a=va_arg(args,int);
   if(n!=NULL && n->d.c->walls[a]!=NULL && n->d.c->walls[a]->ls!=NULL &&
    n->d.c->walls[a]->ls->d.ls->fl!=NULL)
    data=n->d.c->walls[a]->ls->d.ls->fl;
   break;
  default:
   fprintf(errf,"getdata: value of info not known: %d (max %d)\n",it,
    ds_number); my_exit(); 
  }
 return (unsigned char *)data;
 }
 
unsigned char *getdata(enum datastructs it,struct node *n,...)
 { 
 unsigned char *r;
 va_list args;
 va_start(args,n);
 r=vl_getdata(it,n,args);
 va_end(args);
 return r;
 }
 
size_t getsize(enum datastructs it,struct thing *t)
 {
 size_t size=0;
 switch(it)
  {
  case ds_producer: size=20; break;
  case ds_sdoor: size=52; break;
  case ds_leveldata: size=sizeof(struct leveldata); break;
  case ds_internal: size=sizeof(struct viewdata); break;
  case ds_cube: size=40; break;
  case ds_wall: size=28; break;
  case ds_door: size=24; break;
  case ds_corner: size=6; break;
  case ds_turnoff: size=6; break;
  case ds_changedlight: size=8; break;
  case ds_flickeringlight: size=16; break;
  case ds_thing: 
   switch(t->type1)
    {
    case tt1_reactor: size=sizeof(struct reactor); break;
    case tt1_hostage: case tt1_item: size=sizeof(struct item); break;
    case tt1_dmstart: case tt1_coopstart: case tt1_secretstart:
     size=sizeof(struct start); break;
    case tt1_robot: size=init.d_ver>=d2_10_sw ? sizeof(struct D2_robot) 
     : sizeof(struct D1_robot); break;
    case tt1_mine: size=sizeof(struct mine); break;
    default: fprintf(errf,"getsize: Unknown thing-type: %.2x\n",
     (unsigned)t->type1); my_exit();
    }
   size-=(unsigned char *)&t->type1-(unsigned char *)t;
   break;
  default:
   fprintf(errf,"getsize: value of info not known: %d\n",it); my_exit(); 
  }
 return size;
 }

int setno(struct infoitem *i,void *no,struct node *n,...)
 {
 int k,ret=1;
 unsigned char *data,*src=no;
 va_list args;
 va_start(args,n);
 if((data=vl_getdata(i->infonr,n,args))!=NULL && i->offset>=0)
  for(k=0;k<i->length;k++)
   *(data+i->offset+k)=*(src+k);
 else ret=0;
 va_end(args);
 return ret;
 }

int getno(struct infoitem *i,void *no,struct node *n,...)
 {
 unsigned char *data,*dest=no;
 int k,e;
 va_list args;
 va_start(args,n);
 if((e=((data=vl_getdata(i->infonr,n,args))!=NULL && i->offset>=0))!=0)
  for(k=0;k<i->length;k++)
   *(dest+k)=*(data+i->offset+k);
 va_end(args);
 return e;
 }
 
enum sdoortypes getsdoortype(struct sdoor *sd)
 {
 switch(sd->type)
  {
  case switch_producer: return sdtype_cube;
  case switch_turnofflight: case switch_turnonlight: return sdtype_side;
  case switch_exit: case switch_secretexit: case switch_nothing:
   return sdtype_none;
  case switch_opendoor: case switch_closedoor: 
  case switch_illusion_on: case switch_illusion_off:
  case switch_unlockdoor: case switch_lockdoor:
  case switch_openwall: case switch_closewall:
  case switch_wall_to_ill:
   return sdtype_door;
  default: fprintf(errf,"Unknown sdoortype: %x\n",sd->type); my_assert(0);
   return 0;
  }
 }

void setsdoortargets(struct sdoor *sd)
 {
 int i;
 switch(getsdoortype(sd))
  {
  case sdtype_none: sd->num=0; break;
  case sdtype_cube: case sdtype_side:
   for(i=0;i<sd->num;i++) sd->cubes[i]=sd->target[i]->no;
   break;
  case sdtype_door:
   for(i=0;i<sd->num;i++)
    { sd->cubes[i]=sd->target[i]->d.d->c->no;
    sd->walls[i]=sd->target[i]->d.d->wallnum; }
   break;
  }
 for(i=sd->num;i<10;i++) { sd->cubes[i]=0xffff; sd->walls[i]=0; }
 }

void freecube(void *n)
 {
 struct cube *c=n;
 int w;
 for(w=0;w<6;w++)
  { if(c->walls[w]) FREE(c->walls[w]); 
    if(c->polygons[w*2]) FREE(c->polygons[w*2]);
    if(c->polygons[w*2+1]) FREE(c->polygons[w*2+1]); }
 freelist(&c->sdoors,NULL);
 FREE(c);
 }

void freedoor(void *n)
 {
 struct door *d=n;
 freelist(&d->sdoors,NULL);
 FREE(d);
 }

void freelistpnt(void *n)
 {
 struct listpoint *lp=n;
 freelist(&lp->c,NULL);
 FREE(lp);
 }

void freelightsource(void *n)
 {
 struct lightsource *ls=n;
 struct node *nlse;
 int w,c;
 long overflow;
 if(ls->fl) delflickeringlight(ls);
 for(nlse=ls->effects.head->next;nlse!=NULL;nlse=nlse->next)
  {
  for(w=0;w<6;w++) if(nlse->prev->d.lse->cube->d.c->walls[w])
   for(c=0;c<4;c++)
    {
    overflow=nlse->prev->d.lse->cube->d.c->walls[w]->corners[c].light-
     nlse->prev->d.lse->add_light[w*4+c];
    nlse->prev->d.lse->cube->d.c->walls[w]->corners[c].light=
     overflow<view.illum_minvalue ? view.illum_minvalue : overflow;
    }
  freenode(&ls->effects,nlse->prev,free);
  }
 FREE(ls);
 }

void freelist(struct list *l,void (*freeentry)(void *))
 {
 struct node *n;
 for(n=l->head->next;n!=NULL;n=n->next)
  freenode(l,n->prev,freeentry);
 l->size=0;
 initlist(l);
 }
 
void freenode(struct list *l,struct node *n,void (*freeentry)(void *))
 {
 unlistnode(l,n);
 if(freeentry!=NULL)
  freeentry(n->d.v);
 FREE(n);
 }

void sortlist(struct list *l,int start)
 {
 struct node *n;
 int i;
 for(n=l->head,i=0;n->next!=NULL;n=n->next,i++) n->no=start+i;
 l->size=i; l->maxnum=i+start;
 }
 
int copylist(struct list *dl,struct list *sl,size_t s)
 {
 struct node *sn;
 void *data;
 for(sn=sl->tail;sn->prev!=NULL;sn=sn->prev)
  {
  if((data=MALLOC(s))==NULL) return 0;
  memcpy(data,sn->d.v,s);
  addheadnode(dl,sn->no,data);
  }
 return 1;
 }

int compstrs(const char *s1,const char *s2)
 {
 char buffer1[256],buffer2[256];
 int i,c;
 strncpy(buffer1,s1,256); buffer1[255]=0;
 for(i=0;i<strlen(buffer1);i++) buffer1[i]=toupper(buffer1[i]);
 strncpy(buffer2,s2,256); buffer2[255]=0;
 for(i=0;i<strlen(buffer2);i++) buffer2[i]=toupper(buffer2[i]);
 if(strlen(buffer1)>strlen(buffer2)) { i=1; buffer1[strlen(buffer2)]=0; }
 else if(strlen(buffer2)>strlen(buffer1)) { i=-1;buffer2[strlen(buffer1)]=0; }
 else i=0;
 return (c=strcoll(buffer1,buffer2))==0 ? i : c; 
 }
 
int qs_compstrs(const void *s1,const void *s2)
 {
 const char *ss1=*(const char **)s1,*ss2=*(const char **)s2;
 return compstrs(ss1,ss2);
 }

int isbinary(int x) { return x=='0' || x=='1'; }
