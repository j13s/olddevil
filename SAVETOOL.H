/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void *readdoor(FILE *f);
void *D1_REG_readsdoor(FILE *lf);
void *D2_REG_readsdoor(FILE *lf);
void *readthing(FILE *lf);
void *readpnt(FILE *lf);
void *D1_REG_readproducer(FILE *lf);
void *D2_REG_readproducer(FILE *lf);
void *D1_REG_readcube(FILE *lf);
void *D2_REG_readcube(FILE *lf);
void *readflickeringlight(FILE *lf);
int D2_REG_readcube2(FILE *lf,struct node *c);
void *readchangedlight(FILE *lf);
void *readturnoff(FILE *lf);
struct edoor *readedoors(FILE *f,unsigned long num,unsigned long size);
int savedoor(FILE *f,struct node *d,va_list args);
int savething(FILE *f,struct node *t,va_list args);
int D1_REG_savesdoor(FILE *f,struct node *d,va_list args);
int D1_REG_saveproducer(FILE *f,struct node *p,va_list args);
int saveflickeringlight(FILE *f,struct node *n,va_list args);
int savedata(FILE *f,struct node *data,va_list args);
int D1_REG_savecube(FILE *f,struct node *n,va_list args);
int D2_REG_savecube(FILE *f,struct node *c,va_list args);
int D2_REG_savecube2(FILE *f,struct node *n,va_list args);
int savepoint(FILE *f,struct node *p,va_list args);
int readlist(FILE *f,struct list *l,void *(*readdata)(FILE *),int num);
int savelist(FILE *f,struct list *l,
int (*saveproc)(FILE *,struct node *,va_list args),int withnum,...);
int readasciicube(struct leveldata *ld,FILE *f);
