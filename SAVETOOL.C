/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    savetool.c - saving single things. used for macros & levels.
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

extern int init_test;

struct wall *readwall(FILE *lf,int no);

struct wall *readwall(FILE *lf,int no)
 {
 struct wall *w;
 if((w=malloc(sizeof(struct wall)))==NULL)
  { fprintf(errf,"No mem for walls.\n"); my_exit(); }
#ifdef SHAREWARE
 if(fread(w,getsize(ds_wall,NULL),1,lf)!=1)
  { fprintf(errf,"Can't read wall.\n"); my_exit(); }
#else
 if(fread(&w->texture1,sizeof(short int),1,lf)!=1)
  { fprintf(errf,"Can't read texture1 of wall.\n"); my_exit(); }
 if((w->texture1&0x8000)!=0)
  {
  if(fread(&w->texture2,sizeof(short int),1,lf)!=1)
   { fprintf(errf,"Can't read texture1 of wall.\n"); my_exit(); }
  w->texture1&=0x7fff;
  }
 else
  w->texture2=0;
 if(fread(&w->corners[0],sizeof(struct corner),4,lf)!=4)
  { fprintf(errf,"Can't read corners.\n"); my_exit(); }
 if(init_test&2)
  {
  fprintf(errf,"Read wall: %hx %hx /",w->texture1,w->texture2);
  fprintf(errf," %hx %hx %hx, %hx %hx %hx, %hx %hx %hx, %hx %hx %hx\n",
   w->corners[0].xpos,w->corners[0].ypos,w->corners[0].light,
   w->corners[1].xpos,w->corners[1].ypos,w->corners[1].light,
   w->corners[2].xpos,w->corners[2].ypos,w->corners[2].light,
   w->corners[3].xpos,w->corners[3].ypos,w->corners[3].light);
  }
#endif
 w->no=no;
 return w;
 }

void *readdoor(FILE *f)
 {
 struct door *d;
 if((d=malloc(sizeof(struct door)))==NULL)
  { fprintf(errf,"No mem for door.\n"); my_exit(); }
 if(fread(d,getsize(ds_door,NULL),1,f)!=1)
  { fprintf(errf,"Can't read doors.\n"); my_exit(); }
 initlist(&d->sdoors);
 d->edoor=0;
 if(init_test&2)
  fprintf(errf,"read door: c: %lx w: %lx sdoor: %x\n",d->cubenum,d->wallnum,
   d->sdoor);
 return d;
 }
 
void *readsdoor(FILE *lf)
 {
 struct sdoor *sd;
 if((sd=malloc(sizeof(struct sdoor)))==NULL)
  { fprintf(errf,"No mem for sdoor.\n"); my_exit(); }
 if(fread(sd,getsize(ds_sdoor,NULL),1,lf)!=1)
  { fprintf(errf,"Can't read sdoor.\n"); my_exit(); }
 if(sd->c0500!=0x0500 || sd->cff00!=0xff00 || sd->cffffffff!=0xffffffff)
  fprintf(errf,"SDOOR %hd %hd %hd typ=%lx: constant varies: %hx %hx %lx\n",
   sd->num,sd->cubes[0],sd->walls[0],sd->type,sd->c0500,sd->cff00,
   sd->cffffffff); 
 if(init_test&2)
  fprintf(errf,"read sdoor: type: %lx num: %hd\n",sd->type,sd->num);
 return sd;
 }
 
