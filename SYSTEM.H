/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

/* Some constants you must modify: */
/* quite clear: */
#define SYS_COMPILER_NAME "GNU-C 2.57 with libgrx 1.03beta"
/* is switching between modes allowed: */
#define SYS_VGA_SUPPORTED 1

/* Don't modify the following. It should stay the same for all different
   DEVIL-versions. If you think you must modify it, please mail me. */
/* the event definitions */
struct sys_event {
 enum flagtypes { se_f_lbutton=1,se_f_rbutton=2,se_f_keypress=3 } flags;
 int  x,y;	/* coordinates of mouse */
 enum buttontypes { se_bt_none,se_bt_left,se_bt_right } buttons;
  /* button state */
 int  key;	/* key code from keyboard */
 enum kbstattypes { se_ks_shift=1,se_ks_alt=2,se_ks_ctrl=4,se_ks_numlock=8,
  se_ks_capslock=16,se_ks_scrollock=32 } kbstat;  
 };

/* the function prototypes */
/* changing modes, initialize */
int sys_initgrfx(int supervga);
void sys_clearall(void);
int sys_changeto256(void);
int sys_changeto16(void);
int sys_inittxtspace(void);
void sys_initmouse(void);
char *sys_initmenu(int x1,int y1,int x2,int y2);
void sys_initend(void);
void sys_setcolor(int i,int r,int g,int b);
void sys_getcolor(int i,int *r,int *g,int *b);
/* drawing */
void sys_drawbutton(int x1,int y1,int x2,int y2,char *txt);
void sys_text(int x,int y,char *txt,int fg,int bg);
void sys_menutext(int x,int y,char *txt,int u);
void sys_filledbox(int x1,int y1,int x2,int y2,int c,int xor);
void sys_line(int x1,int y1,int x2,int y2,int c,int xor);
void sys_initclipping(int x1,int y1,int x2,int y2);
void sys_clipline(int x1,int y1,int x2,int y2,int c,int xor);
void sys_killclipping(void);
void sys_dumpmenu(void);
void sys_dumpmenuchar(char x,int column,int row);
void sys_drawtexture(int plot,int x,int y,int txt_x,int txt_y,char *txt);
/* others */
int sys_getnumlock(void);
int sys_numlockonoff(void);
void sys_disablectrlc(void);
int sys_getevent(struct sys_event *se,int wait);
char *sys_searchfirstfile(char *fname);
char *sys_searchnextfile(void);




