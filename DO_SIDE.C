/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_side.c - included in "change.c". all functions for side-effects.
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
    
enum sidecodes { sc_changeitemtype,sc_changetxt,sc_changeswitch,
 sc_producer,sc_changepnt,sc_itemgrfx,sc_changedooranim,sc_setorigin,
 sc_changecubetype,sc_changedoortype,sc_doorchangetxt1,sc_doorchangetxt2,
 sc_changethingtxt,sc_number };

int dsc_changeitemtype(struct infoitem *i,int to,int no)
 { 
 struct thing *t;
 if(view.pcurrthing==NULL)
  { printmsg("No current thing."); return no; }
 t=view.pcurrthing->d.t;
 view.pcurrthing->d.t=changething(t,no,view.pcurrcube->d.c);
 free(t);
 if(view.pcurrthing->d.t==NULL)
  { printmsg("Can't make thing.");
    freenode(&l->things,view.pcurrthing,NULL); }
 return no;
 }

void drawtxt(int n,int *data)
 {
 struct wall *w=view.pcurrcube->d.c->walls[view.currwall];
 if(*data==0)
  w->texture1=n;
 else
  { w->texture2=n|*(data+1); }
 drawtextures(w);
 }
 
int dsc_changetxt(struct infoitem *i,int to,int no)
 {
 int n=no,ok=1,data[2];
 struct objdata *od;
 if(!view.pcurrcube || view.pcurrcube->d.c->walls[view.currwall]==NULL)
  { if(!to) printmsg("No current wall."); return no; }
 if(view.pcurrcube->d.c->d[view.currwall]!=NULL)
  od=&view.txtnums[i->offset/2+txt1_wall];
 else
  od=&view.txtnums[i->offset/2];
 if(!to)
  {
  if(i->offset==0)
   { data[0]=0; 
     ok=readltype(drawtxt,od->str,od,1,&n,data); }
  else
   { data[0]=1;
     data[1]=view.pcurrcube->d.c->walls[view.currwall]->texture2&0xc000;
     ok=readltype(drawtxt,od->str,od,1,&n,data); }
  }
 if(ok) return n;
 else return no;
 }

int dsc_changethingtxt(struct infoitem *i,int to,int no)
 {
 int n=no,ok=1,data=0;
 struct objdata *od;
 if(!view.pcurrthing)
  { printmsg("No current thing."); return no; }
 od=&view.txtnums[0];
 if(!to) ok=readltype(drawtxt,od->str,od,1,&n,&data); 
 if(ok) return n;
 else return no;
 }
 
int dsc_doorchangetxt(struct infoitem *i,int to,int no)
 {
 int n,data[2];
 int ok=1;
 struct objdata *od;
 if(!view.pcurrdoor || !view.pcurrdoor->d.d->w)
  { if(!to) printmsg("No wall."); return no; }
 n=(i->sidefuncnr==sc_doorchangetxt1) ? view.pcurrdoor->d.d->w->texture1 :
  view.pcurrdoor->d.d->w->texture2;
 od=&view.txtnums[txt1_wall+i->sidefuncnr-sc_doorchangetxt1];
 if(!to)
  {
  if(i->sidefuncnr==sc_doorchangetxt1)
   { data[0]=0; 
     ok=readltype(drawtxt,od->str,od,1,&n,data); }
  else
   { data[0]=1;
     data[1]=view.pcurrdoor->d.d->w->texture2&0xc000;
     ok=readltype(drawtxt,od->str,od,1,&n,data); }
  }
 else n=no; /* for tagged objects */
 if(ok) 
  if(i->sidefuncnr==sc_doorchangetxt1) 
   { view.pcurrdoor->d.d->w->texture1=n;
     if(view.pcurrdoor->d.d->d && view.pcurrdoor->d.d->d->d.d->w)
      view.pcurrdoor->d.d->d->d.d->w->texture1=n; }
  else
   { view.pcurrdoor->d.d->w->texture2=n;
     if(view.pcurrdoor->d.d->d && view.pcurrdoor->d.d->d->d.d->w)
      view.pcurrdoor->d.d->d->d.d->w->texture2=n; }
 return no;
 }

