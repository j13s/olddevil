/**
 ** SETMODE.C
 **
 **  Copyright (C) 1992, Csaba Biegl
 **    820 Stirrup Dr, Nashville, TN, 37221
 **    csaba@vuse.vanderbilt.edu
 **
 **  This file is distributed under the terms listed in the document
 **  "copying.cb", available from the author at the address above.
 **  A copy of "copying.cb" should accompany this file; if not, a copy
 **  should be available from where this file was obtained.  This file
 **  may not be distributed without a verbatim copy of "copying.cb".
 **  You should also have received a copy of the GNU General Public
 **  License along with this program (it is in the file "copying");
 **  if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
 **  Cambridge, MA 02139, USA.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **/

#define  _SETMODE_C_

#include "grx.h"
#include "libgrx.h"
#include "grdriver.h"
#include "interrup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


GrContext _GrContext;			/* current graphics context */
GrContext _GrVidPage;			/* the whole screen */

int  _GrCurrentMode    = GR_default_text;	/* current video mode */
int  _GrCanBcopyInBlit = FALSE;		/* separate R/W pages in adapter */
int  _GrBigFrameBuffer = FALSE;		/* set if frame buffer > 64K */
int  _GrNumColors      = 256;		/* number of colors */
int  _GrAdapterType    = GR_VGA;	/* type of video adapter */
int  _GrDriverIndex    = 0;		/* low-level video routine selector */
int  _GrRdOnlyOffset   = 0;		/* add for read-only video page */
int  _GrWrOnlyOffset   = 0;		/* add for write-only video page */

int  _GrScreenX = 80;			/* screen width  */
int  _GrScreenY = 25;			/* screen height */

char _GrMouseDrawn = 0;			/* set if mouse drawn */
char _GrMouseCheck = 0;			/* set if mouse blocking needed */

static void dummy(void) {}

int  (*_GrMouseBlock)(GrContext *c,int x1,int y1,int x2,int y2);
void (*_GrMouseUnBlock)(int);
void (*_GrMouseUnInit)(void) = dummy;

static void (*setmodehook)(void) = NULL;

#ifndef _SINGLE_MODE_
handler  _GrResetValues[NumOfHandlers]	  = { NULL };
handler *_GrResetAddresses[NumOfHandlers] = { NULL };
#endif

struct mode_entry {
    short w,h,c,mode_no;
};

static struct mode_entry mode_table[] = {
    { 80,  25,  2,   0x07 },	/* text modes */
    { 40,  25,  16,  0x01 },
    { 80,  25,  16,  0x03 },
    { 320, 200, 16,  0x0d },	/* graphics modes */
    { 640, 200, 16,  0x0e },
    { 640, 350, 16,  0x10 },
    { 640, 480, 16,  0x12 },
    { 800, 600, 16,  (-1) },	/* we have to trust the user's mode number on this */
    { 320, 200, 256, 0x13 },
    { 640, 480, 256, 0x101 }
};

static int set_BIOS_mode(int BIOSno,int wdt,int hgt,int col,int *modep)
{
	struct mode_entry *mp = mode_table;
	int count = sizeof(mode_table) / sizeof(mode_table[0]);
	REGISTERS reg;
	int retval;

	for( ; --count >= 0; mp++) {
	    if(mp->w != wdt) continue;
	    if(mp->h != hgt) continue;
	    if(mp->c != col) continue;
	    if(wdt >= 320) {
		if(mp->mode_no != (-1)) BIOSno = mp->mode_no;
		retval = (col == 16) ? (GRD_VGA | GRD_4_PLANES) : (GRD_VGA | GRD_8_PLANES);
		*modep = GR_width_height_color_graphics;
	    }
	    else {
		BIOSno = mp->mode_no;
		retval = GRD_VGA | GRD_4_PLANES;
		*modep = GR_width_height_text;
	    }
	    if(BIOSno>0xff)
	     { reg.r_ax=0x4f02; reg.r_bx=BIOSno;
	       retval|=GRD_RW_64K; } /* SUPER VGA */
	    else reg.r_ax = BIOSno;
	    int10(&reg);
	    _GrScreenX = wdt;
	    _GrScreenY = hgt;
	    return(retval);
	}
	return(-1);
}

