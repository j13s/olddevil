/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    inout.c - input/output functions for temporary in/output
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
#include "tools.h"
#include "userio.h"

#define MAX_MSNNAME_LEN 21
#define FIRSTHOGFILE "DESCENT.TXB"
#define MSNEXT (init.d_ver>=d2_10_sw ? "MN2" : "MSN")

int vprintmsg(const char *txt,va_list args,int wait)
 {
 char buffer[10240];
 vsprintf(buffer,txt,args); 
 return w_okcancel(buffer,wait ? TXT_OK : NULL,wait>1 ? TXT_CANCEL : NULL,
  NULL);
 }

void waitmsg(const char *txt,...)
 {
 va_list args;
 va_start(args,txt);
 vprintmsg(txt,args,1);
 va_end(args);
 }
 
void printmsg(const char *txt,...)
 {
 va_list args;
 va_start(args,txt);
 vprintmsg(txt,args,0);
 va_end(args);
 }
 
int yesnomsg(const char *txt,...)
 {
 int ret;
 va_list args;
 va_start(args,txt);
 ret=vprintmsg(txt,args,2);
 va_end(args);
 return ret;
 }
 
int isfloat(int x) { return isdigit(x)||x=='.'||x=='-'; }

int getfloat(const char *txt,const char *help,int length,int digits,
 float *input)
 {
 struct w_window w,*ow;
 char str[100];
 struct w_button *b;
 struct w_b_string bs;
 my_assert(length<=99);
 sprintf(str,"%*.*f",length,digits,*input);
 bs.max_length=length; bs.str=str; bs.allowed_char=isfloat;
 bs.l_char_entered=bs.r_char_entered=bs.l_string_entered=
  bs.r_string_entered=NULL;
 checkmem(b=w_addstdbutton(NULL,w_b_string,0,0,-1,-1,txt,&bs,0));
 w.xpos=w.ypos=-1; 
 w.maxxsize=w.maxysize=-1; w.shrunk=0; w.title=NULL; w.help=help;
 w.buttons=0; w.refresh=wr_normal; w.hotkey=0;
 w.refresh_routine=w.close_routine=NULL; w.click_routine=NULL;
 w_winoutsize(&w,b->xsize,b->ysize);
 checkmem(ow=w_openwindow(&w));
 w_buttoninwin(ow,b,1);
 w_handleuser(1,&b,1,&ow,0,NULL,NULL);
 w_closewindow(ow);
 sscanf(str,"%f",input);
 return 1;
 }

int path_max_length;
char current_path[1024],current_ext[100];
 
/* Draw the path current_path to w,x,y. Cut the start off if it is longer
  than xsize */
void uio_drawpath(struct w_window *w,int x,int y,int xsize)
 {
 char buffer[1024],*p;
 ws_makepath(current_path,buffer); 
 strcpy(current_path,buffer);
 p=buffer;
 while(strlen(p)>0 && ws_pixstrlen(p)>xsize) p++;
 if(strlen(p)>=3)
  { if(p!=buffer) p[0]=p[1]=p[2]='.'; 
    w_drawouttext(w,x,y,xsize,p); }
 }
 
void uio_updown(struct w_button **b,int bno,int add)
 {
 struct w_b_strlist *bsls=b[bno]->d.sls;
 if(add<-1 || add>1) bsls->start_no+=bsls->no_rows/add;
 else bsls->start_no+=add; 
 if(bsls->start_no>bsls->no_strings-bsls->no_rows)
  bsls->start_no=bsls->no_strings-bsls->no_rows;
 if(bsls->start_no<0) bsls->start_no=0;
 w_drawbutton(b[bno]);
 }
void uio_files_one_up(struct w_button *b) { uio_updown(b->data,5,-1); }
void uio_files_one_down(struct w_button *b) { uio_updown(b->data,5,1); }
void uio_files_page_up(struct w_button *b) { uio_updown(b->data,5,-2); }
void uio_files_page_down(struct w_button *b) { uio_updown(b->data,5,2); }
void uio_dirs_one_up(struct w_button *b) { uio_updown(b->data,2,-1); }
void uio_dirs_one_down(struct w_button *b) { uio_updown(b->data,2,1); }
void uio_dirs_page_up(struct w_button *b) { uio_updown(b->data,2,-2); }
void uio_dirs_page_down(struct w_button *b) { uio_updown(b->data,2,2); }

/* current_path is changed->this routine is called */
void uio_changedir(struct w_button **b,char *oldpath)
 {
 struct w_b_strlist *b_files=b[5]->d.sls,*b_dirs=b[2]->d.sls;
 int i,no,dno;
 char **files,**dirs,*t;
 uio_drawpath(b[0]->w,b[0]->xpos,b[0]->ypos+b[0]->ysize+1,b[0]->xsize);
 files=ws_getallfilenames(current_path,current_ext,&no);
 dirs=ws_getalldirs(current_path,"*",&dno);
 if(no<0 || dno<0)
  { if(no==-2 || dno==-2) waitmsg(TXT_ILLEGALPATH,current_path); 
    if(no==-1 || dno==-1) waitmsg(TXT_CANTFINDPATH,current_path); 
    strcpy(current_path,oldpath);
    uio_drawpath(b[0]->w,b[0]->xpos,b[0]->ypos+b[0]->ysize+1,b[0]->xsize);
    files=ws_getallfilenames(current_path,current_ext,&no);
    dirs=ws_getalldirs(current_path,"*",&dno);
    my_assert(no>=0 && dno>=0); }
 if(files) qsort(files,no,sizeof(char *),qs_compstrs);
 if(dirs) qsort(dirs,dno,sizeof(char *),qs_compstrs);
 for(i=0;i<b_files->no_strings;i++) 
  { t=(char *)b_files->strings[i]; FREE(t); }
 for(i=0;i<b_dirs->no_strings;i++) 
  { t=(char *)b_dirs->strings[i]; FREE(t); }
 FREE(b_files->strings); FREE(b_dirs->strings);
 b_files->no_strings=no; b_dirs->no_strings=dno;
 b_files->start_no=b_dirs->start_no=0;
 FREE(b_files->sel_arr);
 checkmem(b_files->sel_arr=MALLOC(no));
 for(i=0;i<no;i++) b_files->sel_arr[i]=0;
 FREE(b_dirs->sel_arr);
 checkmem(b_dirs->sel_arr=MALLOC(dno));
 for(i=0;i<dno;i++) b_dirs->sel_arr[i]=0;
 b_files->strings=files; b_dirs->strings=dirs;
 w_drawbutton(b[2]); w_drawbutton(b[5]);
 }
 
