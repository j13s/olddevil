/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    initio.c - reading "devil.ini" file
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
    
#include <dir.h>
#include "structs.h"
#include "tools.h"
#include "initio.h"

void skipline(FILE *f);
void readpoint(FILE *f,struct point *p);
int readobjtype(FILE *f,struct objtype *ot);
int iniread(FILE *f,char *s,...);
void iniobjtyperead(FILE *f,char *tn,struct objdata *od);
void readstring(FILE *f,char **s);
void readevent(FILE *f,struct eventcode *e);
void findmarker(FILE *f,char *m,int *number);
void printobjtypelists(struct infoitem *is,int num,int indent);

int init_test;  

#ifdef SHAREWARE
char *txt_names[txt_number] = { "SNormalTxt","SExtraTxt","SWNormalTxt",
 "SWExtraTxt" };
#else
char *txt_names[txt_number] = { "NormalTxt","ExtraTxt","WNormalTxt",
 "WExtraTxt" };
#endif

void findmarker(FILE *f,char *m,int *number)
 {
 char puffer[255],newmarker[200];
 fseek(f,0,SEEK_SET); /* not the smartest solution */
 newmarker[0]=':';
 strcpy(&newmarker[1],m);
 newmarker[strlen(m)+1]=0;
 do
  {
  if(fgets(puffer,255,f)==NULL)
   { printf("Can't find marker :%s\n",m); exit(0); }
  }
 while(strncmp(newmarker,puffer,strlen(newmarker)));
 *number=atoi(&puffer[strlen(newmarker)+1]);
 }

void skipline(FILE *f)
 {
 char puffer[255];
 if(fgets(puffer,255,f)==NULL)
  { printf("Can't skip line.\n"); exit(0); }
 if(strlen(puffer)>80)
  { printf("Skipline: line too long\n"); exit(0); }
 }
 
void readpoint(FILE *f,struct point *p)
 {
 if(fscanf(f,"%lg%lg%lg",&p->x[0],&p->x[1],&p->x[2])!=3)
  { printf("Can't read point.\n"); exit(0); }
 }
 
void readstring(FILE *f,char **s)
 {
 int i=0;
 char puffer[255];
 while(fgetc(f)!='{')
  if(feof(f)) { printf("Can't find start of string.\n"); exit(0); }
 do
  {
  puffer[i++]=fgetc(f);
  }
 while(puffer[i-1]!='}' && i<255 && !feof(f));
 puffer[i-1]=0;
 if(feof(f) || i==255)
  { printf("Can't read string.\n"); exit(0); }
 if((*s=malloc(strlen(puffer)+1))==NULL)
  { printf("No mem for string.\n"); exit(0); }
 strcpy(*s,puffer);
 }

int readobjtype(FILE *f,struct objtype *ot)
 {
 int num;
 if(fscanf(f,"%x%d",&ot->no,&num)!=2)
  { printf("Can't read objtype.\n"); exit(0); }
 readstring(f,&ot->str); 
 if(init_test&1)
  fprintf(errf,"Read objtype: %x %d {%s}\n",ot->no,num,ot->str);
 return num;
 }   

void readevent(FILE *f,struct eventcode *e)
 {
 if(fscanf(f,"%x%x%x%x%x",&e->no,&e->flags,&e->kbstat,&e->key,&e->where)!=5)
  { printf("Can't read event.\n"); exit(0); }
 readstring(f,&e->txt);
 if(e->where<ecw_screen || e->where>=ecw_num_of_boxes)
  { printf("Wrong boxtype in event %d\n",e->no); exit(0); }
 }

int readinfotype(FILE *f,struct infoitem *i)
 {
 int p,inr;
 readstring(f,&i->txt);
 if(fscanf(f,"%d%d%d%d%d%d%d%d%d%d",&inr,&i->offset,&i->length,&i->row,
  &i->column,&i->menulength,&i->numchildren,&p,&i->class,&i->sidefuncnr)!=10)
  { printf("Can't read info item header.\n"); exit(0); }
 i->infonr=inr;
 return p;
 }
 
