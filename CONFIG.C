/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    config.c - handling of the config-file
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
#include "readlvl.h"
#include "initio.h"
#include "tools.h"
#include "userio.h"
#include "grfx.h"
#include "options.h"
#include "tag.h"
#include "do_event.h"
#include "do_mod.h"
#include "readtxt.h"
#include "opt_txt.h"
#include "version.h"
#include "askcfg.h"
#include "config.h"
#include "lac_cfg.h"

#define CFG_FNAME "PLAY%.2d"
#define CFG_CURNAME "TMPDEVIL"

#define CUROBJNO(n) ((n)!=NULL ? n->no : -1)

extern int init_test; 

int savelvlconfig(FILE *f,struct leveldata *ld,int no)
 {
 int i,num_locked;
 struct node *n;
 sortlist(&ld->cubes,0); sortlist(&ld->things,0); sortlist(&ld->doors,0);
 if(fprintf(f,"{%s} * Levelfilename\n",
  ld->filename!=NULL ? ld->filename : "")<0) return 0;
 if(fprintf(f,"%d * Level properly illuminated?\n",ld->levelillum)<0) 
  return 0;
 if(fprintf(f,"%d %d %d %d %d %d * current objects\n",
  CUROBJNO(ld->pcurrcube),CUROBJNO(ld->pcurrpnt),ld->currwall,ld->curredge,
  CUROBJNO(ld->pcurrthing),CUROBJNO(ld->pcurrdoor))<0) return 0;
 if(fprintf(f,"%g %g %g * position of the user\n",ld->e0.x[0],ld->e0.x[1],
  ld->e0.x[2])<0) return 0;
 for(i=0;i<3;i++)
  if(fprintf(f,"%g %g %g * orientation of the user\n",ld->e[i].x[0],
   ld->e[i].x[1],ld->e[i].x[2])<0) return 0;
 for(n=ld->cubes.head,num_locked=0;n->next!=NULL;n=n->next)
  for(i=0;i<6;i++) num_locked+=(n->d.c->walls[i] && n->d.c->walls[i]->locked);
 if(fprintf(f,"%d * Number of locked sides\n",num_locked)<0)
  return 0;
 for(n=ld->cubes.head,num_locked=0;n->next!=NULL;n=n->next)
  for(i=0;i<6;i++) 
   if(n->d.c->walls[i] && n->d.c->walls[i]->locked)
    if(fprintf(f,"%d ",n->no*6+i)<0) return 0;
 for(i=0;i<tt_number;i++)
  {
  if(fprintf(f,"%d * Number of tagged objects %d\n",ld->tagged[i].size,i)<0)
   return 0;
  for(n=ld->tagged[i].head;n->next!=NULL;n=n->next)
   switch(i)
    {
    case tt_edge: if(fprintf(f,"%d ",(n->d.n->no)*24+n->no%24)<0) return 0;
     break;
    case tt_wall: if(fprintf(f,"%d ",(n->d.n->no)*6+n->no%6)<0) return 0;
     break;
    default: if(fprintf(f,"%d ",n->d.n->no)<0) return 0;
    }
  if(fprintf(f,"\n")<0) return 0;
  }
 if(fprintf(f,"\n")<0) return 0;
 return 1;
 }

#define ERRORSAVECFG(f) { fclose(f); \
 waitmsg(TXT_CANTWRITESTATUSFILE,init.lastname); \
 remove(init.lastname); return 0; }
