/*  plotsys.c - texture mapping, system dependent routines */
#include "structs.h"
#include "plotdata.h"
#include "plotsys.h"

#include "plotmath.c"

#define LIN_PIXELS 16
#define TRANSPARENT_COLOR 0xfe

static unsigned char *drawbuffer;
struct ws_bitmap *drawbitmap;

#define SYS_LIGHTSCANLINE(NUM_PIXELS,ADD_F1,ADD_F2,ADD_F3) {\
     txt_u=n_txt_u; txt_v=n_txt_v; \
     run_f1+=ADD_F1; f1=1.0/run_f1; \
     run_f2+=ADD_F2; run_f3+=ADD_F3; f2=f1*run_f2; f1=f1*run_f3; \
     n_txt_u=p->a_txt.x[0]+(f1*p->r_txt.x[0])+(f2*p->s_txt.x[0]); \
     n_txt_v=p->a_txt.x[1]+(f1*p->r_txt.x[1])+(f2*p->s_txt.x[1]);\
     add_txt_u=(n_txt_u-txt_u)/(NUM_PIXELS); \
     add_txt_v=(n_txt_v-txt_v)/(NUM_PIXELS); \
     for(cur_pos=cur_line+ps_x,i=(NUM_PIXELS);i>0;i--)\
      {\
      *(cur_pos++)=*(colors+\
       *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE)))); \
      txt_u+=add_txt_u; txt_v+=add_txt_v; light+=d_light;\
      if(light>c_light) { c_light+=d_x; colors+=add_colors; }\
      }   \
     ps_x+=(NUM_PIXELS); \
     }
         
#define SYS_PLOTWITHCOLOR { \
    if(d_x>=LIN_PIXELS>>1) \
     { \
     e_r_ps_x=d_x/LIN_PIXELS*LIN_PIXELS+ps_x; d_x<<=16; \
     while(ps_x<e_r_ps_x) \
      { SYS_LIGHTSCANLINE(LIN_PIXELS,add_f1,add_f2,add_f3) } \
     e_rest=e_ps_x-e_r_ps_x;\
     if(e_rest>=LIN_PIXELS>>1) \
      { SYS_LIGHTSCANLINE(e_rest,er_n_3d.x[0]*(e_rest), \
         er_rXd.x[0]*(e_rest),er_dXs.x[0]*(e_rest)) } \
     else if(e_rest!=0)\
      {\
      txt_u=n_txt_u; txt_v=n_txt_v; \
      for(cur_pos=cur_line+e_r_ps_x,i=e_rest;i>0;i--) \
       { \
       *(cur_pos++)=*(colors+ \
        *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE)))); \
       txt_u+=add_txt_u; txt_v+=add_txt_v; light+=d_light;\
       if(light>c_light) { c_light+=d_x; colors+=add_colors; } \
       } \
      } \
     } \
    else \
     { e_rest=d_x; d_x<<=16; SYS_LIGHTSCANLINE(e_rest, \
        er_n_3d.x[0]*e_rest,er_rXd.x[0]*e_rest,er_dXs.x[0]*e_rest) } \
    }
   
#define SYS_SCANLINE(NUM_PIXELS,ADD_F1,ADD_F2,ADD_F3) {\
     txt_u=n_txt_u; txt_v=n_txt_v; \
     run_f1+=ADD_F1;  f1=1.0/run_f1; \
     run_f2+=ADD_F2; run_f3+=ADD_F3; f2=f1*run_f2; f1=f1*run_f3; \
     n_txt_u=p->a_txt.x[0]+(f1*p->r_txt.x[0])+(f2*p->s_txt.x[0]); \
     n_txt_v=p->a_txt.x[1]+(f1*p->r_txt.x[1])+(f2*p->s_txt.x[1]);\
     add_txt_u=(n_txt_u-txt_u)/(NUM_PIXELS); \
     add_txt_v=(n_txt_v-txt_v)/(NUM_PIXELS); \
     for(cur_pos=cur_line+ps_x,i=(NUM_PIXELS);i>0;i--)\
      {\
      *(cur_pos++)=*(colors+\
       *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE)))); \
      txt_u+=add_txt_u; txt_v+=add_txt_v; \
      }   \
     ps_x+=(NUM_PIXELS); \
     }
         
