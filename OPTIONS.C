/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    options.c - the option windows
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
#include "userio.h"
#include "tools.h"
#include "calctxt.h"
#include "do_side.h"
#include "plot.h"
#include "tag.h"
#include "grfx.h"
#include "do_opts.h"
#include "readtxt.h"
#include "opt_txt.h"
#include "options.h"

#include "lac_cfg.h"

void drawoptbuttons(struct infoitem *i);
void undrawoptbuttons(struct infoitem *i);

struct w_window *optionwins[in_number];
struct w_button *b_optwins[in_internal][NUM_OPTBUTTONS];

int getthingcoord(float *dno,int axis)
 {
 if(!view.pcurrthing) return 0;
 my_assert(axis>=0 && axis<3);
 *dno=view.pcurrthing->d.t->p[0].x[axis];
 return 1;
 }
 
int getavgcubelight(unsigned long *no)
 { 
 int i,j,div;
 struct node *n;
 n=view.pcurrcube;
 if(n==NULL) return 0;
 for(i=0,div=0,*no=0;i<6;i++)
  if(n->d.c->walls[i]!=NULL)
   for(j=0,div+=4;j<4;j++) *no+=n->d.c->walls[i]->corners[j].light;
 if(div>0) *no/=div;
 else *no=0;
 return 1;
 }
 
int getavgsidelight(unsigned long *no)
 { 
 int j,wallno;
 struct node *n;
 n=view.pcurrcube; wallno=view.currwall; 
 if(n==NULL || n->d.c->walls[wallno]==NULL) return 0;
 for(j=0,*no=0;j<4;j++) *no+=n->d.c->walls[wallno]->corners[j].light;
 *no/=4;
 return 1;
 }

/* change data of infoitem i to value data. If withtagged==1 change for
 all tagged objects,too. */
void changedata(struct infoitem *i,int withtagged,void *data)
 {
 struct node *n,*cn;
 int no_obj;
 if(i->sidefuncnr==-2) return;
 if(i->tagnr<in_internal) l->levelsaved=0;
 /* first the tagged objects. */
 if(withtagged && i->tagnr<in_internal) 
  {
  for(n=l->tagged[i->tagnr].head,no_obj=0;n->next!=NULL;n=n->next)
   {
   switch(i->infonr)
    {
    case ds_wall: case ds_flickeringlight:
     if(view.pcurrcube!=n->d.n || view.currwall!=n->no%6)
      if(i->sidefuncnr>=0 && i->sidefuncnr<sc_number)
       no_obj+=do_sideeffect(i->sidefuncnr,i,data,n->d.n,(int)(n->no%6),0,1);
      else no_obj+=setno(i,data,n->d.n,n->no%6);
     break;
    case ds_corner:
     if(view.pcurrcube!=n->d.n || view.currwall!=(n->no%24)/4 || 
      view.curredge!=(n->no%24)%4)
      if(i->sidefuncnr>=0 && i->sidefuncnr<sc_number)
       no_obj+=do_sideeffect(i->sidefuncnr,i,data,n->d.n,(n->no%24)/4,
        (n->no%24)%4,1);
      else no_obj+=setno(i,data,n->d.n,(n->no%24)/4,(n->no%24)%4);
     break;
    case ds_thing:
     if(view.pcurrthing && (n->d.n->d.t->type1==view.pcurrthing->d.t->type1
      || i->sidefuncnr==sc_thingtype) && view.pcurrthing!=n->d.n)
      {
      if(i->sidefuncnr>=0 && i->sidefuncnr<sc_number)
       no_obj+=do_sideeffect(i->sidefuncnr,i,data,n->d.n,0,0,1);     
      else no_obj+=setno(i,data,n->d.n);
      }
     break;
    case ds_door:
     if(view.pcurrdoor && (n->d.n->d.d->type1==view.pcurrdoor->d.d->type1
      || i->sidefuncnr==sc_walltype || i->sidefuncnr==sc_switch) &&
      view.pcurrdoor!=n->d.n)
      {
      if(i->sidefuncnr>=0 && i->sidefuncnr<sc_number)
       no_obj+=do_sideeffect(i->sidefuncnr,i,data,n->d.n,0,0,1);     
      else no_obj+=setno(i,data,n->d.n);
      }
     break;
    default:
     switch(i->tagnr)
      {
      case tt_cube: cn=view.pcurrcube; break;
      case tt_thing: cn=view.pcurrthing; break;
      case tt_door: cn=view.pcurrdoor; break;
      case tt_pnt: cn=view.pcurrpnt; break;
      default: my_assert(0); exit(2);
      }
     if(cn!=n->d.n)
      if(i->sidefuncnr>=0 && i->sidefuncnr<sc_number)
       no_obj+=do_sideeffect(i->sidefuncnr,i,data,n->d.n,0,0,1);
        /* the 1 is for dsc_switch and opendoors */
      else no_obj+=setno(i,data,n->d.n);
    }
   }
  printmsg(TXT_CHANGEDTAGGED,no_obj+1);
  }
 if(i->sidefuncnr>=0 && i->sidefuncnr<sc_number)
  do_sideeffect(i->sidefuncnr,i,data,getnode(i->infonr),view.currwall,
   view.curredge,0);
 else 
  {
  setno(i,data,NULL);
  if(i->sidefuncnr!=-1)
   waitmsg("Unkown sideeffect function number: %s %d",i->txt,i->sidefuncnr);
  }
 }
 
