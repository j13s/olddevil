/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
    
int init_prev_no_next(enum infos what);
void nextobject(enum tagtypes tt);
void prevobject(enum tagtypes tt);
void resetsideedge(void);
void b_refreshtagno(enum infos what);

extern struct node *adjust_light_nc;
extern int adjust_light_wall;
