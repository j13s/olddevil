/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de) 
    Further info see .c-files. */
int readlvl(char *name,struct leveldata *l);
int savelvl(FILE *f);
struct texture *readtxts(char *fname);
void initdata(void);
