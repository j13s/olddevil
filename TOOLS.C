/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    tools.c - all functions which have no better place
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

extern int supervga;

int drawnumlock(void)
 {
 int xoffs=view.bounds[ecw_movebuttons][0],
  yoffs=view.bounds[ecw_movebuttons][1],xsize,ysize;
 xsize=(view.bounds[ecw_movebuttons][2]-xoffs)/3;
 ysize=(view.bounds[ecw_movebuttons][3]-yoffs)/3;
 sys_drawbutton(xoffs,yoffs+2*ysize,xoffs+3*xsize,yoffs+3*ysize,
  sys_getnumlock() ? "Move" : "Modify");
 return sys_getnumlock();
 }
 
unsigned char *getsidedata(unsigned int *se,int *length)
 {
 unsigned char *data;
 int d,maxd;
 maxd=*(se++);
 if((data=getdata(*(se++)))==NULL) return NULL;
 for(d=0;d<maxd-3;d++) 
  if((data=*(unsigned char **)(data+*(se++)))==NULL) return NULL;
 *length=*(se+1);
 return data+*se;
 }
 
int getsideno(unsigned int *se,unsigned long *no)
 {
 int length,d;
 unsigned char *data;
 if((data=getsidedata(se,&length))==NULL) return 0;
 for(d=0,*no=0;d<length;d++)
  *no|=(*(data+d)<<(d*8));
 return 1;
 }
 
void setsideno(unsigned int *se,unsigned long no)
 {
 unsigned char *data;
 int length,d;
 if((data=getsidedata(se,&length))==NULL) return;
 for(d=0;d<length;d++)
  *(data+d)=(no>>(d*8))&0xfful;
 }
 
void dosideeffects(struct infoitem *i,unsigned long no)
 {
 int k;
 for(k=0;k<i->numsideeffs;k++) setsideno(i->sideeffs[k],no);
 }
 
struct infoitem *findnpmenuitem(int row,int column,int nextprev,
 int *mindist,struct infoitem *mini,struct infoitem *is,int num)
 {
 unsigned long no;
 int dist,k;
 struct infoitem *i;
 for(i=is;i-is<num;i++)
  {
  dist=((i->row-row)*view.smenuwidth+i->column-column)*nextprev;
  if(dist>=0 && dist<*mindist)
   { *mindist=dist; mini=i; }
  if(i->numchildren>0)
   {
   if(getno(i,&no))
    for(k=0;k<i->numchildren;k++)
     if(i->itemchildren[k]==no)
      mini=findnpmenuitem(row,column,nextprev,mindist,mini,i->children[k],
       i->numechildren[k]);
   }
  }
 return mini;
 }

/* searches next (nextprev=1) or previous (nextprev=-1) menuitem.
   if i==NULL the first or last menuitem is returned */
struct infoitem *findmenuitem(struct infoitem *i,int nextprev)
 {
 int mindist=view.smenuheight*view.smenuwidth;
 struct infoitem *si;
 if(i!=NULL)
  si=findnpmenuitem(i->row,i->column+nextprev,nextprev,&mindist,
   NULL,view.info[view.showwhat],view.infonum[view.showwhat]);
 else si=NULL;
 if(si==NULL)
  if(nextprev==1)
   si=findnpmenuitem(0,0,1,&mindist,NULL,view.info[view.showwhat],
    view.infonum[view.showwhat]);
  else
   si=findnpmenuitem(view.menuheight,view.smenuwidth,-1,&mindist,
    NULL,view.info[view.showwhat],view.infonum[view.showwhat]);
 return si;
 }
 
int fwritestring(FILE *f,char *s)
 {
 int n;
 n=strlen(s)+1;
 if(fwrite(&n,sizeof(int),1,f)!=1)
  { printmsg("Can't save string length for %s.",s); return 0; }
 if(fwrite(s,1,strlen(s)+1,f)!=strlen(s)+1)
  { printmsg("Can't save string %s.",s); return 0; }
 return 1;
 }
 
char *freadstring(FILE *f)
 {
 int i;
 char *e;
 if(fread(&i,sizeof(int),1,f)!=1)
  { fprintf(errf,"Can't read size of string in freadstring\n"); return NULL; }
 if((e=malloc(i))==NULL)
  { fprintf(errf,"No mem for text (%d) in freadstring.\n",i); return NULL; }
 if(fread(e,1,i,f)!=i)
  { fprintf(errf,"Can't read string in freadstring\n"); free(e); return NULL;}
 return e;
 }
 
