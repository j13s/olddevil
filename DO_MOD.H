/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

void dec_makeedgecoplanar(int ec);
void dec_enlargeshrink(int ec);
void dec_setexit(int ec);
void dec_calctxts(int ec);
void dec_aligntxts(int ec);
void dec_lightshading(int ec);
void dec_mineillum(int ec);
void dec_enterdata(int ec);
void dec_makecorridor(int ec);
int recalc_corridor(struct corridor *c);
int movecorr(struct point *add,union move_params *params);
int turncorr(struct point *turn,float fx,float fy,int with_left,
 union move_params *params);
void close_corr_win(struct corridor *c);
void delete_corridor(struct corridor *c);

extern int corr_win1_xpos,corr_win1_ypos,corr_win2_xpos,corr_win2_ypos; 


