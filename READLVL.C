/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    readlvl.c - reading whole levels.
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

#include <dir.h>

#include "structs.h"
#include "userio.h"
#include "tools.h"
#include "savetool.h"
#include "insert.h"
#include "plot.h"
#include "click.h"
#include "do_opts.h"
#include "options.h"
#include "tag.h"
#include "grfx.h"
#include "plottxt.h"
#include "do_move.h"
#include "do_mod.h"
#include "do_light.h"
#include "readtxt.h"
#include "readlvl.h"

#include "lac_cfg.h"

enum descent loading_level_version;

/* this structure should be Descent version independent */
struct fileheadversion 
 { char lvlp[4] NONANSI_FLAG; unsigned long int version NONANSI_FLAG; };
#define LEVVER_D2_12_REG 8
#define LEVVER_D2_11_REG 7
#define LEVVER_D2_10_REG 6
#define LEVVER_D2_10_SW 5
#define LEVVER_D1_REG 1
struct D1_REG_levelfilehead
 {
 struct fileheadversion fh NONANSI_FLAG;
 unsigned long int minedata_offset NONANSI_FLAG;
 unsigned long int gamedata_offset NONANSI_FLAG;
 unsigned long int hostagetxt_offset NONANSI_FLAG;
 } D1_REG_stdlevelfilehead={ { {'L','V','L','P'},LEVVER_D1_REG },0x14,0,0 };
struct D2_SW_levelfilehead
 {
 struct fileheadversion fh NONANSI_FLAG;
 unsigned long int minedata_offset NONANSI_FLAG; /* 0x23 */
 unsigned long int gamedata_offset NONANSI_FLAG;
 char group[11] NONANSI_FLAG; /* GROUPA.256 and a linefeed */
 unsigned long stuff[2] NONANSI_FLAG; /* 0x1e and 0xffffffff */
 } D2_SW_stdlevelfilehead={ { {'L','V','L','P'},LEVVER_D2_10_SW },0x23,0,
  { 'G','R','O','U','P','A','.','2','5','6','\x0A' },{ 0x1e,0xffffffff } };
struct D2_REG_levelfilehead
 {
 struct fileheadversion fh;
 unsigned long int minedata_offset NONANSI_FLAG;
 unsigned long int gamedata_offset NONANSI_FLAG;
 char *palname; /* terminated by a linefeed */
 unsigned long reactor_time,reactor_strength NONANSI_FLAG;
  /* 0x1e and 0xffffffff */ 
 unsigned long flickering_lights NONANSI_FLAG;
 unsigned long secret_cubenum NONANSI_FLAG;
 unsigned long secret_orient[9] NONANSI_FLAG;
 } D2_REG_stdlevelfilehead={ {{ 'L','V','L','P' },LEVVER_D2_11_REG}, 0,0,NULL,
  0x1e,0xffffffff, 0, 0, { 0x10000,0,0,0,0,0x10000,0,0x10000,0 } };
struct D1_minedata
 {
 unsigned char version NONANSI_FLAG; /* always 0 */
 unsigned short numpts NONANSI_FLAG,
  numcubes NONANSI_FLAG;
 };
struct D2_minedata
 {
 unsigned char version NONANSI_FLAG; /* always 0 */
 unsigned short numpts NONANSI_FLAG,
  numcubes NONANSI_FLAG;
 };
struct D1_gamedata
 {
 /* header for things etc. */
 unsigned short signature NONANSI_FLAG; /* 0x6705 */
 unsigned short version NONANSI_FLAG; /* reg. version=0x19 */
 long sizedir NONANSI_FLAG; /* 0x77 */
 char mine_filename[15] NONANSI_FLAG; /* never used by anyone */
 long level NONANSI_FLAG; /* always zero in not saved games I suppose */
 unsigned long posplayer NONANSI_FLAG,sizeplayer NONANSI_FLAG;
 unsigned long posthings NONANSI_FLAG,
  numthings NONANSI_FLAG,sizethings NONANSI_FLAG; 
  /* size=0x108 */
 unsigned long posdoors NONANSI_FLAG,
  numdoors NONANSI_FLAG,sizedoors NONANSI_FLAG; 
  /* size=0x18 */
 unsigned long posopendoors NONANSI_FLAG,
  numopendoors NONANSI_FLAG,sizeopendoors NONANSI_FLAG; 
  /* num=0, size=0x10 */
 /* switches */
 unsigned long possdoors NONANSI_FLAG,
  numsdoors NONANSI_FLAG,sizesdoors NONANSI_FLAG; 
  /* size=0x36 */
 unsigned long stuff2[3] NONANSI_FLAG;  /* always zero */
 /* doors opening after blown the reactor */
 unsigned long posedoors NONANSI_FLAG,
  numedoors NONANSI_FLAG,sizeedoors NONANSI_FLAG; 
  /* size=0x2a */
 /* robot producers */
 unsigned long posproducer NONANSI_FLAG,
  numproducer NONANSI_FLAG,sizeproducer NONANSI_FLAG; 
  /* size=0x10 */
 } D1_stdgamedata = { 0x6705,0x19,0x77,{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },0,
    0,0x8e, 0,0,0x108, 0,0,0x18, 0,0,0x10, 0,0,0x36, { 0,0,0 }, 0,0,0x2a, 
    0,0,0x10 };
struct D2_gamedata
 {
 /* header for things etc. */
 unsigned short signature NONANSI_FLAG; /* 0x6705 */
 unsigned short version NONANSI_FLAG; /* reg. version=0x20 */
 long sizedir NONANSI_FLAG; /* 0x8f */
 char mine_filename[15] NONANSI_FLAG; /* never used by anyone */
 long level NONANSI_FLAG; /* always zero in not saved games I suppose */
 unsigned long posplayer NONANSI_FLAG,sizeplayer NONANSI_FLAG;
 unsigned long posthings NONANSI_FLAG,
  numthings NONANSI_FLAG,sizethings NONANSI_FLAG; 
  /* size=0x108 */
 unsigned long posdoors NONANSI_FLAG,
  numdoors NONANSI_FLAG,sizedoors NONANSI_FLAG; 
  /* size=0x18 */
 unsigned long posopendoors NONANSI_FLAG,
  numopendoors NONANSI_FLAG,sizeopendoors NONANSI_FLAG; 
  /* num=0, size=0x10 */
 /* switches */
 unsigned long possdoors NONANSI_FLAG,
  numsdoors NONANSI_FLAG,sizesdoors NONANSI_FLAG; 
  /* size=0x34 */
 unsigned long stuff2[3] NONANSI_FLAG;  /* always zero */
 /* doors opening after blown the reactor */
 unsigned long posedoors NONANSI_FLAG,
  numedoors NONANSI_FLAG,sizeedoors NONANSI_FLAG; 
  /* size=0x2a */
 /* robot producers */
 unsigned long posproducer NONANSI_FLAG,
  numproducer NONANSI_FLAG,sizeproducer NONANSI_FLAG; 
  /* size=0x14 */
 /* this are all lamps that can be shot in the level */
 unsigned long posturnoff NONANSI_FLAG,
  numturnoff NONANSI_FLAG,sizeturnoff NONANSI_FLAG; 
  /* size=0x06 */
 /* this are all lights that are changed by turnoff lamps */
 unsigned long poschangedlight NONANSI_FLAG,
  numchangedlight NONANSI_FLAG,sizechangedlight NONANSI_FLAG; 
  /* size=0x08 */
 } D2_stdgamedata = { 0x6705,0x20,0x8f,{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },0,
    0,0x8e, 0,0,0x108, 0,0,0x18, 0,0,0x10, 0,0,0x34, { 0,0,0 }, 0,0,0x2a, 
    0,0,0x14, 0,0,0x6, 0,0,0x8 };
 
unsigned long sizeplayer=0;
char *playerdata=NULL;
 
extern int init_test;

void killallproducers(struct leveldata *ld)
 {
 struct node *n;
 freelist(&ld->producers,free);
 for(n=ld->cubes.head;n->next!=NULL;n=n->next) n->d.c->prodnum=-1;
 }
 
void killallswitches(struct leveldata *ld)
 {
 struct node *n;
 freelist(&ld->sdoors,free);
 for(n=ld->doors.head;n->next!=NULL;n=n->next) n->d.d->sdoor=0xff;
 killallproducers(ld);
 } 
 
void killalldoors(struct leveldata *ld)
 {
 struct node *n;
 int i;
 freelist(&ld->doors,free);
 if(ld->edoors) FREE(ld->edoors);
 checkmem(ld->edoors=MALLOC(sizeof(struct edoor)));
 ld->edoors->num=0;
 for(n=ld->cubes.head;n->next!=NULL;n=n->next) 
  for(i=0;i<6;i++) n->d.c->doors[i]=0xff;
 killallproducers(ld);
 killallswitches(ld);
 }

