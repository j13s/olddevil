/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    savetool.c - saving single things. used for macros & levels.
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

extern int init_test;
extern enum descent loading_level_version;

struct wall *readwall(FILE *lf,int no)
 {
 struct wall *w;
 checkmem(w=MALLOC(sizeof(struct wall)));
 if(fread(&w->texture1,sizeof(short int),1,lf)!=1) { FREE(w); return NULL; }
 if((w->texture1&0x8000)!=0)
  {
  if(fread(&w->texture2,sizeof(short int),1,lf)!=1) { FREE(w); return NULL; }
  w->txt2_direction=(w->texture2>>14)&3;
  w->texture2&=0x3fff;
  }
 else
  { w->texture2=0; w->txt2_direction=0; }
 w->texture1&=0x3fff;
 if(fread(&w->corners[0],sizeof(struct corner),4,lf)!=4)
  { FREE(w); return NULL; }
 w->locked=0; w->ls=NULL; 
 if(init_test&2)
  {
  fprintf(errf,"Read wall: %hx %hx /",w->texture1,w->texture2);
  fprintf(errf," %hx %hx %hx, %hx %hx %hx, %hx %hx %hx, %hx %hx %hx\n",
   w->corners[0].x[0],w->corners[0].x[1],w->corners[0].light,
   w->corners[1].x[0],w->corners[1].x[1],w->corners[1].light,
   w->corners[2].x[0],w->corners[2].x[1],w->corners[2].light,
   w->corners[3].x[0],w->corners[3].x[1],w->corners[3].light);
  }
 w->no=no;
 return w;
 }

void *readdoor(FILE *f)
 {
 struct door *d;
 checkmem(d=MALLOC(sizeof(struct door)));
 if(fread(d,getsize(ds_door,NULL),1,f)!=1) { FREE(d); return NULL; }
 initlist(&d->sdoors);
 d->edoor=0;
 d->tagged=NULL; d->w=NULL; d->c=NULL; d->sd=NULL; d->d=NULL;
 if(init_test&2)
  {
  fprintf(errf,
   "read door: t1: %d t2: %d state %d key %d stuff %d anim: %d\n",
    d->type1,d->type2,d->state,d->key,d->stuff,d->animtxt);
  fprintf(errf,"         c: %lx w: %lx sd: %x\n",
   d->cubenum,d->wallnum,d->sdoor);
  if(d->linked_wall!=-1)
   fprintf(errf,"read door: linked_wall: %lx\n",d->linked_wall);
  }
 return d;
 }
 
struct D1_sdoor
 {
 char type_0 NONANSI_FLAG; /* always 0 */
 unsigned short flags NONANSI_FLAG;
 long value NONANSI_FLAG; /* 0x00050000 */
 long time NONANSI_FLAG; /* -1 */
 char link_num NONANSI_FLAG; /* -1 */
 unsigned short int num  NONANSI_FLAG;
 unsigned short int cubes[10] NONANSI_FLAG;
 unsigned short int walls[10] NONANSI_FLAG;
 };
void *D1_REG_readsdoor(FILE *lf)
 {
 struct D1_sdoor D1_sd;
 struct sdoor *sd;
 int i;
 checkmem(sd=MALLOC(sizeof(struct sdoor)));
 if(fread(&D1_sd,sizeof(struct D1_sdoor),1,lf)!=1) { FREE(sd); return NULL; } 
 sd->d=NULL;
 for(i=0;i<10;i++) sd->target[i]=NULL;
 sd->value=D1_sd.value;
 sd->link_num=D1_sd.link_num;
 sd->num=D1_sd.num; sd->flags=0;
 for(i=0;i<10;i++) 
  { sd->cubes[i]=D1_sd.cubes[i]; sd->walls[i]=D1_sd.walls[i]; }
 switch(D1_sd.flags & 0x149)
  {
  case 0x00: sd->type=switch_nothing; break;
  case 0x01: sd->type=switch_opendoor; break;
  case 0x40: sd->type=switch_producer; break;
  case 0x08: sd->type=switch_exit; break;
  case 0x100: sd->type=switch_secretexit; break;
  default: fprintf(errf,"Unknown sdoor %lx: %x\n",ftell(lf),D1_sd.flags); 
   my_assert(0);
  }
 if(init_test&2)
  {
  fprintf(errf,"read sdoor: type: %hx flags: %hx num: %hd v: %lx l: %lx\n",
   sd->type,sd->flags,sd->num,sd->value,sd->link_num); 
  for(i=0;i<10;i++) fprintf(errf," %hd,%hd",sd->cubes[i],sd->walls[i]);
  fprintf(errf,"\n");
  }
 return sd;
 }
 
