/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
    
int w_xmaxwinsize(void);
int w_ymaxwinsize(void);
int w_xmaxwininsize(struct w_window *w);
int w_ymaxwininsize(struct w_window *w);
int w_xwininsize(struct w_window *w);
int w_ywininsize(struct w_window *w);
int w_xwinincoord(struct w_window *w,int x);
int w_ywinincoord(struct w_window *w,int y);
void w_winoutsize(struct w_window *w,int xs,int ys);
int w_xscreencoord(struct w_window *w,int x);
int w_yscreencoord(struct w_window *w,int y);
int w_xwinspacecoord(int x);
int w_ywinspacecoord(int y);
int w_xscrspacecoord(int x);
int w_yscrspacecoord(int y);
char *w_copystr(const char *str);
int w_makecolor(int r,int g,int b);
int w_titlebarheight(void);
