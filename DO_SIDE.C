/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_side.c - all functions for side-effects.
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
#include "calctxt.h"
#include "insert.h"
#include "options.h"
#include "plot.h"
#include "tag.h"
#include "plottxt.h"
#include "do_move.h"
#include "do_side.h"

int dsc_cubetype(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 unsigned char *t=d;
 struct producer *cp;
 if(n==NULL) return 0;
 if(*t==cube_producer && !n->d.c->cp)
  {
  if(l->producers.size==MAX_DESCENT_RCENTERS+1)
   waitmsg(TXT_INSTOOMANYRCENTERS);
  checkmem(cp=MALLOC(sizeof(struct producer)));
  checkmem(n->d.c->cp=addnode(&l->producers,l->producers.size,cp));
  *cp=stdproducer;
  cp->c=n;
  }
 else if(*t!=cube_producer && n->d.c->cp)
  {
  killsdoorlist(&n->d.c->sdoors,n->no);
  freenode(&l->producers,n->d.c->cp,free);
  n->d.c->cp=NULL;
  freelist(&n->d.c->sdoors,NULL);
  }
 return setno(i,d,n);
 }
 
int dsc_set_avg_cubelight(struct infoitem *i,void *d,struct node *n,
 int wallno,int pntno,int tagged)
 {
 int j,k;
 unsigned short *no=d;
 if(n==NULL) return 0;
 for(k=0;k<6;k++)
  if(n->d.c->walls[k]!=NULL)
   for(j=0;j<4;j++) n->d.c->walls[k]->corners[j].light=*no;
 if(!tagged) { drawopt(in_wall);  plotlevel(); }
 return 1;
 }
 
int dsc_set_avg_sidelight(struct infoitem *i,void *d,struct node *n,
 int wallno,int pntno,int tagged)
 {
 int j;
 unsigned short *no=d;
 if(n==NULL) return 0;
 if(n->d.c->walls[wallno]!=NULL)
  for(j=0;j<4;j++) n->d.c->walls[wallno]->corners[j].light=*no;
 if(!tagged) { drawopt(in_cube); plotlevel(); }
 return 1;
 }
 
