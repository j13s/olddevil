/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    grfx.c - init graphics, drawing menus.
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
#include "plot.h"
#include "do_event.h" /* for do_bmap.h */
#include "do_bmap.h"
#include "grfx.h"

void printline(int lno,char *fstr,...);
void plotgrid(void);
char *structname[ds_number]={ "cube","side","corner","thing","door",
 "internal","level","switch","producer","point" };
int supervga=1,oldwhite,grfxmode=0,oldxpos,oldypos;
unsigned char *palette;

void changeto256(void)
 {
 if(supervga || grfxmode) return;
 oldwhite=view.color[WHITE];
 view.color[WHITE]=255;
 oldxpos=view.txt_xpos; oldypos=view.txt_ypos;
 view.txt_xpos=10; view.txt_ypos=10;
 sys_changeto256();
 setcolors(1);
 view.menuheight=view.smenuheight-3;
 grfxmode=1;
 }

void changeto16(void)
 {
 if(supervga || !grfxmode) return;
 sys_changeto16();
 view.color[WHITE]=oldwhite;
 setcolors(0);
 drawallbuttons();
 view.menuheight=view.smenuheight-3;
 view.txt_xpos=oldxpos; view.txt_ypos=oldypos;
 grfxmode=0;
 }
 
int rebuild_screen(int plot_what)
 {
 struct node *n;
 if((plot_what&0x2)!=0)
  {
  if((view.drawwhat&DW_BITMAP)!=0)
   { if(view.pcurrwall) plotbitmap(view.pcurrwall); }
  else
   {
   changeto16();
   makeview();
   if((view.drawwhat&DW_CUBES)!=0)
    for(n=l->cubes.head;n->next!=NULL;n=n->next)
     plotcube(n,0);
   if((view.drawwhat&DW_THINGS)!=0)
    for(n=l->things.head;n->next!=NULL;n=n->next)
     plotthing(n,0);
   if((view.drawwhat&DW_DOORS)!=0)
    for(n=l->doors.head;n->next!=NULL;n=n->next)
     plotdoor(n,0);   
   if((view.drawwhat&DW_GRID)!=0) plotgrid();
   }
  }
 if((view.drawwhat&DW_BITMAP)==0)
  {
  if((plot_what&0x4)!=0) drawmenu(); 
  if((plot_what&0x6)!=0) plotcurrent(); 
  }
 drawnumlock();
 return plot_what;
 }
 
void printline(int lno,char *fstr,...)
 {
 va_list args;
 char buffer[100];
 if(lno>=view.smenuheight) return;
 va_start(args,fstr);
 vsprintf(buffer,fstr,args);
 buffer[view.smenuwidth]=0; /* so the string fits in the line */
 strcpy(&view.menubuffer[lno*view.smenuwidth],buffer);
 va_end(args);
 }
 