void *readthing(FILE *lf)
 {
 int j;
 struct thing *t;
 unsigned long size=0;
 if((t=malloc(sizeof(struct thing)))==NULL)
  { fprintf(errf,"No mem for thing.\n"); my_exit(); }
 if(fread(&t->type1,sizeof(struct thing)-((unsigned char *)&t->type1-
  (unsigned char *)t),1,lf)!=1)
  { fprintf(errf,"Can't read thing.\n"); my_exit(); }
 size=getsize(ds_thing,t)+(unsigned char *)&t->type1-(unsigned char *)t;
 if(init_test&2)
  fprintf(errf,"Read thing type/pos %lu: %x %x size: %ld pos: %ld %ld %ld\n",
   ftell(lf),(int)t->type1,(int)t->type2,size,t->pos[0],t->pos[1],t->pos[2]);
 switch(t->type1)
  {
  case 0xe: t->color=1; break;
  case 9: t->color=3; break;
  case 3: t->color=2; break;
  case 7: t->color=2; break;
  case 4: t->color=1; break;
  case 2: t->color=0; break;
  default: fprintf(errf,"Unknown thing-type: %.2x\n",(unsigned)t->type1);
   my_exit();
  }
 if((t=realloc(t,size))==NULL)
  { fprintf(errf,"No mem for item.\n"); my_exit(); }   
 if(fread((unsigned char *)t+sizeof(struct thing),
  size-sizeof(struct thing),1,lf)!=1)
  { fprintf(errf,"Can't read stdata.\n"); my_exit(); }
 for(j=0;j<3;j++)
  t->p[0].x[j]=t->pos[j];
 setthingpts(t);
 if(t->type1==4 && t->type2==0) /* starting place */
  {
  for(j=0;j<3;j++)
   {
   view.e0.x[j]=t->p[0].x[j]; 
   view.e[0].x[j]=((struct start *)t)->orientation[j];
   view.e[1].x[j]=((struct start *)t)->orientation[j+3];
   }
  normalize(&view.e[0]); normalize(&view.e[1]);
  VECTOR(&view.e[2],&view.e[0],&view.e[1]);
  }
 return t;
 }
 
struct edoor *readedoors(FILE *f,unsigned long num,unsigned long size)
 {
 struct edoor *data;
 if(num!=1) 
  { fprintf(errf,"Only one edoor struct allowed."); my_exit(); }
 if(size!=sizeof(struct edoor))
  { fprintf(errf,"Illegal size for edoor."); my_exit(); }
 if((data=malloc(sizeof(struct edoor)))==NULL)
  { fprintf(errf,"No mem for data in edoor.\n");return NULL; }
 if(fread(data,sizeof(struct edoor),1,f)!=1)
  { fprintf(errf,"Can't read data in edoor.\n");
    free(data); return NULL; }
 return data;
 }
 
void *readpnt(FILE *lf)
 {
 long int coords[3];
 struct listpoint *p;
 if(fread(coords,sizeof(long int),3,lf)!=3)
  { fprintf(errf,"Can't read point.\n"); my_exit(); }
 if((p=malloc(sizeof(struct listpoint)))==NULL)
  { fprintf(errf,"No mem for point.\n"); my_exit(); }
 p->p.x[0]=coords[0]; p->p.x[1]=coords[1]; p->p.x[2]=coords[2];
 initlist(&p->c);
 return p;
 }
 
void *readproducer(FILE *f)
 {
 struct producer *p;
 if((p=malloc(sizeof(struct producer)))==NULL)
  { fprintf(errf,"No mem for producer.\n"); return NULL; }
 if(fread(p,getsize(ds_producer,NULL),1,f)!=1)
  { fprintf(errf,"Can't read producer.\n"); free(p); return NULL; }
 if(p->c01f40000!=0x1f40000 || p->c00050000!=0x50000)
  fprintf(errf,"Producer cube %hd: Constant varies: %lx %lx\n",p->cubenum,
   p->c01f40000,p->c00050000);
 if(init_test&2)
  fprintf(errf,"read producer: cube: %hx what: %lx\n",p->cubenum,p->prodwhat);
 return p;
 }
 