int readmacroheader(FILE *f,struct macro *m)
 {
 char head[5];
 if(fread(head,1,5,f)!=5)
  { fprintf(errf,"Can't read macro head for macro file %s.\n",m->filename);
    return 0; }
 if(strncmp(head,"MACRO",5)!=0) 
  { fprintf(errf,"No macro: %s in %s\n",head,m->filename); return 0; }
 if(fread(&m->groupno,sizeof(int),1,f)!=1)
  { fprintf(errf,"Can't group number in macrofile %s\n",m->filename); 
    return 0; }
 free(m->shorttxt);
 if((m->shorttxt=freadstring(f))==NULL)
  { fprintf(errf,"Can't read shorttxt for macro %s\n",m->filename);return 0; }
 initlist(&m->cubes); initlist(&m->things); initlist(&m->doors);
 initlist(&m->pts); initlist(&m->sdoors); initlist(&m->producers);
 m->wallno=0; m->longtxt=NULL;
 return 1;
 }
 
struct infoitem *findii(struct infoitem *i,int num,int offset)
 {
 int k,l;
 struct infoitem *reti;
 for(k=0;k<num;k++)
  {
  if(i[k].offset==offset) return &i[k];
  for(l=0;l<i[k].numchildren;l++)
   if((reti=findii(i[k].children[l],i[k].numdchildren[l],offset))!=NULL)
    return reti;
  }
 return NULL;
 }

struct infoitem *findinfoitem(enum infos incode,int offset)
 { return findii(view.info[incode],view.descripnum[incode],offset); }

/* in t is the old structure (NULL is possible), newtype is the new type1 of
   the thing and c is for the orientation */
struct thing *changething(struct thing *t,int newtype,struct cube *c)
 {
 struct thing *t2;
 struct point coords[3];
 int j,i;
 size_t size=0;
 void *data=NULL;
 if(t!=NULL && newtype==t->type1) return t;
 switch(newtype)
  {
  case 2: size=sizeof(struct robot); data=&stdrobot; break;
  case 3: size=sizeof(struct item); data=&stdhostage; break;
  case 4: size=sizeof(struct start); data=&stdstart; break;
  case 7: size=sizeof(struct item); data=&stditem; break;
  case 9: size=sizeof(struct reactor); data=&stdreactor; break;
  case 0xe: size=sizeof(struct start); data=&stdcoopstart; break;
  default: fprintf(errf,"Unknown thing type %d in changething.\n",t->type1);
   my_exit();
  }
 if((t2=malloc(size))==NULL)
  { printmsg("No mem for extended thing."); return NULL; }
 memcpy(t2,data,size);
 if(t!=NULL)  
  { for(j=0;j<3;j++) t2->p[j]=t->p[j]; 
    t2->type1=newtype; }
 if(t!=NULL) /* orientation */
  for(j=0;j<9;j++) t2->orientation[j]=t->orientation[j];
 else 
  {
  for(j=0;j<3;j++)
   {
   coords[0].x[j]=c->p[0]->d.p->x[j]-c->p[3]->d.p->x[j];
   coords[1].x[j]=c->p[1]->d.p->x[j]-c->p[0]->d.p->x[j];
   }
  normalize(&coords[0]);
  for(j=0;j<3;j++)
   coords[1].x[j]-=SCALAR(&coords[0],&coords[1])*coords[0].x[j];
  normalize(&coords[1]);
  VECTOR(&coords[2],&coords[0],&coords[1]);
  for(j=0;j<3;j++)
   for(i=0;i<3;i++)
    t2->orientation[j*3+i]=coords[j].x[i]*65536;
  }
 setthingpts(t2);
 view.oldpcurrthing=NULL;
 return t2;
 }
 
void printdatabyte(int row,int column,unsigned char *data,int offset)
 { sprintf(&view.menubuffer[(row+1)*view.smenuwidth+column+5],
    "%.2x",(unsigned int)data[offset]); }

void markdatabyte(struct infoitem *i,int byte)
 {
 int x1,y1,x2,y2,row,column;
 row=(i->offset+byte)/4+view.maxrow[view.showwhat]+3-view.menuoffset+1;
 column=((i->offset+byte)%4)*3+4;
 x1=column*view.fontwidth+view.fontwidth/2+view.bounds[ecw_uppermenu][0];
 y1=row*view.fontheight+view.bounds[ecw_uppermenu][1]+1;
 x2=x1+3*view.fontwidth-1;
 y2=y1+view.fontheight-2;
 sys_filledbox(x1,y1,x2,y2,view.color[(HILIGHTCOLORS+i->class)],1);
 if(byte==0) sys_line(x1,y1,x1,y2,view.color[WHITE],0);
 if(byte==i->length-1) sys_line(x2,y1,x2,y2,view.color[WHITE],0);
 }
 
