/* This routine saves a level as a ASCII-readable textfile */
int txt_savelevel(char *path,char *fname,struct leveldata *ld,int testlevel)
 {
 FILE *f;
 char *fullpath;
 struct node *n;
 int i,j;
 if(l==NULL || fname==NULL || !checklvl(ld,testlevel)) return 0;
 fullpath=makelevelpath(path,fname);
 if((f=fopen(fullpath,"w"))==NULL) { FREE(fullpath); return 0; }
 fprintf(f,"* DEVIL Level file %s. Versionnumber:\n",fullpath);
 FREE(fullpath);
 fprintf(f,"%d\n",FF_VERSION);
 fprintf(f,"* Minedata. Levelname, number of points, number of cubes.\n");
 fprintf(f,"%s\n%4d %3d\n",ld->fullname,ld->pts.size,ld->cubes.size);
 fprintf(f,"* Points: no x y z\n");
 for(n=ld->pts.head;n->next!=NULL;n=n->next)
  fprintf(f,"%4d %11ld %11ld %11ld\n",n->no,(long)n->d.p->x[0],
   (long)n->d.p->x[1],(long)n->d.p->x[2]);
 fprintf(f,"* Cubes: no p0..p7 light type\n");
 fprintf(f,"*        neighbour wall\n");
 for(n=ld->cubes.head;n->next!=NULL;n=n->next)
  {
  fprintf(f,"%3d %4d %4d %4d %4d %4d %4d %4d %4d %5d %2d\n",n->no,
   n->d.c->p[0]->no,n->d.c->p[1]->no,n->d.c->p[2]->no,n->d.c->p[3]->no,
   n->d.c->p[4]->no,n->d.c->p[5]->no,n->d.c->p[6]->no,n->d.c->p[7]->no,
   (int)n->d.c->light,(int)n->d.c->type);
  if(n->d.c->type==4) /* producer */
   fprintf(f," %#10lx\n",n->d.c->cp ? n->d.c->cp->d.cp->prodwhat : 0);
  for(i=0;i<6;i++)
   {
   fprintf(f," %3d %3d\n",n->d.c->nc[i] ? n->d.c->nc[i]->no : -1,
    n->d.c->d[i] ? n->d.c->d[i]->no : -1);
   if(n->d.c->nc[i]==NULL || n->d.c->d[i]!=NULL)
    {
    if(n->d.c->walls[i]!=NULL) 
     {
     fprintf(f," %5u %5u\n",(unsigned int)n->d.c->walls[i]->texture1,
      (unsigned int)n->d.c->walls[i]->texture2|
      ((unsigned int)(n->d.c->walls[i]->txt2_direction&3)<<14));
     for(j=0;j<4;j++)
      fprintf(f," %5d %5d %5u",(int)n->d.c->walls[i]->corners[j].xpos,
       (int)n->d.c->walls[i]->corners[j].ypos,
       (unsigned int)n->d.c->walls[i]->corners[j].light);
     fprintf(f,"\n");
     }
    else { fprintf(f,"* ERROR\n0 0\n0 0 0\n0 0 0\n0 0 0\n0 0 0\n");
            printmsg(TXT_NOSIDE,n->no,i); }
    }
   }
  fprintf(f,"* end of cube %d\n",n->no);
  }
 fprintf(f,"* Gamedata: exitcube, exitwall\n");
 fprintf(f,"*           no. things, no. walls, no. open at end\n");
 fprintf(f,"%d %d\n%d %d %d\n",ld->exitcube->no,ld->exitwall,ld->things.size,
  ld->doors.size,ld->edoors ? ld->edoors->num : 0);
 fprintf(f,"* Things: no type id c_type c_id c_count size\n");
 fprintf(f,"*         x y z\n");
 fprintf(f,"*         o1x o1y o1z o2x o2y o2z\n");
 for(n=ld->things.head;n->next!=NULL;n=n->next)
  {
  fprintf(f,"%4d %3u %3u %3d %3d %3d %11ld\n",n->no,n->d.t->type1,
   n->d.t->type2,(int)n->d.t->contain_type1,(int)n->d.t->contain_type2,
   (int)n->d.t->contain_count,n->d.t->size);
  fprintf(f," %11ld %11ld %11ld\n %11ld %11ld %11ld %11ld %11ld %11ld\n",
   (long)n->d.t->p[0].x[0],(long)n->d.t->p[0].x[1],(long)n->d.t->p[0].x[2],
   n->d.t->orientation[0],n->d.t->orientation[1],n->d.t->orientation[2],
   n->d.t->orientation[3],n->d.t->orientation[4],n->d.t->orientation[5]);
  if(n->d.t->type1==2) fprintf(f," %4u\n",
   (unsigned int)n->d.R->c.behavior&0x7f);
  fprintf(f,"* end of thing %d\n",n->no);
  }
 fprintf(f,"* Walls: no type flags cubeno sideno anim key hp switchtype\n");
 fprintf(f,"*        for switch: number of 10 objects to be switched\n");   
 for(n=ld->doors.head;n->next!=NULL;n=n->next)
  {
  fprintf(f,"%3d %3u %#5x %3d %1u %3u %3u %11ld %#5x\n",n->no,n->d.d->type1,
   n->d.d->type2,n->d.d->c->no,(unsigned int)n->d.d->wallnum,
   n->d.d->animtxt,n->d.d->key,n->d.d->hitpoints,
   (unsigned int)n->d.d->switchtype);
  if(n->d.d->switchtype==0x1 || n->d.d->switchtype==0x40)
   {
   if(n->d.d->sd==NULL) 
    { fprintf(f," * ERROR\n -1 -1 -1 -1 -1 -1 -1 -1 -1 -1");
      printmsg(TXT_NOSWITCHDATA,n->no); }
   else    
    for(i=0;i<10;i++) fprintf(f," %3d",n->d.d->sd->d.sd->target[i] ?
     n->d.d->sd->d.sd->target[i]->no : -1);
   fprintf(f,"\n");
   }
  }
 fprintf(f,"* Open at end: number of doors to open at end\n");
 if(ld->edoors)
  {
  for(n=ld->doors.head,i=0;n->next!=NULL;n=n->next)
   if(n->d.d->edoor) { fprintf(f,"%3d ",n->no); i++; }
  if(i!=ld->edoors->num) printmsg(TXT_WRONGNUMBEROFEDOORS);
  }
 fprintf(f,"\n* End of the file\n");
 fclose(f); 
 return 1;
 }
 
/* this finds the string x of length n in file f */ 
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

