/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    plot.c - all vector drawing routines.
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
#include "system.h"
#include "tools.h"
#include "insert.h"
#include "plot.h"
#define COLORNUM2(d,min) (((d)>=view.maxvisibility) ? (min) : \
   (d<=0.0) ? GRAYSCALE : ((int)((GRAYSCALE-1)*(1-(d)/view.maxvisibility))+1))
#define WCOLORNUM(d,h) view.color[(h)<=0 ? COLORNUM2(d,(h)==0 ? 1 : BLACK) :\
 HILIGHTCOLORS+(h)-1]
#define TCOLORNUM(d,c,h) view.color[((h)==0 || (h)<0) ? \
 (((d)>view.maxvisibility) ? BLACK : (c)+THINGCOLORS) : HILIGHTCOLORS+(h)-1]
int partcolor(struct door *d,double dist,int h)
 {
 int i,key;
 switch(d->type1)
  {
  case 3: case 4: case 5: /* a texture/switch/window */
   return COLORNUM2(dist,h==0 ? 1 : BLACK); 
  case 1: /* blow door */
   return DOORCOLORS;
  case 2: /* normal door */
   for(i=0,key=d->key;key!=0;i++,key=key>>1);
   return i+DOORCOLORS;
  default:
   return WHITE;
  }
 }
#define PCOLORNUM(dist,door,h) view.color[(h)<=0 ? partcolor(door,dist,h) : \
 HILIGHTCOLORS+(h)-1]

int pointinsight(struct point *p,struct point *d,double *dist);
int pointinangle(struct point *d,double *dist);
int pointvisible(struct point *p,double *d);
void calcpoint(struct point *p,struct pixel *pix,struct point *v,
 double xoff,double yoff,double d);
 int calchit(double *r,double ax,double ay,double ex,double ey,
 double dx,double dy);
void plotline(struct pixel *ps,struct pixel *pe,struct point *p1,
 struct point *p2,int color,int xor);
void plotwall(struct cube *c,int wno,int hilight);

void printpos(void)
 {
 char buffer[100];
 sprintf(buffer,"%.1f,%.1f,%.1f",view.e0.x[0]/65536.0,view.e0.x[1]/65536.0,
  view.e0.x[2]/65536.0);
 buffer[view.smenuwidth]=0;
 sys_text(view.bounds[ecw_lowermenu][0],view.bounds[ecw_lowermenu][3]+1,
  buffer,view.color[WHITE],view.color[BLACK]);
 sprintf(buffer,"%5.3f,%5.3f,%5.3f",view.e[2].x[0],view.e[2].x[1],
  view.e[2].x[2]);
 buffer[view.smenuwidth]=0;
 sys_text(view.bounds[ecw_lowermenu][0],view.bounds[ecw_lowermenu][3]+1+
  view.fontheight,buffer,view.color[WHITE],view.color[BLACK]);
 }
 
void makeview(void)
 {
 int i;
 printpos();
 /* Position of viewpoint: */
 for(i=0;i<3;i++)
  {
  /* now from the middle of the screen to behind the screen: */
  view.x0.x[i]=view.e0.x[i]-view.e[2].x[i]*view.dist;
  view.m0.x[i]=view.e0.x[i]-view.x0.x[i];
  }
 view.oldpcurrcube=view.oldpcurrthing=NULL;
 view.oldpcurrpnt=NULL;
 /* calculate x and y view angle (1=sqr(LENGTH(&view.e[i]))) */
 view.xviewphi=view.dist/sqrt(view.dist*view.dist+1);
 view.yviewphi=view.dist/sqrt(view.dist*view.dist+1);
 /* kill oldpicture, one pixel more in every direction because of
    two points thick tagged lines */
 sys_filledbox(view.xoffset-1,view.yoffset-1,view.xsize+view.xoffset+1,
  view.ysize+view.yoffset+1,view.color[BLACK],0);
 }

