/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_move.c - included in "do_event.c". all functions for moving/rotating.
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
    
int makedirection(enum evcodes ec,struct point *e,struct point *r)
 {
 int x,i; 
 struct point *p;
 switch(ec)
  {
  case ec_pmoveforw: p=&e[2]; x=1; break;
  case ec_pmoveback: p=&e[2]; x=-1; break;
  case ec_pmoveright: p=&e[0]; x=1; break;
  case ec_pmoveleft: p=&e[0]; x=-1; break;
  case ec_pmoveup: p=&e[1]; x=1; break;
  case ec_pmovedown: p=&e[1]; x=-1; break;
  default: fprintf(errf,"Unknown event code in move pnt.\n"); return 0; break;
  }
 for(i=0;i<3;i++) r->x[i]=x*p->x[i]*view.pmovefactor;
 return 1;
 }
 
int pm_thing(struct list *l,enum evcodes ec)
 {
 struct point r;
 struct node *sn,*n;
 struct point coords[3];
 int i,j;
 for(sn=l->head;sn->next!=NULL;sn=sn->next)
  {
  n=sn->d.n;
  if(n->d.t->type1==2 || n->d.t->type1==4 || n->d.t->type1==9)
   {
   for(i=0;i<3;i++)
    for(j=0;j<3;j++)
     coords[i].x[j]=n->d.t->orientation[i*3+j]/65536.0;
   if(!makedirection(ec,coords,&r)) return 1;
   }
  else
   if(!makedirection(ec,view.e,&r)) return 1;
  for(i=0;i<3;i++)
   n->d.t->p[0].x[i]+=r.x[i];
  setthingpts(n->d.t);
  }
 return 6;
 }

int move_pnts(struct list *l,enum evcodes ec)
 {
 struct point r;
 if(!makedirection(ec,view.e,&r)) return 1;
 return move_pntlist(l,&r);
 }
 
int pm_pnt(struct list *l,enum evcodes ec)
 {
 struct list lp;
 struct node *n,*np;
 int j;
 /* Build a list of points to move with move_pnts */
 initlist(&lp);
 for(n=l->head;n->next!=NULL;n=n->next)
   /* hope you don't wanna move too many cubes. I think there
      must be a better solution than this "findnode" */
   if(!findnode(&lp,
    (np=n->d.n->d.c->p[wallpts[(n->no%24)/4][(n->no%24)%4]])->no))
    addnode(&lp,np->no,np);
 j=(lp.size>0) ? move_pnts(&lp,ec) : 1;
 freelist(&lp,NULL);
 return j;
 }
 
int pm_cube(struct list *l,enum evcodes ec)
 {
 struct list lp;
 struct node *n;
 int j;
 /* Build a list of points to move with move_pnts */
 initlist(&lp);
 for(n=l->head;n->next!=NULL;n=n->next)
  for(j=0;j<8;j++)
   /* hope you don't wanna move too many cubes. I think there
      must be a better solution than this "findnode" */
   if(!findnode(&lp,n->d.n->d.c->p[j]->no))
    addnode(&lp,n->d.n->d.c->p[j]->no,n->d.n->d.c->p[j]);
 j=(lp.size>0) ? move_pnts(&lp,ec) : 1;
 freelist(&lp,NULL);
 return j;
 }
 
int pm_wall(struct list *l,enum evcodes ec)
 {
 struct list lp;
 struct node *n;
 int j;
 /* Build a list of points to move with move_pnts */
 initlist(&lp);
 for(n=l->head;n->next!=NULL;n=n->next)
  for(j=0;j<4;j++)
   /* hope you don't wanna move too many walls. I think there
      must be a better solution than this "findnode" */
   if(!findnode(&lp,n->d.n->d.c->p[wallpts[n->no%6][j]]->no))
    addnode(&lp,n->d.n->d.c->p[wallpts[n->no%6][j]]->no,
     n->d.n->d.c->p[wallpts[n->no%6][j]]); 
 j=(lp.size>0) ? move_pnts(&lp,ec) : 1;
 freelist(&lp,NULL);
 return j;
 }

int (*pmove[tt_number])(struct list *l,enum evcodes ec) =
 { pm_cube,pm_wall,pm_pnt,pm_thing };

int maketurndir(enum evcodes ec,struct point *oc,struct point *nc)
 {
 int a0,a1,a2;
 double x;
 switch(ec)
  {
  case ec_pbankleft: a0=0;a1=1;a2=2;x=view.protangle; break;
  case ec_pbankright: a0=0;a1=1;a2=2;x=-view.protangle; break; 
  case ec_pturnleft: a0=2;a1=0;a2=1;x=-view.protangle; break;
  case ec_pturnright: a0=2;a1=0;a2=1;x=view.protangle; break;
  case ec_pturnup: a0=1;a1=2;a2=0;x=-view.protangle; break;
  case ec_pturndown: a0=1;a1=2;a2=0;x=view.protangle; break;
  default: fprintf(errf,"Unknown event code in pturn.\n"); return 0; break;
  }
 turn(oc,nc,a0,a1,a2,x);
 return 1;
 } 
  
