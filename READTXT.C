/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    readtxt.c - reading the texture data.
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
#include "grfx.h"
#include "userio.h"
#include "tools.h"
#include "initio.h"
#include "readtxt.h"

extern int init_test;
extern int txtoffsets[desc_number];

const char *txt_names[txt_all] = { "NormalTxt","ExtraTxt","WNormalTxt",
 "WExtraTxt","WDoors" };

struct POG_header
{
 char signature[4] NONANSI_FLAG;
 long int version NONANSI_FLAG;
 long int num_textures NONANSI_FLAG;
};
void readpogfile(FILE *f)
 {
 struct POG_header head;
 struct pig_txt pt;
 unsigned short int *texture_index,i;
 if(f==NULL) return; 
 printmsg(TXT_READINGPOGFILE);
 rewind(f);
 if(fread(&head,sizeof(struct POG_header),1,f)!=1)
  { printmsg(TXT_POGERRORINHEADER); return; }
 if(strncmp(head.signature,"DPOG",4)) 
  { printmsg(TXT_POGERRORINHEADER); return; }
 checkmem(texture_index=MALLOC(sizeof(short int)*head.num_textures));
 if(fread(texture_index,sizeof(short int),head.num_textures,f)!=
  head.num_textures)
  { FREE(texture_index); printmsg(TXT_POGERRORINHEADER); return; }
 for(i=0;i<head.num_textures;i++)
  {
  /* the following -- is very strange */
  if(--texture_index[i]>=pig.num_pigtxts)
   { waitmsg(TXT_POGINVALIDINDEX,i,texture_index[i]); continue; }
  if(fread(&pt,18,1,f)!=1)
   { waitmsg(TXT_POGFILENOTCOMPLETE,i); FREE(texture_index); return; }
  pt.f=f;
  memset(pt.name,' ',8); strncpy(pt.name,pt.rname,8); pt.name[8]=0;
  pt.xsize=pt.rlxsize+((pt.ruxsize&0x0f)<<8);
  pt.ysize=pt.rysize+((pt.ruxsize&0xf0)<<4);
  if(pt.xsize!=64 || pt.ysize!=64)
   { waitmsg(TXT_POGINVALIDSIZE,i,pt.xsize,pt.ysize); continue; }
  pt.pigno=texture_index[i];
  pt.num_anims=pt.anim_t2=0;
  pt.data=NULL; pt.offset+=head.num_textures*(18+2)+sizeof(struct POG_header);
  if(pig.pig_txts[texture_index[i]].data)
   FREE(pig.pig_txts[texture_index[i]].data);
  pig.pig_txts[texture_index[i]]=pt;
  }
 FREE(texture_index);
 }

int readcustomtxts(FILE *f);

struct D1_texture
 {
 char rname[8] NONANSI_FLAG;
 unsigned char ruxsize,rxsize NONANSI_FLAG;
 unsigned char rysize NONANSI_FLAG;
 unsigned char type1,type2 NONANSI_FLAG;
 unsigned long int offset NONANSI_FLAG;
 }; 
