/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_ins.c - functions handling an event for inserting&deleting.
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
#include "readlvl.h"
#include "tag.h"
#include "insert.h"
#include "macros.h"
#include "plot.h"
#include "options.h"
#include "do_event.h"
#include "do_ins.h"

void dec_newlevel(int ec)
 {
 struct leveldata *ld,*old_l;
 const char *pigname=init.d_ver>=d2_10_sw ? "GROUPA.256" : "DESCENT.256";
 ld=emptylevel(); 
 checkmem(ld->fullname=MALLOC(strlen(TXT_STDLEVELNAME)+1));
 strcpy(ld->fullname,TXT_STDLEVELNAME);
 checkmem(ld->pigname=MALLOC(strlen(pigname)+1));
 strcpy(ld->pigname,pigname);
 old_l=l; l=ld; ld->pcurrcube=newcube(ld); l=old_l;
 ld->currwall=3; ld->pcurrwall=ld->pcurrcube->d.c->walls[ld->currwall];
 ld->pcurrpnt=ld->pts.head;
 newlevelwin(ld,0);
 view.pcurrthing=insertthing(view.pcurrcube,NULL);
 view.pcurrthing->d.t=changething(view.pcurrthing->d.t,NULL,
  4,view.pcurrcube->d.c);
 view.currwall=0;
 drawopts(); plotlevel();
 }
 
void dec_makemacro(int ec)
 { 
 if(l==NULL || l->pcurrcube==NULL) { printmsg(TXT_NOLEVEL); return; }
 if(view.pcurrcube->d.c->nc[view.currwall]!=NULL &&
  view.pcurrcube->d.c->nc[view.currwall]->d.c->tagged!=NULL)
  { printmsg(TXT_MAKEMACROCONNCUBE); return; }
 untag(tt_cube,view.pcurrcube); /* currcube must be head */
 checkmem(view.pcurrcube->d.c->tagged=  
  addheadnode(&l->tagged[tt_cube],view.pcurrcube->no,view.pcurrcube));
 view.pcurrmacro=buildmacro(l);
 if(view.pcurrmacro!=NULL)
  { printmsg(TXT_MACROMADE); newlevelwin(view.pcurrmacro,1); }
 else waitmsg(TXT_CANTMAKEMACRO);
 plotlevel();
 }
 
void dec_savemacro(int ec) 
 { 
 char *fname;
 if(!view.pcurrmacro) { printmsg(TXT_NOMACRO); return; }
 if((fname=getfilename(&init.macropath,view.pcurrmacro->filename,
  init.levelext,TXT_SAVEMACRO,1))!=NULL)
  {
  if(!savelevel(fname,view.pcurrmacro,0,1,init.d_ver,0))
   waitmsg(TXT_CANTSAVEMACRO,view.pcurrmacro->fullname);
  else printmsg(TXT_MACROSAVED,view.pcurrmacro->fullname,fname);
  FREE(fname);
  }
 }

void dec_loadmacro(int ec) 
 {
 char *fname;
 if((fname=getfilename(&init.macropath,NULL,
  ec==ec_readdbbfile ? "BLK" : init.levelext,TXT_LOADMACRO,0))!=
  NULL)
  {
  if((view.pcurrmacro=ec==ec_readdbbfile ? readdbbfile(fname) :
   readlevel(fname))==NULL) 
   waitmsg(TXT_CANTOPENMACRO,fname);
  else 
   { printmsg(TXT_MACROLOADED,view.pcurrmacro->fullname);
     newlevelwin(view.pcurrmacro,1); }
  FREE(fname);
  }
 }