int pointinsight(struct point *p,struct point *d,double *dist)
 {
 int i;
 for(i=0;i<3;i++)
  d->x[i]=p->x[i]-view.x0.x[i];
 *dist=LENGTH(d);
 return *dist<view.maxvisibility;
 }
 
int pointinangle(struct point *d,double *dist)
 {
 double m0_d;
 if(0.0>(m0_d=SCALAR(d,&view.e[2])))
  { *dist=-*dist; return 0; }
 else
  { return view.xviewphi*(*dist)*view.dist<=m0_d;}
 }
 
int pointvisible(struct point *p,double *dist)
 {
 struct point d;
 int insight=pointinsight(p,&d,dist);
 return pointinangle(&d,dist) && insight;
 }

/* calculates the point of intersection of the line between p and v
 and the screen and saves the result in coordinates of the screen
 in pix. xoff and yoff are the coordinates of the orthogonal projection of v
 on the screen (for the viewpoint this is the middle of the screen 0.5,0.5) 
 dist is the length of viewpoint->screen */
void calcpoint(struct point *p,struct pixel *pix,struct point *viewpoint,
 double xoff,double yoff,double dist)
 {
 int i;
 struct point a,b,d,v;
 double x;
 v=*viewpoint;
 for(i=0;i<3;i++)
  { d.x[i]=p->x[i]-v.x[i]; a.x[i]=v.x[i]-view.e0.x[i]; 
    b.x[i]=p->x[i]-view.e0.x[i]; }
 if(0.0<SCALAR(&b,&view.e[2])*SCALAR(&a,&view.e[2]))
  for(i=0;i<3;i++) /* use mirrored viewpoint */
   { d.x[i]+=2*view.e[2].x[i]*dist; v.x[i]+=2*view.e[2].x[i]*dist; } 
 x=SCALAR(&view.e[2],&d); /* y*x */
 /* d=p-v / p=C / v=F / d=x / e[2]=y */
 for(i=0;i<3;i++)
  {
  a.x[i]=x*view.e[2].x[i]+v.x[i]; /* D=(y*x)*y */
  b.x[i]=p->x[i]-a.x[i]; /* d=C-D */
  }
 pix->x=SCALAR(&b,&view.e[0])*dist/fabs(x)+xoff;
 pix->y=SCALAR(&b,&view.e[1])*dist/fabs(x)+yoff;
 }

/* calculates point of intersection of two lines m1*t1+b1, m2*t2+b2:
    a=b2-b1, e=m1, d=m2 */
int calchit(double *r,double ax,double ay,double ex,double ey,
 double dx,double dy)
 { 
 double D=ey*dx-ex*dy,s;
 if(D==0.0) 
  { *r=0; return 0; } /* no hit at all, lines are parallel */ \
 *r=(ay*dx-ax*dy)/D; s=(ex*ay-ey*ax)/D; 
 return (*r>=0.0 && *r<=1.0) && (s>=0.0 && s<=1.0); 
 }
 