void uio_string_getfname(struct w_button *b)
 { 
 char *s=b->d.str->str,oldpath[80],fp[80],drive[3],
  path[66],name[9],ext[5],rawpath[80];
 struct w_b_strlist *bd;
 struct w_button **b_arr=b->data;
 int i,num_sel;
 strcpy(oldpath,current_path);
 ws_splitpath(s,drive,path,name,ext);
 if(path[0]!='/' && path[0]!='\\' && drive[0]==0)
  { strcpy(rawpath,current_path); strcat(rawpath,"/"); }
 else rawpath[0]=0;
 strcat(rawpath,s);
 if(strlen(rawpath)==0) return;
 if(rawpath[strlen(rawpath)-1]==':')
  { rawpath[strlen(rawpath)+1]=0; rawpath[strlen(rawpath)]='/'; }
 ws_makepath(rawpath,fp);
 ws_splitpath(fp,drive,path,name,ext);
 if(path[0]=='/') 
  if(strlen(drive)>0) strcpy(current_path,drive);
  else current_path[0]=0;
 if(strlen(path)>0)
  {
  strcat(current_path,path);
  uio_changedir(b->data,oldpath);
  }
 bd=b_arr[2]->d.sls;
 for(i=0;i<bd->no_strings;i++)
  {
  strcpy(fp,name); strcat(fp,ext); strcat(fp,"/");
  if(strcmp(bd->strings[i],fp)==0) break;
  }
 if(i<bd->no_strings)
  { strcat(current_path,"/"); strcat(current_path,fp);
    uio_changedir(b->data,oldpath);
    b->d.str->str[0]=0; }
 else
  {
  fp[strlen(fp)-1]=0;
  strncpy(b->d.str->str,fp,b->d.str->max_length);
  b->d.str->str[b->d.str->max_length]=0;
  b->d.str->offset=0;
  bd=b_arr[5]->d.sls;
  for(i=0,num_sel=0;i<bd->no_strings;i++)
   {
   num_sel+=bd->sel_arr[i]=ws_matchname(fp,bd->strings[i]);
   if(num_sel>=bd->max_selected) 
    if(bd->max_selected==1) 
     { strcpy(b_arr[0]->d.str->str,bd->strings[i]); break; }
   }
  w_drawbutton(b_arr[5]);
  }
 w_drawbutton(b_arr[0]);
 }
void uio_sel_fname(struct w_button *b)
 {
 struct w_b_string *bs=((struct w_button **)b->data)[0]->d.str;
 int sel;
 for(sel=0;sel<b->d.sls->no_strings;sel++)
  if(b->d.sls->sel_arr[sel]) break;
 if(sel>=b->d.sls->no_strings) return;
 strncpy(bs->str,b->d.sls->strings[sel],bs->max_length);
 bs->str[bs->max_length]=0;
 w_drawbutton(((struct w_button **)b->data)[0]);
 }
void uio_sel_dir(struct w_button *b)
 {
 struct w_b_strlist *b_dirs=b->d.sls;
 char buffer[1024];
 int sel;
 for(sel=0;sel<b->d.sls->no_strings;sel++)
  if(b->d.sls->sel_arr[sel]) break;
 if(sel>=b->d.sls->no_strings) return;
 strcpy(buffer,current_path);
 strcat(current_path,"/"); 
 strcat(current_path,b_dirs->strings[sel]);
 current_path[strlen(current_path)-1]=0;
 uio_changedir(b->data,buffer);
 }

/* makes a file-requester with the files files and the dirs dirs in the
   window w in the rectangle (x,y)-(x+as much place as needed,y+ysize), that
   means:
   -A string button to enter the filename directly (default: defname (must
    be without path), max. number of chars=maxlength)
   -2*2 scroll up/down button
   -And two list buttons for the path&files (the strings are sorted)
   The pointers to the seven buttons are returned in the array
   b: b[0],...,b[6]=stringbutton,up,files,down,up,dirs,down. The mem for
   the substructures w_b_... is reserved in this function. You MUST
   free it when you don't need them anymore. You MUSTN'T free the array
   b before you have closed the window.
   
   returns a 0 if there is not enough room, the width of the whole thing if
   successful. */