void *D2_REG_readsdoor(FILE *lf)
 {
 struct sdoor *sd;
 int i;
 checkmem(sd=MALLOC(sizeof(struct sdoor)));
 if(fread(sd,getsize(ds_sdoor,NULL),1,lf)!=1) { FREE(sd); return NULL; } 
 sd->d=NULL;
 for(i=0;i<10;i++) sd->target[i]=NULL;
 if(init_test&2)
  {
  fprintf(errf,"read sdoor: type: %hx flags: %hx num: %hd v: %lx l: %lx\n",
   sd->type,sd->flags,sd->num,sd->value,sd->link_num); 
  for(i=0;i<10;i++) fprintf(errf," %hd,%hd",sd->cubes[i],sd->walls[i]);
  fprintf(errf,"\n");
  }
 return sd;
 }
 
void *readthing(FILE *lf)
 {
 int i,j,special=0;
 struct thing *t;
 unsigned long size=0;
 struct point p;
 struct D1_robot D1_r;
 struct D2_robot *D2_r;
 if(init_test&2)
  fprintf(errf,"Read thing type/pos %lx:",ftell(lf));
 checkmem(t=MALLOC(sizeof(struct thing)));
 if(fread(&t->type1,sizeof(struct thing)-((unsigned char *)&t->type1-
  (unsigned char *)t),1,lf)!=1) { FREE(t); return NULL; }
 size=getsize(ds_thing,t)+(unsigned char *)&t->type1-(unsigned char *)t;
 if(init_test&2)
  fprintf(errf,"%x %x size: %ld pos: %ld %ld %ld cube: %hd\n",
   (int)t->type1,(int)t->type2,(long)getsize(ds_thing,t),t->pos[0],
    t->pos[1],t->pos[2],t->cube);
 switch(t->type1)
  {
  case tt1_coopstart: t->color=1; if(t->size==0) t->size=stdcoopstart.t.size;
   if(t->movement==mt_none) special=1;
   break;
  case tt1_reactor: t->color=3; if(t->size==0) t->size=stdreactor.t.size; 
   break;
  case tt1_hostage: t->color=2; if(t->size==0) t->size=stdhostage.t.size; 
   break;
  case tt1_item: t->color=2; if(t->size==0) t->size=view.tsize; 
   break;
  case tt1_dmstart: t->color=1; if(t->size==0) t->size=stdstart.t.size; 
   if(t->movement==mt_none) special=1;
   break;
  case tt1_mine: t->color=0; if(t->size==0) t->size=view.tsize; break;
  case tt1_robot: t->color=0;
   if(t->size==0) t->size=(t->type2<NUM_ROBOTS) ? robotsize[t->type2] : 
    D2_stdrobot.t.size; 
   if(t->movement==mt_none) special=2;
   if(init.d_ver>=d2_10_sw && loading_level_version<d2_10_sw)
    special=3;
   break;
  default: waitmsg("Unknown thing-type: %.2x.",(unsigned)t->type1); 
   t->color=2; if(t->size==0) t->size=view.tsize;
   switch(t->control)
    {
    case ct_mine: size+=sizeof(struct control_mine); break;
    case ct_ai: size+=init.d_ver<d2_10_sw ? 
     sizeof(struct D1_control_ai) : sizeof(struct D2_control_ai); 
     if(init.d_ver>=d2_10_sw && loading_level_version<d2_10_sw)
      special=3;
     break;
    case ct_powerup: size+=sizeof(struct control_powerup); break;
    case ct_none: case ct_slew: case ct_control: break;
    default: waitmsg("Unknown thing control-type: %.2x.\n"\
     "Can't load level complete.",(unsigned)t->control); FREE(t); return NULL;
    }
   switch(t->movement)
    {
    case mt_none: break;
    case mt_physics: size+=sizeof(struct movement_physics); break;
    case mt_spinning: size+=sizeof(struct movement_spinning); break;
    default: waitmsg("Unknown thing movement-type: %.2x.\n"\
     "Can't load level complete.",(unsigned)t->movement); FREE(t);return NULL;
    }
   switch(t->render)
    {
    case rt_polyobj: size+=sizeof(struct render_polyobj); break;
    case rt_hostage: case rt_powerup: 
     size+=sizeof(struct render_powerup); break;
    default: waitmsg("Unknown thing render-type: %.2x.\n"\
     "Can't load level complete.",(unsigned)t->render); FREE(t);return NULL;
    }
  }
 checkmem(t=REALLOC(t,size));
 switch(special)
  {
  case 1: /* a start with movement mt_none */
   if(fread((unsigned char *)t+sizeof(struct thing)+
    sizeof(struct movement_physics),size-sizeof(struct thing)-
    sizeof(struct movement_physics),1,lf)!=1) { FREE(t); return NULL; }
   ((struct start *)t)->m=stdstart.m;
   break;
  case 2: /* a robot with movement mt_none */
   if(fread((unsigned char *)t+sizeof(struct thing)+
    sizeof(struct movement_physics),size-sizeof(struct thing)-
    sizeof(struct movement_physics),1,lf)!=1) { FREE(t); return NULL; }
   ((struct start *)t)->m=D2_stdrobot.m;
   break;
  case 3: /* loading a Descent 1 level in Descent 2 mode */
   if(fread(&D1_r.m,sizeof(struct D1_robot)-sizeof(struct thing),1,lf)!=1)
    { FREE(t); return NULL; }
   D2_r=(struct D2_robot *)t;
   D2_r->m=D1_r.m; D2_r->r=D1_r.r;
   D2_r->c.behavior=D1_r.c.behavior;
   for(j=0;j<MAX_AIFLAGS;j++) D2_r->c.flags[j]=D1_r.c.flags[j];
   D2_r->c.path_length=D1_r.c.path_length;
   D2_r->c.cur_path_index=D1_r.c.cur_path_index;
   D2_r->c.hide_cube=D1_r.c.hide_cube; D2_r->c.hide_index=D1_r.c.hide_index;
   D2_r->c.dying_sound_playing=0;
   break;
  default:
   if(fread((unsigned char *)t+sizeof(struct thing),
    size-sizeof(struct thing),1,lf)!=1) { FREE(t); return NULL; }
  }
 for(j=0;j<3;j++) t->p[0].x[j]=t->pos[j];
 for(j=0;j<3;j++)
  {
  for(i=0;i<3;i++) p.x[i]=t->orientation[j*3+i];
  if(LENGTH(&p)==0) break;
  }
 if(j!=3) 
  for(j=0;j<3;j++)
   for(i=0;i<3;i++) t->orientation[j*3+i]=view.e[j].x[i]*65536;
 setthingpts(t);
 if(init_test&2)
  {
  fprintf(errf,"Thingdata:\n");
  for(j=&t->type1-(unsigned char *)t;j<size;j++)
   {
   if(j==sizeof(struct thing)) fprintf(errf," / ");
   fprintf(errf," %.2x",*((unsigned char *)t+j));
   if((j-(&t->type1-(unsigned char *)t))%20==19) fprintf(errf,"\n");
   }
  fprintf(errf,"\n");
  }
 /* here it is not important if we use D1_robot or D2_robot */
 if(t->type1==tt1_robot && ((struct D1_robot *)t)->c.behavior==0)
  ((struct D1_robot *)t)->c.behavior=0x81;
 if(t->type1==tt1_reactor && init.d_ver>=d2_10_sw && 
  loading_level_version<d2_10_sw)
  { t->type2=0;
    ((struct reactor *)t)->r.model_num=reactorpolyobj[0]; }
 t->tagged=NULL; t->nc=NULL;
 return t;
 }
 