int savestatus(int playlevel)
 {
 FILE *f;
 struct node *n;
 int i,j,k;
 struct leveldata *ld=l;
 char fname[255];
 in_changecurrentlevel(NULL); in_changecurrentlevel(ld); /* so the
  data in the level structure is correct */
 sortlist(&view.levels,0);
 if((f=fopen(init.lastname,"w"))==NULL)
  { waitmsg(TXT_CANTWRITESTATUSFILE,init.lastname); return 0; }
 if(fprintf(f,"* Initdata\n")<0) ERRORSAVECFG(f);
 if(fprintf(f,":INITDATA\n")<0) ERRORSAVECFG(f);
 if(fprintf(f,"%g %g %g %g %g %g %d %g %g\n",view.dist,view.maxvisibility,
  view.movefactor,view.pmovefactor,view.rotangle,view.protangle,
  view.bm_movefactor,view.flatsideangle,view.bm_stretchfactor)<0)
  ERRORSAVECFG(f);
 if(fprintf(f,"%g %g %g %g %g %g %g %d %d %d %d %d\n",
  view.gridlength,view.maxconndist,view.maxuserconnect,view.illum_quarterway,
  view.illum_brightness,view.distcenter,view.minclicksodist,
  (int)view.illum_minvalue,fb_savedata.xpos,fb_savedata.ypos,
  fb_savedata.xsize,fb_savedata.ysize)<0) ERRORSAVECFG(f);
 for(i=0;i<tlw_num;i++)
  if(fprintf(f,"%d %d %d %d %d\n",tl_win[i].zoom.selected,
   tl_win[i].oldxpos,tl_win[i].oldypos,tl_win[i].oldxsize<0 ? 0
   : tl_win[i].oldxsize,tl_win[i].oldysize)<0) ERRORSAVECFG(f);
 fprintf(f,"\n");
 if(fprintf(f,
  "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "\
  "%d %d %d %d %d %g %d %d\n",
  view.displayed_group,view.gridonoff,view.askone,view.asktagged,
  view.drawwhat|DW_ALLLINES,view.currmode,view.movemode,
  CUROBJNO(view.pdeflevel),CUROBJNO(view.pdefcube),view.defwall,
  view.warn_convex,view.warn_doublekeys,view.warn_tworeactors,
  view.warn_thingoutofbounds,view.warn_gridrot,view.doubleclick,
  view.whichdisplay,view.render,corr_win1_xpos,corr_win1_ypos,
  corr_win2_xpos,corr_win2_ypos,view.gamma_corr,view.coord_axis,
  view.flip_y_axis,view.draw_orig_lines,view.littlebulbson,
  view.mouse_flipaxis,view.blinkinglightson,(double)view.timescale,
  view.warn_frameratetoosmall,view.warn_illuminate)<0) 
  ERRORSAVECFG(f);
 if(pig.current_pogname!=NULL)
  { strcpy(fname,init.pogpath); strcat(fname,"/");
    strcat(fname,pig.current_pogname); }
 else strcpy(fname,"NoPogFile");
 if(fprintf(f,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n",init.levelpath,init.pogpath,
  init.macropath,init.missionpath,init.txtlistpath,init.lightname,fname)<0)
  ERRORSAVECFG(f);
 if(!playlevel)
  { for(n=view.levels.head,i=0,j=-1,k=-1;n->next!=NULL;n=n->next)
     if(n->d.lev->filename)
      { if(n->d.lev==l) j=i;
        if(n->d.lev==view.pcurrmacro) k=i; i++; } }
 else { i=view.levels.size; j=l ? l->n->no : -1; 
        k=view.pcurrmacro ? view.pcurrmacro->n->no : -1; }
 if(fprintf(f,"%d %d %d * Number of levels, current level, current macro\n",i,
  j,k)<0) ERRORSAVECFG(f);
 for(n=view.levels.head,i=0,j=-1;n->next!=NULL;n=n->next)
  {
  if(playlevel)
   {
   if(n->d.lev==l) sprintf(fname,"%s/"CFG_CURNAME".%s",init.cfgpath,
    init.levelext);
   else sprintf(fname,"%s/"CFG_FNAME".%s",init.cfgpath,i++,init.levelext);
   i=n->d.lev->levelsaved;
   if(!savelevel(fname,n->d.lev,playlevel>0 ? n->d.lev==l : -1,0,init.d_ver,0))
    ERRORSAVECFG(f);
   n->d.lev->levelsaved=i;
  if(((init.d_ver==d1_10_sw) || (d1_10_reg) || (d1_14_reg)) && l!=NULL)
  {
    char dr[MAXDRIVE], pa[MAXDIR], fi[MAXFILE], ex[MAXEXT];
    char pg1filename[MAXPATH], pg1tmpname[MAXPATH];
    FILE* pg1file, *tmpfile;
    char* data;
    int size;

    fnsplit(l->filename, dr, pa, fi, ex);
    fnmerge(pg1filename, dr, pa, fi, ".pg1");
    pg1file = fopen(pg1filename,"rb");
    if(!pg1file)
    {
      fnmerge(pg1filename, dr, pa, fi, ".dtx");
      pg1file = fopen(pg1filename,"rb");
    }
    if(!pg1file)
    {
      fnmerge(pg1filename, dr, pa, "devil", ".pg1");
      pg1file = fopen(pg1filename,"rb");
    }
    if(!pg1file)
    {
      fnmerge(pg1filename, dr, pa, "devil", ".dtx");
      pg1file = fopen(pg1filename,"rb");
    }
    if(pg1file)
    {
      fnsplit(fname, dr, pa, fi, ex);
      fnmerge(pg1tmpname, dr, pa, fi, ".dtx");
      tmpfile=fopen(pg1tmpname, "wb");
      fseek(pg1file,0,SEEK_END);
      size=ftell(pg1file);
      rewind(pg1file);  
      checkmem(data=MALLOC(size));
      if(fread(data,1,size,pg1file)==size)
      {
        fwrite(data,1,size,tmpfile);
      }
      FREE(data);
      fclose(pg1file);
      fclose(tmpfile);
    }
   }
   fprintf(f,"%s\n",fname);
   }
  else
   if(n->d.lev->filename) 
    {
    if(n->d.lev==l) j=i; i++;
    fprintf(f,"%s\n",n->d.lev->filename);
    }
  }
 for(n=view.levels.head,i=0;n->next!=NULL;n=n->next)
  if(playlevel || n->d.lev->filename)
   {
   if(fprintf(f,":LEVELDATA%.2d %d\n",i++,!playlevel ? 1 :
    n->d.lev->levelsaved)<0)
    ERRORSAVECFG(f);
   if(playlevel) n->d.lev->levelsaved=1; /* that savelvl don't ask */
   if(!savelvlconfig(f,n->d.lev,i)) ERRORSAVECFG(f);
   }
 if(!w_savecfg(f)) ERRORSAVECFG(f);
 fclose(f);
 return 1;
 }

