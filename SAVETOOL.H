/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de) 
    Further info see .c-files. */
void *readdoor(FILE *f);
void *readsdoor(FILE *lf);
void *readthing(FILE *lf);
void *readpnt(FILE *lf);
void *readproducer(FILE *lf);
void *readcube(FILE *lf);
struct edoor *readedoors(FILE *f,unsigned long num,unsigned long size);
int savedoor(FILE *f,struct node *d);
int savesdoor(FILE *f,struct node *sd);
int savething(FILE *f,struct node *t);
int saveproducer(FILE *f,struct node *prod);
int savecube(FILE *f,struct node *c);
int savepoint(FILE *f,struct node *p);
int readlist(FILE *f,struct list *l,void *(*readdata)(FILE *),int num);
int savelist(FILE *f,struct list *l,int (*saveproc)(FILE *,struct node *),
 int withnum);