#define SYS_PLOTWITHOUTCOLOR { \
    if(d_x>=LIN_PIXELS>>1) \
     { \
     e_r_ps_x=d_x/LIN_PIXELS*LIN_PIXELS+ps_x; d_x<<=16; \
     while(ps_x<e_r_ps_x) \
      { SYS_SCANLINE(LIN_PIXELS,add_f1,add_f2,add_f3) } \
     e_rest=e_ps_x-e_r_ps_x;\
     if(e_rest>=LIN_PIXELS>>1) \
      { SYS_SCANLINE(e_rest,er_n_3d.x[0]*(e_rest), \
         er_rXd.x[0]*(e_rest),er_dXs.x[0]*(e_rest)) } \
     else if(e_rest!=0)\
      {\
      txt_u=n_txt_u; txt_v=n_txt_v; \
      for(cur_pos=cur_line+e_r_ps_x,i=e_rest;i>0;i--) \
       { \
       *(cur_pos++)=*(colors+ \
        *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE)))); \
       txt_u+=add_txt_u; txt_v+=add_txt_v; \
       } \
      } \
     } \
    else \
     { SYS_SCANLINE(d_x,er_n_3d.x[0]*d_x,er_rXd.x[0]*d_x,er_dXs.x[0]*d_x) } \
    }
   
   
/* The plottxt functions use the y-coord with negative sign.!!!! */
void psys_plottxt(struct polygon *p,struct render_point *start,
 unsigned long offset,unsigned char *txt_data)
 {
 struct render_point *ppl,*ppr;
  /* current point on the left/right edge */
 struct point d,rXd,dXs,er_n_3d,er_rXd,er_dXs;
 float pl_x0=0.0,pr_x0=0.0,add_pl_x0=0.0,add_pr_x0=0.0;
  /* horizontal start/end and adds if y-coord is increased. */
 float run_line1,run_line2,run_line3,run_f1,run_f2,run_f3,add_f1,add_f2,
  add_f3,f1,f2;
  /* three factors to calculate u/v which are increased (instead
     of multiplied) for better performance */
  /* light, left/right edge light and adds */
 unsigned char *cur_line,*cur_pos;
 unsigned char *colors,*dest;
 long int add_txt_u=0,add_txt_v=0,n_txt_u,n_txt_v,ps_x,c_light,
  ps_y,e_ps_x,e_r_ps_x,e_rest,add_ll=0,add_rl=0,l_light=0,r_light=0,
  d_light,d_x,add_colors;
 register long int txt_u,txt_v,light,i;
 int next_lr,xsize;
 if(start==NULL) return;
 dest=drawbuffer+offset;
 xsize=init.xres;
 SUB_3D(&d,&p->a_3d,&x0);
 VECTOR(&rXd,&p->r_3d,&d);
 VECTOR(&dXs,&d,&p->s_3d);
 for(i=0;i<3;i++)
  { er_n_3d.x[i]=SCALAR_3D(&p->n_3d,&er[i]);
    er_rXd.x[i]=SCALAR_3D(&rXd,&er[i]);
    er_dXs.x[i]=SCALAR_3D(&dXs,&er[i]); }
 add_f1=er_n_3d.x[0]*LIN_PIXELS;
 add_f2=er_rXd.x[0]*LIN_PIXELS;
 add_f3=er_dXs.x[0]*LIN_PIXELS;
 ppl=ppr=start; ps_y=start->x[1];
 next_lr=0;
 cur_line=dest+(-ps_y+max_ycoord)*xsize+max_xcoord;
 run_line1=ps_y*er_n_3d.x[1]+z_dist*er_n_3d.x[2];
 run_line2=ps_y*er_rXd.x[1]+z_dist*er_rXd.x[2];
 run_line3=ps_y*er_dXs.x[1]+z_dist*er_dXs.x[2];
 do
  {
  if(next_lr>=0)
   {
   l_light=ppl->light; pl_x0=ppl->x[0]; /* to avoid rounding errors */
   ppl=ppl->next;
   while(ppl->x[1]==ppl->prev->x[1]) /* horizontal line */
    { if(ppl==ppr) return; pl_x0=ppl->x[0]; l_light=ppl->light;
      ppl=ppl->next; }
   f1=1.0/(ppl->x[1]-ppl->prev->x[1]);
   add_pl_x0=(ppl->x[0]-ppl->prev->x[0])*f1;
   add_ll=(ppl->light-ppl->prev->light)*f1; 
   }
  if(next_lr<=0)
   {
   r_light=ppr->light; pr_x0=ppr->x[0]; /* to avoid rounding errors */
   ppr=ppr->prev;
   while(ppr->x[1]==ppr->next->x[1]) /* horizontal line */
    { if(ppl==ppr) return; pr_x0=ppr->x[0]; r_light=ppr->light;
      ppr=ppr->prev; }
   f1=1.0/(ppr->x[1]-ppr->next->x[1]);
   add_pr_x0=(ppr->x[0]-ppr->next->x[0])*f1;
   add_rl=(ppr->light-ppr->next->light)*f1;
   }
  next_lr=(ppl->x[1]<=ppr->x[1] ? 1 : -1);
  for(;ps_y<round((next_lr>0 ? ppl : ppr)->x[1]);ps_y++,
   cur_line-=xsize,run_line1+=er_n_3d.x[1],run_line2+=er_rXd.x[1],
   run_line3+=er_dXs.x[1],pl_x0+=add_pl_x0,pr_x0+=add_pr_x0,l_light+=add_ll,
   r_light+=add_rl)
   {
   ps_x=round(pl_x0); e_ps_x=round(pr_x0);
   colors=view.lightcolors+(((l_light+0x80)>>8)&0xff00);
   if(e_ps_x>ps_x) 
    {
    d_light=r_light-l_light; d_x=e_ps_x-ps_x;
    light=l_light*d_x; c_light=light+(d_x<<15);
    if(d_light<0) { d_light=-d_light; add_colors=-0x100; }
    else add_colors=0x100;
    run_f1=run_line1+ps_x*er_n_3d.x[0];
    run_f2=run_line2+ps_x*er_rXd.x[0];
    run_f3=run_line3+ps_x*er_dXs.x[0];
    f1=1.0/run_f1; f2=f1*run_f2; f1=f1*run_f3; 
    n_txt_u=p->a_txt.x[0]+(f1*p->r_txt.x[0])+(f2*p->s_txt.x[0]); 
    n_txt_v=p->a_txt.x[1]+(f1*p->r_txt.x[1])+(f2*p->s_txt.x[1]); 
    if(d_light!=0) { SYS_PLOTWITHCOLOR }
    else  { SYS_PLOTWITHOUTCOLOR }
    }
   } 
  }
 while(ppl!=ppr);
 }