int pr_thing(struct list *nl,enum evcodes ec)
 {
 int k,l;
 struct point coords[3];
 struct node *n,*sn;
 for(sn=nl->head;sn->next!=NULL;sn=sn->next)
  {
  n=sn->d.n;
  if(n->d.t->type1!=2 && n->d.t->type1!=4 && n->d.t->type1!=9) continue;
  for(k=0;k<3;k++)
   for(l=0;l<3;l++)
    coords[k].x[l]=n->d.t->orientation[k*3+l]/65536.0;
  maketurndir(ec,coords,coords);
  for(k=0;k<3;k++)
   for(l=0;l<3;l++)
    n->d.t->orientation[k*3+l]=coords[k].x[l]*65536;
  setthingpts(n->d.t);
  }
 return 6;
 }

int turningcoords(struct point *offset,struct point *coords)
 {
 int k,l;
 struct cube *c;
 double x;
 /* rotate around the center of the current cube. z-axis goes in positive
    direction through the center of the current wall, x-axis&y-axis
    through the center of the walls with the current point */
 c=view.pcurrcube->d.c;
 for(l=0;l<3;l++) 
  {
  offset->x[l]=0;
  for(k=0;k<8;k++)
   offset->x[l]+=c->p[k]->d.p->x[l]/8.0;
  }
 for(l=0;l<3;l++)
  {
  coords[2].x[l]=-offset->x[l];
  for(k=0;k<4;k++) 
   coords[2].x[l]+=c->p[wallpts[view.currwall][k]]->d.p->x[l]/4.0;
  }
 normalize(&coords[2]);
 for(l=0;l<3;l++)
  coords[0].x[l]=c->p[wallpts[view.currwall][1]]->d.p->x[l]-
   c->p[wallpts[view.currwall][0]]->d.p->x[l];
 x=SCALAR(&coords[0],&coords[2])/LENGTH(&coords[2]);
 for(l=0;l<3;l++)
  coords[0].x[l]-=x*coords[2].x[l];
 normalize(&coords[0]);
 VECTOR(&coords[1],&coords[2],&coords[0]);
 return 1;
 }

void turnpnt(struct point *offset,struct point *coords,struct point *ncoords,
 struct point *pnt)
 {
 struct point tpnt,pc;
 int k;
 for(k=0;k<3;k++)
  tpnt.x[k]=pnt->x[k]-offset->x[k];
 for(k=0;k<3;k++)
  pc.x[k]=SCALAR(&coords[k],&tpnt);
 for(k=0;k<3;k++)
  pnt->x[k]=ceil(pc.x[0]*ncoords[0].x[k]+pc.x[1]*ncoords[1].x[k]+
   pc.x[2]*ncoords[2].x[k]+offset->x[k]);
 }    
     
int pr_cube(struct list *nl,enum evcodes ec)
 {
 struct point offset,coords[3],ncoords[3],*save,*p;
 struct node *n,*cn;
 struct list lp;
 int j,k,currcubeins=0;
 if(ec!=ec_pbankleft && ec!=ec_pbankright && ec!=ec_pturnleft &&
  ec!=ec_pturnright) 
  { printmsg("Only banking allowed with cubes."); return 1; }
 initlist(&lp);
 if(!turningcoords(&offset,coords)) return 1;
 /* add current cube to tag list */
 if(ec==ec_pbankleft || ec==ec_pbankright)
  { if((currcubeins=((cn=findnode(nl,view.pcurrcube->no))==NULL))==1)
     cn=addnode(nl,view.pcurrcube->no,view.pcurrcube); }
 else
  { cn=NULL; ec=ec==ec_pturnleft ? ec_pbankleft : ec_pbankright; }
 /* turn coordsystem */
 maketurndir(ec,coords,ncoords);
 if((save=malloc(sizeof(struct point)*nl->size*8))==NULL)
  { printmsg("No mem to save coords\n"); return 1; }
 for(n=nl->head;n->next!=NULL;n=n->next)
  for(j=0;j<8;j++)
   if(!findnode(&lp,n->d.n->d.c->p[j]->no))
    addnode(&lp,n->d.n->d.c->p[j]->no,n->d.n->d.c->p[j]->d.p);
 for(n=lp.head,p=save;n->next!=NULL;n=n->next,p++)
  {
  *p=*n->d.p;
  turnpnt(&offset,coords,ncoords,n->d.p);
  }
 for(n=lp.head,k=1;n->next!=NULL;n=n->next)
  if((k=testpnt(n))==0) break;
 if(!k)
  for(p=save,n=lp.head;n->next!=NULL;n=n->next,p++)
   *n->d.p=*p;
 else
  for(n=lp.head;n->next!=NULL;n=n->next)
   newcorners(n);
 free(save);
 freelist(&lp,NULL);
 if(cn!=NULL && currcubeins) /* for empty tagged list */
  freenode(nl,cn,NULL);
 return (!k) ? 1 : 6;
 }