/* reads texture sd from file f in direction dir.
   dir=0 -> normal (origin left upper corner x+ y+).
   dir=1 -> 90ø   (origin right upper corner x- y+). 
   dir=2 -> 180ø   (origin right lower corner x- y-).
   dir=3 -> 270ø    (origin left lower corner x+ y-). */
void readbitmap(FILE *f,struct texture *sd,int dir)
 {
 unsigned char *sbitmap,*bitmap=view.txt_sbuffer,
  *buffer,*fill,*nobytesinrow;
 unsigned int size;
 unsigned int i;
 int startx,starty,endx,endy,addx,addy,mx,my,x,y;
 if(sd->xsize!=64 || sd->ysize!=64)
  { printmsg("Texture of wrong size: %d %d",sd->xsize,sd->ysize); return; }
 switch(dir)
  {
  case 0: startx=starty=0; addx=addy=1; mx=1; my=64; break;
  case 1: startx=63; starty=0; addx=-1; addy=1; mx=64; my=1; break;
  case 2: startx=starty=63; addx=addy=-1; mx=1; my=64; break;
  case 3: startx=0; starty=63; addx=1; addy=-1; mx=64; my=1; break;
  default:
   printmsg("Unknown direction in readbitmap: %d\n",dir); return;
  }
 endx=63-startx; endy=63-starty;
 switch(sd->type1)
  {
  case 0: case 0x10: case 1: case 0x11:
   y=starty-addy;
   do
    {
    y+=addy;
    x=startx-addx;
    do
     {
     x+=addx;
     if(fread(&bitmap[y*my+x*mx],1,1,f)!=1)
      { printmsg("Can't read uncompressed bitmap."); return; }
     }
    while(x!=endx);
    }
   while(y!=endy);
   break;
  case 8: case 9: case 0x18: case 0x19: case 0x0b: case 0x1b:
   if(fread(&size,sizeof(unsigned long int),(size_t)1,f)!=1)
    { printmsg("Can't read size of compressed bitmap."); return; }
   if((nobytesinrow=malloc((size_t)sd->ysize))==NULL)
    { printmsg("No mem for no of bytes in row."); return; }
   if((buffer=malloc((size_t)size-4-sd->ysize))==NULL)
    { printmsg("No mem for buffer for compressed bitmap."); return; }
   if(fread(nobytesinrow,1,(size_t)sd->ysize,f)!=sd->ysize)
    { printmsg("Can't read nobytesinrow."); return; } /* no need for this */
   if(fread(buffer,(int)size-4-sd->ysize,1,f)!=1)
    { printmsg("Can't read compressed bitmap."); return; }
   y=starty-addy;
   fill=buffer;
   do
    {
    y+=addy;
    x=startx-addx;
    do
     {
     if((*fill&0xe0)==0xe0)
      {
      for(i=0;i<(*fill&0x1f);i++)
       {
       if(x==endx)
        { printmsg("Error reading compressed bitmap."); 
	return; }
       x+=addx;
       bitmap[y*my+x*mx]=*(fill+1);
       }
      fill+=2;
      }
     else
      { x+=addx; bitmap[y*my+x*mx]=*(fill++); }
     }
    while(x!=endx);
    fill++; /*0xe0*/
    }
   while(y!=endy);
   free(buffer);
   free(nobytesinrow);
   break;
  default:
   printmsg("Bitmap-type %d not implemented.",(int)sd->type1);
  }
 for(fill=bitmap,sbitmap=view.txt_buffer;fill-bitmap<64*64;fill++,
  sbitmap++)
  if(*fill!=0xff) *sbitmap=*fill;
 }

void drawonetexture(int rtxtnum,int w)
 {
 struct texture *sd;
 char *txt;
 int plot;
 unsigned int txtnum=rtxtnum&0x3fff; /* the first two bits are direction */
 sd=(txtnum<view.numtxtref) ? view.txtref[txtnum] : NULL;
 if((plot=(sd!=NULL && (txtnum!=0 || w!=1)))==0)
  txt=(rtxtnum>0) ? "UNKNOWN " : "Nothing ";
 else
  {
  txt=sd->name;
  fseek(view.pigfile,(long)sd->offset+view.txt_bmoffset,SEEK_SET);
  readbitmap(view.pigfile,sd,(rtxtnum>>14)&0x3);
  }
 sys_drawtexture(plot,view.txt_xpos,view.txt_ypos,view.txt_xpos,
  view.txt_ypos + (w ? 85 : 70),txt); 
 }