void plotline(struct pixel *ps,struct pixel *pe,struct point *p1,
 struct point *p2,int color,int xor)
 {
 int x1,y1,x2,y2,hit[4],numhits;
 double dx,dy,t[4];
 struct pixel s,e,*pix,pnew;
 struct point *p,d;
 /* clip the line */
 if((ps->x<=0&&pe->x<=0)||(ps->y<=0&&pe->y<=0)||(ps->x>=1&&pe->x>=1)|| 
  (ps->y>=1&&pe->y>=1)||(ps->d<=0&&pe->d<=0))
  return;
 /* first shorten the lines through the screen */
 if(ps->d<0.0&&pe->d>0.0) { p=p2; p2=p1; p1=p; pix=ps; ps=pe; pe=pix; }
 if(ps->d>0.0&&pe->d<0.0)
  { /* now we have a problem: this line is too long */
  /* so we calculate a new one (ooops) */
  /* calculate orthogonal projection of p1 on the screen: */
  for(x1=0;x1<3;x1++)
   d.x[x1]=p1->x[x1]-view.e0.x[x1]; 
  /* calculate point of intersection */
  calcpoint(p2,&pnew,p1,SCALAR(&d,&view.e[0]),SCALAR(&d,&view.e[1]),
   SCALAR(&d,&view.e[2])-view.dist);
  /* now in &pnew is the point of intersection on the screen */
  pnew.d=sqrt(view.dist*view.dist+pnew.x*pnew.x+pnew.y*pnew.y);
  pnew.x-=0.5; pnew.y-=0.5; 
  pe=&pnew;
  /* clip the line */
  if((ps->x<=0&&pe->x<=0)||(ps->y<=0&&pe->y<=0)||(ps->x>=1&&pe->x>=1)|| 
   (ps->y>=1&&pe->y>=1)||(ps->d<=0&&pe->d<=0))
   return;
  }
 if(ps->x>=0&&ps->x<=1&&pe->x>=0&&pe->x<=1&&ps->y>=0&&ps->y<=1&&pe->y>=0&&
  pe->y<=1)
  { s=*ps; e=*pe; }
 else
  { /* now there's need for clipping */
  dx=pe->x-ps->x; dy=pe->y-ps->y; 
  if(dx*dx+dy*dy<=2.0/view.xsize) return; /* too short */
  /* now look at the edge of the screen */
  hit[0]=calchit(&t[0],ps->x,ps->y,1.0,0.0,dx,dy);
  hit[1]=calchit(&t[1],ps->x-1.0,ps->y,0.0,1.0,dx,dy);
  hit[2]=calchit(&t[2],ps->x-1.0,ps->y-1.0,-1.0,0.0,dx,dy);
  hit[3]=calchit(&t[3],ps->x,ps->y-1.0,0.0,-1.0,dx,dy);
  numhits=hit[0]+hit[1]+hit[2]+hit[3];
 if(!numhits) { return; /* line completely not in screen */ }
  else
   {
   if(hit[0]) { s.x=t[0]; s.y=0.0; e=(ps->y<0.0) ? *pe : *ps; }
   else if(hit[1]) { s.x=1.0; s.y=t[1]; e=(ps->x>1.0) ? *pe : *ps; }
   else if(hit[2]) { s.x=1.0-t[2]; s.y=1.0; e=(ps->y>1.0) ? *pe : *ps; }
   else if(hit[3]) { s.x=0.0; s.y=1.0-t[3]; e=(ps->x<0.0) ? *pe : *ps; }
   if(numhits>1)
    {
    if(hit[3]) 
     { if((hit[2]==0||t[2]!=1.0) && (t[0]!=0.0||hit[0]==0))  
        {e.x=0.0; e.y=1.0-t[3];} }
    else if(hit[2]) { if(t[1]!=1.0||hit[1]==0) {e.x=1.0-t[2]; e.y=1.0;} }
    else if(hit[1]) { if(t[0]!=1.0||hit[0]==0) {e.x=1.0; e.y=t[1];} }
    }
   }
  }
 x1=(int)(s.x*(view.xsize-1))+view.xoffset;
 x2=(int)(e.x*(view.xsize-1))+view.xoffset;
 y1=(int)view.ysize-(s.y*(view.ysize-1))+view.yoffset;
 y2=(int)view.ysize-(e.y*(view.ysize-1))+view.yoffset;
 sys_line(x1,y1,x2,y2,color,xor);
/* if(color==view.color[HILIGHTCOLORS+2])
  { / tagged, make one line left and one line right next the real line /
  ty=2*(x2-x1)<(y2-y1) ? 0 : (x2-x1)>0 ? 1 : -1;
  tx=2*(y2-y1)<(x2-x1) ? 0 : (y2-y1)>0 ? 1 : -1;
  sys_line(x1+tx,y1+ty,x2+tx,y2+ty,color,xor);
  sys_line(x1-tx,y1-ty,x2-tx,y2-ty,color,xor); 
  } */
 }

