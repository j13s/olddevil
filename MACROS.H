/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de) 
    Further info see .c-files. */
int readdescription(char **ltxt);
int readmacro(struct macro *m);
int savemacro(FILE *f,struct macro *m);
struct macro *buildmacro(struct list *l,struct list *tl);
int insertmacro(struct macro *m,int connectnow);
void killmacro(struct macro *m);
