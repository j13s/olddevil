/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
int tag(enum tagtypes tt,struct node *data,...);
int tagall(enum tagtypes tt);
int untag(enum tagtypes tt,struct node *n,...);
int untagall(enum tagtypes tt);
int testtag(enum tagtypes tt,struct node *n,...);
void tagobject(enum infos what);
void tagallobjects(enum infos what);
void tagfilter(struct infoitem *i,int op,void *data);
void mousetagbox(struct leveldata *ld,struct w_event *we,int op);
void switch_tag(enum infos what,struct node *n,...);
int tagflatsides(struct node *cube,int w);
