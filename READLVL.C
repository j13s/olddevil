/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    readlvl.c - reading whole levels.
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de)

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
#include "tools.h"
#include "savetool.h"
#include "event.h"
#include "insert.h"
#include "readlvl.h"

extern int init_test;

struct leveldata initlevdat= {
 { "LVLP",0x1,0x14,0,0,0,0,0 }, 
 { 0x196705,0x77,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ,0,0x74, 
   0,0,0x108, 0,0,0x18, 0,0,0x10, 0,0,0x36, { 0,0,0 }, 0,0,0x2a, 0,0,0x10 },
 { 0x19, {}, "GUILE" },
#ifdef SHAREWARE
 { 0,0,0,0,0xff,0xff,0xff,0xff,0xa } 
#endif
 };
 
void find(FILE *f,unsigned char *x,short int n)
 {
 short int i=0;
 unsigned char buffer;
 while(!feof(f))
  {
  fread(&buffer,sizeof(unsigned char),(size_t)1,f);
  if(buffer==x[i])
   {
   i+=1;
   if(i==n)
    { fseek(f,ftell(f)-n,SEEK_SET); return; }
   }
  else if(i>0) { fseek(f,ftell(f)-i,SEEK_SET); i=0; }
  }
 }

int readlvl(char *lname,struct leveldata *ld)
 {
 int i,j;
 char buffer[31],*pos,fname[strlen(lname)+strlen(view.levelpath)+5];
 FILE *lf;
 struct node *n;
 initdata();
 if((pos=strchr(lname,'.'))!=NULL) *pos=0;
 if((l->lname=malloc(strlen(lname)+5))==NULL)
  { fprintf(errf,"No mem for lname.\n"); return 0; }
 strcpy(fname,view.levelpath); strcpy(l->lname,lname);
#ifdef SHAREWARE
 strcat(l->lname,".sdl");
#else
 strcat(l->lname,".rdl");
#endif
 strcat(fname,l->lname);
 if((lf=fopen(fname,"rb"))==NULL)
  { printmsg("Can't open level %s",fname); return 0; }
 if(fread(&ld->lh1,sizeof(struct levelhead1),1,lf)!=1)
  { fprintf(errf,"Can't read levelheader.\n"); fclose(lf); return 0; }
 printmsg("Reading %ld pts...",ld->lh1.numpts);
 if(init_test&2)
  fprintf(errf,"%d points, %d cubes\n",ld->lh1.numpts,ld->lh1.numcubes);
 if(!readlist(lf,&ld->pts,readpnt,ld->lh1.numpts))
  { fprintf(errf,"Can't add points in list.\n"); my_exit(); }
 printmsg("Reading %ld cubes...",ld->lh1.numcubes);
 for(i=0;i<ld->lh1.numcubes;i++)
  {
  if((n=addnode(&ld->cubes,i,readcube(lf)))==NULL)
   { fprintf(errf,"Can't add cubes in list.\n"); my_exit(); }
  for(j=0;j<6;j++)
   if(n->d.c->nextcubes[j]==0xfffe) /* exit */
    { view.exitcube=n; view.exitwall=j;
      n->d.c->nextcubes[j]=0xffff; }
  }
 printmsg("Reading levelheader 2...");
 if(fread(&ld->lh2,sizeof(struct levelhead2),1,lf)!=1)
  { fprintf(errf,"Can't read stuff in header for things: dirsize.\n"); 
  return 0; }
 printmsg("Reading fullname of level...");
 i=0;
 while((buffer[i++]=fgetc(lf))!=0 && i<31);
 if(buffer[i-1]!=0)
  { fprintf(errf,"Can't read full name of level.\n"); return 0; }
 if((ld->fullname=malloc(strlen(buffer)+1))==NULL)
  { fprintf(errf,"No mem for fullname.\n"); return 0; }
 strcpy(ld->fullname,buffer);
 printmsg("Reading levelhead 3...");
 if(fread(&ld->lh3,sizeof(struct levelhead3),1,lf)!=1)
  { fprintf(errf,"Can't read levelhead 3.\n"); return 0; }
 if(strncmp(ld->lh3.guile,"GUILE",5)!=0)
  { fprintf(errf,"Wrong position %lx at guile.\n",ftell(lf)); return 0; }
 fseek(lf,ld->lh2.posthings,SEEK_SET);
 printmsg("Reading %ld things...",ld->lh2.numthings);
 for(i=0;i<ld->lh2.numthings;i++)
  if(!addnode(&ld->things,i,readthing(lf)))
   { fprintf(errf,"Can't add things in list.\n"); my_exit(); }
 printmsg("Reading %ld doors...",ld->lh2.numdoors);
 fseek(lf,ld->lh2.posdoors,SEEK_SET);
 if(!readlist(lf,&ld->doors,readdoor,ld->lh2.numdoors))
  { fprintf(errf,"Can't add door in list.\n"); my_exit(); } 
 fseek(lf,ld->lh2.possdoors,SEEK_SET);
 if(!readlist(lf,&ld->sdoors,readsdoor,ld->lh2.numsdoors))
  { fprintf(errf,"Can't add door to list.\n"); my_exit(); } 
 fseek(lf,ld->lh2.posedoors,SEEK_SET);
 if((ld->edoors=readedoors(lf,ld->lh2.numedoors,ld->lh2.sizeedoors))==NULL)
  { fprintf(errf,"Can't read edoors.\n"); return 0; }
 fseek(lf,ld->lh2.posproducer,SEEK_SET);
 if(!readlist(lf,&ld->producers,readproducer,ld->lh2.numproducer))
  { fprintf(errf,"Can't add producer to list.\n"); my_exit(); }  
#ifdef SHAREWARE
 if(fread(ld->end,1,9,lf)!=9)
  { fprintf(errf,"Can't read end.\n"); return 0; } 
#endif
 fclose(lf);
 for(n=l->cubes.head;n->next!=NULL;n=n->next)
  { initcube(n); 
    if(n->no%10==0)
     printmsg("Init cube %d/%d",n->no,l->cubes.size); }
 for(n=l->doors.head;n->next!=NULL;n=n->next)
  {
  initdoor(n); 
  if(n->no%10==0)
   printmsg("Init door %d/%d",n->no,l->doors.size);
  }
 for(i=0;i<l->edoors->num;i++)
  {
  if((n=findnode(&l->cubes,l->edoors->cubes[i]))==NULL)
   { printmsg("Can't find end door cube: %d",l->edoors->cubes[i]);continue;}
  if(n->d.c->d[l->edoors->walls[i]]==NULL)
   { printmsg("No door for end cube wall: %d %d",l->edoors->cubes[i], 
      l->edoors->walls[i]); continue; }
  n->d.c->d[l->edoors->walls[i]]->d.d->edoor=1;
  }
 view.pcurrcube=(l->cubes.size>0) ? l->cubes.head : NULL;
 view.pcurrthing=(l->things.size>0) ? l->things.head : NULL;
 view.pcurrwall=(view.pcurrcube!=NULL) ? view.pcurrcube->d.c->walls[0] : NULL;
 view.pcurrdoor=(l->doors.size>0) ? l->doors.head : NULL;
 view.curmenuitem=findmenuitem(NULL,1);
 printmsg("OK");
 return 1;
 }
 
