/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    structs.h - global variables, structures, defines and so on.
    Copyright (C) 1995  Achim Stremplat (ubdb@rzstud1.uni-karlsruhe.de)

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
#include <grx.h>
#include <grxfont.h>
#include <mousex.h>

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
#define DW_BITMAP 0x20
#define DW_DATA 0x10
#define DW_DOORS 0x8
#define DW_ALLLINES 0x4
#define DW_CUBES 0x1
#define DW_THINGS 0x2
#define DW_DEFAULT 0x0f
/* colors */
#define BLACK 0
#define GRAYSCALE 16
#define THINGCOLORS (GRAYSCALE+1)
#define NUM_OF_THINGS 4
#define DOORCOLORS (THINGCOLORS+NUM_OF_THINGS)
#define NUM_OF_DOORS 5
#define HILIGHTCOLORS (DOORCOLORS+NUM_OF_DOORS)
#define MAXHILIGHT 4
#define MENUCOLORS (HILIGHTCOLORS+MAXHILIGHT)
#define NUM_OF_MCOLORS 6
#define USEDCOLORS (MENUCOLORS+NUM_OF_MCOLORS+1)
#define WHITE (USEDCOLORS-1)

#define MAX_OBJSTR_LENGTH 15
#define MAX_XNOBUTTON 2
#define MAX_YNOBUTTON 4

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
 struct point p[11]; /* 0 is center, 1..10 are the drawn lines */
 int color;
 /* descent stuff */
 unsigned char type1 __attribute__ ((packed));
 unsigned char type2 __attribute__ ((packed));
 unsigned short stuff[2] __attribute__ ((packed));
 unsigned short cube __attribute__ ((packed));
 long pos[3] __attribute__ ((packed));
 };
struct item
 {
 struct thing t __attribute__ ((packed));
 unsigned short stuff0[22] __attribute__ ((packed));
 long pos2[3] __attribute__ ((packed));
 unsigned long stuff1 __attribute__ ((packed));
 unsigned char stuff2[3] __attribute__ ((packed));
 unsigned long grfx __attribute__ ((packed));
 unsigned long stuff3 __attribute__ ((packed));
 unsigned char stuff4 __attribute__ ((packed));
 };
struct robot
 {
 struct thing t __attribute__ ((packed));
 long orientation[9] __attribute__ ((packed));
 long pos2[5] __attribute__ ((packed));
 unsigned char droptype,dropsubtype,drophowmany __attribute__ ((packed));
 unsigned long stuff1[39] __attribute__ ((packed));
 unsigned long texture __attribute__ ((packed));
 };
struct start
 {
 struct thing t __attribute__ ((packed));
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
 long orientation[9] __attribute__ ((packed));
 unsigned long stuff0[39] __attribute__ ((packed));
 unsigned char stuff1[3] __attribute__ ((packed));
 };
struct reactor
 {
 struct thing t __attribute__ ((packed));
 long orientation[9] __attribute__ ((packed));
 unsigned long stuff0[2] __attribute__ ((packed));
 unsigned long pos2[3] __attribute__ ((packed));
 unsigned long stuff1 __attribute__ ((packed));
 unsigned long stuff2[16] __attribute__ ((packed));
 unsigned char stuff3[3] __attribute__ ((packed));
 unsigned long texture __attribute__ ((packed));
 };
enum sdoortypes { sdtype_door,sdtype_cube };
struct sdoor
 {
 unsigned long int type __attribute__ ((packed));
 unsigned short int c0500 __attribute__ ((packed));
 unsigned short int cff00 __attribute__ ((packed));
 unsigned long int cffffffff __attribute__ ((packed));
 unsigned short int num  __attribute__ ((packed));
 unsigned short int cubes[10] __attribute__ ((packed));
 unsigned short int walls[10] __attribute__ ((packed));
 /* my stuff */
 struct node *d __attribute__ ((packed));
 struct node *target[10] __attribute__ ((packed));
 };
struct edoor
 {
 unsigned short int num __attribute__ ((packed));
 unsigned short int cubes[10] __attribute__ ((packed));
 unsigned short int walls[10] __attribute__ ((packed));
 };
