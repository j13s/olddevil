/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void initcoordsystem(int lr,struct point *e0,struct point *er,
 struct point *x0);
void makeview(int lr); 
int getpixelcoords(int lr,struct point *p,struct pixel *pix);
void plot3dline(struct point *sp,struct point *ep,int color,int xor,
 int checkdist);
void plotthing(struct thing *t,int hilight); 
void plotmarker(struct point *p,int hilight);
void plotpnt(struct node *n,int w,int c,int hilight);
void plotcube(struct node *n,int hilight,int xor,int withalllines,
 int withlockedsides);
void plotwall(struct cube *c,int wno,int hilight,int xor);
void plotdoor(struct node *n,int hilight,int xor);
void plotcorridor(struct corridor *c);
void plottagwall(struct cube *c,int wallno,int hilight,int xor);
void plotcurrent(void);
void plotlevel(void);
unsigned long cont_plotlevel(struct lightsource **ls);
void copytoscreen(void);
void initdescmap(void);
void dec_frames(int ec);
void move_user(struct point *new_pos);