void printinfoitem(struct infoitem *i)
 {
 struct objtype *ot;
 char *buffer,dtext[strlen(i->txt)+6];
 int k,l,row;
 unsigned long no=0;
 double dno=0.0;
 unsigned char *data;
 unsigned int **si;
 row=i->row-view.menuoffset;
 if(row>=view.menuheight-1) return;
 if(i->offset<0 && i->od!=NULL) 
  {
  sprintf(&view.menubuffer[view.smenuwidth*(row+1)
   +i->column+1],"%.*s",view.smenuwidth,i->txt);
  return;
  }
 if((data=getdata(i->infonr))==NULL)
  { sprintf(&view.menubuffer[(i->row+1)*view.smenuwidth+i->column],
     " No %s data",structname[i->infonr]); return; }
 switch(i->length)
  {
  case 0: /* side effect field */
   for(si=i->sideeffs,dno=0,k=0;si-i->sideeffs<i->numsideeffs;si++)
    if(getsideno(*si,&no)) { dno+=no; k++; }
    if(k>0) no=dno/k;
   break;
  case 1: case 2: case 4:
   if(!getno(i,&no)) return;
   break;
  case 8:
   dno=*(double *)(data+i->offset);
   if(i->od!=NULL)
    { fprintf(errf,"double width objtype (%s) is not possible.\n",i->txt);
      my_exit(); }
   break;
  default: fprintf(errf,"Unknown length %d (%s) in printinfoitem.\n",
   i->length,i->txt); my_exit();
  }
 if(row>=0)
  if(i->od==NULL)
   {
   if(i->length<=4)
    {
    if(i->txt[strlen(i->txt)-1]=='L') /* special type for light */
     { 
     strcpy(dtext,i->txt);
     dno=no/327.67;
     strcpy(&dtext[strlen(i->txt)-1],"5.2f%%");
     sprintf(&view.menubuffer[view.smenuwidth*(row+1)+i->column+1],
      dtext,dno);
     }
    else
     sprintf(&view.menubuffer[view.smenuwidth*(row+1)+i->column+1],
      i->txt,no);
    }
   else
    {
    strcpy(dtext,i->txt);
    switch(dtext[strlen(dtext)-1])
     {
     case 'C': /* special type for coords */
      dno/=65536; dtext[strlen(dtext)-1]='g'; break;
     case 'D': /* special type for degrees */
      dno*=180/M_PI; dtext[strlen(dtext)-1]='g'; break;
     }
    sprintf(&view.menubuffer[view.smenuwidth*(row+1)+i->column+1],
     dtext,dno);
    }
   }
  else
   {
   ot=findotype(i->od,no);
   if(ot!=NULL) 
    buffer=ot->str;
   else
    {
    buffer="            "; /* not the cleanest, but the shortest idea */
    sprintf(buffer,"? (%.8lx)",no);
    }
   sprintf(&view.menubuffer[view.smenuwidth*(row+1)
    +i->column+1],"%.*s",view.smenuwidth,buffer);
   }
 for(k=0;k<i->numchildren;k++)
  if(i->itemchildren[k]==no)
   for(l=0;l<i->numechildren[k];l++)
    printinfoitem(&i->children[k][l]);
 }
  
void printmenu(struct infoitem *is,int menunum)
 {
 struct infoitem *i;
 size_t size;
 int row,column,offset;
 unsigned char *data;
 for(i=is;i-is<menunum;i++)
  printinfoitem(i);
 if(view.curmenuitem!=NULL && 
  view.curmenuitem->row-view.menuoffset<view.smenuheight-1)
  sys_dumpmenuchar('*',view.curmenuitem->column,
   view.curmenuitem->row-view.menuoffset+1);
 if((view.drawwhat&DW_DATA)==0) return;
 row=view.maxrow[view.showwhat]+3-view.menuoffset;
 if(row>=view.menuheight) return;
 if((data=getdata(view.showwhat))==NULL)
  return;
 size=getsize(view.showwhat,view.pcurrthing->d.t);
 if(row<=0)
  { offset = -4*row; row=0; }
 else 
  {
  offset=0;
  sprintf(&view.menubuffer[row*view.smenuwidth],
   "Data %.4lx",(long)size); 
  }
 column=0;
 while(offset<size && row<view.menuheight-1)
  {
  if(column==0)
   sprintf(&view.menubuffer[(row+1)*view.smenuwidth],"%.4x",
    offset);  
  printdatabyte(row,column,data,offset++);
  column+=3;
  if(column/3==4) 
   { row+=1; column=0; }
  }
 }
 
void markknown(struct infoitem *is,int allnum,int minoffset,int maxoffset)
 {
 struct infoitem *i;
 int k;
 unsigned long no;
 void *data;
 for(i=is;i-is<allnum;i++)
  {
  if(i->infonr!=view.showwhat) continue;
  if((data=getdata(i->infonr))==NULL) continue;
  for(k=0;k<i->length;k++)
   if(i->offset+k>=minoffset && i->offset+k<=maxoffset)
    markdatabyte(i,k);
  if(i->numchildren>0)
   if(getno(i,&no))
    for(k=0;k<i->numchildren;k++)
     if(i->itemchildren[k]==no)
      markknown(i->children[k],i->numdchildren[k],minoffset,maxoffset);
  }
 }
 
