/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

enum tlw_types { tlw_t1,tlw_t2,tlw_anim,tlw_thing,tlw_num };
struct txt_list_win
 {
 enum tlw_types type;
 struct w_window *win;
 struct ham_txt **t;
 struct w_button *b_up,*b_down,*b_mode,*b_display,*b_zoom,*b_m_clear,
  *b_m_load,*b_m_save,*b_m_level,*b_m_down,**b_texture;
 char **txt_buffer;
 struct ws_bitmap **txt_bitmap;
 unsigned char *marked_txts;
 int oldxpos,oldypos,oldxsize,oldysize,offset,maxnum,maxoffset,txt_xoffset,
  txt_yoffset,txt_xsize,txt_ysize,dontcalltwice,oldxnumtxt,oldynumtxt,oldmode,
  olddisplay,curtxtno,oldtxttyp;
 struct infoitem *i;
 struct w_b_press up,down,texture,m_clear,m_load,m_save,m_level;
 struct w_b_choose display,mode,zoom;
 int num_usertxtlist,num_leveltxtlist;
 struct ham_txt **usertxtlist,**leveltxtlist;
 };
struct txtwin_savedata { int zoom,xpos,ypos,xsize,ysize; };
extern struct txtwin_savedata tw_savedata[tlw_num];
struct fitbitmap_savedata
 { int xpos,ypos,xsize,ysize; };
extern struct fitbitmap_savedata fb_savedata;
extern struct txt_list_win tl_win[tlw_num];    
struct fl_savedata { int data_ok,old_xpos,old_ypos; } ;
extern struct fl_savedata fl_savedata;

void redraw_fitbitmap(struct w_window *w,void *d);
void refresh_txtlist(struct infoitem *i);
void b_fitbitmap(struct w_button *b,int withtagged);
void b_selecttexture(struct w_button *b,int withtagged);
void b_chartexture(struct w_button *b,int withtagged);
void b_stringtexture(struct w_button *b,int withtagged);
void b_selectdooranim(struct w_button *b,int withtagged);
void b_l_playdooranim(struct w_button *b);
void b_l_enddooranim(struct w_button *b);
void drawtxtlines(struct w_button *b,struct wall *w);
void drawdooranim(struct w_button *b,struct ham_txt *t,int anim,int t1,int t2,
 int t2_d);
void fb_refreshall(void);
void fb_move_texture(int axis,int dir);
void fb_turn_texture(int x,int y,int z,int dir);
int fb_isactive(void);
void fb_tofront(void);