void drawtextures(struct wall *w)
 {
 struct corner *c,*c0;
 int orig_x[9],orig_y[9],i;
 double l;
 changeto256();
 sys_filledbox(view.txt_xpos,view.txt_ypos,view.txt_xpos+64,
  view.txt_ypos+85+view.fontheight,view.color[BLACK],0);
 if(w==NULL) { drawonetexture(-1,0); drawonetexture(-1,1); return; }
 drawonetexture(w->texture1,0);
 if(w->texture2!=0) drawonetexture(w->texture2,1);
 else drawonetexture(-1,1);
 sys_initclipping(view.txt_xpos,view.txt_ypos,view.txt_xpos+63,
  view.txt_ypos+63);
 c0=&view.pcurrwall->corners[0];
 orig_x[0]=orig_x[1]=orig_x[2]=(c0->xpos*64/2048)%64;
 orig_x[3]=orig_x[4]=orig_x[5]=(c0->xpos*64/2048)%64+64;
 orig_x[6]=orig_x[7]=orig_x[8]=(c0->xpos*64/2048)%64-64;
 orig_y[0]=orig_y[3]=orig_y[6]=(c0->ypos*64/2048)%64;
 orig_y[1]=orig_y[4]=orig_y[7]=(c0->ypos*64/2048)%64+64;
 orig_y[2]=orig_y[5]=orig_y[8]=(c0->ypos*64/2048)%64-64;
 c=&view.pcurrwall->corners[1];
 l=sqrt((c->xpos-c0->xpos)*(c->xpos-c0->xpos)+(c->ypos-c0->ypos)*(c->ypos-
  c0->ypos))/32;
 for(i=0;i<9;i++)
  sys_clipline(view.txt_xpos+orig_x[i],view.txt_ypos+orig_y[i],
   (int)((c->xpos-c0->xpos)/l)+view.txt_xpos+orig_x[i],
   (int)((c->ypos-c0->ypos)/l)+view.txt_ypos+orig_y[i],
   supervga ? view.color[HILIGHTCOLORS+3] : view.color[WHITE],0);
 c=&view.pcurrwall->corners[3];
 l=sqrt((c->xpos-c0->xpos)*(c->xpos-c0->xpos)+(c->ypos-c0->ypos)*(c->ypos-
  c0->ypos))/32;
 for(i=0;i<9;i++)
  {
  sys_clipline(view.txt_xpos+orig_x[i],view.txt_ypos+orig_y[i],
   (int)((c->xpos-c0->xpos)/l)+view.txt_xpos+orig_x[i],
   (int)((c->ypos-c0->ypos)/l)+view.txt_ypos+orig_y[i],
   supervga ? view.color[HILIGHTCOLORS+4] : view.color[WHITE],0);
  sys_clipline(view.txt_xpos+orig_x[i]+3,view.txt_ypos+orig_y[i]+3,
   view.txt_xpos+orig_x[i]-3,view.txt_ypos+orig_y[i]-3,view.color[WHITE],0);
  sys_clipline(view.txt_xpos+orig_x[i]-3,view.txt_ypos+orig_y[i]+3,
   view.txt_xpos+orig_x[i]+3,view.txt_ypos+orig_y[i]-3,view.color[WHITE],0);
  }
 sys_killclipping();
 }
 
void beam(struct point *p)
 {
 int x;
 view.e0=*p;
 for(x=0;x<3;x++)
  view.e0.x[x]-=view.tsize*4*view.e[2].x[x]; 
 }
 
int testvector(struct cube *c,int p0,int p1,int p2,int p3,
 struct point *p)
 { 
 int i;
 struct point econtra,eco[3],h;
 for(i=0;i<3;i++)
  { 
  eco[0].x[i]=c->p[p1]->d.p->x[i]-c->p[p0]->d.p->x[i];
  eco[1].x[i]=c->p[p2]->d.p->x[i]-c->p[p0]->d.p->x[i];
  eco[2].x[i]=c->p[p3]->d.p->x[i]-c->p[p0]->d.p->x[i];
  h.x[i]=p->x[i]-c->p[p0]->d.p->x[i];
  }
 VECTOR(&econtra,&eco[2],&eco[1]);
 if(SCALAR(&econtra,&h)<0.0) return 0;
 VECTOR(&econtra,&eco[0],&eco[2]);
 if(SCALAR(&econtra,&h)<0.0) return 0;
 VECTOR(&econtra,&eco[1],&eco[0]);
 if(SCALAR(&econtra,&h)<0.0) return 0;
 return 1;
 }
 