struct edoor *readedoors(FILE *f,unsigned long num,unsigned long size)
 {
 struct edoor *data,stuff;
 int i;
 if(num>1) waitmsg("Only one end door struct allowed. Ignoring the rest.");
 if(size!=sizeof(struct edoor)) waitmsg("Wrong end door struct size.");
 checkmem(data=MALLOC(sizeof(struct edoor)));
 if(num==0) 
  { for(i=0;i<10;i++) { data->cubes[i]=data->walls[i]=0; }
    data->num=0; return data; }
 if(fread(data,sizeof(struct edoor),1,f)!=1) { FREE(data); return NULL; }
 for(i=1;i<num;i++)
  if(fread(&stuff,sizeof(struct edoor),1,f)!=1)
   waitmsg("Can't read ignored 'open at end'-doors."); 
 return data;
 }
 
void *readpnt(FILE *lf)
 {
 long int coords[3];
 struct listpoint *p;
 if(fread(coords,sizeof(long int),3,lf)!=3) return NULL;
 checkmem(p=MALLOC(sizeof(struct listpoint)));
 p->p.x[0]=coords[0]; p->p.x[1]=coords[1]; p->p.x[2]=coords[2];
 p->tagged=NULL; initlist(&p->c);
 if(init_test&2)
  fprintf(errf,"Read coords: %g %g %g\n",p->p.x[0],p->p.x[1],p->p.x[2]);
 return p;
 }
 