int findtxt(int old_txt)
 {
 int i;
 for(i=0;i<init.num_convtxts;i++)
  if(init.convtxts[i*2]==old_txt) return init.convtxts[i*2+1];
 return -1;
 }
 
unsigned char findanim(unsigned old_anim)
 {
 int i;
 for(i=0;i<init.num_convanims;i++)
  if(init.convanims[i*2]==old_anim) return init.convanims[i*2+1];
 return 0xff;
 }

void convert_textures(struct leveldata *ld)
 {
 int all_found=1,w,new_txt;
 struct node *nc;
 unsigned char new_anim;
 printmsg(TXT_CONVTEXTURES);
 for(nc=ld->cubes.head;nc->next!=NULL;nc=nc->next)
  for(w=0;w<6;w++)
   if(nc->d.c->walls[w])
    {
    if((new_txt=findtxt(nc->d.c->walls[w]->texture1))!=-1)
     nc->d.c->walls[w]->texture1=new_txt;
    else 
     { all_found=0; nc->d.c->walls[w]->texture1=init.def_t1; }
    if(nc->d.c->walls[w]->texture2!=0)
     if((new_txt=findtxt(nc->d.c->walls[w]->texture2))!=-1)
      nc->d.c->walls[w]->texture2=new_txt;
     else 
      { all_found=0; nc->d.c->walls[w]->texture2=init.def_t2; }
    }
 if(!all_found)
  waitmsg(TXT_NOTALLTXTSCONV,pig.rdl_txts[init.def_t1].pig->name,
   init.def_t1,init.def_t2==0 ? "Nothing" : 
   pig.rdl_txts[init.def_t2].pig->name,init.def_t2);
 for(nc=ld->doors.head;nc->next!=NULL;nc=nc->next)
  if((new_anim=findanim(nc->d.d->animtxt))!=0xff) nc->d.d->animtxt=new_anim;
 }
 
int D1_REG_readlvldata(FILE *lf,struct leveldata *ld,int version)
 {
 int i,j;
 struct node *n;
 void *d;
 struct D1_REG_levelfilehead lfh;
 struct D1_minedata md;
 struct D1_gamedata gd;
 char buffer[31];
 const char *palname;
 if(version!=LEVVER_D1_REG) return 0;
 if(fread(&lfh.minedata_offset,sizeof(unsigned long),2,lf)!=2)
  { fclose(lf); return 0; }
 if(fseek(lf,lfh.minedata_offset,SEEK_SET))
  { fclose(lf); return 0; }
 if(fread(&md,sizeof(struct D1_minedata),1,lf)!=1)
  { fclose(lf); return 0; }
 /* texture are translated if init.d_ver>=d2_10_sw */
 palname=init.d_ver>=d2_10_sw ? "groupa.256" : "descent.256";
 checkmem(ld->pigname=MALLOC(strlen(palname)+1));
 strcpy(ld->pigname,palname);
 printmsg(TXT_READINGPTS,md.numpts);
 if(init_test&2)
  fprintf(errf,"%d points, %d cubes\n",md.numpts,md.numcubes);
 if(!readlist(lf,&ld->pts,readpnt,md.numpts)) { fclose(lf); return 0; }
 /* Init fullname: Needed when an error occurs during loading the level */
 checkmem(ld->fullname=MALLOC(strlen("LEVEL WITH ERROR")+1));
 strcpy(ld->fullname,"LEVEL WITH ERROR");
 for(i=0;i<md.numcubes;i++)
  {
  if(init_test&2) fprintf(errf,"Reading cube %d/%d\n",i,md.numcubes);
  if((d=D1_REG_readcube(lf))==NULL) return 1; /* try to init level */
  checkmem(n=addnode(&ld->cubes,i,d));
  for(j=0;j<6;j++)
   if(n->d.c->nextcubes[j]==0xfffe) /* exit */
    { ld->exitcube=n; ld->exitwall=j;
      n->d.c->nextcubes[j]=0xffff; }
  }
 fseek(lf,lfh.gamedata_offset,SEEK_SET);
 if(fread(&gd,sizeof(struct D1_gamedata),1,lf)!=1)
  { waitmsg(TXT_CANTREADLVL); killalldoors(ld); return 1; }
 i=0;
 while(isprint(buffer[i++]=fgetc(lf)) && i<31);
 buffer[i-1]=0; FREE(ld->fullname);
 checkmem(ld->fullname=MALLOC(strlen(buffer)+1));
 strcpy(ld->fullname,buffer);
 fseek(lf,gd.posplayer,SEEK_SET);
 sizeplayer=gd.sizeplayer;
 checkmem(playerdata=REALLOC(playerdata,sizeplayer));
 if(fread(playerdata,sizeplayer,1,lf)!=1)
  { killalldoors(ld); return 1; }
 fseek(lf,gd.posthings,SEEK_SET);
 if(!readlist(lf,&ld->things,readthing,gd.numthings)) 
  { killalldoors(ld); return 1; }
 fseek(lf,gd.posdoors,SEEK_SET);
 if(!readlist(lf,&ld->doors,readdoor,gd.numdoors)) 
  { killalldoors(ld); return 1; }
 fseek(lf,gd.possdoors,SEEK_SET);
 if(!readlist(lf,&ld->sdoors,D1_REG_readsdoor,gd.numsdoors)) 
  { killallswitches(ld); return 1; }
 fseek(lf,gd.posedoors,SEEK_SET);
 if((ld->edoors=readedoors(lf,gd.numedoors,gd.sizeedoors))==NULL)
  return 1;
 fseek(lf,gd.posproducer,SEEK_SET);
 if(!readlist(lf,&ld->producers,D1_REG_readproducer,gd.numproducer)) 
  { killallproducers(ld); return 1; }
 fclose(lf); ld->levelillum=0; 
 if(init.d_ver>=d2_10_sw) convert_textures(ld);
 return 1; 
 }

void makelightsources(struct leveldata *ld,struct turnoff *to,int tonum,
 struct changedlight *cl,int clnum,struct flickering_light *fl,int flnum)
 {
 int nto,ncl,i;
 struct lightsource *ls;
 struct ls_effect *lse,**cubelse;
 struct flickering_light *nfl;
 struct node **cubes,*n;
 if(!to || !cl) return;
 checkmem(cubelse=MALLOC(sizeof(struct ls_effect *)*ld->cubes.size));
 checkmem(cubes=MALLOC(sizeof(struct node *)*ld->cubes.size));
 for(n=ld->cubes.head,i=0;n->next!=NULL && i<ld->cubes.size;n=n->next,i++)
  { cubes[i]=n; cubelse[i]=NULL; }
 for(nto=0;nto<tonum;nto++)
  {
  if(to[nto].cube>=ld->cubes.size || to[nto].side>=6 ||
   cubes[to[nto].cube]->d.c->walls[to[nto].side]==NULL) continue;
  checkmem(ls=MALLOC(sizeof(struct lightsource)));
  ls->cube=cubes[to[nto].cube];
  ls->w=to[nto].side; initlist(&ls->effects); ls->fl=NULL;
  checkmem(ls->cube->d.c->walls[(int)ls->w]->ls=addnode(&ld->lightsources,-1,
   ls));
  for(ncl=to[nto].offset;ncl<to[nto].offset+to[nto].num_changed && ncl<clnum;
      ncl++)
   {
   if(cl[ncl].cube>=ld->cubes.size || cl[ncl].side>=6) continue;
   if((lse=cubelse[cl[ncl].cube])==NULL)
    {
    checkmem(cubelse[cl[ncl].cube]=lse=MALLOC(sizeof(struct ls_effect)));
    checkmem(addnode(&ls->effects,-1,lse));
    lse->cube=cubes[cl[ncl].cube];
    memset(lse->smoothed,0,sizeof(unsigned char)*24);
    memset(lse->add_light,0,sizeof(unsigned char)*24);
    }
   for(i=0;i<4;i++) lse->add_light[cl[ncl].side*4+i]=cl[ncl].sub[i]<<10;
   }
  for(ncl=to[nto].offset;ncl<to[nto].offset+to[nto].num_changed && ncl<clnum;
      ncl++)
   if(cl[ncl].cube<ld->cubes.size) cubelse[cl[ncl].cube]=NULL;
  }
 if(fl!=NULL) for(i=0;i<flnum;i++)
  {
  if(fl[i].cube<0 || fl[i].cube>=ld->cubes.size ||
   cubes[fl[i].cube]->d.c->walls[fl[i].wall]==NULL ||
   cubes[fl[i].cube]->d.c->walls[fl[i].wall]->ls==NULL) continue;
  checkmem(cubes[fl[i].cube]->d.c->walls[fl[i].wall]->ls->d.ls->fl=nfl=
   MALLOC(sizeof(struct flickering_light)));
  *nfl=fl[i];
  nfl->ls=cubes[fl[i].cube]->d.c->walls[fl[i].wall]->ls;
  nfl->calculated=0; nfl->state=1;
  }
 FREE(cubes); FREE(cubelse);
 }