int makefilebuttons(struct w_window *w,int x,int y,int ysize,
 char **files,int nofiles,char **dirs,int nodirs,const char *defname,
 struct w_button **b)
 {
 struct w_b_press *b_up,*b_down,*b_up2,*b_down2;
 struct w_b_strlist *b_fnames,*b_dirs;
 struct w_b_string *b_name;
 int i,no_rows,xsize;
 if(files) qsort(files,nofiles,sizeof(char *),qs_compstrs);
 if(dirs) qsort(dirs,nodirs,sizeof(char *),qs_compstrs);
 no_rows=ysize/(w_titlebarheight()+2)-5;
 if(no_rows<=0) return 0;
 checkmem(b_name=MALLOC(sizeof(struct w_b_string)));
 b_name->max_length=255; /* Should be enough for every path */
 checkmem(b_name->str=MALLOC(b_name->max_length+1)); 
 b_name->str[b_name->max_length]=0;
 if(defname) strncpy(b_name->str,defname,b_name->max_length);
 else b_name->str[0]=0;
 b_name->allowed_char=isgraph;
 b_name->l_char_entered=b_name->r_char_entered=NULL;
 b_name->l_string_entered=b_name->r_string_entered=uio_string_getfname;
 checkmem(b_fnames=MALLOC(sizeof(struct w_b_strlist)));
 b_fnames->no_strings=nofiles; b_fnames->strings=files;
 checkmem(b_fnames->sel_arr=MALLOC(nofiles));
 for(i=0;i<nofiles;i++) b_fnames->sel_arr[i]=0;
 b_fnames->no_rows=no_rows;
 b_fnames->l_string_selected=b_fnames->r_string_selected=uio_sel_fname;
 checkmem(b_dirs=MALLOC(sizeof(struct w_b_strlist)));
 b_dirs->no_strings=nodirs; b_dirs->strings=dirs;
 checkmem(b_dirs->sel_arr=MALLOC(nodirs));
 for(i=0;i<nodirs;i++) b_dirs->sel_arr[i]=0;
 b_dirs->no_rows=no_rows;
 b_dirs->l_string_selected=b_dirs->r_string_selected=uio_sel_dir;
 b_dirs->start_no=0;
 b_fnames->max_selected=b_dirs->max_selected=1;
 if(defname)
  {
  for(i=0;i<nofiles;i++) if(compstrs(files[i],defname)>=0) break;
  if(i<nofiles && compstrs(files[i],defname)==0) b_fnames->sel_arr[i]=1;
  b_fnames->start_no=i-b_fnames->no_rows;
  if(b_fnames->start_no<0) b_fnames->start_no=0;
  }
 else b_fnames->start_no=0;
 checkmem(b_up=MALLOC(sizeof(struct w_b_press)));
 checkmem(b_down=MALLOC(sizeof(struct w_b_press)));
 b_up->l_pressed_routine=b_up->l_routine=uio_files_one_up;
 b_up->r_pressed_routine=b_up->r_routine=uio_files_page_up;
 b_up->delay=40; b_up->repeat=15; 
 b_down->l_pressed_routine=b_down->l_routine=uio_files_one_down;
 b_down->r_pressed_routine=b_down->r_routine=uio_files_page_down;
 b_down->delay=40; b_down->repeat=15;
 checkmem(b_up2=MALLOC(sizeof(struct w_b_press)));
 checkmem(b_down2=MALLOC(sizeof(struct w_b_press)));
 *b_up2=*b_up; *b_down2=*b_down;
 b_up2->l_pressed_routine=b_up2->l_routine=uio_dirs_one_up;
 b_up2->r_pressed_routine=b_up2->r_routine=uio_dirs_page_up;
 b_down2->l_pressed_routine=b_down2->l_routine=uio_dirs_one_down;
 b_down2->r_pressed_routine=b_down2->r_routine=uio_dirs_page_down;
 xsize=20+ws_pixstrlen("M")*12*2;
 path_max_length=xsize-6;
 checkmem(b[0]=w_addstdbutton(w,w_b_string,x,y,xsize,-1,NULL,b_name,1));
 checkmem(b[1]=w_addstdbutton(w,w_b_press,x,b[0]->ypos+b[0]->ysize+
  w_titlebarheight()+2,xsize/2,-1,"/\\",b_up2,1));
 checkmem(b[4]=w_addstdbutton(w,w_b_press,x+xsize/2,b[1]->ypos,
  xsize/2,-1,"/\\",b_up,1));
 checkmem(b[2]=w_addstdbutton(w,w_b_list,x,b[1]->ypos+b[1]->ysize,
  xsize/2,no_rows*(w_titlebarheight()+2),NULL,b_dirs,1));
 checkmem(b[5]=w_addstdbutton(w,w_b_list,x+xsize/2,b[1]->ypos+b[1]->ysize,
  xsize/2,no_rows*(w_titlebarheight()+2),NULL,b_fnames,1));
 checkmem(b[3]=w_addstdbutton(w,w_b_press,x,b[2]->ypos+b[2]->ysize,
  xsize/2,-1,"\\/",b_down2,1));
 checkmem(b[6]=w_addstdbutton(w,w_b_press,x+xsize/2,b[2]->ypos+
  b[2]->ysize,xsize/2,-1,"\\/",b_down,1));
 for(i=0;i<7;i++) b[i]->data=b;
 return xsize;
 }
 
/* Get a filename. Start with path *path and filename defname, only
 files with extension ext are allowed (there is more than one extension
 allowed, each must be three characters long and they must be seperated
 with an arbitrary character) The window has the title 'title'
 and if overwrite_warning is 1, you get a warning before the filename
 is accepted. The last path is saved in *path. */
char *getfilename(char **path,const char *defname,const char *ext,
 const char *title,int overwrite_warning)
 {
 FILE *tf;
 char **files,**dirs,*fname,*fullpath,*pnt,*oldpnt,buffer[256];
 int i,no,dno,xsize,ok_cancel;
 struct w_window w,*ow;
 struct w_button *b[9];
 struct w_b_press b_ok,b_cancel;
 my_assert(path!=NULL);
 if(defname)
  if((pnt=strrchr(defname,'\\'))==NULL && (pnt=strrchr(defname,'/'))==NULL)
   pnt=(char *)defname;
  else pnt++;
 else pnt=NULL;
 strcpy(current_path,*path); strcpy(current_ext,ext);
 files=ws_getallfilenames(current_path,current_ext,&no);
 dirs=ws_getalldirs(current_path,"*",&dno);
 if(no<0 || dno<0)
  {
  if(no==-2 || dno==-2) waitmsg(TXT_ILLEGALPATH,current_path);
  if(no==-1 || dno==-1) waitmsg(TXT_CANTFINDPATH,current_path);
  ws_makepath("/",current_path); 
  files=ws_getallfilenames(current_path,current_ext,&no);
  dirs=ws_getalldirs(current_path,"*",&dno);
  if(no==-2 || dno==-2) {waitmsg(TXT_ILLEGALPATH,current_path); return NULL;}
  if(no==-1 || dno==-1) {waitmsg(TXT_CANTFINDPATH,current_path); return NULL;}
  }
 w.xpos=w.ypos=-1;
 w.xsize=26+ws_pixstrlen("M")*12*2;
 w.ysize=w_ymaxwinsize()*2/3;
 w.maxxsize=w.maxysize=-1; w.shrunk=0; w.title=title; w.buttons=wb_drag;
 w.refresh=wr_normal;
 w.refresh_routine=NULL; w.close_routine=NULL; w.click_routine=NULL; 
 checkmem(ow=w_openwindow(&w));
 b_ok.delay=0; b_ok.repeat=-1;
 b_ok.l_pressed_routine=b_ok.r_pressed_routine=b_ok.l_routine=b_ok.r_routine=
  NULL;
 b_cancel=b_ok;
 my_assert((xsize=makefilebuttons(ow,0,w_titlebarheight()+2,w_ywininsize(ow),
  files,no,dirs,dno,pnt,b))>0);
 w_resizeinwin(ow,xsize,b[6]->ypos+b[6]->ysize);
 checkmem(b[7]=w_addstdbutton(ow,w_b_press,0,0,w_xwininsize(ow)/2, 
  w_titlebarheight()+2,TXT_OK,&b_ok,1));
 checkmem(b[8]=w_addstdbutton(ow,w_b_press,b[7]->xsize,0,
  w_xwininsize(ow)/2,b[7]->ysize,TXT_CANCEL,&b_cancel,1));
 uio_drawpath(ow,b[0]->xpos,b[0]->ypos+b[0]->ysize+1,w_xwininsize(ow));
 ok_cancel=w_handleuser(2,&b[7],1,&ow,0,NULL,NULL);
 fname=b[0]->d.str->str;
 for(i=0;i<b[2]->d.sls->no_strings;i++) FREE(b[2]->d.sls->strings[i]);
 FREE(b[2]->d.sls->strings);
 for(i=0;i<b[5]->d.sls->no_strings;i++) FREE(b[5]->d.sls->strings[i]);
 FREE(b[5]->d.sls->strings);
 for(i=0;i<7;i++) FREE(b[i]->d.d);
 w_closewindow(ow);
 if(ok_cancel!=0 || strlen(fname)==0) { FREE(fname); return NULL; }
 if(fname!=NULL)
  {
  if((oldpnt=pnt=strrchr(fname,'.'))==NULL) pnt=fname+strlen(fname);
  if(pnt-fname>8) pnt=fname+8;
  checkmem(fullpath=MALLOC((pnt-fname)+strlen(current_path)+6));
  *pnt=0; strcpy(fullpath,current_path); strcat(fullpath,"/"); 
  strcat(fullpath,fname);
  strcat(fullpath,"."); fullpath[strlen(fullpath)+3]=0;
  if(oldpnt) strncat(fullpath,oldpnt+1,3); 
  else { oldpnt=strchr(ext,'.'); strncat(fullpath,ext,(oldpnt ? 
   oldpnt-ext : strlen(ext))<=3 ? (oldpnt ? oldpnt-ext : strlen(ext)) : 3); }
  FREE(fname);
  checkmem(*path=REALLOC(*path,strlen(current_path)+1));
  strcpy(*path,current_path);
  }
 else fullpath=NULL;
 if(overwrite_warning && fullpath!=NULL)
  {
  if((tf=fopen(fullpath,"r"))!=NULL)
   { 
   fclose(tf); 
   sprintf(buffer,TXT_OVERWRITEFILE,fullpath);
   if(!w_okcancel(buffer,TXT_OK,TXT_CANCEL,NULL))
    { FREE(fullpath); fullpath=NULL; }
   }
  }
 return fullpath;
 }