void plot3dline(struct point *sp,struct point *ep,int color,int hilight,
 int xor)
 {
 struct pixel pix[2];
 struct point d;
 int a,b;
 a=pointinsight(sp,&d,&pix[0].d) || hilight!=0;
 a=a&&pointinangle(&d,&pix[0].d);
 b=pointinsight(ep,&d,&pix[1].d) || hilight!=0;
 b=b&&pointinangle(&d,&pix[1].d);
 if(a||b)
  {
  calcpoint(sp,&pix[0],&view.x0,0.5,0.5,view.dist);
  calcpoint(ep,&pix[1],&view.x0,0.5,0.5,view.dist);
  plotline(&pix[0],&pix[1],sp,ep,
   TCOLORNUM((pix[0].d+pix[1].d)/2,color,hilight),xor);
  }
 }
 
void plotgrid(void)
 {
 struct point splot,eplot,p;
 double sizeofgrid,runx,runy;
 int i,n,j;
 if(!view.gridonoff) return;
 p=view.x0;
 fittogrid(&p);
 sizeofgrid=(view.gridlength>=view.maxvisibility/10) ? view.gridlength :
  (int)(view.maxvisibility/10/view.gridlength+0.5)*view.gridlength;
 if(sizeofgrid==0) { printmsg("Gridsize=0???"); return; }
 printmsg("Shown grid %g:Real grid %g = %d:1.",sizeofgrid,view.gridlength,
  (int)(sizeofgrid/view.gridlength));
 n=view.maxvisibility/sizeofgrid+1;
 if(n>view.plottimes) n=view.plottimes;
 fprintf(errf,"Plotting %d times.\n",n); fflush(errf);
 /* filling the whole visible ball&more with lines */
 for(runx=-sizeofgrid*n;runx<=sizeofgrid*n;runx+=sizeofgrid)
  for(runy=-sizeofgrid*n;runy<=sizeofgrid*n;runy+=sizeofgrid)
   for(i=0;i<1;i++)
    {
    splot.x[i]=eplot.x[i]=p.x[i]+runx;
    j=i+1>=3 ? i-2 : i+1; splot.x[j]=eplot.x[j]=p.x[j]+runy;
    j=i+2>=3 ? i-1 : i+2;
    splot.x[j]=p.x[j]-sizeofgrid*n; eplot.x[j]=p.x[j]+sizeofgrid*n;
    plot3dline(&splot,&eplot,0,1,0);  
    }
 fprintf(errf,"End of plotgrid.\n"); fflush(errf);
 }
 
void plotmarker(struct point *p,int hilight,int xor)
 {
 struct point dp[8];
 int i;
 makemarker(p,dp);
 for(i=0;i<8;i+=2)
  plot3dline(&dp[i],&dp[i+1],WHITE,hilight,xor);
 }

