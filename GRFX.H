/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void initgrph(int title);
void closegrph(void);
void initdescmap(void);
void changemovemode(enum movetypes mm);
void changecurrmode(enum tagtypes tt);
void changepigfile(char *palname);
void changepogfile(char *pogfilename);
void newpalette(unsigned char *palette);
