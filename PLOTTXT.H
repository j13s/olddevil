/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void plotline(int o_x1,int o_y1,int o_x2,int o_y2,int color,int xor);
void clearlevelwin(void);
void init_txtgrfx(void);
void render_level(int lr,struct node *start_cube,int withobjects,int depth);
void initfilledside(struct cube *cube,int wall);
