/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    devil.c - main()
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
#include "system.h"
#include "tools.h"
#include "grfx.h"
#include "readlvl.h"
#include "initio.h"
#include "event.h"
#include "do_event.h" /* just for enum evcodes */
#include "do_bmap.h"

FILE *errf;
struct viewdata view;
struct leveldata *l;

void my_exit(void)
 {
 closegrph();
 exit(0);
 }

enum cmdline_params { clp_nosvga,num_cmdlineparams };
char *cmdline_switches[num_cmdlineparams]={ "VGA" };
char *cmdline_txts[num_cmdlineparams]=
 { "use only VGA-mode (no Super-VGA)" };
int main(int argn,char *argc[])
 {
 int i,j;
 struct sys_event se;
 char *fname;
 printf("Program was compiled with %s\n",SYS_COMPILER_NAME);
 if(sizeof(double)!=8 || sizeof(int)!=4 || sizeof(short int)!=2
  || sizeof(long int)!=4 || sizeof(char)!=1)
  {printf("Wrong double/int size. Check your compiler flags.\n"); exit(0);}
#ifdef TEST
 errf=fopen("output.tst","w");
#else
 errf=fopen("devil.err","w");
#endif
 for(j=1;j<argn;j++)
  {
  char buffer[strlen(argc[j])];
  sscanf(argc[j]," %s",buffer);
  for(i=0;i<strlen(buffer);i++) buffer[i]=toupper(buffer[i]);
  for(i=0;i<num_cmdlineparams;i++)
   {
   if(strcmp(buffer,cmdline_switches[i])==0)
    switch(i)
     {
     case clp_nosvga: supervga=0; break;
     }
   else
    {
    printf("Unknown parameter [%s]. Allowed parameters:\n",argc[j]);
    for(i=0;i<num_cmdlineparams;i++)
     printf("%s -- %s\n",cmdline_switches[i],cmdline_txts[i]);
    exit(0);
    }
   }
  }
 /* ok ok, l should not be of leveldata * but of leveldata,
  but I didn't feel like changing all the references */
 if((l=malloc(sizeof(struct leveldata)))==NULL)
  { printf("No mem for level.\n"); exit(0); }
 initeditor("devil.ini");
 initgrph(); if(!sys_getnumlock()) sys_numlockonoff();
 fname=l->lname;
#ifdef SHAREWARE
 readlvl(fname,l);
#else
 readlvl(fname,l);
#endif
 free(fname);
 sys_disablectrlc();
 rebuild_screen(0xff);
 do
  { sys_getevent(&se,1); drawnumlock(); }
 while(event(&se));
 closegrph();
 return 1;
 }
