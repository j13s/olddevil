/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_event.c - all functions, the user can directly call with a event.
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
#include "macros.h"
#include "readlvl.h"
#include "change.h"
#include "event.h"
#include "do_event.h"
#include "do_bmap.h"
#include "credits.h"

extern MouseEvent *me;

/* returns zero if aborted, one if file is created, two if file is
   overwritten */
int openwritefile(char *simplefn,char **realfn,char *path,char *ext,FILE **f)
 {
 char buffer[strlen(path)+strlen(simplefn)+5],x,*pos;
 int ret=1,i;
 strcpy(buffer,path);  strcat(buffer,simplefn);
 if((pos=strrchr(buffer,'.'))!=NULL) *pos=0;
 for(i=0;i<3;i++) buffer[strlen(buffer)+i+1]=ext[i];
 buffer[strlen(buffer)+4]=0; buffer[strlen(buffer)]='.';
 if((*realfn=malloc(strlen(buffer)-strlen(path)+1))==NULL)
  { printmsg("No mem for filename."); return 0; }
 strcpy(*realfn,&buffer[strlen(path)]);
 for(i=0;i<strlen(*realfn);i++) *(*realfn+i)=toupper(*(*realfn+i));
 if((*f=fopen(buffer,"rb"))!=NULL)
  {
  fclose(*f);
  printmsg("Overwrite file %s?",buffer);
  if(!readkeyb("Overwrite? (y/n)","%c",&x) || x!='y') 
   { printmsg("Not saved."); return 0; }
  ret=2;
  }
 if((*f=fopen(buffer,"wb"))==NULL)
  { printmsg("Can't open file %s",buffer); return 0; }
 return ret;
 }
 
int dec_dummy(enum evcodes e) {return 1;} /* just a dummy that I
 don't have to change all values in levedit.ini if I kill one
 event. */

struct infoitem *searchinfo(struct infoitem *i,int num,int x,int y)
 {
 struct infoitem *i2,*neari;
 unsigned long no;
 int k;
 for(i2=i;i2-i<num;i2++)
  {
  if(i2->row==y && ((i2->column<=x && i2->column+i2->menulength>=x)
   || i2->menulength==0) && i2->class==1)
   return i2;
  for(k=0;k<i2->numchildren;k++)
   {
   if(getno(i2,&no))
    if(i2->itemchildren[k]==no)
     if((neari=searchinfo(i2->children[k],i2->numechildren[k],x,y))!=NULL) 
      return neari;
   }
  }
 return NULL;
 }

int dec_changevalue(enum evcodes e)
 {
 struct infoitem *i;
 /* first find the clicked info item */
 if(me->flags==M_KEYPRESS) i=view.curmenuitem; 
 else
  i=searchinfo(view.info[view.showwhat],view.infonum[view.showwhat],
   (me->x-view.menu.txr_xpos)/view.menu.txr_font->fnt_width,
   (me->y-view.menu.txr_ypos)/view.menu.txr_font->fnt_height-1+
   view.menuoffset);
 if(i==NULL) return 1;
 return changevalue(i,e==ec_changevalue||e==ec_chval_without_tag,
  e==ec_changevalue||e==ec_chval_without_se); 
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
 
#define ROW(y) (((y)-view.menu.txr_ypos)/view.menu.txr_font->fnt_height)
#define COL(x) (((((x)-view.menu.txr_xpos)/view.menu.txr_font->fnt_width)\
 -5)/3)
#define OFFSET(row,col) (((row)-view.maxrow[view.showwhat]-\
 4+view.menuoffset)*4+(col))
int dec_changedata(enum evcodes ec)
 {
 int x1,y1,x2,y2,row,column,oldrow,oldcol,offset,changed;
 struct infoitem *i;
 size_t size;
 MouseEvent new_me;
 unsigned char *data,*olddata,oldbyte;
 if((view.drawwhat&DW_DATA)==0) return 1;
 if((data=getdata(view.showwhat))==0) return 1;
 row=ROW(me->y);
 column=COL(me->x);
 if(row<=0 || row>=view.menuheight || column<0 || column>3) return 1;
 offset=OFFSET(row,column);
 size=getsize(view.showwhat,view.pcurrthing->d.t);
 if(offset<0 || offset>=size) return 1;
 if((olddata=malloc(size))==NULL)
  { fprintf(errf,"No mem for olddata.\n"); my_exit(); }
 memcpy(olddata,data,size);
 do
  {
  x1=(4+column*3)*view.menu.txr_font->fnt_width+
   view.menu.txr_font->fnt_width/2+view.menu.txr_xpos;
  y1=row*view.menu.txr_font->fnt_height+view.menu.txr_ypos+1;
  x2=x1+3*view.menu.txr_font->fnt_width-1;
  y2=y1+view.menu.txr_font->fnt_height-2;
  GrFilledBox(x1,y1,x2,y2,HILIGHTCOLORS|GrXOR);
  i=findoffsetinfo(view.info[view.showwhat],view.descripnum[view.showwhat],
   offset);
  if(i==NULL) printmsg("Unknown");
  else printmsg("D: %s O: %d L: %d",i->txt,i->offset,i->length);
  MouseGetEvent(M_BUTTON_UP|M_KEYPRESS,&new_me);
  GrFilledBox(x1,y1,x2,y2,HILIGHTCOLORS|GrXOR);
  oldrow=row; oldcol=column;
  if(new_me.flags==M_KEYPRESS)
   switch(new_me.key)
    {
    case 587: column--; break;
    case 589: column++; break;
    case 592: row++; break;
    case 584: row--; break;
    default:
     if(isxdigit(new_me.key))
      {
      oldbyte=*(data+offset);
      *(data+offset)=(new_me.key-(isdigit(new_me.key) ? '0' : 'a'-10))*16;
      GrDumpChar(new_me.key,column*3+5,row,&view.menu);
      MouseGetEvent(M_KEYPRESS,&new_me);
      if(!(changed=isxdigit(new_me.key)))
       *(data+offset)=oldbyte;
      else
       *(data+offset)+=new_me.key-(isdigit(new_me.key) ? '0' : 'a'-10);
      printdatabyte(row-1,column*3,data,offset);
      GrDumpText(column*3+5,row,2,1,&view.menu);
      if(i!=NULL)
       markdatabyte(i,offset-i->offset);
      if(changed) column++;
      }
    }
  else
   { row=ROW(new_me.y); column=COL(new_me.x); }
  offset=OFFSET(row,column);
  if(offset<0 || offset>=size) { column=oldcol; row=oldrow; }
  if(column<0) { column=3; row--; }
  else if(column>3) { column=0; row++; }
  if(row<1) row=1;
  else if(row>=view.menuheight) row=view.menuheight-1;
  offset=OFFSET(row,column);
  }
 while(new_me.flags!=M_KEYPRESS || (new_me.key!=27 && new_me.key!=13));
 if(new_me.key==27) memcpy(data,olddata,size);
 return 4;
 }
 