void markknownbytes(struct infoitem *is,int allnum)
 {
 int minoffset,maxoffset,row;
 size_t size;
 if(getdata(view.showwhat)==NULL)
  return;
 size=getsize(view.showwhat,view.pcurrthing->d.t);
 row=view.maxrow[view.showwhat]+3-view.menuoffset;
 if(row>=view.menuheight-1) return;
 minoffset = (row<0) ? -4*row : 0; 
 maxoffset = minoffset + (view.menuheight-1-((row<0) ? 0 : row))*4 -1;
 if(maxoffset>=size) maxoffset=size-1;
 if(maxoffset<=0 || minoffset>=size) return;
 markknown(is,allnum,minoffset,maxoffset);
 }
 
void printmenuline(void)
 {
 int n1,x;
 sys_drawbutton(view.bounds[ecw_menuline][0],view.bounds[ecw_menuline][1],
  view.bounds[ecw_menuline][2],view.bounds[ecw_menuline][3],NULL);
 for(n1=0,x=1;n1<view.nummenus;n1++)
  { 
  sys_menutext(x,2,view.menuline[n1].str,view.menuline[n1].no&0xfffful);
  x+=strlen(view.menuline[n1].str)*view.fontwidth;
  }
 }
 
void drawmenu(void)
 {
 struct wall *w;
 unsigned int t;
 struct texture *sd;
 char *txt;
 memset(view.menubuffer,(int)' ',view.smenuwidth*
  (view.smenuheight-1));
 if(view.showwhat<in_number)
  printmenu(view.info[view.showwhat],view.infonum[view.showwhat]);
 else
  { fprintf(errf,"value of showwhat not known: %d\n",view.showwhat); 
    my_exit(); }
 printline(0,"DEVIL:%8s/%1d",view.bnames[view.showwhat],view.currmode);
 printpos();
 printline(view.smenuheight-2,"Cube: %.4d/%.1d/%.1d/%.1d/%s",
  (view.pcurrcube==NULL) ? -1 : view.pcurrcube->no,view.currwall,
  view.currpnt,wallpts[view.currwall][view.currpnt],
  (view.pcurrcube==NULL) ? "---" :
  (view.currmode==tt_pnt) ? 
  findnode(&view.tagged[tt_pnt],
   view.pcurrcube->no*24+view.currwall*4+view.currpnt)==NULL ? "No " : "Yes" :
  (view.currmode==tt_wall) ?
  findnode(&view.tagged[tt_wall],view.pcurrcube->no*6+view.currwall)==NULL ?
   "No " : "Yes" :
  findnode(&view.tagged[tt_cube],view.pcurrcube->no)==NULL ? "No " : "Yes");
 printline(view.smenuheight-1,"T:%.4d/%s W:%.3d/%s",
  (view.pcurrthing==NULL) ? -1 : view.pcurrthing->no,
  (view.pcurrthing==NULL) ? "---" :
  findnode(&view.tagged[tt_thing],view.pcurrthing->no)==NULL ? "No " : "Yes",
  (view.pcurrdoor==NULL) ? -1 : view.pcurrdoor->no,
  (view.pcurrdoor==NULL) ? "---" :
  findnode(&view.tagged[tt_door],view.pcurrdoor->no)==NULL ? "No " : "Yes");
 sys_dumpmenu();
 if(view.showwhat<in_number && (view.drawwhat&DW_DATA)!=0)
  markknownbytes(view.info[view.showwhat],view.descripnum[view.showwhat]);
 if(view.currmode==tt_door)
  w=(view.pcurrdoor!=NULL) ? view.pcurrdoor->d.d->w : NULL;
 else
  w=view.pcurrwall;
 if(supervga) drawtextures(w);
 else
  {
  t=(w==NULL) ? view.numtxtref : w->texture1;
  sd=(t<view.numtxtref) ? view.txtref[t] : NULL;
  txt=(sd==NULL) ? "UNKNOWN " : sd->name;
  sys_text(view.txt_xpos,view.txt_ypos,txt,view.color[WHITE],
   view.color[BLACK]);
  t=(w==NULL) ? view.numtxtref : w->texture2&0x3fff;
  sd=(t<view.numtxtref && t!=0) ? view.txtref[t] : NULL;
  txt=(sd==NULL) ? (t!=0 ? "UNKNOWN " : "Nothing ") : sd->name;
  sys_text(view.txt_xpos+9*view.fontwidth,view.txt_ypos,txt,view.color[WHITE],
   view.color[BLACK]);
  }
 printmenuline();
 }