int readtxts(FILE *f)
 {
 struct D1_texture D1_txt;
 int i,newpig;
 union { char c[8]; unsigned long ul[2]; } head;
 unsigned long int numshort,numlong;
 if(fread(&head,sizeof(char),8,f)!=8) return 0;
 switch(init.d_ver)
  {
  case d2_10_sw: case d2_10_reg: case d2_11_reg: case d2_12_reg:
   if(strncmp(head.c,"PPIG",4)) return 0;
   if(fread(&numshort,sizeof(unsigned long),(size_t)1,f)!=1) return 0;
   numlong=0; 
   break;
  case d1_10_sw: case d1_10_reg: case d1_14_reg:
   if(head.ul[0]>0x10000)
    { /* D1 1.4 */
    fseek(f,head.ul[0],SEEK_SET);
    if(fread(&numshort,sizeof(unsigned long),(size_t)1,f)!=1) return 0;
    if(fread(&numlong,sizeof(unsigned long),(size_t)1,f)!=1) return 0;
    }
   else
    { /* D1 1.0 */ numshort=head.ul[0]; numlong=head.ul[1]; }
   break;
  default: return 0;
  }
 my_assert(pig.num_pigtxts==0 || numshort==pig.num_pigtxts);
 pig.num_pigtxts=numshort; 
 if(init_test&1) fprintf(errf,"Found %d textures.\n",pig.num_pigtxts);
 newpig=(pig.pig_txts==NULL);
 if(!newpig) /* kill the old textures */
  {
  for(i=0;i<pig.num_pigtxts;i++)
   if(pig.pig_txts[i].data) FREE(pig.pig_txts[i].data);
  FREE(pig.pig_txts);
  }
 checkmem(pig.pig_txts=MALLOC(sizeof(struct pig_txt)*pig.num_pigtxts));
 for(i=0;i<pig.num_pigtxts;i++)
  {
  if(init.d_ver>=d2_10_sw)
  { if(fread(&pig.pig_txts[i],18,1,f)!=1) return 0; }
  else
   {
   if(fread(&D1_txt,17,1,f)!=1) return 0;
   pig.pig_txts[i].rname[8]=0; 
   strncpy(pig.pig_txts[i].rname,D1_txt.rname,8);
   pig.pig_txts[i].rlxsize=D1_txt.ruxsize==0x80 ? 0x40 : D1_txt.rxsize;
   pig.pig_txts[i].ruxsize=D1_txt.ruxsize==0x80 ? 0x01 : 0;
   pig.pig_txts[i].rysize=D1_txt.rysize;
   pig.pig_txts[i].type1=D1_txt.type1;
   pig.pig_txts[i].type2=D1_txt.type2;
   pig.pig_txts[i].offset=D1_txt.offset;   
   }
  pig.pig_txts[i].f=f;
  memset(pig.pig_txts[i].name,' ',8);
  strncpy(pig.pig_txts[i].name,pig.pig_txts[i].rname,8);
  pig.pig_txts[i].name[8]=0;
  pig.pig_txts[i].xsize=pig.pig_txts[i].rlxsize+(pig.pig_txts[i].ruxsize<<8);
  pig.pig_txts[i].ysize=(unsigned short)pig.pig_txts[i].rysize;
  if(newpig)
   {
   pig.pig_txts[i].pigno=i;
   pig.pig_txts[i].num_anims=pig.pig_txts[i].anim_t2=0;
   pig.pig_txts[i].data=NULL;
   }
  if(init_test&1)
   fprintf(errf,"Found %d: %s %u %u %u %u %lx\n",i,pig.pig_txts[i].name,
    (unsigned)pig.pig_txts[i].xsize,(unsigned)pig.pig_txts[i].ysize,
    (unsigned)pig.pig_txts[i].type1,(unsigned)pig.pig_txts[i].type2,
    pig.pig_txts[i].offset);
  }
 for(i=0;i<pig.num_pigtxts;i++)
  pig.pig_txts[i].offset+=ftell(f)+20*numlong;
 return 1;
 }

