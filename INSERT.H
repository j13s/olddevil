/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void fittogrid(struct point *p);
struct wall *insertwall(struct node *cube,int wallnum,int txt2);
int insertsdoor(struct node *d);
struct node *insertdoor(struct node *c,int wallnum,int animtxt,int walltxt);
struct node *insertthing(struct node *c,int t1);
void cleansdoor(struct sdoor *sd);
/* the delete functions do not check if the deleted object is tagged!! */
void killtarget(struct sdoor *sd,int n);
void killsdoorlist(struct list *l,int no);
void deleteconnect(struct node *c,int wallnum);
void deletesdoor(struct node *n);
void deletesingledoor(struct node *n);
void deletedoor(struct node *n);
void deletecube(struct node *n);
void initcube(struct node *c);
void initdoor(struct node *d);
int changepnt(struct node *oldp,struct node *newp,struct node *sn);
int connect(struct node *nc1,int w1,struct node *nc2,int w2);
int connectcubes(struct list *taggedcubelist);
void newwallpnt(struct cube *c,struct wall *w,struct point *np);
void newcubecorners(struct cube *c,int pointnum);
int testpnt(struct node *np);
void newcorners(struct node *np);
void arrangebitmaps(struct node *c,struct wall *w1,struct list *ws);
void growshrink(struct node **ps,int *pntnos,int groworshrink);
int move_pntlist(struct list *l,struct point *r);

