/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_system.c - All non-ANSI-C routines
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

#include <signal.h>    
#include <stdlib.h>
#include <grx20.h>
#include <dos.h>
#include <dir.h>
#include <unistd.h>
#include <fnmatch.h>
#include <sys/farptr.h>
#include <sys/stat.h>
#include "wins_int.h"
#include "w_tools.h"

#define DEFAULT_FONT "wins.fnt"

struct ws_internals {
 GrTextOption textopt;
 GrPattern pat_box;
 int mousecounter; /* displaymouse & erasemouse */
 } ws_private= {
  { NULL,{0},{0},GR_BYTE_TEXT,GR_TEXT_RIGHT,GR_ALIGN_LEFT,GR_ALIGN_TOP },
  { 0 },0 };

extern FILE *errf;

/* Initialize Grfx-Mode&Mouse.Try to use xres,yres,colors and font <fontname>.
   If you are successful, return a 1.
   If you can't initialize this mode, return a 0. */
int ws_initgrfx(int xres,int yres,int colors,const char *fontname)
 {
 static unsigned char pixels[2];
/* unsigned char value; */
 GrBitmap pat_bm={ 0,2,pixels,1,0,0 };
 if((ws_private.textopt.txo_font=GrLoadFont((char *)(fontname==NULL ?
  DEFAULT_FONT : fontname)))==NULL) 
  { fprintf(errf,"Can't find font %s\n",fontname); return 0; }
 if(!GrSetMode(GR_width_height_color_graphics,xres,yres,colors))
  { fprintf(errf,"Can't set grfx-mode. Try to load VESA-driver\n");return 0; }
 if(GrSizeX()!=xres || GrSizeY()!=yres || GrNumColors()!=colors) 
  { fprintf(errf,"Can't set grfx-mode. Try to load VESA-driver\n");return 0; }
 GrClearScreen(0);
 if(!GrMouseDetect()) 
  { fprintf(errf,"Can't find mouse\n");return 0; }
 GrMouseEventMode(0);
 GrMouseInit(); 
 GrMouseSetColors(255,0);
 GrMouseDisplayCursor();
 pixels[0]=0xaa; pixels[1]=0x55;
 ws_private.pat_box.gp_bitmap=pat_bm;
/* value=_farpeekb(0x40,0x17);
 _farpokeb(0x40,0x17,value^0x20); */
 return 1;
 }

/* Setting colors. */
void ws_setcolor(int i,int r,int g,int b)
 { GrFreeColor(i); GrSetColor(i,r,g,b); }

/* Reinit mouse colors (if palette is new) */
void ws_resetmousecolors(void)
 {
 GrMouseEraseCursor();
 GrMouseSetColors(w_makecolor(255,255,255),w_makecolor(0,0,0));
 GrMouseUpdateCursor();
 GrMouseDisplayCursor();
 }
 
/* Getting color. */
void ws_getcolor(int i,int *r,int *g,int *b)
 { GrQueryColor(i,r,g,b); }
 
/* Create bitmap with bm inside. */
struct ws_bitmap *ws_createbitmap(int xsize,int ysize,char *bm)
 {
 GrContext *gc;
 struct ws_bitmap *bmap;
 char *mem[4];
 if(bm!=NULL)
  { mem[0]=bm; mem[1]=mem[2]=mem[3]=NULL; }
 if(xsize<0 || ysize<0 || xsize>GrSizeX() || ysize>GrSizeY()) return NULL;
 if((bmap=MALLOC(sizeof(struct ws_bitmap)))==NULL) return NULL;
 if((gc=GrCreateContext(xsize,ysize,bm!=NULL ? mem : NULL,NULL))==NULL)
  { FREE(bmap); return NULL; }
 if(bm==NULL)
  {
  GrSetContext(gc);
  GrClearContext(notes.colindex[cv_winfill]);
  GrSetContext(NULL);
  }
 bmap->bitmap=(void *)gc;
 bmap->xpos=0; bmap->ypos=0; bmap->xsize=xsize; bmap->ysize=ysize;
 bmap->w=NULL;
 return bmap;
 }

/* get the pointer to the data of the bitmap. */
unsigned char *ws_getbitmapdata(struct ws_bitmap *b)
 { return ((GrContext *)(b->bitmap))->gc_baseaddr[0]; }
 
/* Clear the bitmap (fill it with color c) */
void ws_clearbitmap(struct ws_bitmap *b,int c)
 {
 GrSetContext((GrContext *)b->bitmap);
 GrClearContext(c);
 GrSetContext(NULL);
 }
 
