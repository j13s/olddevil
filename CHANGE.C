/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    change.c - change a value in the menu.
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
#include "insert.h"
#include "grfx.h"
#include "event.h"
#include "change.h"

extern struct sys_event *se;

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
 double dlight;
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
    free(*(char **)(data+neari->offset));
    if((*(char **)(data+neari->offset)=malloc(strlen(txt)+1))==NULL)
     { fprintf(errf,"No mem for string.\n"); my_exit(); }
    strcpy(*(char **)(data+neari->offset),txt);
    no=(unsigned long)*((unsigned char **)data+neari->offset);
    break;
   case 'd': case 'i': case 'x': /* for char: length=1 to read as int */
    if(neari->length>4)
     { printmsg("Illegal length for %s in changevalue: %d\n",
        neari->txt,neari->length); return 4; }
    inpstr[2]=pos[strlen(pos)-1];
    if(readkeyb(txt,inpstr,&no)==0) return 4;
    break;
   case 'g': case 'f': case 'e': case 'C': case 'D':
    if(readkeyb(txt,"%lg",data+neari->offset)==0) return 4;
    switch(pos[strlen(pos)-1])
     {
     case 'C': /* coord */
      *(double *)(data+neari->offset)*=65536.0; break;
     case 'D': /* degrees*/
      *(double *)(data+neari->offset)/=180/M_PI; break;
     }
    break;
   case 'L': /* light */
    if(readkeyb(txt,"%lg",&dlight)==0) return 4;
    if(dlight>100) dlight=100.0;
    else if(dlight<0) dlight=0.0;
    no=(short int)(dlight*327.67); 
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
 if((neari->od!=NULL || pos==NULL || pos[strlen(pos)-1]=='d' || 
  pos[strlen(pos)-1]=='i' || pos[strlen(pos)-1]=='x' ||
  pos[strlen(pos)-1]=='L') && with_tagged)
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

struct infoitem *findoffsetinfo(struct infoitem *i,int num,int offset)
 {
 struct infoitem *i2,*neari;
 int k;
 unsigned long no;
 for(i2=i;i2-i<num;i2++)
  {
  if(i2->offset<=offset && i2->offset+i2->length>offset)
   return i2;
  for(k=0;k<i2->numchildren;k++)
   if(getno(i,&no))
    if(i2->itemchildren[k]==no)
     if((neari=findoffsetinfo(i2->children[k],i2->numdchildren[k],
      offset))!=NULL) 
      return neari;
  }
 return NULL;
 }
 
#define ROW(y) (((y)-view.bounds[ecw_uppermenu][1])/view.fontheight)
#define COL(x) (((((x)-view.bounds[ecw_uppermenu][0])/view.fontwidth)-5)/3)
#define OFFSET(row,col) (((row)-view.maxrow[view.showwhat]-\
 4+view.menuoffset)*4+(col))
int changedata(void)
 {
 int x1,y1,x2,y2,row,column,oldrow,oldcol,offset,changed;
 struct infoitem *i;
 size_t size;
 struct sys_event new_se;
 unsigned char *data,*olddata,oldbyte;
 if((view.drawwhat&DW_DATA)==0) return 1;
 if((data=getdata(view.showwhat))==0) return 1;
 row=ROW(se->y);
 column=COL(se->x);
 if(row<=0 || row>=view.menuheight || column<0 || column>3) return 1;
 offset=OFFSET(row,column);
 size=getsize(view.showwhat,view.pcurrthing->d.t);
 if(offset<0 || offset>=size) return 1;
 if((olddata=malloc(size))==NULL)
  { fprintf(errf,"No mem for olddata.\n"); my_exit(); }
 memcpy(olddata,data,size);
 do
  {
  x1=(4+column*3)*view.fontwidth+view.fontwidth/2+
   view.bounds[ecw_uppermenu][0];
  y1=row*view.fontheight+view.bounds[ecw_uppermenu][1]+1;
  x2=x1+3*view.fontwidth-1;
  y2=y1+view.fontheight-2;
  sys_filledbox(x1,y1,x2,y2,HILIGHTCOLORS,1);
  i=findoffsetinfo(view.info[view.showwhat],view.descripnum[view.showwhat],
   offset);
  if(i==NULL) printmsg("Unknown");
  else printmsg("D: %s O: %d L: %d",i->txt,i->offset,i->length);
  sys_getevent(&new_se,1);
  sys_filledbox(x1,y1,x2,y2,HILIGHTCOLORS,1);
  oldrow=row; oldcol=column;
  if(new_se.flags==se_f_keypress)
   switch(new_se.key)
    {
    case 587: column--; break;
    case 589: column++; break;
    case 592: row++; break;
    case 584: row--; break;
    default:
     if(isxdigit(new_se.key))
      {
      oldbyte=*(data+offset);
      *(data+offset)=(new_se.key-(isdigit(new_se.key) ? '0' : 'a'-10))*16;
      sys_dumpmenuchar(new_se.key,column*3+5,row);
      sys_getevent(&new_se,1);
      if(!(changed=isxdigit(new_se.key)))
       *(data+offset)=oldbyte;
      else
       *(data+offset)+=new_se.key-(isdigit(new_se.key) ? '0' : 'a'-10);
      printdatabyte(row-1,column*3,data,offset);
      drawmenu();
      if(changed) column++;
      }
    }
  else
   { row=ROW(new_se.y); column=COL(new_se.x); }
  offset=OFFSET(row,column);
  if(offset<0 || offset>=size) { column=oldcol; row=oldrow; }
  if(column<0) { column=3; row--; }
  else if(column>3) { column=0; row++; }
  if(row<1) row=1;
  else if(row>=view.menuheight) row=view.menuheight-1;
  offset=OFFSET(row,column);
  }
 while(new_se.flags!=se_f_keypress || (new_se.key!=27 && new_se.key!=13));
 if(new_se.key==27) memcpy(data,olddata,size);
 return 4;
 }