void closegrph(void)
 { sys_clearall(); }

void makecolor(int i,int r,int g,int b)
 {
 int n,dist,mdist=255+255+255+1,rn,bn,gn;
 for(n=0;n<(supervga ? 256 : 16);n++)
  {
  sys_getcolor(n,&rn,&gn,&bn);
  dist=abs(r-rn)+abs(g-gn)+abs(b-bn);
  if(dist<mdist)
   { 
   view.color[i]=n; 
   if((mdist=dist)==0) break;
   }
  }
 }

void setcolors(int color256)
 {
 int i;
 if(color256)
  {
  for(i=1;i<256;i++)
   sys_setcolor(i,
    palette[i*3]+(view.brightness*palette[i*3])/(255-view.brightness),
    palette[i*3+1]+(view.brightness*palette[i*3+1])/(255-view.brightness),
    palette[i*3+2]+(view.brightness*palette[i*3+2])/(255-view.brightness));
  sys_setcolor(0,0,0,0);
  sys_setcolor(255,255,255,255);
  }
 else
  {
  sys_setcolor(0,0,0,0);
  sys_setcolor(1,255,0,0);
  sys_setcolor(2,0,255,0);
  sys_setcolor(3,0,0,255);
  sys_setcolor(4,255,255,0);
  sys_setcolor(5,255,0,255);
  sys_setcolor(6,255,100,255);
  sys_setcolor(7,255,100,100);
  for(i=0;i<7;i++)
   sys_setcolor(i+8,75+(i*125)/6,75+(i*125)/6,75+(i*125)/6);
  sys_setcolor(15,255,255,255);
  }
 }

void drawmenubutton(int no,int xoffs,int yoffs,int bxsize,int bysize)
 {
 int x,y;
 if(no>=MAX_XNOBUTTON*MAX_YNOBUTTON) 
  { printf("drawmenubutton: no %d too large: %d\n",no,MAX_XNOBUTTON*
    MAX_YNOBUTTON); my_exit(); }
 x=(no % MAX_XNOBUTTON)*bxsize+xoffs; y=(no / MAX_XNOBUTTON)*bysize+yoffs;
 sys_drawbutton(x,y,x+bxsize,y+bysize,view.bnames[no]);
 }
 
void drawallbuttons()
 {
 int i,j,bxoffs,byoffs,bxsize,bysize;
 char *movebuttontext[2][3]={ { "<<","/\\",">>" }, { "<-","\\/","->" } };
 /* menu button menu */
 bxoffs=view.bounds[ecw_lowermenu][0];
 byoffs=view.bounds[ecw_lowermenu][1];
 view.bxsize=bxsize=(view.bounds[ecw_lowermenu][2]-bxoffs)/MAX_XNOBUTTON;
 view.bysize=bysize=(view.bounds[ecw_lowermenu][3]-byoffs)/MAX_YNOBUTTON;
 for(i=0;i<view.numbuttons;i++) drawmenubutton(i,bxoffs,byoffs,bxsize,bysize);
 /* move button menu */
 bxoffs=view.bounds[ecw_movebuttons][0];
 byoffs=view.bounds[ecw_movebuttons][1];
 bxsize=(view.bounds[ecw_movebuttons][2] - bxoffs)/3;
 bysize=(view.bounds[ecw_movebuttons][3] - byoffs)/3;
 for(j=0;j<2;j++)
  for(i=0;i<3;i++)
   sys_drawbutton(bxoffs+i*bxsize,byoffs+j*bysize,bxoffs+(i+1)*bxsize,
    byoffs+(j+1)*bysize,movebuttontext[j][i]);
 drawnumlock();
 /* line above printmsg */
 sys_line(view.xoffset,view.yoffset+view.ysize+2,view.xoffset+view.xsize,
  view.yoffset+view.ysize+2,view.color[WHITE],0);
 }
 