/*
  Read and apply changes found in a DTX PIG patch file
*/
int readcustomtxts(FILE *f)
{
  struct D1_texture D1_txt;
  int i,j,k;
  union { char c[8]; unsigned long ul[2]; } head;
  unsigned long int numshort,numlong;
  if(fread(&head,sizeof(char),8,f)!=8)
    return 0;
  switch(init.d_ver)
  {
    case d2_10_sw: case d2_10_reg: case d2_11_reg: case d2_12_reg:
      return 0;
      break;

    case d1_10_sw:
    case d1_10_reg:
    case d1_14_reg:
      if(head.ul[0]>0x10000)
      { /* D1 1.4 */
        fseek(f,head.ul[0],SEEK_SET);
        if(fread(&numshort,sizeof(unsigned long),(size_t)1,f)!=1)
          return 0;
        if(fread(&numlong,sizeof(unsigned long),(size_t)1,f)!=1)
          return 0;
      }
      else
      { /* D1 1.0 */
        numshort=head.ul[0];
        numlong=head.ul[1];
      }
      break;

    default:
      return 0;
  }

  if((numshort<=pig.num_pigtxts) && (pig.pig_txts!=NULL))
  {
    if(init_test&1)
      fprintf(errf,"Found %ld custom textures.\n",numshort);
    for(i=0;i<numshort;i++)
    {
      char s[9];
      if(fread(&D1_txt,17,1,f)!=1)
        return 0;
      memcpy(s, D1_txt.rname,8);
      s[8] = 0;

      /* Weird construction, I know, but it seems it works with animations txts */
      for(j=0,k=0;((k < (D1_txt.ruxsize &0x0F)) || (stricmp(s, pig.pig_txts[j].rname)!=0)) && (j<pig.num_pigtxts); j++)
        if(stricmp(s, pig.pig_txts[j].rname)==0)
          k++;
      if(j < pig.num_pigtxts)
      {
        pig.pig_txts[j].rlxsize=D1_txt.ruxsize==0x80 ? 0x40 : D1_txt.rxsize;
        pig.pig_txts[j].ruxsize=D1_txt.ruxsize==0x80 ? 0x01 : 0;
        pig.pig_txts[j].rysize=D1_txt.rysize;
        pig.pig_txts[j].type1=D1_txt.type1;
        pig.pig_txts[j].type2=D1_txt.type2;
        pig.pig_txts[j].offset=D1_txt.offset+8+17*numshort+20*numlong;
        pig.pig_txts[j].xsize=pig.pig_txts[j].rlxsize+(pig.pig_txts[j].ruxsize<<8);
        pig.pig_txts[j].ysize=(unsigned short)pig.pig_txts[j].rysize;
        if(pig.pig_txts[j].data)
          FREE(pig.pig_txts[j].data);
        pig.pig_txts[j].f=f;
        /*readbitmap((char*)pig.pig_txts[j].data,&pig.pig_txts[j], NULL,1);*/
        /*pig.pig_txts[j].f=NULL;*/
      }
    }
  }
  return 1;
} 

void copytxtnums(enum txttypes dest,enum txttypes source)
 {
 int n;
 n=pig.num_txtlist[dest]+pig.num_txtlist[source];
 checkmem(pig.txtlist[dest]=REALLOC(pig.txtlist[dest],
  n*sizeof(struct ham_txt *)));
 memmove(&pig.txtlist[dest][pig.num_txtlist[dest]],
  pig.txtlist[source],pig.num_txtlist[source]*sizeof(struct ham_txt *));
 pig.num_txtlist[dest]=n;
 }
 
void inserttexture(enum txttypes dest,struct ham_txt *txt)
 {
 checkmem(pig.txtlist[dest]=REALLOC(pig.txtlist[dest],
  (++pig.num_txtlist[dest])*sizeof(struct ham_txt *)));
 pig.txtlist[dest][pig.num_txtlist[dest]-1]=txt;
 }
 
int cmp_txts(const void *t1,const void *t2)
 {
 struct ham_txt *txt1=*(struct ham_txt **)t1,*txt2=*(struct ham_txt **)t2;
 if(txt1==NULL || txt1->pig==NULL) return -1;
 if(txt2==NULL || txt2->pig==NULL) return 1;
 return compstrs(txt1->pig->name,txt2->pig->name);
 }

