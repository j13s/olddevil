/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    system.c - all system dependent functions.
    This is system.c for GNU_C.
    If you program a system.c for a compiler please mail me the #define
    constant you use (like GNU_C) and the name of your compiler, so I can
    update devil.c. I will inform you if a new version is released and 
    E-mail you the updates (or mail you where to get them or something
    like that).
    
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
#include <grx.h>
#include <grxfont.h>
#include <mousex.h>
#include <sys/farptr.h>
#include <dos.h>
#include <dir.h>
#include "system.h"
#include "structs.h"

/* System-dependent global variables. Only used in this file, so delete
  what you don't need, add what you need. */
struct systemview 
 {
 GrFont *grfont; /* the graphics font */
 GrContext *txt_complete; /* for readbitmap */
 GrContext *txt_window;  /* for fit bitmap */
 GrTextRegion menu; /* the menu on the upper right side */
 GrFBoxColors boxcolors; /* the colors for the buttons */
 int oldpos,oldheight,oldcolor; /* for saving the menu and color
  datas if switching between 320*200 and 640*480 */
 struct ffblk macfiles; /* for searching macros. */
 } sys_view;

/* Init graphics mode. If supervga=1 try to init 640*480*256. If this
   won't work or if supervga=0 try to init 640*480*16.
   Also init view.maxxsize (=640), view.maxysize (=480) and
   fontwidth, fontheight.
   Error messages in FILE *errf.
   Return:
    -1 for no graphics mode.
    0 for normal VGA.
    1 for SVGA. */
int sys_initgrfx(int supervga)
 {
 sys_view.grfont=CHECK_FONT(NULL); /* not documented function of libgrx */
 if(supervga) GrSetMode(GR_width_height_color_graphics,640,480,256);
 if(!supervga || GrSizeX()!=640 || GrSizeY()!=480 || GrNumColors()!=256)
  {
  if(supervga)
   { fprintf(errf,"Can't init graphics mode 640*480*256: %d %d %d\n",
      GrSizeX(),GrSizeY(),GrNumColors()); 
     fprintf(errf,"Trying 640*480*16...\n"); supervga=0; }
  GrSetMode(GR_width_height_color_graphics,640,480,16);
  if(GrSizeX()!=640 || GrSizeY()!=480 || GrNumColors()!=16)
   { fprintf(errf,"Can't init graphics mode 640*480*16: %d %d %d\n",
      GrSizeX(),GrSizeY(),GrNumColors()); return -1; }
  }
 view.maxxsize=640; view.maxysize=480; 
 view.fontheight=sys_view.grfont->fnt_height;
 view.fontwidth=sys_view.grfont->fnt_width;
 return supervga;
 }
 
/* Switch to texture graphics mode (must be 320*200*256). This function is
   only needed if SVGA is not guaranteed. Be careful with the mouse pointer.
   In 320*200*256 mouse is(!) supported by DEVIL.
   You must guarantee that the menu functions work properly in 320*200.
   If you don't program this set SYS_VGA_SUPPORTED in system.h to 0.
   returns:
    0 if not possible
    1 if possible */
int sys_changeto256(void)
 {
 sys_view.oldpos=sys_view.menu.txr_xpos;
 sys_view.oldheight=sys_view.menu.txr_height;
 sys_view.oldcolor=sys_view.menu.txr_fgcolor.v; 
 GrSetMode(GR_width_height_color_graphics,320,200,256);
 MouseSetColors(view.color[WHITE],view.color[BLACK]);
 MouseDisplayCursor();
 sys_view.menu.txr_xpos=100; 
 view.smenuheight=sys_view.menu.txr_height=
  GrMaxY()/sys_view.menu.txr_font->fnt_height;
 sys_view.menu.txr_fgcolor.v=view.color[WHITE];
 sys_view.menu.txr_bgcolor.v=view.color[BLACK];
 return GrSizeX()==320 && GrSizeY()==200 && GrNumColors()==256;
 }