/* Copy from bitmap src xpos,ypos bitmap with size xsize,ysize to bitmap
   dst at position x1,y1 (which must be large enough). bitmap=NULL means
   screen. if withbg=1 the notes.colindex[cv_winfill] color will be
   drawn otherwise this color is not drawn */
void ws_copybitmap(struct ws_bitmap *dst,int x1,int y1,
 struct ws_bitmap *src,int xpos,int ypos,int xsize,int ysize,int withbg)
 { GrBitBlt(dst==NULL ? NULL : dst->bitmap,x1,y1,
    src==NULL ? NULL : src->bitmap,xpos,ypos,xpos+xsize-1,ypos+ysize-1,
    withbg ? GrWRITE : GrIMAGE|notes.colindex[cv_winfill]); }
 
/* Copy area (x1,y1)-(x2,y2) from a bitmap (if bm==NULL from the screen)
   and create a structure ws_bitmap with the area in bitmap. Returns NULL if
   not successful, the pointer to the structure otherwise. */
struct ws_bitmap *ws_savebitmap(struct ws_bitmap *bm,int x1,int y1,int xsize,
 int ysize)
 {
 GrContext *gc;
 struct ws_bitmap *bmap;
 if(xsize<0 || ysize<0 || y1<0 || x1<0 || x1+xsize>GrSizeX() || 
  y1+ysize>GrSizeY()) return NULL;
 checkmem(bmap=MALLOC(sizeof(struct ws_bitmap)));
 checkmem(gc=GrCreateContext(xsize,ysize,NULL,NULL));
 GrBitBlt(gc,0,0,bm==NULL ? NULL : bm->bitmap,x1,y1,x1+xsize-1,y1+ysize-1,
  GrWRITE);
 bmap->bitmap=(void *)gc;
 bmap->xpos=x1; bmap->ypos=y1;
 bmap->xsize=xsize; bmap->ysize=ysize;
 bmap->w=NULL;
 return bmap;
 }

/* Free complete structure bm (with the structure itself) */
void ws_freebitmap(struct ws_bitmap *bm)
 {
 my_assert(bm);
 if(bm->bitmap) GrDestroyContext((GrContext *)bm->bitmap);
 free(bm);
 }
 
/* Restore the content of bitmap on the position saved in bitmap and
   kill bm. */
void ws_restorebitmap(struct ws_bitmap *bm)
 {
 my_assert(bm!=NULL && bm->bitmap!=NULL);
 GrBitBlt(NULL,bm->xpos,bm->ypos,(GrContext *)bm->bitmap,0,0,bm->xsize-1,
  bm->ysize-1,GrWRITE);
 ws_freebitmap(bm);
 }
 
/* Draw a line with color c. */
void ws_drawline(int x1,int y1,int x2,int y2,int c,int xor)
 { GrLine(x1,y1,x2,y2,c|(xor ? GrXOR : 0)); }
 
/* Draw a circle with color c. */
void ws_drawcircle(int x,int y,int r,int c,int xor)
 { GrCircle(x,y,r,c|(xor ? GrXOR : 0)); }
 
/* Draw a line with color c to bitmap bm. */
void ws_bmdrawline(struct ws_bitmap *bm,int x1,int y1,int x2,int y2,int c,
 int xor)
 { 
 GrSetContext((GrContext *)bm->bitmap);
 GrLine(x1,y1,x2,y2,c|(xor ? GrXOR : 0));
 GrSetContext(NULL);
 }
 
/* Draw a (not-filled) box with color c. */
void ws_drawbox(int x1,int y1,int xsize,int ysize,int c,int xor)
 { GrBox(x1,y1,x1+xsize-1,y1+ysize-1,c|(xor ? GrXOR : 0)); }
 
/* Draw a (filled) box with color c. */
void ws_drawfilledbox(int x1,int y1,int xsize,int ysize,int c,int xor)
 { GrFilledBox(x1,y1,x1+xsize-1,y1+ysize-1,c|(xor ? GrXOR : 0)); }

/* draw framed box, x,y left upper edge (xs,ys size) of box with frame.
   ltc = left, top color, rbc= right, bottom color of frame.
    inc=fill color. w = width of frame */
void ws_drawframedbox(int x,int y,int xs,int ys,int w,int ltc,int rbc,int inc)
 { 
 GrFBoxColors bc;
 bc.fbx_intcolor=inc;
 bc.fbx_topcolor=bc.fbx_leftcolor=ltc;
 bc.fbx_rightcolor=bc.fbx_bottomcolor=rbc;
 GrFramedBox(x+w,y+w,x+xs-2*w,y+ys-2*w,w,&bc); 
 }

/* set clip box. All routines can only draw in this box. if x1==-1 the
 box is the whole screen */
