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
#include <math.h>
#include <time.h>

/* Well, this is not so nice, but I need it. Perhaps you must set a flag
   also. GNU-C will align characters to longs and savetool needs packed
   structures (every type as long as in sizeof()) */
#ifdef GNU_C
#define NONANSI_FLAG __attribute__ ((packed))
#else
#define NONANSI_FLAG 
#endif

#define VECTOR(c,a,b) { \
 (c)->x[0]=(double)(a)->x[1]*(b)->x[2]-(double)(a)->x[2]*(b)->x[1]; \
 (c)->x[1]=(double)(a)->x[2]*(b)->x[0]-(double)(a)->x[0]*(b)->x[2]; \
 (c)->x[2]=(double)(a)->x[0]*(b)->x[1]-(double)(a)->x[1]*(b)->x[0]; }
/*
#define NEXTCOMP(i) ((i)==2 ? 0 : (i)+1)
#define PREVCOMP(i) ((i)==0 ? 2 : (i)-1)
#define VECTORCOMP(c,a,b,i) { \
 (c)->x[i]=(double)(a)->x[NEXTCOMP(i)]*(b)->x[PREVCOMP(i)]-  \
  (double)(a)->x[PREVCOMP(i)]*(b)->x[NEXTCOMP(i)]; }
#define VECTOR2COMP(c,a,b,i,j) { VECTORCOMP(c,a,b,i); VECTORCOMP(c,a,b,j); }
*/
#define SCALAR(a,b) ((double)(a)->x[0]*(b)->x[0]+ \
 (double)(a)->x[1]*(b)->x[1]+(double)(a)->x[2]*(b)->x[2])
#define LENGTH(a) sqrt((double)SCALAR(a,a))
#define DW_BITMAP 0x40
#define DW_GRID 0x20
#define DW_DATA 0x10
#define DW_DOORS 0x8
#define DW_ALLLINES 0x4
#define DW_THINGS 0x2
#define DW_CUBES 0x1
#define DW_DEFAULT 0x0f
/* colors */
#define BLACK 0
#define GRAYSCALE 16
#define THINGCOLORS (GRAYSCALE+1)
#define NUM_OF_THINGS 4
#define DOORCOLORS (THINGCOLORS+NUM_OF_THINGS)
#define NUM_OF_DOORS 5
#define HILIGHTCOLORS (DOORCOLORS+NUM_OF_DOORS)
#define MAXHILIGHT 5
#define MENUCOLORS (HILIGHTCOLORS+MAXHILIGHT)
#define NUM_OF_MCOLORS 6
#define USEDCOLORS (MENUCOLORS+NUM_OF_MCOLORS+1)
#define WHITE (USEDCOLORS-1)

#define MAX_OBJSTR_LENGTH 15
#define MAX_XNOBUTTON 2
#define MAX_YNOBUTTON 3

struct node 
 { 
 struct node *next,*prev; 
 int no;
 union {
  struct cube *c;
  struct producer *cp;
  struct point *p;
  struct listpoint *lp;
  struct macro *m;
  struct thing *t;
  struct robot *R;
  struct reactor *X;
  struct item *I;
  struct start *S;
  struct door *d;
  struct sdoor *sd;
  struct infoitem *i;
  struct node *n;
  void *v; } d;
 };
struct list
 {
 struct node *head,*dummy,*tail;
 int size;
 };
struct point {double x[3];};
struct pixel {double x,y,d;};
struct listpoint
 {
 struct point p;
 struct list c; /* list of cubenodes. no is no of point in cube */
 };
struct thing
 {
 /* my stuff */
 struct point p[11] NONANSI_FLAG; /* 0 is center, 1..10 are the drawn lines */
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
 };
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
 };
struct movement_spinning
 { long spin_rate[3] NONANSI_FLAG; };