struct D1_producer
 {
 unsigned long prodwhat NONANSI_FLAG;
 unsigned long hitpoints NONANSI_FLAG; /* 0x01f40000=32768000 */
 unsigned long interval NONANSI_FLAG; /* 0x00050000=327680 */
 unsigned short cubenum NONANSI_FLAG;
 unsigned short stuffnum NONANSI_FLAG;
 };
void *D1_REG_readproducer(FILE *f)
 {
 struct D1_producer D1_p;
 struct producer *p;
 checkmem(p=MALLOC(sizeof(struct producer)));
 if(fread(&D1_p,sizeof(struct D1_producer),1,f)!=1)
  { FREE(p); return NULL; }
 p->prodwhat[0]=D1_p.prodwhat; p->prodwhat[1]=0;
 p->hitpoints=D1_p.hitpoints; p->interval=D1_p.interval;
 p->cubenum=D1_p.cubenum; p->stuffnum=D1_p.stuffnum;
 if(init_test&2)
  {
  fprintf(errf,"read producer: cube: %hx %hx what: %lx %lx\n",p->cubenum,
   p->stuffnum,p->prodwhat[0],p->prodwhat[1]);
  if(p->hitpoints!=0x1f40000 || p->interval!=0x50000)
   fprintf(errf,"Producer cube %hd: Constant varies: %lx %lx\n",p->cubenum,
    p->hitpoints,p->interval);
  }
 p->c=NULL;
 return p;
 }

void *D2_REG_readproducer(FILE *f)
 {
 struct producer *p;
 checkmem(p=MALLOC(sizeof(struct producer)));
 if(fread(p,getsize(ds_producer,NULL),1,f)!=1) { FREE(p); return NULL; }
 if(init_test&2)
  {
  fprintf(errf,"read producer: cube: %hx %hx what: %lx %lx\n",p->cubenum,
   p->stuffnum,p->prodwhat[0],p->prodwhat[1]);
  if(p->hitpoints!=0x1f40000 || p->interval!=0x50000)
   fprintf(errf,"Producer cube %hd: Constant varies: %lx %lx\n",p->cubenum,
    p->hitpoints,p->interval);
  }
 p->c=NULL;
 return p;
 }

void *readturnoff(FILE *lf)
 {
 struct turnoff *to;
 checkmem(to=MALLOC(sizeof(struct turnoff)));
 if(fread(to,getsize(ds_turnoff,NULL),1,lf)!=1) { FREE(to); return NULL; }
 if(init_test&2)
  fprintf(errf,"Read turnoff: %hd %d %d %hd\n",to->cube,
   (int)to->side,(int)to->num_changed,to->offset);
 return to;
 }
 
void *readchangedlight(FILE *lf)
 {
 struct changedlight *cl;
 checkmem(cl=MALLOC(sizeof(struct changedlight)));
 if(fread(cl,getsize(ds_changedlight,NULL),1,lf)!=1) { FREE(cl);return NULL; }
 if(init_test&2)
  fprintf(errf,"Read changed light: %hd %d %d / %d %d %d %d\n",cl->cube,
   (int)cl->side,(int)cl->stuff,(int)cl->sub[0],(int)cl->sub[1],
   (int)cl->sub[2],(int)cl->sub[3]);
 return cl;
 }
 