void light_entered(struct w_button *b,int withtagged)
 {
 struct infoitem *i=(struct infoitem *)b->data;
 float dno;
 unsigned short no;
 my_assert(getno(i,&no,NULL)) 
 if(sscanf(b->d.str->str,"%g",&dno)==1)
  {
  if(dno>200) dno=200; else if(dno<0) dno=0;
  no=(unsigned short)(dno*327.67);
  }
 if(no>theMaxLight) no=theMaxLight;
 sprintf(b->d.str->str,"%3.2f%%",no/327.67);
 if(withtagged<2) changedata(i,withtagged,&no);
 else tagfilter(i,withtagged-2,&no);
 }
void left_light_entered(struct w_button *b) { light_entered(b,MWT(b,0)); }
void right_light_entered(struct w_button *b) { light_entered(b,MWT(b,1)); }
void float_entered(struct w_button *b,int withtagged,float f,const char
 *fstr,int saveint,float lowerlimit,float upperlimit)
 {
 struct infoitem *i=(struct infoitem *)b->data;
 float newdno,dno;
 long no;
 if(!getno(i,saveint ? (void *)&no : (void *)&dno,NULL))
  { no=0; newdno=dno=0; }
 if(sscanf(b->d.str->str,"%g",&newdno)==1 && newdno>=lowerlimit &&
  newdno<=upperlimit)
  no=dno=newdno*f;
 sprintf(b->d.str->str,fstr,dno/f);
 if(withtagged<2)
  changedata(i,withtagged,saveint ? (void *)&no : (void *)&dno);
 else
  tagfilter(i,withtagged-2,saveint ? (void *)&no : (void *)&dno);
 }
void left_degrees_entered(struct w_button *b) 
 { float_entered(b,MWT(b,0),M_PI/180.0,"%6.2f",0,-360.0,360.0); }
void right_degrees_entered(struct w_button *b) 
 { float_entered(b,MWT(b,1),M_PI/180.0,"%6.2f",0,-360.0,360.0); }
void left_dlight_entered(struct w_button *b) 
 { float_entered(b,MWT(b,0),0.32,"%6.2f",1,0.0,100.0); }
void right_dlight_entered(struct w_button *b) 
 { float_entered(b,MWT(b,1),0.32,"%6.2f",1,0.0,100.0); }
void left_float_entered(struct w_button *b) 
 { float_entered(b,MWT(b,0),1.0,"%6.2f",0,-1e10,1e10); }
void right_float_entered(struct w_button *b) 
 { float_entered(b,MWT(b,1),1.0,"%6.2f",0,-1e10,1e10); }
void left_coord_entered(struct w_button *b) 
 { float_entered(b,MWT(b,0),65536.0,"%10.4f",0,-32767.0,32767.0); }
void right_coord_entered(struct w_button *b)
 { float_entered(b,MWT(b,1),65536.0,"%10.4f",0,-32767.0,32767.0); }
void l_size_entered(struct w_button *b)
 { float_entered(b,MWT(b,0),65536.0,"%10.4f",1,0.0,100.0); }
void r_size_entered(struct w_button *b)
 { float_entered(b,MWT(b,1),65536.0,"%10.4f",1,0.0,100.0); }
void l_uvcoord_entered(struct w_button *b)
 { float_entered(b,MWT(b,0),1.0,"%10.0f",1,-16.0*32*64,16.0*32*64); }
void r_uvcoord_entered(struct w_button *b)
 { float_entered(b,MWT(b,1),1.0,"%10.0f",1,-16.0*32*64,16.0*32*64); }
void int_entered(struct w_button *b,int withtagged)
 {
 struct infoitem *i=(struct infoitem *)b->data;
 int newno=0,no=0;
 getno(i,&no,NULL);
 if(sscanf(b->d.str->str,"%i",&newno)==1) no=newno;
 sprintf(b->d.str->str,"%d",no);
 if(withtagged<2) changedata(i,withtagged,&no);
 else tagfilter(i,withtagged-2,&no);
 }
void left_int_entered(struct w_button *b) { int_entered(b,MWT(b,0)); }
void right_int_entered(struct w_button *b) { int_entered(b,MWT(b,1)); }

void fl_printmask(char *s,unsigned long mask)
 { 
 int i,j,k; 
 for(i=0;i<32;i++) s[i]=(mask&(1<<i))!=0 ? '1' : '0';
 /* check for patterns */
 for(i=1;i<=16;i<<=1)
  {
  for(j=i,k=0;j<32;j++,k++)
   if(s[k]!=s[j]) break;
  if(j==32) { s[i]=0; break; }
  }
 s[32]=0; 
 }
 
