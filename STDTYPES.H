/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    stdtypes.h - the default things you insert.
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
    
/* stdtypes for unknown bytes/constants */
#define STDORIENTATION { 0x10000,0,0,0,0x10000,0,0,0,0x10000 }
long stdorientation[9] = STDORIENTATION;
#define EMPTYPNT { 0,0,0 }
#define D_EMPTYPNT {EMPTYPNT}
#define T_EMPTYPNTS { D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT,\
 D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT,D_EMPTYPNT }
#define T_EMPTYCNTRLPUP { 0 }
#define T_EMPTYMOVEPHYSICS { {0,0,0},{0,0,0},0,0,0,{0,0,0},{0,0,0},0,0 }
#define T_EMPTYCNTRLAI { 0x81,{ 0,0,0,0,0,0,0,0,0,0,0 },0,0,0,0,0,0 }
#define D2_T_EMPTYCNTRLAI { 0x81,{ 0,0,0,0,0,0,0,0,0,0,0 },0,0,0,0 }
#define T_EMPTYCNTRLMINE { 5,0,0x9f }
#define EMPTYLST { NULL,NULL,NULL,0 }
struct item stditem =
 { { T_EMPTYPNTS,NULL,NULL,2,tt1_item,1,ct_powerup,mt_none,rt_powerup,0,0,
     EMPTYPNT,STDORIENTATION,0x20000,0,EMPTYPNT,0xff,0,0 },T_EMPTYCNTRLPUP,
     { 0x12,0,0 } };
struct item stdhostage =
 { { T_EMPTYPNTS,NULL,NULL,2,tt1_hostage,0,ct_powerup,mt_none,rt_hostage,0,0,
     EMPTYPNT,STDORIENTATION,0x50000,0,EMPTYPNT,0xff,0,0 },T_EMPTYCNTRLPUP,
     { 0x21,0,0 } };
#define T_EMPTYANIMANGLES { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},\
 {0,0,0},{0,0,0},{0,0,0},{0,0,0} }
#define T_EMPTYPOLYOBJ { 0,T_EMPTYANIMANGLES,0,-1 }
struct mine stdmine =
 { { T_EMPTYPNTS,NULL,NULL,0,tt1_mine,0x33,ct_mine,mt_physics,rt_polyobj,0,0,
     EMPTYPNT,STDORIENTATION,0x242be,0x140000,EMPTYPNT,0xff,0,0 },
     { { 0,0,0 },{ 0,0,0 },0x10000,0x872,0,{ 0,0,0 },{ 0,0,0 },0,0x104 },
     T_EMPTYCNTRLMINE,T_EMPTYPOLYOBJ };
struct D1_robot D1_stdrobot =
 { { T_EMPTYPNTS,NULL,NULL,0,tt1_robot,3,ct_ai,mt_physics,rt_polyobj,0,0,
     EMPTYPNT,STDORIENTATION,0x4d5ed,0,EMPTYPNT,0xff,0,0 },T_EMPTYMOVEPHYSICS,
     T_EMPTYCNTRLAI,T_EMPTYPOLYOBJ };
struct D2_robot D2_stdrobot =
 { { T_EMPTYPNTS,NULL,NULL,0,tt1_robot,3,ct_ai,mt_physics,rt_polyobj,0,0,
     EMPTYPNT,STDORIENTATION,0x4d5ed,0,EMPTYPNT,0xff,0,0 },T_EMPTYMOVEPHYSICS,
     D2_T_EMPTYCNTRLAI,T_EMPTYPOLYOBJ };
unsigned long robotsize[NUM_ROBOTS]={
 0x06172b,0x05a11d,0x06ee3a,0x04d5ed,0x0501a1,0x05453f,0x06172b,0x047a24, 
 0x0497c6,0x04b5ed,0x03c2dd,0x045317,0x045317,0x026a67,0x0377a0,0x0923ae, 
 0x06172b,0x185f25,0x046e0e,0x05453f,0x04f127,0x052e20,0x047c95,0x1605c9, 
 0x05c631,0x063b14,0x08f2b6,0x049cd7,0x07ffc8,0x062f21,0x0b3ced,0x0d9d38, 
 0x185f25,0x031fae,0x047c95,0x047c95,0x0568e6,0x039aa0,0x05b213,0x055bef, 
 0x068dc8,0x029523,0x04bc93,0x05c545,0x05d29d,0x10cf73,0x0d0437,0x0675bf, 
 0x06251d,0x046bd0,0x02dc22,0x058269,0x0f2c46,0x0b9270,0x068dc8,0x063b14, 
 0x046bd0,0x063b14,0x046bd0,0x05c631,0x06251d,0x029523,0x140610,0x029523,   
 0x0dd012,0x0f7bdd };
