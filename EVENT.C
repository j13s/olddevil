/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    event.c - handling user input.
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
#include "do_event.h"
#include "do_bmap.h"
#include "event.h"
 
enum evcodes giveevcode(int f,int ks,int key,int x,int y,int buttons,
 long timestamp);

MouseEvent *me;

int altkeys[26]={ 286,304,302,288,274,289,290,291,279,292,293,294,
 306,305,280,281,272,275,287,276,275,303,273,301,300,277 };
enum evcodes menuline(int mouse_or_key,int key,int x,int y)
 {
 MouseEvent me;
 GrContext savepic;
 int x1=0,y1,x2,y2,xsize,i,ret=-1;
 struct objdata *menu=NULL;
 if(mouse_or_key==0)
  for(i=0;i<view.nummenus;i++)
   {
   x1+=strlen(view.menuline[i].str)*view.menu.txr_font->fnt_width;
   if(x1>x) { menu=&view.menuline[i]; break; }
   }
 else
  for(i=0;i<view.nummenus;i++)
   {
   x1+=strlen(view.menuline[i].str)*view.menu.txr_font->fnt_width;
   if(key==altkeys[toupper(
    view.menuline[i].str[view.menuline[i].no&0xfffful])-'A'])
    { menu=&view.menuline[i]; break; }
   }
 if(menu==NULL)
  { printmsg("Can't find menu."); return -1; }
 x1-=strlen(menu->str)*view.menu.txr_font->fnt_width;
 y1=view.yoffset;
 for(i=0,xsize=strlen(menu->str);i<menu->size;i++)
  if(xsize<strlen(menu->data[i]->str)) xsize=strlen(menu->data[i]->str);
 x2=x1+xsize*view.menu.txr_font->fnt_width+4;
 y2=y1+menu->size*view.menu.txr_font->fnt_height+2;
 GrCreateContext(x2-x1,y2-y1,NULL,&savepic);
 GrBitBlt(&savepic,0,0,NULL,x1,y1,x2,y2,GrWRITE);
 GrFramedBox(x1+1,y1+1,x2-2,y2-2,1,&boxcolors);
 for(i=0;i<menu->size;i++)
  {
  GrTextXY(x1+2,y1+i*view.menu.txr_font->fnt_height+1,
   menu->data[i]->str,view.color[MENUCOLORS+5],view.color[MENUCOLORS]);
  GrHLine(x1+2+view.menu.txr_font->fnt_width*(menu->data[i]->no&0xfffful),
   x1+1+view.menu.txr_font->fnt_width*((menu->data[i]->no+1)&0xfffful),
   y1+i*view.menu.txr_font->fnt_height+view.menu.txr_font->fnt_baseline+3,
   view.color[MENUCOLORS+5]);
  }
 MouseGetEvent(M_BUTTON_UP|M_KEYPRESS,&me);
 if(me.flags==M_KEYPRESS)
  {
  for(i=0;i<menu->size;i++)
   if(me.key==menu->data[i]->str[menu->data[i]->no&0xfffful]) 
    { ret=menu->data[i]->no>>16; break; }
  }
 else
  {
  if(me.y>y1) i=(me.y-y1-1)/view.menu.txr_font->fnt_height;
  else i=-1;
  if(i>=0 && i<menu->size)
   ret=menu->data[i]->no>>16;
  }
 GrBitBlt(NULL,x1,y1,&savepic,0,0,x2-x1,y2-y1,GrWRITE);
 GrDestroyContext(&savepic);
 return ret;
 }

long lasttime=0;