int D2_REG_readlvldata(FILE *lf,struct leveldata *ld,int version)
 {
 int i,j;
 struct node *n;
 void *d;
 struct D2_REG_levelfilehead lfh;
 struct D2_minedata md;
 struct D2_gamedata gd;
 char buffer[31];
 struct turnoff *turnoffs,*to;
 struct changedlight *changedlights,*cl;
 struct flickering_light *fl_lights=NULL,*fl;
 if(fread(&lfh.minedata_offset,sizeof(unsigned long),2,lf)!=2)
  { fclose(lf); return 0; }
 if(version==LEVVER_D2_12_REG)
  if(fread(buffer,1,7,lf)!=7)
   { fclose(lf); return 0; }
 i=0; while(isprint(buffer[i++]=fgetc(lf)) && i<31);
 buffer[i-1]=0; 
 checkmem(ld->pigname=MALLOC(strlen(buffer)+1));
 strcpy(ld->pigname,buffer);
 if(fread(&lfh.reactor_time,sizeof(unsigned long),2,lf)!=2)
  { fclose(lf); return 0; }
 ld->reactor_time=lfh.reactor_time;
 ld->reactor_strength=lfh.reactor_strength;
 if(version>=LEVVER_D2_11_REG)
  {
  if(fread(&lfh.flickering_lights,sizeof(unsigned long),1,lf)!=1)
   { fclose(lf); return 0; }
  if(lfh.flickering_lights>0)
   {
   checkmem(fl_lights=MALLOC(sizeof(struct flickering_light)*
    lfh.flickering_lights));
   for(j=0;j<lfh.flickering_lights;j++)
    if((fl=readflickeringlight(lf))!=NULL) { fl_lights[j]=*fl; FREE(fl); }
    else { FREE(fl_lights); fclose(lf); return 0; }
   }
  }
 else lfh.flickering_lights=0;
 if(fread(&lfh.secret_cubenum,sizeof(unsigned long),10,lf)!=10)
  { fclose(lf); return 0; }
 for(j=0;j<3;j++)
  {
  ld->secret_orient[j]=lfh.secret_orient[j];
  ld->secret_orient[j+3]=lfh.secret_orient[j+6];
  ld->secret_orient[j+6]=lfh.secret_orient[j+3];
  }
 if(fseek(lf,lfh.minedata_offset,SEEK_SET))
  { fclose(lf); return 0; }
 if(fread(&md,sizeof(struct D2_minedata),1,lf)!=1)
  { fclose(lf); return 0; }
 printmsg(TXT_READINGPTS,md.numpts);
 if(init_test&2)
  fprintf(errf,"%d points, %d cubes\n",md.numpts,md.numcubes);
 if(!readlist(lf,&ld->pts,readpnt,md.numpts)) { fclose(lf); return 0; }
 /* Init fullname: Needed when an error occurs during loading the level */
 checkmem(ld->fullname=MALLOC(strlen("LEVEL WITH ERROR")+1));
 strcpy(ld->fullname,"LEVEL WITH ERROR");
 for(i=0;i<md.numcubes;i++)
  {
  if(init_test&2) fprintf(errf,"Reading cube %d/%d\n",i,md.numcubes);
  if((d=D2_REG_readcube(lf))==NULL) return 1; /* try to init level */
  checkmem(n=addnode(&ld->cubes,i,d));
  for(j=0;j<6;j++)
   if(n->d.c->nextcubes[j]==0xfffe) /* exit */
    { ld->exitcube=n; ld->exitwall=j;
      n->d.c->nextcubes[j]=0xffff; }
  if(i==lfh.secret_cubenum) ld->secretcube=n;
  }
 for(n=ld->cubes.head;n->next!=NULL;n=n->next)
  if(!D2_REG_readcube2(lf,n)) return 1;
 fseek(lf,lfh.gamedata_offset,SEEK_SET);
 if(fread(&gd,sizeof(struct D2_gamedata),1,lf)!=1)
  { waitmsg(TXT_CANTREADLVL); killalldoors(ld); return 1; }
 i=0;
 while(isprint(buffer[i++]=fgetc(lf)) && i<31);
 buffer[i-1]=0; FREE(ld->fullname);
 checkmem(ld->fullname=MALLOC(strlen(buffer)+1));
 strcpy(ld->fullname,buffer);
 fseek(lf,gd.posplayer,SEEK_SET);
 sizeplayer=gd.sizeplayer;
 checkmem(playerdata=REALLOC(playerdata,sizeplayer));
 if(fread(playerdata,sizeplayer,1,lf)!=1)
  { killalldoors(ld); return 1; }
 fseek(lf,gd.posthings,SEEK_SET);
 if(!readlist(lf,&ld->things,readthing,gd.numthings)) 
  { killalldoors(ld); return 1; }
 fseek(lf,gd.posdoors,SEEK_SET);
 if(!readlist(lf,&ld->doors,readdoor,gd.numdoors)) 
  { killalldoors(ld); return 1; }
 fseek(lf,gd.possdoors,SEEK_SET);
 if(!readlist(lf,&ld->sdoors,D2_REG_readsdoor,gd.numsdoors)) 
  { killallswitches(ld); return 1; }
 fseek(lf,gd.posedoors,SEEK_SET);
 if((ld->edoors=readedoors(lf,gd.numedoors,gd.sizeedoors))==NULL)
  return 1;
 fseek(lf,gd.posproducer,SEEK_SET);
 if(!readlist(lf,&ld->producers,D2_REG_readproducer,gd.numproducer)) 
  { killallproducers(ld); return 1; }
 fseek(lf,gd.posturnoff,SEEK_SET);
 if(gd.numturnoff>0)
  {
  checkmem(turnoffs=MALLOC(sizeof(struct turnoff)*gd.numturnoff));
  for(j=0;j<gd.numturnoff;j++)
   if((to=readturnoff(lf))!=NULL) { turnoffs[j]=*to; FREE(to); }
   else { FREE(turnoffs); return 1; }
  }
 else turnoffs=NULL;
 if(gd.numchangedlight>0)
  {
  checkmem(changedlights=MALLOC(sizeof(struct changedlight)*
   gd.numchangedlight));
  for(j=0;j<gd.numchangedlight;j++)
   if((cl=readchangedlight(lf))!=NULL) { changedlights[j]=*cl; FREE(cl); }
   else { FREE(changedlights); FREE(turnoffs); return 1; }
  }
 else changedlights=NULL;
 fclose(lf);
 ld->levelillum=(gd.numturnoff>0);
 if(turnoffs!=NULL)
  makelightsources(ld,turnoffs,gd.numturnoff,changedlights,
   gd.numchangedlight,fl_lights,lfh.flickering_lights);
 if(turnoffs!=NULL) FREE(turnoffs);
 if(changedlights!=NULL) FREE(changedlights);
 if(fl_lights!=NULL) FREE(fl_lights);
 return 1; 
 }
 
int readlvldata(char *filename,struct leveldata *ld)
 {
 FILE *lf;
 struct fileheadversion fhv;
 if(filename==NULL || ld==NULL) return 0;
 if(ld->filename) FREE(ld->filename);
 checkmem(ld->filename=MALLOC(strlen(filename)+1));
 strcpy(ld->filename,filename);
 if((lf=fopen(ld->filename,"rb"))==NULL) return 0;
 if(fread(&fhv,sizeof(struct fileheadversion),1,lf)!=1)
  { fclose(lf); return 0; }
 if(strncmp(fhv.lvlp,"LVLP",4)) { fclose(lf); return 0; }
 switch(fhv.version)
  {
  case LEVVER_D2_10_REG: case LEVVER_D2_11_REG: 
   loading_level_version=fhv.version;
   return D2_REG_readlvldata(lf,ld,fhv.version); 
  case LEVVER_D2_12_REG:
   waitmsg("Can't read levels from the Vertigo series.");
   return 0; 
  case LEVVER_D1_REG: loading_level_version=d1_14_reg;
   return D1_REG_readlvldata(lf,ld,fhv.version);
  default: 
   waitmsg("Unknown level version.");
   return 0;
  }
 return 0;
 }
 