void dec_insert(int ec)
 { 
 float scale=1.0;
 int warn=0,i,pn,special=(ec==ec_insspecial || ec==ec_insfastspecial),
  fast=(ec==ec_insertfast || ec==ec_insfastspecial);
 struct node *n;
 if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
 switch(view.currmode)
  {
  case tt_cube: 
   if(!view.pcurrmacro) { printmsg(TXT_NOMACRO); return; }
   view.drawwhat|=DW_ALLLINES;
   if(view.pcurrmacro==l) { printmsg(TXT_MACROITSELF); return; }
   if(special)
    if(!getfloat(TXT_GETSCALE,NULL,4,1,&scale)) return;
    else if(scale<0.1 || scale>10.0)
     { waitmsg(TXT_SCALERANGE,scale); return; }
   if(insertmacro(view.pcurrmacro,fast,scale)!=0) 
    {
    untagall(tt_cube); untagall(tt_door); untagall(tt_thing);
    for(n=l->cubes.head;n->no<view.pcurrmacro->cubes.size;n=n->next)
     tag(tt_cube,n);
    for(n=l->doors.head;n->no<view.pcurrmacro->doors.size;n=n->next)
     tag(tt_door,n);
    for(n=l->things.head;n->no<view.pcurrmacro->things.size;n=n->next)
     tag(tt_thing,n);
    l->levelsaved=l->levelillum=0; 
    }
   plotlevel();
   break;
  case tt_wall: 
   warn=(l->pts.size<=MAX_DESCENT_VERTICES);
   view.drawwhat|=DW_ALLLINES;
   if(view.pcurrcube->d.c->nc[view.currwall]!=NULL) 
    {
    if(fast)
     for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
      if(n->d.n->d.c->nc[n->no%6]!=NULL && n->d.n->d.c->d[n->no%6]==NULL)
       deleteconnect(n->d.n,n->no%6,!special);
    if(view.pcurrcube->d.c->d[view.currwall]!=NULL) 
     { printmsg(TXT_DOORINTHEWAY,view.currwall,view.pcurrcube->no); return; }
    deleteconnect(view.pcurrcube,view.currwall,!special);
    }
   else
    if(view.pcurrcube->d.c->nc[view.currwall]==NULL) 
     if(view.pcurrcube->d.c->d[view.currwall]!=NULL)
      printmsg(TXT_CANTINSCUBE);
     else 
      { checkmem(n=insertcube(NULL,NULL,view.pcurrcube,view.currwall,-1.0));
        if(fast) makesidestdshape(n->d.c,view.currwall);
	plotlevel(); }
   drawopt(in_wall); l->levelsaved=l->levelillum=0;
   if(l->pts.size>MAX_DESCENT_VERTICES && warn) waitmsg(TXT_INSTOOMANYPTS);
   break;
  case tt_edge: 
   warn=(l->pts.size<=MAX_DESCENT_VERTICES);
   view.drawwhat|=DW_ALLLINES;
   if(fast)
    for(n=l->tagged[tt_edge].head;n->next!=NULL;n=n->next)
     {
     pn=wallpts[(n->no%24)/4][(n->no%24)%4];
     for(i=0;i<3;i++)
      if(n->d.n->d.c->d[wallno[pn][0][i]]!=NULL) break;
     if(i==3) insertpnt(n->d.n,pn);
     }
   pn=wallpts[view.currwall][view.curredge];
   for(i=0;i<3;i++)
    if(view.pcurrcube->d.c->d[wallno[pn][0][i]]!=NULL) 
     { if(!fast)
        printmsg(TXT_DOORINTHEWAY,wallno[pn][0][i],view.pcurrcube->no); 
       break; }    
   if(i==3) insertpnt(view.pcurrcube,pn);
   if(fast) printmsg(TXT_TAGGEDPOINTSINSERTED);
   else 
    printmsg(TXT_POINTINSERTED,view.curredge,view.pcurrcube->no,
     view.currwall);
   if(l->pts.size>MAX_DESCENT_VERTICES && warn) waitmsg(TXT_INSTOOMANYPTS);
   drawopt(in_wall); drawopt(in_edge); l->levelsaved=l->levelillum=0;
   break;
  case tt_thing:
   if(l->things.size==MAX_DESCENT_OBJECTS && !yesnomsg(TXT_INSTOOMANYTHINGS))
    return;
   if(view.pcurrcube)
    {
    n=view.pcurrthing;
    view.pcurrthing=insertthing(view.pcurrcube,view.pcurrthing);
    if(view.pcurrthing==NULL) view.pcurrthing=n;
    plotcurrent(); drawopt(in_thing); l->levelsaved=0;
    }
   else printmsg(TXT_NOCURRCUBE);
   break;
  case tt_door: 
   if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
   warn=(l->doors.size<=MAX_DESCENT_WALLS);
   if(fast)
    {
    for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
     if((init.d_ver>=d2_10_sw || n->d.n->d.c->walls[n->no%6]==NULL)
      && n->d.n->d.c->d[n->no%6]==NULL)
      checkmem(insertdoor(n->d.n,n->no%6));
    }
   if((init.d_ver>=d2_10_sw||view.pcurrcube->d.c->walls[view.currwall]==NULL)
    && view.pcurrcube->d.c->d[view.currwall]==NULL)
    view.pcurrdoor=insertdoor(view.pcurrcube,view.currwall); 
   else if(!fast) printmsg(view.pcurrcube->d.c->d[view.currwall] ?
    TXT_DOORALREADYTHERE : TXT_WALLINTHEWAY);
   plotlevel(); drawopt(in_wall); drawopt(in_door); 
   l->levelsaved=l->levelillum=0;
   if(warn && l->doors.size>MAX_DESCENT_WALLS) printmsg(TXT_INSTOOMANYDOORS);
   break;
  default: printmsg(TXT_NOINSERTMODE,init.bnames[view.currmode]);
  }
 plotlevel();
 }