int dec_loadlevel(enum evcodes ec)
 {
 char buffer[view.menu.txr_width+1],*fname;
 printmsg("Default filename: %s",l->lname);
 if(readkeyb("Load file:","%[^\n]",buffer))
  {
  fname=strlen(buffer)!=0 ? buffer : l->lname;
  if(readlvl(fname,l)) printmsg("OK");
  else initdata();
  free(fname);  fflush(errf);
  }
 return 6;
 }

int dec_savelevel(enum evcodes ec)
 {
 char buffer[view.menu.txr_width+1],*fname;
 FILE *f;
 printmsg("Default filename: %s",l->lname);
 if(readkeyb("Save file:","%[^\n]",buffer))
  {
  fname=(strlen(buffer)!=0) ? buffer : l->lname;
#ifdef SHAREWARE
  if(openwritefile(fname,&l->lname,view.levelpath,"sdl",&f))
#else
  if(openwritefile(fname,&l->lname,view.levelpath,"rdl",&f))
#endif
   {
   if(savelvl(f)) printmsg("Level saved");
   fclose(f);
   }
  free(fname);
  }
 return 6; 
 }
 
int dec_clickthing(enum evcodes ec)
 { 
 if(findthing(me->x,me->y))
  { view.currmode=tt_thing; view.showwhat=in_thing;
    view.menuoffset=0; view.curmenuitem=findmenuitem(NULL,1);
    printmsg("Clicked thing %d",view.pcurrthing->no); return 6; }
 else return 1; 
 }
 
int dec_clickdoor(enum evcodes ec)
 {
 if(finddoor(me->x,me->y))
  { view.currmode=tt_door; view.showwhat=in_door;  view.menuoffset=0;
    view.curmenuitem=findmenuitem(NULL,1); 
    printmsg("Clicked door %d",view.pcurrdoor->no); return 6; }
 else return 1;
 }
   
int dec_clickcube(enum evcodes ec)
 { 
 if(findcube(me->x,me->y)) 
  {
  if(view.currmode!=tt_cube && view.currmode!=tt_wall &&
   view.currmode!=tt_pnt)
   { view.currmode=tt_cube; view.showwhat=in_cube; view.menuoffset=0;
     view.curmenuitem=findmenuitem(NULL,1); }
  printmsg("Clicked cube %d",view.pcurrcube->no); 
  return 6; 
  }
 else return 1;
 }
 
int dec_togdraw(enum evcodes ec)
 { 
 switch(ec)
  {
  case ec_togdrawthings: view.drawwhat^=DW_THINGS; break;
  case ec_togdrawlines: view.drawwhat^=DW_ALLLINES; break;
  case ec_togdrawcubes: view.drawwhat^=DW_CUBES; break;
  case ec_togdrawdoors: view.drawwhat^=DW_DOORS; break;
  case ec_togdrawdata: view.drawwhat^=DW_DATA; break;
  default: fprintf(errf,"Unknown eventcode toggle draw: %d\n",ec); my_exit();
  }
 view.drawwhat&=~DW_BITMAP; 
 printmsg("Now drawing %s%s%s%s%s",
  view.drawwhat&DW_CUBES ? "cubes " : "",
  view.drawwhat&DW_DOORS ? "doors " : "",
  view.drawwhat&DW_THINGS ? "things " : "",
  view.drawwhat&DW_ALLLINES ? "lines " : "",
  view.drawwhat&DW_DATA ? "data " : "");
 return 6; 
 }
 
