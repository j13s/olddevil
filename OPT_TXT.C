/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    opt_txt.c - the texture/animation button handling
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
#include "calctxt.h"
#include "readtxt.h"
#include "grfx.h"
#include "tag.h"
#include "plot.h"
#include "do_move.h"
#include "options.h"
#include "opt_txt.h"

extern struct w_window *optionwins[in_number];

void b_changetexture(struct infoitem *i,long no,int withtagged);

int txt_win_open=-1,txtpos[4][2];
struct w_window txt_w;
#define TXT_REALSIZE 64
#define TXT_EDGE 2
#define TXT_XSIZE (TXT_REALSIZE+2*TXT_EDGE)
#define TXT_YSIZE (TXT_REALSIZE+2*TXT_EDGE)
struct txt_list_win tl_win[tlw_num];
const char *tl_title[tlw_num]={ "Texture 1","Texture 2","Animation",
 TXT_TLW_THINGTEXTURE };
#define TLW_TM_NUM_OPTIONS 3 
const char *tlw_tm_options[TLW_TM_NUM_OPTIONS]={ TXT_TLW_ALL,TXT_TLW_LEVEL,
 TXT_TLW_USER };
const char *tlw_modes[2]={ TXT_TLW_TXTMODE,TXT_TLW_LISTMODE };
const char *tlw_zoom[3]={ "/1","/2","/4" };
struct txtwin_savedata tw_savedata[tlw_num];
 
/* searches for the list tlw the next valid number. starts at offset
 in steps add. returns a tlw->maxnum if it reaches the end and a
 -1 if it reaches the start of the list. Offset must be a valid value-incdec*/
int next_txtno(struct txt_list_win *tlw,int offset,int add)
 {
 int incdec=add>0 ? 1 : -1,rdlno;
 enum txttypes tt;
 switch(tlw->type)
  {
  case tlw_t1: tt=view.pcurrcube && view.pcurrcube->d.c->nc[view.currwall] ? 
   txt1_wall : txt1_normal; break;
  case tlw_t2: tt=view.pcurrcube && view.pcurrcube->d.c->nc[view.currwall] ? 
   txt2_wall : txt2_normal; break;
  case tlw_thing: tt=txt_thing; break;
  case tlw_anim: tt=txt_door; break;
  default: my_assert(0); exit(2);
  }
 rdlno=offset>=0 && offset<tlw->maxnum ? tlw->t[offset]->rdlno : -1;
 while(add!=0)
  { 
  offset+=incdec;
  if(offset<0) return -1;
  else if(offset>=tlw->maxnum) return tlw->maxnum;
  if(tlw->t[offset]->txtlistno[tt]>=0 && tlw->t[offset]->rdlno!=rdlno)
   { add-=incdec; rdlno=tlw->t[offset]->rdlno; }
  }
 return offset;
 }
   
/* don't forget the deleting of the buttons in wl_refresh if you change
 this below!!!!!! */
int tl_xtxtspace(struct txt_list_win *tlw)
 { return w_xwininsize(tlw->win); }
int tl_ytxtspace(struct txt_list_win *tlw)
 { return (w_ywininsize(tlw->win)-tlw->b_up->ysize*2); }
int tl_xnumtxt(struct txt_list_win *tlw)
 { return tl_xtxtspace(tlw)/tlw->txt_xsize; }
int tl_ynumtxt(struct txt_list_win *tlw)
 { return tl_ytxtspace(tlw)/tlw->txt_ysize; }
int tl_gettxtno(struct w_button *b)
 { struct txt_list_win *tlw=b->data;
   return next_txtno(tlw,tlw->offset,
    (b->xpos-tlw->txt_xoffset)/tlw->txt_xsize+
    (b->ypos-tlw->txt_yoffset)/tlw->txt_ysize*tl_xnumtxt(tlw)); }    
/* don't forget the deleting of the buttons in wl_refresh if you change
 this above!!!!!! */

void initdisplay(struct txt_list_win *tlw)
 {
 int i;
 long rdl_curtxt=0;
 if(tlw->display.selected==tlw->olddisplay) return;
 tlw->olddisplay=tlw->display.selected;
 if(tlw->mode.selected==0)
  {
  if(!getno(tlw->i,&rdl_curtxt,NULL)) rdl_curtxt=-10;
  if(tlw->type==tlw_anim && rdl_curtxt>=0 && rdl_curtxt<pig.num_anims) 
   rdl_curtxt=pig.anim_starts[rdl_curtxt];
  switch(tlw->display.selected)
   {
   case 0: /* all textures */
    tlw->t=pig.txtlist[txt_all]; tlw->maxnum=pig.num_txtlist[txt_all]; break;
   case 1: /* textures from level */
    tlw->t=tlw->leveltxtlist; tlw->maxnum=tlw->num_leveltxtlist; break;
   case 2: case 3: /* user defined, load/save */
    tlw->t=tlw->usertxtlist; tlw->maxnum=tlw->num_usertxtlist; break;
   default: my_assert(0); 
   }
  }
 else
  { tlw->t=pig.txtlist[txt_all]; tlw->maxnum=pig.num_txtlist[txt_all];
    rdl_curtxt=-10; }
 tlw->maxoffset=next_txtno(tlw,tlw->maxnum,-tl_xnumtxt(tlw)*
  tl_ynumtxt(tlw)); 
 if(tlw->maxoffset<0) tlw->maxoffset=0;
 tlw->txt_xoffset=0; tlw->txt_yoffset=tlw->b_up->ysize;
 tlw->offset=next_txtno(tlw,-1,1); tlw->curtxtno=-1;
 if(tlw->marked_txts) FREE(tlw->marked_txts);
 checkmem(tlw->marked_txts=CALLOC(sizeof(unsigned char),tlw->maxnum));
 if(rdl_curtxt==0 && (tlw->type==txt2_normal || tlw->type==txt2_wall)) i=0;
 else
  for(i=0;i<tlw->maxnum;i++) if(tlw->t[i]==&pig.rdl_txts[rdl_curtxt]) break;
 if(i<tlw->maxnum)
  {
  tlw->marked_txts[i]=1; 
  tlw->offset=next_txtno(tlw,i-tl_xnumtxt(tlw)*tl_ynumtxt(tlw)/2<0 ? -1 : 
   i-tl_xnumtxt(tlw)*tl_ynumtxt(tlw)/2,1);
  tlw->curtxtno=i;
  }
 }

void tl_drawmarktxt(struct w_button *b,int on)
 { struct txt_list_win *tlw=b->data;
   ws_drawbox(w_xwinincoord(tlw->win,b->xpos-1),
    w_ywinincoord(tlw->win,b->ypos-1),b->xsize+2,b->ysize+2,
    on ? view.color[HILIGHTCOLORS] : view.color[BLACK],0); }
    
void tl_refreshtxts(struct txt_list_win *tlw)
 {
 int x,y,x2,y2,i,size;
 char texture[TXT_REALSIZE*TXT_REALSIZE];
 if(!tlw->b_texture) return;
 for(y=0,i=tlw->offset;y<tl_ynumtxt(tlw);y++)
  for(x=0;x<tl_xnumtxt(tlw);x++)
   {
   switch(tlw->zoom.selected)
    {
    case 0: size=64;
     memset(tlw->txt_buffer[y*tl_xnumtxt(tlw)+x],view.color[BLACK],64*64);
     if(i<tlw->maxnum)
      readbitmap(tlw->txt_buffer[y*tl_xnumtxt(tlw)+x],NULL,tlw->t[i],0);
     break;
    case 1: size=32;
     if(i<tlw->maxnum)
      { memset(texture,view.color[BLACK],64*64);
        if(i<tlw->maxnum) readbitmap(texture,NULL,tlw->t[i],0);
        for(x2=0;x2<32;x2++)
         for(y2=0;y2<32;y2++)
          tlw->txt_buffer[y*tl_xnumtxt(tlw)+x][y2*32+x2]=
           texture[y2*2*TXT_REALSIZE+x2*2]; }
     else
      memset(tlw->txt_buffer[y*tl_xnumtxt(tlw)+x],view.color[BLACK],32*32);
     break;
    case 2: size=16;
     if(i<tlw->maxnum)
      { memset(texture,view.color[BLACK],64*64);
        readbitmap(texture,NULL,tlw->t[i],0);
        for(x2=0;x2<16;x2++)
         for(y2=0;y2<16;y2++)
          tlw->txt_buffer[y*tl_xnumtxt(tlw)+x][y2*16+x2]=
           texture[y2*4*TXT_REALSIZE+x2*4]; }
     else
      memset(tlw->txt_buffer[y*tl_xnumtxt(tlw)+x],view.color[BLACK],16*16);
     break;
    default: size=64; my_assert(0);
    }
   if(i<tlw->maxnum && tlw->t[i]!=NULL)
    {
    for(x2=0;x2<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE;x2++)
     if(tlw->t[i]->my_light[x2]) break;
    if(view.littlebulbson && x2<ILLUM_GRIDSIZE*ILLUM_GRIDSIZE)
     {
     for(x2=0;x2<BULBSIZE;x2++)
      for(y2=0;y2<BULBSIZE;y2++)
       tlw->txt_buffer[y*tl_xnumtxt(tlw)+x][y2*size+x2+size*(size-BULBSIZE)]=
	pig.bulb[y2*BULBSIZE+x2];
     if(tlw->t[i]->shoot_out_txt>=0 || tlw->t[i]->anim_seq>=0)
      for(x2=0;x2<BULBSIZE;x2++)
       for(y2=0;y2<BULBSIZE;y2++)
	tlw->txt_buffer[y*tl_xnumtxt(tlw)+x][y2*size+x2+
	 (size+1)*(size-BULBSIZE)]=pig.brokenbulb[y2*BULBSIZE+x2];
     }
    }
   w_drawbutton(tlw->b_texture[y*tl_xnumtxt(tlw)+x]);
   tl_drawmarktxt(tlw->b_texture[y*tl_xnumtxt(tlw)+x],i<tlw->maxnum &&
    tlw->marked_txts[i]);
   i=next_txtno(tlw,i,1);
   }
 }   
   