int readlvlconfig(FILE *f,struct leveldata *ld)
 {
 int n0,n1,n2,n3;
 struct node *n;
 char buffer[256];
 if(fscanf(f,"%255[^}]",buffer)!=1) return 0; buffer[255]=0;
 skipline(f);
 if(ld->filename) FREE(ld->filename);
 if(strlen(buffer)>1)
  {
  checkmem(ld->filename=MALLOC(strlen(buffer)));
  strcpy(ld->filename,&buffer[1]);
  }
 if(fscanf(f,"%d",&ld->levelillum)!=1) return 0;
 skipline(f);
 if(fscanf(f,"%d%d%d%d%d%d",&n0,&n1,&ld->currwall,&ld->curredge,&n2,&n3)!=6) 
  return 0;
 skipline(f);
 if(n0>0) ld->pcurrcube=findnode(&ld->cubes,n0);
 if(n1>0) ld->pcurrpnt=findnode(&ld->pts,n1);
 if(!ld->pcurrcube && ld->cubes.size) ld->pcurrcube=ld->cubes.head;
 ld->pcurrwall=ld->pcurrcube==NULL ? NULL : 
  ld->pcurrcube->d.c->walls[ld->currwall];
 if(n2>0) ld->pcurrthing=findnode(&ld->things,n2);
 if(!ld->pcurrthing && ld->things.size) ld->pcurrthing=ld->things.head;
 if(n3>0) ld->pcurrdoor=findnode(&ld->doors,n3);
 if(!ld->pcurrdoor && ld->doors.size) ld->pcurrdoor=ld->doors.head;
 if(fscanf(f,"%g%g%g",&ld->e0.x[0],&ld->e0.x[1],&ld->e0.x[2])!=3) return 0;
 skipline(f);
 for(n1=0;n1<3;n1++)
  {
  if(fscanf(f,"%g%g%g",&ld->e[n1].x[0],&ld->e[n1].x[1],&ld->e[n1].x[2])!=3)
   return 0;
  skipline(f);
  }
 in_changecurrentlevel(ld);
 if(fscanf(f,"%d",&n0)!=1) return 0;
 skipline(f);
 for(n1=0;n1<n0;n1++)
  {
  if(fscanf(f,"%d",&n2)!=1) return 0;
  if((n=findnode(&ld->cubes,n2/6))!=NULL && n->d.c->walls[n2%6])
   n->d.c->walls[n2%6]->locked=1;
  }
 for(n0=0;n0<tt_number;n0++)
  {
  if(fscanf(f,"%d",&n1)!=1) return 0;
  skipline(f);
  for(n2=0;n2<n1;n2++)
   {
   if(fscanf(f,"%d",&n3)!=1) return 0;
   switch(n0)
    {
    case tt_cube:
     if((n=findnode(&ld->cubes,n3))!=NULL) tag(tt_cube,n);
     break;
    case tt_wall:
     if((n=findnode(&ld->cubes,n3/6))!=NULL) tag(tt_wall,n,n3%6);
     break;
    case tt_edge:
     if((n=findnode(&ld->cubes,n3/24))!=NULL) tag(tt_edge,n,(n3%24)/4,
      (n3%24)%4);
     break;
    case tt_pnt:
     if((n=findnode(&ld->pts,n3))!=NULL) tag(tt_pnt,n);
     break;
    case tt_thing:
     if((n=findnode(&ld->things,n3))!=NULL) tag(tt_thing,n);
     break;
    case tt_door:
     if((n=findnode(&ld->doors,n3))!=NULL) tag(tt_door,n);
     break;
    default: return 0;
    }
   }
  skipline(f);
  }
 return 1;
 }
 
#define ERRORREADCFG(f) { fclose(f); \
 waitmsg(TXT_CANTREADSTATUSFILE,init.lastname); \
 remove(init.lastname); return; }