int dec_menu(enum evcodes ec)
 {
 switch(ec)
  {
  case ec_menucubes: view.currmode=tt_cube; view.showwhat=in_cube; break;
  case ec_menuthings: view.currmode=tt_thing; view.showwhat=in_thing; break;
  case ec_menudoors: view.currmode=tt_door; view.showwhat=in_door; break;
  case ec_menuwalls: view.currmode=tt_wall; view.showwhat=in_wall; break;
  case ec_menucorners: view.currmode=tt_pnt; view.showwhat=in_corner; break;
  case ec_menuint: view.showwhat=in_internal; break;
  default: printmsg("Unknown event code in dec_menu.\n"); break;
  }
 view.menuoffset=0; view.curmenuitem=findmenuitem(NULL,1); 
 return 6;
 }
 
int dec_changething(enum evcodes ec)
 {
 unsigned long no;
 if(!view.pcurrthing) return 1;
 view.oldpcurrthing=view.pcurrthing;
 switch(ec)
  {
  case ec_prevthing: view.pcurrthing=view.pcurrthing->prev;
   if(view.pcurrthing->prev==NULL)
    view.pcurrthing=l->things.tail;
   break;
  case ec_nextthing: view.pcurrthing=view.pcurrthing->next;
   if(view.pcurrthing->next==NULL)
    view.pcurrthing=l->things.head;
   break;
  case ec_choosething: 
   if(!readkeyb("Thing no.:","%li",&no)) return 1;
   if((view.pcurrthing=findnode(&l->things,no))==NULL)
    view.pcurrthing=view.oldpcurrthing; 
   break;
  default: fprintf(errf,"Unknown event code in dec_changething.\n"); break;
  }
 view.curmenuitem=findmenuitem(NULL,1);
 return 4;
 }
 
int dec_changedoor(enum evcodes ec)
 {
 unsigned long no;
 if(view.pcurrdoor==NULL)
  { view.pcurrdoor=(l->doors.size>0) ? l->doors.head : NULL;
    return 4;}
 view.oldpcurrdoor=view.pcurrdoor;
 switch(ec)
  {
  case ec_prevdoor: view.pcurrdoor=view.pcurrdoor->prev;
   if(view.pcurrdoor->prev==NULL)
    view.pcurrdoor=l->doors.tail;
   break;
  case ec_nextdoor: view.pcurrdoor=view.pcurrdoor->next;
   if(view.pcurrdoor->next==NULL)
    view.pcurrdoor=l->doors.head;
   break;
  case ec_choosedoor: 
   if(!readkeyb("Door no.:","%li",&no)) return 1;
   if((view.pcurrdoor=findnode(&l->doors,no))==NULL)
    view.pcurrdoor=view.oldpcurrdoor;
   break;
  default: fprintf(errf,"Unknown event code in dec_changedoor.\n"); break;
  }
 view.curmenuitem=findmenuitem(NULL,1); return 4;
 }
 
int dec_changecube(enum evcodes ec)
 {
 unsigned long no;
 int ret=4;
 if(!view.pcurrcube) return 1;
 view.oldpcurrcube=view.pcurrcube;
 switch(ec)
  {
  case ec_nextcube: view.pcurrcube = view.pcurrcube->next; 
   if(view.pcurrcube->next==NULL)
    view.pcurrcube=l->cubes.head;
   break;
  case ec_prevcube: view.pcurrcube = view.pcurrcube->prev;
   if(view.pcurrcube->prev==NULL)
    view.pcurrcube=l->cubes.tail;
   break;
  case ec_choosecube: 
   if(!readkeyb("Cube no.:","%li",&no)) return 1;
   if((view.pcurrcube=findnode(&l->cubes,no))==NULL)
    view.pcurrcube=view.oldpcurrcube;
   break;
  case ec_gowall:
   if(view.pcurrdoor)
    { view.pcurrcube=view.pcurrdoor->d.d->c; 
      view.currwall=view.pcurrdoor->d.d->w->no; ret=6; }
   break;
  default: fprintf(errf,"Unknown event code in changecube.\n"); break;
  } 
 view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall];
 view.oldpcurrpnt=
  view.oldpcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
 view.curmenuitem=findmenuitem(NULL,1);
 return ret;
 }
 
int dec_changewall(enum evcodes ec)
 {
 if(!view.pcurrcube) return 1;
 switch(ec)
  {
  case ec_nextwall: view.currwall++; break;
  case ec_prevwall: view.currwall--; break;
  default: fprintf(errf,"Unknown event code in changewall.\n"); break;
  }
 if(view.currwall<0) view.currwall=6-(-view.currwall)%6;
 else view.currwall%=6;
 view.pcurrwall=view.pcurrcube->d.c->walls[view.currwall]; 
 view.curmenuitem=findmenuitem(NULL,1);
 return 4; 
 }

int dec_beam(enum evcodes ec)
 {
 int i,j;
 struct point p;
 switch(view.currmode)
  {
  case tt_pnt: if(!view.pcurrcube) return 1;
   beam(view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p);
  case tt_cube: if(!view.pcurrcube) return 1;
   for(j=0;j<3;j++)
    { p.x[j]=0;
    for(i=0;i<8;i++) p.x[j]+=view.pcurrcube->d.c->p[i]->d.p->x[j]/8; }
   break;
  case tt_wall: if(!view.pcurrcube) return 1;
   for(j=0;j<3;j++)
    { p.x[j]=0;
    for(i=0;i<4;i++) 
     p.x[j]+=
      view.pcurrcube->d.c->p[wallpts[view.currwall][i]]->d.p->x[j]/4;
    }
   break;
  case tt_thing: if(!view.pcurrthing) return 1;
   p=view.pcurrthing->d.t->p[0]; break;
  case tt_door: if(!view.pcurrdoor) return 1;
   p=view.pcurrdoor->d.d->p; break;
  default: return 1;
  }
 beam(&p); return 6;
 }
 