/* find cube around position p. if there's no cube, return NULL */
struct node *findpntcube(struct list *cl,struct point *p)
 {
 struct node *n;
 /* this is certainly not the fastest method doing this */
 for(n=cl->head;n->next!=NULL;n=n->next)
  if(testvector(n->d.c,0,1,3,4,p) && testvector(n->d.c,6,2,5,7,p)) return n;
 return NULL;
 } 

void makemarker(struct point *mp,struct point *np)
 {
 int j;
 for(j=0;j<8;j++)
  { np[j].x[0]=mp->x[0]; np[j].x[1]=mp->x[1]; np[j].x[2]=mp->x[2]; }
 np[0].x[0]-=view.tsize;np[0].x[1]-=view.tsize;np[0].x[2]-=view.tsize;
 np[1].x[0]+=view.tsize;np[1].x[1]+=view.tsize;np[1].x[2]+=view.tsize;
 np[2].x[0]-=view.tsize;np[2].x[1]+=view.tsize;np[2].x[2]+=view.tsize;
 np[3].x[0]+=view.tsize;np[3].x[1]-=view.tsize;np[3].x[2]-=view.tsize;
 np[4].x[0]+=view.tsize;np[4].x[1]-=view.tsize;np[4].x[2]+=view.tsize;
 np[5].x[0]-=view.tsize;np[5].x[1]+=view.tsize;np[5].x[2]-=view.tsize;
 np[6].x[0]+=view.tsize;np[6].x[1]+=view.tsize;np[6].x[2]-=view.tsize;
 np[7].x[0]-=view.tsize;np[7].x[1]-=view.tsize;np[7].x[2]+=view.tsize;
 }
 
void setthingpts(struct thing *t)
 {
 struct point p;
 int j;
 if(t->type1==3 || t->type1==7)
  {
  for(j=8;j<11;j++)
   { t->p[j].x[0]=t->p[0].x[0]; t->p[j].x[1]=t->p[0].x[1]; 
     t->p[j].x[2]=t->p[0].x[2]; }
  makemarker(&t->p[0],&t->p[1]);
  }
 else
  {
  for(j=1;j<11;j++)
   t->p[j]=t->p[0];
  for(j=0;j<3;j++)
   p.x[j]=t->orientation[j+6];
  for(j=0;j<3;j++)
   {
   t->p[2].x[j]+=p.x[j]/LENGTH(&p)*view.tsize*3;
   t->p[3].x[j]+=p.x[j]/LENGTH(&p)*view.tsize*3;
   t->p[9].x[j]+=p.x[j]/LENGTH(&p)*view.tsize*3;
   }
  for(j=0;j<3;j++)
   p.x[j]=t->orientation[j+3];
  for(j=0;j<3;j++)
   {
   t->p[6].x[j]+=p.x[j]/LENGTH(&p)*view.tsize;
   t->p[7].x[j]+=p.x[j]/LENGTH(&p)*view.tsize;
   t->p[10].x[j]+=p.x[j]/LENGTH(&p)*view.tsize;
   }
  for(j=0;j<3;j++)
   p.x[j]=t->orientation[j];
  for(j=0;j<3;j++)
   {
   t->p[1].x[j]-=p.x[j]/LENGTH(&p)*view.tsize;
   t->p[8].x[j]-=p.x[j]/LENGTH(&p)*view.tsize;
   t->p[4].x[j]+=p.x[j]/LENGTH(&p)*view.tsize;
   t->p[5].x[j]+=p.x[j]/LENGTH(&p)*view.tsize;
   }
  }
 }
  
void printmsg(char *txt,...)
 {
 va_list args;
 va_start(args,txt);
 vprintmsg(txt,args);
 va_end(args);
 }
 
void vprintmsg(char *txt,va_list args)
 {
 int addy=3;
 char buffer[255];
 vsprintf(buffer,txt,args); 
 buffer[view.xsize/view.fontwidth]=0;
 if(buffer[0]=='*') addy+=view.fontheight+1; 
 sys_filledbox(view.xoffset,view.yoffset+view.ysize+addy,
  view.xoffset+view.xsize,view.maxysize-1,view.color[BLACK],0);
 sys_text(view.xoffset,view.yoffset+view.ysize+addy,
  buffer[0]=='*' ? &buffer[1] : buffer,view.color[WHITE],view.color[BLACK]);
 }