int readasciilevel(char *filename,struct leveldata *ld)
 {
 FILE *lf;
 char buffer[256];
 char const *palname;
 struct node *n;
 int i;
 if(filename==NULL || ld==NULL) return 0;
 if(ld->filename) FREE(ld->filename);
 ld->filename=NULL;
 checkmem(ld->fullname=MALLOC(strlen("ASCII level")+1));
 strcpy(ld->fullname,"ASCII level");
 palname=init.d_ver>=d2_10_sw ? "groupa.256" : "descent.256";
 checkmem(ld->pigname=MALLOC(strlen(palname)+1));
 strcpy(ld->pigname,palname);
 if((lf=fopen(filename,"r"))==NULL) return 0;
 if(fscanf(lf," %255s",buffer)==NULL || strcmp(buffer,"DMB_BLOCK_FILE")!=0)
  { waitmsg(TXT_WRONGBLKHEAD,filename,buffer); return 0; }
 while(readasciicube(ld,lf));
 fclose(lf);
 for(n=ld->cubes.head;n->next!=NULL;n=n->next)
  for(i=0;i<6;i++)
   if(n->d.c->nextcubes[i]>ld->cubes.size) n->d.c->nextcubes[i]=0xffff;
 initlevel(ld);
 return 1;
 }
 
int initlevel(struct leveldata *ld)
 {
 struct node *n,*n2;
 int i,found_start;
 struct thing *t;
 struct leveldata *oldl;
 oldl=l; l=ld;
 for(n=ld->cubes.head;n->next!=NULL;n=n->next)
  {
  if(n->no%10==0) printmsg(TXT_INITCUBE,n->no,ld->cubes.size); 
  switch(initcube(n))
   { case 0: n=n->prev; freenode(&ld->cubes,n->next,freecube); break;
     case -1: return 0; }
  }
 for(n=ld->doors.head;n->next!=NULL;n=n->next)
  {
  if(n->no%10==0) printmsg(TXT_INITDOOR,n->no,ld->doors.size);
  if(!initdoor(n)) { n=n->prev; freenode(&ld->doors,n->next,freedoor); }
  }
 if(init.d_ver>=d2_10_sw)
  {
  for(n=ld->sdoors.head;n->next!=NULL;n=n->next)
   if(n->d.sd->type==switch_secretexit) break;
  if(n->next==NULL) ld->secretcube=NULL;
  else
   {
   if(ld->secretcube==NULL) /* D1 level */
    ld->secretcube=n->d.sd->d->d.d->c;
   my_assert(ld->secretcube!=NULL);
   makesecretstart(ld,n->d.sd->d,ld->secretcube);
   }
  }
 if(init.d_ver>=d2_11_reg)
  for(n=ld->lightsources.head;n->next!=NULL;n=n->next)
   if(n->d.ls->fl!=NULL)
    for(n2=n->d.ls->effects.head;n2->next!=NULL;n2=n2->next)
     checkmem(addnode(&n2->d.lse->cube->d.c->fl_lights,-1,n->d.ls->fl));
 for(i=0;i<(ld->edoors ? ld->edoors->num : 0);i++)
  {
  if((n=findnode(&ld->cubes,ld->edoors->cubes[i]))==NULL)
   { waitmsg(TXT_ERROPENATEND,ld->edoors->cubes[i]); continue; }
  if(n->d.c->d[ld->edoors->walls[i]]==NULL)
   { waitmsg(TXT_ERROAENODOOR,ld->edoors->cubes[i],ld->edoors->walls[i]);
     continue; }
  n->d.c->d[ld->edoors->walls[i]]->d.d->edoor=1;
  }
 for(n=ld->things.head,found_start=0;n->next!=NULL;n=n->next)
  {
  t=n->d.t;
  if(t->type1==tt1_dmstart && t->type2==0) /* starting place */
   {
   found_start=1;
   for(i=0;i<3;i++)
    {
    ld->e0.x[i]=t->p[0].x[i]; 
    ld->e[0].x[i]=t->orientation[i]; ld->e[1].x[i]=t->orientation[i+3];
    }
   normalize(&ld->e[0]); normalize(&ld->e[1]);
   VECTOR(&ld->e[2],&ld->e[0],&ld->e[1]);
   }
  setthingcube(t);
  }
 ld->currwall=ld->curredge=0;
 ld->pcurrpnt=(ld->pts.size>0) ? ld->pts.head : NULL;
 ld->pcurrcube=(ld->cubes.size>0) ? ld->cubes.head : NULL;
 ld->pcurrthing=(ld->things.size>0) ? ld->things.head : NULL;
 ld->pcurrwall=(ld->pcurrcube!=NULL) ? ld->pcurrcube->d.c->walls[0] : NULL;
 ld->pcurrdoor=(ld->doors.size>0) ? ld->doors.head : NULL;
 if(!found_start && ld->pcurrcube!=NULL) /* no starting place found */
  {
  for(i=0;i<3;i++) 
   {
   ld->e0.x[i]=ld->pcurrcube->d.c->p[0]->d.p->x[i];
   ld->e[0].x[i]=ld->pcurrcube->d.c->p[1]->d.p->x[i]-ld->e0.x[i];
   ld->e[1].x[i]=ld->pcurrcube->d.c->p[3]->d.p->x[i]-ld->e0.x[i];
   }
  normalize(&ld->e[0]); normalize(&ld->e[1]);
  VECTOR(&ld->e[2],&ld->e[0],&ld->e[1]);
  }
 ld->rendercube=NULL; ld->inside=0;
 for(n=ld->producers.head->next;n!=NULL;n=n->next)
  if(n->prev->d.cp->c==NULL) killnode(&ld->producers,n->prev);
 l=oldl;
 for(i=0;i<NUM_SAVED_POS;i++)
  { ld->saved_pos[i].e0=ld->e0;
    ld->saved_pos[i].e[0]=ld->e[0];
    ld->saved_pos[i].e[1]=ld->e[1];
    ld->saved_pos[i].e[2]=ld->e[2];
    ld->saved_pos[i].distcenter=view.distcenter;
    ld->saved_pos[i].max_vis=view.maxvisibility; }
 return 1;
 }
 
/* change all internal data to the level ld but draw nothing */
void in_changecurrentlevel(struct leveldata *ld)
 {
 int i;
 if(l==ld) return;
 if(l!=NULL)
  {
  l->pcurrcube=view.pcurrcube; l->pcurrthing=view.pcurrthing;
  l->pcurrdoor=view.pcurrdoor; l->pcurrwall=view.pcurrwall;
  l->pcurrpnt=view.pcurrpnt;
  l->curredge=view.curredge; l->currwall=view.currwall;
  l->e0=view.e0; for(i=0;i<3;i++) l->e[i]=view.e[i];
  render_resetlights(l);
  }
 if(ld!=NULL)
  {
  view.pcurrcube=ld->pcurrcube; view.pcurrthing=ld->pcurrthing;
  view.pcurrdoor=ld->pcurrdoor; view.pcurrwall=ld->pcurrwall;
  view.pcurrpnt=ld->pcurrpnt;
  view.curredge=ld->curredge; view.currwall=ld->currwall;
  view.e0=ld->e0; for(i=0;i<3;i++) view.e[i]=ld->e[i];
  }
 else
  {
  view.pcurrthing=view.pcurrcube=view.pcurrdoor=view.pcurrpnt=NULL;
  view.pcurrwall=NULL;
  view.curredge=view.currwall=0; 
  }
 l=ld;
 if(l) init_rendercube(); 
 }
 
void b_plotlevel(struct w_window *w,void *d)
 {
 struct leveldata *ld,*lw=d;
 my_assert(lw!=NULL && w!=NULL);
 /* needed when window is opened the first time: */
 lw->w=w; ld=l; in_changecurrentlevel(lw); plotlevel();
 in_changecurrentlevel(ld);
 }
void b_closelevel(struct w_window *w,void *d)
 { closelevel(d,1); plotlevel(); }
void b_clicklevel(struct w_window *w,void *d,struct w_event *we)
 { 
 struct leveldata *ld=d;
 my_assert(d!=NULL && we!=NULL && we->ws!=NULL)
 if(ld!=l)
  { if(we->ws->buttons==ws_bt_none) changecurrentlevel(ld);
    return; }
 if(we->x<0 || we->y<0 || we->x>=w_xwininsize(w) || we->y>=w_ywininsize(w))
  return; /* click on a system button */
 click_in_level(w,we);
 }

