/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    askcfg.c - configuring of Devil
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
#include "title.h"
#include "version.h"
#include "tools.h"
#include "askcfg.h"

#define NUM_GRFXDRVS 5 
const char *grfxdrvs[NUM_GRFXDRVS]=
 { "VESA","S3VESA","ET4000","CL5426","MACH64" };

void makedospath(const char *oldpath,char *newpath)
 { int i;
   ws_makepath(oldpath,newpath);
   for(i=0;i<strlen(newpath);i++) if(newpath[i]=='/') newpath[i]='\\'; }
   
#ifdef GER
#include <german/askcfg.c>
#else
#include <english/askcfg.c>
#endif

int writeconfig(int olddata)
 {
 char devilpath[200],d1path[200],d2path[200],missionpath[200],*h;
 int i,d_ver,grfxdriver,resolution;
 FILE *cfgfile;
 if(olddata)
  {
  if((h=getenv("DEVILPATH"))!=NULL) makedospath(h,devilpath);
  else devilpath[0]=0;
  makedospath(init.pigpaths[d1_14_reg],d1path);
  makedospath(init.pigpaths[d2_10_reg],d2path);
  d_ver=init.d_ver>=d2_10_sw ? 2 : 1;
  if((h=getenv("GRX20DRV"))!=NULL)
   for(i=0,grfxdriver=0;i<NUM_GRFXDRVS;i++)
    if(!strcmp(h,grfxdrvs[i])) { grfxdriver=i; break; }
  for(i=0,resolution=0;i<NUM_RESOLUTIONS;i++)
   if(init.xres==res_xysize[i][0] && init.yres==res_xysize[i][1])
    { resolution=i; break; }
  }
 else
  { 
  makedospath(".",devilpath);
  if((h=strrchr(devilpath,'/'))!=NULL) *h=0;
  makedospath("/DESCENT",d1path);
  makedospath("/DESCENT2",d2path);
  resolution=0; grfxdriver=0; d_ver=2;
  }
 missionpath[0]=0;
 if(!askconfigdata(&d_ver,devilpath,d1path,d2path,missionpath,&grfxdriver,
  &resolution)) { printf(TXT_CFGFILENOTWRITTEN); ws_waitforkey(); exit(1); }
 if((cfgfile=fopen(init.cfgname,"w"))==NULL)
  { printf(TXT_CANTWRITECFG,init.cfgname); ws_waitforkey(); exit(2); }
 ws_makepath(d1path,d1path); ws_makepath(d2path,d2path);
 ws_makepath(missionpath,missionpath);
 fprintf(cfgfile,VERSION"\n");
 fprintf(cfgfile,":PATHS %d\n",desc_number);
 fprintf(cfgfile,"{%s}\n",missionpath);
 fprintf(cfgfile,"{descent2.ham}\n{}\n");
 fprintf(cfgfile,"{%s}\n{%s}\n",d1path,d1path);
 fprintf(cfgfile,"{}\n{%s}\n{%s}\n{%s}\n\n",d2path,d2path,d2path);
 fprintf(cfgfile,":KEYS %d\n",NUM_HOTKEYS);
 for(i=0;i<NUM_HOTKEYS;i++) 
  fprintf(cfgfile,"%d %d %d {%s}\n",hotkeys[i].kbstat,hotkeys[i].key,
   hotkeys[i].event,hotkeys[i].txt);
 fprintf(cfgfile,"\n:DESCENTVERSION %d\n",d_ver);
 fprintf(cfgfile,"\n:RESOLUTION %d\n",resolution);
 fclose(cfgfile);
 remove(init.lastname);
 if((cfgfile=fopen(init.batchfilename,"w"))==NULL)
  { printf(TXT_CANTWRITECFG,init.batchfilename); ws_waitforkey(); exit(2); }
 makedospath(devilpath,devilpath);
 makedospath(d1path,d1path); makedospath(d2path,d2path);
 makedospath(missionpath,missionpath);
 fprintf(cfgfile,"set DESCENTDRIVE=%c\n",
  d_ver>=d2_10_sw ? d2path[0] : d1path[0]);
 fprintf(cfgfile,"set DESCENTPATH=%s\n",d_ver>=d2_10_sw ? d2path : d1path);
 fprintf(cfgfile,"set DESCENTSTART=%s\n",d_ver>=d2_10_sw ?
  "D2.BAT" : "DESCENT.BAT");
 fprintf(cfgfile,"set MISSIONPATH=%s\n",missionpath);
 fprintf(cfgfile,"set DEVILDRIVE=%c\n",devilpath[0]);
 fprintf(cfgfile,"set DEVILPATH=%s\n",devilpath);
 fprintf(cfgfile,"set GRX20DRV=%s\n",grfxdrvs[grfxdriver]);
 fclose(cfgfile);
 printf(TXT_CFGFILEWRITTEN); ws_waitforkey(); exit(1);
 }
 

