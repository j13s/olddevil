/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
enum evcodes { ec_credits,ec_changenumlock,ec_clickcube,ec_clickthing,
 ec_togdrawthings,ec_togdrawcubes,ec_togdrawlines,ec_menucubes,ec_menuint,
 ec_menudoors,ec_menuthings,ec_nextthing,ec_prevthing,ec_nextcube,
 ec_prevcube,ec_nextwall,ec_prevwall,ec_tag,
 ec_togdrawdata,ec_pturnleft,ec_pturnright,ec_pturnup,ec_pturndown,
 ec_incvisibility,ec_decvisibility,ec_inczoom,ec_deczoom,ec_incmovefactor,
 ec_decmovefactor,ec_moveforward,ec_movebackward,ec_moveleft,
 ec_moveright,ec_moveup,ec_movedown,ec_bankleft,ec_turnup,ec_bankright,
 ec_turnright,ec_turndown,ec_turnleft,ec_incrotangle,
 ec_decrotangle,ec_nextdoor,ec_prevdoor,ec_menuwalls,
 ec_menucorners,ec_loadlevel,ec_savelevel,ec_clickdoor,
 ec_togdrawdoors,ec_tagall,ec_beam,ec_help,ec_makemacro,ec_savemacro,
 ec_choosemacro,ec_fastquit,ec_pmovedown,ec_pmoveup,ec_pmoveright,
 ec_pmoveleft,ec_pmoveback,ec_pmoveforw,ec_nextpnt,ec_prevpnt,ec_fitbitmap,
 ec_insert,ec_chval_without_se,ec_pbankleft,ec_pbankright,
 ec_scrollup,ec_scrolldown,ec_changedata,ec_incpmovefactor,
 ec_decpmovefactor,ec_delete,ec_gridonoff,ec_decrgridsize,
 ec_incrgridsize,ec_changevalue,ec_mmoveup,ec_mmovedown,
 ec_connectcubes,ec_choosecube,ec_choosething,ec_choosedoor,
 ec_chval_without_tag,ec_chval_without_all,ec_fastinsert,ec_setexit,
 ec_deleteall,ec_gowall,ec_quit,ec_connectbitmaps,ec_togdrawgrid,
 ec_incprotangle,ec_decprotangle,ec_aligntox,ec_aligntoy,ec_aligntoz,
 ec_aligntonegx,ec_aligntonegy,ec_aligntonegz,ec_tagwholepnt,
 ec_insscaled,ec_insfastscaled,ec_sidecube,ec_grow,ec_shrink,ec_calctxts,
 ec_num_of_codes };
int do_event(enum evcodes evcode);

extern int mousex,mousey;

