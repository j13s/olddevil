/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
    
void w_clearwindow(struct w_window *w);
void w_drawline(struct w_window *w,int x1,int y1,int x2,int y2,int col,
 int xor);
void w_drawbuttonbox(struct w_window *w,int x,int y,int xs,int ys);
void w_drawclipline(struct w_window *w,int x1,int y1,int x2,int y2,int col,
 int xor);
void w_drawbitmap(struct w_window *w,int xp,int yp,int xs,int ys,
 struct ws_bitmap *bm,int withbg);
void w_drawouttext(struct w_window *w,int x,int y,int width,char *txt); 
void w_wintofront(struct w_window *w);
void w_refreshstart(struct w_window *w);
void w_refreshend(struct w_window *w);
void w_refreshwin(struct w_window *w);
int w_inrefresh(struct w_window *w);
void w_changewintitle(struct w_window *w,char *s);