int dec_fastquit(enum evcodes ec)
 { return 0; }
 
int dec_changevisibility(enum evcodes ec)
 { view.maxvisibility *= (ec==ec_incvisibility) ? view.visscala : 
    1/view.visscala; return 6; }

int dec_changemovefactor(enum evcodes ec)
 { 
 if(ec==ec_incmovefactor || ec==ec_decmovefactor)
  view.movefactor *= (ec==ec_incmovefactor) ? view.movescala : 
   1/view.movescala; 
 else
  view.pmovefactor *= (ec==ec_incpmovefactor) ? view.movescala : 
   1/view.movescala; 
 return 4;
 }
 
int dec_changezoom(enum evcodes ec)
 { view.dist *= (ec==ec_inczoom) ? 1/view.distscala : view.distscala;
   return 6; }

int dec_move(enum evcodes ec)
 {
 int x,i;
 struct point *p;
 switch(ec)
  {
  case ec_moveforward: p=&view.e[2]; x=1; break;
  case ec_movebackward: p=&view.e[2]; x=-1; break;
  case ec_moveright: p=&view.e[0]; x=1; break;
  case ec_moveleft: p=&view.e[0]; x=-1; break;
  case ec_moveup: p=&view.e[1]; x=1; break;
  case ec_movedown: p=&view.e[1]; x=-1; break;
  default: fprintf(errf,"Unknown event code in move.\n"); return 1; break;
  }
 for(i=0;i<3;i++)
  view.e0.x[i]+=x*view.movefactor*p->x[i];
 return 2;
 }
 
int dec_turn(enum evcodes ec)
 {
 int a0,a1,a2;
 double x;
 switch(ec)
  {
  case ec_bankleft: a0=0;a1=1;a2=2;x=view.rotangel; break;
  case ec_bankright: a0=0;a1=1;a2=2;x=-view.rotangel; break;
  case ec_turnleft: a0=2;a1=0;a2=1;x=-view.rotangel; break;
  case ec_turnright: a0=2;a1=0;a2=1;x=view.rotangel; break;
  case ec_turnup: a0=1;a1=2;a2=0;x=view.rotangel; break;
  case ec_turndown: a0=1;a1=2;a2=0;x=-view.rotangel; break;
  default: fprintf(errf,"Unknown event code in turn.\n"); return 1; break;
  }
 turn(view.e,view.e,a0,a1,a2,x);
 return 2;
 }

int dec_changerotangle(enum evcodes ec)
 { view.rotangel*=(ec==ec_incrotangle) ? view.rotscala : 1/view.rotscala;
   return 4; }

#include "do_move.c"

int dec_pmove(enum evcodes ec)
 {
 /* move the current cube,wall,point,thing */
 if(!view.pcurrcube) return 1;
 return pmove[view.currmode](&view.tagged[view.currmode],ec);
 }
 
int dec_pturn(enum evcodes ec)
 {
 if(!view.pcurrcube) return 1;
 switch(view.currmode)
  {
  case tt_thing: return pr_thing(&view.tagged[tt_thing],ec);
  case tt_cube: return pr_cube(&view.tagged[tt_cube],ec);
  default: return 1;
  }
 }
 
int dec_changepnt(enum evcodes ec)
 {
 if(!view.pcurrcube) return 1;
 view.oldpcurrpnt=
  view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
 view.currpnt+=(ec==ec_nextpnt) ? 1 : -1;
 view.currpnt&=0x3;
 return 4;
 }
 
int dec_fitbitmap(enum evcodes ec)
 { if(!view.pcurrwall) return 1;
   return initbitmapedit(view.pcurrwall); }
 
int dec_tagall(enum evcodes ec)
 {
 struct list *cl;
 struct node *n,*cn;
 int i,j;
 if(view.tagged[view.currmode].size==0)
  {
  switch(view.currmode)
   {
   case tt_cube: cl=&l->cubes; break;
   case tt_wall:
    freelist(&view.tagged[tt_wall],NULL);
    for(cn=l->cubes.head;cn->next!=NULL;cn=cn->next)
     for(i=0;i<6;i++)
      if(cn->d.c->walls[i]!=NULL)
       addnode(&view.tagged[tt_wall],cn->no*6+i,cn);
    return 6;
   case tt_pnt: 
    freelist(&view.tagged[tt_pnt],NULL);
    for(cn=l->cubes.head;cn->next!=NULL;cn=cn->next)
     for(i=0;i<6;i++)
      if(cn->d.c->walls[i]!=NULL)
       for(j=0;j<4;j++)
        addnode(&view.tagged[tt_pnt],cn->no*24+i*4+j,cn);
    return 6;
   case tt_door: cl=&l->doors; break;
   case tt_thing: cl=&l->things; break;
   default: fprintf(errf,"Unknown tagall type: %d\n",view.currmode); return 1;
   }
  freelist(&view.tagged[view.currmode],NULL);
   for(n=cl->head;n->next!=NULL;n=n->next)
    addnode(&view.tagged[view.currmode],n->no,n);
  }
 else
  freelist(&view.tagged[view.currmode],NULL);
 return 6;
 }
 
