/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    tag.c - functions for tagging or untagging something.
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
#include "plot.h"
#include "calctxt.h"
#include "options.h"
#include "tools.h"

int tagedge(struct node *nc,va_list args)
 {
 int wno,pno;
 wno=va_arg(args,int); pno=va_arg(args,int);
 if(nc==NULL || nc->d.c->walls[wno]==NULL) return 1;
 if(nc->d.c->walls[wno]->tagged[pno]) return 0;
 checkmem(nc->d.c->walls[wno]->tagged[pno]=addnode(&l->tagged[tt_edge],
  nc->no*24+wno*4+pno,nc));
 return 1;
 }

void untagedge(struct node *nc,va_list args)
 {
 int wno,pno;
 wno=va_arg(args,int); pno=va_arg(args,int);
 if(nc==NULL || nc->d.c->walls[wno]==NULL) return;
 if(nc->d.c->walls[wno]->tagged[pno])
  freenode(&l->tagged[tt_edge],nc->d.c->walls[wno]->tagged[pno],NULL);
 nc->d.c->walls[wno]->tagged[pno]=NULL;
 }
 
int tagcube(struct node *nc,va_list args)
 {
 if(nc->d.c->tagged==NULL)
  {
  checkmem(nc->d.c->tagged=addnode(&l->tagged[tt_cube],nc->no,nc));
  return 1;
  }
 else return 0;
 }

void untagcube(struct node *tn,va_list args)
 {
 if(tn->d.c->tagged) freenode(&l->tagged[tt_cube],tn->d.c->tagged,NULL);
 tn->d.c->tagged=NULL;
 }
 
int tagwall(struct node *nc,va_list args)
 {
 int no=va_arg(args,int);
 if(nc->d.c->tagged_walls[no]==NULL)
  {
  checkmem(nc->d.c->tagged_walls[no]=addnode(&l->tagged[tt_wall],
   nc->no*6+no,nc));
  return 1;
  }
 else return 0;
 }

void untagwall(struct node *tn,va_list args)
 {
 int wno=va_arg(args,int);
 if(tn->d.c->tagged_walls[wno])
  freenode(&l->tagged[tt_wall],tn->d.c->tagged_walls[wno],NULL);
 tn->d.c->tagged_walls[wno]=NULL;
 }
 
int tagpnt(struct node *np,va_list args)
 {
 struct node *n;
 if(np->d.lp->tagged==NULL)
  {
  checkmem(n=addnode(&l->tagged[tt_pnt],np->no,np));
  np->d.lp->tagged=n;
  return 1;
  }
 else return 0;
 }

void untagpnt(struct node *tn,va_list args)
 {
 if(tn->d.lp->tagged)
  freenode(&l->tagged[tt_pnt],tn->d.lp->tagged,NULL);
 tn->d.lp->tagged=NULL;
 }
 
int tagthing(struct node *nt,va_list args)
 {
 struct node *n;
 if(nt->d.t->tagged==NULL)
  {
  checkmem(n=addnode(&l->tagged[tt_thing],nt->no,nt));
  nt->d.t->tagged=n;
  return 1;
  }
 else return 0;
 }

void untagthing(struct node *tn,va_list args)
 {
 if(tn->d.t->tagged) freenode(&l->tagged[tt_thing],tn->d.t->tagged,NULL);
 tn->d.t->tagged=NULL;
 }
 
int tagdoor(struct node *nd,va_list args)
 {
 struct node *n;
 if(nd->d.d->tagged==NULL)
  {
  checkmem(n=addnode(&l->tagged[tt_door],nd->no,nd));
  nd->d.d->tagged=n;
  return 1;
  }
 else return 0;
 }

void untagdoor(struct node *tn,va_list args)
 {
 if(tn->d.d->tagged) freenode(&l->tagged[tt_door],tn->d.d->tagged,NULL);
 tn->d.d->tagged=NULL;
 }
 
int (*tag_something[tt_number])(struct node *n,va_list arg)=
 { tagcube,tagwall,tagedge,tagpnt,tagthing,tagdoor };
 
