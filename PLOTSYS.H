/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

void psys_plottxt(struct polygon *p,struct render_point *start,
 unsigned long offset,unsigned char *txt_data);
void psys_plottransparent(struct polygon *p,struct render_point *start,
 unsigned long offset,unsigned char *txt_data);
void psys_plotline(int o_x1,int o_y1,int o_x2,
 int o_y2,int color,int xor);
void psys_cleararea(int x,int y,int xsize,int ysize);
void psys_copytoscreen(int x,int y,int xpos,int ypos,int xsize,int ysize);
void psys_initdrawbuffer(void);

