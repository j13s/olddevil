/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    structs.h - global variables, structures, defines and so on.
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
    
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#include <wins_all.h>
#ifdef GER
#include <german/text.h>
#define INIFILE "config/devilg.ini"
#else
#include <english/text.h>
#define INIFILE "config/devil.ini"
#endif

#include "descdefs.h"

/* Well, this is not so nice, but I need it. Perhaps you must set a flag
   also. GNU-C will align characters to longs and savetool needs packed
   structures (every type as long as in sizeof()) */
#if defined(GNU_C) && defined(GO32)
#define NONANSI_FLAG __attribute__ ((packed))
#else
#define NONANSI_FLAG 
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#define ILLUM_GRIDSIZE 2
#define VECTOR(c,a,b) { \
 (c)->x[0]=(float)(a)->x[1]*(b)->x[2]-(float)(a)->x[2]*(b)->x[1]; \
 (c)->x[1]=(float)(a)->x[2]*(b)->x[0]-(float)(a)->x[0]*(b)->x[2]; \
 (c)->x[2]=(float)(a)->x[0]*(b)->x[1]-(float)(a)->x[1]*(b)->x[0]; }
#define SCALAR(a,b) ((float)(a)->x[0]*(b)->x[0]+ \
 (float)(a)->x[1]*(b)->x[1]+(float)(a)->x[2]*(b)->x[2])
#define LENGTH(a) sqrt((float)SCALAR(a,a))
#define DW_GROUPPREVIEW 0x20
#define DW_XTAGGED 0x10
#define DW_DOORS 0x8
#define DW_THINGS 0x4
#define DW_ALLLINES 0x2
#define DW_CUBES 0x1
#define DW_DEFAULT 0x2f
/* colors */
#define BLACK 0
#define GRAYSCALE 16
#define THINGCOLORS (GRAYSCALE+1)
#define NUM_OF_THINGS 4
#define DOORCOLORS (THINGCOLORS+NUM_OF_THINGS)
#define NUM_OF_DOORS 5
#define HILIGHTCOLORS (DOORCOLORS+NUM_OF_DOORS)
#define MAXHILIGHT 8
#define USEDCOLORS (HILIGHTCOLORS+MAXHILIGHT+1)
#define WHITE (USEDCOLORS-1)

#define MAX_OBJSTR_LENGTH 15

enum tagtypes { tt_cube,tt_wall,tt_edge,tt_pnt,tt_thing,tt_door,tt_number };
enum movetypes { mt_you,mt_obj,mt_current,mt_texture,mt_number };
enum infos { in_cube,in_wall,in_edge,in_pnt,in_thing,in_door,in_internal,
 in_number };
enum datastructs { ds_cube,ds_wall,ds_corner,ds_thing,ds_door,ds_internal,
 ds_leveldata,ds_sdoor,ds_producer,ds_point,ds_turnoff,ds_changedlight,
 ds_flickeringlight,ds_number };
enum infotype { it_selbutton,it_markbutton,it_light,it_size,it_coord,
 it_degree,it_float,it_int,it_cubelight,it_sidelight,it_texture,
 it_dooranim,it_thingtexture,it_thingcoord,it_fl_mask,it_fl_delay,
 it_uvcoord,it_deltalight };

struct point {float x[3];};
struct pixel {int x,y; float d;};
struct line {struct node *s,*e;int color;};
struct move_par_pnts
 {
 struct list *pnt_list,*side_list,*cube_list;
 struct point *ref_pnt;
 struct node *cube;
 int wall;
 };
struct move_par_things
 { int withtagged;
   struct node *nc; };
struct move_par_corr
 { struct track *t;
   struct corridor *c; };
union move_params
 { struct move_par_pnts p;
   struct move_par_things t;
   struct move_par_corr c; };
struct corridorelement 
 {
 struct point center,**pnts,*rel_pnts;
 struct node **cubes;
 };
struct track
 {
 struct point x,coords[3];
 float length,l_c2,old_twist; /* l_c2=length of tangential vector */
  /* old_twist == twist angle of last calculation */
 int fixed; /* <=0 is moved from the program, >0 can be moved by the user */
 struct track *modify_too; /* for circle, movecorr */
 };