struct hoglevel 
 {
 char name[14],fullpath[256]; 
 unsigned long pos,size; 
 int secret;
 };
 
/* Return all levels in 'hogfile'. The result is stored in hognames,
  mem etc. is reserved by this function. Returned is the number of levels
  (negative if the hog-file is the original Descent.hog) */
int uio_getallhoglevels(FILE *hogfile,struct hoglevel ***hognames)
 { 
 char signature[4],name[14];
 unsigned long size;
 int hognum=0,i,origdescent=0;
 *hognames=NULL;
 if(fread(signature,1,3,hogfile)!=3) 
  { waitmsg(TXT_NOHOGSIGNATURE); return 0; }
 signature[3]=0;
 if(strcmp(signature,"DHF")) { waitmsg(TXT_NOHOGSIGNATURE); return 0; }
 while(fread(name,1,13,hogfile)==13)
  {
  name[13]=0;
  for(i=0;i<strlen(name);i++) name[i]=toupper(name[i]);
  if(strcmp(name,FIRSTHOGFILE)==0) origdescent=1;
  if(fread(&size,4,1,hogfile)!=1) break;
  if(strlen(name)>=5 && strstr(init.alllevelexts,&name[strlen(name)-3])!=NULL)
   {
   checkmem(*hognames=REALLOC(*hognames,sizeof(struct hoglevel *)*
    (++hognum)));
   checkmem((*hognames)[hognum-1]=MALLOC(sizeof(struct hoglevel)));
   strcpy((*hognames)[hognum-1]->name,name);
   (*hognames)[hognum-1]->pos=ftell(hogfile);
   (*hognames)[hognum-1]->size=size;
   (*hognames)[hognum-1]->secret=0;
   }
  if(fseek(hogfile,size,SEEK_CUR)) break;
  }
 return origdescent ? -hognum : hognum;
 }
 
void uio_sel_hoglevs(struct w_button *b) 
 {
 int i;
 struct w_button **b_arr=b->data;
 struct w_b_strlist *bh=b_arr[11]->d.sls;
 for(i=0;i<strlen(b->d.str->str);i++)
  b->d.str->str[i]=toupper(b->d.str->str[i]);
 for(i=0;i<bh->no_strings;i++)
  bh->sel_arr[i]=ws_matchname(b->d.str->str,bh->strings[i]);
 w_drawbutton(b_arr[11]);
 }

void uio_hoglev_one_up(struct w_button *b) { uio_updown(b->data,11,-1); }
void uio_hoglev_one_down(struct w_button *b) { uio_updown(b->data,11,1); }
void uio_hoglev_page_up(struct w_button *b) { uio_updown(b->data,11,-2); }
void uio_hoglev_page_down(struct w_button *b) { uio_updown(b->data,11,2); }

char *hogfilename=NULL;
struct hoglevel **hoglev_arr;
int hoglevnum;

void uio_rebuild_hoglist(struct w_button *b,int curpnt)
 {
 struct w_b_strlist *bh=b->d.sls;
 int i;
 for(i=0;i<bh->no_strings;i++) FREE(bh->strings[i]);
 FREE(bh->strings); FREE(bh->sel_arr);
 bh->no_strings=bh->max_selected=hoglevnum; 
 if(!hoglevnum)
  { bh->start_no=0; bh->strings=NULL; bh->sel_arr=NULL; }
 else
  {
  bh->start_no=curpnt+bh->no_rows>=bh->no_strings ? 
   bh->no_strings-bh->no_rows-1 : curpnt;
  if(bh->start_no<0) bh->start_no=0;
  checkmem(bh->strings=MALLOC(sizeof(char *)*bh->no_strings));
  checkmem(bh->sel_arr=CALLOC(bh->no_strings,1));
  for(i=0;i<bh->no_strings;i++) 
   {
   checkmem(bh->strings[i]=MALLOC(strlen(hoglev_arr[i]->name)+
    (hoglev_arr[i]->secret ? 4 : 1)));
   strcpy(bh->strings[i],hoglev_arr[i]->name);
   if(hoglev_arr[i]->secret)
    { strcat(bh->strings[i],",00");
      bh->strings[i][strlen(bh->strings[i])-2]=hoglev_arr[i]->secret/10+'0';
      bh->strings[i][strlen(bh->strings[i])-1]=hoglev_arr[i]->secret%10+'0'; }
   }
  }
 w_drawbutton(b);
 }
 