void readstatus(char* lfname)
 {
 FILE *f;
 struct node *n;
 int lno,c,d,e,mno,i;
 char fname[256];
 struct leveldata *ld;
 float new_brightness;
 if((f=fopen(init.lastname,"r"))==NULL)
  { printmsg(TXT_CANTREADSTATUSFILE,init.lastname); return; }
 my_assert(findmarker(f,"INITDATA",&lno));
 if(fscanf(f,"%g%g%g%g%g%g%d%g%g",&view.dist,
  &view.maxvisibility,&view.movefactor,&view.pmovefactor,&view.rotangle,
  &view.protangle,&view.bm_movefactor,&view.flatsideangle,
  &view.bm_stretchfactor)!=9)
  ERRORREADCFG(f);
 skipline(f);
 if(fscanf(f,"%g%g%g%g%g%g%g%hd%d%d%d%d",&view.gridlength,&view.maxconndist,
  &view.maxuserconnect,&view.illum_quarterway,&new_brightness,
  &view.distcenter,&view.minclicksodist,&view.illum_minvalue,
  &fb_savedata.xpos,&fb_savedata.ypos,&fb_savedata.xsize,&fb_savedata.ysize)
  !=12) ERRORREADCFG(f);
 for(i=0;i<tlw_num;i++)
  {
  if(fscanf(f,"%d%d%d%d%d",&tw_savedata[i].zoom,
   &tw_savedata[i].xpos,&tw_savedata[i].ypos,&tw_savedata[i].xsize,
   &tw_savedata[i].ysize)!=5) ERRORREADCFG(f);
  if(tw_savedata[i].xsize>0) tl_win[i].oldxsize=-1;
  }
 if(fscanf(f,
  "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%hd%d%d%d%d%hd%hd%g%d%d",
  &view.displayed_group,&view.gridonoff,&view.askone,&view.asktagged,
  &view.drawwhat,&d,&e,&lno,&c,&view.defwall,&view.warn_convex,
  &view.warn_doublekeys,&view.warn_tworeactors,&view.warn_thingoutofbounds,
  &view.warn_gridrot,&view.doubleclick,&view.whichdisplay,&view.render,
  &corr_win1_xpos,&corr_win1_ypos,&corr_win2_xpos,&corr_win2_ypos,
  &view.gamma_corr,&view.coord_axis,&view.flip_y_axis,
  &view.draw_orig_lines,&view.littlebulbson,&view.mouse_flipaxis,
  &view.blinkinglightson,&view.timescale,&view.warn_frameratetoosmall,
  &view.warn_illuminate)!=32) 
  ERRORREADCFG(f);
 set_illum_brightness(new_brightness);
 view.currmode=d; view.movemode=e;
 skipline(f);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 FREE(init.levelpath); checkmem(init.levelpath=MALLOC(strlen(fname)+1)); 
 strcpy(init.levelpath,fname);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 FREE(init.pogpath); checkmem(init.pogpath=MALLOC(strlen(fname)+1)); 
 strcpy(init.pogpath,fname);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 FREE(init.macropath); checkmem(init.macropath=MALLOC(strlen(fname)+1)); 
 strcpy(init.macropath,fname);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 FREE(init.missionpath); checkmem(init.missionpath=MALLOC(strlen(fname)+1)); 
 strcpy(init.missionpath,fname);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 FREE(init.txtlistpath); checkmem(init.txtlistpath=MALLOC(strlen(fname)+1)); 
 strcpy(init.txtlistpath,fname);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 FREE(init.lightname); checkmem(init.lightname=MALLOC(strlen(fname)+1)); 
 strcpy(init.lightname,fname);
 if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
 if(strcmp(fname,"NoPogFile")!=0)
  { printmsg(TXT_LOOKINGFORPOGFILE,fname); changepogfile(fname); }
 if(c>0 && lno>0)
  if((view.pdeflevel=findnode(&view.levels,lno))!=NULL)
   if((view.pdefcube=findnode(&view.pdeflevel->d.lev->cubes,c))==NULL)
    view.pdeflevel=NULL;
 if(fscanf(f,"%d%d%d",&lno,&d,&mno)!=3) ERRORREADCFG(f);
 skipline(f);
 for(c=0;c<lno;c++)
  {
  if(fscanf(f,"%255s",fname)!=1) ERRORREADCFG(f);
  if(lfname == NULL)
  {
      if((ld=readlevel(fname))==NULL)
          printmsg(TXT_CANTOPENLVL,fname);
      else
          newlevelwin(ld,1);
  }
  }
  if(lfname != NULL)
  {
      if((ld=readlevel(lfname))==NULL)
          printmsg(TXT_CANTOPENLVL,fname);
      else
          newlevelwin(ld,0);
  }
 in_changecurrentlevel(NULL);
  if(lfname == NULL)
  {
 for(c=0,n=view.levels.head;n->next!=NULL && c<lno;c++,n=n->next)
  {
  sprintf(fname,"LEVELDATA%.2d",c);
  my_assert(findmarker(f,fname,&n->d.lev->levelsaved));
  if(!readlvlconfig(f,n->d.lev)) ERRORREADCFG(f);
  in_changecurrentlevel(NULL);
  }
 }
 w_readcfg(f);
 fclose(f);
 if(d>=0) 
  {
  for(e=0,n=view.levels.head;e<d && n->next;n=n->next,e++);
  if(n->next!=NULL) changecurrentlevel(n->d.lev);
  }
 if(mno>=0)
  {
  for(e=0,n=view.levels.head;e<mno && n->next;n=n->next,e++);
  if(n->next!=NULL) view.pcurrmacro=n->d.lev;
  } 
 drawopt(in_internal); changemovemode(view.movemode);
 changecurrmode(view.currmode);
 }
 
const char *txtgroup_txt[11]={NULL,TXT_TOP,TXT_UP,TXT_DOWN,TXT_LIST,TXT_TXT1,
 TXT_TOP,TXT_UP,TXT_DOWN,TXT_LIST,TXT_TXT2 };