int dec_tag(enum evcodes ec)
 {
 struct node *n;
 int no;
 void *d;
 if(!view.pcurrcube) return 1;
 switch(view.currmode)
  {
  case tt_cube: no=view.pcurrcube->no; d=view.pcurrcube; break;
  case tt_wall: no=view.pcurrcube->no*6+view.currwall; 
   d=view.pcurrcube; break;
  case tt_pnt: no=view.pcurrcube->no*24+view.currwall*4+view.currpnt; 
   d=view.pcurrcube; break;
  case tt_thing: if(!view.pcurrthing) return 1;
   no=view.pcurrthing->no; d=view.pcurrthing; break;
  case tt_door: if(view.pcurrdoor==NULL) return 1;
   no=view.pcurrdoor->no; d=view.pcurrdoor; break;
  default: fprintf(errf,"Unknown tag type: %d\n",view.currmode); return 1;
  }
 if((n=findnode(&view.tagged[view.currmode],no))==NULL)
  addnode(&view.tagged[view.currmode],no,d);
 else
  freenode(&view.tagged[view.currmode],n,NULL);
 return 6;
 }
 
int dec_insert(enum evcodes ec)
 {
 struct node *n;
 if(!view.pcurrcube) return 1;
 switch(view.currmode)
  {
  case tt_thing: 
   view.oldpcurrthing=view.pcurrthing;
   view.pcurrthing=insertthing(view.pcurrcube,7);
   break;
  case tt_door: 
   view.oldpcurrdoor=view.pcurrdoor;
   view.pcurrdoor=insertdoor(view.pcurrcube,view.currwall,4,0x18f);
   break;
  case tt_cube: 
   if(view.pcurrwall==NULL)
    { printmsg("No current side."); return 1; }
   if(view.pcurrcube->d.c->nc[view.currwall]!=NULL)
    { printmsg("Already a cube tacked on."); return 1; }
   if(view.pcurrmacro==NULL)
    { printmsg("No macro loaded."); return 1; }
   freelist(&view.tagged[tt_cube],NULL);
   freelist(&view.tagged[tt_door],NULL);
   freelist(&view.tagged[tt_thing],NULL);
   if(insertmacro(view.pcurrmacro,ec==ec_fastinsert))
    printmsg("Macro inserted.");
   for(n=l->cubes.head;n->no<view.pcurrmacro->cubes.size;n=n->next)
    addnode(&view.tagged[tt_cube],n->no,n);
   for(n=l->doors.head;n->no<view.pcurrmacro->doors.size;n=n->next)
    addnode(&view.tagged[tt_door],n->no,n);
   for(n=l->things.head;n->no<view.pcurrmacro->things.size;n=n->next)
    addnode(&view.tagged[tt_thing],n->no,n);
   break;
  case tt_wall: 
   if(view.pcurrcube->d.c->nc[view.currwall]==NULL)
    { printmsg("There is no connection."); return 1; }
   /* and cut the connection */
   deleteconnect(view.pcurrcube,view.currwall);
   break;
  case tt_pnt: printmsg("You can't insert points."); break;
  default: fprintf(errf,"Unknown mode %d in insert\n",view.currmode);
   my_exit();
  }
 return 6;
 }
 
int dec_delete(enum evcodes ec)
 {
 int k;
 struct node *n,*dn;
 if(!view.pcurrcube) return 1;
 view.oldpcurrcube=view.oldpcurrthing=view.oldpcurrdoor=NULL;
 switch(view.currmode)
  {
  case tt_cube: 
   for(n=view.tagged[tt_cube].tail;n->prev!=NULL;n=n->prev)
    {
    for(k=0;k<6;k++) if(n->d.n->d.c->d[k]!=NULL) break;
    if(k==6) deletecube(n->d.n);
    }
   break;
  case tt_thing:
   for(n=view.tagged[tt_thing].tail;n->prev!=NULL;n=n->prev)
    {
    if(n->d.n->no==view.pcurrthing->no) 
     { 
     view.pcurrthing=view.pcurrthing->prev->prev ? view.pcurrthing->prev :
      view.pcurrthing->next;
     if(!view.pcurrthing->next)
      { printmsg("Can't kill last thing (should be start)."); 
        view.pcurrthing=view.pcurrthing->prev; break; } 
     }
    freenode(&l->things,n->d.n,free);
    }
   break;
  case tt_door:
   for(n=view.tagged[tt_door].tail;n->prev!=NULL;n=n->prev)
    {
    if(n->d.n->d.d->d!=NULL)
     if((dn=findnode(&view.tagged[tt_door],n->d.n->d.d->d->no))!=NULL)
      freenode(&view.tagged[tt_door],dn,NULL);
    deletedoor(n->d.n);
    }
   break;
  case tt_wall:
   /* kill wall, therefore connect two cubes */
   if(view.pcurrcube->d.c->nc[view.currwall]!=NULL)
    { printmsg("Cube is already connected."); return 1; }
   if(view.tagged[tt_wall].size!=1)
    { printmsg("You must tag exactly one wall."); return 1; }
   if(connect(view.pcurrcube,view.currwall,view.tagged[tt_wall].head->d.n,
    view.tagged[tt_wall].head->no%6))
    { freelist(&view.tagged[tt_wall],NULL); view.pcurrwall=NULL; }
   break;
  case tt_pnt: break;
  default: fprintf(errf,"Unknown mode %d in delete\n",view.currmode);
   my_exit();
  }
 freelist(&view.tagged[view.currmode],NULL);
 return 6;
 }
 