void ws_setclipping(int x1,int y1,int x2,int y2)
 {
 if(x1<0) { GrResetClipBox(); }
 else GrSetClipBox(x1,y1,x2,y2);
 }
 
/* String length in pixels */
int ws_pixstrlen(const char *txt)
 {return txt==NULL ? 0 : GrStringWidth((char *)txt,strlen(txt),
   &ws_private.textopt);}
 
/* String length in chars which fit in w pixels (assuming all chars
   have an equal width) */
int ws_charstrlen(int w)
 { return w/GrCharWidth('M',&ws_private.textopt); }
 
/* Draw text txt to x,y (left, upper edge). maximum pixel width is w.
   fg,bg=fore/background color. if bg=-1 background is transparent. */
void ws_drawtext(int x,int y,int w,const char *txt,int fg,int bg)
 { 
 char buffer[256];
 int clen=ws_charstrlen(w);
 my_assert(txt!=NULL);
 if(clen>255) clen=255;
 strncpy(buffer,txt,clen);
 buffer[clen]=0;
 if(strlen(buffer)==0) return;
 ws_private.textopt.txo_bgcolor.v=bg==-1 ? GrOR : bg;
 ws_private.textopt.txo_fgcolor.v=fg;
 GrDrawString(buffer,strlen(buffer),x,y,&ws_private.textopt);
 }
 
void ws_bmdrawtext(struct ws_bitmap *bm,int x,int y,int w,const char *txt,
 int fg,int bg)
 { 
 char buffer[256];
 int clen=ws_charstrlen(w);
 my_assert(txt!=NULL);
 if(clen>255) clen=255;
 strncpy(buffer,txt,clen);
 buffer[clen]=0;
 if(strlen(buffer)==0) return;
 ws_private.textopt.txo_bgcolor.v=bg==-1 ? GrOR : bg;
 ws_private.textopt.txo_fgcolor.v=fg;
 GrSetContext((GrContext *)bm->bitmap);
 GrDrawString(buffer,strlen(buffer),x,y,&ws_private.textopt);
 GrSetContext(NULL);
 }
 
/* Switch to normal textmode */
void ws_textmode(void)
 { GrMouseUnInit(); GrSetMode(GR_default_text); }

int altkeys_az[26]={ 286,304,302,288,274,289,290,291,279,292,293,294,
 306,305,280,281,272,275,287,276,275,303,273,301,300,277 };
int altkeys_09[10]={ 385,376,377,378,379,380,381,382,383,384 };
/* get an event (key or mousebutton pressed/released).
   if wait>0 wait for an event. 
   if wait==0 don't wait for an event. if there's no event return 0.
   if wait<0 wait -wait/100 seconds for an event.
   Translate the event in DEVIL - format. See w_system.h.
   make an event for pressing&for releasing the mousebutton but only
   one for pressing a key. If the Ctrl or the Alt key is pressed you must
   return the normal code for the letter. This must work for the Ctrl for the
   letters and for the Alt key for the numbers+the letters. */
int ws_getevent(struct ws_event *se,int wait)
 {
 int kbstat_ttable[6] = { GR_KB_SHIFT,GR_KB_ALT,GR_KB_CTRL,GR_KB_NUMLOCK,
  GR_KB_CAPSLOCK,GR_KB_SCROLLOCK };
 GrMouseEvent me,me2;
 int i;
 if(wait)
  {
  me.flags=GR_M_KEYPRESS;
  while(GrMousePendingEvent() && me.flags==GR_M_KEYPRESS)
   GrMouseGetEvent(GR_M_KEYPRESS|GR_M_BUTTON_CHANGE|GR_M_POLL|GR_M_NOPAINT,
    &me);
  if(wait>0) 
   { GrMouseGetEvent(GR_M_BUTTON_CHANGE|GR_M_KEYPRESS|GR_M_NOPAINT,&me); }
  else
   GrMouseGetEventT(GR_M_BUTTON_CHANGE|GR_M_KEYPRESS|GR_M_NOPAINT,&me,
    -wait*10);
  }
 else
  {
  GrMouseGetEvent(GR_M_BUTTON_CHANGE|GR_M_KEYPRESS|GR_M_POLL|GR_M_NOPAINT,
   &me);
  me2=me;
  while(GrMousePendingEvent() && me2.flags==GR_M_KEYPRESS)
   GrMouseGetEvent(GR_M_KEYPRESS|GR_M_BUTTON_CHANGE|GR_M_POLL|GR_M_NOPAINT,
    &me2);
  }  
 se->key=me.key; se->x=me.x; se->y=me.y;
 se->flags=(me.flags&GR_M_KEYPRESS) ? ws_f_keypress :
  (me.flags&GR_M_RIGHT_UP || me.flags&GR_M_RIGHT_DOWN) ? ws_f_rbutton : 
  (me.flags&GR_M_LEFT_UP || me.flags&GR_M_LEFT_DOWN) ? ws_f_lbutton : 
  ws_f_none;
 se->buttons=ws_bt_none;
 if(me.buttons&GR_M_LEFT) se->buttons|=ws_bt_left;
 if(me.buttons&GR_M_RIGHT) se->buttons|=ws_bt_right;
 for(i=0,se->kbstat=0;i<6;i++)
  if((me.kbstat&kbstat_ttable[i])!=0)
   se->kbstat|=1<<i;
 if(se->kbstat&ws_ks_alt)
  {
  for(i=0;i<26;i++) if(se->key==altkeys_az[i]) 
   { se->key=i+((se->kbstat&ws_ks_shift)!=0 ? 'A' : 'a'); break; }
  if(i==26) 
   for(i=0;i<10;i++) if(se->key==altkeys_09[i]) { se->key=i+48; break; }
  }
 else
  if((se->kbstat&ws_ks_ctrl)!=0 && se->key<27) 
   se->key+=((se->kbstat&ws_ks_shift)!=0 ? 'A' : 'a')-1;
 return (me.flags&(GR_M_BUTTON_CHANGE|GR_M_KEYPRESS))!=0;
 }

