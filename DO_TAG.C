/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_tag.c - functions handling an event for tagging.
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
#include "tag.h"
#include "plot.h"
#include "options.h"
#include "do_event.h"
#include "do_tag.h"

void dec_usecubetag(int ec)
 {
 struct node *n;
 int i,j;
 if(view.currmode!=tt_wall && view.currmode!=tt_pnt && view.currmode!=tt_edge)
  { printmsg(TXT_MODEUSECUBETAG); return; }
 untagall(view.currmode);
 for(n=l->tagged[tt_cube].head;n->next!=NULL;n=n->next)
  switch(view.currmode)
   {
   case tt_wall: for(i=0;i<6;i++) tag(tt_wall,n->d.n,i); break;
   case tt_edge: for(i=0;i<6;i++) for(j=0;j<4;j++) tag(tt_edge,n->d.n,i,j);
    break;
   case tt_pnt: for(i=0;i<8;i++) tag(tt_pnt,n->d.n->d.c->p[i]); break;
   default: my_assert(0);
   }
 plotlevel(); drawopt(view.currmode);
 }
 
void dec_usesidetag(int ec)
 {
 struct node *n;
 int j;
 if(view.currmode!=tt_pnt&&view.currmode!=tt_edge)
  { printmsg(TXT_MODEUSESIDETAG); return; } 
 untagall(view.currmode);
 for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
  for(j=0;j<4;j++) 
   if(view.currmode==tt_pnt) tag(tt_pnt,n->d.n->d.c->p[wallpts[n->no%6][j]]);
   else tag(tt_edge,n->d.n,n->no%6,j);
 plotlevel(); drawopt(view.currmode);
 }

void dec_usepnttag(int ec)
 {
 struct node *n,*n2;
 int j;
 if(view.currmode!=tt_edge)
  { printmsg(TXT_MODEUSEPNTTAG); return; } 
 untagall(view.currmode);
 for(n=l->tagged[tt_pnt].head;n->next!=NULL;n=n->next)
  for(n2=n->d.n->d.lp->c.head;n2->next!=NULL;n2=n2->next)
   for(j=0;j<3;j++) tag(tt_edge,n2->d.n,wallno[n2->no][0][j],
    wallno[n2->no][1][j]);
 plotlevel(); drawopt(view.currmode);
 }

void dec_tagspecial(int ec)
 {
 struct node *n;
 int i;
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 switch(view.currmode)
  {
  case tt_edge: 
   for(n=view.pcurrcube->d.c->p[wallpts[view.currwall][view.curredge]]->
    d.lp->c.head;n->next!=NULL;n=n->next)
    for(i=0;i<3;i++)
     if(n->d.n->d.c->walls[wallno[n->no][0][i]])
      switch_tag(tt_edge,n->d.n,wallno[n->no][0][i],wallno[n->no][1][i]);
   break;
  case tt_pnt:
   for(n=view.pcurrpnt->d.lp->c.head;n->next!=NULL;n=n->next)
    for(i=0;i<3;i++)
     if(n->d.n->d.c->walls[wallno[n->no][0][i]])
      switch_tag(tt_edge,n->d.n,wallno[n->no][0][i],wallno[n->no][1][i]);
   break;
  case tt_wall:
   if(view.pcurrcube->d.c->walls[view.currwall]) 
    view.pcurrcube->d.c->walls[view.currwall]->locked^=1;
   break;
  default: ;
  }
 plotlevel(); drawopt(in_wall); drawopt(in_edge);
 }

void dec_tagflatsides(int ec)
 {
 int num_tagged;
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 num_tagged=tagflatsides(view.pcurrcube,view.currwall);
 plotlevel();
 printmsg(TXT_FLATTAGGED,num_tagged);
 }
