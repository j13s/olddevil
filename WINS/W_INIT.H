/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

int w_initwins(int xres,int yres,int colors,const char *fontname);
void w_newpalette(unsigned char *palette);
int w_initmenu(FILE *mfile,void (**action)(int),int max_anr);
void w_setcolors(int *palette);
struct w_window *w_openwindow(struct w_window *w);
struct ws_bitmap *w_createlinesbm(int xsize,int ysize,int n,int *lines);
void w_buttoninwin(struct w_window *w,struct w_button *b,int draw);
struct w_button *w_addstdbutton(struct w_window *w,enum but_types type,
 int xpos,int ypos,int xsize,int ysize,const char *txt,void *data,int draw);
struct w_button *w_addimagebutton(struct w_window *w,enum but_types type,
 int xpos,int ypos,int xsize,int ysize,struct ws_bitmap *bm,
 struct ws_bitmap *abm,void *data,int with_bg,int draw);
struct w_button *w_addbutton(struct w_button *b,int draw);
void w_deactivatebutton(struct w_button *b);
void w_activatebutton(struct w_button *b);
void w_dontchangebutton(struct w_button *b);
void w_setdrawbutton(struct w_button *b,int drawn);
void w_undrawbutton(struct w_button *b);
void w_drawbutton(struct w_button *b);
void w_deletebutton(struct w_button *b);
void w_shrinkwin(struct w_window *w);
void w_resizewin(struct w_button *b);
void w_resizeinwin(struct w_window *w,int xinsize,int yinsize);
void w_closewindow(struct w_window *w);
void w_closewins(void);
void w_curwin(struct w_window *w);
int w_isthiscurwin(struct w_window *w);
void w_butwin(struct w_window *w);
int w_savecfg(FILE *f);
void w_readcfg(FILE *f);
void w_initkbstatcursor(char *data,int w,int h,int xo,int yo,
 long *colortable,enum kbstattypes kbstat,enum kbstattypes bitmask);
void set_titlebar_text(char const *txt);