void *D1_REG_readcube(FILE *lf)
 {
 struct cube *c;
 int j;
 unsigned char controlbyte;
 checkmem(c=MALLOC(sizeof(struct cube)));
 if(fread(&controlbyte,sizeof(unsigned char),1,lf)!=1)
  { FREE(c); return NULL; }
 if(init_test&2) fprintf(errf,"Read cube (%lx): %x ",ftell(lf),
  (int)controlbyte);
 for(j=0;j<6;j++)
  {
  if((controlbyte&(1<<j))!=0)
   {
   if(fread(&c->nextcubes[j],sizeof(unsigned short int),1,lf)!=1)
    { FREE(c); return NULL; }
   }
  else
   c->nextcubes[j]=0xffff;
  }
 if(fread(&c->pts[0],sizeof(short int),8,lf)!=8)
  { FREE(c); return NULL; }
 if((controlbyte&(1<<6))!=0)
  { if(fread(&c->type,sizeof(char),4,lf)!=4 || fread(&c->light,
     sizeof(short),1,lf)!=1)
     { FREE(c); return NULL; } }
 else
  { if(fread(&c->light,sizeof(short int),1,lf)!=1)
     { FREE(c); return NULL; }
    c->prodnum=-1; c->value=0; c->type=0; }
 if(fread(&controlbyte,sizeof(unsigned char),1,lf)!=1)
  { FREE(c); return NULL; }
 if(init_test&2)
  fprintf(errf,"%hx %hx %hx %x %hd %hd %hd %hd %hd %hd %hd %hd\n",c->type,
   c->prodnum,c->value,(int)controlbyte,c->pts[0],c->pts[1],c->pts[2],
   c->pts[3],c->pts[4],c->pts[5],c->pts[6],c->pts[7]);
 for(j=0;j<6;j++)
  {
  if((controlbyte&(1<<j))!=0)
   {
   if(fread(&c->doors[j],sizeof(unsigned char),1,lf)!=1)
    { FREE(c); return NULL; }
   }
  else 
   c->doors[j]=0xff;
  }
 for(j=0;j<6;j++)
  c->walls[j]=(c->nextcubes[j]==0xffff || c->doors[j]!=0xff) ?
   c->walls[j]=readwall(lf,j) : NULL;
 initlist(&c->sdoors); initlist(&c->things);
 for(j=0;j<8;j++) c->p[j]=NULL;
 for(j=0;j<6;j++) 
  { c->nc[j]=NULL; c->d[j]=NULL; c->tagged_walls[j]=NULL; }
 c->cp=NULL;
 return c;
 }
 
void *D2_REG_readcube(FILE *lf)
 {
 struct cube *c;
 int j;
 unsigned char controlbyte;
 checkmem(c=MALLOC(sizeof(struct cube)));
 if(fread(&controlbyte,sizeof(unsigned char),1,lf)!=1)
  { FREE(c); return NULL; }
 if(init_test&2) fprintf(errf,"Offset %lx Read cube: %x ",
  (long int)ftell(lf)-1, (int)controlbyte);
 for(j=0;j<6;j++)
  {
  if((controlbyte&(1<<j))!=0)
   {
   if(fread(&c->nextcubes[j],sizeof(unsigned short int),1,lf)!=1)
    { FREE(c); return NULL; }
   }
  else
   c->nextcubes[j]=0xffff;
  }
 if(fread(&c->pts[0],sizeof(short int),8,lf)!=8)
  { FREE(c); return NULL; }
 c->prodnum=-1; c->value=0; c->type=0; c->light=0xffff;
 if(fread(&controlbyte,sizeof(unsigned char),1,lf)!=1)
  { FREE(c); return NULL; }
 for(j=0;j<6;j++)
  {
  if((controlbyte&(1<<j))!=0)
   {
   if(fread(&c->doors[j],sizeof(unsigned char),1,lf)!=1)
    { FREE(c); return NULL; }
   }
  else 
   c->doors[j]=0xff;
  }
 if(init_test&2)
  {
  fprintf(errf,"%hx %hx %hx %hx %hx %hx\n",c->nextcubes[0],c->nextcubes[1],
   c->nextcubes[2],c->nextcubes[3],c->nextcubes[4],c->nextcubes[5]);
  fprintf(errf,"%x %hd %hd %hd %hd %hd %hd %hd %hd\n",
   (int)controlbyte,c->pts[0],c->pts[1],c->pts[2],
   c->pts[3],c->pts[4],c->pts[5],c->pts[6],c->pts[7]);
  fprintf(errf," %x %x %x %x %x %x\n",c->doors[0],c->doors[1],c->doors[2],
   c->doors[3],c->doors[4],c->doors[5]);
  }
 for(j=0;j<6;j++)
  c->walls[j]=(c->nextcubes[j]==0xffff || c->doors[j]!=0xff) ?
   c->walls[j]=readwall(lf,j) : NULL;
 initlist(&c->sdoors); initlist(&c->things);
 for(j=0;j<8;j++) c->p[j]=NULL;
 for(j=0;j<6;j++) 
  { c->nc[j]=NULL; c->d[j]=NULL; c->tagged_walls[j]=NULL; }
 c->cp=NULL;
 return c;
 }
 
int D2_REG_readcube2(FILE *lf,struct node *n)
 { 
 int ret;
 unsigned long light;
 ret=(fread(n->d.c,1,4,lf)==4 && fread(&light,4,1,lf)==1); 
 n->d.c->light=(light>>5)&0x7fff;
 if(init_test&2)
  fprintf(errf,
   "Readcube2 %d: type=%x prodnum=%x value=%x flags=%x light=%lx-->l=%hx\n",
    n->no,n->d.c->type,n->d.c->prodnum,n->d.c->value,n->d.c->flags,
    light,n->d.c->light);
 return ret;
 }
 