#define MAX_AIFLAGS 11
struct control_ai
 {
 unsigned char behavior  NONANSI_FLAG;
 unsigned char flags[MAX_AIFLAGS] NONANSI_FLAG;
 unsigned short hide_cube NONANSI_FLAG;
 unsigned short hide_index  NONANSI_FLAG;
 unsigned short path_length NONANSI_FLAG;
 unsigned short cur_path_index NONANSI_FLAG;
 unsigned short start_cube NONANSI_FLAG;
 unsigned short end_cube  NONANSI_FLAG;
 };
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
struct robot
 {
 struct thing t NONANSI_FLAG;
 struct movement_physics m NONANSI_FLAG;
 struct control_ai c NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct start
 {
 struct thing t NONANSI_FLAG;
 struct movement_physics m NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
struct reactor
 {
 struct thing t NONANSI_FLAG;
 struct render_polyobj r NONANSI_FLAG;
 };
enum sdoortypes { sdtype_door,sdtype_cube };
struct sdoor
 {
 char type_0 NONANSI_FLAG; /* always 0 */
 short flags NONANSI_FLAG;
 long value NONANSI_FLAG; /* 0x00050000 */
 long time NONANSI_FLAG; /* -1 */
 char link_num NONANSI_FLAG; /* -1 */
 unsigned short int num  NONANSI_FLAG;
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
 unsigned short stuff NONANSI_FLAG; /* 0 */
 /* my stuff */
 struct node *d NONANSI_FLAG; /* door from other side */
 unsigned char edoor NONANSI_FLAG;
 struct node *sd NONANSI_FLAG;
  /* the sdoor corresponding to this door */
 struct list sdoors NONANSI_FLAG; /* doors opening this door */
 struct node *c NONANSI_FLAG;
 struct wall *w NONANSI_FLAG;
 struct point p NONANSI_FLAG;
 };
struct corner
 {
 short int xpos,ypos NONANSI_FLAG;
 short int light NONANSI_FLAG;
 };
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
 struct node *p[4] NONANSI_FLAG;
 int no NONANSI_FLAG;
 unsigned short int orientation NONANSI_FLAG; 
  /* 0,1,2,3 rotating second txt */
 };
struct producer
 {
 /* producer produces all roboter with the number of the setted bit */
 unsigned long prodwhat NONANSI_FLAG;
 unsigned long hitpoints NONANSI_FLAG; /* 0x01f40000=32768000 */
 unsigned long interval NONANSI_FLAG; /* 0x00050000=327680 */
 unsigned short cubenum NONANSI_FLAG;
 unsigned short stuffnum NONANSI_FLAG;
  /* is the same as value in cube */
 /* my stuff */
 struct node *c NONANSI_FLAG;
 };
struct cube
 {
 unsigned short int nextcubes[6] NONANSI_FLAG;
 unsigned short int pts[8] NONANSI_FLAG;
 unsigned char type NONANSI_FLAG;
 char prodnum NONANSI_FLAG;
 unsigned short int value,light NONANSI_FLAG;
 unsigned char doors[6] NONANSI_FLAG;
 /* my stuff */
 struct list sdoors NONANSI_FLAG;
 struct wall *walls[6] NONANSI_FLAG;
 struct node *p[8],*nc[6],*d[6] NONANSI_FLAG;
 struct node *cp NONANSI_FLAG; /* producer */
 };
struct texture
 {
 char rname[8] NONANSI_FLAG;
 unsigned char ruxsize,rxsize NONANSI_FLAG;
 unsigned char rysize NONANSI_FLAG;
 unsigned char type1,type2 NONANSI_FLAG;
 unsigned long int offset NONANSI_FLAG;
 /* my stuff */
 char name[9] NONANSI_FLAG;
 unsigned short xsize,ysize NONANSI_FLAG;
 };
/*
 sizes: 0x108,0x18,0x10,0x36,0x0,0x2a,0x10
 sdoors=description of switch walls.
 edoors=doors opening when the reactor was blown
 wdoors=producer description */
struct levelhead1
 {
 /* header */
 char lvlp[4] NONANSI_FLAG;
 unsigned long int version NONANSI_FLAG; /* 0x1 */
 unsigned long int minedata_offset NONANSI_FLAG; /* 0x14 */
 unsigned long int gamedata_offset NONANSI_FLAG;
 unsigned long int hostagetxt_offset NONANSI_FLAG; /* not used */
 unsigned char version0 NONANSI_FLAG; /* always 0 */
#ifdef SHAREWARE
 unsigned long numpts NONANSI_FLAG,
  numcubes NONANSI_FLAG;
#else
 unsigned short numpts NONANSI_FLAG,
  numcubes NONANSI_FLAG;
#endif
 };
struct levelhead2
 {
 /* header for things etc. */
 unsigned short signature NONANSI_FLAG; /* 0x6705 */
 unsigned short version NONANSI_FLAG; /* reg. version=0x19 */
 long sizedir NONANSI_FLAG;
 char mine_filename[15] NONANSI_FLAG; /* never used by anyone */
 long level NONANSI_FLAG; /* always zero in not saved games I suppose */
 unsigned long posguile NONANSI_FLAG,sizeguile NONANSI_FLAG; 
 unsigned long posthings NONANSI_FLAG,
  numthings NONANSI_FLAG,sizethings NONANSI_FLAG; 
  /* size=0x108 */
 unsigned long posdoors NONANSI_FLAG,
  numdoors NONANSI_FLAG,sizedoors NONANSI_FLAG; 
  /* size=0x18 */
 unsigned long posopendoors NONANSI_FLAG,
  numopendoors NONANSI_FLAG,sizeopendoors NONANSI_FLAG; 
  /* num=0, size=0x10 */
 /* doors opening if entering a sector */
 unsigned long possdoors NONANSI_FLAG,
  numsdoors NONANSI_FLAG,sizesdoors NONANSI_FLAG; 
  /* size=0x36 */
 unsigned long stuff2[3] NONANSI_FLAG;  /* always zero */
 /* doors opening after blown the reactor */
 unsigned long posedoors NONANSI_FLAG,
  numedoors NONANSI_FLAG,sizeedoors NONANSI_FLAG; 
  /* size=0x2a */
 /* doors open with walls */
 unsigned long posproducer NONANSI_FLAG,
  numproducer NONANSI_FLAG,sizeproducer NONANSI_FLAG; 
  /* size=0x10 */
 };
struct levelhead3
 {
 unsigned short num_of_saved_pofs NONANSI_FLAG; 
  /* seems to be a bug in the specs: this number is in my registered
     levels always 25 not as it should be 78 */
#ifdef SHAREWARE
 char pofs[728] NONANSI_FLAG;
  /* a long list of pofs (56*13 byte) */
#else
 char pofs[1014] NONANSI_FLAG;
  /* a long list of pofs (78*13 byte) */
#endif
 char guile[0x74] NONANSI_FLAG;  /* don't know, don't mind */
 };
struct leveldata
 {
 struct levelhead1 lh1;
 struct levelhead2 lh2;
 struct levelhead3 lh3;
#ifdef SHAREWARE
 unsigned char end[9];
  /* always 0x00000000 0xffffffff 0x0a */
#endif
 /* my stuff. the first part (lists and doors) must be exactly like the
    macro structure */
 struct list pts,cubes,things,doors,sdoors,producers;
 struct edoor *edoors;
 char *fullname;
 char *lname; /* name of the file */
 };
struct macro
 {
 struct list pts,cubes,things,doors,sdoors,producers;
 char *shorttxt,*longtxt,*filename;
 int groupno,wallno; /* wallno is the wall where the macro is tagged on */
  /* groupno is -1 if macro is not saved yet */
 };
struct eventcode
 {
 int no;
 int flags,kbstat,key,where;
 char *txt;
 };
enum evwcodes { ecw_screen, ecw_plot, ecw_uppermenu, ecw_lowermenu,
 ecw_menuline, ecw_movebuttons, ecw_num_of_boxes };
struct objtype
 {
 int no;
 char *str;
 };
struct objdata
 { 
 int no;
 char *str;
 int size;
 struct objtype **data;
 };
enum tagtypes { tt_cube,tt_wall,tt_pnt,tt_thing,tt_door,tt_number };
enum infos { in_cube,in_wall,in_corner,in_thing,in_door,in_internal,
 in_number };
enum datastructs { ds_cube,ds_wall,ds_corner,ds_thing,ds_door,ds_internal,
 ds_leveldata,ds_sdoor,ds_producer,ds_point,ds_number };
enum infoclass { ic_dontchange,ic_display,ic_progchanges };
struct infoitem
 {
 char *txt; /* description and normal printf-like format string with
  only one variable */
 enum datastructs infonr; /* to get the corresponding structure */
 int offset,row,column,menulength,numchildren,class; 
  /* offset: pos. in structure; row,column: position in menu;
     menulength: length in menu; class<0 for side effects (->class 1),
     otherwise enum infoclass */
 struct infoitem **children; 
 int *numechildren,*numdchildren;
  /* how many entries in children (display and all) */
  /* children=infoitems depending on this infoitem */
 unsigned long *itemchildren;
  /* itemchildren=array of the numbers belonging to children */
 struct objdata *od; /* =NULL if normal field, otherwise the list of objs
  for this field. */
 int length; /* in bytes max. 4 */
 unsigned int **sideeffs; /* where to put the nums of the sideeffect.
  1. depth = number of following entries (at least two)
  2. number = number of starting structure (like ds_...) 
  3. pointers = offset of pointer 
  4. length = length of entry in bytes 
  use for this getsideno, setsideno, dosideeffects */
 int numsideeffs; /* how many side effects */
 int sidefuncnr; /* call which function */
 };
struct macrogroup
 {
 struct objtype ot;
 struct list macros;
 };
enum txttypes { txt1_normal,txt2_normal,txt1_wall,txt2_wall,txt_number };
struct viewdata
 {
 double dist,distscala; /* this is the distance viewpoint-screen */
 double maxvisibility,visscala; /* maxdistance which will be drawn */
 double movefactor; /* how fast we will move */
 double pmovefactor,movescala; /* how fast the points will move */
 double rotangle; /* how fast will we turn */
 double protangle,rotscala; /* how fast will we turn */
 double gridlength,gridscala;
 double maxconndist; /* for connectcubes */
 double clickphi; /* this is the angle for clicking */
 double tsize; /* size of things */
 double dsize; /* size of beat out of doors */
 struct point e0; /* Pos. of screen (in the middle) */
 int gridonoff,plottimes;
 /* this is what you can set */
 struct point e[3]; /* orientation of screen (left-handed system) */
 char *pigname; /* name of .pig file */
 unsigned int numkeymacros;
 int drawwhat;
 /* for the user-defined keys */
 struct eventcode *events;
 int noevents;
 int bounds[ecw_num_of_boxes][4]; /* bounds of the four possible boxes */
 /* this is what system.c sets */
 int maxxsize,maxysize,xsize,ysize,xoffset,yoffset,fontheight,fontwidth,
  smenuwidth,smenuheight;
 double xviewphi,yviewphi; /* this is the cos(view angle) */
 struct point x0,m0; /* x0 viewpoint, m0 line viewpoint-center of screen */
 double mincorner; /* the maximum angle - that means min. cos(angle) -
    in a cube */
 double minweirdwall; /* the min cos(angle) of lines in a wall */
 /* some other things */
 struct objdata *menuline;
 int nummenus;
 struct macro *pcurrmacro,**keymacros;
 char *macropath,*levelpath;
 struct objdata macros;
 int color[USEDCOLORS];
 int mouse;
 double mousesensivity;
 int brightness;
 char *palettename;
 struct objdata txtnums[txt_number],doornums;
 int *doorstarts;
 int txtoffset;
 struct texture **txtref;
 int numtxtref;
 struct texture *txts;
 int numtxtdir,txt_bmoffset;
 unsigned char *txt_buffer,*txt_sbuffer;
 FILE *pigfile;
 int fit_maxxnum,fit_maxynum,fit_xpos,fit_ypos;
 int txt_xpos,txt_ypos;
 struct list tagged[tt_number]; /* cubes,walls,pts,things */
 enum tagtypes currmode;
 struct node *pcurrcube,*oldpcurrcube,*pcurrthing,*oldpcurrthing,
  *pcurrdoor,*oldpcurrdoor;
 struct wall *pcurrwall;
 int currpnt,currwall; /* 0..3 currpnt of currwall / 0..5 cwall of ccube */
 struct node *exitcube;
 int exitwall;
 struct point *oldpcurrpnt;
 int menuoffset; /* first line to display */
 char *menubuffer;
 int showwhat;
 int bxsize,bysize,numbuttons;
 char **bnames;
 int waittime;
 struct infoitem *info[in_number];
 int infonum[in_number],descripnum[in_number],maxrow[in_number],menuheight;
  /* infonum=number of displayed items, descripnum=number of all items 
     maxrow=maxrow of displayed items
     menuheight=number of lines which can be displayed */
 struct infoitem *curmenuitem;
 };

/* if m is a coordsystem in MATRIXMULT the vectors are in the columns */
#define MATRIXMULT(e,m,v) { int i;\
 for(i=0;i<3;i++) \
  ((e)->x[i])=((m)[0].x[i]*(v)->x[0]+(m)[1].x[i]*(v)->x[1]+ \
   (m)[2].x[i]*(v)->x[2]); }
