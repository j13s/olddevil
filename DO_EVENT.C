/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    do_event.c - all functions, that the user can directly call with a event.
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
#include "readlvl.h"
#include "plot.h"
#include "grfx.h"
#include "options.h"
#include "tag.h"
#include "opt_txt.h"
#include "do_tag.h"
#include "do_mod.h"
#include "do_light.h"
#include "do_ins.h"
#include "do_stat.h"
#include "do_move.h"
#include "config.h"
#include "credits.h"
#include "do_event.h"
#include "lac_cfg.h"

void dec_fastquit(int ec) { savestatus(0); closegrph(); exit(1); }

void dec_quit(int evcode)
 {
 struct node *n;
 savestatus(0);
 for(n=view.levels.head;n->next!=NULL;n=n->next)
  if(!closelevel(n->d.lev,1)) return;
 closegrph(); exit(1); 
 }

void dec_loadlevel(int ec)
 {
 char *fname;
 struct leveldata *ld;
 if((fname=getfilename(&init.levelpath,NULL,init.alllevelexts,
  TXT_LOADLEVEL,0))!=NULL)
  {
  if((ld=openlevel(fname))==NULL) waitmsg(TXT_CANTOPENLVL,fname);
  else printmsg(TXT_LEVELREAD,ld->fullname,ld->filename);
  FREE(fname);
  }
 }

void dec_savelevel(int ec)
 {
 char *fname;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 if(l->filename!=NULL && init.d_ver>=d2_10_sw &&
  strchr(l->filename,'.')-l->filename==strlen(l->filename)-4)
  strcpy(&l->filename[strlen(l->filename)-3],"RL2");
 if(ec!=ec_fastsave || l->filename==NULL)
  fname=getfilename(&init.levelpath,l->filename,init.levelext,
   TXT_SAVELEVEL,1);
 else
  {checkmem(fname=MALLOC(strlen(l->filename)+1)); strcpy(fname,l->filename);}
 if(fname!=NULL)
  {
      int fulllightinfo = 0;
      if(isF2SavingAll)
          fulllightinfo = (ec==ec_savelevel);
      else
          fulllightinfo = (ec!=ec_savewithfulllightinfo);

  if(!savelevel(fname,l,1,1,init.d_ver,fulllightinfo))
   waitmsg(TXT_CANTSAVELVL,l->fullname);
  else 
   { printmsg(TXT_LEVELSAVED,l->fullname,fname);
     checkmem(l->filename=REALLOC(l->filename,strlen(fname)+1));
     strcpy(l->filename,fname); }
  FREE(fname);
  }
 plotlevel(); drawopts();
 }

void dec_playlevel(int ec) 
 { 
 if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
 if(ec==ec_savetodescdir)
  { if(savestatus(1) && saveplaymsn(1)) 
     printmsg(TXT_LEVELSAVED,l->fullname,"Devil Play Level");
    l->levelsaved=0; }
 else if(savestatus(1) && saveplaymsn(0)) { closegrph(); exit(0); }
 }

void dec_credits(int ec) 
 {
 char *txt=NULL;
 int length=0,i=-1,l;
 while(cred_txt[++i]!=NULL) 
  {
  l=ws_charstrlen(w_xmaxwinsize()-20);
  if(strlen(cred_txt[i])<l) l=strlen(cred_txt[i]);
  length+=l+1;
  checkmem(txt=REALLOC(txt,length));
  strncpy(txt+length-l-1,cred_txt[i],l);
  txt[length-1]='\n';
  }
 txt[length-1]=0;
 waitmsg(txt); 
 FREE(txt);
 }

#define NUM_KBSTATSDESCR 6 
struct kbstatdescr { int kbstat; const char *txt; }
 txt_kbstat[NUM_KBSTATSDESCR] =
 { { 1,"Shift" }, { 2,"Alt" }, { 4,"Ctrl" }, { 8,"Numlock" },
   { 0x10,"Capslock" }, { 0x20,"Scrollock" } };