void *readflickeringlight(FILE *lf)
 {
 struct flickering_light *fl;
 checkmem(fl=MALLOC(sizeof(struct flickering_light)));
 if(fread(fl,getsize(ds_flickeringlight,NULL),1,lf)!=1) 
  { FREE(fl); return NULL; }
 if(init_test&2)
  fprintf(errf,"Read flickering light: %hd %hd %lx %ld %ld\n",fl->cube,
   fl->wall,fl->mask,fl->timer,fl->delay);
 return fl; 
 }
 
int readlist(FILE *f,struct list *l,void *(*readdata)(FILE *),int num)
 {
 int i,n;
 void *d;
 struct node *np;
 if(num<0) { if(fread(&n,sizeof(int),1,f)!=1) return 0; }
 else n=num;
 for(i=0;i<n;i++)
  {
  if(init_test&2) fprintf(errf,"Reading list entry %d (%d)\n",i,n);
  if((d=readdata(f))==NULL) return 0;
  checkmem(np=addnode(l,i,d));
  }
 return 1;
 }
 
int savepoint(FILE *f,struct node *n,va_list args)
 {
 struct point *p=n->d.p;
 long coords[3];
 coords[0]=p->x[0]; coords[1]=p->x[1]; coords[2]=p->x[2];
 return (fwrite(coords,sizeof(long int),3,f)==3);
 }

int D1_REG_savecube(FILE *f,struct node *n,va_list args)
 {
 struct cube *c=n->d.c;
 int j,exitwall;
 short int t1,t2;
 char *offset;
 struct node *exitcube;
 int size;
 unsigned char cbnext,cbdoor=0;
 exitcube=va_arg(args,struct node *);
 exitwall=va_arg(args,int);
 c->prodnum=c->cp ? c->cp->no : -1; 
 cbnext=(c->type!=0 || c->prodnum!=-1 || c->value!=0) ? 0x40 : 0x0;
 for(j=0;j<6;j++)
  {
  if(c->nc[j]!=NULL) cbnext|=1<<j;
  if(c->d[j]!=NULL) cbdoor|=1<<j;
  }
 if(exitcube!=NULL && n->no==exitcube->no) cbnext|=1<<exitwall;
 if(fwrite(&cbnext,1,1,f)!=1) return 0;
 for(j=0;j<8;j++) c->pts[j]=c->p[j]->no;
 for(j=0;j<6;j++)
  {
  c->doors[j]=(c->d[j]!=NULL) ? c->d[j]->no : 0xff;
  if(exitcube!=NULL && n->no==exitcube->no && j==exitwall)
   c->nextcubes[j]=0xfffe;
  else
   c->nextcubes[j]=(c->nc[j]!=NULL) ? c->nc[j]->no : 0xffff;
  }
 for(j=0;j<6;j++)
  if(cbnext&(1<<j)) 
   if(fwrite(&c->nextcubes[j],sizeof(short int),1,f)!=1) return 0;
 if(fwrite(&c->pts[0],sizeof(short int),8,f)!=8) return 0;
 if(cbnext&(1<<6))
  { if(fwrite(&c->type,sizeof(char),4,f)!=4 || 
     fwrite(&c->light,sizeof(short),1,f)!=1) return 0; }
 else
  { if(fwrite(&c->light,sizeof(short int),1,f)!=1) return 0; }
 if(fwrite(&cbdoor,1,1,f)!=1) return 0;
 for(j=0;j<6;j++)
  if(cbdoor&(1<<j))
   if(fwrite(&c->doors[j],sizeof(char),1,f)!=1) return 0;
 for(j=0;j<6;j++)
  {
  if(c->nextcubes[j]==0xffff || c->doors[j]!=0xff)
   {
   t1=c->walls[j]->texture1;
   size=getsize(ds_wall,NULL)-4;
   offset=(char *)c->walls[j]+4;
   if(c->walls[j]->texture2!=0) 
    { t1|=0x8000;t2=c->walls[j]->texture2|(c->walls[j]->txt2_direction<<14); }
   else t2=0;
   if(fwrite(&t1,sizeof(short int),1,f)!=1) return 0;
   if(t2!=0) if(fwrite(&t2,sizeof(short int),1,f)!=1) return 0;
   if(fwrite(offset,size,1,f)!=1) return 0;
   }
  }
 return 1;
 }
 