int iniread(FILE *f,char *t,...)
 {
 int i,n,ret=0;
 va_list alist;
 va_start(alist,t);
 n=strlen(t);
 for(i=0;i<n;i++)
  {
  switch(t[i])
   {
   case 'g': 
    if(fscanf(f,"%lg",va_arg(alist,double *))!=1)
     { printf("Can't read double.\n"); exit(0); } break;
   case 'p': readpoint(f,va_arg(alist,struct point *)); break;
   case 'U': 
    if(fscanf(f,"%lu",va_arg(alist,unsigned long *))!=1)
     { printf("Can't read unsigned long.\n"); exit(0); } break;
   case 'X': 
    if(fscanf(f,"%lx",va_arg(alist,unsigned long *))!=1)
     { printf("Can't read unsigned long hex.\n"); exit(0); } break;
   case 'd': 
    if(fscanf(f,"%d",va_arg(alist,int *))!=1)
     { printf("Can't read int.\n"); exit(0); } break;
   case 'x': 
    if(fscanf(f,"%x",va_arg(alist,int *))!=1)
     { printf("Can't read int.\n"); exit(0); } break;
   case 'e': readevent(f,va_arg(alist,struct eventcode *)); break;
   case 's': readstring(f,va_arg(alist,char **)); break;
   case 'o': ret=readobjtype(f,va_arg(alist,struct objtype *)); break;
   case 'i': ret=readinfotype(f,va_arg(alist,struct infoitem *)); break;
   default: printf("Unknown type %c\n",t[i]); exit(0);
   }
  skipline(f);
  }
 va_end(alist);
 return ret;
 }

int initinforead(FILE *f,struct infoitem **is,int num,enum infos infonr)
 {
 int a,b,iinr=0,menunum=0,c,d;
 struct infoitem *i2;
 if(num==0) *is=NULL; 
 else if((*is=malloc(sizeof(struct infoitem)*num))==NULL)
  { printf("No mem for infos.\n"); exit(0); }
 for(i2=*is;i2-*is<num;i2++)
  {
  if(init_test&1)
   fprintf(errf,"Reading infonr: entry %ld num %d\n",i2-*is,num);
  b=iniread(f,"i",i2);
  if(i2->class<0)
   {
   i2->numsideeffs=-i2->class;
   if(init_test&1) fprintf(errf,"numsideeffs=%d\n",i2->numsideeffs);
   if((i2->sideeffs=malloc(sizeof(unsigned int *)*i2->numsideeffs))==NULL)
    { printf("No mem for side effects.\n"); exit(0); }
   for(c=0;c<i2->numsideeffs;c++)
    {
    if(fscanf(f,"%i",&d)!=1)
     { printf("Can't read depth of side effect.\n"); exit(0); }
    if(d<3)
     { printf("Illegal depth %d for side effect %d in %s\n",d,c,i2->txt);
       exit(0); }
    if(init_test&1) fprintf(errf," depth of sideeff=%d ",d);
    if((i2->sideeffs[c]=malloc(sizeof(unsigned int)*d+1))==NULL)
     { printf("No mem for buffering the side effects.\n"); exit(0); }
    *i2->sideeffs[c]=d;
    for(a=0;a<d;a++)
     {
     if(fscanf(f,"%i",i2->sideeffs[c]+a+1)!=1)
      { printf("Can't read side effect number for %s.\n",i2->txt); exit(0); }
     if(init_test&1) fprintf(errf," %d",*(i2->sideeffs[c]+a+1));
     }
    skipline(f);
    if(init_test&1) fprintf(errf," \n");
    }
   }
  else
   { i2->sideeffs=NULL; i2->numsideeffs=0; }
  if(i2->class==ic_display || i2->class<0)
   { menunum++;
   if(i2->row>view.maxrow[infonr]) view.maxrow[infonr]=i2->row; }
  if(init_test&1)
   fprintf(errf,"Header: %s offset=%d row=%d col=%d numchild=%d numdesc=%d\n",
    i2->txt,i2->offset,i2->row,i2->column,i2->numchildren,b);
  if(i2->numchildren>0)
   {
   if((i2->children=malloc(sizeof(struct infoitem *)*i2->numchildren))==NULL)
    { printf("No mem for info children.\n"); exit(0); }
   if((i2->itemchildren=malloc(sizeof(unsigned long)*i2->numchildren))==NULL)
    { printf("No mem for info item children.\n"); exit(0); }
   if((i2->numechildren=malloc(sizeof(int)*i2->numchildren))==NULL)
    { printf("No mem for info nume children.\n"); exit(0); }
   if((i2->numdchildren=malloc(sizeof(int)*i2->numchildren))==NULL)
    { printf("No mem for info nume children.\n"); exit(0); }
   }
  else
   { i2->children=NULL; i2->itemchildren=NULL; 
     i2->numechildren=i2->numdchildren=NULL; }
  if(b>0)
   {
   if((i2->od=malloc(sizeof(struct objdata)))==NULL)
    { printf("No mem for objdata.\n"); exit(0); }
   i2->od->str=i2->txt;
   i2->od->size=b;
   if((i2->od->data=malloc(sizeof(struct objtype *)*
    (size_t)i2->od->size))==NULL)
    { printf("No mem for objtype *.\n"); exit(0); }
   for(b=0,iinr=0;b<i2->od->size;b++) 
    {
    if((i2->od->data[b]=malloc(sizeof(struct objtype)))==NULL)
     { printf("No mem for objtype.\n"); exit(0); }
    a=iniread(f,"o",i2->od->data[b]);
    if(a>0) 
     {
     if(iinr>=i2->numchildren)
      { printf("Too many children %d/%d: %d>=%d\n",b,i2->od->size,iinr+1,
         i2->numchildren); exit(0); }
     i2->itemchildren[iinr]=i2->od->data[b]->no;
     i2->numdchildren[iinr]=a;
     i2->numechildren[iinr]=initinforead(f,&i2->children[iinr],a,infonr);
     iinr++;
     }
    }
   }
  else
   i2->od=NULL;
  if(i2->class<0) i2->class=ic_display;
  }
 return menunum;
 }
 