int initinforead(FILE *f,struct infoitem **is,int num,enum infos infonr)
 {
 int a,b,iinr=0,read_txt=0;
 struct infoitem *i2;
 if(num==0) *is=NULL; 
 else if((*is=MALLOC(sizeof(struct infoitem)*num))==NULL)
  { printf("No mem for infos.\n"); exit(2); }
 for(i2=*is;i2-*is<num;i2++)
  {
  if(init_test&1)
   fprintf(errf,"Reading infonr: entry %ld num %d\n",(long)(i2-*is),num);
  if(read_txt>0)
   { 
   b=0; 
   *i2=*(i2-read_txt);
   i2->multifuncnr=read_txt;
   i2->txt=(char *)txtgroup_txt[read_txt];
   if(read_txt>5) i2->offset+=2;
   if(++read_txt==(i2->type==it_texture ? 11 : 
    (i2->type==it_thingtexture ? 6 : 5))) read_txt=0; 
   }
  else b=iniread(f,"i",i2);
  i2->tagnr=infonr;
  if((i2->type==it_texture||i2->type==it_dooranim||i2->type==it_thingtexture)
   && i2->multifuncnr==0)
   {
   switch(i2->type)
    {
    case it_texture: num+=10; break;
    case it_thingtexture: num+=5; break;
    case it_dooranim: num+=4; break;
    default: my_assert(0);
    }
   a=i2-*is;
   if((*is=REALLOC(*is,num*sizeof(struct infoitem)))==NULL)
    { printf("No mem for extra info entries.\n"); exit(2); }
   i2=*is+a;
   i2->od=NULL;
   read_txt=1; i2->txt=NULL;
   }
  if(init_test&1)
   fprintf(errf,"Header: %s offset=%d numchild=%d numdesc=%d\n",
    i2->txt,i2->offset,i2->numchildren,b);
  if(i2->numchildren>0)
   {
   if((i2->children=MALLOC(sizeof(struct infoitem *)*i2->numchildren))==NULL)
    { printf("No mem for info children.\n"); exit(2); }
   if((i2->itemchildren=MALLOC(sizeof(unsigned long)*i2->numchildren))==NULL)
    { printf("No mem for info item children.\n"); exit(2); }
   if((i2->numinchildren=MALLOC(sizeof(int)*i2->numchildren))==NULL)
    { printf("No mem for info numinchildren.\n"); exit(2); }
   }
  else
   { i2->children=NULL; i2->itemchildren=NULL; i2->numinchildren=NULL; }
  if(b>0)
   {
   if((i2->od=MALLOC(sizeof(struct objdata)))==NULL)
    { printf("No mem for objdata.\n"); exit(2); }
   i2->od->size=b;
   if((i2->od->data=MALLOC(sizeof(struct objtype *)*
    (size_t)i2->od->size))==NULL)
    { printf("No mem for objtype *.\n"); exit(2); }
   for(b=0,iinr=0;b<i2->od->size;b++) 
    {
    if((i2->od->data[b]=MALLOC(sizeof(struct objtype)))==NULL)
     { printf("No mem for objtype.\n"); exit(2); }
    a=iniread(f,"o",i2->od->data[b]);
    if(a>0) 
     {
     if(iinr>=i2->numchildren)
      { printf("Too many children %d/%d: %d>=%d\n",b,i2->od->size,iinr+1,
         i2->numchildren); exit(2); }
     i2->itemchildren[iinr]=i2->od->data[b]->no;
     i2->numinchildren[iinr]=initinforead(f,&i2->children[iinr],a,infonr);
     iinr++;
     }
    }
   }
  else
   i2->od=NULL;
  }
 return num;
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
    { num=curi->numinchildren[s]; ncuri=curi->children[s]; break; }  
 if(ncuri==NULL)
  { printf("Can't find %d for %s\n",(*ot)->no,(*ot)->str); exit(2); }
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
    { printf("First/Last item of %s not with '!'\n",(*ot)->str); exit(2); }
   if((*lastot)->no<0 || (*lastot)->no>=in_number)
    { printf("Illegal number for starting insert: %d (0-%d)\n",(*lastot)->no,
       in_number-1); exit(2); }
   if(init_test&1)
    fprintf(errf,"Searching in list %d: %d\n",(*lastot)->no,
     init.infonum[(*lastot)->no]);
   curi=findcopytypelist(init.info[(*lastot)->no],
    init.infonum[(*lastot)->no],lastot+1);
   if(curi->od==NULL)
    { printf("No objtypes in source typelist.\n"); exit(2); }
   oldnum=lastot-i->od->data;
   if((i->od->data=REALLOC(i->od->data,(oldnum+curi->od->size)*
    sizeof(struct objtype **)))==NULL)
    { printf("No mem for type list.\n"); exit(2); }
   lastot=&i->od->data[oldnum];
   if(init_test&1)
    fprintf(errf,"Inserting from %s %d to %s %d\n",curi->txt,
     curi->od->size,i->txt,i->od->size);
   for(s=0;s<curi->od->size;s++)
    {
    if(s<i->od->size-oldnum)
     { FREE(lastot[s]->str); FREE(lastot[s]); } 
    lastot[s]=curi->od->data[s];
    if(init_test&1)
     fprintf(errf,"Insert: %d %s\n",lastot[s]->no,lastot[s]->str);
    }
   i->od->size=curi->od->size+oldnum; 
   }
  for(s=0;s<i->numchildren;s++)
   initobjtypelists(i->children[s],i->numinchildren[s]);
  }
 }
  
void printobjtypelists(struct infoitem *is,int num,int indent)
 {
 struct infoitem *i;
 int a;
 for(i=is;i-is<num;i++)
  {
  fprintf(errf,"%d %d %s %d %d %d %d %p\n",indent,i->type,i->txt,i->offset,
   i->length,i->sidefuncnr,i->numchildren,i->children);
  if(i->od!=NULL)
   for(a=0;a<i->od->size;a++)
    fprintf(errf,"  %x %s\n",i->od->data[a]->no,i->od->data[a]->str);
  for(a=0;a<i->numchildren;a++)
   {
   fprintf(errf," CHILD: %lx %d %p\n",i->itemchildren[a],
    i->numinchildren[a],i->children[a]);
   printobjtypelists(i->children[a],i->numinchildren[a],indent+1);
   }
  }
 }
 
