/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    plottxt.c - texture mapping 
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program (file COPYING); if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include "structs.h"
#include "plotdata.h"
#include "plotsys.h"   
#include "readtxt.h"
#include "userio.h"
#include "in_plot.h"
#include "plottxt.h"

#include "lac_cfg.h"

#define DEBUG 0

#include "plotmath.c"

void inittimer(void) { psys_inittimer(); }
void releasetimer(void) { psys_releasetimer(); }

void plottxt(int lr,struct polygon *p,struct render_point *start,
 unsigned char *txt,int transparent)
 {
 if(transparent) 
  psys_256_transparent_plottxt(p,start,w_ywinincoord(l->w,0)*init.xres+
   w_xwinincoord(l->w,lr ? w_xwininsize(l->w)/2+1 : 0),txt);
 else 
  psys_256_plottxt(p,start,w_ywinincoord(l->w,0)*init.xres+
   w_xwinincoord(l->w,lr ? w_xwininsize(l->w)/2+1 : 0),txt); 
 } 

void plotline(int o_x1,int o_y1,int o_x2,int o_y2,int color,int xor)
 {
 psys_plotline(w_xwinincoord(l->w,o_x1),w_ywinincoord(l->w,o_y1),
  w_xwinincoord(l->w,o_x2),w_ywinincoord(l->w,o_y2),color,xor);
 }

void pol_init_rscoords(struct polygon *p)
 {
 int i;
 float ls,lr,f;
 struct point_2d u,v;
 p->a_3d=*p->pnts[0].p_3d; 
 SUB_3D(&p->r_3d,p->pnts[1].p_3d,p->pnts[0].p_3d);
 SUB_3D(&p->s_3d,p->pnts[p->num_pnts-1].p_3d,p->pnts[0].p_3d);
 lr=sqrt((float)SCALAR_3D(&p->r_3d,&p->r_3d));
 for(i=0;i<3;i++) p->r_3d.x[i]/=lr;
 f=SCALAR_3D(&p->s_3d,&p->r_3d);
 for(i=0;i<3;i++) p->s_3d.x[i]-=f*p->r_3d.x[i];
 ls=sqrt((float)SCALAR_3D(&p->s_3d,&p->s_3d)); 
 for(i=0;i<3;i++) p->s_3d.x[i]/=ls;
 VECTOR(&p->n_3d,&p->r_3d,&p->s_3d);
 if(p->pnts[0].corner!=NULL)
  {
  for(i=0;i<2;i++) 
   { p->a_txt.x[i]=p->pnts[0].corner->x[i]<<3;
     u.x[i]=(p->pnts[1].corner->x[i]<<3)-p->a_txt.x[i];
     v.x[i]=(p->pnts[p->num_pnts-1].corner->x[i]<<3)-p->a_txt.x[i]; }
  /* we need to stretch r_txt and s_txt */
  for(i=0;i<2;i++) 
   { p->r_txt.x[i]=-u.x[i]/lr;
     p->s_txt.x[i]=-(v.x[i]-u.x[i]/lr*f)/ls; }
  p->a_txt.x[1]*=TXTSIZE; p->r_txt.x[1]*=TXTSIZE; p->s_txt.x[1]*=TXTSIZE;
  }
 /* init the list so that the points are ordered counter-clockwise
  if you look from the frontside. The pnts in the array must be
  in the right order (they are sorted this way in Descent) */   
 for(i=0;i<p->num_pnts;i++)
  { p->pnts[i].next=&p->pnts[i+1>=p->num_pnts ? 0 : i+1];
    p->pnts[i].prev=&p->pnts[i-1<0 ? p->num_pnts-1 : i-1]; }
 }

#define ZERO 1e-5

/* clip the line sp/ep at the line sb/eb (the inside of the polygon must
 be at the rightside of the line) Return a 1 if a piece of the line is in the
 boundaries, a 0 if the whole line is out of bounds.
 Return in f1 the factor: sp->sp+(ep-sp)*f1
 and in f2 ep->ep-(ep-sp)*f2. Don't change sp,ep. */
