/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    initio.c - reading "devil.ini" file
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
#include "macros.h"
#include "insert.h"
#include "readlvl.h"
#include "do_event.h"
#include "initio.h"

void readpoint(FILE *f,struct point *p);
int readobjtype(FILE *f,struct objtype *ot);
void readstring(FILE *f,char **s);
void printobjtypelists(struct infoitem *is,int num,int indent);

int init_test;  
int txtoffsets[desc_number];

int findmarker(FILE *f,const char *m,int *number)
 {
 char puffer[255],newmarker[200];
 fseek(f,0,SEEK_SET); /* not the smartest solution */
 newmarker[0]=':';
 strcpy(&newmarker[1],m);
 newmarker[strlen(m)+1]=0;
 do
  {
  if(fgets(puffer,255,f)==NULL) return 0;
  }
 while(strncmp(newmarker,puffer,strlen(newmarker)));
 *number=atoi(&puffer[strlen(newmarker)+1]);
 return 1;
 }

void skipline(FILE *f)
 {
 char puffer[255];
 if(fgets(puffer,255,f)==NULL)
  { printf("Can't skip line.\n"); exit(2); }
 if(strlen(puffer)>80)
  { printf("Skipline: line too long\n"); exit(2); }
 }
 
void readpoint(FILE *f,struct point *p)
 {
 if(fscanf(f,"%g%g%g",&p->x[0],&p->x[1],&p->x[2])!=3)
  { printf("Can't read point.\n"); exit(2); }
 }
 
void readstring(FILE *f,char **s)
 {
 int i=0;
 char puffer[255];
 while(fgetc(f)!='{')
  if(feof(f)) { printf("Can't find start of string.\n"); exit(2); }
 do
  {
  puffer[i++]=fgetc(f);
  }
 while(puffer[i-1]!='}' && i<255 && !feof(f));
 puffer[i-1]=0;
 if(feof(f) || i==255)
  { printf("Can't read string.\n"); exit(2); }
 if((*s=MALLOC(strlen(puffer)+1))==NULL)
  { printf("No mem for string.\n"); exit(2); }
 strcpy(*s,puffer);
 }

int readobjtype(FILE *f,struct objtype *ot)
 {
 int num;
 if(fscanf(f,"%x%d",&ot->no,&num)!=2)
  { printf("Can't read objtype.\n"); exit(2); }
 readstring(f,&ot->str); 
 if(init_test&1)
  fprintf(errf,"Read objtype: %x %d {%s}\n",ot->no,num,ot->str);
 return num;
 }   

void readkeycode(FILE *f,struct w_keycode *kc,char **s)
 {
 if(fscanf(f,"%i%i%i",&kc->kbstat,&kc->key,&kc->event)!=3)
  { printf("Can't read keycode.\n"); exit(2); }
 readstring(f,s);
 }

int readinfotype(FILE *f,struct infoitem *i)
 {
 int p,inr,tnr;
 readstring(f,&i->txt);
 if(strlen(i->txt)==0) { FREE(i->txt); i->txt=NULL; }
 if(fscanf(f,"%d%d%d%d%d%d%d",&tnr,&inr,&i->offset,&i->length,
  &i->multifuncnr,&p,&i->sidefuncnr)!=7)
  { printf("Can't read info item header.\n"); exit(2); }
 i->infonr=inr;
 i->type=tnr;
 i->numchildren = (tnr==it_selbutton) ? i->multifuncnr : 0;
 return p;
 }
 
int iniread(FILE *f,const char *t,...)
 {
 int i,n,ret=0;
 struct w_keycode *wkey;
 va_list alist;
 va_start(alist,t);
 n=strlen(t);
 for(i=0;i<n;i++)
  {
  switch(t[i])
   {
   case 'g': 
    if(fscanf(f,"%g",va_arg(alist,float *))!=1)
     { printf("Can't read float.\n"); exit(2); } break;
   case 'p': readpoint(f,va_arg(alist,struct point *)); break;
   case 'U': 
    if(fscanf(f,"%lu",va_arg(alist,unsigned long *))!=1)
     { printf("Can't read unsigned long.\n"); exit(2); } break;
   case 'X': 
    if(fscanf(f,"%lx",va_arg(alist,unsigned long *))!=1)
     { printf("Can't read unsigned long hex.\n"); exit(2); } break;
   case 'd': 
    if(fscanf(f,"%d",va_arg(alist,int *))!=1)
     { printf("Can't read int.\n"); exit(2); } break;
   case 'x': 
    if(fscanf(f,"%x",va_arg(alist,int *))!=1)
     { printf("Can't read int.\n"); exit(2); } break;
   case 's': readstring(f,va_arg(alist,char **)); break;
   case 'o': ret=readobjtype(f,va_arg(alist,struct objtype *)); break;
   case 'i': ret=readinfotype(f,va_arg(alist,struct infoitem *)); break;
   case 'k': wkey=va_arg(alist,struct w_keycode *);
    readkeycode(f,wkey,va_arg(alist,char **)); break;
   default: printf("Unknown type %c\n",t[i]); exit(2);
   }
  skipline(f);
  }
 va_end(alist);
 return ret;
 }

void addcfgpath(char **s)
 {
 char *r;
 checkmem(r=MALLOC(strlen(*s)+strlen(init.cfgpath)+2));
 strcpy(r,init.cfgpath); strcat(r,"/"); strcat(r,*s);
 FREE(*s); *s=r;
 }