#define NUM_KEYDESCR 58
struct keydescr { int key; const char *txt; } txt_key[NUM_KEYDESCR] =
 { { 27,"Esc" }, { 9,"Tab" }, { 271,"Tab" },
   { 315,"F1" }, { 316,"F2" }, { 317,"F3" }, { 318,"F4" }, { 319,"F5" },
   { 320,"F6" }, { 321,"F7" }, { 322,"F8" }, { 323,"F9" }, { 324,"F10" },   
   { 595,"Del" }, { 8,"BS" }, { 594,"Ins" }, { 32,"Space" }, { 335,"N3" },
   { 328,"N2" }, { 337,"N1" }, { 331,"N4" }, { 336,"N8" }, { 333,"N6" },
   /* some keys I want to display not in the usual way */
   { '!',"1" }, { '"',"2" }, {'õ',"3" }, { '#',"3" }, { '$',"4" },
   /* F-keys with shift: */
   { 340,"F1" }, { 341,"F2" }, { 342,"F3" }, { 343,"F4" }, { 344,"F5" }, 
   { 345,"F6" }, { 346,"F7" }, { 347,"F8" }, { 348,"F9" }, { 349,"F10" },
   { 391,"F11" }, { 392,"F12" },
   /* F-keys with ctrl: */
   { 350,"F1" }, { 351,"F2" }, { 352,"F3" }, { 353,"F4" }, { 354,"F5" },
   { 355,"F6" }, { 356,"F7" }, { 357,"F8" }, { 358,"F9" }, { 359,"F10" },
   { 393,"F11" }, { 394,"F12"}, 
   { 587,"<-" }, { 589,"->" }, { 584,"/\\" }, { 592,"\\/" } };

#define COLUMNSIZE 200   
int cur_help_pos,num_codes,num_columns;

void help_refresh(struct w_window *w)
 {
 char buffer[100];
 int i,j,o_y,x,y;
 o_y=(int)w->data; 
 ws_drawfilledbox(w_xwinincoord(w,0),w_ywinincoord(w,o_y),
  w_xwininsize(w),w_ywininsize(w)-o_y,view.color[BLACK],0);
 for(i=cur_help_pos,x=0,y=0;i<(view.num_keycodes>num_codes+cur_help_pos ? 
  num_codes+cur_help_pos : view.num_keycodes);i++,y++)
  {
  if(y==num_codes/num_columns) { y=0; x++; }
  buffer[0]=0;
  for(j=0;j<NUM_KBSTATSDESCR;j++)
   if(view.ec_keycodes[i].kbstat&txt_kbstat[j].kbstat)
    sprintf(&buffer[strlen(buffer)],"%s+",txt_kbstat[j].txt);
  for(j=0;j<NUM_KEYDESCR;j++)
   if(view.ec_keycodes[i].key==txt_key[j].key)
    { sprintf(&buffer[strlen(buffer)],"%s: ",txt_key[j].txt); break; }
  if(j>=NUM_KEYDESCR) 
   if(!isgraph(view.ec_keycodes[i].key))
    sprintf(&buffer[strlen(buffer)],"???: ");
   else sprintf(&buffer[strlen(buffer)],"%c: ",view.ec_keycodes[i].key);
  sprintf(&buffer[strlen(buffer)],"%s",view.txt_keycode[i]);  
  ws_drawtext(w_xwinincoord(w,x*COLUMNSIZE+5),
   w_ywinincoord(w,o_y+y*w_titlebarheight()),
   COLUMNSIZE-10,buffer,view.color[WHITE],-1);
  }
 }
 
void help_bup(struct w_button *b)
 { if(cur_help_pos>0) { cur_help_pos--; help_refresh(b->w); } } 
void help_btop(struct w_button *b) { cur_help_pos=0; help_refresh(b->w); }
void help_bdown(struct w_button *b)
 { if(cur_help_pos<view.num_keycodes-num_codes)
    { cur_help_pos++; help_refresh(b->w); } }
void help_bbottom(struct w_button *b)
 { cur_help_pos=view.num_keycodes-num_codes; 
   if(cur_help_pos<0) cur_help_pos=0; help_refresh(b->w); }
   
