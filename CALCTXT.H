/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void newwall_offset(struct cube *c,struct wall *w,int op0,int op1);
void recalcwall(struct cube *c,int wallno);
void newcubecorners(struct node *c,int pointnum);
void newcorners(struct node *np);
void calc_txtcoords(struct wall *w,int corner,struct point *new_p,
 short int *new_coords);
int findaligncubetoline(int wno,int wp1,int wp2);
int findnbwalltoline(struct node *c,struct cube *nc,
 int wall,int wp1,int wp2);
void arrangebitmaps(struct node *c,struct wall *w1);
int findpath(struct list *dest);
