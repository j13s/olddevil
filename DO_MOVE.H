/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
void b_move(int axis,int dir);
void b_turn(int x,int y,int z,int dir);
void moveobj_mouse(int withtagged,int wx,int wy,struct node *nc,int wall);
void moveyou_mouse(struct w_window *w,int wx,int wy);
void dec_move0(int ec);
void dec_move1(int ec);
void dec_move2(int ec);
void dec_move3(int ec);
void dec_move4(int ec);
void dec_move5(int ec);
void dec_turn0(int ec);
void dec_turn1(int ec);
void dec_turn2(int ec);
void dec_turn3(int ec);
void dec_turn4(int ec);
void dec_turn5(int ec);
void dec_aligntoaxis(int ec);
void dec_sidecube(int ec);
void dec_gowall(int ec);
void dec_beam(int ec);
void dec_prevcube(int ec);
void dec_nextcube(int ec);
void dec_prevside(int ec);
void dec_nextside(int ec);
void dec_prevpnt(int ec);
void dec_nextpnt(int ec);
void dec_prevthing(int ec);
void dec_nextthing(int ec);
void dec_prevwall(int ec);
void dec_nextwall(int ec);
void dec_prevedge(int ec);
void dec_nextedge(int ec);
void dec_resetsideedge(int ec);
void turnsinglepnt(struct point *axis,float cosang,float sinang,
 struct point *s);
void dec_savepos(int ec);
void dec_gotopos(int ec);
void init_rendercube(void);
void movecorr_mouse(struct w_window *w,int wx,int wy,struct track *t);
void move_texture_with_mouse(int sx,int sy,struct corner *cs,
 struct node *cube,int wall,int withtagged,float angle,
 void (*refresh_func)(void));
void move_texture(struct corner *cs,int dx,int dy);
void stretch_texture(struct corner *cs,int sx,int sy);
void rotate_texture(struct corner *cs,float angle);