struct door
 {
 unsigned long cubenum __attribute__ ((packed));
 unsigned long wallnum __attribute__ ((packed));
 unsigned short stuff1 __attribute__ ((packed)); /* 0 */
 unsigned short hitpoints __attribute__ ((packed));
 unsigned long stuff2 __attribute__ ((packed)); /* ffffffff */
 unsigned char type1,type2 __attribute__ ((packed));
 unsigned char stuff3 __attribute__ ((packed)); 
 unsigned char sdoor __attribute__ ((packed));
 unsigned char animtxt __attribute__ ((packed));
 unsigned char key __attribute__ ((packed));
 unsigned short stuff4 __attribute__ ((packed)); /* 0 */
 /* my stuff */
 struct node *d __attribute__ ((packed)); /* door from other side */
 unsigned char edoor __attribute__ ((packed));
 struct node *sd __attribute__ ((packed));
  /* the sdoor corresponding to this door */
 struct list sdoors __attribute__ ((packed)); /* doors opening this door */
 struct node *c __attribute__ ((packed));
 struct wall *w __attribute__ ((packed));
 struct point p __attribute__ ((packed));
 };
struct corner
 {
 short int xpos,ypos __attribute__ ((packed));
 short int light __attribute__ ((packed));
 };
struct wall
 {
 short int texture1,texture2 __attribute__ ((packed));
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
 struct corner corners[4] __attribute__ ((packed));
 /* my stuff */
 struct node *p[4] __attribute__ ((packed));
 int no __attribute__ ((packed));
 unsigned short int orientation __attribute__ ((packed)); 
  /* 0,1,2,3 rotating second txt */
 };
struct producer
 {
 /* producer produces all roboter with the number of the setted bit */
 unsigned long prodwhat __attribute__ ((packed));
 unsigned long c01f40000 __attribute__ ((packed));
 unsigned long c00050000 __attribute__ ((packed));
 unsigned short cubenum __attribute__ ((packed));
 unsigned short stuffnum __attribute__ ((packed));
  /* is the same as subtype in cube */
 /* my stuff */
 struct node *c __attribute__ ((packed));
 };
struct cube
 {
 unsigned short int nextcubes[6] __attribute__ ((packed));
 unsigned short int pts[8] __attribute__ ((packed));
 unsigned char type __attribute__ ((packed));
 unsigned char prodnum __attribute__ ((packed));
 unsigned short int subtype,stuff __attribute__ ((packed));
 unsigned char doors[6] __attribute__ ((packed));
 /* my stuff */
 struct list sdoors __attribute__ ((packed));
 struct wall *walls[6] __attribute__ ((packed));
 struct node *p[8],*nc[6],*d[6] __attribute__ ((packed));
 struct node *cp __attribute__ ((packed)); /* producer */
 };
struct texture
 {
 char rname[8] __attribute__ ((packed));
 unsigned char ruxsize,rxsize __attribute__ ((packed));
 unsigned char rysize __attribute__ ((packed));
 unsigned char type1,type2 __attribute__ ((packed));
 unsigned long int offset __attribute__ ((packed));
 /* my stuff */
 char name[9] __attribute__ ((packed));
 unsigned short xsize,ysize __attribute__ ((packed));
 };
/*
 sizes: 0x108,0x18,0x10,0x36,0x0,0x2a,0x10
 sdoors=description of switch walls.
 edoors=doors opening when the reactor was blown
 wdoors=producer description */
struct levelhead1
 {
 /* header */
 char lvlp[4] __attribute__ ((packed));
 unsigned long int c1 __attribute__ ((packed)),c14 __attribute__ ((packed));
 unsigned long int endofcubes __attribute__ ((packed)),
  sizeall __attribute__ ((packed));
 unsigned char stuff0 __attribute__ ((packed));
#ifdef SHAREWARE
 unsigned long numpts __attribute__ ((packed)),
  numcubes __attribute__ ((packed));
#else
 unsigned short numpts __attribute__ ((packed)),
  numcubes __attribute__ ((packed));
#endif
 };