#define SYS_TLIGHTSCANLINE(NUM_PIXELS,ADD_F1,ADD_F2,ADD_F3) {\
     txt_u=n_txt_u; txt_v=n_txt_v; \
     run_f1+=ADD_F1; f1=1.0/run_f1; \
     run_f2+=ADD_F2; run_f3+=ADD_F3; f2=f1*run_f2; f1=f1*run_f3; \
     n_txt_u=p->a_txt.x[0]+(f1*p->r_txt.x[0])+(f2*p->s_txt.x[0]); \
     n_txt_v=p->a_txt.x[1]+(f1*p->r_txt.x[1])+(f2*p->s_txt.x[1]);\
     add_txt_u=(n_txt_u-txt_u)/(NUM_PIXELS); \
     add_txt_v=(n_txt_v-txt_v)/(NUM_PIXELS); \
     for(cur_pos=cur_line+ps_x,i=(NUM_PIXELS);i>0;i--)\
      {\
      if((pixel=\
       *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE))))!= \
       TRANSPARENT_COLOR) \
       *(cur_pos++)=*(colors+pixel); \
      else cur_pos++; \
      txt_u+=add_txt_u; txt_v+=add_txt_v; light+=d_light; \
      if(light>c_light) { c_light+=d_x; colors+=add_colors; }\
      }   \
     ps_x+=(NUM_PIXELS); \
     }
         