void fl_mask(struct w_button *b,int withtagged)
 {
 struct infoitem *i=(struct infoitem *)b->data;
 unsigned long new_mask;
 int j,k;
 for(j=0,new_mask=0,k=0;j<32;j++,k++)
  { if(k>=strlen(b->d.str->str)) k=0; 
    if(b->d.str->str[k]=='1') new_mask|=1<<j; }
 fl_printmask(b->d.str->str,new_mask);
 b->d.str->offset=0;
 if(withtagged<2) changedata(i,withtagged,&new_mask);
 else tagfilter(i,withtagged-2,&new_mask);
 }
void l_fl_mask(struct w_button *b) { fl_mask(b,MWT(b,0)); }
void r_fl_mask(struct w_button *b) { fl_mask(b,MWT(b,1)); }

void select_button(struct w_button *b,int withtagged) 
 {
 struct infoitem *i=(struct infoitem *)b->data;
 unsigned long oldno,no;
 my_assert(getno(i,&oldno,NULL));
 if(b->d.ls->selected>=0 && b->d.ls->selected<i->od->size)
  no=i->od->data[b->d.ls->selected]->no;
 if(withtagged<2) changedata(i,withtagged,&no);
 else tagfilter(i,withtagged-2,&no);
 /* (De-)Activate the children  */
 if(oldno!=no)
  {
  b->d.ls->selected=oldno;
  undrawoptbuttons(i);
  b->d.ls->selected=no;
  drawoptbuttons(i);
  } 
 }
void left_select_button(struct w_button *b) { select_button(b,MWT(b,0)); }
void right_select_button(struct w_button *b) { select_button(b,MWT(b,1)); }

void tag_button(struct w_button *b,int withtagged) 
 {
 struct infoitem *i=(struct infoitem *)b->data;
 unsigned char *data;
 int n;
 checkmem(data=MALLOC(i->length));
 for(n=0;n<i->length;n++) data[n]=0;
 for(n=0;n<b->d.tag->num_options;n++)
  if(b->d.tag->tagged[n] && i->od && n<i->od->size && 
   i->od->data[n]->no<i->length*8) 
   data[i->od->data[n]->no/8]|=1<<(i->od->data[n]->no%8);
 if(withtagged<2) changedata(i,withtagged,data);
 else tagfilter(i,withtagged-2,data);
 FREE(data);
 }
void left_tag_button(struct w_button *b) { tag_button(b,MWT(b,0)); }
void right_tag_button(struct w_button *b) { tag_button(b,MWT(b,1)); }

void b_l_change_txt(struct w_button *b) 
 { if(MWT(b,0)<2) b_selecttexture(b,0); }
void b_r_change_txt(struct w_button *b)
 { if(MWT(b,1)<2) b_selecttexture(b,1); }
void b_l_txt_char(struct w_button *b) { if(MWT(b,0)<2) b_chartexture(b,0); }
void b_r_txt_char(struct w_button *b) { if(MWT(b,0)<2) b_chartexture(b,1); }
void b_l_txt_string(struct w_button *b) { b_stringtexture(b,MWT(b,0)); }
void b_r_txt_string(struct w_button *b) { b_stringtexture(b,MWT(b,1)); }
void b_l_fitbitmap(struct w_button *b) { b_fitbitmap(b,0); }
void b_r_fitbitmap(struct w_button *b) { b_fitbitmap(b,1); }
void b_l_change_dooranim(struct w_button *b)
 { if(MWT(b,0)<2) b_selectdooranim(b,0); }
void b_r_change_dooranim(struct w_button *b)
 { if(MWT(b,1)<2) b_selectdooranim(b,1); }