int readpalettes(FILE *f)
 {
 int i,j;
 unsigned char num_pal;
 if(fread(&num_pal,1,1,f)!=1 || NUM_PALETTES!=num_pal) return 0;
 for(i=0;i<NUM_PALETTES;i++)
  {
  if(fread(palettes[i].name,1,8,f)!=8) return 0;
  palettes[i].name[8]=0;
  if(fread(palettes[i].palette,3,256,f)!=256) return 0;
  for(j=0;j<3*255;j++)
   palettes[i].palette[j]=palettes[i].palette[j]<<2; /* values are 0..63 */
  checkmem(palettes[i].mem_lighttables=
   malloc((NUM_SECURITY*2+NUM_LIGHTCOLORS+1)*256));
  palettes[i].lighttables=(unsigned char *)
   ((unsigned long)palettes[i].mem_lighttables&0xffffff00);
  if(fread(&palettes[i].lighttables[256*NUM_SECURITY],256,
   NUM_LIGHTCOLORS,f)!=NUM_LIGHTCOLORS) return 0;
  /* this is to get proper light values even if we have some trouble with
     rounding */
  for(j=0;j<NUM_SECURITY;j++)
   { memcpy(&palettes[i].lighttables[256*j],
      &palettes[i].lighttables[NUM_SECURITY*256],256);
     memcpy(&palettes[i].lighttables[256*(NUM_LIGHTCOLORS+NUM_SECURITY+j)],
      &palettes[i].lighttables[(NUM_SECURITY+NUM_LIGHTCOLORS-1)*256],256); } 
  }
 view.lightcolors=&palettes[0].lighttables[NUM_SECURITY*256];
 return 1;
 }

void initeditor(const char *fn,int c)
 {
 int i,n;
 FILE *f;
 char *palname;
 if((f=fopen(fn,"r+"))==NULL)
  { printf(TXT_CANTOPENINI TXT_UNZIPWITHD,fn); exit(2); }
 printf(TXT_READINI);
 my_assert(findmarker(f,"INITDATA",&init_test));
 iniread(f,"dddpppdgggggggggdggggggggggggggggdddddddddddddddddsssssssssssssss",
  &view.movemode,&view.currmode,&view.whichdisplay,&view.e0,&view.e[0],
  &view.e[1],&view.doubleclick,&view.distcenter,&view.dist,&view.distscala,
  &view.movefactor,&view.pmovefactor,&view.movescala,&view.rotangle,
  &view.protangle,&view.rotscala,&view.bm_movefactor,
  &view.flatsideangle,&view.bm_stretchfactor,&view.maxvisibility,
  &view.visscala,&view.tsize,&view.dsize,&view.minclicksodist,
  &view.mincorner,&view.minweirdwall,&view.maxconndist,
  &view.maxuserconnect,
  &view.gridlength,&view.gridscala,&view.mapangle,&view.illum_quarterway,
  &view.illum_brightness,&n,&view.gridonoff,&view.coord_axis,
  &view.clickradius,&view.displayed_group,
  &view.askone,&view.asktagged,&view.warn_convex,&view.warn_doublekeys,
  &view.warn_tworeactors,&view.warn_thingoutofbounds,&view.warn_gridrot,
  &txtoffsets[0],&txtoffsets[1],&txtoffsets[2],&txtoffsets[3],
  &init.waittime,&init.macropath,&init.levelpath,&init.pogpath,
  &init.txtlistpath,&init.cfgpath,&init.fontname,&palname,
  &init.cfgname,&init.lightname,&init.convtablename,&init.lastname,
  &init.batchfilename,&init.menuname,&pig.bulbname,&pig.brokenbulbname);
 view.illum_minvalue=n; view.render=0; view.gamma_corr=7<<9;
 view.blinkinglightson=0; view.timescale=1.0; view.warn_frameratetoosmall=1;
 view.warn_illuminate=1;
 addcfgpath(&init.cfgname); addcfgpath(&init.lastname); 
 addcfgpath(&init.menuname); addcfgpath(&init.fontname); 
 addcfgpath(&init.batchfilename); addcfgpath(&init.lightname);
 addcfgpath(&init.convtablename);
 addcfgpath(&pig.bulbname); addcfgpath(&pig.brokenbulbname);
 init.xres=640; init.yres=480; view.drawwhat=DW_DEFAULT;
 if(init_test&1) printf("Init output on: %d.\n",init_test);
 normalize(&view.e[0]); 
 normalize(&view.e[1]); 
 VECTOR(&view.e[2],&view.e[1],&view.e[0]); /* left-handed system */
 my_assert(findmarker(f,"BUTTONS",&init.numbuttons));
 if(init.numbuttons!=in_number)
  { printf("Wrong number of buttons: %d.\n",init.numbuttons); exit(2); }
 for(i=0;i<init.numbuttons;i++) iniread(f,"s",&init.bnames[i]);
 fclose(f);
 addcfgpath(&palname);
 if((f=fopen(palname,"rb"))==NULL)
  { printf("Can't open palette-file: %s.\n",palname); exit(2); }
 FREE(palname);
 if(!readpalettes(f))
  { printf("Can't read palette file.\n"); exit(2); }
 fclose(f);
 initlist(&view.levels); view.txt_window=NULL;
 view.defwall=0; view.pdefcube=NULL; view.pdeflevel=NULL;
 view.pcurrmacro=NULL; pig.current_pigname=NULL;
 pig.default_pigname=NULL; pig.pig_txts=NULL;
 pig.num_pigtxts=0; view.littlebulbson=1;
 }