void dec_help(int ec)
 {
 struct w_window helpwin,*w;
 struct w_b_press up,down,ok;
 struct w_button *b_up,*b_down,*b_ok;
 struct w_keycode exitkey;
 num_columns=(w_xmaxwinsize()-20)/COLUMNSIZE;
 if((w_ymaxwinsize()-20)/w_titlebarheight()*num_columns>view.num_keycodes)
  num_columns=view.num_keycodes*w_titlebarheight()/(w_ymaxwinsize()-20)+1;
 helpwin.xpos=helpwin.ypos=helpwin.maxxsize=helpwin.maxysize=-1;
 helpwin.xsize=20+num_columns*COLUMNSIZE;
 helpwin.ysize=w_ymaxwinsize(); helpwin.shrunk=0; 
 helpwin.title=NULL;
 helpwin.hotkey=0; helpwin.buttons=wb_none; helpwin.refresh=wr_normal;
 helpwin.refresh_routine=NULL;
 helpwin.close_routine=NULL; helpwin.click_routine=NULL;
 checkmem(w=w_openwindow(&helpwin));
 up.delay=10; up.repeat=5; up.l_pressed_routine=up.l_routine=help_bup;
 up.r_pressed_routine=NULL; up.r_routine=help_btop;
 down=up; down.l_pressed_routine=down.l_routine=help_bdown;
 down.r_routine=help_bbottom;
 ok=up; ok.l_pressed_routine=ok.r_routine=ok.l_routine=NULL;
 checkmem(b_up=w_addstdbutton(w,w_b_press,0,0,50,-1,TXT_UP,&up,1));
 checkmem(b_down=w_addstdbutton(w,w_b_press,50,0,50,-1,TXT_DOWN,&down,1));
 checkmem(b_ok=w_addstdbutton(w,w_b_press,100,0,w_xwininsize(w)-100,-1,
  TXT_OK,&ok,1));
 w->data=(void *)(b_ok->ysize+2); cur_help_pos=0;
 num_codes=(w_ymaxwinsize()-b_ok->ysize-2)/w_titlebarheight()*num_columns;
 help_refresh(w);
 exitkey.kbstat=0; exitkey.key=27; exitkey.event=-1; exitkey.flags=w_kf_exit;
 w_handleuser(1,&b_ok,1,&w,1,&exitkey,NULL);
 w_closewindow(w);
 }

void dec_cubemode(int ec)
 { shrinkopt_win(tt_cube); changecurrmode(tt_cube); }
void dec_sidemode(int ec)
 { shrinkopt_win(tt_wall); changecurrmode(tt_wall); }
void dec_pntmode(int ec) 
 { shrinkopt_win(tt_pnt); changecurrmode(tt_pnt); }
void dec_edgemode(int ec) 
 { shrinkopt_win(tt_edge); changecurrmode(tt_edge); }
void dec_thingmode(int ec) 
 { shrinkopt_win(tt_thing); changecurrmode(tt_thing); }
void dec_wallmode(int ec)
 { shrinkopt_win(tt_door); changecurrmode(tt_door); }

void dec_movemode(int ec)
 {
   if(changeCubeEnabled)
     changemovemode(view.movemode<mt_texture-1 ? view.movemode+1 : 0);
   else
     changemovemode(view.movemode<mt_texture-2 ? view.movemode+1 : 0);
 }

void dec_gridonoff(int ec) { view.gridonoff^=1; drawopt(in_internal); }

void dec_tagall(int ec) 
 { if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
   tagallobjects(view.currmode); }
void dec_tag(int ec)
 { if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
   tagobject(view.currmode); }

void dec_movefactor(int ec) 
 {
 float f=ec==ec_incrmovefactor ? view.movescala : 1/view.movescala;
 switch(view.movemode)
  {
  case mt_you: view.movefactor*=f; break;
  case mt_obj: view.pmovefactor*=f; break;
  case mt_current: case mt_texture: break;
  default: my_assert(0);
  }
 drawopt(in_internal);
 }
 
void dec_rotangle(int ec) 
 {
 float f=ec==ec_incrrotangle ? view.rotscala : 1/view.rotscala;
 switch(view.movemode)
  {
  case mt_you: view.rotangle*=f; break;
  case mt_obj: view.protangle*=f; break;
  case mt_current: case mt_texture: break;
  default: my_assert(0);
  }
 drawopt(in_internal);
 }
 
void dec_visibility(int ec)
 { view.maxvisibility*=ec==ec_incrvisibility ? view.visscala : 
    1/view.visscala; drawopt(in_internal); plotlevel(); }
    