struct infoitem *findcopytypelist(struct infoitem *curi,int num,
 struct objtype **ot)
 {
 int s;
 struct infoitem *ncuri=NULL,*si;
 if((*ot)->str[0]=='>' || (*ot)->str[0]=='!')
  {
  for(si=curi,ncuri=NULL;si-curi<num;si++)
   if(si->offset==(*ot)->no) { ncuri=si; num=0; break; }
  }
 else
  for(s=0,ncuri=NULL;s<curi->numchildren;s++)
   if(curi->itemchildren[s]==(*ot)->no)
    { num=curi->numdchildren[s]; ncuri=curi->children[s]; break; }  
 if(ncuri==NULL)
  { printf("Can't find %d for %s\n",(*ot)->no,(*ot)->str); exit(0); }
 if((*ot)->str[0]!='!') return findcopytypelist(ncuri,num,ot+1);
 return ncuri;
 }
 
void initobjtypelists(struct infoitem *is,int num)
 {
 struct infoitem *i,*curi;
 struct objtype **ot,**lastot;
 int s,oldnum;
 if(init_test&1)
  fprintf(errf,"Init objtype list. num=%d\n",num);
 for(i=is;i-is<num;i++)
  {
  if(i->od==NULL) continue;
  for(ot=i->od->data,lastot=NULL;ot-i->od->data<i->od->size;ot++)
   {
   if((*ot)->str[0]=='>' || (*ot)->str[0]=='#' || (*ot)->str[0]=='!')
    { lastot=ot; break; }
   }
  if(lastot!=NULL)
   {
   if(i->od->data[i->od->size-1]->str[0]!='!' || (*lastot)->str[0]!='!')
    { printf("First/Last item of %s not with '!'\n",(*ot)->str); exit(0); }
   if((*lastot)->no<0 || (*lastot)->no>=in_number)
    { printf("Illegal number for starting insert: %d (0-%d)\n",(*lastot)->no,
       in_number-1); exit(0); }
   if(init_test&1)
    fprintf(errf,"Searching in list %d: %d\n",(*lastot)->no,
     view.descripnum[(*lastot)->no]);
   curi=findcopytypelist(view.info[(*lastot)->no],
    view.descripnum[(*lastot)->no],lastot+1);
   if(curi->od==NULL)
    { printf("No objtypes in source typelist.\n"); exit(0); }
   oldnum=lastot-i->od->data;
   if((i->od->data=realloc(i->od->data,(oldnum+curi->od->size)*
    sizeof(struct objtype **)))==NULL)
    { printf("No mem for type list.\n"); exit(0); }
   lastot=&i->od->data[oldnum];
   if(init_test&1)
    fprintf(errf,"Inserting from %s %d to %s %d\n",curi->od->str,
     curi->od->size,i->od->str,i->od->size);
   for(s=0;s<curi->od->size;s++)
    {
    if(s<i->od->size-oldnum)
     { free(lastot[s]->str); free(lastot[s]); } 
    lastot[s]=curi->od->data[s];
    if(init_test&1)
     fprintf(errf,"Insert: %d %s\n",lastot[s]->no,lastot[s]->str);
    }
   i->od->size=curi->od->size+oldnum; 
   }
  for(s=0;s<i->numchildren;s++)
   initobjtypelists(i->children[s],i->numdchildren[s]);
  }
 }
  
