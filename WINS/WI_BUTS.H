/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

int wi_drawbutton(struct wi_button *b,int highlighted);
void wi_undrawbutton(struct wi_button *b);
int (*wi_buttonhandling[w_b_number])(struct ws_event *ws,
 struct wi_button *b);
void wi_changecurbutton(struct node *n);
int wi_handlestringbutton(struct ws_event *ws,struct wi_button *b,int p,
 int leftright);
int wi_choosecoords(struct wi_button *b,int *xp,int *yp,int *xs,int *ys,
 int *collength,int *colwidth);