void tl_refresh(struct w_window *w,void *d)
 {
 struct txt_list_win *tlw=d;
 int x,y;
 if(!tlw->b_up || tlw->dontcalltwice) return;
 switch(tlw->type)
  {
  case tlw_t1: case tlw_t2:
   if(!view.pcurrcube)
    { if(!w->shrunk) w_shrinkwin(w); return; }
   break;
  case tlw_thing:
   if(!view.pcurrthing || (view.pcurrthing->d.t->type1!=tt1_robot &&
    view.pcurrthing->d.t->type1!=tt1_reactor))
    { if(!w->shrunk) w_shrinkwin(w); return; }
   break;
  case tlw_anim:
   if(!view.pcurrdoor) { if(!w->shrunk) w_shrinkwin(w); return;  }
   break;
  default: my_assert(0);
  }
 if(tlw->oldxsize!=w->xsize || tlw->oldysize!=w->ysize ||
  tlw->oldmode!=tlw->mode.selected)
  {
  if(tlw->b_texture!=NULL)
   {
   for(y=0;y<tlw->oldynumtxt;y++)
    for(x=0;x<tlw->oldxnumtxt;x++)
     { w_deletebutton(tlw->b_texture[y*tlw->oldxnumtxt+x]);
       FREE(tlw->txt_buffer[y*tlw->oldxnumtxt+x]);
       ws_freebitmap(tlw->txt_bitmap[y*tlw->oldxnumtxt+x]); }
   FREE(tlw->txt_buffer); FREE(tlw->txt_bitmap); FREE(tlw->b_texture);
   }
  if(tl_xtxtspace(tlw)%tlw->txt_xsize!=0 || 
   tl_ytxtspace(tlw)%tlw->txt_ysize!=0 || tlw->oldmode!=tlw->mode.selected)
   { w->xsize+=tlw->txt_xsize/2; w->ysize+=tlw->txt_ysize/2;
     x=tl_xnumtxt(tlw); 
     if(x*tlw->txt_xsize<2*TXT_XSIZE) 
      x=(TXT_XSIZE+tlw->txt_xsize-1)/tlw->txt_xsize*2;
     x*=tlw->txt_xsize; 
     if(x>w_xmaxwininsize(tlw->win)) 
      x=(w_xmaxwininsize(tlw->win)/tlw->txt_xsize)*tlw->txt_xsize;
     y=tl_ynumtxt(tlw); 
     if(y*tlw->txt_ysize<TXT_YSIZE) 
      y=(TXT_YSIZE+tlw->txt_ysize-1)/tlw->txt_ysize*2;
     y=y*tlw->txt_ysize+tlw->b_up->ysize*2;
     if(y>w_ymaxwininsize(tlw->win)) 
      y=((w_ymaxwininsize(tlw->win)-tlw->b_up->ysize*2)/tlw->txt_ysize)*
       tlw->txt_ysize+tlw->b_up->ysize*2;
     tlw->b_up->xsize=x-tlw->b_zoom->xpos-tlw->b_zoom->xsize;
     tlw->b_down->xsize=x-tlw->b_display->xpos-tlw->b_display->xsize;
     tlw->b_m_down->xsize=x-tlw->b_m_save->xpos-tlw->b_m_save->xsize;
     tlw->b_down->ypos=tlw->b_display->ypos=tlw->b_m_clear->ypos=
      tlw->b_m_load->ypos=tlw->b_m_save->ypos=tlw->b_m_level->ypos=
      tlw->b_m_down->ypos=y-tlw->b_down->ysize;
     tlw->dontcalltwice=1; w_resizeinwin(w,x,y); tlw->dontcalltwice=0; }
  checkmem(tlw->b_texture=MALLOC(tl_xnumtxt(tlw)*tl_ynumtxt(tlw)*
   sizeof(struct w_button *)));
  checkmem(tlw->txt_buffer=MALLOC(tl_xnumtxt(tlw)*tl_ynumtxt(tlw)*
   sizeof(char *)));
  checkmem(tlw->txt_bitmap=MALLOC(tl_xnumtxt(tlw)*tl_ynumtxt(tlw)*
   sizeof(struct ws_bitmap *)));
  for(y=0;y<tl_ynumtxt(tlw);y++)
   for(x=0;x<tl_xnumtxt(tlw);x++)
    {
    checkmem(tlw->txt_buffer[y*tl_xnumtxt(tlw)+x]=
     MALLOC((tlw->txt_xsize-TXT_EDGE*2)*(tlw->txt_ysize-TXT_EDGE*2)));
    checkmem(tlw->txt_bitmap[y*tl_xnumtxt(tlw)+x]=ws_createbitmap(
     tlw->txt_xsize-TXT_EDGE*2,tlw->txt_ysize-TXT_EDGE*2,
     tlw->txt_buffer[y*tl_xnumtxt(tlw)+x]));
    checkmem(tlw->b_texture[y*tl_xnumtxt(tlw)+x]=w_addimagebutton(tlw->win,
     w_b_press,x*tlw->txt_xsize+tlw->txt_xoffset+TXT_EDGE-1,
     y*tlw->txt_ysize+tlw->txt_yoffset+TXT_EDGE-1,-1,-1,
     tlw->txt_bitmap[y*tl_xnumtxt(tlw)+x],
     tlw->txt_bitmap[y*tl_xnumtxt(tlw)+x],&tlw->texture,1,0))
    tlw->b_texture[y*tl_xnumtxt(tlw)+x]->data=tlw;
    }
  initdisplay(tlw);
  tlw->oldxsize=w->xsize; tlw->oldysize=w->ysize;
  tlw->oldxnumtxt=tl_xnumtxt(tlw); tlw->oldynumtxt=tl_ynumtxt(tlw);
  tlw->oldmode=tlw->mode.selected;
  }
 tlw->oldxpos=w_xwinspacecoord(w->xpos); 
 tlw->oldypos=w_ywinspacecoord(w->ypos);
 w_drawbutton(tlw->b_up); 
 w_drawbutton(tlw->b_mode); w_drawbutton(tlw->b_zoom);
 if(tlw->mode.selected==0)
  { w_drawbutton(tlw->b_display); w_drawbutton(tlw->b_down); }
 else
  { w_drawbutton(tlw->b_m_clear); w_drawbutton(tlw->b_m_load);
    w_drawbutton(tlw->b_m_save); w_drawbutton(tlw->b_m_level);
    w_drawbutton(tlw->b_m_down); }
 tl_refreshtxts(tlw);
 }

void tl_close(struct w_window *w,void *d)
 {
 struct txt_list_win *tlw=d;
 int x,y;
 for(y=0;y<tlw->oldynumtxt;y++)
  for(x=0;x<tlw->oldxnumtxt;x++)
   { w_deletebutton(tlw->b_texture[y*tlw->oldxnumtxt+x]);
     FREE(tlw->txt_buffer[y*tlw->oldxnumtxt+x]);
     ws_freebitmap(tlw->txt_bitmap[y*tlw->oldxnumtxt+x]); }
 w_closewindow(w); tlw->win=NULL;
 FREE(tlw->txt_buffer); FREE(tlw->txt_bitmap); FREE(tlw->b_texture);
 tlw->b_up=NULL; tlw->b_texture=NULL; tlw->olddisplay=-1;
 FREE(tlw->marked_txts); 
 } 
 
