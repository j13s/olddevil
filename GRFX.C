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
#include "userio.h"
#include "tools.h"
#include "plot.h"
#include "initio.h"
#include "tag.h"
#include "insert.h"
#include "calctxt.h"
#include "do_event.h"
#include "do_move.h"
#include "options.h"
#include "do_light.h"
#include "opt_txt.h"
#include "readlvl.h"
#include "readtxt.h"
#include "plottxt.h"
#include "grfx.h"
#if defined(GNU_C) && defined(GO32)
#include "title.h"
#endif

extern int init_test;

void closegrph(void) { w_closewins(); releasetimer(); }

/* Routines for button window: */
void b_changemode(struct w_button *b)
 {
 if(b->d.ls->selected>=tt_number || b->d.ls->selected<0)
  printmsg("Unknown mode in changemode.\n");
 else
  { view.currmode=b->d.ls->selected; shrinkopt_win(view.currmode); }
 }
 
void b_changemovemode(struct w_button *b)
 {
 if(b->d.ls->selected>=mt_number || b->d.ls->selected<0)
  printmsg("Unknown mode in changemovemode.\n");
 else 
  if(b->d.ls->selected==mt_texture)
   if(fb_isactive()) { fb_tofront(); view.movemode=mt_texture; }
   else changemovemode(view.movemode==mt_texture ? mt_you : view.movemode);
  else view.movemode=b->d.ls->selected;
 }

void b_move0(struct w_button *b) { b_move(2,-1);}
void b_move1(struct w_button *b) { b_move(1,1);}
void b_move2(struct w_button *b) { b_move(2,1);}
void b_move3(struct w_button *b) { b_move(0,-1);}
void b_move4(struct w_button *b) { b_move(1,-1);}
void b_move5(struct w_button *b) { b_move(0,1);}
void (*b_moveall[6])(struct w_button *b) =
 { b_move0,b_move1,b_move2,b_move3,b_move4,b_move5 };
 
void b_turn0(struct w_button *b) { b_turn(0,1,2,1); }
void b_turn1(struct w_button *b) { b_turn(1,2,0,-1); }
void b_turn2(struct w_button *b) { b_turn(0,1,2,-1); }
void b_turn3(struct w_button *b) { b_turn(2,0,1,-1); }
void b_turn4(struct w_button *b) { b_turn(1,2,0,1); }
void b_turn5(struct w_button *b) { b_turn(2,0,1,1); }
void (*b_turnall[6])(struct w_button *b) =
 { b_turn0,b_turn1,b_turn2,b_turn3,b_turn4,b_turn5 };
 
void b_changelevel(struct w_button *b)
 { 
 struct node *n;
 for(n=view.levels.head;n->next!=NULL;n=n->next)
  if(!strcmp(n->d.lev->fullname,b->d.ls->options[b->d.ls->selected]))
   { changecurrentlevel(n->d.lev); break; }
 if(!n->next) changecurrentlevel(NULL);
 }
 
void b_changepigfile(struct w_button *b)
 {
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 FREE(l->pigname);
 checkmem(l->pigname=MALLOC(strlen(b->d.ls->options[b->d.ls->selected])+5));
 strcpy(l->pigname,b->d.ls->options[b->d.ls->selected]);
 strcat(l->pigname,".256");
 newpigfile(l->pigname,pig.pogfile);
 }

struct w_button *b_movemode,*b_currmode,*b_movebts[6],*b_pigfile,*b_pogfile;

void changepogfile(char *pogfilename)
 {
 char const *pogname;
 FILE *pf=NULL;
 if(!pogfilename || (pf=fopen(pogfilename,"rb"))==NULL)
  pogname=NULL; 
 else
  {
  if((pogname=strrchr(pogfilename,'/'))!=NULL) pogname++;
  else pogname=pogfilename;
  }
 if(pig.current_pogname)
  { FREE(pig.current_pogname); pig.current_pogname=NULL; }
 if(pogname)
  {
  checkmem(pig.current_pogname=MALLOC(strlen(pogname)+1));
  strcpy(pig.current_pogname,pogname);
  }
 if(pogfilename) FREE(pogfilename);
 newpigfile(l ? l->pigname : pig.default_pigname,pf);
 }

void b_changepogfile(struct w_button *b)
 {
 char *pogfilename;
 pogfilename=getfilename(&init.pogpath,pig.current_pogname,"POG",
  "Select pogfile",0);
 if(!pogfilename && !yesnomsg(TXT_DEFAULTPOGFILE)) return;
 changepogfile(pogfilename);
 }