/* control for the start is set at saving */
struct start stdstart =
 { { T_EMPTYPNTS,NULL,NULL,1,tt1_dmstart,0,ct_none,mt_physics,rt_polyobj,0,0,
     EMPTYPNT,STDORIENTATION,310325,0,EMPTYPNT,0xff,0,0 },T_EMPTYMOVEPHYSICS,
     T_EMPTYPOLYOBJ };
struct start stdcoopstart =
 { { T_EMPTYPNTS,NULL,NULL,1,tt1_coopstart,8,ct_none,mt_physics,rt_polyobj,0,
     0,EMPTYPNT,STDORIENTATION,310325,0,EMPTYPNT,0xff,0,0 },
     T_EMPTYMOVEPHYSICS,T_EMPTYPOLYOBJ };
struct start stdsecretstart =
 { { T_EMPTYPNTS,NULL,NULL,1,tt1_secretstart,0,ct_none,mt_physics,rt_polyobj,
     0,0,EMPTYPNT,STDORIENTATION,310325,0,EMPTYPNT,0xff,0,0 },
     T_EMPTYMOVEPHYSICS,T_EMPTYPOLYOBJ };
unsigned long reactorsize[NUM_REACTORS]=
 { 0xc14ed,0xf7b7b,0xb25a4,0xb6b07,0xd59db,0xaf83c,0xb91df };
unsigned long reactorpolyobj[NUM_REACTORS]=
 { 0x5d,0x5f,0x61,0x63,0x65,0x67,0x69 };
struct reactor stdreactor =
 { { T_EMPTYPNTS,NULL,NULL,3,9,1,ct_control,mt_none,rt_polyobj,0,0,EMPTYPNT,
     STDORIENTATION,0xc14ed,0xc81999,EMPTYPNT,0xff,0,0 },{ 0x5f,
     T_EMPTYANIMANGLES,0,-1 } };
struct producer stdproducer=
 { {0,0},0x01f40000,0x00050000,0,0,NULL };
struct sdoor stdsdoor=
 { switch_opendoor,0,0,0x050000,-1,{0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0} };
struct door stdncdoor=
 { 0,0,-1,-1,door1_normal,door2_closeagain,0,0xff,4,1,0,0,0,switch_nothing,
   NULL,NULL,EMPTYLST,NULL,NULL,D_EMPTYPNT,NULL,0,NULL };
struct door stdnonncdoor=
 { 0,0,-1,-1,door1_switchwithwall,0,0,0xff,0xff,-1,0,0,0,switch_nothing,NULL,
   NULL,EMPTYLST,NULL,NULL,D_EMPTYPNT,NULL,0,NULL };
struct point stdcubepts[8]={
 {{655360,655360,-655360}},{{655360,-655360,-655360}},
 {{-655360,-655360,-655360}},{{-655360,655360,-655360}},
 {{655360,655360,655360}},{{655360,-655360,655360}},
 {{-655360,-655360,655360}},{{-655360,655360,655360}} };
struct corner stdcorners[4]={
 { { 0,0 },0x6000 }, { { 0,2047 },0x6000}, { { -2047,2047 },0x6000}, 
   { { -2047,0 },0x6000} };
/* wall opposite to index wall */
int oppwalls[6]={ 2,3,0,1,5,4 };
/* first index is the wallno, second the number of the corner */
int wallpts[6][4]=
 { {7,6,2,3}, {0,4,7,3}, {0,1,5,4}, {2,6,5,1}, {4,5,6,7}, {3,2,1,0} };
/* first index pointnum, second wallno/cornernum, third the three walls/co. */
int wallno[8][2][3]={ 
 { {1,2,5}, {0,0,3} }, 
 { {2,3,5}, {1,3,2} },
 { {0,3,5}, {2,0,1} },
 { {0,1,5}, {3,3,0} },
 { {1,2,4}, {1,3,0} },
 { {2,3,4}, {2,2,1} },
 { {0,3,4}, {1,1,2} },
 { {0,1,4}, {0,2,3} } };
/* 'neighboured' points. first index is pointnum, second index
   are the three neighbours */
int nb_pnts[8][3]={ {3,1,4}, {0,2,5}, {1,3,6}, {2,0,7}, 
 {7,5,0}, {4,6,1}, {5,7,2}, {6,4,3} };
/* neighboured sides. first index is sidenum, second index are the four
   neighbours (the sides sharing one line with the former side). The
   second index starts with the line wallpts[first index][0->1] and
   ends with the line wallpts[1. index][3->0]. */
int nb_sides[6][4]=
 { { 4,3,5,1 },{ 2,4,0,5 },{ 5,3,4,1 },{ 0,4,2,5 },{ 2,3,0,1 },{ 0,3,2,1 } };