void inittxts(void)
 {
 int txttyp,typnum[txt_number],rdlnum,firstanimtxt,i,animnum;
 char text[100],*pos;
 static struct ham_txt txt_nothing,txt_default; /* for thing&txt2 */
 static struct pig_txt ptxt_nothing,ptxt_default;
 for(i=0;i<txt_number;i++) pig.num_txtlist[i]=0;
 firstanimtxt=-1;
 for(rdlnum=0;rdlnum<pig.num_rdltxts;rdlnum++)
  { 
  if(pig.rdl_txts[rdlnum].pigno==0) 
   { pig.rdl_txts[rdlnum].pig=NULL; continue; }
  my_assert(pig.rdl_txts[rdlnum].pigno>=0 && 
   pig.rdl_txts[rdlnum].pigno<pig.num_pigtxts);
  pig.rdl_txts[rdlnum].pig=&pig.pig_txts[pig.rdl_txts[rdlnum].pigno];
  if(init_test&1) fprintf(errf,"rdlnum=%d pignum=%d name=%s type1=%x\n",
   rdlnum,pig.rdl_txts[rdlnum].pigno,pig.rdl_txts[rdlnum].pig->name,
   pig.rdl_txts[rdlnum].pig->type1);
  pig.rdl_txts[rdlnum].pig->num_anims=1; 
  pig.rdl_txts[rdlnum].pig->anim_t2=0; 
  strcpy(text,pig.rdl_txts[rdlnum].pig->name);
  if((pos=strpbrk(text," 0123456789"))!=NULL) *pos=0;
  if(strstr(pig.anim_txt_names,text)==NULL) 
   {
   switch(pig.rdl_txts[rdlnum].pig->type1&0x03)
    {
    case 0: pig.num_txtlist[txt1_normal]++; break;
    case 1: pig.num_txtlist[txt2_normal]++; break;
    case 2: case 3: pig.num_txtlist[txt2_wall]++; break;
    }
   }
  else
   if(firstanimtxt<0) firstanimtxt=rdlnum;
   else
    if(strcmp(pig.rdl_txts[rdlnum].pig->name,
     pig.rdl_txts[firstanimtxt].pig->name))
     { pig.rdl_txts[firstanimtxt].pig->num_anims=rdlnum-firstanimtxt;
       pig.rdl_txts[firstanimtxt].pig->anim_t2=
        ((pig.rdl_txts[rdlnum-1].pig->type1&2)!=0);
       firstanimtxt=rdlnum; }
  }
 if(firstanimtxt>=0)
  { pig.rdl_txts[firstanimtxt].pig->num_anims=rdlnum-firstanimtxt;
    pig.rdl_txts[firstanimtxt].pig->anim_t2=
     ((pig.rdl_txts[rdlnum-1].pig->type1&2)!=0); }
 /* now set the pointer for the typlists */
 for(txttyp=0;txttyp<txt_door;txttyp++)
  {                 
  if(init_test&1)
   fprintf(errf,"textures: typ=%d num=%d\n",txttyp,pig.num_txtlist[txttyp]);
  checkmem(pig.txtlist[txttyp]=MALLOC(sizeof(struct ham_txt *)*
   pig.num_txtlist[txttyp]));
  typnum[txttyp]=0;
  }
 for(rdlnum=0;rdlnum<pig.num_rdltxts;rdlnum++)
  {
  if(pig.rdl_txts[rdlnum].pig==NULL) continue;
  strcpy(text,pig.rdl_txts[rdlnum].pig->name);
  if((pos=strpbrk(text," 0123456789"))!=NULL) *pos=0;
  if(strstr(pig.anim_txt_names,text)==NULL ||
   (init.d_ver>=d2_12_reg && pig.rdl_txts[rdlnum].pig->frame_num==0))
   switch(pig.rdl_txts[rdlnum].pig->type1&0x03)
    {
    case 0:
     pig.txtlist[txt1_normal][typnum[txt1_normal]]=&pig.rdl_txts[rdlnum];
     pig.rdl_txts[rdlnum].txtlistno[txt1_normal]=typnum[txt1_normal]++;
     break;
    case 1: /* transparent */
     pig.txtlist[txt2_normal][typnum[txt2_normal]]=&pig.rdl_txts[rdlnum];
     pig.rdl_txts[rdlnum].txtlistno[txt2_normal]=typnum[txt2_normal]++;
     break;
    case 2: case 3: /* super-transparent */
     pig.txtlist[txt2_wall][typnum[txt2_wall]]=&pig.rdl_txts[rdlnum];
     pig.rdl_txts[rdlnum].txtlistno[txt2_wall]=typnum[txt2_wall]++;
     break;
    }
  }
 for(i=0;i<txt_door;i++)
  {
  if(init_test&1)
   fprintf(errf,"textures: typ=%d num=%d == %d?\n",i,pig.num_txtlist[i],
    typnum[i]);
  my_assert(typnum[i]==pig.num_txtlist[i]);
  }
 /* init the doors */
 for(animnum=0;animnum<pig.num_anims;animnum++)
  {
  rdlnum=pig.anim_starts[animnum];
  my_assert(rdlnum>=0 && rdlnum<pig.num_rdltxts);
  if(rdlnum!=0)
   { pig.anims[animnum]=&pig.rdl_txts[rdlnum];
     pig.num_txtlist[txt_door]++; }
  else pig.anims[animnum]=NULL;
  }
 checkmem(pig.txtlist[txt_door]=CALLOC(sizeof(struct ham_txt *),
  pig.num_txtlist[txt_door]));
 for(animnum=0,typnum[txt_door]=0;animnum<pig.num_anims;animnum++)
  if(pig.anims[animnum])
   { my_assert(typnum[txt_door]<pig.num_txtlist[txt_door]);
     pig.txtlist[txt_door][typnum[txt_door]]=pig.anims[animnum];
     pig.anims[animnum]->txtlistno[txt_door]=typnum[txt_door]++; }
 /* init the other lists */     
 pig.txtlist[txt_all]=NULL; /* for realloc in copytxtnums */
 pig.num_txtlist[txt_all]=0;
 pig.txtlist[txt_thing]=NULL; /* for realloc in copytxtnums */
 pig.num_txtlist[txt_thing]=0;
 copytxtnums(txt_all,txt1_normal);
 copytxtnums(txt_all,txt2_normal);
 copytxtnums(txt_all,txt1_wall);
 copytxtnums(txt_all,txt2_wall);
 copytxtnums(txt_all,txt_door);
 copytxtnums(txt1_wall,txt1_normal);
 copytxtnums(txt1_wall,txt2_normal);
 copytxtnums(txt_thing,txt1_wall);
 copytxtnums(txt2_wall,txt2_normal);
 strcpy(ptxt_nothing.name," Nothing");
 ptxt_nothing.num_anims=0; ptxt_nothing.anim_t2=1; ptxt_nothing.pigno=-1;
 ptxt_nothing.f=NULL; ptxt_nothing.frame_num=0;
 ptxt_nothing.xsize=ptxt_nothing.ysize=64; ptxt_nothing.offset=0;
 ptxt_nothing.type1=ptxt_nothing.type2=0;
 strcpy(ptxt_default.name," Default"); 
 ptxt_default.num_anims=0; ptxt_default.anim_t2=1; ptxt_default.pigno=-1;
 ptxt_default.f=NULL; ptxt_default.frame_num=0;
 ptxt_default.xsize=ptxt_default.ysize=64; ptxt_default.offset=0;
 ptxt_default.type1=ptxt_default.type2=0;
 txt_nothing.flags=txt_nothing.light=txt_nothing.hitpoints=
  txt_nothing.xspeed=txt_nothing.yspeed=0;
 txt_nothing.anim_seq=txt_nothing.shoot_out_txt=-1;
 txt_nothing.rdlno=0; txt_nothing.pigno=-1; 
 for(txttyp=0;txttyp<txt_number;txttyp++) txt_nothing.txtlistno[txttyp]=-1;
 txt_nothing.pig=&ptxt_nothing;
 txt_default=txt_nothing; 
 txt_default.pig=&ptxt_default; txt_default.rdlno=-1;
 inserttexture(txt2_normal,&txt_nothing);
 inserttexture(txt2_wall,&txt_nothing);
 inserttexture(txt_all,&txt_nothing); 
 inserttexture(txt_thing,&txt_default); 
 inserttexture(txt_all,&txt_default);
 /* now sort the lists alphabetically */
 for(txttyp=0;txttyp<txt_number;txttyp++)
  {
  qsort(pig.txtlist[txttyp],pig.num_txtlist[txttyp],sizeof(struct ham_txt *),
   cmp_txts); 
  for(i=0;i<pig.num_txtlist[txttyp];i++)
   {
   if(init_test&1)
    fprintf(errf,"txttyp=%d i=%d name=%s rdlno=%d\n",txttyp,
     i,pig.txtlist[txttyp][i]->pig->name,pig.txtlist[txttyp][i]->rdlno);
   pig.txtlist[txttyp][i]->txtlistno[txttyp]=i;
   }
  } 
 }

