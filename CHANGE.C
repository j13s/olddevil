/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    change.c - change a value in the menu.
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
#include "insert.h"
#include "event.h"
#include "change.h"

extern MouseEvent *me;

#include "do_side.c"
 
/* the user wants to change the value of neari */
int changevalue(struct infoitem *neari,int with_se,int with_tagged)
 {
 char *pos,txt[100],*inpstr="%l?";
 unsigned long no;
 unsigned char *data;
 struct node *n;
 struct node **curr,*oldcurr;
 int oldpnt,oldwall,ino;
 pos=strchr(neari->txt,'%');
 if(neari->od==NULL && pos!=NULL && pos[1]!=' ')
  /* so if there are no object types and no % only the side effect will
    be done */
  {
  if((data=getdata(neari->infonr))==NULL)
   { printmsg("Can't get data."); return 1; }
  if(pos-neari->txt>99) pos=&neari->txt[99];
  memcpy(txt,neari->txt,pos-neari->txt);
  txt[pos-neari->txt]=0;
  switch(pos[strlen(pos)-1])
   {
   case 's':     
    if(!readkeyb(txt,"%[^\n]",txt)) return 4;
    free(*(unsigned char **)(data+neari->offset));
    if((*(unsigned char **)(data+neari->offset)=malloc(strlen(txt)+1))==NULL)
     { fprintf(errf,"No mem for string.\n"); my_exit(); }
    strcpy(*(unsigned char **)(data+neari->offset),txt);
    no=(unsigned long)*((unsigned char **)data+neari->offset);
    break;
   case 'd': case 'i': case 'x': /* for char: length=1 to read as int */
    if(neari->length>4)
     { printmsg("Illegal length for %s in changevalue: %d\n",
        neari->txt,neari->length); return 4; }
    inpstr[2]=pos[strlen(pos)-1];
    if(readkeyb(txt,inpstr,&no)==0) return 4;
    break;
   case 'g': case 'f': case 'e': 
    if(readkeyb(txt,"%lg",data+neari->offset)==0) return 4;
    break;
   default:
    fprintf(errf,"Unknown input type %c for %s in changevalue.\n",
     pos[strlen(pos)-1],neari->txt); my_exit();
   }
  }
 else if(neari->od!=NULL && neari->offset>=0)
  {
  if(!getno(neari,&no)) no=0;
  ino=no;
  if(readltype(NULL,neari->txt,neari->od,1,&ino,NULL)==0) return 4;
  no=ino;
  }
 else if(!getno(neari,&no)) no=0;
 if(neari->sideeffs!=NULL && with_se)
  dosideeffects(neari,no);
 if(neari->sidefuncnr>=0 && neari->sidefuncnr<sc_number)
  no=do_extrasideeffect[neari->sidefuncnr](neari,0,no);
 else
  if(neari->sidefuncnr!=-1)
   printmsg("Unknown sideeffect function %d",neari->sidefuncnr);
 setno(neari,no);
 if((neari->od!=NULL || pos[strlen(pos)-1]=='d' || pos[strlen(pos)-1]=='i' ||
  pos[strlen(pos)-1]=='x' || pos==NULL) && with_tagged)
  /* only integer values are done with all tagged objects */
  {
  oldpnt=view.currpnt; oldwall=view.currwall;
  switch(view.currmode)
   {
   case tt_cube: case tt_wall: case tt_pnt: curr=&view.pcurrcube; break;
   case tt_door: curr=&view.pcurrdoor; break;
   case tt_thing: curr=&view.pcurrthing; break;
   default: printmsg("Illegal mode in change value of tagitems."); return 4;
   }
  oldcurr=*curr;
  for(n=view.tagged[view.currmode].head;n->next!=NULL;n=n->next)
   {
   if(view.currmode==tt_thing && n->d.n->d.t->type1!=oldcurr->d.t->type1)
    continue;
   if(view.currmode==tt_wall) view.currwall=n->no%6;
   else if(view.currmode==tt_pnt)
    { view.currwall=(n->no%24)/4; view.currpnt=(n->no%24)%4; }
   *curr=n->d.n;
   setno(neari,no);
   if(neari->sideeffs!=NULL && with_se) dosideeffects(neari,no);
   if(neari->sidefuncnr>=0 && neari->sidefuncnr<sc_number)
    {
    do_extrasideeffect[neari->sidefuncnr](neari,1,no);
    setno(neari,no);
    }
   else
    if(neari->sidefuncnr!=-1)
     printmsg("Unknown sideeffect function %d",neari->sidefuncnr);
   }
  *curr=oldcurr;
  view.currwall=oldwall; view.currpnt=oldpnt;
  }
 return 6;
 }