/* Switch to VGA graphics mode (640*480*16). This function is only needed if
   SVGA is not guaranteed. Be careful with the mouse pointer.
   If you don't program this set SYS_VGA_SUPPORTED in system.h to 0.
   returns:
    0 if not possible
    1 if possible */
int sys_changeto16(void)
 {
 GrSetMode(GR_width_height_color_graphics,640,480,16);
 MouseSetColors(view.color[WHITE],view.color[BLACK]);
 MouseDisplayCursor();
 sys_view.menu.txr_xpos=sys_view.oldpos; 
 view.smenuheight=sys_view.menu.txr_height=sys_view.oldheight;
 sys_view.menu.txr_fgcolor.v=sys_view.oldcolor; 
 return GrSizeX()==640 && GrSizeY()==480 && GrNumColors()==16;
 }

/* Initialize space for textures (not on the screen). This routine
   is called in the texture mode (if not SVGA) as current mode.
   You must use view.txt_buffer (64*64 byte). 
   readbitmap will read the bitmap from descent.pig 
   in txt_buffer. Then you must copy with sys_drawtexture the texture
   out of this buffer to the screen. 
   returns
    0 if not completed
    1 if completed */
int sys_inittxtspace(void)
 {
 /* the texture */
 if((sys_view.txt_complete=GrCreateContext(64,64,view.txt_buffer,NULL))==NULL)
  { fprintf(errf,"No context for texture part.\n"); return 0; }
 return 1;
 }
 
/* Setting colors. Index is not important. I just use it that no
   color-register is used twice but you can omit it. */
void sys_setcolor(int i,int r,int g,int b)
 {
 GrSetColor(i,r,g,b);
 }
 
/* Getting colors. Index is important (of course). */
void sys_getcolor(int i,int *r,int *g,int *b)
 {
 GrQueryColor(i,r,g,b);
 }

/* Init mouse. You must set view.mouse=0 if there is no mouse 1 otherwise. 
   You must be able to check for events without waiting (see sys_getevent) */
void sys_initmouse(void)
 {
 view.mouse=MouseDetect();
 if(view.mouse)
  {
  MouseEventMode(0);
  MouseInit();
  MouseSetColors(view.color[WHITE],view.color[BLACK]);
  MouseDisplayCursor();
  }
 }
 
/* Init text menu in the upper, right corner. x1,y1,x2,y2 are the coords.
   you must set view.smenuheight, view.smenuwidth with the width and height
   of the menu in chars.
   returns the pointer on the buffer for the chars if successful, 
    NULL otherwise */
char *sys_initmenu(int x1,int y1,int x2,int y2)
 {
 int i;
 sys_view.menu.txr_xpos=x1;
 sys_view.menu.txr_ypos=y1;
 sys_view.menu.txr_font=sys_view.grfont;
 view.smenuwidth=sys_view.menu.txr_width=(x2-x1)/sys_view.grfont->fnt_width;
 sys_view.menu.txr_lineoffset=sys_view.menu.txr_width;
 view.smenuheight=sys_view.menu.txr_height=
  (y2-y1)/sys_view.grfont->fnt_height;
 if((sys_view.menu.txr_buffer=malloc(sys_view.menu.txr_width*
  sys_view.menu.txr_height))==NULL)
  { printf("No mem for menu.\n"); return NULL; }
 sys_view.menu.txr_backup=NULL; 
 for(i=0;i<sys_view.menu.txr_width*sys_view.menu.txr_height;i++)
  sys_view.menu.txr_buffer[i]=' ';
 sys_view.menu.txr_fgcolor.v=view.color[WHITE];
 sys_view.menu.txr_bgcolor.v=view.color[BLACK];
 sys_view.menu.txr_chrtype=GR_BYTE_TEXT;
 GrDumpText(0,0,17,5,&sys_view.menu);
 return sys_view.menu.txr_buffer;
 }