/* gives all filenames at 'path' with extension(s) 'ext' (the extensions
   must be seperated with a '.'). In no is the number
   of files found returned. If no==0 no file found, no==-1 path not found,
   no==-2 invalid path or extension. path="" or path=NULL or ext=NULL is not
   allowed. You must free filenames[i] and filenames. */
char **ws_getallfilenames(const char *path,const char *ext,int *no)
 {
 struct ffblk files;
 char buffer[255],real_path[255],**filenames,new_ext[4];
 const char *ext_pos,*old_pos=ext;
 if(path==NULL || strlen(path)==0) { *no=-2; return NULL; }
 strcpy(buffer,path); 
 if(buffer[strlen(buffer)-1]=='/' || buffer[strlen(buffer)-1]=='\\')
  buffer[strlen(buffer)-1]=0;
 if(strlen(buffer)!=2 || buffer[1]!=':')
  if(findfirst(buffer,&files,FA_ARCH|FA_RDONLY|FA_DIREC) ||
   (files.ff_attrib&FA_DIREC)==0) { *no=-1; return NULL; }
 *no=0; filenames=NULL; strcpy(real_path,buffer);
 do
  {
  ext_pos=strchr(old_pos,'.');
  if(ext_pos==NULL) ext_pos=&old_pos[strlen(old_pos)];
  if(ext_pos-old_pos>3) return filenames;
  strncpy(new_ext,old_pos,ext_pos-old_pos);
  new_ext[ext_pos-old_pos]=0; old_pos=ext_pos+1;
  strcpy(buffer,real_path); strcat(buffer,"/*."); strcat(buffer,new_ext);
  if(!findfirst(buffer,&files,FA_ARCH|FA_RDONLY))
   do
    {
    checkmem(filenames=REALLOC(filenames,sizeof(char *)*(++*no)));
    checkmem(filenames[*no-1]=MALLOC((strlen(files.ff_name)+1)*sizeof(char)));
    strcpy(filenames[*no-1],files.ff_name);
    }
   while(!findnext(&files));
  }
 while(strlen(ext_pos)>0);
 return filenames;
 }
 
/* gives all directories at 'path' with extension(s) 'ext' (the extensions
   must be seperated by '.'). In no is the number
   of dirs found returned. If no==0 no file found, no==-1 path not found,
   no==-2 invalid path or extension. path="" or path=NULL or ext=NULL is not
   allowed. You must free filenames[i] and filenames. */