int D2_REG_savecube(FILE *f,struct node *n,va_list args)
 {
 struct cube *c=n->d.c;
 int j,exitwall;
 short int t1,t2;
 char *offset;
 struct node *exitcube;
 int size;
 unsigned char cbnext=0,cbdoor=0;
 exitcube=va_arg(args,struct node *);
 exitwall=va_arg(args,int);
 for(j=0;j<6;j++)
  {
  if(c->nc[j]!=NULL) cbnext|=1<<j;
  if(c->d[j]!=NULL) cbdoor|=1<<j;
  }
 if(exitcube!=NULL && n->no==exitcube->no) cbnext|=1<<exitwall;
 for(j=0;j<6;j++)
  {
  c->doors[j]=(c->d[j]!=NULL) ? c->d[j]->no : 0xff;
  if(exitcube!=NULL && n->no==exitcube->no && j==exitwall)
   c->nextcubes[j]=0xfffe;
  else
   c->nextcubes[j]=(c->nc[j]!=NULL) ? c->nc[j]->no : 0xffff;
  }
 for(j=0;j<8;j++) c->pts[j]=c->p[j]->no;
 if(fwrite(&cbnext,1,1,f)!=1) return 0;
 for(j=0;j<6;j++)
  if(cbnext&(1<<j)) 
   if(fwrite(&c->nextcubes[j],sizeof(short int),1,f)!=1) return 0;
 if(fwrite(&c->pts[0],sizeof(short int),8,f)!=8) return 0;
 if(fwrite(&cbdoor,1,1,f)!=1) return 0;
 for(j=0;j<6;j++)
  if(cbdoor&(1<<j))
   if(fwrite(&c->doors[j],sizeof(char),1,f)!=1) return 0;
 for(j=0;j<6;j++)
  {
  if(c->nextcubes[j]==0xffff || c->doors[j]!=0xff)
   {
   t1=c->walls[j]->texture1;
   size=getsize(ds_wall,NULL)-4;
   offset=(char *)c->walls[j]+4;
   if(c->walls[j]->texture2!=0) 
    { t1|=0x8000;t2=c->walls[j]->texture2|(c->walls[j]->txt2_direction<<14); }
   else t2=0;
   if(fwrite(&t1,sizeof(short int),1,f)!=1) return 0;
   if(t2!=0) if(fwrite(&t2,sizeof(short int),1,f)!=1) return 0;
   if(fwrite(offset,size,1,f)!=1) return 0;
   }
  }
 return 1;
 }

int D2_REG_savecube2(FILE *f,struct node *n,va_list args)
 { 
 struct cube *c=n->d.c;
 unsigned long light;
 c->prodnum=c->cp ? c->cp->no : -1; c->value=0;
 light=c->light<<5;
 return fwrite(c,1,4,f)==4 && fwrite(&light,4,1,f)==1;
 }
 
int savething(FILE *f,struct node *n,va_list args)
 {
 struct thing *t=n->d.t;
 int i;
 for(i=0;i<3;i++) { t->lastpos[i]=0; t->pos[i]=t->p[0].x[i]; }
 if(t->control==ct_mine) 
  { ((struct mine *)t)->c.parent_type=5;
    ((struct mine *)t)->c.parent_num=n->no; }
 return (fwrite(&t->type1,getsize(ds_thing,t),1,f)==1);
 }

int savedoor(FILE *f,struct node *n,va_list args)
 {
 struct door *d=n->d.d;
 d->cubenum=d->c->no; d->sdoor=d->sd ? d->sd->no : 0xff;
 return (fwrite(d,getsize(ds_door,NULL),1,f)==1);
 }

int D1_REG_savesdoor(FILE *f,struct node *d,va_list args)
 {
 struct sdoor *sd=d->d.sd;
 struct D1_sdoor D1_sd;
 int i;
 D1_sd.value=sd->value; D1_sd.link_num=sd->link_num;
 D1_sd.num=sd->num; D1_sd.type_0=0; D1_sd.time=-1;
 for(i=0;i<10;i++) 
  { D1_sd.cubes[i]=sd->cubes[i]; D1_sd.walls[i]=sd->walls[i]; }
 switch(sd->type)
  {
  case switch_opendoor: D1_sd.flags=1; break;
  case switch_producer: D1_sd.flags=0x40; break;
  case switch_exit: D1_sd.flags=0x8; break;
  case switch_secretexit: D1_sd.flags=0x100; break;
  }
 return (fwrite(&D1_sd,sizeof(struct D1_sdoor),1,f)==1); 
 }
 
int D1_REG_saveproducer(FILE *f,struct node *n,va_list args)
 {
 struct D1_producer D1_p;
 struct producer *p=n->d.cp;
 D1_p.prodwhat=p->prodwhat[0]; D1_p.hitpoints=p->hitpoints;
 D1_p.interval=p->interval; D1_p.cubenum=p->cubenum; 
 D1_p.stuffnum=p->stuffnum;
 return (fwrite(&D1_p,sizeof(struct D1_producer),1,f)==1);
 }
 