int mbls[6][110]=
 { { 0,9,-8,1,0, 0,8,-9,0,-1, 0,-8,10,-4,-2, 0,-8,10,6,0, 0,-4,-2,6,0,
     0,5,10,10,5, 0,10,5,10,-5, 0,10,-5,5,-10, 0,5,-10,-5,-10, 
     0,-5,-10,-10,-5, 0,-10,-5,-10,4, 0,-10,4,-8,6, 
     0,4,8,8,4, 0,8,4,8,-4, 0,8,-4,4,-8, 0,4,-8,-4,-8,
     0,-4,-8,-8,-4, 0,-8,-4,-8,3, 0,-8,3,-7,4,
     0,0,10,-10,10, 0,0,10,-4,2, 0,-4,2,-10,10 },
   { 0,-1,2,-1,10, 0,1,2,1,10, 0,0,-10,5,2, 0,0,-10,-5,2, 0,5,2,-5,2, 
     0,5,-10,10,-5, 0,10,-5,10,4, 0,10,4,8,6, 
     0,4,-8,8,-4, 0,8,-4,8,3, 0,8,3,7,4,
     0,0,10,10,10, 0,0,10,4,2, 0,4,2,10,10 },
   { 0,-9,8,-1,0, 0,-8,9,0,1, 0,8,-10,4,2, 0,8,-10,-6,0, 0,4,2,-6,0 },
   { 0,2,1,10,1, 0,2,-1,10,-1, 0,-10,0,2,5, 0,-10,0,2,-5, 0,2,5,2,-5 },
   { 0,-1,-2,-1,-10, 0,1,-2,1,-10, 0,0,10,5,-2, 0,0,10,-5,-2, 0,5,-2,-5,-2 },
   { 0,-2,1,-10,1, 0,-2,-1,-10,-1, 0,10,0,-2,5, 0,10,0,-2,-5, 0,-2,5,-2,-5 }};
const char *movemodes[mt_number]={ TXT_MOVEYOU,TXT_MOVEOBJ,TXT_MOVECURRENT,
				   TXT_MOVETXT };