enum evcodes giveevcode(int f,int ks,int key,int x,int y,int buttons,
 long timestamp)
 {
 struct eventcode *s;
 int where,i;
 if((ks&0x3)!=0) ks|=0x3; /* all shifts */
 /* search where (beginning at one cause of screen) */
 for(i=1,where=0;i<ecw_num_of_boxes && where==0;i++)
  if(x>=view.bounds[i][0] && x<=view.bounds[i][2] && y>=view.bounds[i][1] &&
   y<=view.bounds[i][3]) { where=i; }
 if((where==ecw_menuline && f==M_LEFT_UP) ||
  ((ks&KB_ALT)==KB_ALT && f==M_KEYPRESS))
  return menuline(f==M_KEYPRESS,key,x,y);
 if(f!=M_KEYPRESS)
  switch(where)
   {
   case ecw_uppermenu: 
    key=((y-view.menu.txr_ypos)/view.menu.txr_font->fnt_height)+
     view.menuoffset;
    if(view.showwhat>=0 && view.showwhat<=in_number)
     if(key>view.maxrow[view.showwhat]+2) key=1;
     else key=0;
    break;
   case ecw_lowermenu: 
    /* calculate button number */
    key=(x-view.buttonmenu->gc_xoffset)/view.bxsize+
     (y-view.buttonmenu->gc_yoffset)/view.bysize*MAX_XNOBUTTON;
    break;
   case ecw_movebuttons:
    key=(x-view.movebuttons->gc_xoffset)/(view.movebuttons->gc_xmax/3)
     +(y-view.movebuttons->gc_yoffset)/(view.movebuttons->gc_ymax/3)*3;
    if(f==M_LEFT_DOWN || f==M_RIGHT_DOWN) lasttime=timestamp; 
    if(timestamp-lasttime>=view.waittime || f!=0)
     {
     if(buttons==M_LEFT) f=M_LEFT_UP;
     else if (buttons==M_RIGHT) f=M_RIGHT_UP;
     else f=0;
     }
    break;
   default: key=0; 
   }
 for(s=view.events;s-view.events<view.noevents;s++)
  {
  if(s->flags==f && (s->kbstat&ks)==s->kbstat && s->key==key &&
   (s->where==where || s->where==0))
    return s->no;
  }
 return -1;
 }
 
int readkeyb(char *txt,char *rtype,void *data)
 {
 MouseEvent me;
 char buffer[view.menu.txr_width+3];
 int i;
 memset(&view.menu.txr_buffer[(view.menu.txr_height-1)*view.menu.txr_width],
  ' ',view.menu.txr_width);
 GrDumpText(0,view.menu.txr_height-1,view.menu.txr_width,1,&view.menu);
 printsmsg(txt);
 for(i=0,me.key=0;i<view.menu.txr_width && me.key!=0xd;i++)
  {
  MouseGetEvent(M_BUTTON_UP|M_KEYPRESS,&me);
  if(me.flags==M_BUTTON_UP) return 0;
  if(isprint(me.key)) 
   {    
   buffer[i]=me.key;
   GrDumpChar(buffer[i],i,view.menu.txr_height-1,&view.menu); 
   }
  else
   {
   switch(me.key)
    {
    case 0x8: if(i==0) break;
     buffer[--i]=' ';
     GrDumpChar(' ',i--,view.menu.txr_height-1,&view.menu); 
     break;
    case 0xd: break;
    case 0x1b:
     memset(
      &view.menu.txr_buffer[(view.menu.txr_height-2)*view.menu.txr_width],' ',
      2*view.menu.txr_width);
     GrDumpText(0,view.menu.txr_height-2,view.menu.txr_width,2,&view.menu);
     return 0; break;
    } 
   }
  }
 memset(
  &view.menu.txr_buffer[(view.menu.txr_height-2)*view.menu.txr_width],' ',
  2*view.menu.txr_width);
 GrDumpText(0,view.menu.txr_height-2,view.menu.txr_width,2,&view.menu);
 buffer[i-1]='\n'; buffer[i]=0;
 if(sscanf(buffer,rtype,data)!=1)
  { if(rtype[1]=='[' || rtype[1]=='s') *(unsigned char *)data=0; return 1;
    printmsg("Error reading type %s: [%s]",rtype,buffer); return 0; }  
 return 1;
 }