void *readcube(FILE *lf)
 {
 struct cube *c;
 int j;
#ifndef SHAREWARE
 unsigned char controlbyte;
#endif
 if((c=malloc(sizeof(struct cube)))==NULL)
  { fprintf(errf,"No mem for cube.\n"); my_exit(); }
#ifdef SHAREWARE
 if(fread(c,getsize(ds_cube,NULL),1,lf)!=1)
  { fprintf(errf,"Can't read cube.\n"); my_exit(); }
#else
 if(fread(&controlbyte,sizeof(unsigned char),1,lf)!=1)
  { fprintf(errf,"Can't read cube control byte.\n"); my_exit(); }
 if(init_test&2)
  fprintf(errf,"Read cube: %x ",(int)controlbyte);
 for(j=0;j<6;j++)
  {
  if((controlbyte&(1<<j))!=0)
   {
   if(fread(&c->nextcubes[j],sizeof(unsigned short int),1,lf)!=1)
    { fprintf(errf,"Can't read nextcubes.\n"); my_exit(); }
   }
  else
   c->nextcubes[j]=0xffff;
  }
 if(fread(&c->pts[0],sizeof(short int),11,lf)!=11)
  { fprintf(errf,"Can't read cube points or cube type.\n"); my_exit(); }
 if(fread(&controlbyte,sizeof(unsigned char),1,lf)!=1)
  { fprintf(errf,"Can't read cube doors control byte.\n"); my_exit(); }
 if(init_test&2)
  fprintf(errf,"%hx %hx %x %hd %hd %hd %hd %hd %hd %hd %hd\n",c->type,
   c->prodnum,(int)controlbyte,c->pts[0],c->pts[1],c->pts[2],c->pts[3],
   c->pts[4],c->pts[5],c->pts[6],c->pts[7]);
 for(j=0;j<6;j++)
  {
  if((controlbyte&(1<<j))!=0)
   {
   if(fread(&c->doors[j],sizeof(unsigned char),1,lf)!=1)
    { fprintf(errf,"Can't read cube doors.\n"); my_exit(); }
   }
  else 
   c->doors[j]=0xff;
  }
#endif
 for(j=0;j<6;j++)
  c->walls[j]=(c->nextcubes[j]==0xffff || c->doors[j]!=0xff) ?
   c->walls[j]=readwall(lf,j) : NULL;
 return c;
 }
 
int readlist(FILE *f,struct list *l,void *(*readdata)(FILE *),int num)
 {
 int i,n;
 if(num<0)
  { if(fread(&n,sizeof(int),1,f)!=1)
     { fprintf(errf,"Can't read size of list."); return 0; } }
 else n=num;
 for(i=0;i<n;i++)
  if(!addnode(l,i,readdata(f)))
   { fprintf(errf,"Can't add to list.\n"); return 0; }
 return 1;
 }
 
int savepoint(FILE *f,struct node *n)
 {
 struct point *p=n->d.p;
 long coords[3];
 coords[0]=p->x[0]; coords[1]=p->x[1]; coords[2]=p->x[2];
 if(fwrite(coords,sizeof(long int),3,f)!=3)
  { fprintf(errf,"Savelvl: Can't write point\n"); return 0; }
 return 1;
 }