int dsc_changeswitch(struct infoitem *i,int to,int no)
 {
 struct node *n;
 struct sdoor *sd;
 if(view.pcurrdoor==NULL)
  { printmsg("No current wall."); return no; }
 if(no==0) /* delete switch */
  { 
  deletesdoor(view.pcurrdoor->d.d->sd); view.pcurrdoor->d.d->sd=NULL; 
  return no;
  }
 if(view.pcurrdoor->d.d->sd==NULL) /* make switch */
  insertsdoor(view.pcurrdoor);
 sd=view.pcurrdoor->d.d->sd->d.sd;
 cleansdoor(sd);
 switch(no)
  {
  case 0x0800: case 0x10000: break; /* exit, alternate exit */
  case 0x4000: /* activate producer */
   for(n=view.tagged[tt_cube].head;n->next!=NULL;n=n->next)
    {
    if(n->d.n->d.c->type!=4)
     { if(to==0) printmsg("Tagged cube %d is not a producer.",n->d.n->no);
       continue; }
    if(sd->num>=10)
     { if(to==0) printmsg("Too much cubes tagged (maximum number 10)."); 
       break; }
    addnode(&n->d.n->d.c->sdoors,sd->d->no,sd->d);
    sd->target[sd->num++]=n->d.n;
    }
   break;
  case 0x0100:  /* open door */
   for(n=view.tagged[tt_door].head;n->next!=NULL;n=n->next)
    {
    if(n->no==view.pcurrdoor->no) 
     { printmsg("Door can't open itself."); continue; }
    if(n->d.n->d.d->type1!=2)
     { if(to==0) printmsg("Tagged wall %d is not a door.",n->d.n->no); 
       continue; }
    if(sd->num>=10)
     { if(to==0) printmsg("Too much doors tagged (maximum number 10)."); 
       break; }
    addnode(&n->d.n->d.d->sdoors,sd->d->no,sd->d);
    sd->target[sd->num++]=n->d.n;
    }
   break;
  default: printmsg("Unknown switch type: %x in door %d ?",no,
   view.pcurrdoor->no); 
  }
 return no;
 }

int dsc_changepnt(struct infoitem *i,int to,int no)
 {
 double oldpos,*pos;
 char txt[5];
 if(getdata(i->infonr)==NULL)
  { printmsg("No current pnt."); return no; }
 pos=(double *)(getdata(i->infonr)+i->offset);
 oldpos=*pos;
 strncpy(txt,i->txt,4); txt[4]=0;
 if(readkeyb(txt,"%lg",pos)==0) return no; 
 *pos*=65536.0;
 if(!testpnt(view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]))
  *pos=oldpos;
 else
  newcorners(view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]);
 return no;
 }
 
int dsc_itemgrfx(struct infoitem *i,int to,int no)
 {
 struct objtype **ot;
 struct infoitem *igrfx;
 int k;
 if(view.pcurrthing==NULL)
  { printmsg("No current item."); return no; }
 if((igrfx=findinfoitem(i->infonr,83))==NULL)
  { printmsg("Can't get graphics infoitem."); return no; }
 for(ot=i->od->data,k=-1;ot-i->od->data<i->od->size;ot++)
  if((*ot)->no==no) { k=ot-i->od->data; break; }
 if(k==-1) return no;
 for(ot=igrfx->od->data;ot-igrfx->od->data<igrfx->od->size;ot++)
  if(strcmp((*ot)->str,i->od->data[k]->str)==0)
   { *(getdata(i->infonr)+83)=(*ot)->no; break; }
 return no;
 }
 
int dsc_producer(struct infoitem *i,int to,int no)
 {
 int k;
 struct infoitem *things=findinfoitem(ds_thing,0),*robots=NULL;
 int tagged[32];
 unsigned long rno=no;
 struct producer *cp;
 if(!view.pcurrcube) { printmsg("No cube."); return no; }
 if(view.pcurrcube->d.c->cp==NULL)
  {
  if((cp=malloc(sizeof(struct producer)))==NULL)
   { printmsg("No mem for producer."); return no; }
  if((view.pcurrcube->d.c->cp=addnode(&l->producers,l->producers.size,cp))
   ==NULL)
   { printmsg("Can't insert node for producer."); free(cp); return no; }
  *cp=stdproducer;
  cp->c=view.pcurrcube;
  }
 if(!to)
  {
  if(things==NULL)
   { printmsg("Can't find thingtype????"); return rno; }
  for(k=0;k<things->numchildren;k++)
   if(things->itemchildren[k]==2) { robots=things->children[k]; break; }
  if(robots==NULL) 
   { printmsg("Can't find robots????"); return rno; }
  for(k=0;k<32;k++) tagged[k]=-1;
  for(k=0;k<robots->od->size;k++)
   if(rno&(1<<robots->od->data[k]->no))
    tagged[k]=robots->od->data[k]->no;
  if(!readltype(NULL,i->txt,robots->od,32,tagged,NULL)) return rno;
  for(k=0,rno=0;k<32;k++)
   if(tagged[k]>=0) rno|=(1<<tagged[k]);
  }
 if(rno==0)
  {
  killsdoorlist(&view.pcurrcube->d.c->sdoors,view.pcurrcube->no);
  freenode(&l->producers,view.pcurrcube->d.c->cp,free);
  freelist(&view.pcurrcube->d.c->sdoors,NULL);
  view.pcurrcube->d.c->cp=NULL;
  }
 return rno;
 }
 