int clipline(struct point_2d *sp,struct point_2d *ep,
 struct point_2d *sb,struct point_2d *eb,float *e_f1,float *e_f2)
 {
 struct point_2d ds,de,m1,m2;
 float det,m1_ds,m1_de;
 *e_f1=0.0; *e_f2=0.0;
 SUB_2D(&m2,ep,sp); SUB_2D(&m1,eb,sb); SUB_2D(&ds,sp,sb); SUB_2D(&de,ep,sb);
 det=m2.x[0]*m1.x[1]-m2.x[1]*m1.x[0]; 
 m1_ds=ds.x[1]*m1.x[0]-ds.x[0]*m1.x[1]; 
 m1_de=de.x[1]*m1.x[0]-de.x[0]*m1.x[1]; 
 if(DEBUG>2) fprintf(errf,"Checking boundary: %g %g -> %g %g\n"\
  "m1 %g %g m2 %g %g det %g m1_ds %g m1_de %g\n",
  sb->x[0],sb->x[1],eb->x[0],eb->x[1],m1.x[0],m1.x[1],m2.x[0],m2.x[1],
  det,m1_ds,m1_de);
 if(m1_ds>ZERO) /* start outside */
  { if(m1_de<-ZERO) /* end inside */ { *e_f1=m1_ds/det; return 1; }
    else /* end on border or outside */ return 0; }
 else if(m1_ds<-ZERO) /* start inside */
  { if(m1_de<-ZERO) /* both pnts inside */ return 1;
    else /* end on border or outside */ { *e_f2=1.0-m1_ds/det; return 1; } }
 else /* start on border */
  { if(m1_de>ZERO) /* end outside */ return 0;
    else if(m1_de<-ZERO) /* end inside */ { *e_f1=m1_ds/det; return 2; }
    else /* end on border */ return 2; }
     /* the startpnt must be inserted, but we can't calculate e_f1 properly 
        because det maybe 0 */ 
 }
 
/* Calculate screen coords of the polygon p, clip it and return the
 point with the lowest y-coord. Store the new points in render_pnts.
 Render_pnts is of size MAX_RENDERPNTS */
