/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. 
    These are the structure needed for texturemapping. This file
    is included by plotsys.c, plottxt.c, plot.c and grfx.c */
/* #define PLOTTOSCREEN */
#define MAX_RENDERDEPTH 30
#define MAX_RENDERPNTS 30
#define TXTSIZE 64

struct point_2d { float x[2]; };
struct point_txt { int x[2]; };
struct render_point 
 { struct render_point *prev,*next;
   float x[2]; long light; };
struct polygon_point
 { struct polygon_point *prev,*next; /* prev/next pp. This is a 
    counter-clockwise sorted ring of pnts */
   struct point *p_3d; /* point in normal space */
   struct corner *corner; };
struct polygon
 { int num_pnts;
   struct polygon_point pnts[4];
   struct point a_3d,r_3d,s_3d,n_3d; /* polygon plane */
   struct point_2d a_txt,r_txt,s_txt; /* texture plane */ };

extern struct point e0,er[3]; /* e0 and e[3] for current view */
extern float xviewphi,yviewphi,z_dist; /* this is the cos(view angle) */
extern struct point x0; /* x0 viewpoint, m0 line viewpoint-center of screen */
extern int max_xcoord,max_ycoord; /* (scr_xysize-1)/2 */
extern int scr_xsize,scr_ysize; /* always a uneven number */
