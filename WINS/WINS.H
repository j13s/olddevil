/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

extern void my_exit(void);
    
#include "w_list.h"
   
enum win_buttons { wb_none=0,wb_drag=1,wb_size=2,wb_close=4,wb_shrink=8 };
enum win_refresh { wr_normal,wr_routine };
/* routine=normal refresh, use in shrink a special refresh rountine instead of
    saving the bitmap (used for plotwindow)
   normal=normal refresh, in shrink saving the contents of the window
    in a bitmap */
enum but_types { w_b_press,w_b_drag,w_b_switch,w_b_choose,w_b_string,
 w_b_tag,w_b_list,w_b_number };
enum keycode_flags { w_kf_exit=1 };
 
struct w_keycode 
 { int kbstat,key,event; 
   enum keycode_flags flags; };
 
/* the event definitions */
struct w_event 
 {
 int  x,y;	/* coords of mouse relative to upper,left edge of window
                   freespace */
 struct ws_event *ws;		  
 struct w_window *w; /* window in which the click is. */
 };

struct w_button
 { 
 int no; /* is filled by add_button */
 enum but_types type;
 int xpos,ypos,xsize,ysize; /* size is not used by some buttons */
 struct w_window *w; 
 const char *helptxt;
 struct list inscription,a_inscription;
 void *data; /* this pointer you can use to store a link to your data */
 struct ws_event event; /* the event that activated that button */
 union {
  struct w_b_press *p;
  struct w_b_drag *dr;
  struct w_b_switch *s;
  struct w_b_choose *ls;
  struct w_b_string *str;
  struct w_b_tag *tag;
  struct w_b_strlist *sls;
  struct w_bi_desc *de;
  void *d;
  } d;
 };

struct w_description
 {
 int x,y; /* relative to upper,left edge */
 enum desc_types { w_bdt_txt,w_bdt_image } type;
 int image_with_bg;
 union {
  struct ws_bitmap *bm;
  char *txt;
  } contents;
 };
 
struct w_b_press /* this is a normal click button */
 {
 /* l/r_pressed_routine are called when the button is pressed down after
    delay/100 seconds and each time if the buttons is still pressed down
    after at least repeat/100 seconds. If repeat==0 the routines are called
    immediately after the previous call returned, if repeat<0 the routines
    are called once. */
 int delay,repeat; 
 void (*l_pressed_routine)(struct w_button *b);
 void (*r_pressed_routine)(struct w_button *b);
 void (*l_routine)(struct w_button *b);
 void (*r_routine)(struct w_button *b);
 };

struct w_b_drag /* this is a drag button like the size button */
 {
 /* dx,dy are the coordinates relative to the upper,left edge of the
    button */
 void (*l_pressroutine)(struct w_button *b,int dx,int dy);
 void (*r_pressroutine)(struct w_button *b,int dx,int dy);
 void (*l_dragroutine)(struct w_button *b,int dx,int dy);
 void (*r_dragroutine)(struct w_button *b,int dx,int dy);
 void (*l_relroutine)(struct w_button *b,int dx,int dy);
 void (*r_relroutine)(struct w_button *b,int dx,int dy);
 };

struct w_b_switch /* this is a switch -- when you click it, it stay there */
 {
 int on; /* 1 if clicked, 0 if not */
 void (*l_routine)(struct w_button *b);
 void (*r_routine)(struct w_button *b);
 };
 
struct w_b_choose /* this is a button where you can choose you one item */
 {
 int d_xsize; /* size of button for inscription */
 int num_options; /* number of options */
 const char **options; /* Array of options */
 int selected; /* number of the option selected */
 void (*select_lroutine)(struct w_button *b);
 void (*select_rroutine)(struct w_button *b);
 };

struct w_b_tag /* you can choose several items from a list */
 {
 int num_options,max_options;
 const char **options;
 int *tagged; /* 0 if option is not selected, 1 if selected */
 void (*tag_lroutine)(struct w_button *b); /* called when list is closed */
 void (*tag_rroutine)(struct w_button *b);
 };

struct w_b_string /* button to enter a string */
 {
 int d_xsize; /* size of button for inscription */
 int max_length;
 int offset,length; /* offset and length are needed for scrolling, 
  they're set by the wins-routines. */
 char *str;
 int (*allowed_char)(int x);
 void (*l_char_entered)(struct w_button *b);
 void (*r_char_entered)(struct w_button *b);
 void (*l_string_entered)(struct w_button *b);
 void (*r_string_entered)(struct w_button *b);
 };

struct w_b_strlist /* button like the button for a filelist */
 {
 int no_strings,start_no,no_rows,max_selected;
 char **strings,*sel_arr;
 void (*l_string_selected)(struct w_button *b);
 void (*r_string_selected)(struct w_button *b);
 };
 
struct w_window
 {
 int no; /* is filled by open_window */
 int xpos,ypos,xsize,ysize; /* normal size */
 int maxxsize,maxysize; /* if this is <=0 there is no limit */
 int shrunk; /* only titlebar = 1, 0 otherwise */
 const char *title;
 char hotkey;
 const char *help;
 enum win_buttons buttons;
 enum win_refresh refresh;
 void *data; /* this is the parameter used in the routines. You can
                use it to store a pointer to the contents of the window */
 void (*refresh_routine)(struct w_window *,void *);
 void (*close_routine)(struct w_window *,void *);
 void (*click_routine)(struct w_window *,void *,struct w_event *);
 };