int newpigfile(char *pigname,FILE *pogfile)
 {
 char *pigfname=NULL,*palfname=NULL;
 int i;
 FILE *pf;
 if(!pig.current_pigname || strcmp(pig.current_pigname,pigname) ||
  pogfile!=pig.pogfile)
  { /* new pig-file */
  checkmem(pigfname=MALLOC(strlen(pigname)+
   strlen(init.pigpaths[init.d_ver])+2));
  strcpy(pigfname,init.pigpaths[init.d_ver]); strcat(pigfname,"/");
  strcat(pigfname,pigname); strcpy(&pigfname[strlen(pigfname)-3],"PIG");
  if((pf=fopen(pigfname,"rb"))==NULL)
   { printf("Can't open pigfile in newpigfile: '%s'\n",pigfname);
     FREE(pigfname); return 0; }
  if(!readtxts(pf)) { fclose(pf); FREE(pigfname); return 0; }
  if(pogfile!=pig.pogfile) fclose(pig.pogfile);
  readpogfile(pogfile); pig.pogfile=pogfile;
  checkmem(palfname=MALLOC(strlen(pigname)+1));
  strcpy(palfname,pigname); palfname[strlen(palfname)-4]=0;
  for(i=0;i<strlen(palfname);i++) palfname[i]=toupper(palfname[i]);
  for(i=0;i<NUM_PALETTES;i++)
   if(!strcmp(palettes[i].name,palfname)) break;
  if(i==NUM_PALETTES)
   waitmsg(TXT_CANTFINDPALETTE,palfname,palettes[i=1].name);
  FREE(palfname); FREE(pig.current_pigname);
  checkmem(pig.current_pigname=MALLOC(strlen(pigname)+1));
  strcpy(pig.current_pigname,pigname);
  if(pig.pigfile) fclose(pig.pigfile); 
  pig.pigfile=pf;
  changepigfile(palettes[i].name);
  view.lightcolors=&palettes[i].lighttables[256*NUM_SECURITY];
  newpalette(palettes[i].palette); 
  inittxts();
  }
 return 1;
 }