struct render_point *pol_clip_pnts(struct polygon *p,
 struct render_point *render_pnts,struct render_point *bounds,
 int xsize,int ysize)
 {
 float f1,f2,xs=xsize/2.0,ys=ysize/2.0,v_s0[MAX_RENDERPNTS],
  v_s1[MAX_RENDERPNTS],v_s2[MAX_RENDERPNTS];
 long ls,le;
 struct point v_p[MAX_RENDERPNTS];
 struct render_point *min_rp,*rp,*first_rp,*bp;
 struct polygon_point *pp;
 struct point_2d sp,ep,b_sp,b_ep;
 int i,vis,num_rp,o_num_rp,cur_rp,old_i,clip_status;
 my_assert(MAX_RENDERPNTS>=p->num_pnts);
 /* first we do volume clipping against the viewplane */
 for(i=0,vis=0,pp=&p->pnts[0],rp=render_pnts;i<p->num_pnts;i++,pp=pp->next,
  rp++) 
  {
  SUB_3D(&v_p[i],pp->p_3d,&x0); v_s2[i]=SCALAR_3D(&v_p[i],&er[2]);
  if(v_s2[i]>z_dist) vis=1;
  rp->prev=rp-1; rp->next=rp+1;
  }
 if(!vis) return NULL;
 for(i=0;i<p->num_pnts;i++) 
  { v_s0[i]=SCALAR_3D(&v_p[i],&er[0]); v_s1[i]=SCALAR_3D(&v_p[i],&er[1]); }
 num_rp=cur_rp=p->num_pnts; first_rp=render_pnts;
 render_pnts[0].prev=&render_pnts[num_rp-1];
 render_pnts[num_rp-1].next=&render_pnts[0];
 /* clip polygon against viewplane and calculate screencoords p->p_s */
 old_i=p->num_pnts-1;
 for(i=0,pp=&p->pnts[0],rp=render_pnts,vis=0;i<p->num_pnts;
  i++,pp=pp->next,rp=rp->next) 
  { 
  if(DEBUG>1) fprintf(errf,
   "Clip 3D %d (of %d no %ld prev %ld next %ld):\n"\
   "  dist=%g sp=%g %g %g ep=%g %g %g\n",i,
   p->num_pnts,rp-render_pnts,rp->prev-render_pnts,rp->next-render_pnts,
   z_dist,v_s0[old_i],v_s1[old_i],v_s2[old_i],v_s0[i],v_s1[i],v_s2[i]);
  if(v_s2[i]<=z_dist && v_s2[old_i]<=z_dist)
   { rp->prev->next=rp->next; rp->next->prev=rp->prev; num_rp--;
     if(rp==first_rp) first_rp=rp->next;
     if(DEBUG>1) fprintf(errf,"Killed rp %d: %g %g\n",i,v_s0[i],v_s1[i]); }
  else /* at least one point is before the plane */
   {
   if(v_s2[old_i]<=z_dist)
    /* clip start point against viewplane */
    {
    /* Insert a new point */
    my_assert(cur_rp<MAX_RENDERPNTS);
    render_pnts[cur_rp].prev=rp->prev;
    render_pnts[cur_rp].next=rp; rp=&render_pnts[cur_rp++]; num_rp++;
    rp->prev->next=rp; rp->next->prev=rp;
    f1=(z_dist-v_s2[old_i])/(v_s2[i]-v_s2[old_i]);
    rp->x[0]=v_s0[old_i]+(v_s0[i]-v_s0[old_i])*f1;
    rp->x[1]=v_s1[old_i]+(v_s1[i]-v_s1[old_i])*f1;
    if(pp->corner)
     rp->light=(((long)pp->corner->light+(pp->corner->light-
      pp->prev->corner->light))<<6)*f1-1;
    else rp->light=0;
    if(DEBUG>1) fprintf(errf,"Inserted rp %d (%d) %g: %g %g %lx\n",i,cur_rp-1,
     f1,rp->x[0],rp->x[1],rp->light);
    if(fabs(rp->x[0])<xs || fabs(rp->x[1]<ys)) vis=1;
    rp=rp->next;
    }
   if(v_s2[i]<=z_dist)
    { /* clip the endpoint against the viewplane */
    f1=(z_dist-v_s2[i])/(v_s2[old_i]-v_s2[i]);
    rp->x[0]=v_s0[i]+(v_s0[old_i]-v_s0[i])*f1;
    rp->x[1]=v_s1[i]+(v_s1[old_i]-v_s1[i])*f1;
    if(pp->corner)
     rp->light=(((long)pp->prev->corner->light+
      (pp->prev->corner->light-pp->corner->light))<<6)*f1-1;
    else rp->light=0;
    if(DEBUG>1) fprintf(errf,"Changed rp %d (%d) %g: %g %g %lx\n",i,cur_rp-1,
     f1,rp->x[0],rp->x[1],rp->light);
    if(fabs(rp->x[0])<xs || fabs(rp->x[1]<ys)) vis=1;
    }
   else
    { rp->x[0]=z_dist*v_s0[i]/v_s2[i];
      rp->x[1]=z_dist*v_s1[i]/v_s2[i];
      if(pp->corner) rp->light=((long)pp->corner->light<<6)-1;
      else rp->light=0;
      if(DEBUG>1) fprintf(errf,"Calc rp %d (%d): %g %g %lx\n",i,cur_rp-1,
       rp->x[0],rp->x[1],rp->light);
      if(fabs(rp->x[0])<xs || fabs(rp->x[1]<ys)) vis=1; }
   }
   old_i=i;
  }
 if(!vis || num_rp<3) return NULL;
 /* clip all points on 2d-bounds */
 bp=bounds;
 do
  {
  if(DEBUG>1) fprintf(errf,"Next bounds: %g %g -> %g %g\n",
   bp->prev->x[0],bp->prev->x[1],bp->x[0],bp->x[1]);
  b_sp.x[0]=bp->prev->x[0]; b_sp.x[1]=bp->prev->x[1];
  b_ep.x[0]=bp->x[0]; b_ep.x[1]=bp->x[1];
  sp.x[0]=first_rp->prev->x[0]; sp.x[1]=first_rp->prev->x[1];
  ls=first_rp->prev->light;
  o_num_rp=num_rp;
  for(rp=first_rp,i=0;i<o_num_rp;i++,rp=rp->next) 
   { 
   ep.x[0]=rp->x[0]; ep.x[1]=rp->x[1]; le=rp->light;
   if(DEBUG>1) fprintf(errf,
    "Clip l %d (of %d no %ld prev %ld next %ld): sp=%g %g ep=%g %g\n",
    i,num_rp,rp-render_pnts,rp->prev-render_pnts,rp->next-render_pnts,
    sp.x[0],sp.x[1],ep.x[0],ep.x[1]); 
   if((clip_status=clipline(&sp,&ep,&b_sp,&b_ep,&f1,&f2))!=0)
    { /* something of the line is on the screen */
    if(DEBUG>1) fprintf(errf," f1 %g f2 %g\n",f1,f2);
    if(f1!=0.0 || clip_status==2)
     {
     my_assert(cur_rp<MAX_RENDERPNTS);
     render_pnts[cur_rp].prev=rp->prev;
     render_pnts[cur_rp].next=rp; rp=&render_pnts[cur_rp++]; num_rp++;
     rp->prev->next=rp; rp->next->prev=rp;
     rp->x[0]=sp.x[0]+f1*(ep.x[0]-sp.x[0]);
     rp->x[1]=sp.x[1]+f1*(ep.x[1]-sp.x[1]);
     rp->light=(long)ls+((long)le-ls)*f1;
     if(DEBUG>1) fprintf(errf,"Inserted rp %d (%d): %g %g %lx\n",i,cur_rp-1,
      rp->x[0],rp->x[1],rp->light);
     rp=rp->next;
     }
    if(f2!=0.0)
     { 
     rp->x[0]-=f2*(ep.x[0]-sp.x[0]);
     rp->x[1]-=f2*(ep.x[1]-sp.x[1]);
     rp->light-=((long)le-ls)*f2; 
     if(DEBUG>1) fprintf(errf,"Changed rp %d: %g %g %lx\n",i,rp->x[0],
      rp->x[1],rp->light);
     }
    }
   else /* line is completely outside */
    { rp->prev->next=rp->next; rp->next->prev=rp->prev; num_rp--;
      if(first_rp==rp) first_rp=rp->next;
      if(DEBUG>1) fprintf(errf,"Killed rp %d: %g %g %lx\n",i,rp->x[0],
       rp->x[1],rp->light); }
   sp=ep; ls=le; 
   }
  bp=bp->next;
  }
 while(bp!=bounds);
 if(num_rp>=3)
  {
  /* kill unnecessary points  */
  rp=min_rp=first_rp; o_num_rp=num_rp;
  for(i=0;i<o_num_rp;i++)
   {
   if(DEBUG>1) fprintf(errf,
    "Clipping result all %d no %ld prev %ld next %ld): %g %g %lx\n",
    num_rp,rp-render_pnts,rp->prev-render_pnts,rp->next-render_pnts,
    rp->x[0],rp->x[1],rp->light);
   if(fabs(rp->x[0]-rp->prev->x[0])<0.1 && 
    fabs(rp->x[1]-rp->prev->x[1])<0.1)
    { if(DEBUG>1) fprintf(errf," Killing this point: num_rp=%d\n",num_rp-1);
      if(--num_rp<3) return NULL;
      rp->prev->next=rp->next;
      rp->next->prev=rp->prev;
      if(rp==first_rp) { first_rp=min_rp=rp->prev; } }
   else if(rp->x[1]<min_rp->x[1]) min_rp=rp; 
   rp=rp->next;
   } 
  return min_rp;
  }
 else return NULL;
 }
 