int dsc_changecubetype(struct infoitem *i,int to,int no)
 {
 if(!view.pcurrcube) return no;
 if(no!=4 && view.pcurrcube->d.c->cp)
  {
  killsdoorlist(&view.pcurrcube->d.c->sdoors,view.pcurrcube->no);
  freenode(&l->producers,view.pcurrcube->d.c->cp,free);
  view.pcurrcube->d.c->cp=NULL;
  freelist(&view.pcurrcube->d.d->sdoors,NULL);
  }
 return no;
 }
 
int dsc_changedoortype(struct infoitem *i,int to,int no)
 {
 if(!view.pcurrdoor) return no;
 if(no!=2 && view.pcurrdoor->d.d->sdoors.size>0)
  {
  killsdoorlist(&view.pcurrdoor->d.d->sdoors,view.pcurrdoor->no);
  freelist(&view.pcurrdoor->d.d->sdoors,NULL);
  }
 return no;
 }
 
void drawdoortxt(int n,int *data)
 {
 int no;
 if(n<0) return;
 no=view.doorstarts[n];
 if(no>=0)
  { view.pcurrdoor->d.d->w->texture1=no;
    view.pcurrdoor->d.d->w->texture2=0; }
 else
  { view.pcurrdoor->d.d->w->texture1=*data; 
    view.pcurrdoor->d.d->w->texture2&=0xc000;
    view.pcurrdoor->d.d->w->texture2|=(-no)&0x3fff; }
 drawtextures(view.pcurrdoor->d.d->w);
 }
 
int dsc_changedooranim(struct infoitem *i,int to,int no)
 {
 int n=no,ot1,ot2;
 if(view.pcurrdoor==NULL)
  { printmsg("No current wall."); return no; }
 if(view.pcurrdoor->d.d->w==NULL)
  { printmsg("No side there."); return no; }
 ot1=view.pcurrdoor->d.d->w->texture1;
 ot2=view.pcurrdoor->d.d->w->texture2;
 if(to || readltype(drawdoortxt,i->txt,&view.doornums,1,&n,&ot1))
  {
  to=1;
  if(view.pcurrdoor->d.d->d!=NULL && view.pcurrdoor->d.d->d->d.d->w!=NULL)
   if(view.doorstarts[n]>=0) 
    { view.pcurrdoor->d.d->d->d.d->w->texture2=0; 
      view.pcurrdoor->d.d->d->d.d->w->texture1=view.doorstarts[n]; }
  else 
   { view.pcurrdoor->d.d->d->d.d->w->texture2&=0xc000;
     view.pcurrdoor->d.d->d->d.d->w->texture2|=(-view.doorstarts[n])&0x3fff; }
  printmsg("Texture changed.");
  }
 if(!to)
  { view.pcurrdoor->d.d->w->texture1=ot1;
    view.pcurrdoor->d.d->w->texture2=ot2; }
 return n;
 }

void draworigintxt(int n,int *data)
 { 
 struct wall *w=view.pcurrcube->d.c->walls[view.currwall]; 
 w->texture2=(w->texture2&0x3fff)|(n<<14);
 drawtextures(w);
 }
 
int dsc_setorigin(struct infoitem *i,int to,int no)
 {
 int odir,direction;
 if(view.pcurrcube->d.c->walls[view.currwall]==NULL)
  { printmsg("No side here."); return no; }
 if(view.pcurrcube->d.c->walls[view.currwall]->texture2==0)
  { printmsg("No second texture."); return no; }
 if(!to)
  {
  odir=direction=(view.pcurrcube->d.c->walls[view.currwall]->texture2>>14)&3;
  if(!readltype(draworigintxt,i->txt,i->od,1,&direction,NULL))
   { view.pcurrcube->d.c->walls[view.currwall]->texture2&=0x3fff;
     view.pcurrcube->d.c->walls[view.currwall]->texture2|=odir<<14; }
  }
 else
  { view.pcurrcube->d.c->walls[view.currwall]->texture2&=0x3fff;
    view.pcurrcube->d.c->walls[view.currwall]->texture2|=no<<14; } 
 return direction;
 }
 
int dsc_dummy(struct infoitem *i,int to,int no)
 { printmsg("DUMMYDUMMYDUMMYDUMMY"); return no; }
 
/* i is the current infoitem, to=0 if original,=1 if tagged objects,
   no is the number from the original for the tagged objects.
   returnvalue is only for to=0 valid, then it's the entered value 
   (for side effects) */
int (*do_extrasideeffect[sc_number])(struct infoitem *i,int to,
 int no) =
 { dsc_changeitemtype,dsc_changetxt,dsc_changeswitch,dsc_producer,
   dsc_changepnt,dsc_itemgrfx,dsc_changedooranim,dsc_setorigin,
   dsc_changecubetype,dsc_changedoortype,dsc_doorchangetxt,
   dsc_doorchangetxt,dsc_changethingtxt};
