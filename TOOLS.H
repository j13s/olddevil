/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de) 
    Further info see .c-files. */
/* io-functions */
int numlockonoff(void);
int drawnumlock(void);
int getnumlock(void);
int fwritestring(FILE *f,char *s);
char *freadstring(FILE *f);
int readmacroheader(FILE *f,struct macro *m);
void drawtexture(int rtxtnum,int w);
void printdatabyte(int row,int column,unsigned char *data,int offset);
void markdatabyte(struct infoitem *i,int byte);
void printmsg(char *txt,...);
void printsmsg(char *t);
/* Functions for changing data. Use getdata only if necessary */
void dosideeffects(struct infoitem *i,unsigned long no);
int getsideno(unsigned int *se,unsigned long *no);
void setsideno(unsigned int *se,unsigned long no);
unsigned char *getdata(enum datastructs it);
int getno(struct infoitem *i,unsigned long *no);
void setno(struct infoitem *i,unsigned long no);
size_t getsize(enum datastructs it,struct thing *t);
struct thing *changething(struct thing *t,int newtype,struct cube *c);
/* list fncts */
void initlist(struct list *l);
void freedoor(void *);
void freecube(void *);
void freething(void *);
void cleanmacro(void *);
void freemacro(void *);
void freelistpnt(void *);
void freenode(struct list *l,struct node *n,void (*freeentry)(void *));
void unlistnode(struct list *l,struct node *n);
void freelist(struct list *l,void (*freeentry)(void *));
struct node *insertnode(struct list *l,struct node *nprev,int no,void *data);
struct node *addnode(struct list *l,int no,void *data);
struct node *addheadnode(struct list *l,int no,void *data);
int copylist(struct list *dl,struct list *sl,size_t size_of_data);
struct node *findnode(struct list *l,int no);
void sortlist(struct list *l,int start);
/* many other functions */
struct infoitem *findinfoitem(enum infos incode,int offset);
void beam(struct point *p);
struct infoitem *findmenuitem(struct infoitem *i,int nextprev);
struct objtype *findotype(struct objdata *od,int n);
void turn(struct point *es,struct point *ee,int i,int j,int k,double angel);
int findthing(int x,int y);
int findcube(int x,int y);
int finddoor(int sx,int sy);
void setthingpts(struct thing *t);
void makemarker(struct point *mp,struct point *np);
struct node *findpntcube(struct list *cubelist,struct point *p);
enum sdoortypes getsdoortype(struct sdoor *sd);