void clearlevelwin(void)
 {
 psys_cleararea(w_xwinincoord(l->w,0),w_ywinincoord(l->w,0),
  w_xwininsize(l->w),w_ywininsize(l->w));
 }

void render_filled_polygon(int lr,struct polygon *p,struct render_point *rp,
 unsigned char *texture,int transparent,int sublight)
 {
 int i,j;
 struct point d; 
 struct render_point *db_rp;
 long maxlight;
 if(SCALAR(&p->n_3d,&er[2])>=(xviewphi>yviewphi ? xviewphi : yviewphi)) 
  return;
 for(i=0;i<p->num_pnts;i++)
  {
  for(j=0;j<3;j++) d.x[j]=p->pnts[i].p_3d->x[j]-x0.x[j];
  if(SCALAR(&p->n_3d,&d)<-LENGTH(&d)*0.01) break;
  } 
 if(i==p->num_pnts) return;
 maxlight=31+sublight;
 if(maxlight<0) maxlight=0;
 else maxlight=(maxlight<<16)+0xffff;
 rp->x[0]=round(rp->x[0]);
 rp->x[1]=round(rp->x[1]);
 rp->light=(rp->light*(NUM_LIGHTCOLORS-1-(view.gamma_corr>>10)))/
  (NUM_LIGHTCOLORS-1)-0x8000;
 if(rp->light<0) rp->light=0;
 if(rp->light>maxlight) rp->light=maxlight;
 rp->light=(((int)view.gamma_corr<<6)&0x1f0000)+(rp->light&0x1fffff);
 if(rp->light>maxlight) rp->light=maxlight;
 db_rp=rp->next;
 do
  {
  db_rp->x[0]=round(db_rp->x[0]);
  db_rp->x[1]=round(db_rp->x[1]); 
  if(db_rp->x[0]==db_rp->prev->x[0] &&
   db_rp->x[1]==db_rp->prev->x[1])
   { db_rp->prev->next=db_rp->next;
     db_rp->next->prev=db_rp->prev;
     if(rp==db_rp) rp=(db_rp->prev->x[1]<=db_rp->next->x[1] ?
      db_rp->prev : db_rp->next); }
  else
   {
   db_rp->light=(db_rp->light*(NUM_LIGHTCOLORS-1-
    (view.gamma_corr>>10)))/(NUM_LIGHTCOLORS-1)-0x8000;
   if(db_rp->light<0) db_rp->light=0;
   if(db_rp->light>maxlight) db_rp->light=maxlight;
   db_rp->light=(((int)view.gamma_corr<<6)&0x1f0000)+(db_rp->light&0x1fffff);
   if(db_rp->light>maxlight) db_rp->light=maxlight;
   }
  db_rp=db_rp->next; 
  }
 while(db_rp!=rp);
 if(rp->next==rp->prev) return;
 if(DEBUG)
  {
  fprintf(errf,"Plotting:\n");
  db_rp=rp;
  do
   { fprintf(errf," %g,%g,%lx",db_rp->x[0],db_rp->x[1],db_rp->light);
     my_assert(db_rp->x[0]>=-max_xcoord && db_rp->x[0]<=max_xcoord
      && db_rp->x[1]>=-max_ycoord && db_rp->x[1]<=max_ycoord);
     db_rp=db_rp->next; }
  while(db_rp!=rp);
  fprintf(errf,"\n"); fflush(errf);
  }
 plottxt(lr,p,rp,texture,transparent); 
 }