void tl_changeoffset(struct w_button *b,int amount)
 {
 struct txt_list_win *tlw=b->data;
 int add;
 switch(amount)
  {
  case -1: add=tl_ynumtxt(tlw)>1 ? -tl_xnumtxt(tlw) : -1; break;
  case -2: add=-tl_xnumtxt(tlw)*(tl_ynumtxt(tlw)/2+1);break;
  case 1: add=tl_ynumtxt(tlw)>1 ? tl_xnumtxt(tlw) : 1; break;
  case 2: add=tl_xnumtxt(tlw)*(tl_ynumtxt(tlw)/2+1); break;
  default: my_assert(0); exit(2);
  }
 tlw->offset=next_txtno(tlw,tlw->offset,add);
 if(tlw->offset<0) tlw->offset=next_txtno(tlw,-1,1);
 else if(tlw->offset>tlw->maxoffset) 
  tlw->offset=next_txtno(tlw,tlw->maxoffset+1,-1);
 tl_refreshtxts(tlw);
 }
 
#define INSERTINLVLTXTLIST(num_rdlno) \
 for(s=0;s<tlw->num_leveltxtlist;s++) \
  if(tlw->leveltxtlist[s]->rdlno==num_rdlno) break; \
 if(s>=tlw->num_leveltxtlist) \
  { checkmem(tlw->leveltxtlist=REALLOC(tlw->leveltxtlist, \
     ++tlw->num_leveltxtlist*sizeof(struct ham_txt *))); \
    tlw->leveltxtlist[tlw->num_leveltxtlist-1]=&pig.rdl_txts[num_rdlno]; }
void tl_makelvllist(struct txt_list_win *tlw) 
 {
 struct node *n;
 int w,rdlno,s=0;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 FREE(tlw->leveltxtlist); tlw->leveltxtlist=NULL; tlw->num_leveltxtlist=0;
 switch(tlw->type)
  {
  case tlw_t1: case tlw_t2:
   if(tlw->type==tlw_t2) 
    { checkmem(tlw->leveltxtlist=MALLOC(++tlw->num_leveltxtlist*
       sizeof(struct ham_txt *)));
      tlw->leveltxtlist[0]=pig.txtlist[txt2_normal][0]; }
   for(n=l->cubes.head;n->next!=NULL;n=n->next)
    for(w=0;w<6;w++)
     if(n->d.c->walls[w])
      {
      rdlno=tlw->type==tlw_t1 ? n->d.c->walls[w]->texture1 :
       n->d.c->walls[w]->texture2;
      if((rdlno==0 && tlw->type==tlw_t2) || rdlno<0 || 
       rdlno>=pig.num_rdltxts || !pig.rdl_txts[rdlno].pig) continue;
      INSERTINLVLTXTLIST(rdlno);
      }
   break;
  case tlw_thing: 
   for(n=l->things.head;n->next!=NULL;n=n->next)
    {
    if(n->d.t->type1==tt1_robot) rdlno=((struct D2_robot *)n->d.t)->r.texture;
    else if(n->d.t->type1==tt1_reactor)
     rdlno=((struct reactor *)n->d.t)->r.texture; 
    else continue;
    if(rdlno<0||rdlno>=pig.num_rdltxts || !pig.rdl_txts[rdlno].pig) continue;
    INSERTINLVLTXTLIST(rdlno);
    }
   break;
  case tlw_anim: 
   for(n=l->doors.head;n->next!=NULL;n=n->next)
    {
    if(n->d.d->animtxt>=pig.num_anims || !pig.anims[n->d.d->animtxt])
     continue;
    rdlno=pig.anims[n->d.d->animtxt]->rdlno;
    INSERTINLVLTXTLIST(rdlno);
    }
   break;
  default: my_assert(0);
  }
 if(tlw->leveltxtlist)
  qsort(tlw->leveltxtlist,tlw->num_leveltxtlist,sizeof(struct ham_txt *),
   cmp_txts); 
 }

void tl_l_up(struct w_button *b) { tl_changeoffset(b,-1); }
void tl_r_up(struct w_button *b) { tl_changeoffset(b,-2); } 
void tl_l_down(struct w_button *b) { tl_changeoffset(b,1); } 
void tl_r_down(struct w_button *b) { tl_changeoffset(b,2); } 

void tl_change_texture(struct w_button *b,int withtagged) 
 { 
 struct txt_list_win *tlw=b->data;
 int txt_num,animnum,i,j;
 txt_num=tl_gettxtno(b);
 if(txt_num>=0 && txt_num<tlw->maxnum)
  { 
  w_refreshstart(optionwins[tlw->i->infonr]);
  if(tlw->type==tlw_anim)
   { for(animnum=0;animnum<pig.num_anims;animnum++)
      if(pig.anim_starts[animnum]==tlw->t[txt_num]->rdlno)
       { b_changetexture(tlw->i,animnum,MWT(b,withtagged)); break; } }
       else b_changetexture(tlw->i,tlw->t[txt_num]->rdlno,MWT(b,withtagged));
  w_refreshend(optionwins[tlw->i->infonr]);
  tlw->marked_txts[tlw->curtxtno]=0; 
  for(j=0,i=tlw->offset;j<tl_xnumtxt(tlw)*tl_ynumtxt(tlw);j++,
   i=next_txtno(tlw,i,1))
   if(i==tlw->curtxtno) { tl_drawmarktxt(tlw->b_texture[j],0); break; }
  tlw->curtxtno=txt_num; tl_drawmarktxt(b,1);
  tlw->marked_txts[tlw->curtxtno]=1;
  }
 }
    
void tl_l_texture(struct w_button *b) { tl_change_texture(b,0); }
void tl_r_texture(struct w_button *b) { tl_change_texture(b,1); }

unsigned short tl_readfile(unsigned short **data)
 {
 char *fname;
 FILE *f;
 unsigned short int size;
 fname=getfilename(&init.txtlistpath,NULL,init.d_ver<d2_10_sw ? "TL1" : "TL2",
  TXT_TLW_TITLELOAD,0);
 if(fname==NULL) return 0;
 if((f=fopen(fname,"rb"))==NULL)
  { printmsg(TXT_TLW_CANTLOADTL,fname); return 0; }
 if(fread(&size,sizeof(short int),1,f)!=1)
  { printmsg(TXT_TLW_CANTLOADTL,fname); fclose(f); return 0; }
 checkmem(*data=MALLOC(sizeof(short int)*size));
 if(fread(*data,sizeof(short int),size,f)!=size)
  { printmsg(TXT_TLW_CANTLOADTL,fname); fclose(f); FREE(*data); return 0; }
 fclose(f);
 return size;
 }
 
void tl_loadlist(struct txt_list_win *tlw) 
 { 
 unsigned short *data,i;
 tlw->num_usertxtlist=tl_readfile(&data);
 checkmem(tlw->usertxtlist=MALLOC(sizeof(struct ham_txt *)*
  tlw->num_usertxtlist));
 for(i=0;i<tlw->num_usertxtlist;i++)
  if(data[i]<pig.num_txtlist[txt_all]) 
   tlw->usertxtlist[i]=pig.txtlist[txt_all][data[i]];
  else checkmem(tlw->usertxtlist[i]=REALLOC(tlw->usertxtlist,
    sizeof(struct ham_txt *)*(--tlw->num_usertxtlist)));
 }

void tl_loadmarkers(struct txt_list_win *tlw) 
 { 
 unsigned short i,size,*data;
 size=tl_readfile(&data);
 for(i=0;i<tlw->maxnum;i++) tlw->marked_txts[i]=0;
 for(i=0;i<size;i++) if(data[i]<tlw->maxnum) tlw->marked_txts[data[i]]=1;
 FREE(data);
 }

