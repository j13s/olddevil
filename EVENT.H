/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de) 
    Further info see .c-files. */
int event(MouseEvent *me);
int readkeyb(char *txt,char *rtype,void *data);
int readltype(void (*dosomething)(int no,int *data),char *txt,
 struct objdata *od,int maxtag,int *tagged,int *data);
