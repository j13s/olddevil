/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */
enum evcodes { 
 ec_quit,ec_loadlevel,ec_savelevel,ec_makemacro,
 ec_savemacro,ec_loadmacro,ec_insert,ec_insertfast,
 ec_insspecial,ec_insfastspecial,ec_delete,ec_deletefast,
 ec_deletespecial,ec_enlarge,ec_shrink,ec_enterdata,
 ec_setexit,ec_calctxts,ec_alignbitmaps,ec_setlsfile,
 ec_tagspecial,ec_usecubetag,ec_usesidetag,ec_drawalllines,
 ec_newlevel,ec_makeroom,ec_splitcube,ec_makecorridor,
 ec_beam,ec_gowall,ec_sidecube,ec_posxaxis,
 ec_posyaxis,ec_poszaxis,ec_negxaxis,ec_negyaxis,
 ec_negzaxis,ec_playlevel,ec_credits,ec_help,
 ec_levelinfo,
 ec_move0,ec_move1,ec_move2,ec_move3,ec_move4,ec_move5,
 ec_turn0,ec_turn1,ec_turn2,ec_turn3,ec_turn4,ec_turn5,
 ec_cubemode,ec_sidemode,ec_pntmode,ec_thingmode,ec_wallmode,
 ec_movemode,ec_fastquit,ec_fastsave,ec_gridonoff,
 ec_prevcube,ec_nextcube,ec_prevside,ec_nextside,
 ec_prevpnt,ec_nextpnt,ec_prevthing,ec_nextthing,
 ec_prevwall,ec_nextwall,ec_tagall,ec_tag,
 ec_incrmovefactor,ec_decrmovefactor,ec_incrrotangle,ec_decrrotangle,
 ec_incrvisibility,ec_decrvisibility,ec_savetodescdir,ec_changefullname,
 ec_prevlevel,ec_nextlevel,ec_wireframeonoff,ec_mineillum,ec_setcornerlight,
 ec_setinnercubelight,ec_movethingtocube,ec_missionmanager,
 ec_savepos_0,ec_savepos_1,ec_savepos_2,ec_savepos_3,ec_savepos_4,
 ec_savepos_5,ec_savepos_6,ec_savepos_7,ec_savepos_8,ec_savepos_9,
 ec_overview_on,ec_cor_overview_on,
 ec_gotopos_0,ec_gotopos_1,ec_gotopos_2,ec_gotopos_3,ec_gotopos_4,
 ec_gotopos_5,ec_gotopos_6,ec_gotopos_7,ec_gotopos_8,ec_gotopos_9,
 ec_overview_off,ec_cor_overview_off,ec_savewinpos,ec_loadwinpos,
 ec_reinitgrfx,ec_changeview,ec_render_0,ec_render_1,ec_render_2,
 ec_render_3,ec_tagflatsides,ec_usepnttag,ec_nextedge,
 ec_prevedge,ec_edgemode,ec_makestdside,ec_mineillumsmooth,
 ec_resetsideedge,ec_readdbbfile,ec_savewithfulllightinfo,
 ec_makeedgecoplanar,ec_num_of_codes };
extern void (*do_event[ec_num_of_codes])(int ec);