struct levelhead2
 {
 /* header for things etc. */
 unsigned long c196705 __attribute__ ((packed)),
  sizedir __attribute__ ((packed));
 char stuff1[19] __attribute__ ((packed)); 
 unsigned long posguile __attribute__ ((packed)),
  sizeguile __attribute__ ((packed)); 
 unsigned long posthings __attribute__ ((packed)),
  numthings __attribute__ ((packed)),sizethings __attribute__ ((packed)); 
  /* size=0x108 */
 unsigned long posdoors __attribute__ ((packed)),
  numdoors __attribute__ ((packed)),sizedoors __attribute__ ((packed)); 
  /* size=0x18 */
 unsigned long posempty __attribute__ ((packed)),
  numempty __attribute__ ((packed)),sizeempty __attribute__ ((packed)); 
  /* num=0, size=0x10 */
 /* doors opening if entering a sector */
 unsigned long possdoors __attribute__ ((packed)),
  numsdoors __attribute__ ((packed)),sizesdoors __attribute__ ((packed)); 
  /* size=0x36 */
 unsigned long stuff2[3] __attribute__ ((packed));  /* always zero */
 /* doors opening after blown the reactor */
 unsigned long posedoors __attribute__ ((packed)),
  numedoors __attribute__ ((packed)),sizeedoors __attribute__ ((packed)); 
  /* size=0x2a */
 /* doors open with walls */
 unsigned long posproducer __attribute__ ((packed)),
  numproducer __attribute__ ((packed)),sizeproducer __attribute__ ((packed)); 
  /* size=0x10 */
 };
struct levelhead3
 {
 unsigned short c19 __attribute__ ((packed)); 
#ifdef SHAREWARE
 char pofs[728] __attribute__ ((packed));
  /* a long list of pofs (56*13 byte) */
#else
 char pofs[1014] __attribute__ ((packed));
  /* a long list of pofs (78*13 byte) */
#endif
 char guile[0x74] __attribute__ ((packed));  /* don't know, don't mind */
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
 double rotangel,rotscala; /* how fast will we turn */
 double gridlength,gridscala;
 int gridonoff;
 /* this is what you can set */
 struct point e0; /* Pos. of screen (in the middle) */
 struct point e[3]; /* orientation of screen (left-handed system) */
 double clickphi; /* this is the angle for clicking */
 double tsize; /* size of things */
 char *pigname; /* name of .pig file */
 unsigned int numkeymacros;
 int drawwhat;
 double dsize; /* size of beat out of doors */
 /* for the user-defined keys */
 struct eventcode *events;
 int noevents;
 int bounds[ecw_num_of_boxes][4]; /* bounds of the four possible boxes */
 /* this is what the program sets */
 int maxxsize,maxysize,xsize,ysize,xoffset,yoffset;
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
 GrLineOption taggedline;
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
 GrContext *txt_context,*txt_shape; /* for readbitmap */
 unsigned char *txt_buffer,*txt_sbuffer;
 FILE *pigfile;
 GrContext *texture,*txt_complete; /* for plotbitmap and others */
 GrContext *txt_window;
 int txt_maxxnum,txt_maxynum;
 int txt_txtxpos[2],txt_txtypos[2];
 struct list tagged[tt_number]; /* cubes,walls,pts,things */
 enum tagtypes currmode;
 struct node *pcurrcube,*oldpcurrcube,*pcurrthing,*oldpcurrthing,
  *pcurrdoor,*oldpcurrdoor;
 struct wall *pcurrwall;
 int currpnt,currwall; /* 0..3 currpnt of currwall / 0..5 cwall of ccube */
 struct node *exitcube;
 int exitwall;
 struct point *oldpcurrpnt;
 GrTextRegion menu;
 int menuoffset; /* first line to display */
 int showwhat;
 GrContext *buttonmenu;
 int bxsize,bysize,numbuttons;
 char **bnames;
 GrContext *movebuttons;
 int waittime;
 struct infoitem *info[in_number];
 int infonum[in_number],descripnum[in_number],maxrow[in_number],menuheight;
  /* infonum=number of displayed items, descripnum=number of all items 
     maxrow=maxrow of displayed items
     menuheight=number of lines which can be displayed */
 struct infoitem *curmenuitem;
 };
/*
enum types { door_type1,door_type2,door_openatend,door_key,
 thing_type,thing_grfx,num_of_types }; */
 
void normalize(struct point *p);
void my_exit(void);

extern FILE *errf;
extern struct viewdata view;
extern struct leveldata *l;
extern int altkeys[26];
extern GrFBoxColors boxcolors;
/* stdtypes.h */
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
extern struct start stdstart;
extern struct reactor stdreactor;
extern char *structname[ds_number];
  
