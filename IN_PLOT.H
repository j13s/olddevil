/* internal procedures of plot.h. Needed in plottxt.c/click.c */
int pointinsight(struct point *p,struct point *d,float *dist);
int pointinangle(struct point *d,float *dist);
int calchit(float *r,float ax,float ay,float ex,float ey,
 float dx,float dy);
int in_getpixelcoords(struct point *p,struct pixel *pix);
int getscreencoords(int lr,struct point *sp,struct point *ep,
 struct pixel *spix,struct pixel *epix,int checkdist);
void in_plotwall(int w,struct cube *c,int wno,int hilight,int xor);
void in_plottagwall(int w,struct cube *c,int wallno,int hilight,int xor);
void in_plotthing(int w,struct thing *t,int hilight);
void in_plotcube(int w,struct node *c,int hilight,int drawswitches,int xor,
 int withalllines,int withlockedsides);
void in_plotpnt(int w,struct node *c,int wn,int pn,int hilight);
void in_plotdoor(int w,struct node *d,int hilight,int drawswitched,int xor);
void in_plot3dline(int w,struct point *sp,struct point *ep,int color,int xor,
 int checkdist);
void in_plotcorridor(int lr,struct corridor *c);
void makeview(int lr);
