/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void fittogrid(struct point *p);
void insertpnt(struct node *c,int pn);
struct wall *insertwall(struct node *cube,int wallnum,int txt1,int txt2,
 int dir);
int insertsdoor(struct node *d);
struct node *insertdoor(struct node *c,int wallnum);
struct node *insertthing(struct node *c,struct node *t);
struct node *insertcube(struct list *cubes,struct list *pts,struct node *c,
 int wallnum,float depth);
struct node *newcube(struct leveldata *ld);
void cleansdoor(struct sdoor *sd);
void killtarget(struct sdoor *sd,int n);
void killsdoorlist(struct list *l,int no);
void deleteconnect(struct node *c,int wallnum,int fast);
void deletesdoor(struct node *n);
void deletedoor(struct node *n);
void deletecube(struct list *cubes,struct list *pts,struct node *n);
void delflickeringlight(struct lightsource *ls);
void freewall(struct leveldata *ld,struct cube *c,int w);
int initcube(struct node *c);
int initdoor(struct node *d);
void makedoorpnt(struct door *d);
int changepnt(struct node *oldp,struct node *newp,struct node *sn);
int connectcubes(struct list *pts,struct node *nc1,int w1,struct node *nc2,
 int w2);
int connectsides(struct node *cube,int wallnum);
int testcube(struct node *nc,int withmsg);
int testpnt(struct node *np);
void growshrink(struct node **ps,int *pntnos,int groworshrink);
int move_pntlist(struct list *l,struct point *r);
void makesidestdshape(struct cube *c,int w);
