/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
    
void drawopt(enum infos what);
void drawopts(void);
void makeoptwindow(enum infos what);
void redraw_fitbitmap(struct w_window *w,void *d);
void shrinkopt_win(enum tagtypes what);
void changedata(struct infoitem *i,int withtagged,void *data);
void drawoptbuttons(struct infoitem *i);
#define MWT(b,lr) (lr+(((b->event.kbstat&ws_ks_ctrl)!=0) ? 2 : 0))

#define NUM_OPTBUTTONS 10
extern struct w_window *optionwins[in_number];
extern struct w_button *b_optwins[in_internal][NUM_OPTBUTTONS];

extern int txtpos[4][2];