void dec_changefullname(int ec)
 { 
 struct w_window w,*ow;
 struct w_button *b;
 struct w_b_string s;
 int i;
 char buffer[256];
 struct leveldata *ld=l;
 if(l==NULL) { printmsg(TXT_NOLEVEL); return; }
 w.shrunk=0; w.title=TXT_LVLNAME; w.hotkey=0; w.help=NULL;
 w.buttons=wb_drag; w.refresh=wr_normal; w.data=NULL; w.refresh_routine=NULL;
 w.close_routine=NULL; w.click_routine=NULL;
 w.xpos=-1; w.ypos=-1; w.maxxsize=w.maxysize=-1;
 s.d_xsize=-1; s.max_length=30; 
 for(i=0;i<view.b_levels->num_options;i++)
  if(view.b_levels->options[i]==l->fullname) break;
 if(i>=view.b_levels->num_options) 
  { waitmsg("Error searching levelname..."); return; }
 checkmem(s.str=MALLOC(s.max_length+1));
 if(l->fullname)
  { strncpy(s.str,l->fullname,s.max_length); s.str[s.max_length]=0; }
 else s.str[0]=0;
 s.allowed_char=isprint;
 s.l_char_entered=s.r_char_entered=s.l_string_entered=s.r_string_entered=NULL;
 checkmem(b=w_addstdbutton(NULL,w_b_string,0,0,-1,-1,TXT_LVLNAME,&s,0));
 w_winoutsize(&w,b->xsize,b->ysize);
 checkmem(ow=w_openwindow(&w));
 w_buttoninwin(ow,b,1);
 w_handleuser(1,&b,1,&ow,0,NULL,NULL);
 w_closewindow(ow); 
 FREE(l->fullname);
 checkmem(view.b_levels->options[i]=l->fullname=MALLOC(strlen(s.str)+1));
 strcpy(l->fullname,s.str); FREE(s.str);
 sprintf(buffer,"%s",l->fullname);
 w_changewintitle(l->w,buffer);
 qsort(&view.b_levels->options[1],view.b_levels->num_options-1,sizeof(char *),
  qs_compstrs);
 changecurrentlevel(NULL); changecurrentlevel(ld);
 }
 
void dec_nextlevel(int ec)
 {
 if(!l)
  { if(view.levels.size>0) changecurrentlevel(view.levels.tail->d.lev); }
 else
  changecurrentlevel((l->n->next->next==NULL ? view.levels.head : 
   l->n->next)->d.lev);
 }
 
void dec_prevlevel(int ec)
 {
 if(!l)
  { if(view.levels.size>0) changecurrentlevel(view.levels.head->d.lev); }
 else
  changecurrentlevel((l->n->prev->prev==NULL ? view.levels.tail : 
   l->n->prev)->d.lev);
 }
 
void dec_movethingtocube(int ec)
 {
 struct thing *t;
 int i,j;
 if(!view.pcurrcube) { printmsg(TXT_NOCURRCUBE); return; }
 if(!view.pcurrthing) { printmsg(TXT_NOCURRTHING); return; }
 t=view.pcurrthing->d.t;
 for(j=0;j<3;j++)
  {
  t->p[0].x[j]=0.0;
  for(i=0;i<8;i++)
   t->p[0].x[j]+=view.pcurrcube->d.c->p[i]->d.p->x[j]/8.0;
  }
 setthingpts(t); t->nc=view.pcurrcube; l->levelsaved=0;
 plotlevel();
 }
 
void dec_drawwhat(int ec)
 {
 switch(ec)
  {
  case ec_drawalllines: view.drawwhat^=DW_ALLLINES; 
   if(!(view.drawwhat&DW_ALLLINES)) initdescmap(); break;
  default: my_assert(0);
  }
 plotlevel();
 }
 
void dec_missionmanager(int ec) { hogfilemanager(); }
 
void dec_savewinpos(int ec)
 { 
 FILE *f;
 char *fname;
 int i;
 for(i=0;i<tlw_num;i++)
  if(tl_win[i].win!=NULL) break;
 if(i!=tlw_num) { printmsg(TXT_CLOSETXTWIN); return; }
 fname=getfilename(&init.txtlistpath,NULL,"WIN",TXT_SAVEWINPOS,1);
 if(fname==NULL) return;
 if((f=fopen(fname,"w"))==NULL || !w_savecfg(f)) 
  { printmsg(TXT_CANTSAVEWINPOS,fname); return; }
 for(i=0;i<tlw_num;i++)
  if(tl_win[i].oldxsize!=0)
   fprintf(f," %d %d %d %d %d\n",tl_win[i].zoom.selected,
    tl_win[i].oldxpos,tl_win[i].oldypos,tl_win[i].oldxsize,tl_win[i].oldysize);
  else
   fprintf(f," -1\n");
 fclose(f);
 printmsg(TXT_WINPOSSAVED,fname);
 }
 
void dec_loadwinpos(int ec)
 { 
 FILE *f;
 char *fname;
 int i;
 for(i=0;i<tlw_num;i++)
  if(tl_win[i].win!=NULL) break;
 if(i!=tlw_num) { printmsg(TXT_CLOSETXTWIN); return; }
 fname=getfilename(&init.txtlistpath,NULL,"WIN",TXT_LOADWINPOS,0);
 if(fname==NULL) return;
 if((f=fopen(fname,"r"))==NULL)
  { printmsg(TXT_CANTREADWINPOS,fname); return; }
 w_readcfg(f);
 for(i=0;i<tlw_num;i++)
  {
  if(fscanf(f," %d",&tw_savedata[i].zoom)!=1)
   { waitmsg(TXT_CANTREADWINPOS); return; }
  if(tw_savedata[i].zoom>=0)
   {
   fscanf(f,"%d%d%d%d",&tw_savedata[i].xpos,&tw_savedata[i].ypos,
    &tw_savedata[i].xsize,&tw_savedata[i].ysize);
   tl_win[i].oldxsize=-1;
   }
  }
 fclose(f);
 printmsg(TXT_WINPOSREAD,fname);
 }
 