int dec_deleteall(enum evcodes ec)
 {
 enum tagtypes oldmode=view.currmode;
 char x;
 printmsg("Really kill all tagged %d cubes, %d doors and %d things?",
  view.tagged[tt_cube].size,view.tagged[tt_door].size,
  view.tagged[tt_thing].size);
 if(!readkeyb("Really? (y/n)","%c",&x) || x!='y') return 1;
 view.currmode=tt_thing; dec_delete(ec);
 view.currmode=tt_door; dec_delete(ec);
 view.currmode=tt_cube; dec_delete(ec);
 view.currmode=oldmode;
 return 6;
 }
 
int dec_scroll(enum evcodes ec)
 {
 view.menuoffset+=(ec==ec_scrollup) ? 1 : -1;
 if(view.menuoffset<0) view.menuoffset=0;
 return 4;
 }
 
int dec_grid(enum evcodes ec)
 {
 switch(ec)
  {
  case ec_gridonoff: view.gridonoff^=1; break;
  case ec_incrgridsize: view.gridlength*=view.gridscala; break;
  case ec_decrgridsize: view.gridlength/=view.gridscala; break;
  default: fprintf(errf,"Unknown eventcode in grid.\n"); exit(0);
  }
 return 4;
 }

int dec_makemacro(enum evcodes ec)
 {
 struct node *n;
 char x;
 if(!view.pcurrcube) return 1;
 if(view.pcurrcube->d.c->nc[view.currwall]!=NULL)
  if(findnode(&view.tagged[tt_cube],
   view.pcurrcube->d.c->nc[view.currwall]->no)!=NULL)
   { printmsg("You can't use this wall as connection."); return 1; }
 if(view.pcurrmacro!=NULL && view.pcurrmacro->groupno==-1)
  {
  printmsg("Overwrite current macro %s?",view.pcurrmacro->shorttxt);
  if(!readkeyb("Overwrite? (y/n)","%c",&x) || x!='y') 
   { printmsg("Macro not built."); return 4; }
  cleanmacro(view.pcurrmacro); view.pcurrmacro=NULL;
  }
 if((n=findnode(&view.tagged[tt_cube],view.pcurrcube->no))!=NULL)
  freenode(&view.tagged[tt_cube],n,NULL); /* currcube must be head */
 n=addheadnode(&view.tagged[tt_cube],view.pcurrcube->no,view.pcurrcube);
 view.pcurrmacro=buildmacro(&view.tagged[tt_cube],&view.tagged[tt_thing]);
 if(view.pcurrmacro!=NULL) printmsg("Macro made.");
 return 6;
 }

int dec_savemacro(enum evcodes evcode)
 {
 int no,mgno,overwrite;
 char buffer[view.menu.txr_width+1],*fname;
 FILE *f;
 struct node *n;
 struct list *mglist;
 struct macro *m;
 if(!view.pcurrmacro) { printmsg("No current macro."); return 1; }
 if(!readkeyb("Filename:","%[^\n]",buffer))  return 4;
 if((overwrite=openwritefile(buffer,&fname,view.macropath,
#ifdef SHAREWARE
  "smc",&f))==0) return 4;
#else
  "rmc",&f))==0) return 4;
#endif
 /* now kill an eventually overwritten macro */
 if(overwrite==2)
  for(no=0;no<view.macros.size;no++)
   {
   mglist=&((struct macrogroup *)view.macros.data[no])->macros;
   for(n=mglist->head;n->next!=NULL;n=n->next)
    {
    fprintf(errf,"%s == %s??\n%s == %s %p == %p\n",fname,n->d.m->filename,
     view.pcurrmacro->shorttxt,n->d.m->shorttxt,view.pcurrmacro,n->d.m); 
    fflush(errf);
    if(strcmp(n->d.m->filename,fname)==0 || n->d.m==view.pcurrmacro)
     freenode(mglist,n,(n->d.m!=view.pcurrmacro) ? freemacro : NULL); 
    }
   }
 mgno=view.pcurrmacro->groupno;
 if(mgno!=-1 && strcmp(fname,view.pcurrmacro->filename)!=0)
  /* saved current macro under another filename */
  {
  /* make an entry for the old filename: */
  /* if you get memory problems here, the macro will vanish from the
     list, but I hope you won't get such problems for this few bytes */
  if((m=malloc(sizeof(struct macro)))==NULL)
   { printmsg("No mem for macro."); return 4; }
  m->longtxt=NULL; m->groupno=mgno;
  initlist(&m->cubes); initlist(&m->things); initlist(&m->doors);
  initlist(&m->pts); initlist(&m->sdoors); initlist(&m->producers);
  if((m->filename=malloc(strlen(view.pcurrmacro->filename)+1))==NULL)
   { printmsg("No mem for macro filename: %s\n",view.pcurrmacro->filename);
     free(m); return 4; }
  strcpy(m->filename,view.pcurrmacro->filename);
  if((m->shorttxt=malloc(strlen(view.pcurrmacro->shorttxt)+1))==NULL)
   { printmsg("No mem for macro short text: %s\n",view.pcurrmacro->shorttxt);
     free(m->filename); free(m); return 4; }
  strcpy(m->shorttxt,view.pcurrmacro->shorttxt);
  mglist=&((struct macrogroup *)view.macros.data[m->groupno])->macros;
  if(addnode(mglist,mglist->size,m)==NULL)
   { printmsg("Can't insert new macro to list."); freemacro(m); return 4; }
  }
 /* now modify macro filename+shorttxt */
 if(!readltype(NULL,view.macros.str,&view.macros,1,&mgno,NULL) || 
  mgno>=view.macros.size) 
  { printmsg("Macro not saved."); return 4; }
 /* get the description */
 if(!readkeyb("Short text:","%[^\n]",buffer))
  { printmsg("Macro saving aborted."); return 4; }
 free(view.pcurrmacro->longtxt); view.pcurrmacro->longtxt=NULL;
 if(!readdescription(&view.pcurrmacro->longtxt)) 
  { printmsg("Macro saving aborted."); return 4; }
 free(view.pcurrmacro->shorttxt); view.pcurrmacro->shorttxt=NULL;
 if((view.pcurrmacro->shorttxt=malloc(strlen(buffer)+1))==NULL)
  { printmsg("No mem for short text."); return 4; }
 strcpy(view.pcurrmacro->shorttxt,buffer);
 view.pcurrmacro->groupno=mgno;
 /* and add the current macro to the list */
 mglist=&((struct macrogroup *)view.macros.data[mgno])->macros;
 if(addnode(mglist,mglist->size,view.pcurrmacro)==NULL)
  { printmsg("Can't add macro to list."); return 4; }
 free(view.pcurrmacro->filename);
 view.pcurrmacro->filename=fname;
 if(savemacro(f,view.pcurrmacro)) printmsg("Macro saved.");
 fclose(f);
 return 4;
 }