unsigned char *movebuttondata[6];
int movebuttondsize[6];
void drawallbuttons(struct w_window *w)
 {
 static struct w_b_choose modebutton,modifybutton,pigfile;
 static struct w_b_press movebutton[6],pogfile;
 struct w_button *b1,*b2,*b3;
 struct ws_bitmap *bm;
 int i,j,k,bxsize,bysize,y;
 char **pignames;
 modifybutton.num_options=tt_number;
 modifybutton.options=(const char **)init.bnames;
 modifybutton.selected=view.currmode;
 modifybutton.select_lroutine=modifybutton.select_rroutine=b_changemode;
 modebutton.num_options=mt_number;
 modebutton.options=movemodes;
 modebutton.selected=view.movemode;
 modebutton.select_lroutine=modebutton.select_rroutine=b_changemovemode;
 if(init.d_ver>=d2_10_reg)
  { 
  pignames=ws_getallfilenames(init.pigpaths[init.d_ver],"PIG",
   &pigfile.num_options);
  checkmem(pigfile.options=MALLOC(sizeof(char *)*pigfile.num_options));
  pigfile.selected=0;
  for(i=0;i<pigfile.num_options;i++)
   {
   pignames[i][strlen(pignames[i])-4]=0;
   checkmem(pigfile.options[i]=MALLOC(strlen(pignames[i])+1));
   strcpy((char *)pigfile.options[i],pignames[i]); 
   for(j=1;j<strlen(pigfile.options[i]);j++)
    *((char *)pigfile.options[i]+j)=tolower(pigfile.options[i][j]); 
   strcat(pignames[i],".256");
   if(l && !strcmp(pignames[i],l->pigname)) pigfile.selected=i;
   FREE(pignames[i]);
   } 
  FREE(pignames);
  pigfile.select_lroutine=pigfile.select_rroutine=b_changepigfile;
  }
 checkmem(view.b_levels=MALLOC(sizeof(struct w_b_choose)));
 view.b_levels->num_options=1;
 checkmem(view.b_levels->options=MALLOC(sizeof(char *)*1));
 view.b_levels->options[0]=TXT_NONE;
 view.b_levels->selected=0;
 view.b_levels->select_lroutine=view.b_levels->select_rroutine=b_changelevel;
 bxsize=w_xwininsize(w)/3; 
 w_refreshstart(w);
 checkmem(b1=b_currmode=w_addstdbutton(w,w_b_choose,0,y=0,bxsize*3,-1,
  TXT_MODIFYWHAT,&modifybutton,1)); y+=b1->ysize;
 checkmem(b2=b_movemode=w_addstdbutton(w,w_b_choose,0,y,bxsize*3,-1,
  TXT_MOVEMODE,&modebutton,1)); y+=b2->ysize;
 checkmem(view.levelbutton=w_addstdbutton(w,w_b_choose,0,y,bxsize*3,-1,
  TXT_LEVEL,view.b_levels,1)); y+=view.levelbutton->ysize;
 if(init.d_ver>=d2_10_reg)
  {
  checkmem(b3=b_pigfile=w_addstdbutton(w,w_b_choose,0,y,bxsize*3,-1,
   TXT_LEVELPIGFILE,&pigfile,1)); y+=b3->ysize;
  if(init.d_ver>=d2_12_reg)
   {
   pogfile.delay=0; pogfile.repeat=-1;
   pogfile.l_pressed_routine=pogfile.r_pressed_routine=NULL;
   pogfile.l_routine=pogfile.r_routine=b_changepogfile;
   checkmem(b_pogfile=w_addstdbutton(w,w_b_press,0,y,bxsize*3,-1,
    TXT_POGFILE,&pogfile,1)); y+=b_pogfile->ysize;
   }
  }
 bysize=(w_ywininsize(w)-y)/2;
 /* move button menu */
 for(j=5;j<22;j++)
  {
  for(k=1;k<5;k+=2)
   { 
   mbls[2][j*5+k]=-mbls[0][j*5+k];
   mbls[2][j*5+k+1]=mbls[0][j*5+k+1]; 
   mbls[4][j*5+k]=mbls[1][j*5+k];
   mbls[4][j*5+k+1]=mbls[1][j*5+k+1]; 
   mbls[3][j*5+k]=-mbls[1][j*5+k+1];
   mbls[3][j*5+k+1]=-mbls[1][j*5+k];
   mbls[5][j*5+k]=-mbls[3][j*5+k];
   mbls[5][j*5+k+1]=mbls[3][j*5+k+1];
   mbls[1][j*5+k+1]=-mbls[1][j*5+k+1];
   }
  }
 for(i=0;i<6;i++)
  for(j=0;j<22;j++)
   {
   mbls[i][j*5]=view.color[WHITE];
   /* I was too lazy to do this by hand: */    
   for(k=1;k<5;k+=2)
    { mbls[i][j*5+k]+=j<5 ? 35 : i==1 || i==4 ? 4 : 11; 
      mbls[i][j*5+k+1]+=j>=5 && (i==3 || i==5) ? 18 : 11; }
   }
 for(j=0;j<2;j++)
  for(i=0;i<3;i++)
   {
   checkmem(bm=w_createlinesbm(bxsize-2,bysize-2,j*3+i==0||j*3+i==2 ? 22 : 14,
    mbls[j*3+i]));
   movebuttondata[j*3+i]=ws_getbitmapdata(bm);
   movebuttondsize[j*3+i]=bm->xsize*bm->ysize;
   movebutton[j*3+i].l_routine=movebutton[j*3+i].l_pressed_routine=
    b_turnall[j*3+i]; 
   movebutton[j*3+i].r_routine=movebutton[j*3+i].r_pressed_routine=
    b_moveall[j*3+i];
   movebutton[j*3+i].delay=25; movebutton[j*3+i].repeat=5;
   checkmem(b_movebts[j*3+i]=w_addimagebutton(w,w_b_press,i*bxsize,
    j*bysize+y,-1,-1,bm,bm,&movebutton[j*3+i],0,1));
   }
 w_refreshend(w);
 }

void changemovemode(enum movetypes mm)
 {
 my_assert(mm<mt_number);
 w_refreshstart(view.movewindow);
 b_movemode->d.ls->selected=view.movemode=mm;
 w_drawbutton(b_movemode);
 w_refreshend(view.movewindow);
 }
 
void changecurrmode(enum tagtypes tt)
 {
 my_assert(tt<tt_number && b_currmode!=NULL);
 w_refreshstart(view.movewindow);
 b_currmode->d.ls->selected=view.currmode=tt;
 w_drawbutton(b_currmode);
 w_refreshend(view.movewindow);
 }