#define SYS_PLOTTWITHCOLOR { \
    if(d_x>=LIN_PIXELS>>1) \
     { \
     e_r_ps_x=d_x/LIN_PIXELS*LIN_PIXELS+ps_x; d_x<<=16; \
     while(ps_x<e_r_ps_x) \
      { SYS_TLIGHTSCANLINE(LIN_PIXELS,add_f1,add_f2,add_f3) } \
     e_rest=e_ps_x-e_r_ps_x;\
     if(e_rest>=LIN_PIXELS>>1) \
      { SYS_TLIGHTSCANLINE(e_rest,er_n_3d.x[0]*(e_rest), \
         er_rXd.x[0]*(e_rest),er_dXs.x[0]*(e_rest)) } \
     else if(e_rest!=0)\
      {\
      txt_u=n_txt_u; txt_v=n_txt_v; \
      for(cur_pos=cur_line+e_r_ps_x,i=e_rest;i>0;i--) \
       { \
       if((pixel= \
        *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE))))!= \
	TRANSPARENT_COLOR) \
        *(cur_pos++)=*(colors+pixel); \
       else cur_pos++; \
       txt_u+=add_txt_u; txt_v+=add_txt_v; light+=d_light;\
       if(light>c_light) { c_light+=d_x; colors+=add_colors; } \
       } \
      } \
     } \
    else \
     { e_rest=d_x; d_x<<=16; SYS_TLIGHTSCANLINE(e_rest, \
        er_n_3d.x[0]*e_rest,er_rXd.x[0]*e_rest,er_dXs.x[0]*e_rest) } \
    }
   
#define SYS_TSCANLINE(NUM_PIXELS,ADD_F1,ADD_F2,ADD_F3) {\
     txt_u=n_txt_u; txt_v=n_txt_v; \
     run_f1+=ADD_F1; f1=1.0/run_f1; \
     run_f2+=ADD_F2; run_f3+=ADD_F3; f2=f1*run_f2; f1=f1*run_f3; \
     n_txt_u=p->a_txt.x[0]+(f1*p->r_txt.x[0])+(f2*p->s_txt.x[0]); \
     n_txt_v=p->a_txt.x[1]+(f1*p->r_txt.x[1])+(f2*p->s_txt.x[1]);\
     add_txt_u=(n_txt_u-txt_u)/(NUM_PIXELS); \
     add_txt_v=(n_txt_v-txt_v)/(NUM_PIXELS); \
     for(cur_pos=cur_line+ps_x,i=(NUM_PIXELS);i>0;i--)\
      {\
      if((pixel= \
       *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE))))!= \
       TRANSPARENT_COLOR) \
       *(cur_pos++)=*(colors+pixel); \
      else cur_pos++; \
      txt_u+=add_txt_u; txt_v+=add_txt_v; \
      }   \
     ps_x+=(NUM_PIXELS); \
     }
         
#define SYS_PLOTTWITHOUTCOLOR { \
    if(d_x>=LIN_PIXELS>>1) \
     { \
     e_r_ps_x=d_x/LIN_PIXELS*LIN_PIXELS+ps_x; d_x<<=16; \
     while(ps_x<e_r_ps_x) \
      { SYS_TSCANLINE(LIN_PIXELS,add_f1,add_f2,add_f3) } \
     e_rest=e_ps_x-e_r_ps_x;\
     if(e_rest>=LIN_PIXELS>>1) \
      { SYS_TSCANLINE(e_rest,er_n_3d.x[0]*(e_rest), \
         er_rXd.x[0]*(e_rest),er_dXs.x[0]*(e_rest)) } \
     else if(e_rest!=0)\
      {\
      txt_u=n_txt_u; txt_v=n_txt_v; \
      for(cur_pos=cur_line+e_r_ps_x,i=e_rest;i>0;i--) \
       { \
       if((pixel= \
        *(txt_data+((txt_u>>12)&0x3f)+((txt_v>>12)&(0x3f*TXTSIZE)))) \
        !=TRANSPARENT_COLOR) \
        *(cur_pos++)=*(colors+pixel); \
       else cur_pos++; \
       txt_u+=add_txt_u; txt_v+=add_txt_v; \
       } \
      } \
     } \
    else \
     { SYS_TSCANLINE(d_x,er_n_3d.x[0]*d_x,er_rXd.x[0]*d_x,er_dXs.x[0]*d_x) } \
    }
   
   