int dsc_set_t2_dir(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 { int r=setno(i,d,n,wallno,pntno); if(!tagged) drawopt(in_wall); return r; }
 
int dsc_change_pnt_coord(struct infoitem *i,void *d,struct node *p,
 int wallno,int pntno,int tagged)
 {
 float oldx,oldy,oldz;
 if(p==NULL) return 0;
 oldx=p->d.p->x[0]; oldy=p->d.p->x[1]; oldz=p->d.p->x[2];
 setno(i,d,p);
 if(!testpnt(p))
  { p->d.p->x[0]=oldx; p->d.p->x[1]=oldy; p->d.p->x[2]=oldz; }
 else newcorners(p);
 if(!tagged) { plotlevel(); drawopt(in_pnt); }
 return 1;
 }
 
int dsc_set_uvl(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 { 
 int r=setno(i,d,n,wallno,pntno);
 if(!r) return 0;
 n->d.c->recalc_polygons[wallno]=1;
 if(!tagged) 
  { plotlevel(); drawopt(in_wall); drawopt(in_cube); }  
 return 1; 
 }

int dsc_thing_type(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 static int change;
 int *no=d;
 if(n==NULL || view.pcurrcube==NULL) return 0;
 if(*no!=n->d.t->type1)
  {
  if(n==l->secretstart) l->secretstart=NULL;
  n->d.t=changething(n->d.t,NULL,*no,view.pcurrcube->d.c);
  if(n->d.t==NULL)
   { printmsg(TXT_CANTMAKETHING); freenode(&l->things,n,NULL); }
  if(n->d.t->type1==tt1_secretstart) l->secretstart=n;
  change=1;
  }
 if(change && !tagged) { plotlevel(); drawopt(in_thing); change=0; }
 return 1;
 }
 
int dsc_robot(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 int *no=d,r;
 if(n==NULL) return 0;
 r=setno(i,d,n);
 if(n->d.t->type1==tt1_robot)
  if(*no>=0 && *no<NUM_ROBOTS) n->d.t->size=robotsize[*no]; 
  else n->d.t->size=D2_stdrobot.t.size; 
 else if(n->d.t->type1==tt1_reactor)
  if(*no>=0 && *no<NUM_REACTORS) 
   { n->d.t->size=reactorsize[*no]; 
     ((struct reactor *)n->d.t)->r.model_num=reactorpolyobj[*no]; }
  else 
   { n->d.t->size=stdreactor.t.size; 
     ((struct reactor *)n->d.t)->r.model_num=stdreactor.r.model_num; }
 else n->d.t->size=view.tsize*2;
 setthingpts(n->d.t);
 if(!tagged) plotlevel();
 return r;
 }

int dsc_hostagesize(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 { 
 int r;
 if(n==NULL) return 0; 
 r=setno(i,d,n); setthingpts(n->d.t); 
 if(!tagged) plotlevel(); 
 return r;
 }
 
int itemgrfx[48]={ 0x24,0x12,0x13,0x14,0x18,0x1a,0x19,0x12,0x12,0x12,
 0x22,0x23,0x33,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x12,
 0x31,0x12,0x12,0x45,0x46,0x47,0x48,0x4d,0x49,0x4a,0x4b,0x4c,0x53,0x4e,0x59,
 0x4f,0x5a,0x5b,0x51,0x66,0x52,0x64,0x65 };
int dsc_itemgrfx(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 struct item *item;  
 int r;
 if(n==NULL) return 0;
 r=setno(i,d,n);
 item=(struct item *)n->d.v;
 item->r.vclip=itemgrfx[item->t.type2<48 ? item->t.type2 : 1];
 if(!tagged) drawopt(in_thing);
 return r;
 }
 
int changedoortype(struct node *n,int no,int tagged)
 {
 my_assert(n->d.d->w!=NULL);
 if(no==n->d.d->type1) return 1;
 if(no!=door1_switchwithwall && n->d.d->d==NULL) 
  { printmsg(TXT_WALLTYPENEEDSNB); return 0; }
 n->d.d->type1=no;
 if(n->d.d->sdoors.size>0)
  {
  killsdoorlist(&n->d.d->sdoors,n->no);
  freelist(&n->d.d->sdoors,NULL);
  if(!tagged) plotlevel();
  }
 if(no==door1_blow)
  {
  n->d.d->animtxt=std_anim_blowdoor;
  n->d.d->w->texture1=pig.anims[std_anim_blowdoor]->rdlno;
  n->d.d->w->texture2=0;
  n->d.d->hitpoints=std_blowdoorhp;
  if(!tagged && view.pcurrwall==n->d.d->w) drawopt(in_wall);
  }
 else n->d.d->hitpoints=0;
 if(no==door1_onlyswitch) /* only switch: set texture to empty/nothing */
  { /* is not required because DESCENT ignores it anyway */
  n->d.d->w->texture1=init.d_ver>=d2_10_sw ? D2_std_emptytxt:D1_std_emptytxt;
  n->d.d->w->texture2=0; /* nothing */
  if(!tagged && view.pcurrwall==n->d.d->w) drawopt(in_wall);
  }
 if(no==door1_switchwithwall)
  { n->d.d->w->texture2=std_switch_t2;
    if(n->d.d->sd) n->d.d->sd->d.sd->flags|=switchflag_once;
    if(!tagged && view.pcurrwall==n->d.d->w) drawopt(in_wall); }
 if(no!=door1_blow && no!=door1_normal) /* no door, no blow door -> no anim*/
  n->d.d->animtxt=-1;
 if(no!=door1_normal) n->d.d->key=1; /* no door, no key */
 if(!tagged) drawopt(in_door);
 return 1;
 }
 
int dsc_walltype(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 int no=*(int *)d,ok;
 my_assert(n!=NULL);
 ok=changedoortype(n,no,tagged);
 if(ok && n->d.d->d && (no==door1_normal || no==door1_blow || 
  n->d.d->d->d.d->type1==door1_normal || n->d.d->d->d.d->type1==door1_blow))
  changedoortype(n->d.d->d,no,tagged);
 plotlevel();
 return ok;
 }
 
int dsc_dooranim(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 char no=*(char *)d;
 my_assert(n!=NULL && n->d.d->w!=NULL && n->d.d->d!=NULL && 
  n->d.d->d->d.d->w!=NULL)
 my_assert(no>=0 && no<pig.num_anims && pig.anims[no]!=NULL &&
  pig.anims[no]->pig);
 if(!pig.anims[no]->pig->anim_t2) 
  {
  n->d.d->w->texture2=0; 
  if(n->d.d->old_txt1==-1) n->d.d->old_txt1=n->d.d->w->texture1;
  n->d.d->w->texture1=pig.anims[no]->rdlno;
  n->d.d->d->d.d->w->texture2=0; 
  if(n->d.d->d->d.d->old_txt1==-1)
   n->d.d->d->d.d->old_txt1=n->d.d->d->d.d->w->texture1;
  n->d.d->d->d.d->w->texture1=pig.anims[no]->rdlno; 
  }
 else 
  {
  if(n->d.d->old_txt1>=0) n->d.d->w->texture1=n->d.d->old_txt1;
  n->d.d->old_txt1=-1;
  n->d.d->w->texture2=pig.anims[no]->rdlno;
  if(n->d.d->d->d.d->old_txt1>=0)
   n->d.d->d->d.d->w->texture1=n->d.d->d->d.d->old_txt1;
  n->d.d->d->d.d->old_txt1=-1;
  n->d.d->d->d.d->w->texture2=pig.anims[no]->rdlno;
  }
 n->d.d->animtxt=n->d.d->d->d.d->animtxt=no;
 if(!tagged && 
  (n->d.d->w==view.pcurrwall || n->d.d->d->d.d->w==view.pcurrwall))
  drawopt(in_wall);
 return 1;
 }
 
int dsc_switch(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 struct sdoor *sd;
 unsigned short no=*(unsigned short *)d;
 struct node *sn;
 int r;
 my_assert(n!=NULL);
 if(tagged && no!=switch_producer) return 0;
 if(no==switch_nothing) /* delete switch */
  { if(n->d.d->sd!=NULL) deletesdoor(n->d.d->sd);
    r=setno(i,d,n); if(!tagged) { plotlevel(); drawopt(in_door); } 
    return r; }
 if(n->d.d->sd==NULL) insertsdoor(n); /* make switch */
 sd=n->d.d->sd->d.sd;
 cleansdoor(sd);
 r=setno(i,d,n);
 sd->type=n->d.d->switchtype;
 switch(getsdoortype(sd))
  {
  case sdtype_none:
   if(sd->type==switch_secretexit && init.d_ver>=d2_10_sw &&
    l->secretstart==NULL) 
    makesecretstart(l,n,n->d.d->c); 
   break;
  case sdtype_cube:
   for(sn=l->tagged[tt_cube].head;sn->next!=NULL;sn=sn->next)
    {
    if(sn->d.n->d.c->type!=4)
     { if(!pntno) printmsg(TXT_TAGGEDCUBENOPROD,sn->d.n->no); continue; }
    if(sd->num>=10)
     { if(!pntno) printmsg(TXT_TOOMUCHCUBESTAGGED); break; }
    checkmem(addnode(&sn->d.n->d.c->sdoors,sd->d->no,sd->d));
    sd->target[sd->num++]=sn->d.n;
    }
   break;
  case sdtype_side:
   for(sn=l->tagged[tt_wall].head;sn->next!=NULL;sn=sn->next)
    {
    if(sd->num>=10)
     { if(!pntno) printmsg(TXT_TOOMUCHCUBESTAGGED); break; }
    checkmem(addnode(&sn->d.n->d.c->sdoors,sd->d->no,sd->d));
    sd->walls[sd->num]=sn->no%6;
    sd->target[sd->num++]=sn->d.n;
    }
   break;
  case sdtype_door:
   for(sn=l->tagged[tt_door].head;sn->next!=NULL;sn=sn->next)
    {
    if(sn->no==n->no) { printmsg(TXT_DOOROPENITSELF); continue; }
    if(sn->d.n->d.d->c->d.c->nc[sn->d.n->d.d->wallnum]==NULL)
     { printmsg(TXT_NOSWITCHABLEWALL); continue; }
    if((no==switch_opendoor || no==switch_closedoor || no==switch_unlockdoor
     || no==switch_lockdoor) && sn->d.n->d.d->type1!=door1_normal)
     { printmsg(TXT_TAGGEDWALLNODOOR,sn->d.n->no); continue; } 
    if((no==switch_openwall || no==switch_closewall || no==switch_illusion_off
    || no==switch_illusion_on || no==switch_wall_to_ill) &&
     sn->d.n->d.d->type1!=door1_onlyswitch && 
     sn->d.n->d.d->type1!=door1_shootthrough &&
     sn->d.n->d.d->type1!=door1_onlytxt)
     { printmsg(TXT_TAGGEDWALLNOSWITCHILLWALL,sn->d.n->no); continue; }
    if(sd->num>=10) { printmsg(TXT_TOOMUCHDOORSTAGGED); break; }
    addnode(&sn->d.n->d.d->sdoors,sd->d->no,sd->d);
    sd->target[sd->num++]=sn->d.n;
    }
   break;
  default: my_assert(0);
  }
 if(!tagged) { plotlevel(); drawopt(in_door); }
 return r;
 }
 
int dsc_illum_brightness(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 float dno=*(float *)d;
 if(dno<0.01 || dno>100)
  { waitmsg(TXT_ILLUM_BRIGHTNESS); drawopt(in_internal); return 0; }
 set_illum_brightness(dno);
 return 1;
 }
 
int dsc_dropsomething(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 if(!setno(i,d,n)) return 0;
 if(n->d.t->contain_type1==0 || n->d.t->contain_type1==255)
  n->d.t->contain_count=0;
 if(!tagged) drawopt(in_thing);
 return 1;
 }
 
int dsc_changedrawwhat(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 if(!setno(i,d,n)) return 0;
 if((view.drawwhat&DW_ALLLINES)==0) 
  { printmsg(TXT_CALCMAP); initdescmap(); }
 printmsg(TXT_NODRAWING,
  view.drawwhat&DW_CUBES ? TXT_DWCUBES : "",
  view.drawwhat&DW_DOORS ? TXT_DWWALLS : "",
  view.drawwhat&DW_THINGS ? TXT_DWTHINGS : "",
  view.drawwhat&DW_ALLLINES ? TXT_DWLINES : "",
  view.drawwhat&DW_XTAGGED ? TXT_DWXTAGGED : "");
 if(view.render) init_rendercube(); 
 plotlevel();
 return 1;
 }
 
int dsc_perspective(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 if(!setno(i,d,n)) return 0;
 if(!l) return 1;
 l->whichdisplay=view.whichdisplay;
 if(view.whichdisplay) 
  { view.drawwhat|=DW_CUBES;
    if(view.render>1) view.render=1;
    drawopt(in_internal); } 
 init_rendercube();
 w_refreshwin(l->w);
 return 1;
 }
 
int dsc_thingpos(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 float dno=*(float *)d;
 my_assert(i->offset>=0 && i->offset<3);
 n->d.t->p[0].x[i->offset]=dno;
 setthingpts(n->d.t); setthingcube(n->d.t);
 if(!tagged) plotlevel();
 return 1;
 }
 
int dsc_flythrough(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 int j,k,nft=*(int *)d;
 if(nft>1)
  { view.whichdisplay=0; if(l) l->whichdisplay=view.whichdisplay; }
 else view.drawwhat|=DW_CUBES;
 view.render=nft;
 if(!view.pcurrcube) return 1;
 if(view.render==0 && nft>0) render_resetlights(l);
 if(view.render==3)
  for(j=0;j<3;j++)
   {
   view.e0.x[j]=0.0;
   for(k=0;k<8;k++) 
    view.e0.x[j]+=view.pcurrcube->d.c->p[k]->d.p->x[j]/8.0;
   }
 init_rendercube();
 w_refreshwin(l->w);
 return 1;
 }
 
int dsc_fl_delay(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 int delay=*(long *)d;
 struct flickering_light *fl;
 struct node *nc;
 if(!l || !n || !n->d.c->walls[wallno] || !n->d.c->walls[wallno]->ls) return 0;
 if(delay<1)
  {
  if(n->d.c->walls[wallno]->ls->d.ls->fl)
   delflickeringlight(n->d.c->walls[wallno]->ls->d.ls);
  if(!tagged) drawopt(in_wall);
  return 1;
  }
 if(!n->d.c->walls[wallno]->ls->d.ls->fl)
  {
  if(delay<1) return 0; 
  checkmem(n->d.c->walls[wallno]->ls->d.ls->fl=fl=
   MALLOC(sizeof(struct flickering_light)));
  fl->cube=n->no; fl->wall=wallno; fl->mask=0xffffffff;
  fl->ls=n->d.c->walls[wallno]->ls; fl->state=1;
  fl->calculated=0;
  for(nc=n->d.c->walls[wallno]->ls->d.ls->effects.head;nc->next!=NULL;
      nc=nc->next)
   checkmem(addnode(&nc->d.lse->cube->d.c->fl_lights,-1,fl));
  }
 else fl=n->d.c->walls[wallno]->ls->d.ls->fl;
 fl->delay=fl->timer=delay;
 if(!tagged) drawopt(in_wall);
 return 1;
 }

int dsc_plotlevel(struct infoitem *i,void *d,struct node *n,int wallno,
 int pntno,int tagged)
 {
 if(!setno(i,d,n)) return 0;
 if(!l) return 1;
 plotlevel();
 return 1;
 }

/* i is the current infoitem, d is a pointer to the data which was given
   by the user and may be changed in the functions, n is a pointer to
   the node which should be changed by the data. If needed wallno and
   pntno must be set to provide additional information where to find
   the data to be changed.
   tagged indicates if the change is done on a tagged object or on
   the current object. The tagged objects are changed before the current
   objects are changed, so it is wise to refresh the screen only if
   tagged==0 (to save time). 
   The functions must change the original data. This is not done before
   or after calling this function. If they change the data return a 1
   if not, return a 0 */
int (*dsc_sideeffect[sc_number])(struct infoitem *i,void *d,
 struct node *n,int wallno,int pntno,int tagged)=
 { dsc_cubetype,dsc_set_avg_cubelight,dsc_set_t2_dir,dsc_set_avg_sidelight,
   dsc_change_pnt_coord,dsc_set_uvl,dsc_thing_type,dsc_robot,
   dsc_hostagesize,dsc_itemgrfx,dsc_walltype,dsc_dooranim,dsc_switch,
   dsc_illum_brightness,dsc_dropsomething,dsc_changedrawwhat,
   dsc_thingpos,dsc_perspective,dsc_flythrough,dsc_fl_delay,dsc_plotlevel };

int do_sideeffect(enum sidecodes sc,struct infoitem *i,void *d,
 struct node *n,int wallno,int pntno,int tagged)
 { return dsc_sideeffect[sc](i,d,n,wallno,pntno,tagged); }