int savecube(FILE *f,struct node *n)
 {
 struct cube *c=n->d.c;
 int j;
#ifndef SHAREWARE
 short int t1;
 char *offset;
 int size;
 unsigned char cbnext=0x40,cbdoor=0;
 for(j=0;j<6;j++)
  {
  if(c->nc[j]!=NULL) cbnext|=1<<j;
  if(c->d[j]!=NULL) cbdoor|=1<<j;
  }
 if(view.exitcube!=NULL && n->no==view.exitcube->no) cbnext|=1<<view.exitwall;
 if(fwrite(&cbnext,1,1,f)!=1)
  { printmsg("Can't write cube controlbyte."); return 0; }
#endif
 for(j=0;j<8;j++)
  c->pts[j]=c->p[j]->no;
 for(j=0;j<6;j++)
  {
  c->doors[j]=(c->d[j]!=NULL) ? c->d[j]->no : 0xff;
  if(view.exitcube!=NULL && n->no==view.exitcube->no && j==view.exitwall)
   c->nextcubes[j]=0xfffe;
  else
   c->nextcubes[j]=(c->nc[j]!=NULL) ? c->nc[j]->no : 0xffff;
  }
#ifdef SHAREWARE
 if(fwrite(c,getsize(ds_cube,NULL),1,f)!=1)
  { fprintf(errf,"Savelvl: Can't write cube.\n"); return 0; }
#else
 for(j=0;j<6;j++)
  if(c->nextcubes[j]!=0xffff)
   if(fwrite(&c->nextcubes[j],sizeof(short int),1,f)!=1)
    { printmsg("Can't write nextcubes."); return 0; }
 c->prodnum=c->cp->no;
 if(fwrite(&c->pts[0],sizeof(short int),11,f)!=11)
  { printmsg("Can't write cube data."); return 0; }
 if(fwrite(&cbdoor,1,1,f)!=1)
  { printmsg("Can't write door control byte."); return 0; }
 for(j=0;j<6;j++)
  if(c->doors[j]!=0xff)
   if(fwrite(&c->doors[j],sizeof(char),1,f)!=1)
    { printmsg("Can't write cube door information."); return 0; }
#endif
 for(j=0;j<6;j++)
  {
  if(c->nextcubes[j]==0xffff || c->doors[j]!=0xff)
#ifdef SHAREWARE
   if(fwrite(c->walls[j],getsize(ds_wall,NULL),1,f)!=1)
    { printmsg("Savelvl: Can't write wall."); return 0; }
#else
   {
   t1=c->walls[j]->texture1;
   size=getsize(ds_wall,NULL)-2;
   offset=(char *)c->walls[j]+2;
   if(c->walls[j]->texture2!=0) t1|=0x8000; 
   else { size-=2; offset+=2; }
   if(fwrite(&t1,sizeof(short int),1,f)!=1)
    { printmsg("Can't write texture1 for wall."); return 0; }
   if(fwrite(offset,size,1,f)!=1)
    { printmsg("Can't save wall."); return 0; }
   }
#endif
  }
 return 1;
 }
 
int savething(FILE *f,struct node *n)
 {
 struct thing *t=n->d.t;
 int i;
 for(i=0;i<3;i++) t->pos[i]=t->p[0].x[i];
 switch(t->type1)
  {
  case 2: t->stuff[0]=0x0101; t->stuff[1]=1; break;
  case 3: t->stuff[0]=0x000d; t->stuff[1]=4; break;
  case 4: t->stuff[0]=(t->type2==0) ? 0x0105 : 0x0100; t->stuff[1]=1; break;
  case 7: t->stuff[0]=0x000d; t->stuff[1]=5; break;
  case 9: t->stuff[0]=0x0010; t->stuff[1]=1; break;
  case 0xe: t->stuff[0]=0x0100; t->stuff[1]=1; break;
  default: t->stuff[0]=0; t->stuff[1]=0;
  }
 if(fwrite(&t->type1,getsize(ds_thing,t),1,f)!=1)
  { fprintf(errf,"Can't write thing.\n"); return 0; }
 return 1;
 }

int savedoor(FILE *f,struct node *n)
 {
 struct door *d=n->d.d;
 d->cubenum=d->c->no; 
 if(fwrite(d,getsize(ds_door,NULL),1,f)!=1)
  { fprintf(errf,"Can't write door.\n"); return 0; }
 return 1;
 }

int saveproducer(FILE *f,struct node *pr)
 {
 pr->d.cp->stuffnum=pr->d.cp->c->d.c->subtype;
 if(fwrite(pr->d.cp,getsize(ds_producer,NULL),1,f)!=1)
  { fprintf(errf,"Can't write producer.\n"); return 0; }
 return 1;
 }
 
int savesdoor(FILE *f,struct node *sd)
 {
 if(fwrite(sd->d.sd,getsize(ds_sdoor,NULL),1,f)!=1)
  { fprintf(errf,"Can't write sdoor.\n"); return 0; }
 return 1;
 }
 
int savelist(FILE *f,struct list *l,int (*saveproc)(FILE *,struct node *),
 int withnum)
 {
 struct node *n;
 if(withnum)
  if(fwrite(&l->size,sizeof(int),1,f)!=1)
   { printmsg("Can't save size of list."); return 0; }
 for(n=l->head;n->next!=NULL;n=n->next)
  if(!saveproc(f,n)) { printmsg("Can't save list."); return 0; }
 return 1;
 }

 