int tag(enum tagtypes tt,struct node *data,...)
 {
 va_list args;
 my_assert(tt<tt_number && data!=NULL);
 va_start(args,data);
 if(data!=NULL) return tag_something[tt](data,args);
 va_end(args);
 return 0;
 }

void (*untag_something[tt_number])(struct node *tn,va_list args)={ untagcube,
 untagwall,untagedge,untagpnt,untagthing,untagdoor };

void untag(enum tagtypes tt,struct node *tn,...)
 { 
 va_list args;
 my_assert(tt<tt_number && tn!=NULL);
 va_start(args,tn);
 if(tn!=NULL) untag_something[tt](tn,args); 
 va_end(args);
 }
 
int tagall(enum tagtypes tt)
 {
 struct node *cn;
 int i,j;
 switch(tt)
  {
  case tt_cube:
   for(cn=l->cubes.head;cn->next!=NULL;cn=cn->next)
    if(!tag(tt_cube,cn)) return 0;
   break;
  case tt_wall:
   for(cn=l->cubes.head;cn->next!=NULL;cn=cn->next)
    for(i=0;i<6;i++) if(!tag(tt_wall,cn,i)) return 0;
   break;
  case tt_edge:
   for(cn=l->cubes.head;cn->next!=NULL;cn=cn->next)
    for(i=0;i<6;i++) for(j=0;j<4;j++) if(!tag(tt_edge,cn,i,j)) return 0;
   break;
  case tt_pnt:
   for(cn=l->pts.head;cn->next!=NULL;cn=cn->next)
    if(!tag(tt_pnt,cn)) return 0;
   break;
  case tt_door:
   for(cn=l->doors.head;cn->next!=NULL;cn=cn->next)
    if(!tag(tt_door,cn)) return 0;
   break;
  case tt_thing:
   for(cn=l->things.head;cn->next!=NULL;cn=cn->next)
    if(!tag(tt_thing,cn)) return 0;
   break;
  default: waitmsg("Unknown tagall type: %d\n",tt); return 0;
  }
 return 1;
 }

void untagall(enum tagtypes tt)
 {
 struct node *n;
 for(n=l->tagged[tt].head;n->next!=NULL;n=n->next) 
  switch(tt)
   {
   case tt_cube: case tt_pnt: case tt_door: case tt_thing:
    untag(tt,n->d.n); break;
   case tt_wall: untag(tt,n->d.n,n->no%6); break;
   case tt_edge: untag(tt,n->d.n,(n->no%24)/4,(n->no%24)%4); break;
   default: waitmsg("Unknown tagtype in untagall."); 
   }
 }
 
int testtag(enum tagtypes tt,struct node *n,...)
 {
 int ret;
 va_list args;
 my_assert(tt<tt_number && n!=NULL);
 va_start(args,n);
 switch(tt)
  {
  case tt_cube: ret=n->d.c->tagged!=NULL; break;
  case tt_wall: ret=n->d.c->tagged_walls[va_arg(args,int)]!=NULL; break;
  case tt_edge: ret=va_arg(args,int);
   ret=n->d.c->walls[ret]!=NULL && 
    n->d.c->walls[ret]->tagged[va_arg(args,int)]!=NULL; break;
  case tt_pnt: ret=n->d.lp->tagged!=NULL; break;
  case tt_thing: ret=n->d.t->tagged!=NULL; break;
  case tt_door: ret=n->d.d->tagged!=NULL; break;
  default: waitmsg("Unknown tagmode %d in testtag.",tt); ret=0;
  }
 va_end(args);
 return ret;
 }
 
int tag_testfunc(int lr,struct point *p,int x1,int y1,
 int x2,int y2)
 {
 struct pixel pix;
 if(!getpixelcoords(lr,p,&pix) || pix.d>view.maxvisibility*1.1) return 0;
 return (pix.x>=x1 && pix.x<=x2 && pix.y>=y1 && pix.y<=y2);
 }
 