int readltype(void (*dosomething)(int no,int *data),char *txt,
 struct objdata *od,int maxtag,int *tagged,int *data)
 {
 struct objtype **ot;
 char *pos;
 int start,curpos,i;
 int inptag;
 MouseEvent me;
 for(ot=od->data,start=0;ot-od->data<od->size;ot++)
  if((*ot)->no==tagged[0]) 
   { start=ot-od->data; break; }
 if(od->size<view.menu.txr_height)
  { curpos=start+1; start=0; }
 else
  { curpos=start+1; 
    if((start-=view.menu.txr_height/2)<0) start=0;
    curpos-=start; }
 do
  { 
  if(dosomething!=NULL) dosomething(od->data[curpos-1+start]->no,data);
  /* make place in the text window */
  memset(view.menu.txr_buffer,' ',view.menu.txr_width*view.menu.txr_height);
  strcpy(view.menu.txr_buffer,txt);
  for(ot=&od->data[start],pos=&view.menu.txr_buffer[view.menu.txr_width];
   ot-od->data<od->size && pos-view.menu.txr_buffer<view.menu.txr_height*
   view.menu.txr_width-1;ot++,pos+=view.menu.txr_width)
   {
   strcpy(pos+1,(*ot)->str); 
   for(i=0;i<maxtag;i++)
    if(tagged[i]>=0 && (*ot)->no==tagged[i]) *pos='#';
   }
  GrDumpTextRegion(&view.menu);
  GrDumpChar('*',0,curpos,&view.menu);
  MouseGetEvent(M_BUTTON_UP|M_KEYPRESS,&me);
  if(me.flags==M_RIGHT_UP)
   curpos = ((me.y-view.menu.txr_ypos)*2/(view.menu.txr_width*
    view.menu.txr_font->fnt_height)==0) ? curpos-1 : curpos+1;
  else if(me.flags==M_LEFT_UP)
   {
   curpos=(me.y-view.menu.txr_ypos)/view.menu.txr_font->fnt_height;
   if(curpos>=0 && curpos<=od->size)
    {
    if(curpos==0) readkeyb("Number:","%lu",&inptag);
    else inptag=od->data[curpos-1]->no;
    if(maxtag==1) { tagged[0]=inptag; return 1; }
    else
     {
     for(i=0;i<maxtag;i++) if(inptag==tagged[i]) { tagged[i]=-1; break; }
     if(i==maxtag) 
      for(i=0;i<maxtag;i++) if(tagged[i]<0) { tagged[i]=inptag; break; }
     }
    }
   }
  else if(me.flags==M_KEYPRESS && (me.key==0x20 || me.key==0x0d))
   { 
   if(curpos==0) readkeyb("Number:","%li",&inptag);
   else inptag=od->data[curpos-1+start]->no;
   if(maxtag==1) { tagged[0]=inptag; return 1; }
   else if(me.key==0x20)
    {
    for(i=0;i<maxtag;i++) if(inptag==tagged[i]) { tagged[i]=-1; break; }
    if(i==maxtag) 
     for(i=0;i<maxtag;i++) if(tagged[i]<0) { tagged[i]=inptag; break; }
    }
   }
  switch(me.key)
   {
   case 0x248: curpos--; break;
   case 0x250: curpos++; break;
   case 0x247: start=0; curpos=1; break;
   case 0x24f: start=od->size-view.menu.txr_height+1; 
    curpos=view.menu.txr_height-1; break; 
   case 0x249: curpos-=view.menu.txr_height-2; break;
   case 0x251: curpos+=view.menu.txr_height-2; break;
   case 0x0d: return 1; 
   }
  if(curpos<0)
   {
   if(start+curpos>=0) start+=curpos;
   else start=0;
   curpos=0;
   }
  else if(curpos>od->size)
   curpos=od->size;
  if(curpos>=view.menu.txr_height)
   {
   if(start+curpos-1<od->size) start+=curpos-view.menu.txr_height+1;
   else start=od->size-view.menu.txr_height+1;
   curpos=view.menu.txr_height-1;
   }
  }
 while(me.key!=0x1b);
 return 0;
 }
 
int event(MouseEvent *orig_me)
 {
 int evcode;
 me=orig_me;
 if(me->flags==0 && me->key!=0) me->flags=M_KEYPRESS; /*speed up the keys*/
 evcode=giveevcode(me->flags,me->kbstat,me->key,me->x,me->y,me->buttons,
  me->time);
 me->key=0;
 if(evcode>=0 && evcode<ec_num_of_codes)
  {  printmsg(""); /* just kill the status line */
  if((view.drawwhat&DW_BITMAP)==0) return do_event[evcode](evcode);
  else return do_bitmap(evcode);
  }
 else
  if(evcode!=-1)
   { fprintf(errf,"Unknown event code %d (max. %d).\n",evcode,
    ec_num_of_codes); return 0; }
 return 1;
 }