int readconfig(void)
 {
 int i,j;
 FILE *f,*hamf;
 char *ininame,*hamname,*hamfname,buffer[300];
 long int hamver;
 lac_read_cfg();
 while((f=fopen(init.cfgname,"r+"))==NULL)
  { printf(TXT_CANTREADCFG,init.cfgname); return 0; }
 if(fscanf(f,"%s",buffer)!=1)
  { printf(TXT_CANTREADCFG,init.cfgname); return 0; }
 if(strcmp(buffer,VERSION))
  { printf(TXT_CFGWRONGVERSION,init.cfgname); return 0; }
 my_assert(findmarker(f,"PATHS",&i));
 if(i!=desc_number) { printf(TXT_MUSTUPDATECFG); return 0; }
 iniread(f,"s",&init.playmsnpath);
 checkmem(init.missionpath=MALLOC(strlen(init.playmsnpath)+1));
 strcpy(init.missionpath,init.playmsnpath);
 iniread(f,"s",&hamname);
 for(i=0;i<desc_number;i++) iniread(f,"s",&init.pigpaths[i]);
 printf(TXT_READKEYS);
 my_assert(findmarker(f,"KEYS",&view.num_keycodes));
 checkmem(view.ec_keycodes=MALLOC(view.num_keycodes*
  sizeof(struct w_keycode)));
 checkmem(view.txt_keycode=MALLOC(view.num_keycodes*sizeof(char *)));
 for(i=0;i<view.num_keycodes;i++) 
  {
  iniread(f,"k",&view.ec_keycodes[i],&view.txt_keycode[i]);
  view.ec_keycodes[i].flags=0;
  if(view.ec_keycodes[i].event<0||view.ec_keycodes[i].event>=ec_num_of_codes)
   { printf("Illegal keycode %#x %d %d\n",view.ec_keycodes[i].kbstat,
      view.ec_keycodes[i].key,view.ec_keycodes[i].event); return 0; }
  }
 my_assert(findmarker(f,"DESCENTVERSION",&i));
 if(i!=d1_14_reg && i!=d1_10_reg && i!=d2_10_reg && i!=d2_11_reg &&
    i!=d2_12_reg) i=d2_12_reg;
 init.d_ver=i;
 my_assert(findmarker(f,"RESOLUTION",&i));
 if(i<0 && i>=NUM_RESOLUTIONS) i=0;
 init.xres=res_xysize[i][0]; init.yres=res_xysize[i][1];
 fclose(f);
 if(init.d_ver>=desc_number)
  { printf(TXT_ILLEGALDESCENTVERSION); return 0; }
 if(ininames[init.d_ver]==NULL)
  { printf(TXT_NOEDITABLEDVER,vernames[init.d_ver]); return 0; }
 if(init.lightname[strlen(init.lightname)-1]=='X')
  {
  init.lightname[strlen(init.lightname)-1]=init.d_ver>=d2_10_sw ? '2' : '1';
  ws_makepath(init.lightname,buffer);
  FREE(init.lightname); checkmem(init.lightname=MALLOC(strlen(buffer)+1));
  strcpy(init.lightname,buffer);
  }
 if(init.d_ver>=d2_10_sw)
  {
  printf(TXT_READCONVTABLEFILE,init.convtablename);
  if((f=fopen(init.convtablename,"r"))==NULL)
   { printf(TXT_CANTOPENCTFILE,init.convtablename); return 0; }
  if(!findmarker(f,"ConvDefault",&i))
   { printf(TXT_CANTFINDCTMARKER,"ConvDefault"); return 0; }
  if(fscanf(f," %d %d",&init.def_t1,&init.def_t2)!=2)
   { printf(TXT_CANTREADCTDEF); fclose(f); return 0; }
  if(!findmarker(f,"ConvTxts",&init.num_convtxts))
   { printf(TXT_CANTFINDCTMARKER,"ConvTxts"); return 0; }
  checkmem(init.convtxts=MALLOC(sizeof(int)*2*init.num_convtxts));
  for(i=0;i<init.num_convtxts;i++)
   if(fscanf(f," %d %d",&init.convtxts[i*2],&init.convtxts[i*2+1])!=2)
    { fclose(f); printf(TXT_CANTREADCTTXT,i); return 0; }
  if(!findmarker(f,"ConvAnims",&init.num_convanims))
   { printf(TXT_CANTFINDCTMARKER,"ConvAnims"); return 0; }
  checkmem(init.convanims=MALLOC(sizeof(int)*2*init.num_convanims));
  for(i=0;i<init.num_convanims;i++)
   if(fscanf(f," %d %d",&init.convanims[i*2],&init.convanims[i*2+1])!=2)
    { fclose(f); printf(TXT_CANTREADCTANIM,i); return 0; }
  fclose(f);
  }
 printf(TXT_READINIFILEFORDV,vernames[init.d_ver]);
 checkmem(ininame=MALLOC(strlen(init.cfgpath)+
  strlen(ininames[init.d_ver])+2));
 strcpy(ininame,init.cfgpath); strcat(ininame,"/"); 
 strcat(ininame,ininames[init.d_ver]);
 if((f=fopen(ininame,"r"))==NULL) 
  { printf(TXT_CANTOPENVERINIFILE,ininame); return 0; }
 FREE(ininame);
 my_assert(findmarker(f,"DefaultPalFile",&i));
 iniread(f,"s",&pig.default_pigname);
 for(i=0;i<in_number;i++)
  {
  my_assert(findmarker(f,init.bnames[i],&init.infonum[i]));
  init.infonum[i]=initinforead(f,&init.info[i],init.infonum[i],i);
  }
 strcpy(init.levelext,extnames[init.d_ver]);
 checkmem(init.alllevelexts=MALLOC(strlen(extnames[d1_14_reg])+
  strlen(extnames[d2_10_reg])+2));
 if(init.d_ver>=d2_10_sw)
  { strcpy(init.alllevelexts,extnames[d2_10_reg]);
    strcat(init.alllevelexts,"."); }
 else init.alllevelexts[0]=0;
 strcat(init.alllevelexts,extnames[d1_14_reg]);
 my_assert(findmarker(f,"DoorNames",&i));
 iniread(f,"s",&pig.anim_txt_names);
 /* Now read all starts of animation textures for walls */
 my_assert(findmarker(f,"DoorStarts",&pig.num_anims));
 checkmem(pig.anims=MALLOC(sizeof(struct texture *)*pig.num_anims));
 checkmem(pig.anim_starts=MALLOC(sizeof(int)*pig.num_anims));
 for(i=0;i<pig.num_anims;i++)
  my_assert(fscanf(f,"%d",&pig.anim_starts[i])==1);
 for(i=0;i<in_number;i++)
  initobjtypelists(init.info[i],init.infonum[i]);
 if(init_test&1)
  for(i=0;i<in_number;i++)
   { fprintf(errf,"LIST %d *********************\n",i);
     printobjtypelists(init.info[i],init.infonum[i],0); }
 if(init.d_ver<d2_10_sw)
  {
  my_assert(findmarker(f,"RdlNums",&pig.num_rdltxts));
  checkmem(pig.rdl_txts=MALLOC(pig.num_rdltxts*sizeof(struct ham_txt)));
  for(i=0;i<pig.num_rdltxts;i++) 
   {
   pig.rdl_txts[i].rdlno=i; pig.rdl_txts[i].pig=NULL;
   for(j=0;j<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE;j++) pig.rdl_txts[i].my_light[j]=0;
   for(j=0;j<txt_number;j++) pig.rdl_txts[i].txtlistno[j]=-1;
   my_assert(fscanf(f,"%hd ",&pig.rdl_txts[i].pigno)==1);
   pig.rdl_txts[i].flags=pig.rdl_txts[i].light=pig.rdl_txts[i].hitpoints=
    pig.rdl_txts[i].xspeed=pig.rdl_txts[i].yspeed=0;
   pig.rdl_txts[i].anim_seq=pig.rdl_txts[i].shoot_out_txt=-1;
   if(init.d_ver<d1_14_reg && pig.rdl_txts[i].pigno>=721)
    pig.rdl_txts[i].pigno+=(773-721);
   }
  }
 else if(init.pigpaths[d2_10_reg] && strlen(init.pigpaths[d2_10_reg])>0)
  {
  hamfname=makepath(init.pigpaths[d2_10_reg],hamname);
  if((hamf=fopen(hamfname,"rb"))==NULL)
   { printf(TXT_CANTFINDHAMFILE,hamfname); return 0; }
  if(fread(buffer,1,4,hamf)!=4)
   { printf(TXT_CANTREADHAMFILE,hamfname); return 0; }
  if(strncmp(buffer,"HAM!",4))
   { printf(TXT_NOHAMFILE,hamfname); return 0; }
  if(fread(&hamver,4,1,hamf)!=1)
   { printf(TXT_CANTREADHAMFILE,hamfname); return 0; }
  if(hamver!=3)
   { printf(TXT_WRONGHAMVERSION,hamfname,(int)hamver,3); return 0; }
  if(fread(&pig.num_rdltxts,4,1,hamf)!=1)
   { printf(TXT_CANTREADHAMFILE,hamfname); return 0; }
  checkmem(pig.rdl_txts=MALLOC(pig.num_rdltxts*sizeof(struct ham_txt)));
  for(i=0;i<pig.num_rdltxts;i++)
   {
   pig.rdl_txts[i].rdlno=i; pig.rdl_txts[i].pig=NULL;
   for(j=0;j<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE;j++) pig.rdl_txts[i].my_light[j]=0;
   for(j=0;j<txt_number;j++) pig.rdl_txts[i].txtlistno[j]=-1;
   if(fread(&pig.rdl_txts[i].pigno,2,1,hamf)!=1)
    { printf(TXT_CANTREADHAMFILE,hamfname); return 0; }
   /* don't know why the pignums in the HAM file are +1 ?? */
   pig.rdl_txts[i].pigno--;
   }
  for(i=0;i<pig.num_rdltxts;i++)
   if(fread(&pig.rdl_txts[i],20,1,hamf)!=1)
    { printf(TXT_CANTREADHAMFILE,hamfname); return 0; }
   else if(init_test&1)
    fprintf(errf,
     "Read HAM: i=%d f=%lx l=%lx hp=%lx an=%hd sot=%hd vx=%hd vy=%hd\n",i,
     pig.rdl_txts[i].flags&0xff,pig.rdl_txts[i].light,
     pig.rdl_txts[i].hitpoints,pig.rdl_txts[i].anim_seq,
     pig.rdl_txts[i].shoot_out_txt,pig.rdl_txts[i].xspeed,
     pig.rdl_txts[i].yspeed); 
  FREE(hamfname);
  }
 else { printf("No texturelist (No Descent 2 directory).\n"); return 0; }
 FREE(hamname);
 if(!newpigfile(pig.default_pigname,NULL))
  { strcpy(buffer,pig.default_pigname); buffer[strlen(buffer)-4]=0; 
    printf("Can't open pigfile '%s'??\n",buffer); return 0; }
 fclose(f);
 return 1;
 }

