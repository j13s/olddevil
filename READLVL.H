/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
struct leveldata *emptylevel(void);
void newlevelwin(struct leveldata *ld,int shrunk);
struct leveldata *openlevel(char *filename);
struct leveldata *readlevel(char *filename);
struct leveldata *readdbbfile(char *filename);
int initlevel(struct leveldata *ld);
int savelevel(char *fname,struct leveldata *ld,int testlevel,int changename,
 int descent_version,int notalllightinfo);
void in_changecurrentlevel(struct leveldata *ld);
void changecurrentlevel(struct leveldata *ld);
int closelevel(struct leveldata *ld,int warn);