struct corridor
 {
 struct list cubes,points;
 struct list elements;
 int num_pnts,num_cubes; /* in one corridor element */
 int *connected,*front_wall;
  /* array of size num_cubes*6. In each element j is cube_no*6+wall of
     the neighbour cube to cube j/6 wall j%6 saved (-1 if no
     neighbour). */
 float depth;
 struct list tracking; /* the points through which the corridor runs */
 struct leveldata *ld;
 struct point e[3]; /* default coordsystem on the startside of the corridor */
 struct node *startcube; /* the cube where the corridor starts */
 int startwall;
 /* the window stuff: */
 struct w_window *win;
 struct w_button *b_ok,*b_cancel,*b_ins,*b_del,*b_setend,*b_depth,*b_stdform;
 };
struct listpoint
 {
 struct point p;
 struct list c; /* list of cubenodes. no is no of point in cube */
 struct node *tagged;
 };
struct thing
 {
 /* my stuff */
 struct point p[11] NONANSI_FLAG; /* 0 is center, 1..10 are the drawn lines */
 struct node *tagged NONANSI_FLAG; /* corresponding node in taglist */
 struct node *nc NONANSI_FLAG; /* the cube where the thing is in*/
 int color NONANSI_FLAG;
 /* descent stuff */
 unsigned char type1 NONANSI_FLAG;
 unsigned char type2 NONANSI_FLAG;
 unsigned char control NONANSI_FLAG;
 unsigned char movement NONANSI_FLAG;
 unsigned char render NONANSI_FLAG;
 unsigned char flags NONANSI_FLAG;
 unsigned short cube NONANSI_FLAG;
 long pos[3] NONANSI_FLAG;
 /* one word to the orientation:
  the first 3 components define the normalvector on the right side of
   the object.
  the second 3 components define the normalvector on the top of the
  object.
  the third 3 components define the normalvector on the front of the
   object.
  that means the whole thing has too many parameters. the third vector
  is already defined from the first two one. the length of a 
  normalvector is 0x10000 */
 long orientation[9] NONANSI_FLAG;
 long size NONANSI_FLAG;
 long shield NONANSI_FLAG;
 long lastpos[3] NONANSI_FLAG;
 unsigned char contain_type1 NONANSI_FLAG;
 unsigned char contain_type2 NONANSI_FLAG;
 unsigned char contain_count NONANSI_FLAG;
 }; /* Descent data length: 79 */
struct movement_physics
 {
 long velocity[3] NONANSI_FLAG;
 long thrust[3] NONANSI_FLAG;
 long mass NONANSI_FLAG;
 long drag NONANSI_FLAG;
 long brakes NONANSI_FLAG;
 long rotvel[3] NONANSI_FLAG;
 long rotthrust[3] NONANSI_FLAG;
 short turnroll NONANSI_FLAG;
 short flags NONANSI_FLAG;
 }; /* length: 64 */
struct movement_spinning
 { long spin_rate[3] NONANSI_FLAG; };
#define MAX_AIFLAGS 11
struct control_mine
 { 
 short parent_type NONANSI_FLAG;
 short parent_num NONANSI_FLAG;
 long parent_signature NONANSI_FLAG; 
 };
struct D1_control_ai
 {
 unsigned char behavior  NONANSI_FLAG;
 unsigned char flags[MAX_AIFLAGS] NONANSI_FLAG;
 unsigned short hide_cube NONANSI_FLAG;
 unsigned short hide_index  NONANSI_FLAG; 
 unsigned short path_length NONANSI_FLAG;
 unsigned short cur_path_index NONANSI_FLAG;
 unsigned short start_cube NONANSI_FLAG;
 unsigned short end_cube  NONANSI_FLAG;
 }; /* length: 24 */
struct D2_control_ai
 {
 unsigned char behavior  NONANSI_FLAG;
 unsigned char flags[MAX_AIFLAGS] NONANSI_FLAG;
 unsigned short hide_cube NONANSI_FLAG;
 unsigned short hide_index  NONANSI_FLAG; 
 unsigned short path_length NONANSI_FLAG;
 unsigned char cur_path_index NONANSI_FLAG;
 unsigned char dying_sound_playing NONANSI_FLAG;
 }; /* length: 20 */