void tagbox(int lr,int dx1,int dy1,int dx2,int dy2,int op)
 {
 struct node *n;
 int i,j,k;
 struct point c,p;
 my_assert(l!=NULL);
 switch(view.currmode)
  {
  case tt_cube:
   for(n=l->cubes.head;n->next!=NULL;n=n->next)
    {
    for(i=0;i<8;i++)
     if(!tag_testfunc(lr,n->d.c->p[i]->d.p,dx1,dy1,dx2,dy2)) break;
    if(i==8) if(op) tag(view.currmode,n); else untag(view.currmode,n);
    }
   break;
  case tt_wall: 
   for(n=l->cubes.head;n->next!=NULL;n=n->next)
    for(j=0;j<6;j++) 
     {
     for(i=0;i<4;i++) 
      if(!tag_testfunc(lr,n->d.c->p[wallpts[j][i]]->d.p,dx1,dy1,dx2,dy2))
       break;
     if(i==4) if(op) tag(view.currmode,n,j); else untag(view.currmode,n,j);
     }
   break;
  case tt_edge:
   for(n=l->cubes.head;n->next!=NULL;n=n->next)
    for(j=0;j<6;j++)
     {
     for(k=0;k<3;k++) 
      { c.x[k]=0.0; 
        for(i=0;i<4;i++) c.x[k]+=n->d.c->p[wallpts[j][i]]->d.p->x[k]/4.0; }
     for(i=0;i<4;i++) 
      {
      for(k=0;k<3;k++) 
       p.x[k]=c.x[k]*0.4+n->d.c->p[wallpts[j][i]]->d.p->x[k]*0.6;
      if(tag_testfunc(lr,&p,dx1,dy1,dx2,dy2) &&
       tag_testfunc(lr,n->d.c->p[wallpts[j][i]]->d.p,dx1,dy1,dx2,dy2))
       if(op) tag(view.currmode,n,j,i); else untag(view.currmode,n,j,i);
      }
     }
   break;
  case tt_pnt:
   for(n=l->pts.head;n->next!=NULL;n=n->next)
    if(tag_testfunc(lr,n->d.p,dx1,dy1,dx2,dy2)) 
     if(op) tag(view.currmode,n); else untag(view.currmode,n);
   break;
  case tt_door:
   for(n=l->doors.head;n->next!=NULL;n=n->next)
    if(tag_testfunc(lr,&n->d.d->p,dx1,dy1,dx2,dy2)) 
     if(op) tag(view.currmode,n); else untag(view.currmode,n);
   break;
  case tt_thing:
   for(n=l->things.head;n->next!=NULL;n=n->next)
    if(tag_testfunc(lr,&n->d.t->p[0],dx1,dy1,dx2,dy2)) 
     if(op) tag(view.currmode,n); else untag(view.currmode,n);
   break;
  default: my_assert(0);
  }
 }
 
void tagobject(enum infos what)
 {
 my_assert(l!=NULL);
 switch(what)
  {
  case tt_cube:
   if(!view.pcurrcube) return;
   if(!tag(what,view.pcurrcube)) untag(what,view.pcurrcube); 
   break;
  case tt_wall:
   if(!view.pcurrcube) return;
   if(!tag(what,view.pcurrcube,view.currwall))
    untag(what,view.pcurrcube,view.currwall); 
   break;
  case tt_pnt:
   if(!view.pcurrcube) return;
   if(!tag(what,view.pcurrpnt)) untag(what,view.pcurrpnt);
   break;
  case tt_edge:
   if(!view.pcurrcube) return;
   if(!tag(what,view.pcurrcube,view.currwall,view.curredge))
    untag(what,view.pcurrcube,view.currwall,view.curredge);
   break;
  case tt_thing: 
   if(!view.pcurrthing) return;
   if(!tag(what,view.pcurrthing))
    untag(what,view.pcurrthing);
   break;
  case tt_door: 
   if(!view.pcurrdoor) return;
   if(!tag(what,view.pcurrdoor))
    untag(what,view.pcurrdoor);
   break;
  default: my_assert(0);
  }
 plotlevel(); drawopt(what);
 }

