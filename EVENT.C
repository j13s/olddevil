/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    event.c - handling user input.
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
#include "do_event.h"
#include "do_bmap.h"
#include "event.h"
 
enum evcodes giveevcode();

struct sys_event *se;

int altkeys[26]={ 286,304,302,288,274,289,290,291,279,292,293,294,
 306,305,280,281,272,275,287,276,275,303,273,301,300,277 };
enum evcodes menuline(int mouse_or_key,int key,int x,int y)
 {
 struct sys_event se;
 int x1=0,y1,x2,y2,xsize,i,ret=-1;
 struct objdata *menu=NULL;
 if(mouse_or_key==0)
  for(i=0;i<view.nummenus;i++)
   {
   x1+=strlen(view.menuline[i].str)*view.fontwidth;
   if(x1>x) { menu=&view.menuline[i]; break; }
   }
 else
  for(i=0;i<view.nummenus;i++)
   {
   x1+=strlen(view.menuline[i].str)*view.fontwidth;
   if(key==altkeys[toupper(
    view.menuline[i].str[view.menuline[i].no&0xfffful])-'A'])
    { menu=&view.menuline[i]; break; }
   }
 if(menu==NULL)
  { printmsg("Can't find menu with this char."); return -1; }
 x1-=strlen(menu->str)*view.fontwidth;
 y1=view.yoffset;
 for(i=0,xsize=strlen(menu->str);i<menu->size;i++)
  if(xsize<strlen(menu->data[i]->str)) xsize=strlen(menu->data[i]->str);
 x2=x1+xsize*view.fontwidth+4;
 y2=y1+menu->size*view.fontheight+2;
 sys_drawbutton(x1,y1,x2,y2,NULL);
 for(i=0;i<menu->size;i++)
  sys_menutext(x1+2,y1+i*view.fontheight+1,menu->data[i]->str,
   menu->data[i]->no&0xfffful);
 do sys_getevent(&se,1); 
 while(se.flags!=se_f_keypress && se.buttons!=se_bt_none);
 sys_filledbox(x1,y1,x2,y2,view.color[BLACK],0);
 rebuild_screen(0xff);
 if(se.flags==se_f_keypress)
  {
  for(i=0;i<menu->size;i++)
   if(se.key==menu->data[i]->str[menu->data[i]->no&0xfffful]) 
    { ret=menu->data[i]->no>>16; break; }
  }
 else
  {
  if(se.y>y1) i=(se.y-y1-1)/view.fontheight;
  else i=-1;
  if(i>=0 && i<menu->size)
   ret=menu->data[i]->no>>16;
  }
 return ret;
 }

enum evcodes giveevcode(int *menuused)
 {
 struct eventcode *s;
 struct sys_event rep_se;
 int where,i,repeat=0,retwert;
 clock_t t1;
 /* search where (beginning at one cause of screen) */
 for(i=1,where=0;i<ecw_num_of_boxes && where==0;i++)
  if(se->x>=view.bounds[i][0] && se->x<=view.bounds[i][2] && 
   se->y>=view.bounds[i][1] && se->y<=view.bounds[i][3]) { where=i; }
 if((where==ecw_menuline && se->flags==se_f_lbutton && 
  se->buttons==se_bt_none) || 
  ((se->kbstat&se_ks_alt)==se_ks_alt && se->flags==se_f_keypress))
  { *menuused=1;
    return menuline(se->flags==se_f_keypress,se->key,se->x,se->y); }
 *menuused=0;
 if(se->flags!=se_f_keypress)
  {
  if(se->buttons!=se_bt_none) se->flags=0;
  switch(where)
   {
   case ecw_uppermenu: 
    if((se->y-view.bounds[ecw_uppermenu][1])/view.fontheight>
     view.maxrow[view.showwhat]+2) se->key=1; /* data */
    else se->key=0;
    break;
   case ecw_lowermenu: 
    /* calculate button number */
    se->key=(se->x-view.bounds[ecw_lowermenu][0])/view.bxsize+
     (se->y-view.bounds[ecw_lowermenu][1])/view.bysize*MAX_XNOBUTTON;
    break;
   case ecw_movebuttons:
    se->key=(se->x-view.bounds[ecw_movebuttons][0])/
     ((view.bounds[ecw_movebuttons][2]-view.bounds[ecw_movebuttons][0])/3)
     +(se->y-view.bounds[ecw_movebuttons][1])/
     ((view.bounds[ecw_movebuttons][3]-view.bounds[ecw_movebuttons][1])/3)*3;
    repeat=1;
    break;
   default: se->key=0; 
   }
  }
 for(s=view.events,retwert=-1;s-view.events<view.noevents;s++)
  {
  if(s->flags==se->flags && (s->kbstat&se->kbstat)==s->kbstat && 
   s->key==se->key && (s->where==where || s->where==0))
   { retwert=s->no; break; }
  }
 if(!repeat) return retwert;
 if(retwert<0 || retwert>=ec_num_of_codes) return -1;
 do_event(retwert);
 t1=clock(); while(clock()-t1<CLOCKS_PER_SEC/4); t1=clock();
 while(!sys_getevent(&rep_se,0))
  if((clock()-t1)/(CLOCKS_PER_SEC/20)>=1)
   { t1=clock(); do_event(retwert); }
 return -1;
 }
 