void changepigfile(char *palname)
 {
 int i;
 if(!b_pigfile) return;
 for(i=0,b_pigfile->d.ls->selected=0;i<b_pigfile->d.ls->num_options;i++)
  if(!compstrs(palname,b_pigfile->d.ls->options[i]))
   { b_pigfile->d.ls->selected=i; w_drawbutton(b_pigfile); break; }     
 }  
 
int cntrlcursor_xsize,cntrlcursor_ysize,altcursor_xsize,altcursor_ysize;
#define CNTRLCURSOR_HOT_X 0
#define CNTRLCURSOR_HOT_Y 0
#define ALTCURSOR_HOT_X 0
#define ALTCURSOR_HOT_Y 0
char *control_cursor;
char *alt_cursor;
int cursor_initialized;
void initpalette(void)
 {
 int i;
 for(i=0;i<USEDCOLORS;i++) view.color[i]=i;
 view.color[BLACK]=w_makecolor(0,0,0); 
 for(i=1;i<=GRAYSCALE;i++) 
  view.color[i]=w_makecolor((125*i)/GRAYSCALE+75,
   (125*i)/GRAYSCALE+75,(125*i)/GRAYSCALE+75); /* walls */
 view.color[THINGCOLORS]=w_makecolor(255,0,0); /* things type robot */
 view.color[1+THINGCOLORS]=w_makecolor(255,255,255);/* things type start */
 view.color[2+THINGCOLORS]=w_makecolor(255,255,0); /* things type item */
 view.color[3+THINGCOLORS]=w_makecolor(255,0,255);/* things type reactor */
 view.color[HILIGHTCOLORS]=w_makecolor(0,255,0);/* hilighted cube/thing */
 view.color[HILIGHTCOLORS+1]=w_makecolor(200,150,50);/* hilighted wall;
                                                     cube for sdoor */
 view.color[HILIGHTCOLORS+2]=w_makecolor(0,0,255); /* tagged cube/thing */
 view.color[HILIGHTCOLORS+3]=w_makecolor(255,0,255); /* hilight for sdoor
                                                   for doors, wall line 1 */
 view.color[HILIGHTCOLORS+4]=w_makecolor(255,255,0); /* wall line 2 */
 view.color[HILIGHTCOLORS+5]=w_makecolor(90,180,120); /* tagged&current */
 view.color[HILIGHTCOLORS+6]=w_makecolor(255,100,100); 
 view.color[HILIGHTCOLORS+7]=w_makecolor(100,100,255); 
 view.color[DOORCOLORS]=w_makecolor(255,100,100); /* blow door */
 view.color[DOORCOLORS+1]=w_makecolor(255,255,255); /* no key door */
 view.color[DOORCOLORS+2]=w_makecolor(100,150,255); /* blue key */
 view.color[DOORCOLORS+3]=w_makecolor(255,0,0); /* red key */
 view.color[DOORCOLORS+4]=w_makecolor(255,255,0); /* yellow key */
 view.color[WHITE]=w_makecolor(255,255,255); /* normal white */
 }
 
void newpalette(unsigned char *palette)
 {
 int i,j,nc;
 long colortable[4];
 FILE *f;
 if(init_test&2) 
  { fprintf(errf,"New palette:\n");
    for(i=0;i<256;i++)
     fprintf(errf," %3d: %3d %3d %3d\n",i,palette[i*3],palette[i*3+1],
      palette[i*3+2]); }
 w_newpalette(palette);
 my_assert((f=fopen(pig.bulbname,"rb"))!=NULL &&
  fread(pig.bulb,1,BULBSIZE*BULBSIZE,f)==BULBSIZE*BULBSIZE);
 fclose(f);
 my_assert((f=fopen(pig.brokenbulbname,"rb"))!=NULL &&
  fread(pig.brokenbulb,1,BULBSIZE*BULBSIZE,f)==BULBSIZE*BULBSIZE);
 fclose(f);
 colortable[0]=0;
 colortable[1]=w_makecolor(100,200,255);
 colortable[2]=w_makecolor(255,255,0);
 for(i=0;i<BULBSIZE*BULBSIZE;i++) pig.bulb[i]=colortable[(int)pig.bulb[i]];
 colortable[0]=0;
 colortable[1]=w_makecolor(75,75,75);
 colortable[2]=w_makecolor(100,200,255);
 colortable[3]=w_makecolor(255,255,255);
 for(i=0;i<BULBSIZE*BULBSIZE;i++)
  pig.brokenbulb[i]=colortable[(int)pig.brokenbulb[i]];
 nc=w_makecolor(255,255,255);
 for(i=0;i<6;i++)
  if(b_movebts[i])
   {
   for(j=0;j<movebuttondsize[i];j++)
    if(movebuttondata[i][j]) movebuttondata[i][j]=nc;
   w_drawbutton(b_movebts[i]);
   }
 initpalette();
 /* redraw the cursors */
 if(cursor_initialized)
  {
  colortable[0]=1; colortable[1]=view.color[DOORCOLORS];
  w_initkbstatcursor(control_cursor,cntrlcursor_xsize,cntrlcursor_ysize,
   CNTRLCURSOR_HOT_X,CNTRLCURSOR_HOT_Y,colortable,ws_ks_ctrl,
   ws_ks_ctrl|ws_ks_alt);
  w_initkbstatcursor(alt_cursor,altcursor_xsize,altcursor_ysize,
   ALTCURSOR_HOT_X,ALTCURSOR_HOT_Y,colortable,ws_ks_alt,
   ws_ks_ctrl|ws_ks_alt);
  }
 plotlevel(); 
 }
 
