/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

void psys_256_plottxt(struct polygon *p,struct render_point *start,
 unsigned long offset,unsigned char *txt_data);
void psys_256_transparent_plottxt(struct polygon *p,struct render_point *start,
 unsigned long offset,unsigned char *txt_data);
void psys_plotline(int o_x1,int o_y1,int o_x2,
 int o_y2,int color,int xor);
void psys_cleararea(int x,int y,int xsize,int ysize);
void psys_copytoscreen(int x,int y,int xpos,int ypos,int xsize,int ysize);
void psys_initdrawbuffer(void);
void psys_inittimer(void);
void psys_releasetimer(void);
unsigned long psys_gettime(void);
#define TIMER_DIGITS_POW_2 10