void printobjtypelists(struct infoitem *is,int num,int indent)
 {
 struct infoitem *i;
 int a,b;
 for(i=is;i-is<num;i++)
  {
  fprintf(errf,"%d %s %d %d %d %p / %d %d %d\n",indent,i->txt,i->offset,
   i->length,i->numchildren,i->children,i->row,i->column,i->menulength);
  if(i->od!=NULL)
   for(a=0;a<i->od->size;a++)
    fprintf(errf,"  %x %s\n",i->od->data[a]->no,i->od->data[a]->str);
  for(a=0;a<i->numsideeffs;a++)
   {
   fprintf(errf, "Sideeffect %d: depth=%d -> ",a,i->sideeffs[a][0]);
   for(b=0;b<i->sideeffs[a][0];b++)
    fprintf(errf," %d",i->sideeffs[a][b+1]);
   fprintf(errf,"\n");
   }
  for(a=0;a<i->numchildren;a++)
   {
   fprintf(errf," CHILD: %lx %d %d %p\n",i->itemchildren[a],
    i->numdchildren[a],i->numechildren[a],i->children[a]);
   printobjtypelists(i->children[a],i->numdchildren[a],indent+1);
   }
  }
 }
 
void readtxts(void)
 {
 int i;
 unsigned long int numlong;
 FILE *f;
 if((view.pigfile=f=fopen(view.pigname,"rb"))==NULL)
  { fprintf(errf,"Can't open bitmaps.\n"); exit(0); }
 if(fread(&view.numtxtdir,sizeof(long),(size_t)1,f)!=1)
  { fprintf(errf,"Can't read number of short entries.\n"); exit(0); }
 if(fread(&numlong,sizeof(long),(size_t)1,f)!=1)
  { fprintf(errf,"Can't read number of long entries.\n"); exit(0); }
 if(init_test&1)
  fprintf(errf,"Found %d short, %lu long entries.\n",view.numtxtdir,numlong);
 if((view.txts=malloc(sizeof(struct texture)*view.numtxtdir))==NULL)
  { printf("No mem for txt-dir.\n"); exit(0); }
 for(i=0;i<view.numtxtdir;i++)
  {
  if(fread(&view.txts[i],17,1,f)!=1)
   { fprintf(errf,"Can't read dir of textures.\n"); exit(0); }
  memset(view.txts[i].name,' ',8);
  strncpy(view.txts[i].name,view.txts[i].rname,
   strlen(view.txts[i].rname)<9 ? strlen(view.txts[i].rname) : 8);
  view.txts[i].name[8]=0;
  if(view.txts[i].ruxsize==0x80) view.txts[i].xsize=320;
  else view.txts[i].xsize=(unsigned short)view.txts[i].rxsize;
  view.txts[i].ysize=(unsigned short)view.txts[i].rysize;
  if(init_test&1)
   fprintf(errf,"Found %d: %s %u %u %u %u %lx\n",i,view.txts[i].name,
    (unsigned)view.txts[i].xsize,(unsigned)view.txts[i].ysize,
    (unsigned)view.txts[i].type1,(unsigned)view.txts[i].type2,
    view.txts[i].offset);
  }
 view.txt_bmoffset=ftell(view.pigfile)+20*numlong;
 } 