int makeoptbuttons(struct w_window *w,int num,struct infoitem *is,int y)
 {
 static int x=0,oldy=0;
 struct infoitem *i;
 struct w_b_choose *b_choose;
 struct w_b_string *b_string;
 struct w_b_tag *b_tagged;
 struct w_b_press *b_press;
 int k,n,activate,maxy,cy;
 unsigned long no;
 short sho;
 long sno;
 float dno;
 char helptxt[100];
 unsigned char *data;
 for(i=is,maxy=0;i-is<num;i++)
  {
  activate=1; no=0; dno=0; sno=0;
  switch(i->type)
   {
   case it_selbutton:  
    activate=getno(i,&no,NULL);
    checkmem(b_choose=MALLOC(sizeof(struct w_b_choose)));
    b_choose->num_options=i->od->size;
    checkmem(b_choose->options=MALLOC(sizeof(char *)*i->od->size));
    for(n=0,b_choose->selected=-1;n<i->od->size;n++)
     {
     b_choose->options[n]=i->od->data[n]->str;
     if(no==i->od->data[n]->no) b_choose->selected=n;
     }
    b_choose->select_lroutine=left_select_button;
    b_choose->select_rroutine=right_select_button;
    checkmem(i->b=w_addstdbutton(w,w_b_choose,0,y,w_xwininsize(w),-1,i->txt,
     b_choose,0));
    break;
   case it_markbutton: 
    data=getdata(i->infonr,NULL);
    if(data==NULL) activate=0;
    else { activate=1; data+=i->offset; }
    checkmem(b_tagged=MALLOC(sizeof(struct w_b_tag)));
    b_tagged->num_options=i->od->size;
    checkmem(b_tagged->options=MALLOC(sizeof(char *)*i->od->size));
    checkmem(b_tagged->tagged=MALLOC(sizeof(int)*i->od->size));
    for(n=0;n<i->od->size;n++)
     {
     b_tagged->options[n]=i->od->data[n]->str;
     b_tagged->tagged[n]=data==NULL ? 0 : 
      (i->od->data[n]->no<i->length*8 ? 
      ((data[i->od->data[n]->no/8]&(1<<(i->od->data[n]->no%8))))!=0 : 0);
     }
    b_tagged->max_options=i->multifuncnr;
    b_tagged->tag_lroutine=left_tag_button;
    b_tagged->tag_rroutine=right_tag_button;
    checkmem(i->b=w_addstdbutton(w,w_b_tag,0,y,w_xwininsize(w),-1,i->txt,
     b_tagged,0));
    break;
   case it_light: case it_cubelight: case it_sidelight: case it_deltalight:
    activate=i->type==it_light||i->type==it_deltalight ? getno(i,&no,NULL) : 
     (i->type==it_cubelight ? getavgcubelight(&no) : getavgsidelight(&no));
    checkmem(b_string=MALLOC(sizeof(struct w_b_string)));
    b_string->max_length=8;
    checkmem(b_string->str=MALLOC(sizeof(char)*(b_string->max_length+1)));
    sprintf(b_string->str,"%6.2f%%",i->type==it_deltalight ? no/0.32 :
     no/327.67);
    b_string->allowed_char=isfloat;
    b_string->l_char_entered=b_string->r_char_entered=NULL;
    b_string->l_string_entered=i->type==it_deltalight ? left_dlight_entered :
     left_light_entered;
    b_string->r_string_entered=i->type==it_deltalight ? right_dlight_entered :
     right_light_entered;
    checkmem(i->b=w_addstdbutton(w,w_b_string,0,y,w_xwininsize(w),-1,i->txt,
     b_string,0));
    break;
   case it_coord: case it_size: case it_thingcoord: case it_fl_delay:
   case it_uvcoord:
    activate=getno(i,i->type==it_size||i->type==it_fl_delay||
     i->type==it_uvcoord ? (void *)&sno : (void *)&dno,NULL);
    checkmem(b_string=MALLOC(sizeof(struct w_b_string)));
    b_string->max_length=10;
    checkmem(b_string->str=MALLOC(sizeof(char)*11));
    b_string->allowed_char=isfloat;
    b_string->l_char_entered=b_string->r_char_entered=NULL;
    switch(i->type)
     {
     case it_size: case it_fl_delay: 
      sprintf(b_string->str,"%10.4f",sno/65536.0); 
      b_string->l_string_entered=l_size_entered;
      b_string->r_string_entered=r_size_entered;
      break;
     case it_uvcoord:
      memcpy(&sho,&sno,2);
      sno=sho;
      sprintf(b_string->str,"%10.0f",sno/1.0);
      b_string->l_string_entered=l_uvcoord_entered;
      b_string->r_string_entered=r_uvcoord_entered;
      break;
     default:
      sprintf(b_string->str,"%10.4f",dno/65536.0); 
      b_string->l_string_entered=left_coord_entered;
      b_string->r_string_entered=right_coord_entered;
     }
    checkmem(i->b=w_addstdbutton(w,w_b_string,0,y,w_xwininsize(w),-1,i->txt,
     b_string,0));
    break;
   case it_degree: case it_float: 
    activate=getno(i,&dno,NULL); 
    checkmem(b_string=MALLOC(sizeof(struct w_b_string)));
    b_string->max_length=6;
    checkmem(b_string->str=MALLOC(sizeof(char)*11));
    sprintf(b_string->str,"%6.2f",(i->type==it_degree ? dno/M_PI*180 : dno));
    b_string->allowed_char=isfloat;
    b_string->l_char_entered=b_string->r_char_entered=NULL;
    b_string->l_string_entered=i->type==it_degree ? left_degrees_entered
     : left_float_entered;
    b_string->r_string_entered=i->type==it_degree ? right_degrees_entered 
     : right_float_entered;
    checkmem(i->b=w_addstdbutton(w,w_b_string,0,y,w_xwininsize(w),-1,i->txt,
     b_string,0));
    break;
   case it_int: case it_fl_mask: 
    activate=getno(i,i->type==it_int ? (void *)&sno : (void *)&no,NULL);
    checkmem(b_string=MALLOC(sizeof(struct w_b_string)));
    b_string->max_length=i->type==it_fl_mask ? 32 : 10;
    checkmem(b_string->str=MALLOC(sizeof(char)*(b_string->max_length+1)));
    if(i->type==it_fl_mask) fl_printmask(b_string->str,no);
    else sprintf(b_string->str,"%ld",sno);
    b_string->allowed_char=i->type==it_fl_mask ? isbinary : isxdigit;
    b_string->l_char_entered=b_string->r_char_entered=NULL;
    b_string->l_string_entered=i->type==it_fl_mask ?
     l_fl_mask : left_int_entered;
    b_string->r_string_entered=i->type==it_fl_mask ?
     r_fl_mask : right_int_entered;
    checkmem(i->b=w_addstdbutton(w,w_b_string,0,y,w_xwininsize(w),-1,i->txt,
     b_string,0));
    break;
   case it_texture: case it_dooranim: case it_thingtexture:
    activate=getno(i,&no,NULL);
    k=x;
    switch(i->multifuncnr)
     {
     case 0: /* texture itself */ 
      oldy=y;
      checkmem(b_press=MALLOC(sizeof(struct w_b_press)));
      b_press->delay=0; b_press->repeat=0;
      b_press->l_pressed_routine=i->type==it_dooranim?b_l_playdooranim : NULL;
      b_press->r_pressed_routine=NULL;
      b_press->l_routine=i->type==it_texture ? b_l_fitbitmap : 
       (i->type==it_dooranim ? b_l_enddooranim : NULL);
      b_press->r_routine=i->type==it_texture ? b_r_fitbitmap : NULL;
      checkmem(i->b=w_addimagebutton(w,w_b_press,0,y,-1,-1,
       i->type==it_texture ? pig.txt_bm : (i->type==it_thingtexture ?
        pig.thing_bm : pig.door_bm),
       i->type==it_texture ? pig.txt_bm : (i->type==it_thingtexture ?
        pig.thing_bm : pig.door_bm),b_press,1,0));
      x=i->b->xsize;
      break;
     case 1: case 6:
      if(i->multifuncnr==6) { n=oldy; oldy=y; y=n; }
      else y=oldy;
     case 2: case 3: case 4: case 7: case 8: case 9:
      /* up, down, top, list button */
      if(i->multifuncnr>5) k+=(w_xwininsize(w)-x)/2; 
      checkmem(b_press=MALLOC(sizeof(struct w_b_press)));
      b_press->delay=25;
      b_press->repeat=(i->multifuncnr==2||i->multifuncnr==3||
       i->multifuncnr==7||i->multifuncnr==8 ? 10 : -1);
      b_press->l_pressed_routine=b_press->r_pressed_routine=
       b_press->l_routine=b_press->r_routine=NULL;   
      if((i->multifuncnr-1)%5==1 || (i->multifuncnr-1)%5==2) 
       { /* up or down */
       b_press->l_routine=b_press->l_pressed_routine=
        i->type==it_dooranim ? b_l_change_dooranim : b_l_change_txt;
       b_press->r_routine=b_press->r_pressed_routine=
        i->type==it_dooranim ? b_r_change_dooranim : b_r_change_txt; 
       }
      else
       { b_press->l_routine=
          i->type==it_dooranim ? b_l_change_dooranim : b_l_change_txt;
         b_press->r_routine=
          i->type==it_dooranim ? b_r_change_dooranim : b_r_change_txt; }
      checkmem(i->b=w_addstdbutton(w,w_b_press,k,y,
       (w_xwininsize(w)-x)/(i->type==it_texture ? 2 : 1),x/4+
       (((i->multifuncnr-1)%5==3) ? x%4 : 0),i->txt,b_press,0));
      break;
     case 10: y=oldy;
     case 5:
      checkmem(b_string=MALLOC(sizeof(struct w_b_string)));
      b_string->max_length=16;
      checkmem(b_string->str=MALLOC(sizeof(char)*17));
      if(no<pig.num_rdltxts && pig.rdl_txts[no].pig!=NULL)
       sprintf(b_string->str,"%s (%lu)",(no==0 && i->offset==2) ? TXT_NOTHING
        : pig.rdl_txts[no].pig->name,no);
      else
       sprintf(b_string->str,TXT_UNKNOWN,no);
      b_string->allowed_char=isprint;
      b_string->l_char_entered=b_l_txt_char;
      b_string->l_string_entered=b_l_txt_string;
      b_string->r_char_entered=b_l_txt_char;
      b_string->r_string_entered=b_r_txt_string;
      checkmem(i->b=w_addstdbutton(w,w_b_string,0,y,w_xwininsize(w),-1,i->txt,
       b_string,0));
      break;
     default: my_assert(0);
     }
    break;
   default: i->b=NULL; break;
   }
  if(!activate) w_deactivatebutton(i->b);
  if(i->sidefuncnr==-2) w_dontchangebutton(i->b);
  if(i->b!=NULL)
   { i->b->data=i; y+=i->b->ysize;
     sprintf(helptxt,"Offset: %d Length: %d",i->offset,i->length);
     checkmem(i->b->helptxt=MALLOC(strlen(helptxt)+1));
     strcpy((char *)i->b->helptxt,helptxt); }
  for(k=0,cy=0;k<i->numchildren;k++)
   if((n=makeoptbuttons(w,i->numinchildren[k],i->children[k],y))>cy) cy=n;
  if(cy>maxy) maxy=cy;
  if(cy>y) y=cy;
  }
 return maxy>y ? maxy : y;
 }