struct control_light
 { long intensity NONANSI_FLAG; };
struct control_powerup
 { long count NONANSI_FLAG; };
#define MAX_SUBMODELS 10
struct render_polyobj
 {
 long model_num NONANSI_FLAG;
 short anim_angles[MAX_SUBMODELS][3] NONANSI_FLAG;
 unsigned long flags NONANSI_FLAG;
 long texture NONANSI_FLAG;
 };
struct render_powerup
 {
 long vclip NONANSI_FLAG; /* this is the number of the graphics */
 long frametime NONANSI_FLAG;
 unsigned char framenum NONANSI_FLAG;
 };
struct item
 {
 struct thing t NONANSI_FLAG;
 struct control_powerup c NONANSI_FLAG;
 struct render_powerup r NONANSI_FLAG;
 };
struct D1_robot
 {
 struct thing t NONANSI_FLAG;
 struct movement_physics m NONANSI_FLAG;
 struct D1_control_ai c NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct D2_robot
 {
 struct thing t NONANSI_FLAG;
 struct movement_physics m NONANSI_FLAG;
 struct D2_control_ai c NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct start
 {
 struct thing t NONANSI_FLAG;
 struct movement_physics m NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct mine
 {
 struct thing t NONANSI_FLAG;
 struct movement_physics m NONANSI_FLAG;
 struct control_mine c NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct reactor
 {
 struct thing t NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct sdoor
 {
 unsigned char type NONANSI_FLAG;
 unsigned char flags NONANSI_FLAG;
 unsigned short num NONANSI_FLAG;
 long value NONANSI_FLAG; /* 0x00050000 */
 long link_num NONANSI_FLAG; /* -1 */
 unsigned short int cubes[10] NONANSI_FLAG;
 unsigned short int walls[10] NONANSI_FLAG;
 /* my stuff */
 struct node *d NONANSI_FLAG;
 struct node *target[10] NONANSI_FLAG;
 };
struct edoor
 {
 unsigned short int num NONANSI_FLAG;
 unsigned short int cubes[10] NONANSI_FLAG;
 unsigned short int walls[10] NONANSI_FLAG;
 };
struct door
 {
 unsigned long cubenum NONANSI_FLAG;
 unsigned long wallnum NONANSI_FLAG;
 long hitpoints NONANSI_FLAG;
 long linked_wall NONANSI_FLAG;
 unsigned char type1,type2 NONANSI_FLAG;
 unsigned char state NONANSI_FLAG;  /* should be closed=0 */
 unsigned char sdoor NONANSI_FLAG;
 unsigned char animtxt NONANSI_FLAG;
 unsigned char key NONANSI_FLAG;
 unsigned char stuff NONANSI_FLAG; /* 0 */
 unsigned char cloaking NONANSI_FLAG;
 /* my stuff */
 unsigned char edoor NONANSI_FLAG; /* open at end? */
 unsigned char switchtype NONANSI_FLAG; /* needed that you can change the
  switchtype from 'No switch' to anything else(the button must be activated)*/
 struct node *d NONANSI_FLAG; /* door from other side */
 struct node *sd NONANSI_FLAG; /* the sdoor corresponding to this door */
 struct list sdoors NONANSI_FLAG; /* doors opening this door */
 struct node *c NONANSI_FLAG;
 struct wall *w NONANSI_FLAG;
 struct point p NONANSI_FLAG;
 struct node *tagged NONANSI_FLAG; /* corresponding node in taglist */
 short old_txt1 NONANSI_FLAG; /* old texture 1 of the wall */
 unsigned char *txt_data NONANSI_FLAG;
 };
#define MAX_LIGHT 65207 /* 199% */
struct corner
 {
 short int x[2] NONANSI_FLAG;
 unsigned short int light NONANSI_FLAG;
 };
struct movewall
 { struct node *cube;
   unsigned char wall,move_pnts; };
struct wall
 {
 short int texture1,texture2 NONANSI_FLAG;
 /* corners:
  in the coord-system of the corners is the bitmap drawn.
  (0,0) is the right lower corner of the bitmap, the x-axis
  goes along the lower edge of the bitmap in negative direction
  (means from lower right edge to lower left edge),
  the y-axis from lower right to upper right.
  descent fits the texture in the given polygon, so the polygon should
  have the same form as the real wall. the normal
  coordsystem is scaled 640 times larger than the txture coordsystem.
  corners ->  points:
                        0 1 2 3 corner
  wall 0 (left side):   7 6 2 3
  wall 1 (ceiling):     0 4 7 3 
  wall 2 (right side):  0 1 5 4
  wall 3 (floor):       2 6 5 1
  wall 4 (front):       4 5 6 7
  wall 5 (back):        3 2 1 0
  */
 struct corner corners[4] NONANSI_FLAG;
 /* my stuff */
 int txt2_direction NONANSI_FLAG;
 struct node *p[4] NONANSI_FLAG;
 struct node *tagged[4] NONANSI_FLAG;
 int no NONANSI_FLAG;
 int locked NONANSI_FLAG;
 struct node *ls NONANSI_FLAG; /* lightsource */
 };
struct producer
 {
 /* producer produces all roboter with the number of the setted bit */
 unsigned long prodwhat[2] NONANSI_FLAG;
 unsigned long hitpoints NONANSI_FLAG; /* 0x01f40000=32768000 */
 unsigned long interval NONANSI_FLAG; /* 0x00050000=327680 */
 unsigned short cubenum NONANSI_FLAG;
 unsigned short stuffnum NONANSI_FLAG;
  /* stuffnum is not used in Descent */
 /* my stuff */
 struct node *c NONANSI_FLAG;
 };
struct cube
 {
 unsigned char type NONANSI_FLAG;
 char prodnum NONANSI_FLAG;
 /* value: count every cube with type==4 and write this number in value. */
 unsigned char value NONANSI_FLAG;
 unsigned char flags NONANSI_FLAG; /* is set by Descent ?? */
 unsigned short int light NONANSI_FLAG;
 unsigned short int high_light NONANSI_FLAG;
 unsigned short int pts[8] NONANSI_FLAG;
 unsigned short int nextcubes[6] NONANSI_FLAG;
 unsigned char doors[6] NONANSI_FLAG;
 /* my stuff */
 struct list sdoors NONANSI_FLAG;
 struct wall *walls[6] NONANSI_FLAG;
 struct node *p[8],*nc[6],*d[6] NONANSI_FLAG;
 struct node *cp NONANSI_FLAG; /* producer */
 struct node *tagged NONANSI_FLAG; /* corresponding node in taglist */
 struct node *tagged_walls[6] NONANSI_FLAG;
 struct polygon *polygons[12] NONANSI_FLAG;
 unsigned char recalc_polygons[6] NONANSI_FLAG;
 struct list things NONANSI_FLAG;
 struct list fl_lights NONANSI_FLAG; /* list of f.l. lights that affect this
					cube */
 };
struct flickering_light
 {
 short int cube NONANSI_FLAG,wall NONANSI_FLAG;
 unsigned long mask NONANSI_FLAG,timer NONANSI_FLAG,delay NONANSI_FLAG;
 /* for rendering: */
 struct node *ls NONANSI_FLAG; /* corresponding light source */
 int state NONANSI_FLAG; /* is this light source on or off? */
 unsigned char calculated NONANSI_FLAG;
 }; 
struct turnoff
 {
 unsigned short cube NONANSI_FLAG;
 unsigned char side NONANSI_FLAG;
 unsigned char num_changed NONANSI_FLAG;
 unsigned short offset NONANSI_FLAG;
 };
struct changedlight
 {
 unsigned short cube NONANSI_FLAG;
 unsigned char side NONANSI_FLAG;
 unsigned char stuff NONANSI_FLAG; /* always 0?? */
 unsigned char sub[4] NONANSI_FLAG; /* between 0-32. 32 is dark,0 is bright */
 };
struct ls_effect
 {
 struct node *cube; /* destination cube */
 unsigned short add_light[24];
 unsigned char smoothed[24];
 }; 
struct lightsource
 {
 struct node *cube;
 char w; /* source cube and wall */
 struct list effects; /* list of ls_effects */
 struct flickering_light *fl; 
 };
#define NUM_SAVED_POS 12
struct saved_position
 { 
 struct point e0,e[3];
 float max_vis,distcenter;
 };
struct leveldata
 {
 long reactor_time,reactor_strength;
 struct list pts,cubes,things,doors,sdoors,producers,lightsources;
 struct edoor *edoors;
 char *fullname;
 char *filename;
 char *pigname;
 struct w_window *w;
 struct node *secretcube,*secretstart;
 long secret_orient[9];
 struct node *exitcube;
 int exitwall;
 struct node *pcurrcube,*pcurrthing,*pcurrdoor,*pcurrpnt;
 struct wall *pcurrwall;
 int curredge,currwall; /* 0..3 curredge of currwall / 0..5 cwall of ccube */
 struct node *rendercube;  /* the cube where the rendering starts */
 int inside; /* are we inside or outside of the level (left window) */
 struct list tagged[tt_number]; /* cubes,walls,pts,edges,things */
 struct list lines;
 int whichdisplay; /* ==0 one display, ==1 two for each level */
 struct point e0,e[3];
 int levelsaved,levelillum;
 struct node *n;
 struct corridor *cur_corr;
 struct saved_position saved_pos[NUM_SAVED_POS];
 int x_size[2],y_size[2];  /* window size for single&double mode */
 };
struct objtype
 {
 int no;
 char *str;
 };
struct objdata
 { 
 int size;
 struct objtype **data;
 };
struct infoitem
 {
 enum infotype type; /* type of data */
 char *txt; /* description */
 enum datastructs infonr; /* to get the corresponding structure */
 enum infos tagnr; /* to find the corresponding tag list, 
   if tagnr>=in_internal this field is not done with tagged objects */
 int multifuncnr; /* for selection button=numchildren, for tag button
  =maximum number of tagged options, for texture group:
   0 texture itself
   1-5 texture 1, top,up,down,bottom,name
   6-10 texture 2, top,up,down,bottom,name
  otherwise not relevant */
 int offset,length,numchildren;  /* offset,length: pos. in structure; */
 struct infoitem **children; 
 int *numinchildren; /* how many entries in children */
  /* children=infoitems depending on this infoitem */
 unsigned long *itemchildren;
  /* itemchildren=array of the numbers belonging to children */
 struct objdata *od; /* =NULL if normal field, otherwise the list of objs
  for this field. */
 int sidefuncnr; /* call which function */
 struct w_button *b; /* the button for this infoitem */
 };
/* num_security is the number of extra lighttables at the end and the 
 beginning of the table, to handle the rounding errors which occur during
 the plotting of a texture */
struct viewdata
 {
 float dist NONANSI_FLAG,distscala NONANSI_FLAG;
  /* this is the distance viewpoint-screen */
 float maxvisibility NONANSI_FLAG,visscala NONANSI_FLAG;
  /* maxdistance which will be drawn */
 float movefactor NONANSI_FLAG; /* how fast we will move */
/* 20 */ 
 float pmovefactor NONANSI_FLAG,movescala NONANSI_FLAG; 
  /* how fast the points will move */
 float rotangle NONANSI_FLAG; /* how fast will we turn */
 float protangle NONANSI_FLAG,rotscala NONANSI_FLAG;
  /* how fast will we turn */
/* 40 */  
 int bm_movefactor NONANSI_FLAG; /* how fast are the lines in fit bitmap
  not needed anymore! */
 float flatsideangle NONANSI_FLAG; /* for dec_tagflatsides */
 float bm_stretchfactor NONANSI_FLAG; 
 float gridlength NONANSI_FLAG,gridscala NONANSI_FLAG;
/* 60 */ 
 float maxconndist NONANSI_FLAG,maxuserconnect NONANSI_FLAG;
  /* for connectcubes,connect */
 int clickradius NONANSI_FLAG; /* min. radius of clicking circle */
 float tsize NONANSI_FLAG; /* size of things */
 float dsize NONANSI_FLAG; /* size of beat out of doors */
/* 80 */ 
 struct point e0 NONANSI_FLAG; /* Pos. of screen (in the middle) */
 float mapangle NONANSI_FLAG; /* angle for lines in descent-like map */
 int gridonoff NONANSI_FLAG;
/* 100 */ 
 int askone NONANSI_FLAG,asktagged NONANSI_FLAG;
 int displayed_group NONANSI_FLAG;
 /* This is the length for getting a quarter of intensity of light */
 float illum_quarterway NONANSI_FLAG;
 /* Brightness of Textures */
 float illum_brightness NONANSI_FLAG;
/* 120 */ 
 /* The basic value for every corner */
 unsigned short illum_minvalue NONANSI_FLAG;
/* 122 */ 
 /* Some options for warning */
 int warn_convex NONANSI_FLAG,warn_doublekeys NONANSI_FLAG,
  warn_tworeactors NONANSI_FLAG,warn_thingoutofbounds NONANSI_FLAG,
  warn_gridrot NONANSI_FLAG;
/* 142 */  
 int drawwhat NONANSI_FLAG;
 float distcenter NONANSI_FLAG; /* distance from two views to center */
 int whichdisplay NONANSI_FLAG; /* ==0 one display, ==1 two for each level.*/
 int doubleclick NONANSI_FLAG;
 int render NONANSI_FLAG;
/* 162 */
 unsigned short int gamma_corr NONANSI_FLAG;
/* 164 */
 int coord_axis NONANSI_FLAG;
 int flip_y_axis NONANSI_FLAG;
 int draw_orig_lines NONANSI_FLAG;
 int littlebulbson NONANSI_FLAG;
/* 180 */
 unsigned short int mouse_flipaxis NONANSI_FLAG;
 unsigned short int blinkinglightson NONANSI_FLAG;
 float timescale NONANSI_FLAG;
 int warn_illuminate NONANSI_FLAG;  
 int warn_frameratetoosmall NONANSI_FLAG;  
 float minclicksodist NONANSI_FLAG;
  /* minimum distance screen<->object for clicking */
 struct point e[3]; /* orientation of screen (left-handed system) */
 /* for the user-defined keys */
 struct w_keycode *ec_keycodes;
 char **txt_keycode;
 int num_keycodes;
 float mincorner; /* the maximum angle - that means min. cos(angle) -
    in a cube */
 float minweirdwall; /* the min cos(angle) of lines in a wall */
 /* some other things */
 int color[USEDCOLORS];
 unsigned char *lightcolors;
 enum tagtypes currmode;
 enum movetypes movemode;
 /* these things should only be in the level structure but I started
  with these things in here and had no sense to change this in the
  whole program */
 struct node *pcurrcube,*pcurrthing,*pcurrdoor,*pcurrpnt;
 struct wall *pcurrwall;
 int curredge,currwall;
 /* 0..3 currpnt of currwall / 0..5 cwall of ccube */
 /* the opened levels&macros */
 struct w_button *levelbutton;
 struct w_b_choose *b_macros,*b_levels; /* the button for the level/macro */
 struct list levels;
 struct leveldata *pcurrmacro;
 struct w_window *txt_window,*movewindow;
 /* the default objects */
 struct node *pdeflevel,*pdefcube; /* cube for defwall */
 int defwall;
 };
enum txttypes { txt1_normal,txt2_normal,txt1_wall,txt2_wall,txt_door,
 txt_all,txt_thing,txt_number };
struct pig_txt
 {
 char rname[8] NONANSI_FLAG;
 unsigned char frame_num NONANSI_FLAG;
 unsigned char rlxsize NONANSI_FLAG;
 unsigned char rysize NONANSI_FLAG;
 unsigned char ruxsize NONANSI_FLAG;
 unsigned char type1 NONANSI_FLAG,type2 NONANSI_FLAG;
 unsigned long int offset NONANSI_FLAG;
 /* my stuff */
 FILE *f; /* from which file comes this texture (PIG/POG) */
 char name[9];
 unsigned short xsize,ysize;
 int pigno,num_anims,anim_t2;
  /* anim_t2 indicates if animation is for texture 1 (==0) or 2 (==1) */
 unsigned char *data;
 };
#define NUM_LIGHTCOLORS 32
#define NUM_SECURITY 3
struct palette
 { char name[9]; unsigned char palette[3*256]; 
  unsigned char *lighttables,*mem_lighttables; };
struct ham_txt
 {
 unsigned long flags NONANSI_FLAG,light NONANSI_FLAG,hitpoints NONANSI_FLAG;
 short anim_seq NONANSI_FLAG,shoot_out_txt NONANSI_FLAG;
 short xspeed NONANSI_FLAG,yspeed NONANSI_FLAG;
 /* my stuff */
 short int pigno,rdlno,txtlistno[txt_number];
 struct pig_txt *pig;
 unsigned short my_light[ILLUM_GRIDSIZE*ILLUM_GRIDSIZE];
  /* this is a grid over the texture indicating the brightness of each point
     of the texture */
 };
#define BULBSIZE 8
struct pigdata
 {
 char *current_pigname,*default_pigname,*current_pogname;
 char *txt_buffer,*door_buffer,*thing_buffer;
 struct ws_bitmap *txt_bm,*door_bm,*thing_bm;
 FILE *pigfile,*pogfile;
 int num_pigtxts;
 struct pig_txt *pig_txts; /* Array of textures read out of PIG-file,
  index is number of texture in pig-file */
 int num_rdltxts;
 struct ham_txt *rdl_txts; /* Array of pointer to txts for num out of RDL,
  index is number of texture in rdl-file */
 int num_txtlist[txt_number];
 struct ham_txt **txtlist[txt_number];
  /* for every texture type a alphabetical list of pointers to textures */ 
 int num_anims;
 char *anim_txt_names; /* string with names of textures which are animated
  like doors and therefore have several rdl-numbers for an anim. */
 int *anim_starts; /* start of animations. index is animation number,
  value is rdl_num */
 struct ham_txt **anims; /* Array of pointer to txts for doors,
  index is number of animation in rdl-file */
 char *bulbname,*brokenbulbname;
 char bulb[BULBSIZE*BULBSIZE],brokenbulb[BULBSIZE*BULBSIZE];
 };
struct initdata
 {
 enum descent d_ver; 
 char *fontname,*cfgname,*lastname,*menuname,*batchfilename,*lightname,
  *convtablename;
 char *macropath,*levelpath,*cfgpath,*missionpath,levelext[4],*alllevelexts,
  *txtlistpath,*playmsnpath,*pogpath;
 char *pigpaths[desc_number];
 int xres,yres;
 int numbuttons;
 char *bnames[in_number];
 int waittime;
 struct infoitem *info[in_number];
 int infonum[in_number]; /* number of displayed items */
 int *convtxts,*convanims,num_convtxts,num_convanims,def_t1,def_t2;
 };

/* if m is a coordsystem in MATRIXMULT the vectors are in the columns */
#define MATRIXMULT(e,m,v) { int mm_i;\
 for(mm_i=0;mm_i<3;mm_i++) \
  ((e)->x[mm_i])=((m)[0].x[mm_i]*(v)->x[0]+(m)[1].x[mm_i]*(v)->x[1]+ \
   (m)[2].x[mm_i]*(v)->x[2]); }
/* and in INVMATRIXMULT in the rows */
#define INVMATRIXMULT(e,m,v) { int mm_i;\
 for(mm_i=0;mm_i<3;mm_i++) \
  ((e)->x[mm_i])=((m)[mm_i].x[0]*(v)->x[0]+(m)[mm_i].x[1]*(v)->x[1]+ \
   (m)[mm_i].x[2]*(v)->x[2]); }
   
void normalize(struct point *p);
void my_exit(void);

extern FILE *errf;
extern struct viewdata view;
extern struct initdata init;
extern struct pigdata pig;
extern struct leveldata *l;
extern struct palette palettes[NUM_PALETTES];
/* stdtypes.h */
extern int wallpts[6][4];
extern int wallno[8][2][3];
extern int oppwalls[6];
extern int nb_pnts[8][3];
extern int nb_sides[6][4];
extern struct corner stdcorners[4];
extern struct door stdncdoor,stdnonncdoor;
extern struct sdoor stdsdoor;
extern struct point stdcubepts[8];
extern struct producer stdproducer;
extern long stdorientation[9];
extern struct item stditem,stdhostage;
extern struct D1_robot D1_stdrobot;
extern struct D2_robot D2_stdrobot;
extern struct mine stdmine;
extern struct start stdstart,stdcoopstart,stdsecretstart;
extern struct reactor stdreactor;
extern unsigned long robotsize[NUM_ROBOTS],reactorsize[NUM_REACTORS],
 reactorpolyobj[NUM_REACTORS];
