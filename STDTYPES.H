/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    stdtypes.h - the default things you insert.
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
    
/* stdtypes for unknown bytes/constants */
struct item stditem =
 { { {},2,7,1,{0xd,5},0,{} },{},{},0,{},0x12,0,0 };
struct item stdhostage =
 { { {},2,7,1,{0xd,4},0,{} },{},{},0,{},0x21,0,0 };
#define STDORIENTATION { 0x10000,0,0,0,0x10000,0,0,0,0x10000 }
long stdorientation[9] = STDORIENTATION;
struct robot stdrobot =
 { { {},0,2,3,{0x0101,1},0,{} },STDORIENTATION,{},0xff,0,0,{},0xffffffff };
struct start stdstart =
 { { {},1,4,0,{0x0105,1},0,{} },STDORIENTATION,{},{} };
struct reactor stdreactor =
 { { {},3,9,0,{0x0010,1},0,{} },STDORIENTATION,{0xc14ed,0xc81999},{},
    0x270000ff,{},{},0xffffffff };
struct producer stdproducer=
 { 0,0x01f40000,0x00050000,0,0,NULL };
struct sdoor stdsdoor=
 { 0x0100,0x0500,0xff00,0xffffffff,0,{},{} };
struct door stddoor=
 { 0,0,0,0,0xffffffff,2,0x10,0,0xff,4,1,0,NULL,0,NULL,{},NULL,NULL,
   {{0.0,0.0,0.0}} };
struct point stdcubepts[8]={
 {{0,0,0}},{{0,-655360*2,0}},
 {{-655360*2,-655360*2,0}},{{-655360*2,0,0}},
 {{0,0,655360*2}},{{0,-655360*2,655360*2}},
 {{-655360*2,-655360*2,655360*2}},{{-655360*2,0,655360*2}} };
struct corner stdcorners[4]={
 { 0,0,0x6000 }, {0,2047,0x6000}, {-2048,2047,0x6000}, {-2048,0,0x6000} };
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