void switch_tag(enum infos what,struct node *n,...)
 {
 va_list args;
 int w,e;
 va_start(args,n);
 my_assert(l!=NULL);
 if(n==NULL) return;
 switch(what)
  {
  case tt_cube: case tt_thing: case tt_door: case tt_pnt:
   if(testtag(what,n)) untag(what,n); else tag(what,n); break;
  case tt_wall: 
   w=va_arg(args,int);
   if(testtag(what,n,w)) untag(what,n,w); else tag(what,n,w); break;
  case tt_edge: 
   w=va_arg(args,int); e=va_arg(args,int);
   if(testtag(what,n,w,e)) untag(what,n,w,e); else tag(what,n,w,e); break;
  default: my_assert(0);
  }
 va_end(args);
 }
 
void tagallobjects(enum infos what)
 {
 my_assert(l!=NULL);
 if(l->tagged[what].size==0) tagall(what);
 else untagall(what);
 plotlevel(); drawopt(what);
 }

int comp_tagfilter(struct infoitem *i,unsigned char *d1,unsigned char *d2) 
 { 
 int j;
 for(j=0;j<i->length;j++) if(*(d1+i->offset+j)!=*(d2+j)) break;
 return j==i->length;
 }
 
/* tag all (op==0) objects with the same data in infoitem i or
  untag all (op==1) objects with the same data in infoitem i */ 
void tagfilter(struct infoitem *i,int op,void *data)
 { 
 struct node *n;
 int found,tagged,w;
 my_assert(i!=NULL && data!=NULL);
 if(i->tagnr>=tt_number) return;
 if(i->type==it_cubelight || i->type==it_sidelight || i->type==it_thingcoord
  || i->tagnr==tt_pnt || i->tagnr==tt_edge)
  { printmsg(TXT_CANTUSETAGFILTER); return; }
 switch(i->tagnr)
  {
  case tt_cube: case tt_door:
   for(n=i->tagnr==tt_cube ? l->cubes.head : l->doors.head,found=tagged=0;
    n->next!=NULL;n=n->next)
    if(comp_tagfilter(i,getdata(i->infonr,n),data)) 
     { tagged+=testtag(i->tagnr,n) ? -op : !op; found++;
       if(op) untag(i->tagnr,n); else tag(i->tagnr,n); }
   drawopt(i->infonr); break;
  case tt_thing:
   if(!view.pcurrthing) return;
   for(n=l->things.head,found=tagged=0;n->next!=NULL;n=n->next)
    if(n->d.t->type1==view.pcurrthing->d.t->type1
     || i->offset==0 /* thing type */)
     if(comp_tagfilter(i,getdata(i->infonr,n),data)) 
      { tagged+=testtag(i->tagnr,n) ? -op : !op; found++;
        if(op) untag(i->tagnr,n); else tag(i->tagnr,n); }
   drawopt(i->infonr); break;
  case tt_wall:
   for(n=l->cubes.head,tagged=found=0;n->next!=NULL;n=n->next)
    for(w=0;w<6;w++)
     if(n->d.c->walls[w])
      if(comp_tagfilter(i,getdata(i->infonr,n,w),data)) 
       { tagged+=testtag(i->tagnr,n,w) ? -op : !op; found++;
         if(op) untag(i->tagnr,n,w); else tag(tt_wall,n,w);  }
   drawopt(in_wall); break;
  case tt_pnt: case tt_edge: found=tagged=0; break;
  default: fprintf(errf,"Unknown tagnr: %d\n",i->tagnr); my_assert(0);
  }
 if(op) printmsg(TXT_TAGFILTERUNTAGGED,-tagged,found+tagged);
 else printmsg(TXT_TAGFILTERTAGGED,tagged,found-tagged);
 plotlevel(); 
 }