struct leveldata *emptylevel(void)
 {
 struct leveldata *ld;
 int i;
 if((ld=MALLOC(sizeof(struct leveldata)))==NULL) return NULL;
 for(i=0;i<tt_number;i++) initlist(&ld->tagged[i]);
 initlist(&ld->cubes); initlist(&ld->things);
 initlist(&ld->doors); initlist(&ld->pts);
 initlist(&ld->sdoors); initlist(&ld->producers);
 initlist(&ld->lines); 
 initlist(&ld->lightsources);
 checkmem(ld->edoors=MALLOC(sizeof(struct edoor)));
 ld->edoors->num=0;
 for(i=0;i<10;i++) { ld->edoors->cubes[i]=ld->edoors->walls[i]=0; }
 ld->fullname=ld->filename=NULL;
 if(l)
  { checkmem(ld->pigname=MALLOC(strlen(l->pigname)+1));
    strcpy(ld->pigname,l->pigname); }
 else ld->pigname=NULL;
 ld->w=NULL; ld->exitcube=NULL; ld->exitwall=0;
 ld->reactor_time=0x1e; ld->reactor_strength=0xffffffff;
 ld->levelsaved=1; ld->levelillum=0; ld->secretcube=ld->secretstart=NULL; 
 for(i=0;i<9;i++) ld->secret_orient[i]=stdorientation[i];
 ld->pcurrpnt=ld->pcurrcube=ld->pcurrthing=ld->pcurrdoor=ld->rendercube=NULL;
 ld->inside=0; ld->pcurrwall=NULL;
 ld->curredge=ld->currwall=0; ld->n=NULL;
 ld->whichdisplay=view.whichdisplay; ld->cur_corr=NULL;
 for(i=0;i<3;i++) 
  { ld->e0.x[i]=i==2 ? -655360.0 : 0.0;
    ld->e[0].x[i]=i==0 ? 1.0 : 0.0;
    ld->e[1].x[i]=i==1 ? 1.0 : 0.0;
    ld->e[2].x[i]=i==2 ? 1.0 : 0.0; }
 for(i=0;i<NUM_SAVED_POS;i++)
  { ld->saved_pos[i].e0=ld->e0;
    ld->saved_pos[i].e[0]=ld->e[0];
    ld->saved_pos[i].e[1]=ld->e[1];
    ld->saved_pos[i].e[2]=ld->e[2];
    ld->saved_pos[i].distcenter=view.distcenter;
    ld->saved_pos[i].max_vis=view.maxvisibility; }
 return ld;
 }

struct leveldata *readdbbfile(char *filename)
 {
 struct leveldata *ld;
 checkmem(ld=emptylevel());
 if(!readasciilevel(filename,ld) || !initlevel(ld)) 
  { closelevel(ld,0); return NULL; }
 return ld;
 }

struct leveldata *readlevel(char *filename)
 {
 struct leveldata *ld;
 char *pogfilename;
 /* char *realfilename*/
 checkmem(ld=emptylevel());
 if(!readlvldata(filename,ld) || !initlevel(ld)) 
  { closelevel(ld,0); return NULL; }
 if(init.d_ver>=d2_12_reg && !pig.pogfile)
  {
  /*
  realfilename=strrchr(filename,'/');
  if(!realfilename) realfilename=filename;
  checkmem(pogfilename=MALLOC(strlen(init.pogpath)+strlen(realfilename)+2));
  strcpy(pogfilename,init.pogpath);
  strcat(pogfilename,"/");
  strcat(pogfilename,realfilename);
  strcpy(&pogfilename[strlen(pogfilename)-3],"POG");
  */
  {
    char d[MAXDRIVE], p[MAXDIR], f[MAXFILE], e[MAXEXT];
    fnsplit(filename, d, p, f, e);
    checkmem(pogfilename=MALLOC(strlen(filename)+1));
    fnmerge(pogfilename, d, p, f, ".pog");
  }
  printmsg(TXT_LOOKINGFORPOGFILE,pogfilename);
  changepogfile(pogfilename); FREE(pogfilename);
  }
  if((init.d_ver==d1_10_sw) || (d1_10_reg) || (d1_14_reg))
  {
    char d[MAXDRIVE], p[MAXDIR], f[MAXFILE], e[MAXEXT];
    char pg1filename[MAXPATH];
    FILE* pg1file;

    fnsplit(filename, d, p, f, e);
    fnmerge(pg1filename, d, p, f, ".pg1");
    pg1file = fopen(pg1filename,"rb");
    if(!pg1file)
    {
      fnmerge(pg1filename, d, p, f, ".dtx");
      pg1file = fopen(pg1filename,"rb");
    }
    if(!pg1file)
    {
      fnmerge(pg1filename, d, p, "devil", ".pg1");
      pg1file = fopen(pg1filename,"rb");
    }
    if(!pg1file)
    {
      fnmerge(pg1filename, d, p, "devil", ".dtx");
      pg1file = fopen(pg1filename,"rb");
    }
    if(pg1file)
      readcustomtxts(pg1file);
  }
 return ld;
 }

struct w_window plot_win={ 0,0,0,0,0,0,0,0,NULL,0,NULL,
 wb_drag|wb_size|wb_close|wb_shrink,wr_routine,NULL,b_plotlevel,
 b_closelevel,b_clicklevel };

void newlevelwin(struct leveldata *ld,int shrunk)
 {
 char buffer[256];
 int i;
 struct node *n;
 my_assert(ld!=NULL && ld->fullname!=NULL);
 for(n=view.levels.head,i=0;n->next!=NULL;n=n->next)
  if(n->d.lev->fullname && !strcmp(ld->fullname,n->d.lev->fullname)) i++;
 if(i==0) sprintf(buffer,"%s",ld->fullname);
 else sprintf(buffer,"%s / %d",ld->fullname,i);
 checkmem(ld->n=addnode(&view.levels,-1,ld));
 checkmem(view.b_levels->options=REALLOC(view.b_levels->options,
  sizeof(char *)*(++view.b_levels->num_options)));
 view.b_levels->options[view.b_levels->num_options-1]=ld->fullname;
 qsort(&view.b_levels->options[1],view.b_levels->num_options-1,sizeof(char *),
  qs_compstrs);
 plot_win.title=buffer; plot_win.data=ld;
 plot_win.maxxsize=plot_win.maxysize=-1;
 if(ld->whichdisplay)
  { plot_win.xsize=w_xmaxwinsize(); 
    plot_win.ysize=shrunk ? w_ymaxwinsize()-plot_win.xsize/2 : 
     plot_win.xsize/2; 
    plot_win.xpos=0; plot_win.ypos=shrunk ? plot_win.ysize : 0; }
 else
  {
  plot_win.xsize=plot_win.ysize=shrunk ? w_xmaxwinsize()-w_ymaxwinsize()-10 :
   w_ymaxwinsize();
  plot_win.xpos=shrunk ? w_ymaxwinsize()+5 : 0;
  plot_win.ypos=shrunk && isdigit(buffer[strlen(buffer)-1]) ?
   (((buffer[strlen(buffer)-1]-'0')%5)+in_number)*20+30 : 0;
  }
 plot_win.shrunk=shrunk;
 checkmem(ld->w=w_openwindow(&plot_win)); ld->w->data=ld;
 if(!shrunk) { changecurrentlevel(ld); drawopts(); }
 else /* macro */
  {
  for(i=1;i<view.b_macros->num_options;i++)
   if(!strcmp(view.b_macros->options[i],ld->fullname)) 
    { view.b_macros->selected=i; break; }
  drawopt(in_cube);
  }
 }
struct leveldata *openlevel(char *filename)
 {
 struct leveldata *ld;
 my_assert(filename!=NULL);
 if((ld=readlevel(filename))==NULL) return NULL;
 newlevelwin(ld,0);
 return ld;
 }
 
void changecurrentlevel(struct leveldata *ld)
 {
 int i;
 view.drawwhat|=DW_ALLLINES;
 in_changecurrentlevel(ld);
 w_curwin(ld==NULL ? NULL : ld->w);
 if(ld==view.pcurrmacro) view.pcurrmacro=NULL;
 if(ld!=NULL)
  {
  w_wintofront(ld->w);
  for(i=0;i<view.b_levels->num_options;i++)
   if(!strcmp(view.b_levels->options[i],ld->fullname))
    { view.b_levels->selected=i; break; }
  }
 else view.b_levels->selected=0;
 w_refreshstart(view.movewindow);
 w_drawbutton(view.levelbutton);
 w_refreshend(view.movewindow);
 newpigfile(ld!=NULL ? ld->pigname : pig.default_pigname,pig.pogfile);
 drawopts(); 
 }
 