void uio_hogfile_exclude(struct w_button *b)
 {
 struct w_b_strlist *bh=((struct w_button **)b->data)[11]->d.sls;
 int i,j,removed;
 for(i=0,removed=0;i<bh->no_strings;i++)
  if(bh->sel_arr[i])
   {
   if(hoglevnum>1)
    {
    for(j=0;j<hoglevnum;j++)
     if(hoglev_arr[j]->secret==i+1) 
      { hoglev_arr[j]->secret=0; 
        waitmsg(TXT_CHANGEDSECRETLEV,hoglev_arr[j]->name); }
     else if(hoglev_arr[j]->secret>i) hoglev_arr[j]->secret--;
    memmove(&hoglev_arr[i-removed],&hoglev_arr[i-removed+1],
     (hoglevnum-i+removed-1)*sizeof(struct hoglevel *));
    checkmem(hoglev_arr=REALLOC(hoglev_arr,(--hoglevnum)*
     sizeof(struct hoglevel *)));
    }
   else { FREE(hoglev_arr); hoglevnum=0; }
   removed++;
   }
 uio_rebuild_hoglist(((struct w_button **)b->data)[11],0);
 }
 
void uio_hogfile_addlev(struct w_button *b,int secret)
 {
 struct w_b_strlist *bf=((struct w_button **)b->data)[5]->d.sls,
  *bh=((struct w_button **)b->data)[11]->d.sls;
 int i,startlev,num_sel;
 if(!secret)
  for(i=0,startlev=bh->no_strings;i<bh->no_strings;i++) 
   { if(bh->sel_arr[i] || hoglev_arr[i]->secret) { startlev=i; break; } }
 else startlev=bh->no_strings;
 for(i=0,num_sel=0;i<bf->no_strings;i++) num_sel+=bf->sel_arr[i];
 if(!num_sel) return;
 hoglevnum+=num_sel;
 checkmem(hoglev_arr=REALLOC(hoglev_arr,sizeof(struct hoglevel *)*
  hoglevnum));
 memmove(&hoglev_arr[startlev+num_sel],&hoglev_arr[startlev],
  (hoglevnum-startlev-num_sel)*sizeof(struct hoglevel *));  
 for(i=startlev+num_sel;i<hoglevnum;i++)
  if(hoglev_arr[i]->secret>startlev) hoglev_arr[i]->secret+=num_sel;
 for(i=0;i<bf->no_strings;i++)
  if(bf->sel_arr[i])
   {
   bf->sel_arr[i]=0;
   checkmem(hoglev_arr[startlev]=MALLOC(sizeof(struct hoglevel)));
   strcpy(hoglev_arr[startlev]->name,bf->strings[i]);
   hoglev_arr[startlev]->pos=hoglev_arr[startlev]->size=0;
   hoglev_arr[startlev]->secret=secret;
   strcpy(hoglev_arr[startlev]->fullpath,current_path);
   strcat(hoglev_arr[startlev]->fullpath,"/");
   strcat(hoglev_arr[startlev]->fullpath,bf->strings[i]);
   startlev++;
   }
 uio_rebuild_hoglist(((struct w_button **)b->data)[11],startlev-num_sel);
 w_drawbutton(((struct w_button **)b->data)[5]);
 }
 
void uio_hogfile_include(struct w_button *b)
 { uio_hogfile_addlev(b,0); }
 
void uio_hogfile_secret(struct w_button *b)
 { 
 int i,no;
 if(sscanf(b->d.str->str," %d",&no)!=1) return;
 if(no<1 || no>hoglevnum) return;
 for(i=0;i<hoglevnum;i++)
  if(hoglev_arr[i]->secret==no)
   { printmsg(TXT_DOUBLESECRETLEVEL,no,hoglev_arr[no-1]->name,
      hoglev_arr[i]->name); return; }
 uio_hogfile_addlev(b,no);
 }
 
void uio_hogfile_extract(struct w_button *b)
 { 
 struct w_b_strlist *bh=((struct w_button **)b->data)[11]->d.sls,
  *bf=((struct w_button **)b->data)[5]->d.sls;
 char fullpath[255],*levdat;
 int i;
 FILE *f;
 for(i=0;i<bh->no_strings;i++)
  if(bh->sel_arr[i] && hoglev_arr[i]->size>0)
   {
   bh->sel_arr[i]=0;
   strcpy(fullpath,current_path); strcat(fullpath,"/");
   strcat(fullpath,hoglev_arr[i]->name);
   if((f=fopen(fullpath,"rb"))!=NULL && !yesnomsg(TXT_OVERWRITEFILE,fullpath))
    { fclose(f); return; }
   if(f) fclose(f);
   if((f=fopen(hogfilename,"rb"))==NULL)
    { waitmsg(TXT_CANTOPENHOGFILE,hogfilename); return; }
   if(fseek(f,hoglev_arr[i]->pos,SEEK_SET))
    { waitmsg(TXT_CANTFINDLEVINHOG,hoglev_arr[i]->name,hogfilename);
      fclose(f); return; }
   checkmem(levdat=MALLOC(hoglev_arr[i]->size));
   if(fread(levdat,1,hoglev_arr[i]->size,f)!=hoglev_arr[i]->size)
    { waitmsg(TXT_CANTFINDLEVINHOG,hoglev_arr[i]->name,hogfilename);
      fclose(f); FREE(levdat); return; }
   fclose(f);
   if((f=fopen(fullpath,"wb"))==NULL)
    { waitmsg(TXT_CANTWRITEHOGLEV,hoglev_arr[i]->name,fullpath); FREE(levdat);
      return; }
   if(fwrite(levdat,1,hoglev_arr[i]->size,f)!=hoglev_arr[i]->size)
    { waitmsg(TXT_CANTWRITEHOGLEV,hoglev_arr[i]->name,fullpath); FREE(levdat);
      fclose(f); return; }
   FREE(levdat); fclose(f);
   }
 for(i=0;i<bf->no_strings;i++) FREE(bf->strings[i]);
 FREE(bf->strings); FREE(bf->sel_arr);
 bf->strings=ws_getallfilenames(current_path,current_ext,&bf->no_strings);
 my_assert(bf->no_strings>=0);
 if(bf->strings) qsort(bf->strings,bf->no_strings,sizeof(char *),qs_compstrs);
 bf->start_no=0;
 checkmem(bf->sel_arr=CALLOC(bf->no_strings,1)); 
 w_drawbutton(((struct w_button **)b->data)[5]);
/* printmsg(TXT_LEVELSEXTRACTED,hogfilename); 
   gibt murks auf dem bildschirm, weil der knopf noch gedrueckt ist. */
 }
 
