/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

#ifndef W_SYSTEM
#define W_SYSTEM
/* Some constants you must modify: */
/* quite clear: */
#define SYS_COMPILER_NAME "GNU-C 2.7.1 with libgrx 2.00beta"
/* if your compiler has problems with reserving more then 64K with the
   normal alloc */
#define MALLOC(a) malloc(a)
#define REALLOC(a,b) realloc(a,b)
#define FREE(b) (free(b),b=NULL) 
#define CALLOC(a,b) calloc(a,b)

/* OK. Now the things, you shouldn't change... */
/* this is the highest bit used in enum kbstat */
#define MAX_KBSTAT 6 
/* the event definitions */
struct ws_event {
 enum flagtypes { ws_f_none,ws_f_lbutton,ws_f_rbutton,ws_f_keypress } flags;
 int  x,y;	/* coordinates of mouse */
 enum buttontypes { ws_bt_none=0,ws_bt_left=1,ws_bt_right=2 } buttons;
  /* button state */
 int  key;	/* key code from keyboard */
 enum kbstattypes { ws_ks_shift=1,ws_ks_alt=2,ws_ks_ctrl=4,ws_ks_numlock=8,
  ws_ks_capslock=16,ws_ks_scrollock=32 } kbstat;  
 };
 
struct ws_bitmap
 {
 int xpos,ypos,xsize,ysize; /* where it comes from */
 struct wi_window *w; /* Pointer to the Window over this bitmap */
 void *bitmap; /* Pointer to whatever you need for saving a bitmap */
 };

typedef void ws_cursor;
 
int ws_initgrfx(int xres,int yres,int colors,const char *fontname);
void ws_resetmousecolors(void);
ws_cursor *ws_initcursor(char *data,int w,int h,int xo,int yo,
 long *colortable);
void ws_changecursor(ws_cursor *cursor);
void ws_killcursor(ws_cursor *cursor);
void ws_setcolor(int i,int r,int g,int b);
void ws_getcolor(int i,int *r,int *g,int *b);
struct ws_bitmap *ws_createbitmap(int xsize,int ysize,char *bm);
unsigned char *ws_getbitmapdata(struct ws_bitmap *b);
void ws_copybitmap(struct ws_bitmap *dst,int x1,int y1,
 struct ws_bitmap *src,int xpos,int ypos,int xsize,int ysize,int withbg);
struct ws_bitmap *ws_savebitmap(struct ws_bitmap *bm,int xpos,int ypos,
 int xsize,int ysize);
void ws_clearbitmap(struct ws_bitmap *b,int c);
void ws_restorebitmap(struct ws_bitmap *bm);
void ws_freebitmap(struct ws_bitmap *bm);
void ws_drawline(int x1,int y1,int x2,int y2,int col,int xor);
void ws_drawcircle(int x,int y,int r,int c,int xor);
void ws_bmdrawline(struct ws_bitmap *bm,int x1,int y1,int x2,int y2,int col,
 int xor);
void ws_drawbox(int x1,int y1,int xsize,int ysize,int col,int xor);
void ws_drawfilledbox(int x1,int y1,int xsize,int ysize,int col,int xor);
void ws_drawframedbox(int x,int y,int xs,int ys,int w,int ltc,int rbc,
 int inc);
void ws_drawpatternedbox(int x1,int y1,int xsize,int ysize,int c);
void ws_setclipping(int x1,int y1,int x2,int y2);
void ws_erasemouse(void);
void ws_displaymouse(void);
void ws_mousewarp(int x,int y);
int ws_pixstrlen(const char *txt);
int ws_charstrlen(int w);
void ws_drawtext(int x,int y,int w,const char *txt,int fg,int bg);
void ws_bmdrawtext(struct ws_bitmap *bm,int x,int y,int w,const char *txt,
 int fg,int bg);
void ws_textmode(void);
int ws_getevent(struct ws_event *se,int wait);
char **ws_getallfilenames(const char *path,const char *ext,int *no);
char **ws_getalldirs(const char *path,const char *ext,int *no);
void ws_makepath(const char *oldpath,char *newpath);
void ws_splitpath(char *fullpath,char *drive,char *path,char *name,
 char *ext);
int ws_matchname(char *wildcard,char *name);
void ws_disablectrlc(void);
int ws_waitforkey(void);
int ws_testforkey(void);
void ws_setdriver(const char *name);
#endif