void tl_savemarkers(struct txt_list_win *tlw) 
 {
 unsigned short int i,size;
 char *fname;
 FILE *f;
 for(i=0,size=0;i<tlw->maxnum;i++) size+=tlw->marked_txts[i];
 if(size==0) { printmsg(TXT_TLW_TXTLISTEMPTY); return; }
 fname=getfilename(&init.txtlistpath,NULL,init.d_ver<d2_10_sw ? "TL1" : "TL2",
  TXT_TLW_TITLESAVE,1);
 if(fname==NULL) { printmsg(TXT_TLW_TXTLISTNOTSAVED); return; }
 if((f=fopen(fname,"wb"))==NULL)
  { printmsg(TXT_TLW_CANTSAVETL,fname); return; }
 if(fwrite(&size,sizeof(short int),1,f)!=1)
  { printmsg(TXT_TLW_CANTSAVETL,fname); fclose(f); remove(fname); return; }
 for(i=0;i<tlw->maxnum;i++)
  if(tlw->marked_txts[i])
   if(fwrite(&i,sizeof(short int),1,f)!=1)
    { printmsg(TXT_TLW_CANTSAVETL,fname); fclose(f); remove(fname); return; }
 fclose(f);
 printmsg(TXT_TLW_TLSAVED,fname);
 }

void tl_selectdisplay(struct w_button *b)
 { 
 struct txt_list_win *tlw=b->data;
 switch(b->d.ls->selected)
  {
  case 0: /* all */ break;
  case 1: tl_makelvllist(tlw); break;
  case 2: tl_loadlist(tlw); break;
  default: my_assert(0);
  }
 initdisplay(tlw);
 if(tlw->win) tl_refresh(tlw->win,tlw->win->data);
 }

void tl_selectzoom(struct w_button *b)
 {
 struct txt_list_win *tlw=b->data;
 switch(b->d.ls->selected)
  {
  case 0: tlw->txt_xsize=tlw->txt_ysize=TXT_XSIZE; break;
  case 1: tlw->txt_xsize=tlw->txt_ysize=TXT_REALSIZE/2+2*TXT_EDGE; break;
  case 2: tlw->txt_xsize=tlw->txt_ysize=TXT_REALSIZE/4+2*TXT_EDGE; break;
  default: my_assert(0);
  }
 tlw->oldmode=-1; /* to refresh winsize etc.. */
 tl_refresh(tlw->win,tlw->win->data); 
 }
 
#define MARKTXT(rdlno) \
 for(s=0;s<tlw->maxnum;s++) \
  if(tlw->t[s]->rdlno==rdlno) \
   { tlw->marked_txts[s]=1; break; }
void tl_marklvllist(struct txt_list_win *tlw)
 {
 struct node *n;
 int w,rdlno,s;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 for(s=0;s<tlw->maxnum;s++) tlw->marked_txts[s]=0;
 switch(tlw->type)
  {
  case tlw_t1: case tlw_t2:
   for(n=l->cubes.head;n->next!=NULL;n=n->next)
    for(w=0;w<6;w++)
     if(n->d.c->walls[w])
      {
      rdlno=tlw->type==tlw_t1 ? n->d.c->walls[w]->texture1 :
       n->d.c->walls[w]->texture2;
      if((tlw->type==tlw_t2 && rdlno==0) || rdlno<0 || rdlno>=pig.num_rdltxts
       || !pig.rdl_txts[rdlno].pig) continue;
      MARKTXT(rdlno);
      }
   break;
  case tlw_thing: 
   for(n=l->things.head;n->next!=NULL;n=n->next)
    {
    if(n->d.t->type1==tt1_robot) rdlno=((struct D2_robot *)n->d.t)->r.texture;
    else if(n->d.t->type1==tt1_reactor)
     rdlno=((struct reactor *)n->d.t)->r.texture; 
    else continue;
    if(rdlno<0||rdlno>=pig.num_rdltxts || !pig.rdl_txts[rdlno].pig) continue;
    MARKTXT(rdlno);
    }
   break;
  case tlw_anim: 
   for(n=l->doors.head;n->next!=NULL;n=n->next)
    {
    if(n->d.d->animtxt>=pig.num_anims || !pig.anims[n->d.d->animtxt])
     continue;
    rdlno=pig.anims[n->d.d->animtxt]->rdlno;
    MARKTXT(rdlno);
    }
   break;
  default: my_assert(0);
  }
 }
 
void tl_makelist(struct txt_list_win *tlw,int func)
 { 
 int i;
 switch(func)
  {
  case 0: for(i=0;i<tlw->maxnum;i++) tlw->marked_txts[i]=0; break;
  case 1: tl_marklvllist(tlw); break;
  case 2: tl_loadmarkers(tlw); break;
  case 3: tl_savemarkers(tlw); break;
  default: my_assert(0);
  }
 if(tlw->win) tl_refresh(tlw->win,tlw->win->data);
 }
 
void tl_m_clear(struct w_button *b) { tl_makelist(b->data,0); }
void tl_m_level(struct w_button *b) { tl_makelist(b->data,1); }
void tl_m_load(struct w_button *b) { tl_makelist(b->data,2); }
void tl_m_save(struct w_button *b) { tl_makelist(b->data,3); }

void tl_l_marktxt(struct w_button *b)
 { 
 struct txt_list_win *tlw=b->data;
 int txt_num;
 txt_num=tl_gettxtno(b);
 if(txt_num>=0 && txt_num<tlw->maxnum) tlw->marked_txts[txt_num]^=1;
 tl_drawmarktxt(b,tlw->marked_txts[txt_num]);
 }

void tl_r_marktxt(struct w_button *b) { }

void tl_changemode(struct txt_list_win *tlw,int mode)
 { 
 if(tlw->oldmode==mode) return;
 switch(mode)
  {
  case 0: /* select texture */
   tlw->texture.l_routine=tl_l_texture;
   tlw->texture.r_routine=tl_r_texture;
   w_undrawbutton(tlw->b_m_clear); w_undrawbutton(tlw->b_m_load); 
   w_undrawbutton(tlw->b_m_save); w_undrawbutton(tlw->b_m_level); 
   w_undrawbutton(tlw->b_m_down);
   break;
  case 1: /* make texture list */
   tlw->texture.l_routine=tl_l_marktxt;
   tlw->texture.r_routine=tl_r_marktxt;
   w_undrawbutton(tlw->b_display); w_undrawbutton(tlw->b_down);
   break;
  default: my_assert(0);
  }
 tlw->olddisplay=-1;
 tl_refresh(tlw->win,tlw->win->data);
 }
void tl_selectmode(struct w_button *b) 
 { tl_changemode(b->data,b->d.ls->selected); }

void init_texturehandling(enum tlw_types tlw_type)
 {
 tl_win[tlw_type].type=tlw_type; 
 tl_win[tlw_type].up.delay=25; tl_win[tlw_type].up.repeat=10;
 tl_win[tlw_type].up.l_pressed_routine=
  tl_win[tlw_type].up.l_routine=tl_l_up;
 tl_win[tlw_type].up.r_pressed_routine=
  tl_win[tlw_type].up.r_routine=tl_r_up;
 tl_win[tlw_type].down.delay=25; tl_win[tlw_type].down.repeat=10;
 tl_win[tlw_type].down.l_pressed_routine=
  tl_win[tlw_type].down.l_routine=tl_l_down;
 tl_win[tlw_type].down.r_pressed_routine=
  tl_win[tlw_type].down.r_routine=tl_r_down;
 tl_win[tlw_type].texture.delay=0; tl_win[tlw_type].texture.repeat=-1;
 tl_win[tlw_type].texture.l_pressed_routine=
  tl_win[tlw_type].texture.r_pressed_routine=NULL;
 tl_win[tlw_type].texture.l_routine=tl_l_texture;
 tl_win[tlw_type].texture.r_routine=tl_r_texture;
 tl_win[tlw_type].display.d_xsize=tl_win[tlw_type].mode.d_xsize=-1;
 tl_win[tlw_type].display.num_options=TLW_TM_NUM_OPTIONS; 
 tl_win[tlw_type].display.options=tlw_tm_options; 
 tl_win[tlw_type].display.select_lroutine=
  tl_win[tlw_type].display.select_rroutine=tl_selectdisplay;
 tl_win[tlw_type].display.selected=tl_win[tlw_type].mode.selected=
  tl_win[tlw_type].zoom.selected=0;
 tl_win[tlw_type].mode.num_options=2;
 tl_win[tlw_type].mode.options=tlw_modes;
 tl_win[tlw_type].mode.select_lroutine=
  tl_win[tlw_type].mode.select_rroutine=tl_selectmode;
 tl_win[tlw_type].display.num_options=TLW_TM_NUM_OPTIONS; 
 tl_win[tlw_type].display.options=tlw_tm_options; 
 tl_win[tlw_type].display.select_lroutine=
  tl_win[tlw_type].display.select_rroutine=tl_selectdisplay;
 tl_win[tlw_type].display.selected=0;
 tl_win[tlw_type].m_clear.delay=0; tl_win[tlw_type].m_clear.repeat=-1; 
 tl_win[tlw_type].m_clear.l_pressed_routine=
  tl_win[tlw_type].m_clear.r_pressed_routine=NULL;
 tl_win[tlw_type].m_clear.l_routine=
  tl_win[tlw_type].m_clear.r_routine=tl_m_clear;
 tl_win[tlw_type].m_load=tl_win[tlw_type].m_clear;
 tl_win[tlw_type].m_load.l_routine=
  tl_win[tlw_type].m_load.r_routine=tl_m_load;
 tl_win[tlw_type].m_save=tl_win[tlw_type].m_clear;
 tl_win[tlw_type].m_save.l_routine=
  tl_win[tlw_type].m_save.r_routine=tl_m_save;
 tl_win[tlw_type].m_level=tl_win[tlw_type].m_clear;
 tl_win[tlw_type].m_level.l_routine=
  tl_win[tlw_type].m_level.r_routine=tl_m_level;
 tl_win[tlw_type].zoom.num_options=3;
 tl_win[tlw_type].zoom.options=tlw_zoom;
 tl_win[tlw_type].zoom.select_lroutine=
  tl_win[tlw_type].zoom.select_rroutine=tl_selectzoom;   
 tl_win[tlw_type].olddisplay=-1;
 tl_win[tlw_type].txt_xsize=TXT_XSIZE;
 tl_win[tlw_type].txt_ysize=TXT_YSIZE;
 tl_win[tlw_type].oldxsize=0;
 tl_win[tlw_type].dontcalltwice=0;
 tl_win[tlw_type].oldmode=tl_win[tlw_type].olddisplay=-1;
 }