/* makes the thing nearest to the normal screen coords x,y the current th.*/
int findthing(int sx,int sy)
 {
 int k;
 double x,y,min=view.maxvisibility;
 struct thing *t;
 struct node *n,*min_t;
 struct point v,d; /* v is the direction of the beam */
 x=(double)(sx-view.xoffset)/(view.xsize-1)-0.5;
 y=(double)(view.ysize-(sy-view.yoffset))/(view.ysize-1)-0.5;
 /* now create the beam */
 for(k=0;k<3;k++)
  v.x[k]=view.e0.x[k]+x*view.e[0].x[k]+y*view.e[1].x[k]-
   view.x0.x[k];
 y=LENGTH(&v);
 /* now test all things if they're in the magical beam */
 for(n=l->things.head,min_t=NULL;n->next!=NULL;n=n->next)
  {
  t=n->d.t;
  for(k=0;k<3;k++)
   { d.x[k]=t->p[0].x[k]-view.x0.x[k]; }
  x=LENGTH(&d);
  /* clickphi is already cosin, so it must be a greater value
   for smaller angle */
  if(x<=min && view.clickphi*x*y<SCALAR(&d,&v))
   { min_t=n; min=x; }
  }
 if(min_t!=NULL)
  {
  view.oldpcurrthing=view.pcurrthing; 
  view.pcurrthing=min_t;
  return 1;
  }
 else
  return 0;
 } 
 
/* makes the door nearest to the normal screen coords x,y the current door*/
int finddoor(int sx,int sy)
 {
 int k;
 double x,y,min=view.maxvisibility;
 struct door *door;
 struct node *n,*min_d;
 struct point v,d; /* v is the direction of the beam */
 x=(double)(sx-view.xoffset)/(view.xsize-1)-0.5;
 y=(double)(view.ysize-(sy-view.yoffset))/(view.ysize-1)-0.5;
 /* now create the beam */
 for(k=0;k<3;k++)
  v.x[k]=view.e0.x[k]+x*view.e[0].x[k]+y*view.e[1].x[k]-
   view.x0.x[k];
 y=LENGTH(&v);
 /* now test all things if they're in the magical beam */
 for(n=l->doors.head,min_d=NULL;n->next!=NULL;n=n->next)
  {
  door=n->d.d;
  for(k=0;k<3;k++)
   { d.x[k]=door->p.x[k]-view.x0.x[k]; }
  x=LENGTH(&d);
  /* clickphi is already cosin, so it must be a greater value
   for smaller angle */
  if(x<=min && view.clickphi*x*y<SCALAR(&d,&v))
   { min_d=n; min=x; }
  }
 if(min_d!=NULL)
  {
  view.oldpcurrdoor=view.pcurrdoor; 
  view.pcurrdoor=min_d;
  return 1;
  }
 else
  return 0;
 } 
 
/* makes the cube nearest to the normal screen coords x,y the current cube */
int findcube(int sx,int sy)
 {
 int k,m,o,min_wall=0,min_pno=0;
 double x,y,min,minwallx;
 struct point v,d; /* v is the direction of the beam */
 struct point *p;
 struct node *n,*min_p,*min_c=NULL;
 struct cube *c;
 x=(double)(sx-view.xoffset)/(view.xsize-1)-0.5;
 y=(double)(view.ysize-(sy-view.yoffset))/(view.ysize-1)-0.5;
 /* now create the beam */
 for(k=0;k<3;k++)
  v.x[k]=view.e0.x[k]+x*view.e[0].x[k]+y*view.e[1].x[k]-view.x0.x[k];
 y=LENGTH(&v);
 /* now test all points if they're in the magical beam */
 for(n=l->pts.head,min_p=NULL,min=view.maxvisibility*2;n->next!=NULL;
  n=n->next)
  {
  p=n->d.p;
  for(k=0;k<3;k++)
   { d.x[k]=p->x[k]-view.x0.x[k]; }
  x=LENGTH(&d);
  /* clickphi is already cosin, so it must be a greater value
   for smaller angle */
  if(x<=min && view.clickphi*x*y<SCALAR(&d,&v))
   { min_p=n; min=x; }
  }
 if(min_p!=NULL)
  {
  /* now search the nearest cube with this point */
  for(n=min_p->d.lp->c.head,min=view.maxvisibility*2;n->next!=NULL;
   n=n->next)
   {
   c=n->d.n->d.c;
   for(k=0,x=0;k<8;k++)
    {
    for(m=0;m<3;m++)
     { d.x[m]=c->p[k]->d.p->x[m]-view.x0.x[m]; }
    x+=LENGTH(&d)/8.0;
    }
   if(x<=min) 
    {
    min=x; min_c=n->d.n; min_pno=n->no;
    for(k=0,minwallx=view.maxvisibility*2,min_wall=0;k<3;k++)
     {
     for(o=0,x=0;o<4;o++)
      {
      for(m=0;m<3;m++)
       d.x[m]=c->p[wallpts[wallno[min_pno][0][k]][o]]->d.p->x[m]-view.x0.x[m];
      x+=LENGTH(&d)/4;
      }
     if(x<=minwallx) { min_wall=k; minwallx=x; }
     }
    }
   }
  }
 if(min_c!=NULL)
  {
  view.oldpcurrcube=view.pcurrcube; 
  view.oldpcurrpnt=
   view.oldpcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
  view.pcurrcube=min_c;
  view.currwall=wallno[min_pno][0][min_wall];
  view.currpnt=wallno[min_pno][1][min_wall];
  view.pcurrwall=min_c->d.c->walls[view.currwall];
  return 1;
  }
 else
  return 0;
 } 
 
