/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    grfx.c - init graphics, drawing menus.
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
#include "grfx.h"

void printline(int lno,char *fstr,...);
GrFBoxColors boxcolors;  
char *structname[ds_number]={ "cube","side","corner","thing","door",
 "internal","level","switch","producer","point" };
 
void printline(int lno,char *fstr,...)
 {
 va_list args;
 char buffer[100];
 if(lno>=view.menu.txr_height) return;
 va_start(args,fstr);
 vsprintf(buffer,fstr,args);
 buffer[view.menu.txr_width]=0; /* so the string fits in the line */
 strcpy(&view.menu.txr_buffer[lno*view.menu.txr_width],buffer);
 va_end(args);
 }
 
void printinfoitem(struct infoitem *i)
 {
 struct objtype *ot;
 char *buffer;
 int k,l,row;
 unsigned long no=0;
 double dno=0.0;
 unsigned char *data;
 unsigned int **si;
 row=i->row-view.menuoffset;
 if(row>=view.menuheight-1) return;
 if(i->offset<0 && i->od!=NULL) 
  {
  sprintf(&view.menu.txr_buffer[view.menu.txr_width*(row+1)
   +i->column+1],"%.*s",view.menu.txr_width,i->txt);
  return;
  }
 if((data=getdata(i->infonr))==NULL)
  { sprintf(&view.menu.txr_buffer[(i->row+1)*view.menu.txr_width+i->column],
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
    sprintf(&view.menu.txr_buffer[view.menu.txr_width*(row+1)+i->column+1],
     i->txt,no);
   else
    sprintf(&view.menu.txr_buffer[view.menu.txr_width*(row+1)+i->column+1],
     i->txt,dno);
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
   sprintf(&view.menu.txr_buffer[view.menu.txr_width*(row+1)
    +i->column+1],"%.*s",view.menu.txr_width,buffer);
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
  view.curmenuitem->row-view.menuoffset<view.menu.txr_height-1)
  GrDumpChar('*',view.curmenuitem->column,
   view.curmenuitem->row-view.menuoffset+1,&view.menu);
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
  sprintf(&view.menu.txr_buffer[row*view.menu.txr_width],
   "Data %.4lx",(long)size); 
  }
 column=0;
 while(offset<size && row<view.menuheight-1)
  {
  if(column==0)
   sprintf(&view.menu.txr_buffer[(row+1)*view.menu.txr_width],"%.4x",
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
 GrFramedBox(2,2,view.menu.txr_xpos-4,view.menu.txr_font->fnt_height+2,2,
  &boxcolors);
 for(n1=0,x=0;n1<view.nummenus;n1++)
  {
  GrTextXY(x+2,2,view.menuline[n1].str,view.color[MENUCOLORS+5],
   view.color[MENUCOLORS]); 
  GrHLine(x+1+(view.menu.txr_font->fnt_width*(view.menuline[n1].no)&0xfffful),
   x+1+(view.menu.txr_font->fnt_width*(view.menuline[n1].no+1)&0xfffful),
   4+view.menu.txr_font->fnt_baseline,view.color[MENUCOLORS+5]);
  x+=strlen(view.menuline[n1].str)*view.menu.txr_font->fnt_width;
  }
 }
 
void drawmenu(void)
 {
 struct wall *w;
 memset(view.menu.txr_buffer,(int)' ',view.menu.txr_width*
  (view.menu.txr_height-1));
 if(view.showwhat<in_number)
  printmenu(view.info[view.showwhat],view.infonum[view.showwhat]);
 else
  { fprintf(errf,"value of showwhat not known: %d\n",view.showwhat); 
    my_exit(); }
 printline(0,"DEVIL:%8s/%1d",view.bnames[view.showwhat],view.currmode);
 printline(view.menu.txr_height-2,"Cube: %.4d/%.1d/%.1d/%s",
  (view.pcurrcube==NULL) ? -1 : view.pcurrcube->no,view.currwall,
  view.currpnt,
  (view.pcurrcube==NULL) ? "---" :
  (view.currmode==tt_pnt) ? 
  findnode(&view.tagged[tt_pnt],
   view.pcurrcube->no*24+view.currwall*4+view.currpnt)==NULL ? "No " : "Yes" :
  (view.currmode==tt_wall) ?
  findnode(&view.tagged[tt_wall],view.pcurrcube->no*6+view.currwall)==NULL ?
   "No " : "Yes" :
  findnode(&view.tagged[tt_cube],view.pcurrcube->no)==NULL ? "No " : "Yes");
 printline(view.menu.txr_height-1,"T:%.4d/%s W:%.3d/%s",
  (view.pcurrthing==NULL) ? -1 : view.pcurrthing->no,
  (view.pcurrthing==NULL) ? "---" :
  findnode(&view.tagged[tt_thing],view.pcurrthing->no)==NULL ? "No " : "Yes",
  (view.pcurrdoor==NULL) ? -1 : view.pcurrdoor->no,
  (view.pcurrdoor==NULL) ? "---" :
  findnode(&view.tagged[tt_door],view.pcurrdoor->no)==NULL ? "No " : "Yes");
 GrDumpTextRegion(&view.menu);
 if(view.showwhat<in_number && (view.drawwhat&DW_DATA)!=0)
  markknownbytes(view.info[view.showwhat],view.descripnum[view.showwhat]);
 GrSetContext(view.texture);
 GrClearContext(view.color[BLACK]);
 GrSetContext(NULL);
 if(view.currmode==tt_door)
  w=(view.pcurrdoor!=NULL) ? view.pcurrdoor->d.d->w : NULL;
 else
  w=view.pcurrwall;
 if(w)
  {
  drawtexture(w->texture1,0);
  if(w->texture2!=0)
   drawtexture(w->texture2,1);
  else
   drawtexture(-1,1);
  }
 else
  { drawtexture(-1,0); drawtexture(-1,1); }
 printmenuline();
 }

void closegrph(void)
 {
 MouseUnInit();
 GrDestroyContext(view.txt_complete);
 GrDestroyContext(view.texture);
 GrDestroyContext(view.movebuttons);
 GrDestroyContext(view.buttonmenu);
 GrDestroyContext(view.txt_context); 
 GrDestroyContext(view.txt_shape); 
 free(view.txt_buffer);
 free(view.txt_sbuffer);
 GrSetMode(GR_80_25_text,0,0);
 }

void drawbutton(int no,GrFont *f)
 {
 int x,y,width=2;
 char *txt;
 if(no>=MAX_XNOBUTTON*MAX_YNOBUTTON) 
  { printf("drawbutton: no %d too large: %d\n",no,MAX_XNOBUTTON*
    MAX_YNOBUTTON); my_exit(); }
 txt=view.bnames[no];
 x=(no % MAX_XNOBUTTON)*view.bxsize;
 y=(no / MAX_XNOBUTTON)*view.bysize;
 GrFramedBox(x+width,y+width,x+view.bxsize-width,y+view.bysize-width,
  width,&boxcolors);
 GrTextXY(x+(view.bxsize-f->fnt_width*strlen(txt))/2,
  y+(view.bysize-f->fnt_height)/2,txt,view.color[MENUCOLORS+5],
   view.color[MENUCOLORS]);
 }
 
void makecolor(int i,int r,int g,int b)
 {
 int n,dist,mdist=255+255+255+1,rn,bn,gn;
 for(n=0;n<255;n++)
  {
  GrQueryColor(n,&rn,&gn,&bn);
  dist=abs(r-rn)+abs(g-gn)+abs(b-bn);
  if(dist<mdist)
   { 
   view.color[i]=n; 
   if((mdist=dist)==0) break;
   }
  }
 }

void drawmovebuttons(void)
 {
 int xsize,ysize,xpos,ypos,width,i,j;
 GrTextOption txt={NULL,1,1,{view.color[MENUCOLORS+5]},
  {view.color[MENUCOLORS]},GR_TEXT_RIGHT,GR_ALIGN_CENTER,GR_ALIGN_CENTER,
  GR_BYTE_TEXT};
 xsize=GrMaxX()/3; ysize=GrMaxY()/3; width=2;
 for(i=0;i<3;i++)
  for(j=0;j<2;j++)
   GrFramedBox(i*xsize+width,j*ysize+width,(i+1)*xsize-width,
    (j+1)*ysize-width,width,&boxcolors);
 xpos=xsize/2+1; ypos=ysize/2+1;
 GrDrawChar(17,xpos,ypos,&txt); /* <| */
 GrDrawChar(24,xpos+xsize,ypos,&txt); /* /\ */
 GrDrawChar(16,xpos+2*xsize,ypos,&txt); /* |> */
 GrDrawChar(27,xpos,ypos+ysize,&txt); /* <- */
 GrDrawChar(25,xpos+xsize,ypos+ysize,&txt); /* \/ */
 GrDrawChar(26,xpos+2*xsize,ypos+ysize,&txt); /* -> */
 } 
 
void initgrph(void)
 {
 int i;
 unsigned char *p;
 FILE *f;
 GrFont *grfont=CHECK_FONT(NULL); /* not documented function of libgrx */
 GrSetMode(GR_width_height_color_graphics,640,480,256);
 if(GrSizeX()!=640 || GrSizeY()!=480 || GrNumColors()!=256)
  { fprintf(errf,"Can't init graphics mode 640*480*256: %d %d %d\n",
     GrSizeX(),GrSizeY(),GrNumColors()); my_exit(); }
 if((f=fopen(view.palettename,"rb"))==NULL)
  { fprintf(errf,"Can't open palette file: \"%s\"\n",view.palettename); 
    my_exit(); }
 if((p=malloc(256*3))==NULL)
  { fprintf(errf,"No mem for colors\n"); my_exit(); }
 if(fread(p,(size_t)1,(size_t)256*3,f)!=256*3)
  { fprintf(errf,"Can't read colors.\n"); my_exit(); }
 fclose(f);
 for(i=1;i<256;i++)
  GrSetColor(i,p[i*3]+(view.brightness*p[i*3])/(255-view.brightness),
   p[i*3+1]+(view.brightness*p[i*3+1])/(255-view.brightness),
   p[i*3+2]+(view.brightness*p[i*3+2])/(255-view.brightness));
 free(p);
 GrSetColor(0,0,0,0);
 GrSetColor(255,255,255,255);
 for(i=0;i<USEDCOLORS;i++)
  view.color[i]=i;
 makecolor(BLACK,0,0,0); 
 for(i=1;i<=GRAYSCALE;i++) 
  makecolor(i,(125*i)/GRAYSCALE+75,
   (125*i)/GRAYSCALE+75,(125*i)/GRAYSCALE+75); /* walls */
 makecolor(THINGCOLORS,200,0,0); /* things type robot */
 makecolor(1+THINGCOLORS,0,200,200);/* things type start */
 makecolor(2+THINGCOLORS,0,0,200); /* things type item */
 makecolor(3+THINGCOLORS,200,0,200);/* things type reactor */
 makecolor(HILIGHTCOLORS,0,255,0);/* hilighted cube/thing */
 makecolor(HILIGHTCOLORS+1,255,0,0);  /* hilighted wall; cube for sdoor */
 makecolor(HILIGHTCOLORS+2,0,0,255); /* tagged cube/thing */
 view.taggedline.lno_color=view.color[HILIGHTCOLORS+2];
 view.taggedline.lno_width=2;
 view.taggedline.lno_pattlen=0; view.taggedline.lno_dashpat=NULL;
 makecolor(HILIGHTCOLORS+3,255,100,255); /* hilight for sdoor for doors */
 makecolor(DOORCOLORS,200,100,100); /* blow door */
 makecolor(DOORCOLORS+1,150,150,150); /* no key door */
 makecolor(DOORCOLORS+2,0,0,200); /* blue key */
 makecolor(DOORCOLORS+3,200,0,0); /* red key */
 makecolor(DOORCOLORS+4,200,200,0); /* yellow key */
 makecolor(MENUCOLORS,150,150,150); /* GrFBoxColors for menu (int) */
 makecolor(MENUCOLORS+1,100,100,100); /* top */
 makecolor(MENUCOLORS+2,200,200,200); /* right */
 makecolor(MENUCOLORS+3,200,200,200); /* bottom */
 makecolor(MENUCOLORS+4,100,100,100); /* left */
 makecolor(MENUCOLORS+5,255,255,255); /* textcolor of menu */
 makecolor(WHITE,255,255,255); /* normal white */
 boxcolors.fbx_intcolor=view.color[MENUCOLORS];
 boxcolors.fbx_topcolor=view.color[MENUCOLORS+1];
 boxcolors.fbx_rightcolor=view.color[MENUCOLORS+2];
 boxcolors.fbx_bottomcolor=view.color[MENUCOLORS+3];
 boxcolors.fbx_leftcolor=view.color[MENUCOLORS+4];
 view.maxxsize=GrSizeX();
 view.maxysize=GrSizeY();
 view.ysize=GrSizeY()-2*grfont->fnt_height-13;
 view.xsize=view.ysize;
 view.xoffset=10; view.yoffset=grfont->fnt_height+7;
 view.mouse=MouseDetect();
 MouseEventMode(0);
 MouseInit();
 MouseSetColors(view.color[WHITE],view.color[BLACK]);
 MouseDisplayCursor();
 MouseSetAccel(100,1);
 view.menu.txr_xpos=view.xsize+2*view.xoffset;
 view.menu.txr_ypos=0;
 if((view.texture=GrCreateSubContext(view.maxxsize-70,view.maxysize-100,
  view.maxxsize-4,view.maxysize-36,NULL,NULL))==NULL)
  { printf("No mem for creating sub context texture.\n"); exit(0); }
 view.txt_maxxnum=view.xsize/128;
 view.txt_maxynum=view.ysize/128;
 if((view.txt_window=GrCreateSubContext(view.xoffset+view.xsize/2-
  view.txt_maxxnum*64,view.yoffset+view.ysize/2-view.txt_maxynum*64,
  view.xoffset+view.xsize/2+view.txt_maxxnum*64,
  view.yoffset+view.ysize/2+view.txt_maxynum*64,NULL,NULL))==NULL)
  { printf("No mem for creating sub context txt_window.\n"); exit(0); }
 view.txt_txtxpos[0]=view.txt_txtxpos[1]=view.maxxsize-70;
 view.txt_txtypos[0]=view.maxysize-30;
 view.txt_txtypos[1]=view.maxysize-15;
 if((view.txt_buffer=malloc(64*64))==NULL)
  { fprintf(errf,"No mem for texture buffer.\n"); my_exit(); }
 if((view.txt_sbuffer=malloc(64*64))==NULL)
  { fprintf(errf,"No mem for texture shape buffer.\n"); my_exit(); }
 if((view.txt_context=GrCreateContext(64,64,view.txt_buffer,NULL))==NULL)
  { fprintf(errf,"No context for texture part.\n"); my_exit(); }
 if((view.txt_shape=GrCreateContext(64,64,view.txt_sbuffer,NULL))==NULL)
  { fprintf(errf,"No context for texture shape.\n"); my_exit(); }
 if((view.txt_complete=GrCreateContext(64,64,NULL,NULL))==NULL)
  { fprintf(errf,"No context for complete texture.\n"); my_exit(); }
 view.menu.txr_font=grfont;
 view.menu.txr_width=(view.maxxsize-view.menu.txr_xpos)/grfont->fnt_width;
 view.menu.txr_lineoffset=view.menu.txr_width;
 view.menu.txr_height=(view.maxysize-view.menu.txr_ypos)*3/5/
  grfont->fnt_height;
 view.menuheight=view.menu.txr_height-3; /* 2 input, 1 title */
 if((view.menu.txr_buffer=malloc(view.menu.txr_width*view.menu.txr_height))
  ==NULL)
  { printf("No mem for menu.\n"); my_exit(); }
 view.menu.txr_backup=NULL; 
 for(i=0;i<view.menu.txr_width*view.menu.txr_height;i++)
  { /* view.menu.txr_backup[i]=' '; */ view.menu.txr_buffer[i]=' '; }
 if(view.menu.txr_width<16 && view.menu.txr_height<16)
  { printf("Wrong size (too short) for menu.\n"); my_exit(); }
 view.menu.txr_fgcolor.v=view.color[WHITE];
 view.menu.txr_bgcolor.v=view.color[BLACK];
 view.menu.txr_chrtype=GR_BYTE_TEXT;
 GrDumpText(0,0,17,5,&view.menu);
 /* now draw the Button menu */
 if((view.buttonmenu=GrCreateSubContext(view.menu.txr_xpos,
  view.menu.txr_ypos+view.menu.txr_height*grfont->fnt_height+10,
  view.maxxsize-1,view.texture->gc_yoffset,NULL,NULL))==NULL)
  { printf("No mem for creating button menu.\n"); exit(0); }
 GrSetContext(view.buttonmenu);
 view.bxsize=GrMaxX()/MAX_XNOBUTTON; 
 view.bysize=GrMaxY()/MAX_YNOBUTTON;
 for(i=0;i<view.numbuttons;i++)
  drawbutton(i,grfont);
 GrSetContext(NULL);
 if((view.movebuttons=GrCreateSubContext(view.menu.txr_xpos+5,
  view.texture->gc_yoffset,view.texture->gc_xoffset-5,GrMaxY()-5,NULL,NULL))
  ==NULL)
  { printf("No mem for movebuttons context.\n"); exit(0); }
 GrSetContext(view.movebuttons);
 drawmovebuttons();
 GrSetContext(NULL);
 /* setting boundaries for events */
 view.bounds[ecw_screen][0]=0;
 view.bounds[ecw_screen][1]=0;
 view.bounds[ecw_screen][2]=view.maxxsize; 
 view.bounds[ecw_screen][3]=view.maxysize;
 view.bounds[ecw_plot][0]=view.xoffset;
 view.bounds[ecw_plot][1]=view.yoffset; 
 view.bounds[ecw_plot][2]=view.xoffset+view.xsize; 
 view.bounds[ecw_plot][3]=view.yoffset+view.ysize; 
 view.bounds[ecw_uppermenu][0]=view.menu.txr_xpos; 
 view.bounds[ecw_uppermenu][1]=view.menu.txr_ypos;
 view.bounds[ecw_uppermenu][2]=view.menu.txr_xpos+
  view.menu.txr_width*view.menu.txr_font->fnt_width;
 view.bounds[ecw_uppermenu][3]=view.menu.txr_ypos+view.menu.txr_height*
  view.menu.txr_font->fnt_height; 
 view.bounds[ecw_lowermenu][0]=view.buttonmenu->gc_xoffset; 
 view.bounds[ecw_lowermenu][1]=view.buttonmenu->gc_yoffset; 
 view.bounds[ecw_lowermenu][2]=view.buttonmenu->gc_xoffset+
  view.buttonmenu->gc_xmax;
 view.bounds[ecw_lowermenu][3]=view.buttonmenu->gc_yoffset+
  view.buttonmenu->gc_ymax;
 view.bounds[ecw_menuline][0]=0;
 view.bounds[ecw_menuline][1]=0;
 view.bounds[ecw_menuline][2]=view.menu.txr_xpos-1;
 view.bounds[ecw_menuline][3]=view.yoffset-1;
 view.bounds[ecw_movebuttons][0]=view.movebuttons->gc_xoffset;
 view.bounds[ecw_movebuttons][1]=view.movebuttons->gc_yoffset;
 view.bounds[ecw_movebuttons][2]=view.movebuttons->gc_xoffset+
  view.movebuttons->gc_xmax;
 view.bounds[ecw_movebuttons][3]=view.movebuttons->gc_yoffset+
  view.movebuttons->gc_ymax;
 }