void plotdoor(struct node *n,int hilight)
 {
 struct door *d=n->d.d;
 struct pixel pix[4],ep;
 struct point *p[4];
 struct node *sdn;
 int j,visible=0;
 for(j=0;j<4;j++)
  { 
  calcpoint(p[j]=d->w->p[j]->d.p,&pix[j],&view.x0,0.5,0.5,view.dist);
  if(pointvisible(p[j],&pix[j].d) || hilight!=0) visible=1;
  }
 calcpoint(&d->p,&ep,&view.x0,0.5,0.5,view.dist);
 if(pointvisible(&d->p,&ep.d) || hilight!=0) visible=1;
 if(visible)
  {
  plotline(&pix[0],&ep,p[0],&d->p,PCOLORNUM((pix[0].d+ep.d)/2,d,hilight),0);
  plotline(&pix[1],&ep,p[1],&d->p,PCOLORNUM((pix[1].d+ep.d)/2,d,hilight),0);
  plotline(&pix[2],&ep,p[2],&d->p,PCOLORNUM((pix[2].d+ep.d)/2,d,hilight),0);
  plotline(&pix[3],&ep,p[3],&d->p,PCOLORNUM((pix[3].d+ep.d)/2,d,hilight),0);
  }
 if(d->sd!=NULL && (hilight==1 || hilight==-1))
  {
  if(getsdoortype(d->sd->d.sd)==sdtype_door)
   for(j=0;j<d->sd->d.sd->num;j++) 
    plotdoor(d->sd->d.sd->target[j],(hilight==1) ? 4 : -2);
  else
   for(j=0;j<d->sd->d.sd->num;j++) 
    plotcube(d->sd->d.sd->target[j],(hilight==1) ? 2 : -2);
  }
 if(hilight==1 || hilight==-1)
  for(sdn=d->sdoors.head;sdn->next!=NULL;sdn=sdn->next)
   plotdoor(sdn->d.n,(hilight==1) ? 4 : -2); 
 }
  
void plotwall(struct cube *c,int wno,int hilight)
 {
 struct pixel pix[4];
 struct point *p[4];
 int j;
 for(j=0;j<4;j++)
  { 
  calcpoint(p[j]=c->p[wallpts[wno][j]]->d.p,&pix[j],&view.x0,0.5,0.5,
   view.dist);
  pointvisible(p[j],&pix[j].d);
  }
 for(j=0;j<4;j++)
  plotline(&pix[j],&pix[j==3?0:j+1],p[j],p[j==3?0:j+1],
   WCOLORNUM(0.0,((j==0||j==3) && hilight==2) ? (j==0) ? 4 : 5 : hilight),0);
 }  
   
void plotpnt(struct cube *c,int wn,int pn,int hilight)
 {
 struct point p;
 int i;
 for(i=0;i<3;i++)
  p.x[i]=c->p[wallpts[wn][(pn+1)&0x3]]->d.p->x[i]+
   c->p[wallpts[wn][(pn-1)&0x3]]->d.p->x[i]-
   2*c->p[wallpts[wn][pn]]->d.p->x[i];
 normalize(&p);
 for(i=0;i<3;i++)
  p.x[i]=c->p[wallpts[wn][pn]]->d.p->x[i]+p.x[i]*view.tsize*3;
 plot3dline(c->p[wallpts[wn][pn]]->d.p,&p,WHITE,hilight,0);
 }

void plotcurrent(void)
 {
 struct node *n;
 if(!view.pcurrcube) return;
 if(view.oldpcurrdoor)
  plotdoor(view.oldpcurrdoor,-1);
 if(view.oldpcurrthing)
  plotthing(view.oldpcurrthing,-1);
 if(view.oldpcurrpnt)
  plotmarker(view.oldpcurrpnt,2,1);
 if(view.oldpcurrcube) 
  plotcube(view.oldpcurrcube,-1); 
 /* now plot tagged things */
 switch(view.currmode)
  {
  case tt_cube: for(n=view.tagged[tt_cube].head;n->next!=NULL;n=n->next)
   plotcube(n->d.n,3); break;
  case tt_wall: for(n=view.tagged[tt_wall].head;n->next!=NULL;n=n->next)
   plotwall(n->d.n->d.c,n->no%6,3); break;
  case tt_pnt: for(n=view.tagged[tt_pnt].head;n->next!=NULL;n=n->next)
   plotpnt(n->d.n->d.c,(n->no%24)/4,(n->no%24)%4,3);
   break;
  case tt_thing: for(n=view.tagged[tt_thing].head;n->next!=NULL;n=n->next)
   plotthing(n->d.n,3); break;
  case tt_door:  for(n=view.tagged[tt_door].head;n->next!=NULL;n=n->next)
   plotdoor(n->d.n,3); break;
  default: fprintf(errf,"Unknown tagtype: %d\n",view.currmode); 
  }
 if(view.pcurrthing)
  {
  plotthing(view.pcurrthing,1); 
  view.oldpcurrthing=view.pcurrthing;
  }
 if(view.pcurrdoor)
  {
  view.oldpcurrdoor=view.pcurrdoor;
  plotdoor(view.pcurrdoor,1); 
  }
 if(view.pcurrcube==NULL) return;
 plotcube(view.pcurrcube,1);
 plotwall(view.pcurrcube->d.c,view.currwall,2);
 if(view.exitcube)
  plotwall(view.exitcube->d.c,view.exitwall,1);
 view.oldpcurrpnt=
  view.pcurrcube->d.c->p[wallpts[view.currwall][view.currpnt]]->d.p;
 plotmarker(view.oldpcurrpnt,2,1);
 }
 
