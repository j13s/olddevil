/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
    
enum sidecodes { sc_cubetype,sc_setavgcubelight,sc_sett2direction,
 sc_setavgsidelight,sc_setpntcoord,sc_setuvl,sc_thingtype,
 sc_robot,sc_hostagesize,sc_itemgrfx,sc_walltype,sc_dooranim,sc_switch,
 sc_illum_brightness,sc_dropsomething,sc_changedrawwhat,sc_thingpos,
 sc_perspective,sc_flythrough,sc_fl_delay,sc_plotlevel,sc_number };
int do_sideeffect(enum sidecodes sc,struct infoitem *i,void *d,
 struct node *n,int wallno,int pntno,int tagged);
