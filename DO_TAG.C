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
 int i;
 if(view.currmode!=tt_wall && view.currmode!=tt_pnt)
  { printmsg(TXT_MODEUSECUBETAG); return; }
 untagall(view.currmode);
 for(n=l->tagged[tt_cube].head;n->next!=NULL;n=n->next)
  if(view.currmode==tt_wall) for(i=0;i<6;i++) tag(tt_wall,n->d.n,i);
  else for(i=0;i<8;i++) tag(tt_pnt,n->d.n->d.c->p[i]);
 plotlevel(); drawopt(view.currmode);
 }
 
void dec_usesidetag(int ec)
 {
 struct node *n;
 int j;
 if(view.currmode!=tt_pnt) { printmsg(TXT_MODEUSESIDETAG); return; }
 untagall(tt_pnt);
 for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
  for(j=0;j<4;j++) tag(tt_pnt,n->d.n->d.c->p[wallpts[n->no%6][j]]);
 plotlevel(); drawopt(in_point);
 }

void dec_tagspecial(int ec)
 {
 struct node *n;
 int i,tst;
 fprintf(errf,"KUCKUCK\n");
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 if(view.currmode!=tt_wall)
  {
  if(ec==ec_tagspecialall)
   {
   if(l->tagged_corners.size>0)
    for(n=l->tagged_corners.head->next;n!=NULL;n=n->next)
     untagcorner(n->prev->d.n,(n->prev->no%24)/4,(n->prev->no%24)%4);
   else
    for(n=l->cubes.head;n->next!=NULL;n=n->next)
     for(i=0;i<6;i++)
      if(n->d.c->walls[i]) for(tst=0;tst<4;tst++) tagcorner(n,i,tst);
   }
  else
   {
   if(view.pcurrcube->d.c->walls[view.currwall]->tagged[view.currpnt])
    untagcorner(view.pcurrcube,view.currwall,view.currpnt);
   else
    tagcorner(view.pcurrcube,view.currwall,view.currpnt);
   }
  }
 else
  if(ec==ec_tagspecialall)
   {
   for(n=l->cubes.head,tst=1;n->next!=NULL && tst;n=n->next)
    for(i=0;i<6 && tst;i++) 
     if(n->d.c->walls[i] && n->d.c->walls[i]->locked) tst=0;
   for(n=l->cubes.head;n->next!=NULL;n=n->next)
    for(i=0;i<6;i++) 
     if(n->d.c->walls[i]) n->d.c->walls[i]->locked=tst;
   }
  else
   if(view.pcurrcube->d.c->walls[view.currwall]) 
    view.pcurrcube->d.c->walls[view.currwall]->locked^=1;
 plotlevel(); drawopt(in_wall); drawopt(in_point);
 }

void dec_tagflatsides(int ec)
 {
 int num_tagged;
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 num_tagged=tagflatsides(view.pcurrcube,view.currwall);
 plotlevel();
 printmsg(TXT_FLATTAGGED,num_tagged);
 }