/* turns the coordsystem es to ee.
   i,j give the axis to turn, k is the axis to turn around, i,j,k are
   0,1,2; 1,2,0 or 2,0,1 */
void turn(struct point *es,struct point *ee,int i,int j,int k,double angel)
 {
 struct point ne1,ne2;
 int l;
 for(l=0;l<3;l++)
  {
  ne1.x[l]=cos(angel)*es[i].x[l]+sin(angel)*es[j].x[l];
  ne2.x[l]=-sin(angel)*es[i].x[l]+cos(angel)*es[j].x[l];
  }
 ee[i]=ne1; ee[j]=ne2;
 VECTOR(&ee[k],&ee[i],&ee[j]);
 normalize(&ee[j]);
 normalize(&ee[i]);
 normalize(&ee[k]);
 }
 
/* must be a function because of l (changes in the calculation) 
 prototype in structs.h */
void normalize(struct point *p)
 {
 double l=LENGTH(p);
 p->x[0]/=l; p->x[1]/=l; p->x[2]/=l;
 }
 
struct objtype *findotype(struct objdata *od,int n)
 {
 struct objtype **ot;
 for(ot=od->data;ot-od->data<od->size;ot++)
  if((*ot)->no==n) break;
 if(ot-od->data>=od->size)
  return NULL;
 else
  return *ot;
 }
 
/* gives data for infoitem it. only uses pcurrcube, pcurrdoor, currwall,
 currpnt and  pcurrthing (because of changevalue) */
unsigned char *getdata(enum datastructs it)
 {
 void *data=NULL;
 if(!view.pcurrcube) return NULL;
 switch(it)
  {
  case ds_cube: data=view.pcurrcube->d.c; break;
  case ds_wall: data=view.pcurrcube->d.c->walls[view.currwall]; break;
  case ds_door: if(view.pcurrdoor!=NULL) data=view.pcurrdoor->d.d;
   break;
  case ds_corner: 
   if(view.pcurrcube->d.c->walls[view.currwall]!=NULL) 
    data=&view.pcurrcube->d.c->walls[view.currwall]->corners[view.currpnt];
   break;
  case ds_thing: data=&view.pcurrthing->d.t->type1; break;
  case ds_internal: data=&view; break;
  case ds_leveldata: data=l; break;
  case ds_sdoor:
   if(view.pcurrdoor!=NULL && view.pcurrdoor->d.d->sd!=NULL)
    data=view.pcurrdoor->d.d->sd->d.sd;
   break;
  case ds_producer:
   if(view.pcurrcube->d.c->cp) data=view.pcurrcube->d.c->cp->d.cp; 
   break;
  case ds_point: 
   data=view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
   break;
  default:
   fprintf(errf,"getdata: value of info not known: %d\n",it); my_exit(); 
  }
 return (unsigned char *)data;
 }
 
size_t getsize(enum datastructs it,struct thing *t)
 {
 size_t size=0;
 switch(it)
  {
  case ds_producer: size=16; break;
  case ds_sdoor: size=54; break;
  case ds_leveldata: size=sizeof(struct leveldata); break;
  case ds_internal: size=sizeof(struct viewdata); break;
  case ds_cube: size=40; break;
  case ds_wall: size=28; break;
  case ds_door: size=24; break;
  case ds_corner: size=6; break;
  case ds_thing: 
   switch(t->type1)
    {
    case 9: size=sizeof(struct reactor); break;
    case 3: case 7: size=sizeof(struct item); break;
    case 4: case 0xe: size=sizeof(struct start); break;
    case 2: size=sizeof(struct robot); break;
    default: fprintf(errf,"getsize: Unknown thing-type: %.2x\n",
     (unsigned)t->type1); my_exit();
    }
   size-=(unsigned char *)&t->type1-(unsigned char *)t;
   break;
  default:
   fprintf(errf,"getsize: value of info not known: %d\n",it); my_exit(); 
  }
 return size;
 }