int readkeyb(char *txt,char *rtype,void *data,...)
 {
 va_list va_arg;
 struct sys_event se;
 char buffer[view.xsize/view.fontwidth+1];
 int i;
 va_start(va_arg,data);
 vprintmsg(txt,va_arg);
 va_end(va_arg);
 i=0; se.key=0;
 while(se.key!=0xd)
  {
  buffer[i]='_'; buffer[i+1]=0;
  printmsg("*%s",buffer);
  sys_getevent(&se,1);
  if((se.flags==se_f_lbutton || se.flags==se_f_rbutton) &&
   se.buttons!=se_bt_none) 
   { printmsg(""); return 0; }
  if(isprint(se.key)) 
   { if(i<view.xsize/view.fontwidth-1)
      buffer[i++]=se.key; }
  else
   {
   switch(se.key)
    {
    case 0x8: if(i==0) break;
     buffer[i]=0; buffer[--i]=' '; break;
    case 0xd: buffer[i]=0; printmsg("*%s",buffer); break;
    case 0x1b: printmsg(""); return 0; 
    } 
   }
  }
 buffer[i]='\n'; buffer[i+1]=0;
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
 struct sys_event se;
 for(ot=od->data,start=0;ot-od->data<od->size;ot++)
  if((*ot)->no==tagged[0]) 
   { start=ot-od->data; break; }
 if(od->size<view.smenuheight)
  { curpos=start+1; start=0; }
 else
  { curpos=start+1; 
    if((start-=view.smenuheight/2)<0) start=0;
    curpos-=start; }
 do
  { 
  if(dosomething!=NULL) dosomething(od->data[curpos-1+start]->no,data);
  /* make place in the text window */
  memset(view.menubuffer,' ',view.smenuwidth*view.smenuheight);
  strcpy(view.menubuffer,txt);
  for(ot=&od->data[start],pos=&view.menubuffer[view.smenuwidth];
   ot-od->data<od->size && pos-view.menubuffer<view.smenuheight*
   view.smenuwidth-1;ot++,pos+=view.smenuwidth)
   {
   strcpy(pos+1,(*ot)->str); 
   for(i=0;i<maxtag;i++)
    if(tagged[i]>=0 && (*ot)->no==tagged[i]) *pos='#';
   }
  sys_dumpmenu();
  sys_dumpmenuchar('*',0,curpos);
  sys_getevent(&se,1);
  if(se.flags==se_f_rbutton && se.buttons==se_bt_none)
   curpos = ((se.y-view.bounds[ecw_uppermenu][1])*2/(view.smenuwidth*
    view.fontheight)==0) ? curpos-1 : curpos+1;
  else if(se.flags==se_f_lbutton && se.buttons==se_bt_none)
   {
   curpos=(se.y-view.bounds[ecw_uppermenu][1])/view.fontheight;
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
  else if(se.flags==se_f_keypress && (se.key==0x20 || se.key==0x0d))
   { 
   if(curpos==0) readkeyb("Number:","%li",&inptag);
   else inptag=od->data[curpos-1+start]->no;
   if(maxtag==1) { tagged[0]=inptag; return 1; }
   else if(se.key==0x20)
    {
    for(i=0;i<maxtag;i++) if(inptag==tagged[i]) { tagged[i]=-1; break; }
    if(i==maxtag) 
     for(i=0;i<maxtag;i++) if(tagged[i]<0) { tagged[i]=inptag; break; }
    }
   }
  switch(se.key)
   {
   case 0x248: curpos--; break;
   case 0x250: curpos++; break;
   case 0x247: start=0; curpos=1; break;
   case 0x24f: start=od->size-view.smenuheight+1; 
    curpos=view.smenuheight-1; break; 
   case 0x249: curpos-=view.smenuheight-2; break;
   case 0x251: curpos+=view.smenuheight-2; break;
   case 0x0d: return 1; 
   }
  if(curpos<=0)
   {
   if(start+curpos-1>0) { start+=curpos-1; curpos=1; }
   else { start=0; curpos=0; }
   }
  else if(curpos>od->size) curpos=od->size;
  if(curpos>=view.smenuheight)
   {
   if(start+curpos-1<od->size) start+=curpos-view.smenuheight+1;
   else start=od->size-view.smenuheight+1;
   curpos=view.smenuheight-1;
   }
  }
 while(se.key!=0x1b);
 return 0;
 }
 
int event(struct sys_event *orig_se)
 {
 int evcode,menuused,retwert=1;
 se=orig_se;
 evcode=giveevcode(&menuused);
 if(evcode>=0 && evcode<ec_num_of_codes)
  { printmsg(""); retwert=do_event(evcode); }
 else
  if(evcode!=-1)
   { printmsg("Unknown event code %d (max. %d).\n",evcode,
      ec_num_of_codes); return 1; }
 return retwert;
 }

