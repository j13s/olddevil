/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    wi_menu.c - Handling of the menu
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
    
#include "wins_int.h"
#include "w_init.h"
#include "w_tools.h"
#include "w_draw.h"
#include "wi_buts.h"
#include "wi_menu.h"

/* Draw menutxt (with underline). hl=1->hilighted */
void wi_drawmenutxt(struct wi_menu *m,int hl)
 {
 if(hl)
  ws_drawframedbox(m->x1,m->y1,m->xs,shapes.titlebar_height,1,
   notes.colindex[cv_buttonrb],notes.colindex[cv_buttonlt],
   notes.colindex[cv_buttonin]);
 else
  ws_drawfilledbox(m->x1,m->y1,m->xs,shapes.titlebar_height,
   notes.colindex[cv_buttonin],0);
 ws_drawtext(m->x1+1+shapes.titlebar_height/4,m->y1+1,m->xs,m->txt,
  notes.colindex[hl?cv_textfg:cv_textbg],notes.colindex[cv_buttonin]);
 ws_drawtext(m->x1+shapes.titlebar_height/4,m->y1,m->xs,m->txt,
  notes.colindex[hl?cv_textbg:cv_textfg],-1);
 ws_drawline(m->x1+m->hklinex+shapes.titlebar_height/4,m->y1+m->hkliney,
  m->x1+m->hklinex+m->hklinexs-1+shapes.titlebar_height/4,
  m->y1+m->hkliney,notes.colindex[hl ? cv_textbg : cv_textfg],0);
 }
 
void wi_undrawmenu(struct wi_menu *m,struct list *l,int level)
 {
 struct node *n;
 for(n=l->tail->prev;n!=NULL;n=n->prev)
  {
  if(n->next->d.w_m->level>=level && n->next->d.w_m->bm!=NULL)
   { ws_restorebitmap(n->next->d.w_m->bm); n->next->d.w_m->bm=NULL; }
  if(n->next->d.w_m->level>level) killnode(l,n->next);
  }
 if(m && m->level<=level) wi_drawmenutxt(m,0);
 }
 
struct wi_menu *wi_drawsubmenu(struct list *l,struct wi_menu *m)
 {
 struct wi_menu *m2;
 struct node *n;
 int size;
 wi_drawmenutxt(m,1);
 for(n=m->submenus.head,size=0;n->next!=NULL;n=n->next) size+=n->d.w_m->ys;
 if(size!=0)
  {
  m2=m->submenus.head->d.w_m; 
  checkmem((m->bm=ws_savebitmap(NULL,m2->x1-shapes.width_menuframe,
   m2->y1-shapes.width_menuframe,m2->xs+2*shapes.width_menuframe,
   size+2*shapes.width_menuframe)));
  ws_drawframedbox(m2->x1-shapes.width_menuframe,
   m2->y1-shapes.width_menuframe,m2->xs+2*shapes.width_menuframe,
   size+2*shapes.width_menuframe,shapes.width_menuframe,
   notes.colindex[cv_buttonlt],notes.colindex[cv_buttonrb],
   notes.colindex[cv_buttonin]);
  for(n=m->submenus.head;n->next!=NULL;n=n->next)
   { wi_drawmenutxt(n->d.w_m,0);
     if(l) checkmem(addnode(l,n->no,n->d.w_m)); }
  }
 else m2=NULL;
 return m2;
 }
   
struct wi_menu *wi_mousedrawmenu(struct ws_event *ws,struct list *l)
 { 
 struct node *n;
 struct wi_menu *m=NULL,*om=NULL;
 do
  {
  for(n=l->head,m=NULL;n->next!=NULL;n=n->next)
   if(n->d.w_m->y1<=ws->y && n->d.w_m->y1+n->d.w_m->ys>ws->y &&
    n->d.w_m->x1<=ws->x && n->d.w_m->x1+n->d.w_m->xs>ws->x) 
     { m=n->d.w_m; break; }
  if(om!=m && m!=NULL)
   {
   if(om!=NULL) wi_undrawmenu(om,l,m->level);
   om=m; wi_drawsubmenu(l,m);
   } /* end if(om!=m) */
  }
 while(!ws_getevent(ws,0));
 if(om!=NULL) wi_undrawmenu(om,l,0);
 return m; 
 }
 
struct wi_menu *wi_keydrawmenu(struct wi_menu *m)
 { 
 struct node *n,*mn=NULL;
 struct wi_menu *sm=m;
 int end=0;
 struct ws_event ws;
 my_assert(m!=NULL);
 if(m->submenus.size==0) return m;
 mn=m->submenus.head; 
 my_assert(wi_drawsubmenu(NULL,m)!=NULL);
 m=mn->d.w_m;
 do
  {
  wi_drawmenutxt(m,1);
  ws_getevent(&ws,1);
  if(ws.flags!=ws_f_keypress) end=1;
  else switch(ws.key)
   {
   case 0x1b: m=NULL; end=1; break;
   case 0xd: m=wi_keydrawmenu(m); end=1; break;
   case 0x248: 
    mn=mn->prev->prev ? mn->prev : *(&mn->prev->prev+1); 
    wi_drawmenutxt(m,0); m=mn->d.w_m; wi_drawmenutxt(m,1); 
    break;
   case 0x250: 
    mn=mn->next->next ? mn->next : *(&mn->next->next-1); 
    wi_drawmenutxt(m,0); m=mn->d.w_m; wi_drawmenutxt(m,1); 
    break;
   default: 
    for(n=sm->submenus.head;n->next!=NULL;n=n->next)
     if(n->d.w_m->hotkey==ws.key) 
      { wi_drawmenutxt(m,0); m=n->d.w_m; mn=n; wi_drawmenutxt(m,1); 
        end=1; m=wi_keydrawmenu(m); break; }
   }
  }
 while(!end);
 ws_restorebitmap(sm->bm); sm->bm=NULL;  wi_drawmenutxt(sm,0);
 return m; 
 }
 
int wi_handlemenu(struct ws_event *ws)
 {
 struct wi_menu *m=NULL;
 struct list l;
 struct node *n;
 if(ws->flags!=ws_f_keypress && ws->buttons==ws_bt_none) return 0;
 else ws->key=toupper(ws->key);
 initlist(&l);
 for(n=notes.menu.head;n->next!=NULL;n=n->next)
  {
  checkmem(addnode(&l,0,n->d.w_m)); 
  if(n->d.w_m->hotkey==ws->key) m=n->d.w_m;
  }
 if(m!=NULL && ws->flags==ws_f_keypress)
  { if((m=wi_keydrawmenu(m))!=NULL && m->action!=NULL)
     m->action(m->actionnr); }
 else
  if((m=wi_mousedrawmenu(ws,&l))!=NULL && m->action!=NULL)
   m->action(m->actionnr);
 for(n=l.head->next;n!=NULL;n=n->next)
  {
  if(n->prev->d.w_m->bm)
   { ws_freebitmap(n->prev->d.w_m->bm); n->prev->d.w_m->bm=NULL; }
  killnode(&l,n->prev);
  }
 return 1;
 }