void setno(struct infoitem *i,unsigned long no)
 {
 int k;
 unsigned char *data;
 if((data=getdata(i->infonr))==NULL || i->length>4 || i->offset<0) return;
 for(k=0;k<i->length;k++)
  *(data+i->offset+k)=(no>>(k*8))&0xfful;
 }
 
int getno(struct infoitem *i,unsigned long *no)
 {
 unsigned char *data;
 unsigned long *d;
 if((data=getdata(i->infonr))==NULL) return 0;
 if(i->offset<0) return 0;
 d=(unsigned long *)(data+i->offset);
 if(i->length>4)
  { printmsg("Illegal length for %s in getno %d\n",i->txt,i->length); 
    return 0; }
 *no=*d&(0xfffffffful>>((4-i->length)*8));
 return 1;
 }
 
enum sdoortypes getsdoortype(struct sdoor *sd)
 {
 if(sd->flags==0x0001) return sdtype_door;
 else return sdtype_cube;
 }
 
/* list functions */
void initlist(struct list *l)
 {
 l->dummy=NULL;
 l->head=(struct node *)&l->dummy;
 l->tail=(struct node *)&l->head;
 l->size=0;
 }
 
void freecube(void *n)
 {
 struct wall **w;
 struct cube *c=n;
 for(w=c->walls;w-c->walls<6;w++)
  free(*w);
 freelist(&c->sdoors,NULL);
 free(c);
 }

void freedoor(void *n)
 {
 struct door *d=n;
 freelist(&d->sdoors,NULL);
 free(d);
 }

void cleanmacro(void *n)
 {
 struct macro *m=n;
 freelist(&m->pts,freelistpnt); 
 freelist(&m->doors,freedoor); 
 freelist(&m->things,free); 
 freelist(&m->cubes,freecube);
 freelist(&m->sdoors,free); 
 freelist(&m->producers,free);
 free(m->longtxt);  m->longtxt=NULL;
 }
 
void freemacro(void *n)
 {
 struct macro *m=n;
 cleanmacro(m);
 free(m->shorttxt); free(m->filename); 
 free(m);
 }
 
void freelistpnt(void *n)
 {
 struct listpoint *lp=n;
 freelist(&lp->c,NULL);
 free(lp);
 }
 
void freelist(struct list *l,void (*freeentry)(void *))
 {
 struct node *n;
 for(n=l->head;n->next!=NULL;n=n->next)
  freenode(l,n,freeentry);
 l->size=0;
 initlist(l);
 }
 
struct node *insertnode(struct list *l,struct node *nprev,int no,void *data)
 {
 struct node *n;
 if((n=malloc(sizeof(struct node)))==NULL)
  { fprintf(errf,"No mem for node.\n"); return NULL; }
 n->d.v=data;
 n->next=nprev->next;
 n->prev=nprev;
 n->no=no;
 nprev->next->prev=n;
 nprev->next=n;
 l->size++;
 return n;
 }

struct node *addnode(struct list *l,int no,void *data)
 { return insertnode(l,l->tail,no,data); }
 
struct node *addheadnode(struct list *l,int no,void *data)
 { return insertnode(l,(struct node *)&l->head,no,data); }
 
void freenode(struct list *l,struct node *n,void (*freeentry)(void *))
 {
 unlistnode(l,n);
 if(freeentry!=NULL)
  freeentry(n->d.v);
 free(n);
 }

void unlistnode(struct list *l,struct node *n)
 {
 n->prev->next=n->next;
 n->next->prev=n->prev;
 l->size--;
 }
 
struct node *findnode(struct list *l,int no)
 {
 struct node *n;
 for(n=l->head;n->next!=NULL;n=n->next)
  if(n->no==no) { return n; }
 return NULL;
 }
 
void sortlist(struct list *l,int start)
 {
 struct node *n;
 int i;
 for(n=l->head,i=0;n->next!=NULL;n=n->next,i++)
  n->no=start+i;
 }
 
int copylist(struct list *dl,struct list *sl,size_t s)
 {
 struct node *sn;
 void *data;
 for(sn=sl->tail;sn->prev!=NULL;sn=sn->prev)
  {
  if((data=malloc(s))==NULL) return 0;
  memcpy(data,sn->d.v,s);
  addheadnode(dl,sn->no,data);
  }
 return 1;
 }

