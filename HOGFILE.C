/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    extract.c - extract files from the descent hog-file.
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
    
#include <stdio.h>
#include <stdlib.h>
#include <dir.h>

struct head { 
 char name[13] __attribute__ ((packed));
 unsigned long length __attribute__ ((packed)); } ;
 
int sortfunc(const void *s1,const void *s2)
 { return strcmp(*(char **)s1,*(char **)s2); }
 
void main(int argn,char *argc[])
 {
 FILE *msnf,*hogf,*lf;
 char hogname[13],inpstr[9],filename[256],*buffer,
  *hogpath="..\\",*rdlpath="levels\\",*lfnames[500],*lsnames[500];
 struct head header;
 int j,l,startsort,argstart,num_levels,num_secrets,num_slevel[500];
 struct ffblk sfile;
 printf("Hogfile-Sampler by Achim Stremplat.\n");
 if(argn<3)
  {
  printf("Usage:\n");
  printf("hogfile [options] <msn/hogfile> <levelsnames> <+XX secretlevel>\n");
  printf("Possible options:\n");
  printf("-h <path of msn/hogfile>. Default: %s\n",hogpath);
  printf("-r <path of levelsnames>. Default: %s\n",rdlpath);
  printf("Enter filenames without extensions.\n");
  printf("For levelsnames wildcards are allowed.\n");
  printf("+XX secretlevel: XX must be replaced by the number of the level\n");
  printf("                 the secretlevel is attached to.\n");
  exit(0);
  }
 for(j=1,argstart=1;j<argn;j++)
  {
  if(argc[j][0]!='-') { argstart=j; break; }
  switch(argc[j][1])
   {
   case 'h': hogpath=&argc[j][3]; break;
   case 'r': rdlpath=&argc[j][3]; break;
   default: printf("Unknown option %c\n",argc[j][1]); exit(0);
   }
  }
 if(argn-argstart<2)
  { printf("Not enough parameters.\n");
    printf("Try hogfile without parameters for usage.\n"); exit(0); }
 strcpy(hogname,hogpath); strcat(hogname,argc[argstart]);
 strcpy(filename,hogname); strcat(filename,".hog");
 printf("Opening file %s...\n",filename);
 if((hogf=fopen(filename,"r"))!=NULL)
  { 
  fclose(hogf);
  printf("File %s exists. Overwrite (y/n)?",filename);
  scanf("%c",inpstr);
  if(inpstr[0]!='y')
   { printf("HOG-file creating aborted.\n"); exit(0); }
  }
 if((hogf=fopen(filename,"wb"))==NULL)
  { printf("Can't create HOG-file.\n"); exit(0); }
 if(fwrite("DHF",1,3,hogf)!=3)
  { printf("Can't write hogfile-header.\n"); exit(0); }
 strcpy(hogname,hogpath); strcat(hogname,argc[argstart]);
 strcpy(filename,hogname); strcat(filename,".msn");
 if((msnf=fopen(filename,"w"))==NULL)
  { printf("Can't create MSN-file.\n"); exit(0); }
 printf("Name of mission:");
 scanf(" %255[^\n]",filename);
 fprintf(msnf,"name = %s\n",filename);
 printf("Mission only as multiplayer mission available ? (y/n)");
 do scanf(" %c",filename); while(filename[0]!='y' && filename[0]!='n');
 if(filename[0]=='y') fprintf(msnf,"type = anarchy\n");
 else fprintf(msnf,"type = normal\n");
 for(j=argstart+1,num_levels=0,num_secrets=0;j<argn;j++)
  {
  if(argc[j][0]=='+')
   {
   sscanf(&argc[j][1],"%d",&num_slevel[num_secrets]);
   printf("Found secret level attached to level %d\n",
    num_slevel[num_secrets]); 
   if(++j>=argn) 
    { printf("No name for secret level.\n"); break; }
   }
  else num_slevel[num_secrets]=0;
  strcpy(filename,rdlpath); 
  strncat(filename,argc[j],8); 
  filename[strlen(rdlpath)+8]=0;
  strcat(filename,".rdl");
  if(findfirst(filename,&sfile,FA_ARCH|FA_RDONLY)!=0)
   { printf("No file like %s found.\n",filename); continue; }
  startsort=num_levels;
  do
   {
   strcpy(filename,rdlpath); strcat(filename,sfile.ff_name);
   printf("Opening level %s...",filename); 
   if((lf=fopen(filename,"rb"))==NULL)
    { printf("Can't open level-file.\n"); continue; }
   printf("Reading level..."); fflush(stdout);
   fseek(lf,SEEK_END,0); l=ftell(lf);  fseek(lf,SEEK_SET,0);
   if((buffer=malloc(ftell(lf)))==NULL)
    { printf("No mem for level-file.\n"); fclose(lf); continue; }
   if(fread(buffer,1,l,lf)!=l)
    { printf("Can't read level.\n"); fclose(lf); free(buffer); continue; }
   fclose(lf);
   strcpy(header.name,sfile.ff_name); 
   header.length=l;
   if(fwrite(&header,sizeof(struct head),1,hogf)!=1)
    { printf("Can't write levelhead.\n"); exit(0); }
   if(fwrite(buffer,1,l,hogf)!=l)
    { printf("Can't write level.\n"); exit(0); }
   printf("OK\n");
   free(buffer);
   if(num_slevel[num_secrets])
    { if((lsnames[num_secrets]=malloc(strlen(header.name)+1))==NULL)
       { printf("No mem for levelname."); exit(0); }
      strcpy(lsnames[num_secrets++],header.name); break; }
   else
    { if((lfnames[num_levels]=malloc(strlen(header.name)+1))==NULL)
       { printf("No mem for levelname."); exit(0); }
      strcpy(lfnames[num_levels++],header.name); }
   }
  while(findnext(&sfile)==0);
  if(num_levels-startsort>1)
   qsort(&lfnames[startsort],num_levels-startsort,sizeof(char *),
    sortfunc);
  }
 fclose(hogf); 
 fprintf(msnf,"num_levels = %d\n",num_levels);
 for(j=0;j<num_levels;j++) fprintf(msnf,"%s\n",lfnames[j]);
 if(num_secrets>0)
  {
  fprintf(msnf,"num_secrets = %d\n",num_secrets);
  for(j=0;j<num_secrets;j++)
   if(num_slevel[j]>0 && num_slevel[j]<=num_levels)
    fprintf(msnf,"%s,%d\n",lsnames[j],num_slevel[j]);
   else
    printf("Illegal number %d for secret level.\n",num_slevel[j]);
  }
 fclose(msnf); 
 printf("Order of levels.\n");
 for(j=0;j<num_levels;j++) printf("%d. %s\n",j+1,lfnames[j]);
 printf("If you don't like this order changed it with a text editor\n");
 printf("in the msn-file.\n");
 }
 