#define ANIM_ARROW_ANGLE M_PI/4 
/* reads texture ham_sd from file specified in texture in direction dir.
   if ham_sd==NULL, read pig_sd.
   dir=0 -> normal (origin left upper corner x+ y+).
   dir=1 -> 90ø   (origin right upper corner x- y+). 
   dir=2 -> 180ø   (origin right lower corner x- y-).
   dir=3 -> 270ø    (origin left lower corner x+ y-). */
void readbitmap(char *dest,struct pig_txt *pig_sd,
 struct ham_txt *ham_sd,int dir)
 {
 struct pig_txt *sd=ham_sd ? ham_sd->pig : pig_sd;
 char *sbitmap,bitmap[64*64],*buffer,*fill,*nobytesinrow;
 unsigned int size;
 unsigned int i;
 int startx,starty,endx,endy,addx,addy,mx,my,x,y;
 float l,angle;
 struct ws_bitmap *bm;
 FILE *f;
 my_assert(sd!=NULL);
 f=sd->f; if(f==NULL) return;
 if(sd->pigno<0) return; /* Nothing or Default */
 fseek(f,(long)sd->offset,SEEK_SET); 
 if(sd->xsize!=64 || sd->ysize!=64)
  { printmsg("Texture (rdl: %d pig: %d name: %s) of wrong size: %d %d",
     ham_sd ? ham_sd->rdlno : -1,sd->pigno,sd->name,sd->xsize,sd->ysize);
     return; }
 switch(dir)
  {
  case 0: startx=starty=0; addx=addy=1; mx=1; my=64; break;
  case 1: startx=63; starty=0; addx=-1; addy=1; mx=64; my=1; break;
  case 2: startx=starty=63; addx=addy=-1; mx=1; my=64; break;
  case 3: startx=0; starty=63; addx=1; addy=-1; mx=64; my=1; break;
  default: printmsg(TXT_READBMUNKNOWNDIR,dir); 
   startx=starty=0; addx=addy=1; mx=1; my=64;
  }
 endx=63-startx; endy=63-starty;
 if((sd->type1&0x08)==0)
  {
  y=starty-addy;
  do
   {
   y+=addy;
   x=startx-addx;
   do
    {
    x+=addx;
    if(fread(&bitmap[y*my+x*mx],1,1,f)!=1)
     { printmsg("Can't read uncompressed bitmap."); return; }
    }
   while(x!=endx);
   }
  while(y!=endy);
  }
 else
  {
  if(fread(&size,sizeof(unsigned long int),(size_t)1,f)!=1)
   { printmsg("Can't read size of compressed bitmap."); return; }
  checkmem(nobytesinrow=MALLOC((size_t)sd->ysize));
  checkmem(buffer=MALLOC((size_t)size-4-sd->ysize));
  if(fread(nobytesinrow,1,(size_t)sd->ysize,f)!=sd->ysize)
   { printmsg("Can't read nobytesinrow."); return; } /* no need for this */
  if(fread(buffer,(int)size-4-sd->ysize,1,f)!=1)
   { printmsg("Can't read compressed bitmap."); return; }
  y=starty-addy;
  fill=buffer;
  do
   {
   y+=addy;
   x=startx-addx;
   do
    {
    if((*fill&0xe0)==0xe0)
     {
     for(i=0;i<(*fill&0x1f);i++)
      {
      if(x==endx)
       { printmsg("Error reading compressed bitmap."); return; }
      x+=addx;
      bitmap[y*my+x*mx]=*(fill+1);
      }
     fill+=2;
     }
    else
     { x+=addx; bitmap[y*my+x*mx]=*(fill++); }
    }
   while(x!=endx);
   fill++; /*0xe0*/
   }
  while(y!=endy);
  FREE(buffer);
  FREE(nobytesinrow);
  } 
 for(fill=bitmap,sbitmap=dest;fill-bitmap<64*64;fill++,
  sbitmap++)
  if(*((unsigned char *)fill)!=0xff) *sbitmap=*fill;
 /* draw the lines for the moved textures */
 if(ham_sd!=NULL && (ham_sd->xspeed!=0 || ham_sd->yspeed!=0))
  {
  switch(dir)
   {
   case 2: mx=ham_sd->xspeed; my=ham_sd->yspeed; break;
   case 3: mx=ham_sd->yspeed; my=-ham_sd->xspeed; break;
   case 0: mx=-ham_sd->xspeed; my=-ham_sd->yspeed; break;
   case 1: mx=-ham_sd->yspeed; my=ham_sd->xspeed; break;
   default: mx=ham_sd->xspeed; my=ham_sd->yspeed; 
   }
  checkmem(bm=ws_createbitmap(64,64,dest));
  l=sqrt((float)mx*mx+my*my);
  startx=31+24*mx/l; starty=31+24*my/l;
  angle=my>=0 ? acos(mx/l) : -acos(mx/l);
  endx=31+12*cos(angle+ANIM_ARROW_ANGLE);
  endy=31+12*sin(angle+ANIM_ARROW_ANGLE);
  x=31+12*cos(angle-ANIM_ARROW_ANGLE); addx=0;
  y=31+12*sin(angle-ANIM_ARROW_ANGLE); addy=0;
  size=((mx<0 ? -mx : mx)+(my<0 ? -my : my))>>7; 
  for(i=0;i<size;i++)
   {
   ws_bmdrawline(bm,startx+addx,starty+addy,endx+addx,endy+addy,
    view.color[WHITE],0);
   ws_bmdrawline(bm,startx+addx,starty+addy,x+addx,y+addy,
    view.color[WHITE],0);
   if(mx>0) addx-=2; else if(mx<0) addx+=2;
   if(my>0) addy-=2; else if(my<0) addy+=2;
   ws_bmdrawline(bm,startx+addx,starty+addy,endx+addx,endy+addy,
    view.color[BLACK],0);
   ws_bmdrawline(bm,startx+addx,starty+addy,x+addx,y+addy,
    view.color[BLACK],0);
   if(mx>0) addx-=2; else if(mx<0) addx+=2;
   if(my>0) addy-=2; else if(my<0) addy+=2;
   }
  ws_freebitmap(bm);
  }
 }