int closelevel(struct leveldata *ld,int warn)
 {
 int i;
 my_assert(view.b_levels!=NULL && ld!=NULL);
 if(ld->cur_corr)
  if(warn)
   { printmsg(TXT_CORRINWORK); w_wintofront(ld->cur_corr->win); return 0; }
  else { delete_corridor(ld->cur_corr); close_corr_win(ld->cur_corr); }
 if(warn && !ld->levelsaved && !yesnomsg(TXT_DISCARDCHANGES,ld->fullname))
  return 0;
 if(view.pdeflevel && view.pdeflevel->d.lev==ld) 
  { view.pdeflevel=NULL; view.pdefcube=NULL; view.defwall=0; }
 if(view.pcurrmacro==ld)
  { view.b_macros->selected=0; view.pcurrmacro=NULL; }
 if(ld->fullname!=NULL && ld->w!=NULL)
  {
  for(i=1;i<view.b_levels->num_options;i++)
   if(!strcmp(view.b_levels->options[i],ld->fullname)) break;
  if(view.b_macros->selected==i) view.b_macros->selected=0;
  else if(view.b_macros->selected>i) view.b_macros->selected--;
  for(i+=1;i<view.b_levels->num_options;i++)
   view.b_levels->options[i-1]=view.b_levels->options[i];
  checkmem(view.b_levels->options=REALLOC(view.b_levels->options,
   sizeof(char *)*(--view.b_levels->num_options+1)));
  if(ld==l) changecurrentlevel(NULL);
  if(ld->n) killnode(&view.levels,ld->n);
  }
 for(i=0;i<tt_number;i++) freelist(&ld->tagged[i],free);
 freelist(&ld->lightsources,freelightsource);
 freelist(&ld->lines,free);
 freelist(&ld->pts,free);
 freelist(&ld->cubes,freecube);
 freelist(&ld->things,free);
 freelist(&ld->doors,freedoor);
 freelist(&ld->sdoors,free);
 freelist(&ld->producers,free);
 FREE(ld->edoors); FREE(ld->fullname); FREE(ld->filename); FREE(ld->pigname);
 if(ld->w)
  {
  w_closewindow(ld->w);
  ld->w=NULL;
  drawopt(in_cube);
  /* clear the complete texture cache.  */
  for(i=0;i<pig.num_pigtxts;i++)
   if(pig.pig_txts[i].data!=NULL) 
    { FREE(pig.pig_txts[i].data); pig.pig_txts[i].data=NULL; }
  }
 return 1;
 }
 
int makedoors(struct leveldata *ld)
 {
 struct node *n;
 int i,ok;
 ld->edoors->num=0;
 for(n=ld->doors.head;n->next!=NULL;n=n->next)
  {
  if(n->d.d->edoor)
   {
   if(ld->edoors->num==10)
    if(!yesnomsg(TXT_TOOMANYEDOORS)) return 0;
   ld->edoors->cubes[ld->edoors->num]=n->d.d->c->no;
   ld->edoors->walls[ld->edoors->num++]=n->d.d->wallnum; 
   }
  n->d.d->sdoor=n->d.d->sd!=NULL ? n->d.d->sd->no : 0xff;
  }
 for(n=ld->sdoors.head->next;n!=NULL;n=n->next)
  {
  ok=1;
  if(n->prev->d.sd->d!=NULL)
   switch(getsdoortype(n->prev->d.sd))
    {
    case sdtype_none: n->prev->d.sd->num=0; break;
    case sdtype_door: 
     if(n->prev->d.sd->num==0) ok=0;
     for(i=0;i<n->prev->d.sd->num;i++)
      { n->prev->d.sd->cubes[i]=n->prev->d.sd->target[i]->d.d->c->no;
        n->prev->d.sd->walls[i]=n->prev->d.sd->target[i]->d.d->wallnum; }
     break;
    case sdtype_cube: case sdtype_side:
     if(n->prev->d.sd->num==0) ok=0;
     for(i=0;i<n->prev->d.sd->num;i++)
      n->prev->d.sd->cubes[i]=n->prev->d.sd->target[i]->no; 
     break;
    default: my_assert(0);
    }
  else ok=0;
  if(!ok) deletesdoor(n->prev);
  }
 for(n=ld->producers.head;n->next!=NULL;n=n->next)
  n->d.cp->cubenum=n->d.cp->c->no; 
 return 1;
 }
 
/* make level ready for save. if this is not possible, return 0 */
int checklvl(struct leveldata *ld,int testlevel,struct list *turnoffs,
 struct list *changedlights)
 {
 struct node *n,*nsd,*c,*starts[12],*keys[3],*flags[2],*reactor=NULL;
 void *data;
 int i,no,no_coop_starts,secret=-1;
 sortlist(&ld->cubes,0); /* so we get no higher numbers than size */
 sortlist(&ld->things,0); sortlist(&ld->doors,0);
 sortlist(&ld->pts,0); sortlist(&ld->producers,0); sortlist(&ld->sdoors,0);
 sortlist(&ld->lightsources,0);
 for(n=ld->cubes.head,i=0,no=0;n->next!=NULL;n=n->next) 
  { if(n->d.c->cp!=NULL) 
     { n->d.c->value=no++;
       n->d.c->cp->prev->next=n->d.c->cp->next;
       n->d.c->cp->next->prev=n->d.c->cp->prev;
       n->d.c->cp->next=ld->producers.tail->next;
       n->d.c->cp->prev=ld->producers.tail;
       n->d.c->cp->prev->next=n->d.c->cp;
       n->d.c->cp->next->prev=n->d.c->cp; }
    if(n->d.c->type!=0) i++; }
 if(testlevel<0) return 1;
 if(adjust_light_nc!=NULL)
  {
  printmsg(TXT_LEAVEADJLIGHTMODE,adjust_light_nc->no,adjust_light_wall);
  return 0;
  }
 if(ld->cubes.size>MAX_DESCENT_CUBES && !yesnomsg(TXT_TOOMANYCUBES,
  ld->cubes.size,MAX_DESCENT_CUBES)) return 0;
 if(ld->pts.size>MAX_DESCENT_VERTICES && !yesnomsg(TXT_TOOMANYPTS,
  ld->pts.size,MAX_DESCENT_VERTICES)) return 0;
 if(ld->things.size>MAX_DESCENT_OBJECTS && !yesnomsg(TXT_TOOMANYTHINGS,
  ld->things.size,MAX_DESCENT_OBJECTS))
  return 0;
 if(ld->doors.size>MAX_DESCENT_WALLS && !yesnomsg(TXT_TOOMANYWALLS,
  ld->doors.size,MAX_DESCENT_WALLS)) return 0;
 if(ld->sdoors.size>MAX_DESCENT_SWITCHES && !yesnomsg(TXT_TOOMANYSWITCHES,
  ld->sdoors.size,MAX_DESCENT_SWITCHES)) return 0;
 if(ld->producers.size>MAX_DESCENT_RCENTERS && !yesnomsg(TXT_TOOMANYRCENTERS,
  ld->producers.size,MAX_DESCENT_RCENTERS))
  return 0;
 if(init.d_ver>=d2_10_sw)
  {
  if(turnoffs->size>MAX_DESCENT_TURNOFFS && !yesnomsg(TXT_TOOMANYTURNOFFS,
   turnoffs->size,MAX_DESCENT_TURNOFFS))
   return 0;
  if(changedlights->size>MAX_DESCENT_CHANGEDLIGHTS &&
   !yesnomsg(TXT_TOOMANYCHANGEDLIGHTS,changedlights->size,
   MAX_DESCENT_CHANGEDLIGHTS))
   return 0;
  }
 flags[0]=flags[1]=NULL;
 for(i=0;i<3;i++) keys[i]=NULL; 
 for(i=0;i<11;i++) starts[i]=NULL; 
 no_coop_starts=0;
 if(testlevel)
  {
  my_assert(l==ld);
  for(n=ld->things.head;n->next!=NULL;n=n->next)
   {
   for(i=0;i<3;i++) n->d.t->pos[i]=n->d.t->p[0].x[i];
   if((c=findpntcube(&ld->cubes,&n->d.t->p[0]))==NULL)
    { view.pcurrthing=n;
      if(view.warn_thingoutofbounds && !yesnomsg(TXT_THINGOUTOFBOUNDS,n->no))
       return 0;
      else c=ld->cubes.head; }
   switch(n->d.t->type1)
    {
    case tt1_secretstart:
     if(secret>=0) { waitmsg(TXT_ONLYONESECRETSTART,secret); break; }
     for(nsd=l->sdoors.head;nsd->next!=NULL;nsd=nsd->next)
      if(nsd->d.sd->type==switch_secretexit) break;
     if(nsd->next==NULL) waitmsg(TXT_NOSECRETDOORFORSTART);
     secret=n->no;
     ld->secretcube=c; 
     for(i=0;i<9;i++) ld->secret_orient[i]=n->d.t->orientation[i]; 
     break;
    case tt1_dmstart: 
     if(n->d.t->type2>7)
      { if(!yesnomsg(TXT_ILLNOFORSTART,(int)n->d.t->type2,n->no)) return 0; }
     else
      {
      if(starts[n->d.t->type2]==NULL) starts[n->d.t->type2]=n;
      else
       {
       view.pcurrthing=n;
       if(!yesnomsg(TXT_SAMENOFORSTART,n->d.t->type2,
        starts[n->d.t->type2]->no,n->no)) return 0;
       }
      }
     break;
    case tt1_coopstart:
     if(++no_coop_starts>4 && !yesnomsg(TXT_TOOMANYCOOPSTARTS,n->no))
      return 0;
     if(no_coop_starts<=4) starts[7+no_coop_starts]=n;
     n->d.t->type2=no_coop_starts+7;
     break;
    case tt1_item:
     if(n->d.t->type2==item_id_bluekey || n->d.t->type2==item_id_redkey ||
      n->d.t->type2==item_id_yellowkey)
      {
      if(keys[n->d.t->type2-item_id_bluekey]==NULL) 
       keys[n->d.t->type2-item_id_bluekey]=n;
      else
       {
       view.pcurrthing=n;
       if(view.warn_doublekeys) 
        waitmsg(TXT_TWOKEYSOFSAMECOLOR,
	 keys[n->d.t->type2-item_id_bluekey]->no,n->no);
       }       
      }
     else if(n->d.t->type2==item_id_blueflag || 
      n->d.t->type2==item_id_redflag)
      {
      if(flags[n->d.t->type2-item_id_blueflag]==NULL)
       flags[n->d.t->type2-item_id_blueflag]=n;
      else
       {
       view.pcurrthing=n;
       if(view.warn_tworeactors)
        waitmsg(TXT_TWOREACTORS,flags[n->d.t->type2-item_id_blueflag]->no,
	 n->no);
       }
      }
     break;
    case tt1_reactor:
     if(reactor==NULL) reactor=n;
     else if(view.warn_tworeactors) 
      waitmsg(TXT_TWOREACTORS,n->no,reactor->no);
     c->d.c->type=cube_reactor;
     break;
    }
   n->d.t->cube=c->no;
   }
  if(starts[0]==NULL)
   { if(!yesnomsg(TXT_NOSTART)) return 0; }
  else
   {
   starts[0]->d.t->control=ct_slew;
   for(i=10;i>=0;i--)
    if(starts[i]!=NULL)
     {
     data=starts[i]->d.v; no=starts[i]->no;
     freenode(&ld->things,starts[i],NULL); 
     checkmem(addheadnode(&ld->things,no,data));
     }
   }
  }
 /* search if there's a secret exit */
 for(n=ld->sdoors.head;n->next!=NULL;n=n->next)
  if(n->d.sd->type==switch_secretexit) break;
 if(n->next && secret<0 && !yesnomsg(TXT_NOSECRETSTART,n->d.sd->d->no)) 
  return 0;
 if(n->next==NULL && secret>=0 && !yesnomsg(TXT_NOSECRETEXIT,secret))
  return 0;
 /* empty pnts shouldn't exist but if reading level is not completed... */
 for(n=ld->pts.head->next;n!=NULL;n=n->next) /* search for not-used pnts. */
  if(n->prev->d.lp->c.size==0) 
   freenode(&ld->pts,n->prev,freelistpnt);
 sortlist(&ld->pts,0);
 return 1;
 }
 