int dec_choosemacro(enum evcodes ec)
 {
 int n1,n2;
 struct macrogroup *mg;
 struct objdata od;
 struct node *n;
 int ok=1;
 char x;
 if(view.pcurrmacro!=NULL && view.pcurrmacro->groupno==-1)
  {
  printmsg("Overwrite current not saved macro %s?",view.pcurrmacro->shorttxt);
  if(!readkeyb("Overwrite? (y/n)","%c",&x) || x!='y') 
   { printmsg("Macro not built."); return 4; }
  cleanmacro(view.pcurrmacro); view.pcurrmacro=NULL;
  }
 if(!readltype(NULL,view.macros.str,&view.macros,1,&n1,NULL)) return 4;
 mg=(struct macrogroup *)view.macros.data[n1];
 if((od.size=mg->macros.size)==0)
  { printmsg("No macros in this group."); return 4; }
 if((od.data=calloc(sizeof(struct objtype *),od.size))==NULL)
  { printmsg("No mem for building objdata."); return 4; }
 for(n=mg->macros.head,n2=0;n->next!=NULL;n=n->next,n2++)
  {
  if((od.data[n2]=malloc(sizeof(struct objtype)))==NULL)
   { ok=0; printmsg("No mem for objtype."); break; }
  od.data[n2]->no=n->no;
  od.data[n2]->str=n->d.m->shorttxt;
  }
 if(ok)
  if(readltype(NULL,mg->ot.str,&od,1,&n2,NULL)) 
   {
   for(n=mg->macros.head;n->next!=NULL && n->no!=n2;n=n->next);
   if(n->next!=NULL)
    {
    if(!readmacro(n->d.m)) return 4;
    printmsg("Choosed macro %s",n->d.m->shorttxt);    
    view.pcurrmacro=n->d.m;
    }
   else
    printmsg("Can't find macro no. %d",n2);
   }
 for(n1=0;n1<od.size;n1++) free(od.data[n1]);
 free(od.data);
 return 4;
 }
 
int dec_domenu(enum evcodes ec)
 {
 switch(ec)
  {
  case ec_mmoveup: 
   view.curmenuitem=findmenuitem(view.curmenuitem,-1); break;
  case ec_mmovedown:
   view.curmenuitem=findmenuitem(view.curmenuitem,1); break;
  default: printmsg("Unknown eventcode in domenu."); return 1;
  }
 if(view.curmenuitem->row<view.menuoffset) 
  view.menuoffset=view.curmenuitem->row;
 else if(view.curmenuitem->row>=view.menuoffset+view.menu.txr_height-4)
  view.menuoffset=view.curmenuitem->row-view.menu.txr_height+5;
 return 4;
 }

int dec_changenumlock(enum evcodes ec)
 { numlockonoff(); return 1; }
 
int dec_setexit(enum evcodes ec)
 {
 if(!view.pcurrcube) return 1;
 if(view.pcurrcube->d.c->walls[view.currwall]==NULL)
  { printmsg("There is no side."); return 1; }
 if(view.exitcube!=NULL) insertwall(view.exitcube,view.exitwall,0);
 view.exitcube=view.pcurrcube;
 view.exitwall=view.currwall;
 return 6;
 }
 
int dec_credits(enum evcodes evcode)
 {
 GrFont *fnt=view.menu.txr_font;
 int xoff=0,yoff=view.yoffset,xsize=view.menu.txr_xpos-8,
  ysize=view.maxysize-yoff;
 GrTextOption txt_opt={fnt,1,1,{view.color[MENUCOLORS+5]},
  {view.color[MENUCOLORS]},GR_TEXT_RIGHT,GR_ALIGN_CENTER,GR_ALIGN_CENTER,
  GR_BYTE_TEXT};
 int i,maxrow=(ysize-fnt->fnt_height-6)/(fnt->fnt_height+1),
  columnsize=(xsize-6)/fnt->fnt_width-1;
 char txt[columnsize+1];
 MouseEvent my_me;
 GrFramedBox(xoff+3,yoff+3,xoff+xsize-3,yoff+ysize-3,3,&boxcolors);
 txt[columnsize]=0;
 for(i=0;cred_txt[i]!=NULL && i<maxrow;i++) 
  {
  strncpy(txt,cred_txt[i],columnsize);
  GrDrawString(txt,strlen(txt),xoff+3+xsize/2,
   yoff+3+(2*i+1)*(fnt->fnt_height+1)/2,&txt_opt);
  }
 MouseGetEvent(M_KEYPRESS|M_BUTTON_UP,&my_me);
 GrFilledBox(xoff,yoff,xoff+xsize,yoff+ysize,view.color[BLACK]);
 return 6; 
 }