void initfilledside(struct cube *cube,int wall)
 {
 int i,j;
 struct point d,r,s,n,*p[4];
 float tt;
 struct wall *w=cube->walls[wall];
 struct polygon *p1=NULL,*p2=NULL;
 cube->recalc_polygons[wall]=0;
 for(i=0;i<4;i++) p[i]=cube->p[wallpts[wall][i]]->d.p;
 /* check if the side is in one plane */
 for(j=0;j<3;j++) 
  { d.x[j]=p[2]->x[j]-p[0]->x[j];
    r.x[j]=p[1]->x[j]-p[0]->x[j];
    s.x[j]=p[3]->x[j]-p[0]->x[j]; }
 VECTOR(&n,&r,&s);    
 tt=SCALAR(&d,&n)/LENGTH(&d)/LENGTH(&n);
 if(fabs(tt)<0.0001)
  { /* render the side with four points */
  if((p1=cube->polygons[wall*2])==NULL)
   checkmem(cube->polygons[wall*2]=p1=MALLOC(sizeof(struct polygon)));
  FREE(cube->polygons[wall*2+1]);
  cube->polygons[wall*2+1]=NULL;
  p1->num_pnts=4; 
  for(i=0;i<4;i++) 
   { p1->pnts[i].p_3d=p[i];
     p1->pnts[i].corner=w ? &w->corners[i] : NULL; }
  pol_init_rscoords(p1);
  }
 else if(tt>0.0)
  { /* render the side as two triangles 3,0,1 / 1,2,3 */
  if((p1=cube->polygons[wall*2])==NULL)
   checkmem(cube->polygons[wall*2]=p1=MALLOC(sizeof(struct polygon)));
  if((p2=cube->polygons[wall*2+1])==NULL)
   checkmem(cube->polygons[wall*2+1]=p2=MALLOC(sizeof(struct polygon)));
  p1->num_pnts=p2->num_pnts=3; 
  for(i=0;i<3;i++) 
   { p1->pnts[i].p_3d=p[(i-1)&3];
     p1->pnts[i].corner=w ? &w->corners[(i-1)&3] : NULL; }
  for(i=0;i<3;i++) 
   { p2->pnts[i].p_3d=p[i+1];
     p2->pnts[i].corner=w ? &w->corners[i+1] : NULL; }
  pol_init_rscoords(p1); pol_init_rscoords(p2);
  }
 else 
  { /* render the side as two triangles 0,1,2 / 2,3,0 */
  if((p1=cube->polygons[wall*2])==NULL)
   checkmem(cube->polygons[wall*2]=p1=MALLOC(sizeof(struct polygon)));
  if((p2=cube->polygons[wall*2+1])==NULL)
   checkmem(cube->polygons[wall*2+1]=p2=MALLOC(sizeof(struct polygon)));
  p1->num_pnts=p2->num_pnts=3; 
  for(i=0;i<3;i++) 
   { p1->pnts[i].p_3d=p[i];
     p1->pnts[i].corner=w ? &w->corners[i] : NULL; }
  for(i=0;i<3;i++) 
   { p2->pnts[i].p_3d=p[(i+2)&3];
     p2->pnts[i].corner=w ? &w->corners[(i+2)&3] : NULL; }
  pol_init_rscoords(p1); pol_init_rscoords(p2);
  }
 }