void texture_list(struct infoitem *i,enum txttypes tt,int no)
 {
 struct w_window tl_wininit;
 enum tlw_types tlw_type;
 int saved_zoom=0;
 switch(tt)
  {
  case txt1_normal: case txt1_wall: tlw_type=tlw_t1; break; 
  case txt2_normal: case txt2_wall: tlw_type=tlw_t2; break;
  case txt_thing: tlw_type=tlw_thing; break;
  case txt_door: tlw_type=tlw_anim; break;
  default: my_assert(0); exit(2);
  } 
 if(!tl_win[tlw_type].win)
  {
  if(tl_win[tlw_type].oldxsize<0)
   {
   saved_zoom=tw_savedata[tlw_type].zoom;
   tl_win[tlw_type].oldxsize=0;
   }
  else if(tl_win[tlw_type].oldxsize==0)
   {
   tw_savedata[tlw_type].xpos=tw_savedata[tlw_type].ypos=0;
   tw_savedata[tlw_type].xsize=136;
   tw_savedata[tlw_type].ysize=w_ymaxwinsize();
   }
  if(tl_win[tlw_type].oldxsize==0)
   {
   tl_wininit.xpos=tw_savedata[tlw_type].xpos;
   tl_wininit.ypos=tw_savedata[tlw_type].ypos;
   tl_wininit.xsize=tw_savedata[tlw_type].xsize;
   tl_wininit.ysize=tw_savedata[tlw_type].ysize;
   tl_win[tlw_type].i=i;
   init_texturehandling(tlw_type);
   }
  else
   { tl_wininit.xpos=tl_win[tlw_type].oldxpos;
     tl_wininit.ypos=tl_win[tlw_type].oldypos;
     tl_wininit.xsize=tl_win[tlw_type].oldxsize; 
     tl_wininit.ysize=tl_win[tlw_type].oldysize;
     tl_win[tlw_type].oldxsize=0; /* for wl_refresh */
     tl_win[tlw_type].mode.selected=0; }
  tl_win[tlw_type].dontcalltwice=0;
  tl_win[tlw_type].oldmode=tl_win[tlw_type].olddisplay=-1;
  tl_wininit.maxxsize=tl_wininit.maxysize=-1;
  tl_wininit.shrunk=0; tl_wininit.hotkey=0;
  tl_wininit.title=tl_title[tlw_type];
  tl_wininit.help=NULL; tl_wininit.buttons=wb_close|wb_shrink|wb_drag|wb_size;
  tl_wininit.refresh=wr_routine; tl_wininit.data=&tl_win[tlw_type];
  tl_wininit.refresh_routine=tl_refresh;
  tl_wininit.close_routine=tl_close; tl_wininit.click_routine=NULL;
  tl_win[tlw_type].win=w_openwindow(&tl_wininit);
  checkmem(tl_win[tlw_type].b_mode=w_addstdbutton(tl_win[tlw_type].win,
   w_b_choose,0,0,-1,-1,TXT_TLW_MODE,&tl_win[tlw_type].mode,1));
  checkmem(tl_win[tlw_type].b_zoom=w_addstdbutton(tl_win[tlw_type].win,
   w_b_choose,tl_win[tlw_type].b_mode->xsize,0,-1,-1,NULL,
   &tl_win[tlw_type].zoom,1));
  checkmem(tl_win[tlw_type].b_up=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,tl_win[tlw_type].b_zoom->xpos+tl_win[tlw_type].b_zoom->xsize,0,
   w_xwininsize(tl_win[tlw_type].win)-tl_win[tlw_type].b_zoom->xpos-
   tl_win[tlw_type].b_zoom->xsize,tl_win[tlw_type].b_mode->ysize,TXT_TLW_UP,
   &tl_win[tlw_type].up,1));
  checkmem(tl_win[tlw_type].b_display=w_addstdbutton(tl_win[tlw_type].win,
   w_b_choose,0,w_ywininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_mode->ysize-1,-1,tl_win[tlw_type].b_mode->ysize,
   TXT_TLW_DISPLAY,&tl_win[tlw_type].display,1));
  checkmem(tl_win[tlw_type].b_m_clear=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,0,w_ywininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_mode->ysize-1,-1,tl_win[tlw_type].b_mode->ysize,
   TXT_TLW_CLEAR,&tl_win[tlw_type].m_clear,0));
  checkmem(tl_win[tlw_type].b_m_level=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,tl_win[tlw_type].b_m_clear->xpos+
   tl_win[tlw_type].b_m_clear->xsize,w_ywininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_mode->ysize-1,-1,tl_win[tlw_type].b_mode->ysize,
   TXT_TLW_LEVEL,&tl_win[tlw_type].m_level,0));
  checkmem(tl_win[tlw_type].b_m_load=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,tl_win[tlw_type].b_m_level->xpos+
   tl_win[tlw_type].b_m_level->xsize,w_ywininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_mode->ysize-1,-1,tl_win[tlw_type].b_mode->ysize,
   TXT_TLW_LOAD,&tl_win[tlw_type].m_load,0));
  checkmem(tl_win[tlw_type].b_m_save=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,tl_win[tlw_type].b_m_load->xpos+
   tl_win[tlw_type].b_m_load->xsize,w_ywininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_mode->ysize-1,-1,tl_win[tlw_type].b_mode->ysize,
   TXT_TLW_SAVE,&tl_win[tlw_type].m_save,0));
  checkmem(tl_win[tlw_type].b_m_down=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,tl_win[tlw_type].b_m_save->xpos+tl_win[tlw_type].b_m_save->xsize,
   tl_win[tlw_type].b_display->ypos,w_xwininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_m_save->xpos-tl_win[tlw_type].b_m_save->xsize,
   tl_win[tlw_type].b_display->ysize,TXT_TLW_DOWN,&tl_win[tlw_type].down,0));
  checkmem(tl_win[tlw_type].b_down=w_addstdbutton(tl_win[tlw_type].win,
   w_b_press,tl_win[tlw_type].b_display->xsize,
   tl_win[tlw_type].b_display->ypos,w_xwininsize(tl_win[tlw_type].win)-
   tl_win[tlw_type].b_display->xsize,tl_win[tlw_type].b_display->ysize,
   TXT_TLW_DOWN,&tl_win[tlw_type].down,1));
  tl_win[tlw_type].b_mode->data=tl_win[tlw_type].b_up->data=
   tl_win[tlw_type].b_display->data=tl_win[tlw_type].b_down->data=
   tl_win[tlw_type].b_zoom->data=tl_win[tlw_type].b_m_clear->data=
   tl_win[tlw_type].b_m_load->data=tl_win[tlw_type].b_m_save->data=
   tl_win[tlw_type].b_m_level->data=tl_win[tlw_type].b_m_down->data=
   &tl_win[tlw_type];
  initdisplay(&tl_win[tlw_type]);
  tl_changemode(&tl_win[tlw_type],0); 
  tl_refresh(tl_win[tlw_type].win,tl_win[tlw_type].win->data); 
  tl_win[tlw_type].oldtxttyp=view.pcurrcube!=NULL && 
   view.pcurrcube->d.c->nc[view.currwall]!=NULL;
  if(saved_zoom) 
   { tl_win[tlw_type].zoom.selected=saved_zoom;
     tl_selectzoom(tl_win[tlw_type].b_zoom); }
  }
 else w_wintofront(tl_win[tlw_type].win);
 }
 