int D1_REG_savelevel(FILE *f,struct leveldata *ld)
 {
 struct D1_gamedata gd=D1_stdgamedata;
 struct D1_minedata md;
 struct D1_REG_levelfilehead lh=D1_REG_stdlevelfilehead;
 md.numcubes=ld->cubes.size; md.numpts=ld->pts.size;
 if(fwrite(&lh,sizeof(struct D1_REG_levelfilehead),1,f)!=1)
  { fclose(f); return 0; }
 lh.minedata_offset=ftell(f); md.version=0;
 if(fwrite(&md,sizeof(struct D1_minedata),1,f)!=1)
  { fclose(f); return 0; }
 /* now write all points */
 if(!savelist(f,&ld->pts,savepoint,0)) { fclose(f); return 0; }
 /* and cubes */
 if(!savelist(f,&ld->cubes,D1_REG_savecube,0,ld->exitcube,(int)ld->exitwall))
  { fclose(f); return 0; }
 lh.gamedata_offset=ftell(f);
 if(ld->secretstart) unlistnode(&ld->things,ld->secretstart);
 gd.numthings=ld->things.size; gd.numdoors=ld->doors.size;
 gd.numsdoors=ld->sdoors.size; gd.numproducer=ld->producers.size;
 gd.numedoors=1; 
 if(sizeplayer==0)
  { sizeplayer=D1_stdgamedata.sizeplayer;
    checkmem(playerdata=CALLOC(gd.sizeplayer,1));
    strcpy(playerdata,"GUILE"); }
 gd.sizeplayer=sizeplayer;
 if(fwrite(&gd,sizeof(struct D1_gamedata),1,f)!=1)
  { fclose(f); return 0; }
 if(strlen(ld->fullname)>30) ld->fullname[30]=0;
 if(fwrite(ld->fullname,1,strlen(ld->fullname),f)!=strlen(ld->fullname))
  { fclose(f); return 0; }
 if(fwrite("\0",1,1,f)!=1) { fclose(f); return 0; }
 gd.posplayer=ftell(f); 
 if(fwrite(playerdata,gd.sizeplayer,1,f)!=1)
  { fclose(f); return 0; }
 gd.posthings=ftell(f);
 if(!savelist(f,&ld->things,savething,0))
  { fclose(f); return 0; }
 if(ld->secretstart)
  {
  ld->secretstart->next=ld->things.tail->next;
  ld->secretstart->prev=ld->things.tail;
  ld->things.tail->next=ld->secretstart;
  ld->things.tail=ld->secretstart;
  }
 if(!makedoors(ld)) { fclose(f); return 0;  }
 gd.posdoors=ftell(f);
 if(!savelist(f,&ld->doors,savedoor,0)) { fclose(f); return 0; }
 gd.possdoors=gd.posopendoors=ftell(f);
 if(!savelist(f,&ld->sdoors,D1_REG_savesdoor,0))
  { fclose(f); return 0; }
 gd.posedoors=ftell(f);
 if(fwrite(ld->edoors,sizeof(struct edoor),gd.numedoors,f)!=gd.numedoors)
  { fclose(f); return 0; }
 gd.posproducer=ftell(f);
 if(!savelist(f,&ld->producers,D1_REG_saveproducer,0))
  { fclose(f); return 0; }
 fseek(f,lh.gamedata_offset,SEEK_SET);
 if(fwrite(&gd,sizeof(struct D1_gamedata),1,f)!=1)
  { fclose(f); return 0; }
 fseek(f,0,SEEK_SET);
 if(fwrite(&lh,sizeof(struct D1_REG_levelfilehead),1,f)!=1)
  { fclose(f); return 0; }
 fclose(f);
 return 1;
 }

#include "pofdata.h" 

void createturnoff(struct leveldata *ld,struct lightsource *ls,
 struct list *turnoffs,struct list *changedlights,int notall)
 {
 struct turnoff *to;
 struct changedlight *lc;
 struct node *n;
 int i,j;
 i=ls->cube->d.c->walls[(int)ls->w]->texture1;
 j=ls->cube->d.c->walls[(int)ls->w]->texture2;
 if(notall && (i<0 || i>=pig.num_rdltxts || j<0 || j>=pig.num_rdltxts || 
  (pig.rdl_txts[i].shoot_out_txt<0 && pig.rdl_txts[j].shoot_out_txt<0
   && pig.rdl_txts[i].anim_seq<0 && pig.rdl_txts[j].anim_seq<0)))
  {
  /* is the light a flickering light ? */
  j=ls->fl!=NULL ? 1 : 0;
  /* now check if the light is turned out by a switch */
  for(n=ld->sdoors.head,j=0;n->next!=NULL && !j;n=n->next)
   if(n->d.sd->type==switch_turnofflight || n->d.sd->type==switch_turnonlight)
    for(i=0;i<n->d.sd->num && !j;i++)
     if(n->d.sd->target[i]==ls->cube && (int)n->d.sd->walls[i]==ls->w) j=1;
  if(!j) return;
  }
 checkmem(to=MALLOC(sizeof(struct turnoff)));
 to->cube=ls->cube->no; to->side=ls->w;
 sortlist(&ls->effects,0);
 to->offset=changedlights->size;
 checkmem(addnode(turnoffs,-1,to));
 for(n=ls->effects.head;n->next!=NULL;n=n->next)
  {
  for(i=0;i<6;i++)
   if((n->d.lse->add_light[i*4]>>10)+(n->d.lse->add_light[i*4+1]>>10)+
    (n->d.lse->add_light[i*4+2]>>10)+(n->d.lse->add_light[i*4+3]>>10)>theMinDeltaLight)
    {
    checkmem(lc=MALLOC(sizeof(struct changedlight)));
    lc->cube=n->d.lse->cube->no; lc->side=i;
    lc->stuff=0; 
    for(j=0;j<4;j++) lc->sub[j]=n->d.lse->add_light[i*4+j]>>10;
    checkmem(addnode(changedlights,-1,lc));
    }
  }
 to->num_changed=changedlights->size-to->offset;
 }
 