int saveplaymsn(int savetoddir)
 {
 FILE *f,*sf;
 char fname[200],sfname[200],*data;
 unsigned long size;
 strcpy(fname,init.playmsnpath); 
 strcat(fname,savetoddir ? "/devil" : "/tmpdevil");
 strcat(fname,init.d_ver>=d2_10_sw ? ".mn2" : ".msn");
 if((f=fopen(fname,"w"))==NULL)
  { printmsg(TXT_CANTSAVEPLAYMSN,fname); return 0; }
 fprintf(f,"name = %s\n",savetoddir ? "Devil Level" : "Devil Tmp-Level");
 fprintf(f,"type = normal\n");
 fprintf(f,"num_levels = 1\n");
 fprintf(f,init.d_ver>=d2_10_sw ? "tmpdevil.rl2\n" : "tmpdevil.rdl\n");
 fclose(f);
 strcpy(fname,init.playmsnpath); 
 strcat(fname,savetoddir ? "/devil.hog" : "/tmpdevil.hog");
 if((f=fopen(fname,"wb"))==NULL)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); return 0; }
 if(fwrite("DHF",1,3,f)!=3)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
 if(fwrite(init.d_ver>=d2_10_sw ? "TMPDEVIL.RL2" : "TMPDEVIL.RDL",1,13,f)!=13)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
 strcpy(sfname,init.cfgpath); strcat(sfname,"/"); strcat(sfname,CFG_CURNAME);
 strcat(sfname,init.d_ver>=d2_10_sw ? ".RL2" : ".RDL");
 if((sf=fopen(sfname,"rb"))==NULL)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
 fseek(sf,0,SEEK_END); size=ftell(sf); fseek(sf,0,SEEK_SET);
 if(fwrite(&size,4,1,f)!=1)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); fclose(sf); return 0; }
 checkmem(data=MALLOC(size));
 if(fread(data,1,size,sf)!=size)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); fclose(sf); FREE(data);
    return 0; }
 fclose(sf);
 if(fwrite(data,1,size,f)!=size)
  { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
 FREE(data);
 if(init.d_ver>=d2_12_reg && l!=NULL && pig.pogfile!=NULL)
  {
  if(fwrite("TMPDEVIL.POG",1,13,f)!=13)
   { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
  fseek(pig.pogfile,0,SEEK_END); size=ftell(pig.pogfile);
  rewind(pig.pogfile);  
  if(fwrite(&size,4,1,f)!=1)
   { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
  checkmem(data=MALLOC(size));
  if(fread(data,1,size,pig.pogfile)!=size)
   { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
  if(fwrite(data,1,size,f)!=size)
   { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
  FREE(data);
  }
  if(init.d_ver>=d2_12_reg && l!=NULL)
  {
    char dr[MAXDRIVE], pa[MAXDIR], fi[MAXFILE], ex[MAXEXT];
    char hxmfilename[MAXPATH];
    FILE* hxmfile;

    fnsplit(l->filename, dr, pa, fi, ex);
    fnmerge(hxmfilename, dr, pa, fi, ".hxm");
    hxmfile = fopen(hxmfilename,"rb");
    if(!hxmfile)
    {
      fnmerge(hxmfilename, dr, pa, "devil", ".hxm");
      hxmfile = fopen(hxmfilename,"rb");
    }
    if(hxmfile)
    {
      if(fwrite("TMPDEVIL.HXM",1,13,f)!=13)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
      fseek(hxmfile,0,SEEK_END); size=ftell(hxmfile);
      rewind(hxmfile);  
      if(fwrite(&size,4,1,f)!=1)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
      checkmem(data=MALLOC(size));
      if(fread(data,1,size,hxmfile)!=size)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
      if(fwrite(data,1,size,f)!=size)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
      FREE(data);
      fclose(hxmfile);
    }
  }
  if(((init.d_ver==d1_10_sw) || (d1_10_reg) || (d1_14_reg)) && l!=NULL)
  {
    char dr[MAXDRIVE], pa[MAXDIR], fi[MAXFILE], ex[MAXEXT];
    char pg1filename[MAXPATH], hx1filename[MAXPATH];
    FILE* pg1file, *hx1file;

    fnsplit(l->filename, dr, pa, fi, ex);
    fnmerge(pg1filename, dr, pa, fi, ".pg1");
    pg1file = fopen(pg1filename,"rb");
    if(!pg1file)
    {
      fnmerge(pg1filename, dr, pa, fi, ".dtx");
      pg1file = fopen(pg1filename,"rb");
    }
    if(!pg1file)
    {
      fnmerge(pg1filename, dr, pa, "devil", ".pg1");
      pg1file = fopen(pg1filename,"rb");
    }
    if(!pg1file)
    {
      fnmerge(pg1filename, dr, pa, "devil", ".dtx");
      pg1file = fopen(pg1filename,"rb");
    }
    if(pg1file)
    {
      if(fwrite("TMPDEVIL.PG1",1,13,f)!=13)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
      fseek(pg1file,0,SEEK_END); size=ftell(pg1file);
      rewind(pg1file);  
      if(fwrite(&size,4,1,f)!=1)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
      checkmem(data=MALLOC(size));
      if(fread(data,1,size,pg1file)!=size)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
      if(fwrite(data,1,size,f)!=size)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
      FREE(data);
      fclose(pg1file);
    }
    fnmerge(hx1filename, dr, pa, fi, ".hx1");
    hx1file = fopen(hx1filename,"rb");
    if(!hx1file)
    {
      fnmerge(hx1filename, dr, pa, "devil", ".hx1");
      hx1file = fopen(hx1filename,"rb");
    }
    if(hx1file)
    {
      if(fwrite("TMPDEVIL.HX1",1,13,f)!=13)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
      fseek(hx1file,0,SEEK_END); size=ftell(hx1file);
      rewind(hx1file);  
      if(fwrite(&size,4,1,f)!=1)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); return 0; }
      checkmem(data=MALLOC(size));
      if(fread(data,1,size,hx1file)!=size)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
      if(fwrite(data,1,size,f)!=size)
        { printmsg(TXT_CANTSAVEPLAYHOG,fname); fclose(f); FREE(data); return 0; }
      FREE(data);
      fclose(hx1file);
    }
  }

 fclose(f);
 return 1;
 }