unsigned char *gettexture(int rdlno,char txt1or2)
 {
 static unsigned char unknown_t1[64*64],unknown_t2[64*64];
 int i;
 if(rdlno<0 || rdlno>=pig.num_rdltxts || pig.rdl_txts[rdlno].pig==NULL)
  {
  if(txt1or2==1)
   { memset(unknown_t1,view.color[HILIGHTCOLORS+1],64*64); return unknown_t1; }
  else
   { memset(unknown_t2,0xff,64*64);
     for(i=0;i<64;i++)
      unknown_t2[i*64+31]=unknown_t2[i*64+31]=
       unknown_t2[31*64+i]=unknown_t2[32*64+i]=view.color[HILIGHTCOLORS];
      return unknown_t2; }
  }
 if(pig.rdl_txts[rdlno].pig->data==NULL)
  {
  checkmem(pig.rdl_txts[rdlno].pig->data=MALLOC(64*64));
  memset(pig.rdl_txts[rdlno].pig->data,txt1or2 == 1 ? 0xfe : 0xff,64*64);
  readbitmap((char *)pig.rdl_txts[rdlno].pig->data,NULL,
   &pig.rdl_txts[rdlno],0);
  }
 return pig.rdl_txts[rdlno].pig->data;
 }
 
static int lightsenabled=0;
void render_enablelights(void) { lightsenabled=1; }
void render_disablelights(void)
 { lightsenabled=0; render_resetlights(l); }

/* I think I have some trouble with too big stacks, so I make as much 
 variables as possible static and/or global */