void uio_hogfile_write(struct w_button *b)
 {
 int i,j,samefile=0,num_secrets;
 FILE *sf=NULL,*df,*lf,*mf;
 char *newhogname,*levdat=NULL,msnfname[256],buffer[1024],ext[4];
 struct w_button **b_arr=b->data;
 if(hoglevnum<=0) { printmsg(TXT_NOHOGLEVELS); return; }
 for(i=0;i<strlen(b_arr[17]->d.str->str);i++)
  if(b_arr[17]->d.str->str[i]!=' ') break;
 if(i==strlen(b_arr[17]->d.str->str))
  { printmsg(TXT_NOMISSIONNAME); return; }
 strcpy(buffer,current_path); strcpy(ext,current_ext);
 newhogname=getfilename(&init.missionpath,hogfilename,"HOG","HOG-File",1);
 strcpy(current_path,buffer); strcpy(current_ext,ext);
 if(newhogname==NULL) return;
 if(hogfilename && strcmp(newhogname,hogfilename)==0)
  { strcpy(&newhogname[strlen(newhogname)-3],"NEW"); samefile=1; }
 ws_splitpath(newhogname,NULL,NULL,msnfname,NULL);
 if(strcmp(msnfname,"DESCENT")==0 || strcmp(msnfname,"DESCENT2")==0)
  { printmsg(TXT_CANTOVERWRITEORIGHOG); return; }
 if((df=fopen(newhogname,"wb"))==NULL)
  { waitmsg(TXT_CANTOPENHOGFILE,newhogname); FREE(newhogname); return; }
 if(fwrite("DHF",1,3,df)!=3)
  { waitmsg(TXT_CANTWRITESIGNATURE,newhogname); fclose(df); FREE(newhogname);
    return; }
 strcpy(msnfname,newhogname); strcpy(&msnfname[strlen(msnfname)-3],MSNEXT);
 if((mf=fopen(msnfname,"w"))==NULL)
  { waitmsg(TXT_CANTOPENMSNFILE,msnfname); fclose(df); FREE(newhogname);
    return; }
 fprintf(mf,"name = %s\n",b_arr[17]->d.str->str);
 fprintf(mf,"type = %s\n",b_arr[18]->d.s->on ? "anarchy" : "normal");
 for(i=0,num_secrets=0;i<hoglevnum;i++)
  if(hoglev_arr[i]->secret) num_secrets++;
 fprintf(mf,"num_levels = %d\n",hoglevnum-num_secrets);
 my_assert(hoglev_arr!=NULL);
 for(i=0;i<hoglevnum;i++)
  {
  if(i==hoglevnum-num_secrets)
   fprintf(mf,"num_secrets = %d\n",num_secrets);
  if(hoglev_arr[i]->size>0)
   {
   my_assert(hogfilename!=NULL);
   if(sf==NULL && (sf=fopen(hogfilename,"rb"))==NULL)
    { waitmsg(TXT_CANTOPENHOGFILE,hogfilename); fclose(df); FREE(newhogname);
      fclose(mf); return; }
   if(fseek(sf,hoglev_arr[i]->pos,SEEK_SET))
    waitmsg(TXT_CANTFINDLEVINHOG,hoglev_arr[i]->name,hogfilename);
   else
    {
    checkmem(levdat=MALLOC(hoglev_arr[i]->size));
    if(fread(levdat,1,hoglev_arr[i]->size,sf)!=hoglev_arr[i]->size)
     { waitmsg(TXT_CANTREADLEVFROMHOG,hoglev_arr[i]->name,hogfilename);
       FREE(levdat); levdat=NULL; }
    }
   }
  else
   {
   my_assert(hoglev_arr[i]->fullpath);
   if((lf=fopen(hoglev_arr[i]->fullpath,"rb"))==NULL)
    waitmsg(TXT_CANTREADLEVEL,hoglev_arr[i]->fullpath);
   else
    {
    if(fseek(lf,0,SEEK_END))
     waitmsg(TXT_CANTREADLEVEL,hoglev_arr[i]->fullpath);
    else
     {
     hoglev_arr[i]->size=ftell(lf);
     if(fseek(lf,0,SEEK_SET))
      waitmsg(TXT_CANTREADLEVEL,hoglev_arr[i]->fullpath);
     else
      {
      checkmem(levdat=MALLOC(hoglev_arr[i]->size));
      if(fread(levdat,1,hoglev_arr[i]->size,lf)!=hoglev_arr[i]->size)
       { waitmsg(TXT_CANTREADLEVEL,hoglev_arr[i]->fullpath);
         FREE(levdat); levdat=NULL; }
      }
     }
    fclose(lf);
    }
   }
  if(levdat)
   {
   for(j=strlen(hoglev_arr[i]->name)+1;j<13;j++) hoglev_arr[i]->name[j]=0;
   if(fwrite(hoglev_arr[i]->name,1,13,df)!=13 ||
    fwrite(&hoglev_arr[i]->size,4,1,df)!=1 ||
    fwrite(levdat,1,hoglev_arr[i]->size,df)!=hoglev_arr[i]->size)
    waitmsg(TXT_CANTWRITEHOGLEV,hoglev_arr[i]->name,newhogname);
   FREE(levdat); levdat=NULL;
   if(hoglev_arr[i]->secret) fprintf(mf,"%s,%.2d\n",hoglev_arr[i]->name,
    hoglev_arr[i]->secret);
   else fprintf(mf,"%s\n",hoglev_arr[i]->name);
   }
  }
 fclose(df); fclose(mf); 
 if(sf) fclose(sf);
 if(samefile) { remove(hogfilename); rename(newhogname,hogfilename); }
 hogfilename=newhogname;
 }
 
