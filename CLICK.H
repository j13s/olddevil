/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

void init_beam(struct point *x0,struct point *v,struct w_window *w,
 int sx,int sy);
float checkpnt(struct point *a,struct point *m,struct point *p);
struct track *check_for_corr(struct w_window *w,int sx,int sy);
void click_in_level(struct w_window *w,struct w_event *we);