static struct render_point render_pnts[MAX_RENDERDEPTH][MAX_RENDERPNTS];
static unsigned char buffer[64*64];
static int renderdepth=MAX_RENDERDEPTH,render_drawwhat,render_lr;
static unsigned long timestamp;
void render_cube(int depth,struct node *from,struct node *cube,
 struct render_point *bounds,int sublight)
 {
 unsigned int w,j,x,y;
 static struct render_point *render_start;
 static struct render_point *rp;
 static struct point_2d m1,m2;
 static unsigned char *txt1,*txt2,*txt; /* static because I need to 
  save mem on the stack */
 struct node *n,*ne;
 static struct wall *wall;
 static long overflow;
 static int curpos;
 if(depth>=renderdepth) return;
 if(view.blinkinglightson && lightsenabled)
  {
  for(n=cube->d.c->fl_lights.head;n->next!=NULL;n=n->next)
   {
   if(!n->d.fl->calculated)
    {
    n->d.fl->calculated=1;
    curpos=(unsigned long)(timestamp/n->d.fl->delay)&0x1f;
    j=((n->d.fl->mask>>curpos)&1);
    if(j!=n->d.fl->state)
     {
     n->d.fl->state=j;
     if(j)
      {
      for(ne=n->d.fl->ls->d.ls->effects.head;ne->next!=NULL;ne=ne->next)
       for(w=0;w<6;w++) if((wall=ne->d.lse->cube->d.c->walls[w])!=NULL)
        for(x=0;x<4;x++)
	 {
	 overflow=(long)wall->corners[x].light+ne->d.lse->add_light[w*4+x];
         wall->corners[x].light=overflow>theMaxLight ? theMaxLight : overflow;
	 }
      }
     else
      {
      for(ne=n->d.fl->ls->d.ls->effects.head;ne->next!=NULL;ne=ne->next)
       for(w=0;w<6;w++) if((wall=ne->d.lse->cube->d.c->walls[w])!=NULL)
        for(x=0;x<4;x++)
	 {
	 overflow=(long)wall->corners[x].light-ne->d.lse->add_light[w*4+x];
	 wall->corners[x].light=overflow<0 ? 0 : overflow;
	 }
      }
     }
    }
   }
  }
 for(w=0;w<6;w++) 
  {
  if(!cube->d.c->polygons[w*2] || cube->d.c->recalc_polygons[w])
   initfilledside(cube->d.c,w);
  if(cube->d.c->nc[w]!=NULL && cube->d.c->nc[w]!=from)
   for(j=0;j<2;j++) 
    {
    if(DEBUG) { fprintf(errf,"** %d Clipping&Recursion %d %d %d (%p)\n",depth,
     cube->no,w,j,cube->d.c->polygons[w*2+j]); fflush(errf); }
    if(cube->d.c->polygons[w*2+j] &&
     (render_start=pol_clip_pnts(cube->d.c->polygons[w*2+j],
      render_pnts[depth],bounds,scr_xsize,scr_ysize))!=NULL)
     {
     /* Eliminate parallel pnts */
     rp=render_start;
     if(DEBUG) fprintf(errf,"Recursion:\n");
     do
      {
      m1.x[0]=rp->next->x[0]-rp->x[0]; m1.x[1]=rp->next->x[1]-rp->x[1];
      m2.x[0]=rp->prev->x[0]-rp->x[0]; m2.x[1]=rp->prev->x[1]-rp->x[1];
      if(DEBUG) fprintf(errf," %g,%g,%g,%g",rp->x[0],rp->x[1],
       SCALAR_2D(&m1,&m2)*SCALAR_2D(&m1,&m2),
       SCALAR_2D(&m1,&m1)*SCALAR_2D(&m2,&m2));
      if(SCALAR_2D(&m1,&m2)*SCALAR_2D(&m1,&m2)>=
       SCALAR_2D(&m1,&m1)*SCALAR_2D(&m2,&m2)*0.999999)
       { /* pnt is useless, it lies on the line from prev to next */
       if(DEBUG) fprintf(errf," killed");
       rp->prev->next=rp->next;
       rp->next->prev=rp->prev;
       if(rp==render_start) render_start=rp->prev; 
       }
      rp=rp->next; 
      }
     while(rp!=render_start);
     if(DEBUG) { fprintf(errf,"\n"); fflush(errf); }
     if(render_start->next!=render_start->prev) 
      render_cube(depth+1,cube,cube->d.c->nc[w],render_start,
       cube->d.c->d[w]!=NULL && cube->d.c->d[w]->d.d->type1==door1_cloaked
       ? sublight-(31-cube->d.c->d[w]->d.d->cloaking) : sublight);
     }
    }
  if(cube->d.c->walls[w] && (!cube->d.c->d[w] ||
   (cube->d.c->d[w]->d.d->type1!=door1_onlyswitch &&
   cube->d.c->d[w]->d.d->type1!=door1_cloaked)))
   {
   if(cube->d.c->walls[w]->texture2!=0) 
    { 
    txt=buffer;
    txt1=gettexture(cube->d.c->walls[w]->texture1,1);
    txt2=gettexture(cube->d.c->walls[w]->texture2,2);
    switch(cube->d.c->walls[w]->txt2_direction)
     {
     case 0:
      for(y=0;y<64;y++) for(x=0;x<64;x++)
       txt[y*64+x]=(txt2[y*64+x]>=0xff ? txt1[y*64+x] : txt2[y*64+x]);
      break; 
     case 1:
      for(y=0;y<64;y++) for(x=0;x<64;x++)
       txt[y*64+x]=(txt2[x*64+63-y]>=0xff ? txt1[y*64+x] : txt2[x*64+63-y]);
      break;
     case 2:
      for(y=0;y<64;y++) for(x=0;x<64;x++)
       txt[y*64+x]=(txt2[(63-y)*64+63-x]>=0xff ? txt1[y*64+x] :
        txt2[(63-y)*64+63-x]);
      break;
     case 3:
      for(y=0;y<64;y++) for(x=0;x<64;x++)
       txt[y*64+x]=(txt2[(63-x)*64+y]>=0xff ? txt1[y*64+x] : 
        txt2[(63-x)*64+y]);
      break; 
     default: my_assert(0);
     }
    }
   else txt=gettexture(cube->d.c->walls[w]->texture1,1);
   for(j=0;j<2;j++)
    {
    if(DEBUG)
     { fprintf(errf,"** %d Clipping&Plotting %d %d %d (%p)\n",depth,
     cube->no,w,j,cube->d.c->polygons[w*2+j]); fflush(errf); }
    if(cube->d.c->polygons[w*2+j] &&
     (render_start=pol_clip_pnts(cube->d.c->polygons[w*2+j],
     render_pnts[depth],bounds,scr_xsize,scr_ysize))!=NULL)
     render_filled_polygon(render_lr,cube->d.c->polygons[w*2+j],
       render_start,txt,cube->d.c->nc[w]!=NULL,sublight); 
    }
   }
  }
 /* Now check if we should draw more than the textures */
 if((render_drawwhat&DW_CUBES)!=0) in_plotcube(render_lr,cube,0,0,0,1,1);
 if((render_drawwhat&DW_DOORS)!=0) 
  for(w=0;w<6;w++)
   if(cube->d.c->d[w]) in_plotdoor(render_lr,cube->d.c->d[w],0,0,0);
 if((render_drawwhat&DW_THINGS)!=0) 
  for(n=cube->d.c->things.head;n->next!=NULL;n=n->next)
   in_plotthing(render_lr,n->d.t,0);
 }