void b_refreshoptwin(struct w_window *w,void *d)
 {
 struct infoitem *i=d;
 if(i==NULL || w==NULL) return;
 drawopt(i->tagnr);
 }

void b_clickoptwin(struct w_window *w,void *d,struct w_event *we)
 {
 struct infoitem *i=d;
 if(i==NULL || w==NULL) return;
 if(i->tagnr<tt_number && view.currmode!=i->tagnr) changecurrmode(i->tagnr);
 }
 
struct w_window options_win = { 0,481,140,150,300,150,300,0,NULL,0,"",
 wb_drag|wb_shrink|wb_close,wr_routine,NULL,b_refreshoptwin,NULL,
 b_clickoptwin };
void makeoptwindow(enum infos what)
 {
 int y;
 my_assert(what<in_number);
 options_win.title=init.bnames[what];
 options_win.xpos=init.xres-options_win.xsize-10;
 options_win.ypos=what*20+10;
 options_win.data=init.info[what];
 options_win.hotkey=init.bnames[what]==NULL ? 0 : 
  tolower(init.bnames[what][0]);
 checkmem(optionwins[what]=w_openwindow(&options_win));
 if(what<in_internal) y=init_prev_no_next(what);
 else y=0;
 y=makeoptbuttons(optionwins[what],init.infonum[what],init.info[what],y);
 if(y>0) w_resizeinwin(optionwins[what],w_xwininsize(optionwins[what]),y);
 w_shrinkwin(optionwins[what]);
 }

