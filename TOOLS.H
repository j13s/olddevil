/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
/* io-functions */
char *makepath(const char *path,const char *fname);
/* Functions for changing data. Use getdata only if necessary */
struct node *getnode(enum datastructs it);
unsigned char *getdata(enum datastructs it,struct node *n,...);
int getno(struct infoitem *i,void *no,struct node *n,...);
int setno(struct infoitem *i,void *no,struct node *n,...);
size_t getsize(enum datastructs it,struct thing *t);
void makesecretstart(struct leveldata *ld,struct node *secretwall,
 struct node *cube);
struct thing *changething(struct thing *t,struct thing *clone,int newtype,
 struct cube *c);
void set_illum_brightness(float new_value); 
/* list fncts */
void freedoor(void *);
void freecube(void *);
void freething(void *);
void freelistpnt(void *);
void freelightsource(void *);
void freenode(struct list *l,struct node *n,void (*freeentry)(void *));
void freelist(struct list *l,void (*freeentry)(void *));
int copylist(struct list *dl,struct list *sl,size_t size_of_data);
void sortlist(struct list *l,int start);
/* many other functions */
void turn(struct point *es,struct point *ee,int i,int j,int k,float angel);
void setthingcube(struct thing *t);
void setthingpts(struct thing *t);
void makemarker(struct point *mp,struct point *np);
void make_o_marker(struct point *offset,struct point *coords,float size,
 struct point *pnts);
int checkpntcube(struct node *nc,struct point *p);
struct node *findpntcube(struct list *cubelist,struct point *p);
enum sdoortypes getsdoortype(struct sdoor *sd);
void setsdoortargets(struct sdoor *sd);
int qs_compstrs(const void *s1,const void *s2);
int compstrs(const char *s1,const char *s2);
int isbinary(int x);