void render_resetlights(struct leveldata *ld)
 {
 struct node *n,*ne;
 int w,c;
 long overflow;
 struct flickering_light *fl;
 for(n=ld->lightsources.head;n->next!=NULL;n=n->next) if(n->d.ls->fl!=NULL)
  {
  fl=n->d.ls->fl;
  if(!fl->state)
   {
   fl->state=1;
   for(ne=n->d.ls->effects.head;ne->next!=NULL;ne=ne->next)
    {
    my_assert(ne->d.lse->cube!=NULL);
    for(w=0;w<6;w++) if(ne->d.lse->cube->d.c->walls[w]) for(c=0;c<4;c++)
     {
     overflow=(long)ne->d.lse->cube->d.c->walls[w]->corners[c].light+
      ne->d.lse->add_light[w*4+c];
     ne->d.lse->cube->d.c->walls[w]->corners[c].light=overflow>theMaxLight ?
      theMaxLight : overflow;
     }
    }
   }
  }
 }

unsigned long render_level(int lr,struct leveldata *ld,struct node *start_cube,
 int drawwhat,int depth)
 {
 struct render_point screen_bounds[4];
 int i;
 if(start_cube==NULL) return 0;
 renderdepth=depth<1 ? MAX_RENDERDEPTH : depth;
 if(DEBUG) fprintf(errf,"\n\n*******RENDER LEVEL START********\n\n");
 timestamp=psys_gettime()<<(16-TIMER_DIGITS_POW_2);
 screen_bounds[0].x[0]=-max_xcoord;
 screen_bounds[0].x[1]=-max_ycoord;
 screen_bounds[0].light=0.0;
 screen_bounds[1].x[0]=-max_xcoord;
 screen_bounds[1].x[1]=max_ycoord;
 screen_bounds[1].light=0.0;
 screen_bounds[2].x[0]=max_xcoord;
 screen_bounds[2].x[1]=max_ycoord;
 screen_bounds[2].light=0.0;
 screen_bounds[3].x[0]=max_xcoord;
 screen_bounds[3].x[1]=-max_ycoord;
 screen_bounds[3].light=0.0;
 for(i=0;i<4;i++)
  { screen_bounds[i].prev=&screen_bounds[i==0 ? 3 : i-1];
    screen_bounds[i].next=&screen_bounds[i==3 ? 0 : i+1]; }
 render_drawwhat=drawwhat; render_lr=lr;
 render_cube(0,start_cube,start_cube,screen_bounds,0);
 return (psys_gettime()<<(16-TIMER_DIGITS_POW_2))-timestamp;
 }
 
void init_txtgrfx(void) { psys_initdrawbuffer();  }
