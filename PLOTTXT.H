/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void plotline(int o_x1,int o_y1,int o_x2,int o_y2,int color,int xor);
void clearlevelwin(void);
void init_txtgrfx(void);
unsigned long render_level(int lr,struct leveldata *ld,struct node *start_cube,
 int drawwhat,int depth);
void render_resetlights(struct leveldata *ld);
void render_enablelights();
void render_disablelights();
void initfilledside(struct cube *cube,int wall);
void inittimer(void);
void releasetimer(void);