void dec_delete(int ec)
 {
 int k;
 struct node *n,*tn,*n2;
 if(l==NULL) printmsg(TXT_NOLEVEL);
 if(view.askone && ec==ec_delete && !yesnomsg(TXT_REALLYDELONE)) return;
 else
  if(view.asktagged && ec==ec_deletefast && l->tagged[view.currmode].size!=0
   && !yesnomsg(TXT_REALLYDELTAGGED,l->tagged[view.currmode].size)) return;
 switch(view.currmode)
  {
  case tt_cube:
   view.drawwhat|=DW_ALLLINES;
   if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
   if(ec==ec_deletefast)
    {
    for(n=l->tagged[tt_cube].tail->prev;n!=NULL;n=n->prev)
     {
     for(k=0;k<6;k++) 
      if(n->next->d.n->d.c->d[k]!=NULL) 
       { untag(tt_door,n->next->d.n->d.c->d[k]);
         deletedoor(n->next->d.n->d.c->d[k]); }
     if(l->cubes.size>1) 
      { l->levelsaved=l->levelillum=0; deletecube(NULL,NULL,n->next->d.n); }
     }
    }
   else if(view.pcurrcube)
    {
    if(l->cubes.size>1)
     {
     for(k=0;k<6;k++)
      if(view.pcurrcube->d.c->d[k]!=NULL)
       { untag(tt_door,view.pcurrcube->d.c->d[k]);
         deletedoor(view.pcurrcube->d.c->d[k]); }
     deletecube(NULL,NULL,view.pcurrcube); l->levelsaved=l->levelillum=0;
     }
    else if(ec!=ec_deletefast) printmsg(TXT_CANTKILLLASTCUBE);
    }
   else if(ec!=ec_deletefast) printmsg(TXT_NOCURRCUBE);
   plotlevel(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
   break;
  case tt_wall:
   if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
   view.drawwhat|=DW_ALLLINES;
   if(ec==ec_deletefast)
    {
    for(n=l->tagged[tt_wall].head->next;n!=NULL;n=n->next)
     if(n->prev->d.n->d.c->nc[n->prev->no%6]==NULL)
      if(connectsides(n->prev->d.n,n->prev->no%6)>0)
       { untag(tt_wall,n->prev->d.n,n->prev->no%6); l->levelsaved=0; }
    }
   else if(view.pcurrcube->d.c->nc[view.currwall]==NULL)
    { if(connectsides(view.pcurrcube,view.currwall)<0 && ec!=ec_deletefast)
       printmsg(TXT_NOCONNCUBEFOUND,view.pcurrcube->no,view.currwall); }
   else if(ec!=ec_deletefast)
    { printmsg(TXT_CUBEALREADYCONN,view.currwall,view.pcurrcube->no);return;}
   l->levelsaved=l->levelillum=0;
   drawopt(in_wall); drawopt(in_edge); plotlevel();
   break;
  case tt_thing:
   if(ec==ec_deletefast)
    {
    for(n=l->tagged[tt_thing].tail->prev;n!=NULL;n=n->prev)
     if(l->things.size>1)
      {
      tn=n->next->d.n; /* is needed because n->next is untagged */
      if(tn==view.pcurrthing) 
       view.pcurrthing=tn->prev->prev ? tn->prev : tn->next;
      if(tn==l->secretstart) l->secretstart=NULL;
      untag(tt_thing,tn);
      if(tn->d.t->nc)
       for(n2=tn->d.t->nc->d.c->things.head->next;n2!=NULL;n2=n2->next)
        if(n2->prev->d.t==tn->d.t) 
	 freenode(&tn->d.t->nc->d.c->things,n2->prev,NULL);
      freenode(&l->things,tn,free);
      l->levelsaved=0;
      }
     else printmsg(TXT_CANTKILLLASTTHING);
    }
   else if(view.pcurrthing && l->things.size>1)
    { n=view.pcurrthing->prev->prev ? view.pcurrthing->prev : 
       view.pcurrthing->next;
      tn=view.pcurrthing; untag(tt_thing,tn);
      if(tn->d.t->nc)
       for(n2=tn->d.t->nc->d.c->things.head->next;n2!=NULL;n2=n2->next)
        if(n2->prev->d.t==tn->d.t) 
	 freenode(&tn->d.t->nc->d.c->things,n2->prev,NULL);
      freenode(&l->things,view.pcurrthing,free); view.pcurrthing=n; 
      l->levelsaved=0; }
   else if(ec!=ec_deletefast) printmsg(TXT_CANTKILLLASTTHING);
   plotlevel(); drawopt(in_thing);
   break;
  case tt_door:
   if(ec==ec_deletefast)
    {
    for(n=l->tagged[tt_door].tail->prev;n!=NULL;n=n->prev)
     {
     if(n->next->d.n->d.d->d && n->next->d.n->d.d->d->d.d->tagged==n) 
      n=n->prev;
     deletedoor(n->next->d.n); l->levelsaved=l->levelillum=0;
     }
    }
   else if(view.pcurrdoor)
    { deletedoor(view.pcurrdoor); l->levelsaved=l->levelillum=0; }
   plotlevel(); drawopt(in_door); drawopt(in_wall);
   break;
  default: printmsg(TXT_NODELETEMODE,init.bnames[view.currmode]);
  }
 }
 
void dec_deletespecial(int ec) 
 {
 enum tagtypes oldmode=view.currmode;
 struct node *n;
 int oldasktagged;
 if(l==NULL) printmsg(TXT_NOLEVEL);
 switch(view.currmode)
  {
  case tt_cube: 
   if(!yesnomsg(TXT_KILLALL,l->tagged[tt_cube].size,l->tagged[tt_door].size,
    l->tagged[tt_thing].size)) return;
   oldasktagged=view.asktagged; view.asktagged=0;
   view.currmode=tt_thing; dec_delete(ec_deletefast);
   view.currmode=tt_door; dec_delete(ec_deletefast);
   view.currmode=tt_cube; dec_delete(ec_deletefast);
   view.currmode=oldmode; view.asktagged=oldasktagged;
   break;
  case tt_wall:
   if(l->tagged[tt_wall].size!=1)
    { printmsg(TXT_SDELTAGONEWALL); return; }
   n=l->tagged[tt_wall].head->d.n;
   view.drawwhat|=DW_ALLLINES;
   if(view.pcurrcube && n)
    {
    if(view.pcurrcube->d.c->nc[view.currwall]==NULL && 
     n->d.c->nc[l->tagged[tt_wall].head->no%6]==NULL)
     { if(connectcubes(NULL,view.pcurrcube,view.currwall,
        l->tagged[tt_wall].head->d.n,l->tagged[tt_wall].head->no%6))
        { untagall(tt_wall); view.pcurrwall=NULL; 
	  l->levelsaved=l->levelillum=0; } }
    else printmsg(TXT_CUBEALREADYCONN,view.currwall,view.pcurrcube->no);
    }
   else printmsg(TXT_NOCURRCUBE);
   plotlevel(); drawopt(in_wall); drawopt(in_edge);
   break;
  default: printmsg(TXT_NOSDELETEMODE,init.bnames[view.currmode]);
  }
 }
 
void dec_makeroom(int ec)
 {
 struct node *n;
 int w;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 untagall(tt_wall);
 for(n=l->tagged[tt_cube].head;n->next!=NULL;n=n->next)
  for(w=0;w<6;w++)
   if(n->d.n->d.c->nc[w]==NULL)
    if(connectsides(n->d.n,w)>0) l->levelsaved=0;
    else tag(tt_wall,n->d.n,w);
 plotlevel(); drawopts(); printmsg(TXT_MAKEROOM);
 }

void dec_splitcube(int ec)
 {
 int i,j,wno[6],ncw[6];
 struct point p[8],np[8],c;
 struct cube *cube;
 struct node *nc[6],*newc[6];
 struct wall w;
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 cube=view.pcurrcube->d.c;
 for(j=0;j<6;j++) if(cube->d[j])
  { printmsg(TXT_SPLITDOOR,view.pcurrcube->no,j); return; }
 view.drawwhat|=DW_ALLLINES; 
 for(j=0;j<3;j++) 
  { for(i=0,c.x[j]=0;i<8;i++) c.x[j]+=p[i].x[j]=cube->p[i]->d.p->x[j];
    c.x[j]/=8; }
 for(j=0;j<3;j++) 
  for(i=0;i<8;i++) np[i].x[j]=(p[i].x[j]-c.x[j])*0.5+c.x[j];
 for(i=0;i<6;i++)
  { nc[i]=cube->nc[i];
    if(nc[i]!=NULL)
     { for(j=0;j<6;j++) if(nc[i]->d.c->nc[j]==view.pcurrcube) break;
       my_assert(j<6); ncw[i]=j;
       deleteconnect(view.pcurrcube,i,1); } }
 for(i=0;i<8;i++) *cube->p[i]->d.p=np[i];
 for(i=0;i<6;i++) 
  {
  w=*cube->walls[i];
  checkmem(newc[i]=insertcube(NULL,NULL,view.pcurrcube,i,-1.0));
  for(j=0;j<6;j++) if(newc[i]->d.c->nc[j]==view.pcurrcube) break;
  my_assert(j<6); wno[i]=oppwalls[j];
  for(j=0;j<4;j++) 
   { *newc[i]->d.c->p[wallpts[wno[i]][j]]->d.p=p[wallpts[i][j]];
     newc[i]->d.c->walls[wno[i]]->corners[j]=w.corners[j]; }
  newc[i]->d.c->walls[wno[i]]->texture1=w.texture1;
  newc[i]->d.c->walls[wno[i]]->texture2=w.texture2;
  newc[i]->d.c->walls[wno[i]]->txt2_direction=w.txt2_direction;
  newc[i]->d.c->walls[wno[i]]->locked=w.locked;
  }
 for(i=0;i<6;i++)
  if(nc[i])
   if(!connectcubes(NULL,newc[i],wno[i],nc[i],ncw[i]))  
    { waitmsg(TXT_SPLITCANTCONNECT,wno[i],nc[i]->no,ncw[i]); 
      plotlevel(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
      return; }   
 for(j=0;j<6;j++)
  for(i=0;i<6;i++)
   if(i!=wno[j] && i!=oppwalls[wno[j]] && newc[j]->d.c->nc[i]==NULL)
    if(!connectsides(newc[j],i))
     { waitmsg(TXT_SPLITCANTCONNECT2,newc[j]->no,i); 
       plotlevel(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
       return; } 
 plotlevel(); drawopt(in_cube); drawopt(in_wall); drawopt(in_edge);
 }

void dec_makestdside(int ec)
 { struct node *n;
   if(!l) { printmsg(TXT_NOLEVEL); return; }
   if(l->tagged[tt_wall].size<1) 
    { printmsg(TXT_NOTHINGTAGGED,init.bnames[tt_wall]); return; }
   l->levelsaved=0;
   for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
    makesidestdshape(n->d.n->d.c,n->no%6); 
   plotlevel(); drawopt(in_pnt); drawopt(in_wall); drawopt(in_edge); }
