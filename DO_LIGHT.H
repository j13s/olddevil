/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

void dec_mineillum(int ec);
void dec_setcornerlight(int ec);
void dec_setinnercubelight(int ec);
void dec_setlsfile(int ec);
int read_lightsources(void);
void end_adjustlight(struct wall *w,int save);
void start_adjustlight(struct wall *w);