struct w_window button_win = { 0,481,20,150,135,-1,-1,0,TXT_BUTTONS,0,"",
 wb_drag,wr_normal,NULL,NULL,NULL,NULL };
void initgrph(int showtitle)
 {
 int i;
 FILE *f;
 char *pigname;
 struct ws_event ws;
 struct ws_bitmap *cursor;
 inittimer();
#if defined(GNU_C) && defined(GO32)
 if(showtitle && ws_initgrfx(640,480,256,init.fontname) && titlescreen())
  ws_getevent(&ws,1);
 ws_textmode();
#endif
 if(!w_initwins(init.xres,init.yres,256,init.fontname)) 
  { printf(TXT_CANTINITWINS); my_exit(); }
 if((f=fopen(init.menuname,"r"))==NULL)
  { fprintf(errf,TXT_NOMENUFILE,init.menuname); my_exit(); }
 my_assert(w_initmenu(f,do_event,ec_num_of_codes));
 fclose(f); pigname=pig.current_pigname; pig.current_pigname=NULL;
 newpigfile(pigname,NULL); FREE(pigname);
 /* draw the cursors (init the cursor data) */
 cntrlcursor_ysize=altcursor_ysize=w_titlebarheight();
 cntrlcursor_xsize=(ws_pixstrlen(TXT_TAG)+2+3)/4*4;
 altcursor_xsize=(ws_pixstrlen(TXT_INFO)+2+3)/4*4;
 checkmem(control_cursor=CALLOC(cntrlcursor_xsize,cntrlcursor_ysize));
 checkmem(alt_cursor=CALLOC(altcursor_xsize,altcursor_ysize));
 cursor=ws_createbitmap(cntrlcursor_xsize,cntrlcursor_ysize,control_cursor);
 ws_bmdrawtext(cursor,1,0,cntrlcursor_xsize,TXT_TAG,1,0);
 control_cursor[0]=1; ws_freebitmap(cursor);
 cursor=ws_createbitmap(altcursor_xsize,altcursor_ysize,alt_cursor);
 ws_bmdrawtext(cursor,1,0,altcursor_xsize,TXT_INFO,1,0);
 alt_cursor[0]=1; ws_freebitmap(cursor);
 cursor_initialized=1;
 checkmem(pig.txt_buffer=MALLOC(64*64));
 memset(pig.txt_buffer,0,64*64);
 pig.txt_bm=ws_createbitmap(64,64,pig.txt_buffer);
 checkmem(pig.door_buffer=MALLOC(64*64));
 memset(pig.door_buffer,0,64*64);
 pig.door_bm=ws_createbitmap(64,64,pig.door_buffer);
 checkmem(pig.thing_buffer=MALLOC(64*64));
 memset(pig.thing_buffer,0,64*64);
 pig.thing_bm=ws_createbitmap(64,64,pig.thing_buffer);
 button_win.xpos=w_xmaxwinsize()-button_win.xsize-10;
 button_win.ypos=w_ymaxwinsize()-button_win.ysize-10;
 if(init.d_ver>=d2_10_reg) 
  {
  button_win.ypos-=20; button_win.ysize+=20;
  if(init.d_ver>=d2_12_reg)
   button_win.ypos-=16; button_win.ysize+=16;
  }
 checkmem(view.movewindow=w_openwindow(&button_win));
 drawallbuttons(view.movewindow);
 for(i=0;i<=in_internal;i++) makeoptwindow(i);
 init_txtgrfx(); read_lightsources();
 }