/* The plottxt functions use the y-coord with negative sign.!!!! */
void psys_plottransparent(struct polygon *p,struct render_point *start,
 unsigned long offset,unsigned char *txt_data)
 {
 struct render_point *ppl,*ppr;
  /* current point on the left/right edge */
 struct point d,rXd,dXs,er_n_3d,er_rXd,er_dXs;
 float pl_x0=0.0,pr_x0=0.0,add_pl_x0=0.0,add_pr_x0=0.0;
  /* horizontal start/end and adds if y-coord is increased. */
 float run_line1,run_line2,run_line3,run_f1,run_f2,run_f3,add_f1,add_f2,
  add_f3,f1,f2;
  /* three factors to calculate u/v which are increased (instead
     of multiplied) for better performance */
  /* light, left/right edge light and adds */
 unsigned char *cur_line,*cur_pos;
 unsigned char *colors,*dest;
 long int add_txt_u=0,add_txt_v=0,n_txt_u,n_txt_v,ps_x,c_light,
  ps_y,e_ps_x,e_r_ps_x,e_rest,add_ll=0,add_rl=0,l_light=0,r_light=0,
  d_light,d_x,add_colors;
 register long int txt_u,txt_v,light,i;
 int next_lr,xsize;
 unsigned char pixel;
 if(start==NULL) return;
 dest=drawbuffer+offset;
 xsize=init.xres;
 SUB_3D(&d,&p->a_3d,&x0);
 VECTOR(&rXd,&p->r_3d,&d);
 VECTOR(&dXs,&d,&p->s_3d);
 for(i=0;i<3;i++)
  { er_n_3d.x[i]=SCALAR_3D(&p->n_3d,&er[i]);
    er_rXd.x[i]=SCALAR_3D(&rXd,&er[i]);
    er_dXs.x[i]=SCALAR_3D(&dXs,&er[i]); }
 add_f1=er_n_3d.x[0]*LIN_PIXELS;
 add_f2=er_rXd.x[0]*LIN_PIXELS;
 add_f3=er_dXs.x[0]*LIN_PIXELS;
 ppl=ppr=start; ps_y=start->x[1];
 next_lr=0;
 cur_line=dest+(-ps_y+max_ycoord)*xsize+max_xcoord;
 run_line1=ps_y*er_n_3d.x[1]+z_dist*er_n_3d.x[2];
 run_line2=ps_y*er_rXd.x[1]+z_dist*er_rXd.x[2];
 run_line3=ps_y*er_dXs.x[1]+z_dist*er_dXs.x[2];
 do
  {
  if(next_lr>=0)
   {
   l_light=ppl->light; pl_x0=ppl->x[0]; /* to avoid rounding errors */
   ppl=ppl->next;
   while(ppl->x[1]==ppl->prev->x[1]) /* horizontal line */
    { if(ppl==ppr) return; pl_x0=ppl->x[0]; l_light=ppl->light;
      ppl=ppl->next; }
   f1=1.0/(ppl->x[1]-ppl->prev->x[1]);
   add_pl_x0=(ppl->x[0]-ppl->prev->x[0])*f1;
   add_ll=(ppl->light-ppl->prev->light)*f1; 
   }
  if(next_lr<=0)
   {
   r_light=ppr->light; pr_x0=ppr->x[0]; /* to avoid rounding errors */
   ppr=ppr->prev;
   while(ppr->x[1]==ppr->next->x[1]) /* horizontal line */
    { if(ppl==ppr) return; pr_x0=ppr->x[0]; r_light=ppr->light;
      ppr=ppr->prev; }
   f1=1.0/(ppr->x[1]-ppr->next->x[1]);
   add_pr_x0=(ppr->x[0]-ppr->next->x[0])*f1;
   add_rl=(ppr->light-ppr->next->light)*f1;
   }
  next_lr=(ppl->x[1]<=ppr->x[1] ? 1 : -1);
  for(;ps_y<round((next_lr>0 ? ppl : ppr)->x[1]);ps_y++,
   cur_line-=xsize,run_line1+=er_n_3d.x[1],run_line2+=er_rXd.x[1],
   run_line3+=er_dXs.x[1],pl_x0+=add_pl_x0,pr_x0+=add_pr_x0,l_light+=add_ll,
   r_light+=add_rl)
   {
   ps_x=round(pl_x0); e_ps_x=round(pr_x0);
   colors=view.lightcolors+((l_light>>8)&0xff00);
   if(e_ps_x>ps_x) 
    {
    d_light=r_light-l_light; d_x=e_ps_x-ps_x;
    light=l_light*d_x; c_light=light+(d_x<<15);
    if(d_light<0) { d_light=-d_light; add_colors=-0x100; }
    else add_colors=0x100;
    run_f1=run_line1+ps_x*er_n_3d.x[0];
    run_f2=run_line2+ps_x*er_rXd.x[0];
    run_f3=run_line3+ps_x*er_dXs.x[0];
    f1=1.0/run_f1; f2=f1*run_f2; f1=f1*run_f3; 
    n_txt_u=p->a_txt.x[0]+(f1*p->r_txt.x[0])+(f2*p->s_txt.x[0]); 
    n_txt_v=p->a_txt.x[1]+(f1*p->r_txt.x[1])+(f2*p->s_txt.x[1]); 
    if(d_light!=0) { SYS_PLOTTWITHCOLOR }
    else  { SYS_PLOTTWITHOUTCOLOR }
    }
   } 
  }
 while(ppl!=ppr);
 }