void mousetagbox(struct leveldata *ld,struct w_event *we,int op)
 {
 int sx,sy,ox,oy,xo,yo,x,y,xs,ys;
 int x1,y1,x2,y2,p;
 struct ws_event ws;
 sx=we->ws->x; sy=we->ws->y; 
 if(ld->whichdisplay)
  { xs=w_xwininsize(we->w)/2;
    xo=sx>=w_xwininsize(we->w)/2 ? xs : 0; }
 else { xs=w_xwininsize(we->w); xo=0; }
 ys=w_ywininsize(we->w); yo=0;
 ws_drawbox(sx,sy,0,0,view.color[WHITE],1);
 ox=w_xscreencoord(we->w,sx)-xo; oy=w_yscreencoord(we->w,sy)-yo;
 do
  {
  ws_getevent(&ws,0);
  x=w_xscreencoord(we->w,ws.x); y=w_yscreencoord(we->w,ws.y);
  if(ld->whichdisplay) x-=xo;
  if(x<0) x=0; else if(x>=xs) x=xs-1;
  if(y<0) y=0; else if(y>=ys) y=ys-1;
  if(ox!=x || oy!=y)
   {
   ws_drawbox(sx,sy,w_xwinincoord(we->w,ox+xo)-sx,
    w_ywinincoord(we->w,oy+yo)-sy,view.color[WHITE],1);
   ox=x; oy=y;
   ws_drawbox(sx,sy,w_xwinincoord(we->w,x+xo)-sx,w_ywinincoord(we->w,y+yo)-sy,
    view.color[WHITE],1);
   }
  }
 while(ws.buttons!=ws_bt_none);
 ws_drawbox(sx,sy,w_xwinincoord(we->w,ox+xo)-sx,w_ywinincoord(we->w,oy+yo)-sy,
  view.color[WHITE],1);
 x1=w_xscreencoord(we->w,sx)-xo-xs/2;
 y1=ys/2-(w_yscreencoord(we->w,sy)-yo);
 x2=ox-xs/2;
 y2=ys/2-oy;
 if(x1>x2) { p=x1; x1=x2; x2=p; }
 if(y1>y2) { p=y1; y1=y2; y2=p; }
 tagbox(xo>0,x1,y1,x2,y2,op);
 }

int tagflatsides(struct node *cube,int wall)
 {
 struct node *tw,*n,*nc;
 int wp1,wp2,w,cn1,i,counter=1;
 struct point dv1,dv2,nv1,nv2;
 float cmp_angle;
 cmp_angle=cos(view.flatsideangle);
 tag(tt_wall,cube,wall);
 for(tw=l->tagged[tt_wall].head;tw->next!=NULL;tw=tw->next)
  {
  n=tw->d.n; w=tw->no%6;
  for(i=0;i<3;i++)
   { dv1.x[i]=n->d.c->p[wallpts[w][2]]->d.p->x[i]-
      n->d.c->p[wallpts[w][0]]->d.p->x[i];
     dv2.x[i]=n->d.c->p[wallpts[w][3]]->d.p->x[i]-
      n->d.c->p[wallpts[w][1]]->d.p->x[i]; }
  VECTOR(&nv1,&dv1,&dv2); normalize(&nv1);
  for(wp1=0;wp1<4;wp1++)
   {
   wp2=(wp1+1)&3;
   /* first get the wall which shares wp1 and wp2 */
   cn1=nb_sides[w][wp1];
   if((nc=n->d.c->nc[cn1])==NULL) nc=n; 
   else
    { /* the next wall lies in the neighbour cube */
    /* now find the right wall */
    if((cn1=findnbwalltoline(n,nc->d.c,w,wp1,wp2))==-1) continue;
    }
   if(nc->d.c->walls[cn1]==NULL) continue;
   /* OK, w should be one wall next to w1, now look, if it's already tagged */
   if(nc->d.c->tagged_walls[cn1]!=NULL) continue;
   /* It is not tagged. So check if we should tag it */
   for(i=0;i<3;i++)
    { dv1.x[i]=nc->d.c->p[wallpts[cn1][2]]->d.p->x[i]-
       nc->d.c->p[wallpts[cn1][0]]->d.p->x[i]; 
      dv2.x[i]=nc->d.c->p[wallpts[cn1][3]]->d.p->x[i]-
       nc->d.c->p[wallpts[cn1][1]]->d.p->x[i]; }
   VECTOR(&nv2,&dv1,&dv2); normalize(&nv2);
   if(SCALAR(&nv1,&nv2)<cmp_angle) continue;
   tag(tt_wall,nc,cn1); counter++;
   }
  }
 return counter;
 }