/* Draw a button with text inside. txt=NULL is allowed. */
void sys_drawbutton(int x1,int y1,int x2,int y2,char *txt)
 {
 GrTextOption txt_opt={NULL,1,1,{view.color[MENUCOLORS+5]},
  {view.color[MENUCOLORS]},GR_TEXT_RIGHT,GR_ALIGN_CENTER,GR_ALIGN_CENTER,
  GR_BYTE_TEXT};
 int width=1;
 GrFramedBox(x1+width,y1+width,x2-width,y2-width,width,&sys_view.boxcolors);
 if(txt!=NULL)
  GrDrawString(txt,strlen(txt),x1+(x2-x1)/2+1,y1+(y2-y1)/2+1,&txt_opt);
 } 

/* Draw the text txt to x,y. Use fg as pencil, bg as background color. */
void sys_text(int x,int y,char *txt,int fg,int bg)
 { GrTextXY(x,y,txt,fg,bg); }

/* Draw the text txt to x,y and underline the character on the pos. u.
   Use view.color[MENUCOLORS+5] as pencil, 
   view.color[MENUCOLOR] as background color. If u==-1 make no
   underline. */
void sys_menutext(int x,int y,char *txt,int u)
 {
 GrTextXY(x,y,txt,view.color[MENUCOLORS+5],view.color[MENUCOLORS]); 
 if(u!=-1)
  GrHLine(x-1+view.fontwidth*u,x-1+view.fontwidth*(u+1),
   y+sys_view.menu.txr_font->fnt_baseline+2,view.color[MENUCOLORS+5]);
 }
 
 
/* In this function you can init all the variables you need. It's called
   at the end of the init-routines. */
void sys_initend(void)
 {
 sys_view.boxcolors.fbx_intcolor=view.color[MENUCOLORS];
 sys_view.boxcolors.fbx_topcolor=view.color[MENUCOLORS+1];
 sys_view.boxcolors.fbx_rightcolor=view.color[MENUCOLORS+2];
 sys_view.boxcolors.fbx_bottomcolor=view.color[MENUCOLORS+3];
 sys_view.boxcolors.fbx_leftcolor=view.color[MENUCOLORS+4];
 }

/* this routine should reset the computer to normal txt-mode and free
   all used mem. It's only called at the end of the program. */
void sys_clearall(void)
 {
 MouseUnInit();
 GrDestroyContext(sys_view.txt_complete);
 GrSetMode(GR_80_25_text,0,0);
 }
 
/*Draw a filled box with color c. If xor=1 xor it, otherwise just write it.
  no clipping is possible. */
void sys_filledbox(int x1,int y1,int x2,int y2,int c,int xor)
 { GrFilledBox(x1,y1,x2,y2,xor ? c|GrXOR : c); }
 
/* just draw a line with color c. if xor=1 xored, otherwise normal.
   this can be a no-clipping line (in GNU-C not because I get trouble with
   the mousepointer). */
void sys_line(int x1,int y1,int x2,int y2,int c,int xor)
 { GrLine(x1,y1,x2,y2,xor ? c|GrXOR : c); }

/* Init clipping area for sys_clipline (you must call this before calling
   sys_clipline). */
void sys_initclipping(int x1,int y1,int x2,int y2)
 { GrSetClipBox(x1,y1,x2,y2); }
 
/* Kill the clipping area (after this sys_clipline isn't allowed anymore) */
void sys_killclipping(void)
 { GrResetClipBox(); }
 
/* same as sys_line but with clipping (see also sys_initclipping) */
void sys_clipline(int x1,int y1,int x2,int y2,int c,int xor)
 { GrLine(x1,y1,x2,y2,xor ? c|GrXOR : c); }
 
/* Copy the text from the upper menu to screen. */
void sys_dumpmenu(void)
 { GrDumpTextRegion(&sys_view.menu); }

/* Change one character in the upper menu and print it on the screen. */
void sys_dumpmenuchar(char x,int column,int row)
 { GrDumpChar(x,column,row,&sys_view.menu); }