char **ws_getalldirs(const char *path,const char *ext,int *no)
 {
 struct ffblk files;
 char buffer[255],real_path[255],**filenames,new_ext[4];
 const char *ext_pos,*old_pos=ext;
 if(path==NULL || strlen(path)==0) { *no=-2; return NULL; }
 strcpy(buffer,path); 
 if(buffer[strlen(buffer)-1]=='/' || buffer[strlen(buffer)-1]=='\\')
  buffer[strlen(buffer)-1]=0;
 if(strlen(buffer)!=2 || buffer[1]!=':')
  if(findfirst(buffer,&files,FA_ARCH|FA_RDONLY|FA_DIREC) ||
   (files.ff_attrib&FA_DIREC)==0) { *no=-1; return NULL; }
 *no=0; filenames=NULL; strcpy(real_path,buffer);
 do
  {
  ext_pos=strchr(old_pos,'.');
  if(ext_pos==NULL) ext_pos=&old_pos[strlen(old_pos)];
  if(ext_pos-old_pos>3) return filenames;
  strncpy(new_ext,old_pos,ext_pos-old_pos); 
  new_ext[ext_pos-old_pos]=0; old_pos=ext_pos+1;
  strcpy(buffer,real_path); strcat(buffer,"/*."); strcat(buffer,new_ext);
  if(!findfirst(buffer,&files,FA_ARCH|FA_RDONLY|FA_DIREC)) 
   do
    {
    if(files.ff_attrib&FA_DIREC)
     {
     checkmem(filenames=REALLOC(filenames,sizeof(char *)*(++*no)));
     checkmem(filenames[*no-1]=MALLOC((strlen(files.ff_name)+2)*
      sizeof(char)));
     strcpy(filenames[*no-1],files.ff_name);
     filenames[*no-1][strlen(files.ff_name)]='/';
     filenames[*no-1][strlen(files.ff_name)+1]=0;
     }
    }
   while(!findnext(&files));
  }
 while(strlen(ext_pos)>0);
 return filenames;
 }
 
/* Make the most simple path from oldpath and store it in newpath.
   Therefore remove all '..' etc. */
void ws_makepath(const char *oldpath,char *newpath)
 { char *p,*hp,buffer[300];
   if(newpath!=oldpath) p=hp=newpath;
   else p=hp=buffer;
   _fixpath(oldpath,hp);
   while(*p!=0) { if(*p=='\\') *p='/'; *p=toupper(*p); p++; }
   strcpy(newpath,hp); }

/* Split fullpath in drive,path,name,ext. */   
void ws_splitpath(char *fullpath,char *drive,char *path,char *name,
 char *ext)
 { fnsplit(fullpath,drive,path,name,ext); }
 
/* Test if wildcard string matches name (no path allowed, only the filename)*/
int ws_matchname(char *wildcard,char *name)
 { return !fnmatch(wildcard,name,0); }

/* Disable control-c. (remember: Cube-menu) */
void ws_disablectrlc(void)
 { setcbrk(0); signal(SIGINT,SIG_IGN); }

/* Plot a dot-non-dot-non filled Box */
void ws_drawpatternedbox(int x1,int y1,int xsize,int ysize,int c)
 {
 int x,y,m;
/* This is not yet working in my release from grx 2.0:
 ws_private.pat_box.gp_bitmap.bmp_fgcolor=c|GrXOR;
 ws_private.pat_box.gp_bitmap.bmp_bgcolor=GrXOR;
 GrPatternFilledBox(x1,y1,x1+xsize,y1+ysize,&ws_private.pat_box); */
 m=GrMouseBlock(NULL,x1,y1,x1+xsize,y1+ysize);
 for(y=y1;y<y1+ysize;y++)
  for(x=x1+(y&1);x<x1+xsize;x+=2)
   GrPlot(x,y,GrXOR|c);
 GrMouseUnBlock(m);
 }

/* remove the mouse from the screen, so that the drawing is faster. */
void ws_erasemouse(void) 
 { GrMouseEraseCursor(); ws_private.mousecounter--; }

void ws_displaymouse(void) 
 { if(++ws_private.mousecounter>=0) 
    { ws_private.mousecounter=0; GrMouseDisplayCursor(); } }

/* beams the mouse to x,y */
void ws_mousewarp(int x,int y) { GrMouseWarp(x,y); }

int ws_waitforkey(void)
 { return getkey(); }
 
int ws_testforkey(void)
 { return kbhit(); }

void ws_setdriver(const char *name)
 { GrSetDriver((char *)name); }

/* init cursor structure (which is whatever you want). w=width, h=height,
 xo,yo=hot spot, colortable maps colors in data to colors on screen. */ 
ws_cursor *ws_initcursor(char *data,int w,int h,int xo,int yo,
 long *colortable)
 { return (ws_cursor *)GrBuildCursor(data,w,w,h,xo,yo,colortable); }
 
/* set cursor (cursor==NULL is allowed) */ 
void ws_changecursor(ws_cursor *cursor)
 { if(cursor) 
    { GrMouseEraseCursor(); /* GrMouseSetCursor((GrCursor *)cursor); */
      GrMouseDisplayCursor(); }
   else ws_resetmousecolors(); }
   
/* kill cursor */
void ws_killcursor(ws_cursor *cursor)
 { GrDestroyCursor((GrCursor *)cursor); }