void initdata(void)
 {
 int i;
 view.menuoffset=0; view.curmenuitem=NULL;
 view.oldpcurrthing=view.oldpcurrcube=view.pcurrthing=view.pcurrcube=
  view.oldpcurrdoor=view.pcurrdoor=NULL;
 view.pcurrwall=NULL;
 view.currpnt=view.currwall=0; view.oldpcurrpnt=NULL;
 view.drawwhat=DW_DEFAULT;
 view.exitcube=NULL;
 for(i=0;i<tt_number;i++)
  freelist(&view.tagged[i],NULL);
 freelist(&l->pts,free);
 freelist(&l->cubes,freecube);
 freelist(&l->things,free);
 freelist(&l->doors,freedoor);
 freelist(&l->sdoors,free); freelist(&l->producers,free); free(l->edoors); 
 free(l->fullname);
 *l=initlevdat;
 strncpy(l->lh3.guile,"GUILE",5);
#ifdef SHAREWARE
 *((unsigned long *)l->end)=0;
 *((unsigned long *)(l->end+4))=0xffffffff;
 l->end[8]=0xa;
#endif
 for(i=0;i<tt_number;i++)
  initlist(&view.tagged[i]);
 initlist(&l->cubes); initlist(&l->things);
 initlist(&l->doors); initlist(&l->pts);
 initlist(&l->sdoors); initlist(&l->producers);
 }

int makedoors(void)
 {
 struct node *n;
 int i;
 l->edoors->num=0;
 for(n=l->doors.head;n->next!=NULL;n=n->next)
  {
  if(n->d.d->edoor)
   {
   if(l->edoors->num==10)
    { printmsg("Too many doors to be opened at end (max.: 10)."); return 0; }
   l->edoors->cubes[l->edoors->num]=n->d.d->cubenum;
   l->edoors->walls[l->edoors->num++]=n->d.d->wallnum; 
   }
  n->d.d->sdoor=n->d.d->sd!=NULL ? n->d.d->sd->no : 0xff;
  }
 for(n=l->sdoors.head;n->next!=NULL;n=n->next)
  for(i=0;i<n->d.sd->num;i++)
   {
   if(getsdoortype(n->d.sd)==sdtype_door) 
    { n->d.sd->cubes[i]=n->d.sd->target[i]->d.d->c->no;
      n->d.sd->walls[i]=n->d.sd->target[i]->d.d->wallnum; }
   else n->d.sd->cubes[i]=n->d.sd->target[i]->no;
   }
 for(n=l->producers.head;n->next!=NULL;n=n->next)
  { n->d.cp->cubenum=n->d.cp->c->no; 
    n->d.cp->stuffnum=n->d.cp->c->d.c->subtype; }
 return 1;
 }
 