/* reinit txtlist for infoitem i */
void refresh_txtlist(struct infoitem *i)
 { 
 struct txt_list_win *tlw;
 int newtxttyp=
  (view.pcurrcube!=NULL && view.pcurrcube->d.c->nc[view.currwall]!=NULL);
 if(i->multifuncnr>5) tlw=&tl_win[tlw_t2];
 else tlw=&tl_win[tlw_t1];
 if(tlw->t==NULL || tlw->win==NULL || tlw->win->shrunk || 
  tlw->oldtxttyp==newtxttyp) return;
 tlw->olddisplay=-1; tlw->oldtxttyp=newtxttyp;
 initdisplay(tlw); tl_refresh(tlw->win,tlw);
 }
 
void b_changetexture(struct infoitem *i,long no,int withtagged)
 {
 my_assert(i!=NULL && l!=NULL);
 if(withtagged<2) changedata(i,withtagged,&no);
 else tagfilter(i,withtagged-2,&no);
 if(i->type==it_texture || i->type==it_thingtexture)
  drawoptbuttons(i-i->multifuncnr+5);
 if(i->type==it_texture) drawoptbuttons(i-i->multifuncnr+10);
 drawoptbuttons(i-i->multifuncnr);
 if(view.render) plotlevel();
 }
 
/* function for the 8 top/up/down/list buttons */
void b_selecttexture(struct w_button *b,int withtagged) 
 {
 struct infoitem *i=(struct infoitem *)b->data;
 long txtno=0,rdlno=0;
 int j;
 enum txttypes tt;
 struct txt_list_win *tlw;
 my_assert(getno(i,&rdlno,NULL) && view.pcurrcube!=NULL);
 if(i->infonr==ds_thing) { tt=txt_thing; tlw=&tl_win[tlw_thing]; }
 else
  {
  if(i->multifuncnr>5)
   { tt=view.pcurrcube->d.c->nc[view.currwall]!=NULL ? txt2_wall:txt2_normal;
     tlw=&tl_win[tlw_t2]; }
  else
   { tt=view.pcurrcube->d.c->nc[view.currwall]!=NULL ? txt1_wall:txt1_normal;
     tlw=&tl_win[tlw_t1]; }
  }
 if(tlw->t==NULL) 
  { tlw->t=pig.txtlist[txt_all]; tlw->maxnum=pig.num_txtlist[txt_all];
    tlw->type=tlw-tl_win; }
 if(rdlno==0 && (tlw->type==txt2_normal || tlw->type==txt2_wall))
  txtno=1;
 else
  for(j=0,txtno=-10;j<tlw->maxnum;j++) 
   if(tlw->t[j]==&pig.rdl_txts[rdlno]) { txtno=j; break; }
 if(txtno==-10)
  txtno=(rdlno>=0 && rdlno<pig.num_rdltxts && pig.rdl_txts[rdlno].pig!=NULL
   && pig.rdl_txts[rdlno].txtlistno[tt]>=0) ?
   pig.rdl_txts[rdlno].txtlistno[tt] : 0;
 switch(i->multifuncnr)
  {
  case 1: case 6: txtno=next_txtno(tlw,-1,1); break;
  case 2: case 7: txtno=next_txtno(tlw,txtno,-1); 
   if(txtno<0) txtno=next_txtno(tlw,tlw->maxnum,-1); break;
  case 3: case 8: txtno=next_txtno(tlw,txtno,1);
   if(txtno>=tlw->maxnum) txtno=next_txtno(tlw,-1,1); break;
  case 4: case 9: texture_list(i,tt,txtno); return;
  default: my_assert(0);
  }
 if(txtno>=0 && txtno<tlw->maxnum && tlw->t[txtno]!=NULL)
  rdlno=tlw->t[txtno]->rdlno;
 b_changetexture(i,rdlno,withtagged);
 }
 
/* function for finding the texture with the name */
int bi_gettxtno(struct infoitem *i)
 {
 long no=0;
 enum txttypes tt;
 int n;
 char buffer[9];
 my_assert(getno(i,&no,NULL)!=0 && i!=NULL);
 if(sscanf(i->b->d.str->str,"%*[^(]( %ld",&no)==1) return no;
 strncpy(buffer,i->b->d.str->str,8); buffer[8]=0;
 for(n=0;n<strlen(buffer);n++)
  if(!isalnum(buffer[n])) { buffer[n]=0; break; }
 if(i->infonr==ds_thing) tt=txt_thing; 
 else
  { if(view.pcurrcube->d.c->nc[view.currwall]!=NULL) 
     tt=i->multifuncnr>5 ? txt2_wall : txt1_wall;
    else tt=i->multifuncnr>5 ? txt2_normal : txt1_normal; }
 for(n=0;n<pig.num_txtlist[tt];n++)
  if(pig.txtlist[tt][n]->pig &&
   compstrs(pig.txtlist[tt][n]->pig->name,buffer)>=0) break;
 my_assert(pig.txtlist[tt][n<pig.num_txtlist[tt] ? n : pig.num_txtlist[tt]-1]
  !=NULL);
 return pig.txtlist[tt][n<pig.num_txtlist[tt] ? n : 
  pig.num_txtlist[tt]-1]->rdlno;
 }
 
void b_chartexture(struct w_button *b,int withtagged)
 { 
 long no;
 struct infoitem *i=(struct infoitem *)b->data;
 no=bi_gettxtno(i);
 changedata(i,0,&no);
 drawoptbuttons(i-i->multifuncnr);
 }
 
void b_stringtexture(struct w_button *b,int withtagged)
 {
 long no;
 struct infoitem *i=(struct infoitem *)b->data;
 no=bi_gettxtno(i);
 b_changetexture(i,no,withtagged); 
 }
 
void drawdooranim(struct w_button *b,struct ham_txt *t,int anim,int t1,int t2,
 int t2_d)
 {
 if(t->pig && t->pig->anim_t2)
  {
  if(t1<pig.num_rdltxts && pig.rdl_txts[t1].pig!=NULL)
   readbitmap(pig.door_buffer,NULL,&pig.rdl_txts[t1],0);
  if(t!=NULL) 
   readbitmap(pig.door_buffer,&pig.pig_txts[t->pigno+anim],
    NULL,t2_d);
  }
 else
  {
  memset(pig.door_buffer,0xff,64*64); 
  if(t!=NULL) 
   readbitmap(pig.door_buffer,&pig.pig_txts[t->pigno+anim],NULL,0);
  }
 if(b!=NULL) w_drawbutton(b);
 }
     
void b_selectdooranim(struct w_button *b,int withtagged)
 { 
 struct infoitem *i=(struct infoitem *)b->data;
 unsigned char animno;
 int n,rdlno,txtno;
 struct txt_list_win *tlw=&tl_win[tlw_anim];
 my_assert(getno(i,&animno,NULL));
 if(tlw->t==NULL) 
  { tlw->t=pig.txtlist[txt_all]; tlw->maxnum=pig.num_txtlist[txt_all];
    tlw->type=tlw_anim; }
 if(animno<pig.num_anims) rdlno=pig.anim_starts[animno];
 else rdlno=pig.anim_starts[animno];
 for(n=0,txtno=-10;n<tlw->maxnum;n++) 
  if(tlw->t[n]->rdlno==rdlno) { txtno=n; break; }
 if(txtno==-10)
  txtno=(rdlno>=0 && rdlno<pig.num_rdltxts && pig.rdl_txts[rdlno].pig!=NULL
   && pig.rdl_txts[rdlno].txtlistno[txt_door]>=0) ?
   pig.rdl_txts[rdlno].txtlistno[txt_door] : 0;
 switch(i->multifuncnr)
  {
  case 1: txtno=next_txtno(tlw,-1,1); break;
  case 2: txtno=next_txtno(tlw,txtno,-1); 
   if(txtno<0) txtno=next_txtno(tlw,tlw->maxnum,-1); break;
  case 3: txtno=next_txtno(tlw,txtno,1);
   if(txtno>=tlw->maxnum) txtno=next_txtno(tlw,-1,1); break;
  case 4: texture_list(i,txt_door,txtno); break;
  default: my_assert(0);
  }
 for(n=0;n<pig.num_anims;n++) if(pig.anims[n]==tlw->t[txtno]) break;
 if(n>=pig.num_anims) n=0;
 b_changetexture(i,n,withtagged);
 }
 