/* get the status of the numlock-key. if numlock is on return 1, otherwise
   0. */
int sys_getnumlock(void)
 { return (_farpeekb(0x40,0x17)&0x20)!=0; }
 
/* switch the status of numlock. if it's on turn it off and vice versa.
   return 1 if numlock is on, 0 otherwise. */
int sys_numlockonoff(void)
 {
 unsigned char value;
 value=_farpeekb(0x40,0x17);
 _farpokeb(0x40,0x17,value^0x20);
 return (value^0x20)!=0;
 }

/* draw the text txt at txt_x, txt_y. if plot!=0 also copy the texture
   out of txt_buffer to the screen. txt=NULL is allowed.
   no clipping in both functions is possible. */
void sys_drawtexture(int plot,int x,int y,int txt_x,int txt_y,char *txt)
 {
 if(txt) GrTextXY(txt_x,txt_y,txt,view.color[WHITE],view.color[BLACK]); 
 if(plot) GrBitBlt(NULL,x,y,sys_view.txt_complete,0,0,63,63,GrWRITE);
 }
 
/* Disable control-c. (remember: Cube-menu) */
void sys_disablectrlc(void)
 { setcbrk(0); }
 
/* get an event (key or mousebutton pressed/released).
   if wait==0 don't wait for an event. if there's no event return 0.
   Translate the event in DEVIL - format. See system.h the
   mouseevent structure also.
   make an event for pressing&for releasing the mousebutton but only
   one for pressing a key. */
int sys_getevent(struct sys_event *se,int wait)
 {
 int kbstat_ttable[6] = { KB_SHIFT,KB_ALT,KB_CTRL,KB_NUMLOCK,KB_CAPSLOCK,
  KB_SCROLLOCK };
 MouseEvent me;
 int i;
 MouseGetEvent(M_BUTTON_CHANGE|M_KEYPRESS|(wait ? 0 : M_POLL),&me);
 se->key=me.key; se->x=me.x; se->y=me.y;
 se->flags=(me.flags&M_KEYPRESS) ? se_f_keypress :
  (me.flags&M_RIGHT_UP || me.flags&M_RIGHT_DOWN) ? se_f_rbutton : 
  se_f_lbutton;
 se->buttons=(me.buttons&M_LEFT) ? se_bt_left : 
  (me.buttons&M_RIGHT) ? se_bt_right : se_bt_none;
 for(i=0,se->kbstat=0;i<6;i++)
  if((me.kbstat&kbstat_ttable[i])!=0)
   se->kbstat|=1<<i;
 return me.flags!=0;
 }

/* searches first time for file name pattern <fname>. 
   fname can include a path.
   returns pointer to real filename if found, NULL if not found. 
   real filename must be without path (sorry about this strange
   behaviour, but the GNU-C routine works that way) 
   You can exit the normal way, because grfx mode is not yet initialized. */
char *sys_searchfirstfile(char *fname)
 {
 char *rfname;
 if(findfirst(fname,&sys_view.macfiles,FA_ARCH|FA_RDONLY)==0)
  {
  if((rfname=malloc(strlen(sys_view.macfiles.ff_name)+1))==NULL)
   { printf("No mem for macro filename: %s\n",sys_view.macfiles.ff_name);
     exit(0); }
  strcpy(rfname,sys_view.macfiles.ff_name);
  }
 else rfname=NULL;
 return rfname;
 }
 
/* This routine belongs to sys_searchfirstfile(). It gives the next filename
   matching the pattern. */
char *sys_searchnextfile(void)
 {
 char *rfname;
 if(findnext(&sys_view.macfiles)==0)
  {
  if((rfname=malloc(strlen(sys_view.macfiles.ff_name)+1))==NULL)
   { printf("No mem for macro filename: %s\n",sys_view.macfiles.ff_name);
     exit(0); }
  strcpy(rfname,sys_view.macfiles.ff_name);
  }
 else rfname=NULL;
 return rfname;
 }