int saveflickeringlight(FILE *f,struct node *n,va_list args)
 { my_assert(n->d.fl->ls!=NULL);
   n->d.fl->cube=n->d.fl->ls->d.ls->cube->no;
   return (fwrite(n->d.fl,getsize(ds_flickeringlight,NULL),1,f)==1); }
   
int savedata(FILE *f,struct node *d,va_list args)
 { int ds=va_arg(args,int); return (fwrite(d->d.d,getsize(ds,NULL),1,f)==1); }
 
int savelist(FILE *f,struct list *l,int (*saveproc)(FILE *,struct node *,
 va_list args),int withnum,...)
 {
 struct node *n;
 va_list args;
 va_start(args,withnum);
 if(withnum)
  if(fwrite(&l->size,sizeof(int),1,f)!=1) return 0;
 for(n=l->head;n->next!=NULL;n=n->next)
  if(!saveproc(f,n,args)) break;
 va_end(args);
 return (n->next==NULL);
 }

int readasciicube(struct leveldata *ld,FILE *f)
 {
 int segnr,sidenr,i,j,t1,t2,uvl[6][4][3],child[6],slight;
 struct point pnts[8];
 struct cube *c;
 struct node *n;
 struct listpoint *lp;
 if(fscanf(f," segment %d",&segnr)!=1 || segnr!=ld->cubes.size) return 0;
 for(sidenr=0;sidenr<6;sidenr++)
  {
  if(fscanf(f," side %d",&i)!=1 || i!=sidenr) return 0;
  if(fscanf(f," tmap_num %d tmap_num2 %d",&t1,&t2)!=2) return 0;
  for(i=0;i<4;i++)
   if(fscanf(f," uvls %d %d %d",&uvl[sidenr][i][0],
    &uvl[sidenr][i][1],&uvl[sidenr][i][2])!=3)
    return 0;
  }
 if(fscanf(f," children %d %d %d %d %d %d",&child[0],&child[1],&child[2],
  &child[3],&child[4],&child[5])!=6) return 0;
 for(i=0;i<8;i++)
  if(fscanf(f," vms_vector %d %g %g %g",&j,&pnts[i].x[0],&pnts[i].x[1],
   &pnts[i].x[2])!=4 || j!=i) return 0;
 if(fscanf(f," static_light %d",&slight)!=1) return 0;
 checkmem(c=MALLOC(sizeof(struct cube)));
 for(i=0;i<8;i++)
  {
  for(n=ld->pts.head;n->next!=NULL;n=n->next)
   if(n->d.p->x[0]==pnts[i].x[0] && n->d.p->x[1]==pnts[i].x[1] &&
    n->d.p->x[2]==pnts[i].x[2]) break;
  if(n->next==NULL)
   {
   checkmem(lp=MALLOC(sizeof(struct listpoint)));
   checkmem(n=addnode(&ld->pts,-1,lp));
   lp->tagged=NULL; initlist(&lp->c);
   lp->p=pnts[i];
   }
  c->pts[i]=n->no;
  }
 for(i=0;i<6;i++)
  {
  c->nextcubes[i]=child[i];
  c->doors[i]=0xff;
  if(c->nextcubes[i]==0xffff)
   {
   checkmem(c->walls[i]=MALLOC(sizeof(struct wall)));
   c->walls[i]->texture1=t1&0x3fff;
   c->walls[i]->texture2=t2&0x3fff;
   c->walls[i]->txt2_direction=(t2>>14)&0x3;
   for(j=0;j<4;j++)
    {
    c->walls[i]->corners[j].x[0]=uvl[i][j][0];
    c->walls[i]->corners[j].x[1]=uvl[i][j][1];
    c->walls[i]->corners[j].light=uvl[i][j][2];
    }
   c->walls[i]->no=i; c->walls[i]->locked=0; c->walls[i]->ls=NULL; 
   }
  else c->walls[i]=NULL;
  }
 c->light=slight; c->high_light=0;
 c->prodnum=-1; c->value=0; c->flags=0; c->type=cube_normal; 
 for(j=0;j<8;j++) c->p[j]=NULL;
 for(j=0;j<6;j++) 
  { c->nc[j]=NULL; c->d[j]=NULL; c->tagged_walls[j]=NULL; }
 c->cp=NULL;
 checkmem(addnode(&ld->cubes,-1,c));
 return 1;
 }
