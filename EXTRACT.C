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
#include <ctype.h>
#include <fnmatch.h>

#define LASTFILE "fusion2.pof"

struct head { 
 char name[13] __attribute__ ((packed));
 unsigned long length __attribute__ ((packed)); } ;
 
void main(int argn,char *argc[])
 {
 FILE *hogf,*lf;
 char *hogname="..\\..\\descent.hog";
 char *copyfile,compare[100];
 struct head puffer;
 unsigned long nextpos;
 int i;
 if(argn<3)
  {
   printf("Extract by Achim Stremplat.\n");
   printf("Usage: %s <filename (wildcards allowed)> [<hogfile>]\n",
   argc[0]); exit(0); }
 if(argn>2)
  hogname=argc[2];
 if((hogf=fopen(hogname,"rb"))==NULL)
  { printf("Can't open hogfile: %s\n",hogname); exit(0); }
 if(fread(puffer.name,3,1,hogf)!=1)
  { printf("Can't read first three bytes in hogfile %s.\n",hogname);
    exit(0); }
 puffer.name[3]=0;
 if(strcmp(puffer.name,"DHF"))
  { printf("No descent hogfile: %s!=DHF\n",puffer.name); exit(0); }
 nextpos=ftell(hogf);
 strncpy(compare,argc[1],99);
 for(i=0;i<100;i++) compare[i]=tolower(compare[i]);
 while(strcmp(puffer.name,LASTFILE))
  {
  if(ftell(hogf)!=nextpos)
   { printf("Can't set position %lu in hogfile.\n",nextpos); exit(0); }    
  if(fread(&puffer,1,17,hogf)!=17)
   { printf("Last file %s in hogfile not found (pos %lu).\n",LASTFILE,
      ftell(hogf));
     exit(0); }  
  for(i=0;i<13;i++) puffer.name[i]=tolower(puffer.name[i]);
  if(fnmatch(compare,puffer.name,0)==0)
   {
   printf("Copy file %s, length %lu..",puffer.name,puffer.length);
   if((copyfile=malloc(puffer.length))==NULL)
    { printf("No mem to copy file %s\n",puffer.name); exit(0); }
   if(fread(copyfile,puffer.length,1,hogf)!=1)
    { printf("Can't read hogfile.\n"); exit(0); }
   if((lf=fopen(puffer.name,"wb"))==NULL)
    { printf("Can't open file: %s\n",puffer.name); exit(0); }
   if(fwrite(copyfile,puffer.length,1,lf)!=1)
    { printf("Can't write to file.\n"); exit(0); }
   fclose(lf);
   free(copyfile);
   printf("OK\n");
   }
  nextpos+=puffer.length+17;
  fseek(hogf,nextpos,SEEK_SET);
  }
 }