/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include "w_system.h"
#include "w_list.h"
#include "wins.h"

extern FILE *errf;

struct wi_window
 {
 struct w_window w;
 int oysize; /* old y-size (when window is shrunk) */
 int ixpos,iypos,ixsize,iysize; /* init-position&size */
 int win_contents; /* 1 if contents shown, 0 otherwise */
 int drawn; /* 1 if window is drawn, 0 otherwise */
 int in_refresh; /* window is currently refreshed */
 struct node *n;
 struct ws_bitmap *bg; /* data below this window (if NULL, just the
  background */
 struct ws_bitmap *in; /* data in the window (for shrink) */
 struct list buttonlist; /* buttons in the window */
 };

struct wi_button
 {
 struct w_button b;
 struct node *n;
 int sys_button; /* 1 if system button, 0 if user button */
 int activated; /* 1 if activated, 0 otherwise */
 int drawn; /* 1 if button is on the screen, 0 otherwise */
 int tobechanged; /* 1 if the button can be clicked, 0 otherwise */
 /* the difference between drawn & activated:
  if activated==0 the button is drawn with special marker and can't be
  used but takes the clicking place.
  if drawn==0 the button is not drawn and can't be used and doesn't take
  any clicking place */
 struct ws_bitmap *bm; /* the content of the window */
 };
 
struct wi_menu
 {
 char *txt,*help,hotkey;
 void (*action)(int);
 int actionnr;
 struct list submenus;
 int x1,y1,xs,ys,hklinex,hkliney,hklinexs;
 int level; /* submenu level. 0 is menuline */
 struct ws_bitmap *bm; /* bitmap covered by submenu */
 };
 
struct bounds 
 { int xp,yp,xs,ys; }; /* position & size */
 
struct shapeparameters
 {
 /* Parameters for Windows */
 int titlebar_height,closebutton_width,sizebutton_width;
 /* Parameters for Menus */
 int width_menuframe;
 };

struct kbstatcursor
 {
 enum kbstattypes kbstat,bitmask; /* cursor is activated if
  event.kbstat&bitmask==kbstat */
 ws_cursor *cursor;
 };
 
enum colvalues { cv_bg,cv_winedge,cv_winfill,cv_windrag,
 cv_buttonlt,cv_buttonrb,cv_buttonin,cv_buttonpressed,cv_switchon,
 cv_textfg,cv_textbg,cv_buttonmarker,cv_curwin,cv_curwinlt,cv_curwinrb,
 cv_number };

struct notebook
 {
 int initialized;
 int print_msg;
 int xres,yres,colors;
 int colindex[cv_number];
 struct bounds winspace;
 struct wi_window *cur_win;
 struct list windows;
 struct list menu;
 struct node *cur_but;
 char *titlebar_text;
 struct kbstatcursor *kbstatcursors; 
 int num_kbstatcursors,current_cursor; /* current cursor==-1 for normal c. */
 };
 
extern struct shapeparameters shapes;
extern struct notebook notes;