void psys_copytoscreen(int x1,int y1,int xpos,int ypos,int xsize,int ysize)
 { ws_copybitmap(NULL,x1,y1,drawbitmap,xpos,ypos,xsize,ysize,1); }

void psys_cleararea(int x,int y,int xs,int ys)
 {
 unsigned char *run=drawbuffer+y*init.xres+x;
 int i; for(i=0;i<ys;i++,run+=init.xres) memset(run,0,xs); 
 }
 
void psys_initdrawbuffer(void)
 {
 checkmem(drawbuffer=malloc(init.xres*init.yres));
 checkmem(drawbitmap=ws_createbitmap(init.xres,init.yres,
  (char *)drawbuffer));
 }
 
#define LINE(l_dy,l_add,l_end) \
  { \
  if(l_dy>dx) \
   { \
   if(xor) \
    for(x=0,y=0;x!=l_end;run l_add,x+=dx) \
     { *run^=color; if(x>y) { run+=xsize; y+=l_dy; } } \
   else \
    for(x=0,y=0;x!=l_end;run l_add,x+=dx) \
     { *run=color; if(x>y) { run+=xsize; y+=l_dy; } } \
   } \
  else \
   { \
   if(xor) \
    for(x=0,y=0;y!=l_end;run+=xsize,y+=l_dy) \
     { *run^=color; if(x<y) { run l_add; x+=dx; } } \
   else \
    for(x=0,y=0;y!=l_end;run+=xsize,y+=l_dy) \
     { *run=color; if(x<y) { run l_add; x+=dx; } } \
   } \
  } 
  
void psys_plotline(int o_x1,int o_y1,int o_x2,
 int o_y2,int color,int xor)
 {
 int x,y,dx,dy,x1,y1,x2,y2,end,xsize=init.xres;
 unsigned char *run;
 if(o_y1<o_y2) { x1=o_x1; x2=o_x2; y1=o_y1; y2=o_y2; }
 else if(o_y1>o_y2) { x1=o_x2; x2=o_x1; y1=o_y2; y2=o_y1; }
 else if(o_x1<o_x2) { x1=o_x1; x2=o_x2; y1=y2=o_y1; }
 else { x1=o_x2; x2=o_x1; y1=y2=o_y1; }
 dy=x2-x1; dx=y2-y1;
 run=drawbuffer+y1*xsize+x1;
 end=dy*dx; x=y=0;
 if(dx==0) 
  {
  if(xor) for(x=x1;x<=x2;x++) { *run^=color; run++; } 
  else for(x=x1;x<=x2;x++) { *run=color; run++; } 
  }
 else if(dy==0)
  {
  if(xor) for(y=y1;y<y2;y++) { *run^=color; run+=xsize; }
  else for(y=y1;y<y2;y++) { *run=color; run+=xsize; }
  }
 else if(dy>0) LINE(dy,++,end) else LINE(-dy,--,-end)  
 }
 
