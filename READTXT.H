/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void inittxts(void);
int newpigfile(char *pigname,FILE *pogfile);
void readbitmap(char *dest,struct pig_txt *ptxt,struct ham_txt *txt,int dir);
int cmp_txts(const void *t1,const void *t2);
int readcustomtxts(FILE* pg1file);