int dec_helppage(enum evcodes evcode)
 {
 GrFont *fnt=view.menu.txr_font;
 int row=0,column=0,nocolumns=2,columnsize=(view.xsize-1)/nocolumns,
  maxrow=view.ysize-fnt->fnt_height-1,start=-1,ok=0,
  sizeofpage=maxrow/fnt->fnt_height*nocolumns,
  maxstart=view.noevents-sizeofpage,add=1;
 struct eventcode *s;
 char txt[columnsize/fnt->fnt_width+1];
 MouseEvent my_me;
 txt[columnsize/fnt->fnt_width]=0;
 do
  {
  GrFilledBox(view.xoffset,view.yoffset,view.xoffset+view.xsize,
   view.yoffset+view.ysize,view.color[BLACK]);
  while(view.events[start].flags!=M_KEYPRESS && start>0 && start<maxstart) 
   start+=add;
  for(s=&view.events[start],row=0,column=0;s-view.events<view.noevents;s++)
   {
   if(s->flags==M_KEYPRESS)
    {
    strncpy(txt,s->txt,columnsize/fnt->fnt_width);
    GrTextXY(view.xoffset+column,view.yoffset+row,txt,
     view.color[WHITE],view.color[BLACK]);
    row+=fnt->fnt_height+1;
    if(row>=maxrow) { row=0; column+=columnsize; }
    if(column>view.xsize-columnsize) break;
    }
   } 
  printmsg("Move with cursor keys/page up,down/home,end");
  MouseGetEvent(M_KEYPRESS|M_BUTTON_UP,&my_me);
  if(my_me.flags==M_KEYPRESS)
   {
   switch(my_me.key)
    {
    case 0x247: start-=sizeofpage; add=-1; break;
    case 0x248: start--; add=-1; break;
    case 0x249: start=maxstart; add=-1; break;
    case 0x24f: start+=sizeofpage; add=1; break;
    case 0x250: start++; add=1; break;
    case 0x251: start=0; add=1; break;
    default: ok=1;
    }
   if(start<0) { start=0; add=1; }
   else if(start>maxstart) { start=maxstart; add=-1; }
   }
  else ok=1;
  }
 while(!ok);
 printmsg("Have a nice day.");
 return 6;
 }

int dec_quit(enum evcodes evcode)
 {
 char x;
 if(!readkeyb("Quit? (y/n)","%c",&x) || x!='y') return 1;
 return 0;
 }

int dec_arrangebitmaps(enum evcodes evcode)
 {
 if(!view.pcurrwall) { printmsg("No current side."); return 1; }
 if(view.tagged[tt_wall].size==0) 
  { printmsg("Tag some sides for this."); return 1; }
 arrangebitmaps(view.pcurrcube->d.c,view.pcurrwall,&view.tagged[tt_wall]);
 return 4;
 }
 
int (*do_event[ec_num_of_codes])(enum evcodes evcode)=
{ dec_credits,dec_changenumlock,dec_clickcube,dec_clickthing,
 dec_togdraw,dec_togdraw,dec_togdraw,dec_menu,
 dec_menu,dec_menu,dec_menu,dec_changething,dec_changething,
 dec_changecube,dec_changecube,dec_changewall,dec_changewall,
 dec_tag,dec_togdraw,dec_pturn,dec_pturn,dec_pturn,dec_pturn,
 dec_changevisibility,dec_changevisibility,dec_changezoom,dec_changezoom,
 dec_changemovefactor,dec_changemovefactor,dec_move,dec_move,dec_move,
 dec_move,dec_move,dec_move,dec_turn,dec_turn,dec_turn,dec_turn,dec_turn,
 dec_turn,dec_changerotangle,dec_changerotangle,dec_changedoor,
 dec_changedoor,dec_menu,dec_menu,dec_loadlevel,dec_savelevel,
 dec_clickdoor,dec_togdraw,dec_tagall,dec_beam,dec_helppage,
 dec_makemacro,dec_savemacro,dec_choosemacro,dec_fastquit,dec_pmove,
 dec_pmove,dec_pmove,dec_pmove,dec_pmove,dec_pmove,dec_changepnt,
 dec_changepnt,dec_fitbitmap,dec_insert,dec_changevalue,dec_pturn,
 dec_pturn,dec_scroll,dec_scroll,dec_changedata,dec_changemovefactor,
 dec_changemovefactor,dec_delete,dec_grid,dec_grid,dec_grid,
 dec_changevalue,dec_domenu,dec_domenu,dec_changevalue,dec_changecube,
 dec_changething,dec_changedoor,dec_changevalue,dec_changevalue,
 dec_insert,dec_setexit,dec_deleteall,dec_changecube,dec_quit,
 dec_arrangebitmaps };