void drawoptbuttons(struct infoitem *i)
 {
 int activate=0,k,n,drawbmlines=0;
 unsigned long no=0;
 short sho=0;
 long sno=0;
 float dno=0.0;
 struct wall *w=NULL;
 unsigned char *data;
 my_assert(i!=NULL);
 if(i->b==NULL) return;
 switch(i->type)
  {
  case it_selbutton:
   if((activate=getno(i,&no,NULL))!=0)
    {
    for(n=0,i->b->d.ls->selected=-1;n<i->od->size;n++)
     if(no==i->od->data[n]->no) { i->b->d.ls->selected=n;break; }
    if(i->b->d.ls->selected<0)
     {
     printmsg("Unknown selection in %s: %d",i->txt,no);
     i->b->d.ls->selected=0;
     }
    }
   break;
  case it_markbutton: 
   if((activate=((data=getdata(i->infonr,NULL))!=NULL))!=0)
    {
    data+=i->offset;
    for(n=0;n<i->od->size;n++)
     i->b->d.tag->tagged[n]=i->od->data[n]->no<i->length*8 ? 
      ((data[i->od->data[n]->no/8]&(1<<(i->od->data[n]->no%8))))!=0 : 0;
    }
   break;
  case it_light: case it_cubelight: case it_sidelight:
   if((activate=i->type==it_light ? getno(i,&no,NULL) :
    (i->type==it_cubelight ? getavgcubelight(&no) : getavgsidelight(&no)))!=0)
    sprintf(i->b->d.str->str,"%5.2f%%",no/327.67);
   break;
  case it_thingcoord: 
   if((activate=getthingcoord(&dno,i->offset))!=0)
    sprintf(i->b->d.str->str,"%10.4f",dno/65536.0);
   break;
  case it_coord: 
   if((activate=getno(i,&dno,NULL))!=0)
    sprintf(i->b->d.str->str,"%10.4f",dno/65536.0);
   break;
  case it_uvcoord: 
   if((activate=getno(i,&sho,NULL))!=0)
   {
    sno=sho;
    sprintf(i->b->d.str->str,"%10.0f",sno/1.0);
   }
   break;
  case it_size: 
   if((activate=getno(i,&sno,NULL))!=0)
    sprintf(i->b->d.str->str,"%10.4f",sno/65536.0);
   break;
  case it_degree: 
   if((activate=getno(i,&dno,NULL))!=0)
    sprintf(i->b->d.str->str,"%6.2f",dno/M_PI*180);
   break;
  case it_float: 
   if((activate=getno(i,&dno,NULL))!=0) sprintf(i->b->d.str->str,"%6.2f",dno);
   break;
  case it_int: 
   if((activate=getno(i,&sno,NULL))!=0) sprintf(i->b->d.str->str,"%ld",sno);
   break;
  case it_deltalight: 
   if((activate=getno(i,&no,NULL))!=0)
    sprintf(i->b->d.str->str,"%6.2f%%",no/0.32);
   break;
  case it_fl_mask: 
   if((activate=getno(i,&no,NULL))!=0)
    { i->b->d.str->offset=0; fl_printmask(i->b->d.str->str,no); }
   else sprintf(i->b->d.str->str,"No data");
   break;
  case it_fl_delay:
   activate=((w=(struct wall *)getdata(ds_wall,NULL))!=NULL);
   if(activate)
    { if(!getno(i,&no,NULL)) no=0;
      sprintf(i->b->d.str->str,"%10.4f",no/65536.0); }
   break;
  case it_texture: 
   activate=((w=(struct wall *)getdata(i->infonr,NULL))!=NULL);
   if(activate) no=i->multifuncnr<=5 ? w->texture1 : w->texture2;
   if((i->multifuncnr==5||i->multifuncnr==10) && activate)
    if(no>=0 && no<pig.num_rdltxts && pig.rdl_txts[no].pig!=NULL)
     sprintf(i->b->d.str->str,"%s (%lu)",(no==0 && i->multifuncnr==10) ?
      " Nothing" : pig.rdl_txts[no].pig->name,no);
     else sprintf(i->b->d.str->str,TXT_UNKNOWN,no); 
   if(i->multifuncnr==0)
    {
    memset(pig.txt_buffer,0xff,64*64);
    if(activate)
     {
     if(w->texture1>=0 && w->texture1<pig.num_rdltxts &&
      pig.rdl_txts[w->texture1].pig)
      readbitmap(pig.txt_buffer,NULL,&pig.rdl_txts[w->texture1],0);
     if(w->texture2>0 && w->texture2<pig.num_rdltxts &&
      pig.rdl_txts[w->texture2].pig)
      readbitmap(pig.txt_buffer,NULL,&pig.rdl_txts[w->texture2],
       w->txt2_direction);
     drawbmlines=1;
     }
    }
   else if(i->multifuncnr==4 || i->multifuncnr==8) refresh_txtlist(i);
   break;
  case it_dooranim:
   activate=getno(i,&no,NULL);
   if(i->multifuncnr==0)
    { 
    memset(pig.door_buffer,0xff,64*64);
    w=view.pcurrdoor==NULL ? NULL : view.pcurrdoor->d.d->w;
    if(activate && w!=NULL && no>=0 && no<pig.num_anims && pig.anims[no]!=NULL)
     drawdooranim(NULL,pig.anims[no],0,w->texture1,w->texture2,
      w->txt2_direction);
    }
   break;
  case it_thingtexture:
   activate=getno(i,&sno,NULL);
   if(i->multifuncnr==5 && activate)
    if(sno==-1 || (sno>=0 && sno<pig.num_rdltxts && 
     pig.rdl_txts[sno].pig!=NULL))
     sprintf(i->b->d.str->str,"%s (%ld)",sno==-1 ? " Default" :
      pig.rdl_txts[sno].pig->name,sno);
    else sprintf(i->b->d.str->str,TXT_UNKNOWN,sno); 
   if(i->multifuncnr==0)
    {
    memset(pig.thing_buffer,0xff,64*64);
    if(activate && sno>=0 && sno<pig.num_rdltxts && pig.rdl_txts[sno].pig)
     readbitmap(pig.thing_buffer,NULL,&pig.rdl_txts[sno],0);
    }
   break;
  default: break;
  }
 if(i->b)
  {
  if(activate) w_activatebutton(i->b); 
  else w_deactivatebutton(i->b);
  w_drawbutton(i->b);
  if(drawbmlines && w) drawtxtlines(i->b,w);
  }
 if(i->type==it_selbutton)
  for(k=0;k<i->numchildren;k++)
   if(i->itemchildren[k]==no)
    for(n=0;n<i->numinchildren[k];n++)
     drawoptbuttons(&i->children[k][n]); 
   else
    for(n=0;n<i->numinchildren[k];n++)
     { w_setdrawbutton(i->children[k][n].b,0); 
       w_deactivatebutton(i->children[k][n].b); }
 }
 