void makelights(struct leveldata *ld,struct list *turnoffs,
 struct list *changedlights,struct list *fl_lights,int notall)
 {
 struct node *ntc;
 /* OK, now create the fl_lights and the turnoffs */
 for(ntc=ld->lightsources.head;ntc->next!=NULL;ntc=ntc->next)
  {
  if(init.d_ver>=d2_10_sw) createturnoff(ld,ntc->d.ls,turnoffs,
   changedlights,notall);
  if(ntc->d.ls->fl) checkmem(addnode(fl_lights,-1,ntc->d.ls->fl));
  }
 }

int D2_REG_savelevel(FILE *f,struct leveldata *ld,struct list *turnoffs,
 struct list *changedlights,struct list *fl_lights)
 {
 struct D2_gamedata gd=D2_stdgamedata;
 struct D2_minedata md;
 struct D2_REG_levelfilehead lh=D2_REG_stdlevelfilehead;
 int i,j;
 if(init.d_ver<d2_11_reg) 
  { 
  lh.fh.version=LEVVER_D2_10_REG;
  if(fl_lights->size>0 &&
   !yesnomsg("WARNING: If you save this level for Descent 2 V1.0,\n"\
    "you will loose all data about flickering lights. Continue?")) return 0;
  }
 md.numcubes=ld->cubes.size; md.numpts=ld->pts.size; md.version=0;
 if(fwrite(&lh,sizeof(struct fileheadversion)+sizeof(unsigned long)*2,1,f)!=1)
  { fclose(f); return 0; }
 if(fwrite(ld->pigname,strlen(ld->pigname),1,f)!=1)
  { fclose(f); return 0; }
 if(fwrite("\x0A",1,1,f)!=1) { fclose(f); return 0; }
 lh.reactor_time=ld->reactor_time;
 lh.reactor_strength=ld->reactor_strength;
 if(fwrite(&lh.reactor_time,sizeof(unsigned long),2,f)!=2)
  { fclose(f); return 0; }
 if(lh.fh.version>=LEVVER_D2_11_REG)
  {
  lh.flickering_lights=fl_lights->size;
  if(fwrite(&lh.flickering_lights,sizeof(unsigned long),1,f)!=1)
   { fclose(f); return 0; }
  if(lh.flickering_lights>0)
   if(!savelist(f,fl_lights,saveflickeringlight,0))
    { fclose(f); return 0; }   
  }
 lh.secret_cubenum=ld->secretcube==NULL ? 0 : ld->secretcube->no;
 for(i=0;i<3;i++)
  {
  lh.secret_orient[i]=ld->secret_orient[i];
  lh.secret_orient[i+3]=ld->secret_orient[i+6];
  lh.secret_orient[i+6]=ld->secret_orient[i+3];
  }
 if(fwrite(&lh.secret_cubenum,sizeof(unsigned long),10,f)!=10)
  { fclose(f); return 0; }
 lh.minedata_offset=ftell(f);
 if(fwrite(&md,sizeof(struct D2_minedata),1,f)!=1)
  { fclose(f); return 0; }
 /* now write all points */
 if(!savelist(f,&ld->pts,savepoint,0)) { fclose(f); return 0; }
 /* and cubes */
 if(!savelist(f,&ld->cubes,D2_REG_savecube,0,ld->exitcube,(int)ld->exitwall))
  { fclose(f); return 0; }
 if(!savelist(f,&ld->cubes,D2_REG_savecube2,0))
  { fclose(f); return 0; }
 lh.gamedata_offset=ftell(f);
 if(ld->secretstart) unlistnode(&ld->things,ld->secretstart);
 gd.numthings=ld->things.size; gd.numdoors=ld->doors.size;
 gd.numsdoors=ld->sdoors.size; gd.numproducer=ld->producers.size;
 gd.numturnoff=turnoffs->size; gd.numchangedlight=changedlights->size;
 gd.numedoors=1;
 if(sizeplayer==0)
  { sizeplayer=D2_stdgamedata.sizeplayer;
    checkmem(playerdata=CALLOC(gd.sizeplayer,1));
    strcpy(playerdata,"SPIKE"); }
 gd.sizeplayer=sizeplayer;
 if(fwrite(&gd,sizeof(struct D2_gamedata),1,f)!=1)
  { fclose(f); return 0; }
 if(strlen(ld->fullname)>30) ld->fullname[30]=0;
 if(fwrite(ld->fullname,1,strlen(ld->fullname),f)!=strlen(ld->fullname))
  { fclose(f); return 0; }
 if(fwrite("\x0A",1,1,f)!=1) { fclose(f); return 0; }
 if(fwrite(&pofdatasize,2,1,f)!=1) { fclose(f); return 0; }
 for(i=0;i<pofdatasize;i++)
  {
  if(fwrite(pofdata[i],1,strlen(pofdata[i])+1,f)!=strlen(pofdata[i])+1)
   { fclose(f); return 0; }
  for(j=strlen(pofdata[i])+1;j<13;j++)
   if(fwrite("",1,1,f)!=1) { fclose(f); return 0; }
  }
 gd.posplayer=ftell(f); 
 if(fwrite(playerdata,gd.sizeplayer,1,f)!=1)
  { fclose(f); return 0; }
 gd.posthings=ftell(f);
 if(!savelist(f,&ld->things,savething,0))
  { fclose(f); return 0; }
 if(ld->secretstart)
  {
  ld->secretstart->next=ld->things.tail->next;
  ld->secretstart->prev=ld->things.tail;
  ld->things.tail->next=ld->secretstart;
  ld->things.tail=ld->secretstart;
  }
 if(!makedoors(ld)) { fclose(f); return 0;  }
 gd.posdoors=ftell(f);
 if(!savelist(f,&ld->doors,savedoor,0)) { fclose(f); return 0; }
 gd.possdoors=gd.posopendoors=ftell(f);
 if(!savelist(f,&ld->sdoors,savedata,0,ds_sdoor))
  { fclose(f); return 0; }
 gd.posedoors=ftell(f);
 if(fwrite(ld->edoors,sizeof(struct edoor),gd.numedoors,f)!=gd.numedoors)
  { fclose(f); return 0; }
 gd.posproducer=ftell(f);
 if(!savelist(f,&ld->producers,savedata,0,ds_producer))
  { fclose(f); return 0; }
 gd.posturnoff=ftell(f);
 if(!savelist(f,turnoffs,savedata,0,ds_turnoff))
  { fclose(f); return 0; }
 gd.poschangedlight=ftell(f);
 if(!savelist(f,changedlights,savedata,0,ds_changedlight))
  { fclose(f); return 0; }
 fseek(f,lh.gamedata_offset,SEEK_SET);
 if(fwrite(&gd,sizeof(struct D2_gamedata),1,f)!=1)
  { fclose(f); return 0; }
 fseek(f,0,SEEK_SET);
 if(fwrite(&lh,sizeof(struct fileheadversion)+sizeof(unsigned long)*2,1,f)!=1)
  { fclose(f); return 0; }
 fclose(f);
 return 1;
 }

int savelevel(char *fname,struct leveldata *ld,int testlevel,int changename,
 int descent_version,int notalllightinfo)
 {
 FILE *f;
 int ret;
 struct list turnoffs,changedlights,fl_lights;
 if(ld==NULL || fname==NULL) return 0;
 initlist(&turnoffs); initlist(&changedlights); initlist(&fl_lights);
 if(descent_version>=d2_10_sw)
  {
  /* check if this level should be new illuminated */
  if(testlevel>0 && ld->lightsources.size>0 && !ld->levelillum)
   if(view.warn_illuminate && yesnomsg(TXT_NEWILLUM)) 
    { tagall(tt_cube); dec_mineillum(0); }
  if(ld->levelillum>0) makelights(ld,&turnoffs,&changedlights,&fl_lights,
   notalllightinfo);
  }
 if(!checklvl(ld,testlevel,&turnoffs,&changedlights)) return 0;
 if((f=fopen(fname,"wb"))==NULL) return 0;
 switch(descent_version)
  {
  case d2_10_reg: case d2_11_reg: case d2_12_reg:
   ret=D2_REG_savelevel(f,ld,&turnoffs,&changedlights,&fl_lights);
   freelist(&turnoffs,free); freelist(&changedlights,free);
   freelist(&fl_lights,NULL); break;
  case d1_10_reg: case d1_14_reg: ret=D1_REG_savelevel(f,ld); break;
  default: return 0;
  }
 if(changename)
  {
  ld->levelsaved=1;
  if(ld->filename) FREE(ld->filename);
  checkmem(ld->filename=MALLOC(strlen(fname)+1));
  strcpy(ld->filename,fname);
  }
 return ret;
 }