void makehotkey(struct objtype *ot)
 {
 char *pos;
 if((pos=strchr(ot->str,'>'))==NULL)
  { printf("No hotkey for menupnt %s\n",ot->str); exit(0); }
 ot->no=(ot->no<<16)+(pos-ot->str);
 if(!isupper(toupper(*(pos+1))))
  { printf("Illegal character for hotkey in menupnt %s\n",ot->str); exit(0); }
 memmove(pos,pos+1,strlen(pos)+1);
 }    
  
void copytxtnums(enum txttypes dest,enum txttypes source)
 {
 int n;
 n=view.txtnums[dest].size+view.txtnums[source].size;
 if((view.txtnums[dest].data=realloc(view.txtnums[dest].data,
  n*sizeof(struct objtype *)))==NULL)
  { printf("No mem for txttable.\n"); exit(0); }
 memcpy(&view.txtnums[dest].data[view.txtnums[dest].size],
  view.txtnums[source].data,view.txtnums[source].size*
  sizeof(struct objtype *));
 view.txtnums[dest].size=n;
 }
 
void insertnothing(enum txttypes dest)
 {
 if((view.txtnums[dest].data=realloc(view.txtnums[dest].data,
  sizeof(struct objtype *)*(view.txtnums[dest].size+1)))==NULL)
  { printf("No mem to insert 'Nothing' in texture 2 list.\n"); exit(0); }
 memmove(view.txtnums[dest].data+1,view.txtnums[dest].data,
  sizeof(struct objtype *)*view.txtnums[dest].size);
 if((view.txtnums[dest].data[0]=malloc(sizeof(struct objtype)))
   ==NULL)
  { printf("No mem for objtype.\n"); exit(0); }
 view.txtnums[dest].data[0]->no=0;
 view.txtnums[dest].data[0]->str="Nothing";
 }
 
