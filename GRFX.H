/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void initgrph(void);
void closegrph(void);
void drawmenu(void);
void drawallbuttons(void);
void setcolors(int color256);
int rebuild_screen(int plot_what);
void changeto16(void);
void changeto256(void);

extern int supervga;