/* make level ready for save. if this is not possible, return 0 */
int checklvl(void)
 {
 struct node *n,*c,*start=NULL;
 void *data;
 int i;
 char x;
 if(view.exitcube==NULL)
  { printmsg("WARNING: No exit side defined."); 
    if(!readkeyb("Ignore? (y/n)","%c",&x) || x!='y') return 0; }
 sortlist(&l->cubes,0); /* so we get no higher numbers than size */
 for(n=l->things.head;n->next!=NULL;n=n->next)
  {
  for(i=0;i<3;i++)
   n->d.t->pos[i]=n->d.t->p[0].x[i];
  if((c=findpntcube(&l->cubes,&n->d.t->p[0]))==NULL)
   { printmsg("WARNING: Thing %d out of bounds.",n->no); view.pcurrthing=n; 
     if(!readkeyb("Ignore? (y/n)","%c",&x) || x!='y') return 0;
     else c=l->cubes.head; }
  if(n->d.t->type1==4 && n->d.t->type2==0) 
   if(start==NULL) start=n;
   else
    { printmsg("WARNING: Two starting places: thing %d and thing %d",
       start->no,n->no); view.pcurrthing=n;
      if(!readkeyb("Ignore? (y/n)","%c",&x) || x!='y') return 0; }
  n->d.t->cube=c->no;
  }
 if(start==NULL)
  { printmsg("WARNING: No starting place."); 
    if(!readkeyb("Ignore? (y/n)","%c",&x) || x!='y') return 0; }
 data=start->d.v; i=start->no;
 freenode(&l->things,start,NULL); 
 if(!addheadnode(&l->things,i,data))
  { printmsg("Can't move startnode."); return 0; } 
 sortlist(&l->things,0); sortlist(&l->doors,0);
 sortlist(&l->pts,0); sortlist(&l->producers,0); sortlist(&l->sdoors,0);
 return 1;
 }
 
int savelvl(FILE *f)
 {
 if(!checklvl()) return 0;
 /* sizeall and endofcubes are filled later */
 l->lh1.numcubes=l->cubes.size;
 l->lh1.numpts=l->pts.size;
 if(fwrite(&l->lh1,sizeof(struct levelhead1),1,f)!=1)
  { fprintf(errf,"Savelvl: Can't write header\n"); return 0; }
 /* now write all points */
 if(!savelist(f,&l->pts,savepoint,0)) return 0;
 /* and cubes */
 if(!savelist(f,&l->cubes,savecube,0)) return 0;
 l->lh1.endofcubes=ftell(f);
 l->lh2.numthings=l->things.size;
 l->lh2.numdoors=l->doors.size;
 l->lh2.numsdoors=l->sdoors.size;
 l->lh2.numproducer=l->producers.size;
 l->lh2.numedoors=1;
 if(fwrite(&l->lh2,sizeof(struct levelhead2),1,f)!=1)
  { fprintf(errf,"Savelvl: Can't write header for things.\n"); return 0; }
 if(fwrite(l->fullname,1,strlen(l->fullname)+1,f)!=
  strlen(l->fullname)+1)
  { fprintf(errf,"Savelvl: Can't save fullname.\n"); return 0; }
 if(fwrite(&l->lh3,sizeof(struct levelhead3),1,f)!=1)
  { fprintf(errf,"Can't write levelhead3.\n"); return 0; }
 l->lh2.posguile=ftell(f)-l->lh2.sizeguile;
 l->lh2.posthings=ftell(f);
 if(!savelist(f,&l->things,savething,0)) return 0;
 if(!makedoors()) return 0; 
 l->lh2.posdoors=ftell(f);
 if(!savelist(f,&l->doors,savedoor,0)) return 0;
 l->lh2.possdoors=l->lh2.posempty=ftell(f);
 if(!savelist(f,&l->sdoors,savesdoor,0)) return 0;
 l->lh2.posedoors=ftell(f);
 if(fwrite(l->edoors,sizeof(struct edoor),l->lh2.numedoors,f)!=
  l->lh2.numedoors)
  { fprintf(errf,"Can't write edoors.\n"); return 0; }
 l->lh2.posproducer=ftell(f);
 if(!savelist(f,&l->producers,saveproducer,0)) return 0;
 l->lh1.sizeall=ftell(f);
#ifdef SHAREWARE 
 if(fwrite(l->end,1,9,f)!=9)
  { fprintf(errf,"Can't write end.\n"); return 0; } 
#endif 
 fseek(f,l->lh1.endofcubes,SEEK_SET);
 if(fwrite(&l->lh2,sizeof(struct levelhead2),1,f)!=1) /* cause of pos */
  { fprintf(errf,"Savelvl: Can't write header for things 2.\n"); return 0; }
 fseek(f,0,SEEK_SET);
 if(fwrite(&l->lh1,sizeof(struct levelhead1),1,f)!=1)
  { fprintf(errf,"Savelvl: Can't write header 2\n"); return 0; }
 fclose(f);  
 return 1;
 }