void initgrph(void)
 {
 int i;
 FILE *f;
 if((supervga=sys_initgrfx(supervga))==-1) my_exit();
 if(supervga==0 && SYS_VGA_SUPPORTED==0) 
  { fprintf(errf,"VGA is not supported by this DEVIL-version.\n(%s)",
     SYS_COMPILER_NAME); my_exit(); }
 /* setting boundaries, first part */ 
 view.bounds[ecw_screen][0]=0;
 view.bounds[ecw_screen][1]=0;
 view.bounds[ecw_screen][2]=view.maxxsize; 
 view.bounds[ecw_screen][3]=view.maxysize;
 view.bounds[ecw_plot][0]=10;
 view.bounds[ecw_plot][1]=view.fontheight+7; 
 view.bounds[ecw_plot][2]=10+view.maxysize-2*view.fontheight-13; 
 view.bounds[ecw_plot][3]=10+view.maxysize-2*view.fontheight-13; 
 view.xoffset=view.bounds[ecw_plot][0];
 view.yoffset=view.bounds[ecw_plot][1];
 view.xsize=view.bounds[ecw_plot][2]-view.xoffset;
 view.ysize=view.bounds[ecw_plot][3]-view.yoffset;
 if(!supervga)
  {
  view.txt_xpos=view.bounds[ecw_plot][2]+5;
  view.txt_ypos=view.bounds[ecw_plot][3];
  if(!sys_changeto256()) 
   { fprintf(errf,"Can't set 320*200*256\n"); my_exit(); }
  /* Pos.&size of fit bitmap */
  view.fit_maxxnum=5; view.fit_maxynum=3;
  view.fit_xpos=view.fit_maxxnum/2*64;
  view.fit_ypos=view.fit_maxynum/2*64;
  }
 else
  {
  /* Texture coordinates */
  view.txt_xpos=view.maxxsize-73;
  view.txt_ypos=view.maxysize-105;
  /* Pos.&size of fit bitmap */
  view.fit_maxxnum=view.xsize/64;
  view.fit_maxynum=view.ysize/64;
  view.fit_xpos=view.xoffset+view.fit_maxxnum/2*64;
  view.fit_ypos=view.yoffset+view.fit_maxxnum/2*64;
  }
 if((view.txt_buffer=malloc(64*64))==NULL)
  { fprintf(errf,"No mem for texture buffer.\n"); my_exit(); }
 if((view.txt_sbuffer=malloc(64*64))==NULL)
  { fprintf(errf,"No mem for complete texture buffer.\n"); my_exit(); }
 if(!sys_inittxtspace()) my_exit();
 if(!supervga)
  if(!sys_changeto16())
   { fprintf(errf,"Can't switch to 16 colors.\n"); my_exit(); }
 if((f=fopen(view.palettename,"rb"))==NULL)
  { fprintf(errf,"Can't open palette file: \"%s\"\n",view.palettename); 
    my_exit(); }
 if((palette=malloc(256*3))==NULL)
  { fprintf(errf,"No mem for colors\n"); my_exit(); }
 if(fread(palette,(size_t)1,(size_t)256*3,f)!=256*3)
  { fprintf(errf,"Can't read colors.\n"); my_exit(); }
 fclose(f);
 setcolors(supervga);
 for(i=0;i<USEDCOLORS;i++) view.color[i]=i;
 makecolor(BLACK,0,0,0); 
 for(i=1;i<=GRAYSCALE;i++) 
  makecolor(i,(125*i)/GRAYSCALE+75,
   (125*i)/GRAYSCALE+75,(125*i)/GRAYSCALE+75); /* walls */
 makecolor(THINGCOLORS,255,0,0); /* things type robot */
 makecolor(1+THINGCOLORS,255,255,255);/* things type start */
 makecolor(2+THINGCOLORS,0,0,255); /* things type item */
 makecolor(3+THINGCOLORS,255,0,255);/* things type reactor */
 makecolor(HILIGHTCOLORS,0,255,0);/* hilighted cube/thing */
 makecolor(HILIGHTCOLORS+1,255,0,0);  /* hilighted wall; cube for sdoor */
 makecolor(HILIGHTCOLORS+2,0,0,255); /* tagged cube/thing */
 makecolor(HILIGHTCOLORS+3,255,100,255); /* hilight for sdoor for doors,
                                            wall line 1 */
 makecolor(HILIGHTCOLORS+4,255,255,0); /* wall line 2 */
 makecolor(DOORCOLORS,255,100,100); /* blow door */
 makecolor(DOORCOLORS+1,255,255,255); /* no key door */
 makecolor(DOORCOLORS+2,0,0,255); /* blue key */
 makecolor(DOORCOLORS+3,255,0,0); /* red key */
 makecolor(DOORCOLORS+4,255,255,0); /* yellow key */
 makecolor(MENUCOLORS,150,150,150); /* GrFBoxColors for menu (int) */
 makecolor(MENUCOLORS+1,100,100,100); /* top */
 makecolor(MENUCOLORS+2,200,200,200); /* right */
 makecolor(MENUCOLORS+3,200,200,200); /* bottom */
 makecolor(MENUCOLORS+4,100,100,100); /* left */
 makecolor(MENUCOLORS+5,255,255,255); /* textcolor of menu */
 makecolor(WHITE,255,255,255); /* normal white */
 sys_initmouse();
 /* setting boundaries, second part */ 
 view.bounds[ecw_uppermenu][0]=view.xsize+2*view.xoffset; 
 view.bounds[ecw_uppermenu][1]=0;
 view.bounds[ecw_uppermenu][2]=view.maxxsize;
 view.bounds[ecw_uppermenu][3]=view.maxysize*3/5;
 view.bounds[ecw_lowermenu][0]=view.bounds[ecw_uppermenu][0];
 view.bounds[ecw_lowermenu][1]=view.bounds[ecw_uppermenu][3]+1; 
 view.bounds[ecw_lowermenu][2]=view.maxxsize-1;
 view.bounds[ecw_lowermenu][3]=(supervga ? view.txt_ypos : view.maxysize-105)
  -2*view.fontheight-2;
 view.bounds[ecw_menuline][0]=0;
 view.bounds[ecw_menuline][1]=0;
 view.bounds[ecw_menuline][2]=view.bounds[ecw_uppermenu][0]-1;
 view.bounds[ecw_menuline][3]=view.yoffset-5;
 view.bounds[ecw_movebuttons][0]=view.bounds[ecw_lowermenu][0];
 view.bounds[ecw_movebuttons][1]=view.bounds[ecw_lowermenu][3]+
  2*view.fontheight+2;
 view.bounds[ecw_movebuttons][2]=(supervga ? view.txt_xpos :view.maxxsize)-11;
 view.bounds[ecw_movebuttons][3]=(supervga ? view.maxysize : view.txt_ypos)-5;
 if((view.menubuffer=sys_initmenu(view.bounds[ecw_uppermenu][0],
  view.bounds[ecw_uppermenu][1],view.bounds[ecw_uppermenu][2],
  view.bounds[ecw_uppermenu][3]))==NULL)
  { fprintf(errf,"Can't init menu.\n"); my_exit(); }
 view.menuheight=view.smenuheight-3; /* 2 input, 1 title */
 sys_initend();
 drawallbuttons();
 }