int current_anim=0,add=1;
void b_l_playdooranim(struct w_button *b) 
 {
 if(view.pcurrdoor!=NULL && view.pcurrdoor->d.d->w!=NULL &&
  view.pcurrdoor->d.d->animtxt<pig.num_anims && 
  pig.anims[view.pcurrdoor->d.d->animtxt]!=NULL &&
  pig.anims[view.pcurrdoor->d.d->animtxt]->pig!=NULL)
  {
  b->d.p->repeat=50/pig.anims[view.pcurrdoor->d.d->animtxt]->pig->num_anims;
  drawdooranim(b,pig.anims[view.pcurrdoor->d.d->animtxt],current_anim,
   view.pcurrdoor->d.d->w->texture1,view.pcurrdoor->d.d->w->texture2,
   view.pcurrdoor->d.d->w->txt2_direction);
  current_anim+=add;
  if(current_anim>=pig.anims[view.pcurrdoor->d.d->animtxt]->pig->num_anims-1)
   add=-1;
  if(current_anim<=0) add=1;
  }
 }
 
void b_l_enddooranim(struct w_button *b) 
 { 
 if(view.pcurrdoor!=NULL && view.pcurrdoor->d.d->w!=NULL &&
  view.pcurrdoor->d.d->animtxt<pig.num_anims && 
  pig.anims[view.pcurrdoor->d.d->animtxt]!=NULL)
  {
  drawdooranim(b,pig.anims[view.pcurrdoor->d.d->animtxt],0,
   view.pcurrdoor->d.d->w->texture1,view.pcurrdoor->d.d->w->texture2,
   view.pcurrdoor->d.d->w->txt2_direction);
  current_anim=0; add=1;
  }
 }
 
void drawtxtlines(struct w_button *b,struct wall *w)
 {
 struct corner *c,*c0;
 int orig_x[9],orig_y[9],i;
 float l;
 ws_setclipping(w_xwinincoord(b->w,b->xpos+1),w_ywinincoord(b->w,b->ypos+1),
  w_xwinincoord(b->w,b->xpos+b->xsize-1),
  w_ywinincoord(b->w,b->ypos+b->ysize-1));
 c0=&w->corners[0];
 orig_x[0]=orig_x[1]=orig_x[2]=(c0->x[0]*64/2048)%64;
 orig_x[3]=orig_x[4]=orig_x[5]=(c0->x[0]*64/2048)%64+64;
 orig_x[6]=orig_x[7]=orig_x[8]=(c0->x[0]*64/2048)%64-64;
 orig_y[0]=orig_y[3]=orig_y[6]=(c0->x[1]*64/2048)%64;
 orig_y[1]=orig_y[4]=orig_y[7]=(c0->x[1]*64/2048)%64+64;
 orig_y[2]=orig_y[5]=orig_y[8]=(c0->x[1]*64/2048)%64-64;
 c=&w->corners[1];
 l=sqrt((float)(c->x[0]-c0->x[0])*(c->x[0]-c0->x[0])+
  (float)(c->x[1]-c0->x[1])*(c->x[1]-c0->x[1]))/32;
 if(l==0) return;
 for(i=0;i<9;i++)
  w_drawline(b->w,b->xpos+orig_x[i],b->ypos+orig_y[i],
   (int)((c->x[0]-c0->x[0])/l)+b->xpos+orig_x[i],
   (int)((c->x[1]-c0->x[1])/l)+b->ypos+orig_y[i],
   view.color[HILIGHTCOLORS+3],0);
 c=&w->corners[3];
 l=sqrt((c->x[0]-c0->x[0])*(c->x[0]-c0->x[0])+(c->x[1]-c0->x[1])*(c->x[1]-
  c0->x[1]))/32;
 if(l==0) return;
 for(i=0;i<9;i++)
  {
  w_drawline(b->w,b->xpos+orig_x[i],b->ypos+orig_y[i],
   (int)((c->x[0]-c0->x[0])/l)+b->xpos+orig_x[i],
   (int)((c->x[1]-c0->x[1])/l)+b->ypos+orig_y[i],
   view.color[HILIGHTCOLORS+4],0);
  w_drawline(b->w,b->xpos+orig_x[i]+3,b->ypos+orig_y[i]+3,
   b->xpos+orig_x[i]-3,b->ypos+orig_y[i]-3,view.color[WHITE],0);
  w_drawline(b->w,b->xpos+orig_x[i]-3,b->ypos+orig_y[i]+3,
   b->xpos+orig_x[i]+3,b->ypos+orig_y[i]-3,view.color[WHITE],0);
  }  
 ws_setclipping(-1,-1,-1,-1);
 }     

struct fitbitmap_savedata fb_savedata;
static struct fitbitmap_data
 { struct w_window *w; 
   int changed,autoshrunk; struct wall *currwall;
   char texture[64*64];
   struct ws_bitmap *bm_txt;
   struct corner c[4]; } fb_data;
 
void fb_refreshwin(struct w_window *w,void *d)
 {
 int x,y,xn,yn,xo,yo,*c=d;
 if(w->shrunk) return;
 fb_savedata.xpos=w_xwinspacecoord(w->xpos);
 fb_savedata.ypos=w_ywinspacecoord(w->ypos);
 fb_savedata.xsize=w->xsize; fb_savedata.ysize=w->ysize;
 if(!view.pcurrwall) { if(!w->shrunk) w_shrinkwin(w); return; }
 ws_setclipping(w_xwinincoord(w,0),w_ywinincoord(w,0),
  w_xwinincoord(w,0)+w_xwininsize(w)-1,w_ywinincoord(w,0)+w_ywininsize(w)-1);
 xn=w_xwininsize(w)/128+2;
 yn=w_ywininsize(w)/128+2;
 xo=w_xwininsize(w)/2+w_xwinincoord(w,0);
 yo=w_ywininsize(w)/2+w_ywinincoord(w,0);
 for(y=-yn+1;y<yn;y++) for(x=-xn+1;x<xn;x++)
  ws_copybitmap(NULL,xo+x*64,yo+y*64,fb_data.bm_txt,0,0,64,64,1);
 for(x=0;x<4;x++)
  ws_drawline(xo+fb_data.c[x].x[0]/32,yo+fb_data.c[x].x[1]/32,
   xo+fb_data.c[(x+1)&3].x[0]/32,yo+fb_data.c[(x+1)&3].x[1]/32,
   view.color[HILIGHTCOLORS + (x==0 ? 3 : (x==3 ? 4 : 1))],0);
 if(c!=NULL)
  ws_drawcircle(xo+fb_data.c[*c].x[0]/32,yo+fb_data.c[*c].x[1]/32,3,
   view.color[HILIGHTCOLORS+1],0);
 ws_setclipping(-1,-1,-1,-1);
 }
 
void fb_closewin(void)
 {
 /* if the window is shrunk we need the correct data...*/
 if(fb_data.w->shrunk) w_shrinkwin(fb_data.w);
 ws_freebitmap(fb_data.bm_txt); fb_data.bm_txt=NULL;
 fb_savedata.xpos=w_xwinspacecoord(fb_data.w->xpos);
 fb_savedata.ypos=w_ywinspacecoord(fb_data.w->ypos);
 fb_savedata.xsize=fb_data.w->xsize; fb_savedata.ysize=fb_data.w->ysize;
 w_closewindow(fb_data.w); fb_data.w=NULL; fb_data.currwall=NULL;
 if(view.movemode==mt_texture) changemovemode(mt_you);
 }
 
void fb_closewinbutton(struct w_window *w,void *d)
 {
 if(fb_data.changed && !yesnomsg(TXT_FB_CLOSEWIN)) return;
 fb_closewin();
 }
 
void fb_movetxtpic(void) { fb_refreshwin(fb_data.w,NULL); }