void initeditor(char *fn)
 {
 int i,j,n,maxanimtxt;
 FILE *f;
 struct ffblk macfiles;
 struct macro *m;
 struct macrogroup *mg;
 char text[100],*countedtxts,*doortextures,*pos;
 struct texture *texture;
 if((f=fopen(fn,"r"))==NULL)
  { printf("Can't open initfile %s!\n",fn); exit(0); }
 printf("Reading initdata...\n");
 findmarker(f,"INITDATA",&init_test);
 iniread(f,"sdpppgggggggggggggggggdddddssss",&l->lname,
  &view.showwhat,&view.e0,&view.e[0],&view.e[1],&view.dist,&view.distscala,
  &view.movefactor,&view.pmovefactor,&view.movescala,
  &view.rotangel,&view.rotscala,&view.maxvisibility,&view.visscala,
  &view.mousesensivity,&view.tsize,&view.dsize,&view.clickphi,&view.mincorner,
  &view.minweirdwall,&view.gridlength,&view.gridscala,&view.gridonoff,
  &view.numkeymacros,&view.txtoffset,&view.brightness,&view.waittime,
  &view.macropath,&view.levelpath,&view.pigname,&view.palettename);
 switch(view.showwhat)
  {
  case in_cube: view.currmode=tt_cube; break;
  case in_thing: view.currmode=tt_thing; break;
  case in_door: view.currmode=tt_wall; break;
  case in_wall: view.currmode=tt_wall; break;
  case in_corner: view.currmode=tt_pnt; break;
  case in_number+0: view.currmode=0; break;
  default: printf("Unknown showwhat (%d) in ini-file.\n",view.showwhat);
  }
 if(init_test&1)
  printf("Init output on: %d.\n",init_test);
 view.clickphi=cos(view.clickphi);
 normalize(&view.e[0]); 
 normalize(&view.e[1]); 
 VECTOR(&view.e[2],&view.e[1],&view.e[0]); /* left-handed system */
 printf("Reading menuline...\n");
 findmarker(f,"MENUS",&view.nummenus);
 if(view.nummenus<1)
  { printf("Wrong number of menus: %d\n",view.nummenus); exit(0); }
 if((view.menuline=calloc(sizeof(struct objdata),view.nummenus))==NULL)
  { printf("No mem for %d menus.\n",view.nummenus); exit(0); }
 for(i=0;i<view.nummenus;i++)
  {
  view.menuline[i].size=iniread(f,"o",&view.menuline[i]);
  makehotkey((struct objtype *)&view.menuline[i]);
  if((view.menuline[i].data=calloc(sizeof(struct objtype *),
   view.menuline[i].size))==NULL)
   { printf("No mem for %d menupnts in %d. menu.\n",view.menuline[i].size,
      i); exit(0); }
  for(n=0;n<view.menuline[i].size;n++)
   {
   if((view.menuline[i].data[n]=malloc(sizeof(struct objtype)))==NULL)
    { printf("No mem for %d. menupnt in %d. menu.\n",n,i); exit(0); }
   iniread(f,"o",view.menuline[i].data[n]);
   makehotkey(view.menuline[i].data[n]);
   }
  }
 printf("Reading eventcodes...\n");
 findmarker(f,"EVENTCODES",&view.noevents);
 if((view.events=calloc(sizeof(struct eventcode),(size_t)view.noevents))
  ==NULL)
  { printf("No mem for events.\n"); exit(0); }
 for(i=0;i<view.noevents;i++)
  iniread(f,"e",&view.events[i]);
 printf("Reading buttons...\n");
 findmarker(f,"BUTTONS",&view.numbuttons);
 if(view.numbuttons!=in_number)
  { printf("Wrong number of buttons: %d.\n",view.numbuttons); exit(0); }
 if((view.bnames=calloc(sizeof(char *),view.numbuttons))==NULL)
  { printf("No mem for buttons.\n"); exit(0); }
 for(i=0;i<view.numbuttons;i++)
  iniread(f,"s",&view.bnames[i]);
 printf("Reading descent structure...\n");
 findmarker(f,"TxtNames",&n);
 iniread(f,"s",&countedtxts);
 iniread(f,"s",&doortextures);
 for(n=0;n<in_number;n++)
  {
  findmarker(f,view.bnames[n],&view.descripnum[n]);
  view.maxrow[n]=0;
  view.infonum[n]=initinforead(f,&view.info[n],view.descripnum[n],n);
  }
 printf("Readings texture numbers...\n");
 for(i=0,view.numtxtref=0;i<txt_number;i++)
  {
  findmarker(f,txt_names[i],&view.txtnums[i].size);
  if((view.txtnums[i].data=malloc(sizeof(struct objtype *)*
   view.txtnums[i].size))==NULL)
   { printf("No mem for txttable.\n"); exit(0); }
  view.txtnums[i].str=txt_names[i];
  view.txtnums[i].no=i;
  for(n=0;n<view.txtnums[i].size;n++)
   {
   if((view.txtnums[i].data[n]=malloc(sizeof(struct objtype)))==NULL)
    { printf("No mem for objtype in txttable.\n"); exit(0); }
   if(fscanf(f,"%d",&view.txtnums[i].data[n]->no)!=1)
    { printf("Can't read txtnums %d %d\n",i,n); exit(0); }
   view.txtnums[i].data[n]->str=NULL;
   if(view.txtnums[i].data[n]->no<0)
    {
    view.txtnums[i].data[n]->no=-view.txtnums[i].data[n]->no;
    if(fscanf(f,"%d",&j)!=1)
     { printf("Can't read txtnums %d %d\n",i,n); exit(0); }
    while(--j>=0) 
     {
     if((view.txtnums[i].data[++n]=malloc(sizeof(struct objtype)))==NULL)
      { printf("No mem for objtype in txttable.\n"); exit(0); }
     view.txtnums[i].data[n]->no=view.txtnums[i].data[n-1]->no+1;
     view.txtnums[i].data[n]->str=NULL;
     }
    }
   if(view.txtnums[i].data[n]->no+1>view.numtxtref) 
    view.numtxtref=view.txtnums[i].data[n]->no+1;
   }
  }
 findmarker(f,"DoorStarts",&maxanimtxt);
 /* make doornums */
 iniread(f,"d",&view.doornums.size);
 if((view.doornums.data=malloc(sizeof(struct objtype *)*view.doornums.size))
  ==NULL)
  { printf("No mem for doornums.\n"); exit(0); }
 if((view.doorstarts=malloc(sizeof(int)*maxanimtxt))==NULL)
  { printf("No mem for doorstarts.\n"); exit(0); }
 view.doornums.str=NULL;
 for(i=0,j=0;j<maxanimtxt;j++)
  {
  if(fscanf(f,"%d",&n)!=1)
   { printf("Can't read doornums.\n"); exit(0); }
  if(n!=0) 
   {
   if(i>=view.doornums.size)
    { printf("Too many doornums."); exit(0); }
   if((view.doornums.data[i]=malloc(sizeof(struct objtype)))==NULL)
    { printf("No mem for doornums data.\n"); exit(0); }
   view.doornums.data[i++]->no=j;
   view.doorstarts[j]=n;
   }
  }
 view.macros.no=0; 
 view.macros.str="Macro Groups";
 findmarker(f,"MacroGroups",&view.macros.size);
 if((view.macros.data=malloc(sizeof(struct macrogroup *)*view.macros.size))==
  NULL)
  { printf("No mem for dir of macrogroups (no=%d).\n",view.macros.size);
    exit(0); }
 for(i=0;i<view.macros.size;i++)
  {
  if((mg=malloc(sizeof(struct macrogroup)))==NULL)
   { printf("No mem for macrogroup.\n"); exit(0); }
  iniread(f,"o",&mg->ot);
  mg->ot.no=i;
  initlist(&mg->macros);
  view.macros.data[i]=(struct objtype *)mg;
  }
 fclose(f); f=NULL; 
 strcpy(text,view.macropath);
#ifdef SHAREWARE
 strcat(text,"*.smc");
#else
 strcat(text,"*.rmc");
#endif
 if(findfirst(text,&macfiles,FA_ARCH|FA_RDONLY)==0)
  do
   {
   if((m=malloc(sizeof(struct macro)))==NULL)
    { printf("No mem for macro in file %s\n",macfiles.ff_name); exit(0); }
   view.pcurrmacro=NULL;
   m->longtxt=NULL;
   if((m->filename=malloc(strlen(macfiles.ff_name)+1))==NULL)
    { printf("No mem for macro filename: %s\n",macfiles.ff_name); exit(0); }
   strcpy(m->filename,macfiles.ff_name);
   strcpy(text,view.macropath);
   strcat(text,m->filename);
   if(init_test&1) fprintf(errf,"Found macro file %s\n",m->filename);
   if(f!=NULL) fclose(f);
   if((f=fopen(text,"rb"))==NULL)
    { printf("Can't open macrofile %s\n",text); exit(0); }
   m->shorttxt=NULL;
   if(!readmacroheader(f,m)) 
    { printf("Can't read macroheader (see devil.err)\n"); exit(0); }
   if(init_test&1)
    fprintf(errf,"GroupNo.: %d STxt: %s\n",m->groupno,m->shorttxt);
   if(m->groupno<0 || m->groupno>=view.macros.size)
    { printf("Unknown macrogroup: %d (macrofile=%s).\n",
       m->groupno,macfiles.ff_name); m->groupno=0; }
   mg=(struct macrogroup *)view.macros.data[m->groupno];
   if(init_test&1)
    fprintf(errf,"Adding %d. macro to group %d.\n",mg->macros.size,
     m->groupno);
   addnode(&mg->macros,mg->macros.size,m);
   }
  while(findnext(&macfiles)==0);
 if(f) fclose(f);
 printf("Init objtypes...\n");
 for(n=0;n<in_number;n++)
  initobjtypelists(view.info[n],view.descripnum[n]);
 if(init_test&1)
  for(n=0;n<in_number;n++)
   {
   fprintf(errf,"LIST %d *********************\n",n);
   printobjtypelists(view.info[n],view.descripnum[n],0);
   }
 printf("Reading macrodir...\n");
 if((view.keymacros=calloc(sizeof(struct macro *),view.numkeymacros))==
  NULL)
  { printf("No mem for buffer for key macros.\n"); exit(0); }
 initlist(&l->cubes); initlist(&l->pts);
 initlist(&l->doors); initlist(&l->things);
 initlist(&l->sdoors); initlist(&l->producers);
 for(i=0;i<tt_number;i++)
  initlist(&view.tagged[i]);
 /* init textures */
 printf("Reading texture directory...\n");
 readtxts();
 printf("Init textures...\n");
 if(init_test&1)
  fprintf(errf,"Numtxtref: %d\n",view.numtxtref);
 if((view.txtref=malloc(sizeof(int)*view.numtxtref))==NULL)
  { printf("No mem for texture reference (num=%d)\n",view.numtxtref); 
    exit(0); }
 for(i=0;i<view.numtxtref;i++) view.txtref[i]=NULL;
 /* set in txtref for every existing texture a pointer */
 for(i=0;i<txt_number;i++)
  for(n=0;n<view.txtnums[i].size;n++)
   view.txtref[view.txtnums[i].data[n]->no]=
    (struct texture *)view.txtnums[i].data[n]; /* abuse pointer */
 for(i=0,n=0;i<view.numtxtref;i++)
  if(view.txtref[i]) 
   {
   if(n<view.numtxtdir)
    {
    /* now set name of txtnums */
    ((struct objtype *)view.txtref[i])->str=view.txts[view.txtoffset+n].name;
    /* and the right pointer */
    view.txtref[i]=&view.txts[view.txtoffset+n];
    if(init_test&1)
     fprintf(errf,"%s, %d %d\n",view.txts[view.txtoffset+n].name,n,i);
    /* jump over the rest of animation textures if it's no door */
    strcpy(text,view.txts[view.txtoffset+n].name);
    if((pos=strpbrk(text," 0123456789"))!=NULL) *pos=0;
    if(strstr(doortextures,text)==NULL)
     while(view.txts[view.txtoffset+n].ruxsize==
      view.txts[view.txtoffset+n+1].ruxsize-1) n++;
    /* and jump over others than side textures */
    do
     {
     strcpy(text,view.txts[view.txtoffset+(++n)].name);
     if((pos=strpbrk(text," 0123456789"))!=NULL)
      *pos=0;
     }
    while(strstr(countedtxts,text)==NULL);
    }
   else
    {printf("Too many textures %d>=%d (%d).\n",n,view.numtxtdir,i); exit(0);}
   }
 free(countedtxts);
 free(doortextures);
 copytxtnums(txt1_wall,txt1_normal);
 copytxtnums(txt1_wall,txt2_normal);
 copytxtnums(txt2_wall,txt2_normal);
 /* Insert nothing in txt2 list (must be here, otherwise nothing is not
    at the beginning */
 insertnothing(txt2_normal);
 insertnothing(txt2_wall);
 for(i=0;i<view.doornums.size;i++)
  {
  if(view.doornums.data[i]->no<0 || view.doornums.data[i]->no>=maxanimtxt)
   { printf("Wrong animtexture: %d\n",view.doornums.data[i]->no); exit(0); }
  if(view.doorstarts[view.doornums.data[i]->no]<=-view.numtxtref ||
   view.doorstarts[view.doornums.data[i]->no]>=view.numtxtref)
   { printf("Wrong doorstart: %d\n",
      view.doorstarts[view.doornums.data[i]->no]); exit(0); }
  if((texture=view.txtref[view.doorstarts[view.doornums.data[i]->no]<0 ?
   -view.doorstarts[view.doornums.data[i]->no] :
    view.doorstarts[view.doornums.data[i]->no]])==NULL)
   { printf("Doorstart Texture not known: %d\n",
      view.doorstarts[view.doornums.data[i]->no]); exit(0); }
  view.doornums.data[i]->str=texture->name;
  }
 }