static void int_setmode(int mode,int BIOSno,int width,int height,int colors)
{
	long planesize;
	int  flags;
	int  planes;

	(*_GrMouseUnInit)();
#ifndef _SINGLE_MODE_
	for(flags = 0; flags < NumOfHandlers; flags++) {
	    if(_GrResetValues[flags] == NULL) continue;
	    *(_GrResetAddresses[flags]) = _GrResetValues[flags];
	}
#endif
	if((mode >= GR_80_25_text) && (mode <= GR_width_height_color_graphics)) {
	    memset(&_GrVidPage,0,sizeof(GrContext));
	    flags = (BIOSno > 0) ?
		set_BIOS_mode(BIOSno,width,height,colors,&mode) :
		(int)_GrLowSetMode(mode,width,height,colors);
	    _GrCurrentMode = mode;
	    switch(flags & GRD_TYPE_MASK) {
	      case GRD_VGA:
		_GrVidPage.gc_baseaddr = (char far *)VGA_FRAME;
		_GrAdapterType = GR_VGA;
		break;
	      case GRD_EGA:
		_GrVidPage.gc_baseaddr = (char far *)EGA_FRAME;
		_GrAdapterType  = GR_EGA;
		break;
	      case GRD_HERC:
		_GrVidPage.gc_baseaddr = (char far *)HERC_FRAME;
		_GrAdapterType  = GR_HERC;
		break;
	      case GRD_8514A:
		_GrAdapterType  = GR_8514A;
		break;
	      case GRD_S3:
		_GrAdapterType  = GR_S3;
		break;
	      default:
		_GrLowSetMode(GR_default_text,0,0,0);
		fputs("GrSetMode: unknown adapter type in driver\n",stderr);
		fprintf(stderr,"ADAPERTYPE=%x\n",flags);
		exit(1);
	    }
	    switch(planes = (flags & GRD_PLANE_MASK)) {
#if (GRXPLANES & 16)
	      case GRD_16_PLANES:
		if(_GrAdapterType != GR_VGA) goto Default;
		_GrDriverIndex = VGA32K_DRIVER;
		_GrNumColors = 32768;
		break;
#endif
#if (GRXPLANES & 8) || (GRXPLANES & MODE_8514A)
	      case GRD_8_PLANES:
	      case GRD_8_F_PLANES:
		switch(_GrAdapterType) {
		  case GR_VGA:
		    _GrDriverIndex = VGA256_DRIVER;
		    break;
		  case GR_8514A:
		    _GrDriverIndex = IBM_8514A_DRIVER;
		    break;
		  case GR_S3:
		    _GrDriverIndex = S3_DRIVER;
		    break;
		  default:
		    goto Default;
		}
		_GrNumColors = 256;
		break;
#endif
#if (GRXPLANES & 4)
	      case GRD_4_PLANES:
		if(_GrAdapterType == GR_HERC) goto Default;
		_GrDriverIndex = VGA16_DRIVER;
		_GrNumColors = 16;
		break;
#endif
#if (GRXPLANES & 1)
	      case GRD_1_PLANE:
		if(_GrAdapterType != GR_HERC) goto Default;
		_GrDriverIndex = HERC_DRIVER;
		_GrNumColors = 2;
		break;
#endif
	      default:
	      Default:
		_GrLowSetMode(GR_default_text,0,0,0);
		fputs("GrSetMode: bad color plane # in driver\n",stderr);
		exit(1);
	    }
	    if(mode >= GR_320_200_graphics) {
		planesize = GrPlaneSize(_GrScreenX,_GrScreenY);
#ifdef _MAXVIDPLANESIZE
		if((_GrAdapterType != GRD_8514A) &&
		   (_GrAdapterType != GRD_S3) &&
		   (_GrAdapterType != GRD_W9000) &&
		   (planesize > _MAXVIDPLANESIZE)) {
#ifdef _MAXBIGVIDPLSIZE
		    if(planesize <= _MAXBIGVIDPLSIZE) {
			_GrVidPage.gc_baseaddr = (char far *)BIG_VGA_FRAME;
			_GrBigFrameBuffer = TRUE;
			if((flags & GRD_PAGING_MASK) == GRD_RW_64K) {
			    _GrCanBcopyInBlit = TRUE;
			    _GrRdOnlyOffset   = BIG_RDONLY_OFF;
			    _GrWrOnlyOffset   = BIG_WRONLY_OFF;
			}
			else {
			    _GrCanBcopyInBlit = FALSE;
			    _GrRdOnlyOffset   = 0L;
			    _GrWrOnlyOffset   = 0L;
			}
		    }
		    else
#endif
		    {
			_GrLowSetMode(GR_default_text,0,0,0);
			fputs("GrSetMode: graphics mode too big\n",stderr);
			exit(1);
		    }
		}
		else
#endif
		if(planesize < 0x10000L) {
		    _GrCanBcopyInBlit = TRUE;
		    _GrBigFrameBuffer = FALSE;
		    _GrRdOnlyOffset   = 0;
		    _GrWrOnlyOffset   = 0;
		}
		else if((flags & GRD_PAGING_MASK) == GRD_RW_64K) {
		    _GrCanBcopyInBlit = TRUE;
		    _GrBigFrameBuffer = TRUE;
		    _GrRdOnlyOffset   = RDONLY_OFF;
		    _GrWrOnlyOffset   = WRONLY_OFF;
		}
		else {
		    _GrCanBcopyInBlit = FALSE;
		    _GrBigFrameBuffer = TRUE;
		    _GrRdOnlyOffset   = 0;
		    _GrWrOnlyOffset   = 0;
		}
		_GrVidPage.gc_onscreen	 = TRUE;
		_GrVidPage.gc_frameaddr  = BASE_ADDRESS(&_GrVidPage);
		_GrVidPage.gc_lineoffset = GrLineOffset(_GrScreenX);
		_GrVidPage.gc_xmax = _GrVidPage.gc_xcliphi = _GrScreenX - 1;
		_GrVidPage.gc_ymax = _GrVidPage.gc_ycliphi = _GrScreenY - 1;
		_GrContext = _GrVidPage;
		switch(_GrNumColors) {
		    case 16:  _GrP4Init(flags & GRD_MEM_MASK); break;
#ifndef _INLINE256
		    case 256: if(_GrAdapterType == GR_VGA) _GrP8Init(planes); break;
#endif
		    default:  break;
		}
		GrRefreshColors();
		_GrMouseDrawn = 0;
		_GrMouseCheck = 0;
	    }
	    if(setmodehook != NULL) (*setmodehook)();
	}
}

void GrSetMode(int mode,int width,int height,int colors)
{
	int_setmode(mode,(-1),width,height,colors);
}

void GrSetBIOSMode(int BIOSno,int width,int height,int colors)
{
	int_setmode(GR_80_25_text,((BIOSno > 0) ? BIOSno : 1),width,height,colors);
}

void GrSetModeHook(void (*hookfunc)(void))
{
	setmodehook = hookfunc;
}

int GrCurrentMode(void)
{
	return(_GrCurrentMode);
}

int GrAdapterType(void)
{
	return(_GrAdapterType);
}

int GrScreenX(void)
{
	return(_GrScreenX);
}

int GrScreenY(void)
{
	return(_GrScreenY);
}

int GrSizeX(void)
{
	return(_GrContext.gc_xmax + 1);
}

int GrSizeY(void)
{
	return(_GrContext.gc_ymax + 1);
}

int GrMaxX(void)
{
	return(_GrContext.gc_xmax);
}

int GrMaxY(void)
{
	return(_GrContext.gc_ymax);
}