void plotcube(struct node *n,int hilight)
 {
 struct pixel pix[8];
 unsigned short int j,visible=0,next;
 struct cube *c=n->d.c;
 struct point p;
 struct node *sdn;
 if(hilight==1 || hilight==-1)
  for(sdn=c->sdoors.head;sdn->next!=NULL;sdn=sdn->next)
   plotdoor(sdn->d.n,(hilight==1) ? 4 : -2);
 for(j=0;j<8;j++)
  { 
  if(pointinsight(c->p[j]->d.p,&p,&pix[j].d) || hilight!=0)
   if(pointinangle(&p,&pix[j].d))
    visible|=(0x1<<j);
  }
 next=0xffff;
 if(!visible) return;
 if(hilight==0)
  {
  for(j=0;j<6;j++)
   if(c->nc[j]!=NULL && hilight==0)
    {
    if(((view.drawwhat&DW_ALLLINES)==0) || c->nc[j]->no<n->no)
     switch(j)
      {
      case 0 : next&=~(0x4|0x40|0x400|0x800); break;
      case 1 : next&=~(0x8|0x80|0x100|0x800); break;
      case 2 : next&=~(0x1|0x10|0x100|0x200); break;
      case 3 : next&=~(0x2|0x20|0x200|0x400); break;
      case 4 : next&=~(0x10|0x20|0x40|0x80); break;
      case 5 : next&=~(0x1|0x2|0x4|0x8); break;
      }  
    }
  if(next==0) return;
  }
 for(j=0;j<8;j++)
  calcpoint(c->p[j]->d.p,&pix[j],&view.x0,0.5,0.5,view.dist);  
 if(visible&0x0f)
  for(j=0;j<4;j++)
   if((next&(1<<j))!=0)
    plotline(&pix[j],&pix[j==3?0:(j+1)],c->p[j]->d.p,c->p[j==3?0:j+1]->d.p,
     WCOLORNUM((pix[j].d+pix[j==3?0:(j+1)].d)/2,hilight),0);
 if(visible&0xf0)
  for(j=4;j<8;j++)
   if((next&(1<<j))!=0)
    plotline(&pix[j],&pix[j==7?4:(j+1)],c->p[j]->d.p,c->p[j==7?4:j+1]->d.p,
     WCOLORNUM((pix[j].d+pix[j==7?4:(j+1)].d)/2,hilight),0);
 for(j=0;j<4;j++)
  if((next&(0x100<<j))!=0)
   plotline(&pix[j],&pix[j+4],c->p[j]->d.p,c->p[j+4]->d.p,
    WCOLORNUM((pix[j].d+pix[j+4].d)/2,hilight),0);
 }

void plotthing(struct node *n,int hilight)
 {
 double dist;
 int i;
 struct thing *t=n->d.t;
 struct point p;
 if(hilight==0 && (!pointinsight(&t->p[0],&p,&dist)||!pointinangle(&p,&dist)))
  return;
 for(i=0;i<10;i+=2) /* items are only 4 lines but who cares */
  plot3dline(&t->p[i+1],&t->p[i+2],t->color,hilight,0);
 }