void undrawoptbuttons(struct infoitem *i)
 {
 int k,n;
 if(i->b!=NULL) w_undrawbutton(i->b);
 if(i->type==it_selbutton)
  for(k=0;k<i->numchildren;k++)
   if(i->itemchildren[k]==i->b->d.ls->selected)
    for(n=0;n<i->numinchildren[k];n++)
     undrawoptbuttons(&i->children[k][n]); 
 }
 
void drawopt(enum infos what)
 {
 int i,j,k,act;
 my_assert(what<in_number);
 if(optionwins[what]!=NULL && !optionwins[what]->shrunk)
  {
  w_refreshstart(optionwins[what]);
  w_clearwindow(optionwins[what]);
  if(what<in_internal)
   {
   i=-1;
   if(l==NULL) act=0;
   else switch(what)
    {
    case in_cube: 
     act=l->cubes.size; 
     if(act && view.pcurrcube) 
      {
      i=view.pcurrcube->no;
      b_optwins[what][3]->d.s->on=testtag(what,view.pcurrcube);
      my_assert(view.b_levels->num_options>1);
      b_optwins[what][6]->d.ls->num_options=view.b_levels->num_options-1;
      if(b_optwins[what][6]->d.ls->selected>=
       b_optwins[what][6]->d.ls->num_options)
       { b_optwins[what][6]->d.ls->selected=0; view.pcurrmacro=NULL; }
      checkmem(b_optwins[what][6]->d.ls->options=
       REALLOC(b_optwins[what][6]->d.ls->options,
	sizeof(char *)*(view.b_levels->num_options-1)));
      b_optwins[what][6]->d.ls->options[0]=TXT_NONE;
      b_optwins[what][6]->d.ls->selected=0;
      for(j=1,k=1;j<view.b_levels->num_options;j++)
       if(view.b_levels->selected!=j)
	{
	if(view.pcurrmacro &&
	 strcmp(view.b_levels->options[j],view.pcurrmacro->fullname)==0)
	 b_optwins[what][6]->d.ls->selected=k;
	b_optwins[what][6]->d.ls->options[k++]=view.b_levels->options[j];
	}
      b_refreshtagno(what);
      }
     break;
    case in_wall: 
     act=l->cubes.size; 
     if(act && view.pcurrcube) 
      { 
      i=view.currwall;
      b_optwins[what][3]->d.s->on=testtag(what,view.pcurrcube,view.currwall);
      b_refreshtagno(what);
      if(b_optwins[what][6])
       b_optwins[what][6]->d.s->on=view.pdefcube==view.pcurrcube &&
        view.defwall==view.currwall && 
        view.pdefcube->d.c->walls[view.defwall];
      if(b_optwins[what][8])
       b_optwins[what][8]->d.s->on=
        view.pcurrcube->d.c->walls[view.currwall]!=NULL &&
        view.pcurrcube->d.c->walls[view.currwall]->locked;
      if(b_optwins[what][9])
       b_optwins[what][9]->d.s->on=
	adjust_light_nc==view.pcurrcube && adjust_light_wall==view.currwall;
      }
     break;
    case in_edge:     
     act=l->cubes.size!=0 && view.pcurrcube!=NULL && 
      view.pcurrcube->d.c->walls[view.currwall]!=NULL; 
     if(act)
      { i=view.curredge;
        b_optwins[what][3]->d.s->on=testtag(what,view.pcurrcube,view.currwall,
         view.curredge);
        b_refreshtagno(what); }
     break;
    case in_pnt:
     act=l->pts.size;
     if(act && view.pcurrpnt)
      { i=view.pcurrpnt->no; 
        b_optwins[what][3]->d.s->on=testtag(tt_pnt,view.pcurrpnt);
        b_refreshtagno(what); }
      break;
     break;
    case in_door: 
     act=l->doors.size;
     if(act && view.pcurrdoor)
      { i=view.pcurrdoor->no; 
        b_optwins[what][3]->d.s->on=testtag(tt_door,view.pcurrdoor);
        b_refreshtagno(what); }
      break;
    case in_thing:
     act=l->things.size; 
     if(act && view.pcurrthing)
      { i=view.pcurrthing->no; 
        b_optwins[what][3]->d.s->on=testtag(tt_thing,view.pcurrthing); 
        b_refreshtagno(what); }
     break;
    default: act=0; waitmsg("Unknown infotype %d in drawopt",what);
    }
   if(i>9999) sprintf(b_optwins[what][1]->d.str->str,"****");
   else if(i<0) sprintf(b_optwins[what][1]->d.str->str,"----");
   else sprintf(b_optwins[what][1]->d.str->str,"%.4d",i);
   for(i=0;i<NUM_OPTBUTTONS;i++)  
    if(b_optwins[what][i])
     {
     if(act) w_activatebutton(b_optwins[what][i]);
     else w_deactivatebutton(b_optwins[what][i]);
     }
   if((what==in_door || what==in_thing) && l!=NULL)
    if(b_optwins[what][4]) w_activatebutton(b_optwins[what][4]);
   if(what==in_wall && act)
    {
    if(view.pcurrcube->d.c->walls[view.currwall]==NULL)
     {
     if(b_optwins[what][6]) w_deactivatebutton(b_optwins[what][6]);
     if(b_optwins[what][8]) w_deactivatebutton(b_optwins[what][8]);
     }
    if(b_optwins[what][9])
     if(view.pcurrcube->d.c->walls[view.currwall]==NULL ||
      view.pcurrcube->d.c->walls[view.currwall]->ls==NULL)
      w_deactivatebutton(b_optwins[what][9]);
    }
   for(i=0;i<NUM_OPTBUTTONS;i++)
    if(b_optwins[what][i]) w_drawbutton(b_optwins[what][i]);
   }
  for(i=0;i<init.infonum[what];i++) drawoptbuttons(&init.info[what][i]);
  w_refreshend(optionwins[what]);
  }
 if(what==in_cube || what==in_wall) fb_refreshall();
 }

void drawopts(void)
 { enum infos what; for(what=0;what<in_number;what++) drawopt(what); }

/* Shrink the current mode window and bring window 'what' in the foreground*/ 
void shrinkopt_win(enum tagtypes what)
 {
 if(what!=view.currmode && !optionwins[view.currmode]->shrunk) 
  w_shrinkwin(optionwins[view.currmode]); 
 w_wintofront(optionwins[what]); 
 if(optionwins[what]->shrunk) w_shrinkwin(optionwins[what]);
 }
