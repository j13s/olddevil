/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

int wi_winoverlap(struct wi_window *w1,struct wi_window *w2);
int wi_whereoverlap(struct wi_window *w1,struct wi_window *w2,
 struct bounds *where);
void wi_drawwin(struct wi_window *wi);
void wi_undrawwin(struct wi_window *wi);

