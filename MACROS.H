/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
struct leveldata *buildmacro(struct leveldata *ld);
int insertmacro(struct leveldata *m,int connectnow,float scaling);
void getcubecoords(struct cube *c,int wall,int pnt,struct point *naxis,
 int inout);