/* and in INVMATRIXMULT in the rows */
#define INVMATRIXMULT(e,m,v) { int i;\
 for(i=0;i<3;i++) \
  ((e)->x[i])=((m)[i].x[0]*(v)->x[0]+(m)[i].x[1]*(v)->x[1]+ \
   (m)[i].x[2]*(v)->x[2]); }
   
void normalize(struct point *p);
void my_exit(void);

extern FILE *errf;
extern struct viewdata view;
extern struct leveldata *l;
extern int altkeys[26];
/* stdtypes.h */
enum control_types { ct_none=0,ct_ai=1,ct_slew=5,ct_powerup=13,
 ct_control=16 };
enum movement_types { mt_none=0,mt_physics=1 };
enum render_types { rt_polyobj=1,rt_hostage=4,rt_powerup=5 };
extern int wallpts[6][4];
extern int wallno[8][2][3];
extern int oppwalls[6];
extern struct corner stdcorners[4];
extern struct door stddoor;
extern struct sdoor stdsdoor;
extern struct point stdcubepts[8];
extern struct producer stdproducer;
extern long stdorientation[9];
extern struct item stditem,stdhostage;
extern struct robot stdrobot;
extern struct start stdstart,stdcoopstart;
extern struct reactor stdreactor;
extern char *structname[ds_number];
  