void uio_hogfile_read(struct w_button *b)
 {
 struct w_button **b_arr=b->data;
 FILE *f;
 int origdescent=0,i,equal,levnum,secret_num,nomsnfile=0;
 char *newhogname,msnname[256],buffer[1024],ext[4],*start,*pos;
 struct hoglevel *swap;
 strcpy(buffer,current_path); strcpy(ext,current_ext);
 newhogname=getfilename(&init.missionpath,hogfilename,"HOG","HOG-File",0);
 strcpy(current_path,buffer); strcpy(current_ext,ext);
 if(newhogname==NULL) return;
 FREE(hogfilename); hogfilename=newhogname;
 for(i=0;i<hoglevnum;i++) FREE(hoglev_arr[i]);
 FREE(hoglev_arr); hoglevnum=0; hoglev_arr=NULL;
 if((f=fopen(hogfilename,"rb"))!=NULL)
  { 
  hoglevnum=uio_getallhoglevels(f,&hoglev_arr); 
  if(hoglevnum<0) { origdescent=1; hoglevnum=-hoglevnum; }
  fclose(f);
  }
 if(hoglevnum>0) w_activatebutton(b_arr[13]);
 if(!origdescent && hoglevnum>0)
  {
  strcpy(msnname,hogfilename); strcpy(&msnname[strlen(msnname)-3],MSNEXT);
  levnum=0;
  if((f=fopen(msnname,"r"))!=NULL)
   {
   while(fscanf(f,"\n%[^\n]",buffer)==1)
    {
    if((pos=strchr(buffer,';'))!=NULL) *pos=0;
    for(i=0;i<strlen(buffer);i++) buffer[i]=toupper(buffer[i]);
    if((start=strchr(buffer,'='))!=NULL) { equal=1; *(start++)=0; }
    else { start=buffer; equal=0; }
    while(*start==' ') start++;
    pos=&start[strlen(start)-1];
    while(*pos==' ') *(pos--)=0;
    if(equal)
     {
     if(strstr(buffer,"NAME")!=NULL)
      { strncpy(b_arr[17]->d.str->str,start,b_arr[17]->d.str->max_length);
        b_arr[17]->d.str->str[b_arr[17]->d.str->max_length]=0;
	w_drawbutton(b_arr[17]); }
     else if(strstr(buffer,"TYPE")!=NULL)
      { if(strstr(start,"ANARCHY")!=NULL) b_arr[18]->d.s->on=1;
        w_drawbutton(b_arr[18]); }
     }
    else if(strlen(start)>0)
     {
     if((pos=strchr(start,','))!=NULL) 
      { *pos=0; if(sscanf(pos+1," %d",&secret_num)!=1) secret_num=0; }
     else secret_num=0;
     for(i=0;i<hoglevnum;i++) if(strcmp(hoglev_arr[i]->name,start)==0) break;
     if(i<hoglevnum)
      {
      swap=hoglev_arr[levnum]; hoglev_arr[levnum++]=hoglev_arr[i];
      hoglev_arr[i]=swap;
      hoglev_arr[i]->secret=secret_num;
      }
     }
    }
   fclose(f);
   }
  else nomsnfile=1;
  }
 uio_rebuild_hoglist(b_arr[11],0);
 if(nomsnfile && !origdescent) printmsg(TXT_CANTOPENMSNFILE,msnname);
 }
 
