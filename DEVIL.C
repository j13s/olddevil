/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    devil.c - main()
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
#include "plot.h"
#include "grfx.h"
#include "readlvl.h"
#include "initio.h"
#include "event.h"
#include "do_event.h" /* just for enum evcodes */
#include "do_bmap.h"
#include <dos.h>

FILE *errf;
struct viewdata view;
struct leveldata *l;

void my_exit(void)
 {
 closegrph();
 exit(0);
 }

int main(int argn,char *argc[])
 {
 int evnr,mblocked;
 MouseEvent me;
 struct node *n;
 int oldnumlock=-1;
 char *fname;
#ifdef TEST
 errf=fopen("output.tst","w");
#else
 errf=fopen("devil.err","w");
#endif
 /* ok ok, l should not be of leveldata * but of leveldata,
  but I didn't feel like changing all the references */
 if((l=malloc(sizeof(struct leveldata)))==NULL)
  { printf("No mem for level.\n"); exit(0); }
 initeditor("devil.ini");
 initgrph(); if(!getnumlock()) numlockonoff();
 fname=l->lname;
#ifdef SHAREWARE
 readlvl(fname,l);
#else
 readlvl(fname,l);
#endif
 free(fname);
 evnr=0x6;
 setcbrk(0);
 do
  {
  if((evnr&0x2)!=0)
   {
   mblocked=MouseBlock(NULL,view.xoffset,view.yoffset,view.xsize,view.ysize);
   if((view.drawwhat&DW_BITMAP)!=0)
    { if(view.pcurrwall) plotbitmap(view.pcurrwall); }
   else
    {
    makeview();
    if((view.drawwhat&DW_CUBES)!=0)
     for(n=l->cubes.head;n->next!=NULL;n=n->next)
      plotcube(n,0);
    if((view.drawwhat&DW_THINGS)!=0)
     for(n=l->things.head;n->next!=NULL;n=n->next)
      plotthing(n,0);
    if((view.drawwhat&DW_DOORS)!=0)
     for(n=l->doors.head;n->next!=NULL;n=n->next)
      plotdoor(n,0);   
    }
   MouseUnBlock(mblocked);
   }
  if((view.drawwhat&DW_BITMAP)==0)
   {
   if((evnr&0x4)!=0) drawmenu(); 
   if((evnr&0x6)!=0) plotcurrent();  
   }
  MouseGetEvent(M_POLL|M_BUTTON_CHANGE|M_KEYPRESS,&me);
  if(oldnumlock!=getnumlock()) oldnumlock=drawnumlock();
  evnr=event(&me);
  }
 while(evnr);
 closegrph();
 return 1;
 }