void fb_clickwin(struct w_window *w,void *d,struct w_event *we)
 {
 struct ws_event ws;
 int i,min,xd,yd,min_i,xo,yo;
 struct wall wall;
 if(we->x<0 || we->y<0 || we->x>=w_xwininsize(w) || we->y>=w_ywininsize(w))
  return; /* click on a system button */
 if((we->ws->buttons&ws_bt_left)==0)
  {
  move_texture_with_mouse(w_xwinincoord(w,we->x),w_ywinincoord(w,we->y),
   fb_data.c,NULL,0,0,0.0,fb_movetxtpic);
  fb_data.changed=1;
  }
 else 
  {
  /* Check if we want to move a point */
  xo=w_xwininsize(w)/2+w_xwinincoord(w,0);
  yo=w_ywininsize(w)/2+w_ywinincoord(w,0);
  for(i=0,min=16,min_i=-1;i<4;i++)
   {
   xd=xo+fb_data.c[i].x[0]/32-we->ws->x; yd=yo+fb_data.c[i].x[1]/32-we->ws->y;
   if(xd*xd+yd*yd<min) { min=xd*xd+yd*yd; min_i=i; }
   }
  if(min_i>=0)
   {
   ws_erasemouse();
   fb_data.changed=1;
   ws.x=fb_data.c[min_i].x[0]/32+xo; ws.y=fb_data.c[min_i].x[1]/32+yo; 
   do
    {
    fb_refreshwin(fb_data.w,&min_i);
    xd=ws.x; yd=ws.y; ws_mousewarp(xd,yd);
    while(xd==ws.x && yd==ws.y && !ws_getevent(&ws,0));
    if(ws.x<w_xwinincoord(w,0)) ws.x=w_xwinincoord(w,0);
    else if(ws.x>=w_xwinincoord(w,w_xwininsize(w)))
     ws.x=w_xwinincoord(w,w_xwininsize(w))-1;
    if(ws.y<w_ywinincoord(w,0)) ws.y=w_ywinincoord(w,0);
    else if(ws.y>=w_ywinincoord(w,w_ywininsize(w)))
     ws.y=w_xwinincoord(w,w_ywininsize(w))-1;
    fb_data.c[min_i].x[0]=(ws.x-xo)*32; fb_data.c[min_i].x[1]=(ws.y-yo)*32;
    }
   while(ws.buttons&ws_bt_left);
   ws_displaymouse();
   }
  /* no pnt hit, so check for a double click */
  else if(ws_getevent(&ws,-view.doubleclick))
   {
   if((ws.buttons&ws_bt_left)==0)
    if(ws_getevent(&ws,-view.doubleclick))
     if((ws.buttons&ws_bt_left)==1)
      if(ws.kbstat&ws_ks_shift)
       { if(yesnomsg(TXT_FB_RECALCUV))
          { wall=*view.pcurrwall;
	    recalcwall(view.pcurrcube->d.c,view.currwall);
            for(i=0;i<4;i++) fb_data.c[i]=view.pcurrwall->corners[i]; 
	    fb_data.changed=0; *view.pcurrwall=wall; } }
      else if(ws.kbstat&ws_ks_alt)
       { if(yesnomsg(TXT_FB_STDSHAPE))
          { for(i=0;i<4;i++) fb_data.c[i]=stdcorners[i]; 
	    fb_data.changed=1; } }
      else 
       { if(yesnomsg(TXT_FB_RESETUV))
          { for(i=0;i<4;i++) fb_data.c[i]=view.pcurrwall->corners[i]; 
	    fb_data.changed=0; } }
   }
  fb_movetxtpic();
  }
 }

void fb_initwin(void)
 {
 struct w_window wi;
 int c,cx,cy;
 if(fb_data.bm_txt) ws_freebitmap(fb_data.bm_txt);
 memcpy(fb_data.texture,pig.txt_buffer,64*64);
 checkmem(fb_data.bm_txt=ws_createbitmap(64,64,fb_data.texture));
 fb_data.changed=0; fb_data.currwall=view.pcurrwall;
 for(c=0,cx=0,cy=0;c<4;c++)
  { fb_data.c[c]=view.pcurrwall->corners[c];
    cx+=fb_data.c[c].x[0]; cy+=fb_data.c[c].x[1]; }
 cx/=4; cy/=4;
 for(c=0;c<4;c++)
  { fb_data.c[c].x[0]-=(cx/2048)*2048; fb_data.c[c].x[1]-=(cy/2048)*2048; }
 if(fb_data.w==NULL)
  { /* open the window */
  if(fb_savedata.xsize==0)
   { wi.xpos=0; wi.ypos=0; wi.xsize=w_xmaxwinsize()/2;
     wi.ysize=w_ymaxwinsize()/2; }
  else
   { wi.xpos=fb_savedata.xpos; wi.ypos=fb_savedata.ypos; 
     wi.xsize=fb_savedata.xsize; wi.ysize=fb_savedata.ysize; }
  wi.maxxsize=wi.maxysize=-1; wi.shrunk=0;
  wi.title=TXT_FB_TITLE; wi.hotkey=-1; wi.help=NULL;
  wi.buttons=wb_drag|wb_size|wb_close|wb_shrink; 
  wi.refresh=wr_routine; wi.data=NULL;
  wi.refresh_routine=fb_refreshwin;
  wi.close_routine=fb_closewinbutton; wi.click_routine=fb_clickwin;
  fb_data.autoshrunk=0;
  checkmem(fb_data.w=w_openwindow(&wi));
  }
 if(fb_data.autoshrunk && fb_data.w->shrunk && view.pcurrwall)
  { w_shrinkwin(fb_data.w); fb_data.autoshrunk=0; }
 fb_refreshwin(fb_data.w,NULL); changemovemode(mt_texture);
 }
 
void b_fitbitmap(struct w_button *b,int withtagged)
 {
 struct node *n;
 int c;
 if(view.pcurrcube==NULL || view.pcurrwall==NULL || l==NULL) return;
 if(fb_data.w!=NULL) /* close window */
  {
  fb_closewin();  l->levelsaved=0;
  view.pcurrcube->d.c->recalc_polygons[view.currwall]=1;
  for(c=0;c<4;c++)
   { view.pcurrwall->corners[c].x[0]=fb_data.c[c].x[0];
     view.pcurrwall->corners[c].x[1]=fb_data.c[c].x[1]; }
  if(withtagged)
   for(n=l->tagged[tt_wall].head;n->next!=NULL;n=n->next)
    if(n->d.n->d.c->walls[n->no%6])
     {
     for(c=0;c<4;c++)
      { n->d.n->d.c->walls[n->no%6]->corners[c].x[0]=
	 view.pcurrwall->corners[c].x[0];
        n->d.n->d.c->walls[n->no%6]->corners[c].x[1]=
         view.pcurrwall->corners[c].x[1]; }
     n->d.n->d.c->recalc_polygons[n->no%6]=1;
     }
  plotlevel(); drawopt(in_wall); drawopt(in_edge); 
  }
 else fb_initwin();
 }
 
void fb_refreshall(void)
 {
 int c;
 if(!fb_data.w || fb_data.currwall==view.pcurrwall) return;
 if(fb_data.changed && fb_data.currwall)
  { w_wintofront(fb_data.w);
    if(yesnomsg(TXT_FB_SAVE))
     for(c=0;c<4;c++)
      { fb_data.currwall->corners[c].x[0]=fb_data.c[c].x[0];
	fb_data.currwall->corners[c].x[1]=fb_data.c[c].x[1]; } }
 fb_data.currwall=view.pcurrwall;
 if(!view.pcurrwall)
  { if(!fb_data.w->shrunk) { fb_data.autoshrunk=1; w_shrinkwin(fb_data.w); } }
 else fb_initwin();
 }

void fb_move_texture(int axis,int dir)
 {
 if(fb_data.w==NULL || fb_data.w->shrunk) return;
 switch(axis)
  {
  case 0: move_texture(fb_data.c,dir*32,0); break;
  case 1: move_texture(fb_data.c,0,-dir*32); break;
  case 2: stretch_texture(fb_data.c,-dir,-dir); break;
  default: my_assert(0);
  }
 fb_movetxtpic();
 }

void fb_turn_texture(int x,int y,int z,int dir)
 {
 if(fb_data.w==NULL || fb_data.w->shrunk) return;
 switch(x)
  {
  case 0: rotate_texture(fb_data.c,dir*view.protangle); break;
  case 1: stretch_texture(fb_data.c,0,-dir); break;
  case 2: stretch_texture(fb_data.c,dir,0); break;
  }
  fb_movetxtpic();
 }

int fb_isactive(void)
 { return fb_data.w!=NULL && !fb_data.w->shrunk && view.pcurrwall!=NULL; }

void fb_tofront(void) { w_wintofront(fb_data.w); }