void hogfilemanager(void)
 {
 char **files,**dirs,msnname[MAX_MSNNAME_LEN+1],secretno[3];
 int i,no,dno,no_rows,xsize;
 struct w_window w,*ow;
 struct w_button *b[20];
 struct w_b_press b_close,b_read,b_write,b_extract,b_include,b_exclude,
  b_hf_up,b_hf_down; 
 struct w_b_strlist b_hoglevels;
 struct w_b_string b_hoglevname,b_msnname,b_secret; 
 struct w_b_switch b_anarchy;
 hoglevnum=0; hoglev_arr=NULL;
 strcpy(current_path,init.levelpath); strcpy(current_ext,init.levelext); 
 files=ws_getallfilenames(current_path,current_ext,&no);
 dirs=ws_getalldirs(current_path,"*",&dno);
 if(no<0 || dno<0)
  {
  if(no==-2 || dno==-2) waitmsg(TXT_ILLEGALPATH,current_path);
  if(no==-1 || dno==-1) waitmsg(TXT_CANTFINDPATH,current_path);
  ws_makepath("/",current_path); 
  files=ws_getallfilenames(current_path,current_ext,&no);
  dirs=ws_getalldirs(current_path,"*",&dno);
  if(no==-2 || dno==-2) { waitmsg(TXT_ILLEGALPATH,current_path); return; }
  if(no==-1 || dno==-1) { waitmsg(TXT_CANTFINDPATH,current_path); return; }
  }
 w.xpos=w.ypos=-1;
 xsize=ws_pixstrlen("M")*12;
 w.xsize=378; w.ysize=w_ymaxwinsize()*3/4;
 w.maxxsize=w.maxysize=-1; w.shrunk=0; w.title="HOG-File Manager";
 w.buttons=wb_drag; w.refresh=wr_normal;
 w.refresh_routine=NULL; w.close_routine=NULL; w.click_routine=NULL; 
 checkmem(ow=w_openwindow(&w));
 b_msnname.max_length=MAX_MSNNAME_LEN; b_msnname.offset=0;
 b_msnname.str=msnname; msnname[0]=0;
 b_msnname.allowed_char=isprint;
 b_msnname.l_char_entered=b_msnname.r_char_entered=
  b_msnname.l_string_entered=b_msnname.r_string_entered=NULL;
 b_hoglevname.max_length=12;
 checkmem(b_hoglevname.str=MALLOC(13)); b_hoglevname.str[0]=0;
 b_hoglevname.allowed_char=isgraph;
 b_hoglevname.l_char_entered=b_hoglevname.r_char_entered=NULL;
 b_hoglevname.l_string_entered=b_hoglevname.r_string_entered=uio_sel_hoglevs;
 checkmem(b[9]=w_addstdbutton(ow,w_b_string,0,w_titlebarheight()+6,
  xsize+40,-1,NULL,&b_hoglevname,1));
 b[9]->data=b;
 b_hf_up.delay=40; b_hf_up.repeat=15;
 b_hf_up.l_pressed_routine=b_hf_up.l_routine=uio_hoglev_one_up;
 b_hf_up.r_pressed_routine=b_hf_up.r_routine=uio_hoglev_page_up;
 b_hf_down.delay=40; b_hf_down.repeat=15;
 b_hf_down.l_pressed_routine=b_hf_down.l_routine=uio_hoglev_one_down;
 b_hf_down.r_pressed_routine=b_hf_down.r_routine=uio_hoglev_page_down;
 no_rows=w_ywininsize(ow)/(w_titlebarheight()+2)-4;
 b_hoglevels.no_strings=0; b_hoglevels.strings=NULL;
 b_hoglevels.no_rows=no_rows; b_hoglevels.start_no=0;
 checkmem(b_hoglevels.sel_arr=MALLOC(hoglevnum));
 for(i=0;i<hoglevnum;i++) b_hoglevels.sel_arr[i]=0;
 b_hoglevels.l_string_selected=b_hoglevels.r_string_selected=NULL;
 b_hoglevels.max_selected=b_hoglevels.no_strings;
 checkmem(b[10]=w_addstdbutton(ow,w_b_press,0,b[9]->ypos+b[9]->ysize,
  b[9]->xsize,-1,"/\\",&b_hf_up,1));
 checkmem(b[11]=w_addstdbutton(ow,w_b_list,0,b[10]->ypos+b[10]->ysize,
  b[9]->xsize,no_rows*(w_titlebarheight()+2),NULL,&b_hoglevels,1));
 checkmem(b[12]=w_addstdbutton(ow,w_b_press,0,b[11]->ypos+b[11]->ysize,
  b[9]->xsize,-1,"\\/",&b_hf_down,1));
 b[10]->data=b[11]->data=b[12]->data=b;
 w_drawbuttonbox(ow,b[10]->xpos+b[10]->xsize,b[9]->ypos,
  100,b[12]->ypos+b[12]->ysize-b[9]->ypos);
 b_close.delay=0; b_close.repeat=-1;
 b_close.l_pressed_routine=b_close.r_pressed_routine=b_close.l_routine=
  b_close.r_routine=NULL;
 b_exclude=b_include=b_extract=b_read=b_write=b_close;
 b_include.l_pressed_routine=uio_hogfile_include;
 b_exclude.l_pressed_routine=uio_hogfile_exclude;
 b_extract.l_pressed_routine=uio_hogfile_extract;
 b_write.l_pressed_routine=uio_hogfile_write;
 b_read.l_pressed_routine=uio_hogfile_read;
 b_anarchy.on=0; b_anarchy.l_routine=b_anarchy.r_routine=NULL;
 b_secret.max_length=2; b_secret.offset=0;
 b_secret.str=secretno; b_secret.allowed_char=isdigit; secretno[0]=0;
 b_secret.l_char_entered=b_secret.r_char_entered=
  b_secret.r_string_entered=NULL;
 b_secret.l_string_entered=uio_hogfile_secret;
 checkmem(b[16]=w_addstdbutton(ow,w_b_press,b[9]->xpos+b[9]->xsize+5,
  b[12]->ypos-30,90,-1,TXT_BUT_CLOSE,&b_close,1));
 checkmem(b[7]=w_addstdbutton(ow,w_b_press,b[16]->xpos,
  b[16]->ypos-(b[16]->ysize+2)*4,90,-1,TXT_BUT_OPENHOG,&b_read,1));
 checkmem(b[8]=w_addstdbutton(ow,w_b_press,b[16]->xpos,
  b[16]->ypos-(b[16]->ysize+2)*3,90,-1,TXT_BUT_SAVEHOG,&b_write,1));
 checkmem(b[13]=w_addstdbutton(ow,w_b_press,b[16]->xpos,
  b[16]->ypos-(b[16]->ysize+2)*6,90,-1,TXT_BUT_EXTRACT,&b_extract,1));
 checkmem(b[15]=w_addstdbutton(ow,w_b_press,b[16]->xpos,
  b[16]->ypos-(b[16]->ysize+2)*8,90,-1,"->",&b_exclude,1));
 checkmem(b[14]=w_addstdbutton(ow,w_b_press,b[16]->xpos,
  b[16]->ypos-(b[16]->ysize+2)*9,90,-1,"<-",&b_include,1));
 checkmem(b[19]=w_addstdbutton(ow,w_b_string,b[16]->xpos,
  b[16]->ypos-(b[16]->ysize+2)*10-5,90,-1,"<-Secret:",&b_secret,1));
 b[7]->data=b[8]->data=b[13]->data=b[14]->data=b[15]->data=b[19]->data=b;
 my_assert(makefilebuttons(ow,b[7]->xpos+b[7]->xsize+5,b[9]->ypos,
  w_ywininsize(ow),files,no,dirs,dno,NULL,b)>0);
 checkmem(b[17]=w_addstdbutton(ow,w_b_string,0,0,
  w_xwininsize(ow)-90,b[9]->ypos,TXT_BUT_MISSIONNAME,&b_msnname,1));
 checkmem(b[18]=w_addstdbutton(ow,w_b_switch,b[17]->xsize,0,
  90,b[9]->ypos,TXT_BUT_ANARCHY,&b_anarchy,1));
 b[5]->d.sls->max_selected=b[5]->d.sls->no_strings;  
 uio_drawpath(ow,b[0]->xpos,b[0]->ypos+b[0]->ysize+1,b[0]->xsize);
 w_deactivatebutton(b[13]);
 w_handleuser(1,&b[16],1,&ow,0,NULL,NULL);
 for(i=0;i<b[2]->d.sls->no_strings;i++) FREE(b[2]->d.sls->strings[i]);
 FREE(b[2]->d.sls->strings);
 for(i=0;i<b[5]->d.sls->no_strings;i++) FREE(b[5]->d.sls->strings[i]);
 FREE(b[5]->d.sls->strings);
 FREE(b_hoglevels.sel_arr); FREE(b_hoglevname.str);
 for(i=0;i<7;i++) FREE(b[i]->d.d);
 for(i=0;i<hoglevnum;i++) FREE(hoglev_arr[i]);
 FREE(hoglev_arr);
 w_closewindow(ow);
 }