void dec_reinitgrfx(int ec)
 { if(!l) { printmsg(TXT_NOLEVEL); return; }
   if(!view.render) return;
   init_rendercube();
   plotlevel(); }

void dec_render(int ec)
 {
 int j,k;
 if(!l) { printmsg(TXT_NOLEVEL); return; }
 view.render=ec-ec_render_0;
 if(view.render>1)
  { l->whichdisplay=view.whichdisplay=0; }
 else view.drawwhat|=DW_CUBES;
 if(view.render==3)
  for(j=0;j<3;j++)
   {
   view.e0.x[j]=0.0;
   for(k=0;k<8;k++) 
    view.e0.x[j]+=view.pcurrcube->d.c->p[k]->d.p->x[j]/8.0;
   }
 init_rendercube(); drawopt(in_internal); w_refreshwin(l->w); 
 }
   
void dec_changeview(int ec)
 { if(!l) { printmsg(TXT_NOLEVEL); return; }
   view.whichdisplay^=1;
   l->whichdisplay=view.whichdisplay;
   init_rendercube(); w_refreshwin(l->w); drawopt(in_internal); }
   
void dec_wireframe(int ec)
 { view.drawwhat^=DW_CUBES; plotlevel(); }

void dec_dummy(int ec) { waitmsg("Functioncode %x not used"); }

void (*do_event[ec_num_of_codes])(int ec)=
 { dec_quit,dec_loadlevel,dec_savelevel,dec_makemacro,dec_savemacro,
   dec_loadmacro,dec_insert,dec_insert,dec_insert,dec_insert,dec_delete,
   dec_delete,dec_deletespecial,dec_enlargeshrink,dec_enlargeshrink,
   dec_enterdata,dec_setexit,dec_calctxts,dec_aligntxts,dec_setlsfile,
   dec_tagspecial,dec_usecubetag,dec_usesidetag,dec_drawwhat,
   dec_newlevel,dec_makeroom,dec_splitcube,dec_makecorridor,dec_beam,
   dec_gowall,dec_sidecube,dec_aligntoaxis,dec_aligntoaxis,dec_aligntoaxis,
   dec_aligntoaxis,dec_aligntoaxis,dec_aligntoaxis,dec_playlevel,
   dec_credits,dec_help,dec_statistics,
   dec_move0,dec_move1,dec_move2,dec_move3,dec_move4,dec_move5,
   dec_turn0,dec_turn1,dec_turn2,dec_turn3,dec_turn4,dec_turn5,
   dec_cubemode,dec_sidemode,dec_pntmode,dec_thingmode,dec_wallmode,
   dec_movemode,dec_fastquit,dec_savelevel,dec_gridonoff,
   dec_prevcube,dec_nextcube,dec_prevside,dec_nextside,
   dec_prevpnt,dec_nextpnt,dec_prevthing,dec_nextthing,
   dec_prevwall,dec_nextwall,dec_tagall,dec_tag,dec_movefactor,
   dec_movefactor,dec_rotangle,dec_rotangle,dec_visibility,dec_visibility,
   dec_playlevel,dec_changefullname,dec_prevlevel,dec_nextlevel,
   dec_wireframe,dec_mineillum,dec_setcornerlight,dec_setinnercubelight,
   dec_movethingtocube,dec_missionmanager,
   dec_savepos,dec_savepos,dec_savepos,dec_savepos,dec_savepos,dec_savepos,
   dec_savepos,dec_savepos,dec_savepos,dec_savepos,dec_savepos,dec_savepos,
   dec_gotopos,dec_gotopos,dec_gotopos,dec_gotopos,dec_gotopos,dec_gotopos,
   dec_gotopos,dec_gotopos,dec_gotopos,dec_gotopos,dec_gotopos,dec_gotopos,
   dec_savewinpos,dec_loadwinpos,dec_reinitgrfx,dec_changeview,
   dec_render,dec_render,dec_render,dec_render,dec_tagflatsides,
   dec_usepnttag,dec_nextedge,dec_prevedge,dec_edgemode,dec_makestdside,
   dec_setcornerlight,dec_resetsideedge,dec_loadmacro,dec_savelevel,
   dec_makeedgecoplanar };

