/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
int iniread(FILE *f,const char *s,...);
void skipline(FILE *f);
int findmarker(FILE *f,const char *m,int *number);
void initeditor(const char *fn,int c);
void addcfgpath(char **s);

