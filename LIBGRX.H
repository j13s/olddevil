/**
 ** LIBGRX.H
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

#ifndef _LIBGRX_H_
#define _LIBGRX_H_

#ifndef NULL
# define NULL		0
#endif

#ifndef TRUE
# define TRUE		1
# define FALSE		0
#endif

/*
 * get rid of these stupid keywords (Thanks Intel....)
 */
#if defined(__GNUC__) && !defined(near)
# define near
# define far
# define huge
#endif


/* ================================================================== */
/*			   VERSION SELECTION			      */
/* ================================================================== */

/*
 * The GRXPLANES macro selects the version of the library to be compiled.
 * An adapter support is included in the library if its corresponding bit
 * is set:
 *	1:	1 plane (Hercules) version (drivers TBD)
 *	4:	4 plane (EGA VGA 16 color) version
 *	8:	8 plane (VGA 256 color) version
 *     16:     16 plane (VGA w/ HICOLOR DAC) version
 *     32:	8 plane MODE X - like memory organization (drivers TBD)
 *     64:	8 plane IBM 8514A and compatibles
 *    128:	8 plane S3 graphics accelerator
 *			     :
 *			     : (future extensions)
 *			     :
 *    255:	  all versions
 *    239:	  all versions but the 32K color VGA
 *		  etc...
 * Notes:
 *   The 16 plane version can not be compiled using Turbo C.
 *   Selecting a single mode version will result in a slightly smaller and
 *	faster (faster is true especially for the 256 and 32K color VGA modes)
 *	library.
 *   See also the notes about the video drivers needed for this library
 */

#define MODE_X		32
#define MODE_8514A	64
#define MODE_S3		128

#if (GRXPLANES == 0) || (GRXPLANES == 0xff)
# undef GRXPLANES
#endif

#ifndef GRXPLANES
# ifdef __TURBOC__
#   define GRXPLANES	(1 | 4 | 8 | MODE_X | MODE_8514A | MODE_S3)
# endif
# ifdef __GNUC__
#   define GRXPLANES	(1 | 4 | 8 | 16 | MODE_X | MODE_8514A | MODE_S3)
# endif
#endif

#if defined(__TURBOC__) && (GRXPLANES & 16)
# error Turbo C does not support the HiColor mode
#endif


/* ================================================================== */
/*			COMPILATION OPTIONS			      */
/* ================================================================== */

/*
 * _MAXVIDPLANESIZE
 *	If set use this value as the maximum size for a bitplane in video RAM.
 *	Set it to 1M (limit by GO32) in 32-bit mode, to 64K for the 16-bit
 *	versions. GO32 1.11+ also supports 16MB paging areas -- define
 *	_MAXBIGVIDPLSIZE as this limit.
 */

#ifdef __TURBOC__
# define _MAXVIDPLANESIZE  (1024UL*64)
#endif

#ifdef __GNUC__
# define _MAXVIDPLANESIZE  (1024UL*1024UL*1)
# define _MAXBIGVIDPLSIZE  (1024UL*1024UL*16)
#endif

/*
 * _MAXMEMPLANESIZE
 *	If set use this value as the maximum size for a bitplane in memory.
 *	(Not needed in 32-bit mode, set it to somewhat less than 64K for
 *	the 16-bit versions. Max value is 64K - 4*16 because of the way the
 *	offset to the bitplanes is calculated in 16 color modes.)
 */

#ifdef __TURBOC__
# define _MAXMEMPLANESIZE  (1020UL*64)
#endif

/*
 * _INLINE256
 *	If set, then the appropriate 256 color primitives will be
 *	expanded inline. (set pixel, read pixel)
 *	Also changes the method of pixel address calculations:
 *	instead of a packed long containing (x,y) pairs, true
 *	pixel addresses (base + y*xsize + x) will be calculated inline.
 *	This option is only effective when compiling a 256 color
 *	single mode library.
 */
#if (GRXPLANES == 8)
# ifdef __TURBOC__
#   define _INLINE256		/* won't work beyond 320x200 !!! */
# endif
# ifdef __GNUC__
#   define _INLINE256
# endif
#endif

/*
 * _INLINE32K
 *	If set, then the appropriate 32K color primitives will be
 *	expanded inline. (set pixel, read pixel)
 *	Also changes the method of pixel address calculations:
 *	instead of a packed long containing (x,y) pairs, true
 *	pixel addresses (base + y*xsize + x) will be calculated inline.
 *	This option is only effective when compiling a 256 color
 *	single mode library.
 */
#if (GRXPLANES == 16)
# ifdef __GNUC__
#   define _INLINE32K
# endif
#endif


/* ================================================================== */
/*	    PACK/UNPACK TWO SHORTS INTO A LONG INTEGER		      */
/* ================================================================== */

#ifdef __GNUC__
# define _PACK2SHORTS(h,l)	(((h) << 16) | (l))
# define _UPPERWORD(x)		((int)(((unsigned long)(x)) >> 16))
# define _LOWERWORD(x)		((int)((unsigned short)(x)))
#endif

#ifdef __TURBOC__
# define _PACK2SHORTS(h,l)	((long)((void _seg *)(h) + (void near *)(l)))
# define _UPPERWORD(x)		((unsigned int)(void _seg *)(void far *)(x))
# define _LOWERWORD(x)		((unsigned int)(x))
#endif


/* ================================================================== */
/*	    INTERNALLY USED GLOBAL VARIABLES AND FUNCTIONS	      */
/* ================================================================== */

extern  GrContext _GrContext;		/* current graphics context */
extern  GrContext _GrVidPage;		/* the whole screen */

extern  int  _GrCurrentMode;		/* current video mode */
extern  int  _GrCanBcopyInBlit;		/* separate R/W pages in adapter */
extern  int  _GrBigFrameBuffer;		/* set if frame buffer > 64K */
extern  int  _GrNumColors;		/* number of colors */
extern  int  _GrFreeColors;		/* number of free colors */
extern  int  _GrAdapterType;		/* type of video adapter */
extern  int  _GrDriverIndex;		/* low-level video routine selector */
extern  int  _GrRGBcolorMode;		/* set if VGA DAC inited to RGB mode */
extern  int  _GrRdOnlyOffset;		/* add for read-only video page */
extern  int  _GrWrOnlyOffset;		/* add for write-only video page */

extern  int  _GrScreenX;		/* screen width  */
extern  int  _GrScreenY;		/* screen height */

extern  char _GrMouseDrawn;		/* set if mouse drawn */
extern  char _GrMouseCheck;		/* set if mouse blocking needed */

extern  int  (*_GrMouseBlock)(GrContext *c,int x1,int y1,int x2,int y2);
extern  void (*_GrMouseUnBlock)(int);
extern  void (*_GrMouseUnInit)(void);

/*
 * internal scan line fill and single pixel wide line draw and pixel fill
 * routines used by several thick line and filled solid primitives
 */
typedef void (*_GrScanLineProc)(int x1,int x2,int y,void *fillarg);
typedef void (*_GrLineDrawProc)(int x1,int y1,int x2,int y2,void *fillarg);
typedef void (*_GrPixelDrawProc)(int x,int y,void *fillarg);

extern  void _GrFillSolidScanLine(int x1,int x2,int y,void *arg);
extern  void _GrFillPatternedScanLine(int x1,int x2,int y,void *arg);

extern  void _GrDrawSolidLine(int x1,int y1,int x2,int y2,void *arg);
extern  void _GrDrawSolidPixel(int x,int y,void *arg);

extern  void _GrDummySolidLine(int x1,int y1,int x2,int y2,void *arg);
extern  void _GrDummySolidPixel(int x,int y,void *arg);

#define _GrDrawPatternedLine	((_GrLineDrawProc)GrPatternFilledLine)
#define _GrDrawPatternedPixel	((_GrPixelDrawProc)GrPatternFilledPlot)
#define _GrDummyPatternedLine	_GrDummySolidLine
#define _GrDummyPatternedPixel  _GrDummySolidPixel

/*
 * internal polygon, circle and thick line scan/draw routines
 */
extern void _GrDrawPolygon(int n,int pt[][2],
    int do_close,
    int is_XOR_color,
    _GrPixelDrawProc pixelproc,
    _GrLineDrawProc  lineproc,
    void *fillarg
);

extern void _GrScanConvexPoly(int n,int pt[][2],
    int is_XOR_color,
    _GrPixelDrawProc pixelproc,
    _GrLineDrawProc  borderproc,
    _GrScanLineProc  scanfillproc,
    void *fillarg
);

extern void _GrScanPolygon(int n,int pt[][2],
    int is_XOR_color,
    _GrPixelDrawProc pixelproc,
    _GrLineDrawProc  borderproc,
    _GrScanLineProc  scanfillproc,
    void *fillarg
);

extern void _GrScanEllipse(
    int x,int y,int a,int b,
    int filled,
    int is_XOR_color,
    _GrPixelDrawProc pixelproc,
    _GrLineDrawProc  borderproc,
    _GrScanLineProc  scanfillproc,
    void *fillarg
);

#define IS_XOR_COLOR(c)		(C_OPER(c) == C_XOR)
#define IS_XOR_PATTERN(p)	((p)->gp_ispixmap ?				    \
    IS_XOR_COLOR((p)->gp_pxp_oper) :						    \
    ((IS_XOR_COLOR((p)->gp_bmp_fgcolor) && ((p)->gp_bmp_fgcolor != GrNOCOLOR)) ||   \
     (IS_XOR_COLOR((p)->gp_bmp_bgcolor) && ((p)->gp_bmp_bgcolor != GrNOCOLOR)))	    \
)

/*
 * ellipe and ellipse arc point list generators for cases which are not
 * handled by the ellipse scan algorithm. (arc and thick line cases)
 */
extern  int  _GrGenerateEllipse(int pt[][2],int cx,int cy,int rx,int ry);
extern  int  _GrGenerateEllipseArc(int pt[][2],int cx,int cy,int rx,int ry,int start,int end,int filled);
#ifdef __TURBOC__
#define MAX_ELLIPSE_PTS 256
#endif
#ifdef __GNUC__
#define MAX_ELLIPSE_PTS 1024
#endif

/* ================================================================== */
/*		       CONTEXT ACCESS MACROS			      */
/* ================================================================== */

#define GC		    GrContext
#define GV		    GrVidRAM

#define GCM_INVALID	    0
#define GCM_MYMEMORY	    1		/* set if my context memory */
#define GCM_MYCONTEXT	    2		/* set if my context structure */

#define XMAX(c)		    ((c)->gc_xmax)
#define YMAX(c)		    ((c)->gc_ymax)

#define XLO(c)		    ((c)->gc_xcliplo)
#define XHI(c)		    ((c)->gc_xcliphi)
#define YLO(c)		    ((c)->gc_ycliplo)
#define YHI(c)		    ((c)->gc_ycliphi)

#ifdef _INLINE256
# define BASE_ADDRESS(c)    ((long)((c)->gc_baseaddr))
# define LINE_OFFSET(c)	    ((c)->gc_lineoffset)
# define PIX_OFFS(c,x,y)    ((unsigned)(((y) * LINE_OFFSET(c)) + (x)))
# define PIX_ADDR(c,x,y)    ((c)->gc_frameaddr + PIX_OFFS(c,x,y))
# define PIXEL_SIZE	    1
#elif defined(_INLINE32K)
# define BASE_ADDRESS(c)    ((long)((c)->gc_baseaddr))
# define LINE_OFFSET(c)	    ((c)->gc_lineoffset)
# define PIX_OFFS(c,x,y)    ((unsigned)(((y) * LINE_OFFSET(c)) + ((x) << 1)))
# define PIX_ADDR(c,x,y)    ((c)->gc_frameaddr + PIX_OFFS(c,x,y))
# define PIXEL_SIZE	    2
#else
# define BASE_ADDRESS(c)    (0L)
# define LINE_OFFSET(c)	    (0x10000L)
# define PIX_OFFS(c,x,y)    _PACK2SHORTS(y,x)
# define PIX_ADDR(c,x,y)    ((c)->gc_frameaddr + PIX_OFFS(c,x,y))
# define COORD_X(coord)	    _LOWERWORD(coord)
# define COORD_Y(coord)	    _UPPERWORD(coord)
# define PIXEL_SIZE	    1
#endif


/* ================================================================== */
/*		    CURRENT CONTEXT ACCESS MACROS		      */
/* ================================================================== */

#define CURC		(&_GrContext)
#define SCREEN		(&_GrVidPage)

#define _GrMaxX		XMAX(CURC)
#define _GrMaxY		YMAX(CURC)
#define _GrSizeX	(_GrMaxX + 1)
#define _GrSizeY	(_GrMaxY + 1)

#define _GrLoX		XLO(CURC)
#define _GrLoY		YLO(CURC)
#define _GrHiX		XHI(CURC)
#define _GrHiY		YHI(CURC)

#define PIXEL_OFFS(x,y) PIX_OFFS(CURC,x,y)
#define PIXEL_ADDR(x,y) PIX_ADDR(CURC,x,y)


/* ================================================================== */
/*		    COLOR MANIPULATION MACROS			      */
/* ================================================================== */

#ifdef __TURBOC__
# define C_SET		0		/* color operation flags (XOR...) */
# define C_XOR		(GrXOR >> 8)
# define C_OR		(GrOR  >> 8)
# define C_AND		(GrAND >> 8)
# define C_COLOR	0xff		/* masks out operation flags */
# define C_SIMPLE(c)	(((c) & 0x300) == 0)
# define C_OPER(c)	(((c) >> 8) & 3)
# define C_OPER2(c)	(((c) >> 7) & 6)
#endif

#ifdef __GNUC__
# define C_SET		0		/* color operation flags (XOR...) */
# define C_XOR		(GrXOR >> 24)
# define C_OR		(GrOR  >> 24)
# define C_AND		(GrAND >> 24)
# define C_COLOR	0xffffff	/* masks out operation flags */
# define C_SIMPLE(c)	(((c) & 0x3000000) == 0)
# define C_OPER(c)	(((c) >> 24) & 3)
# define C_OPER2(c)	(((c) >> 23) & 6)
#endif

#define  C_SET2		0
#define  C_XOR2		(C_XOR << 1)
#define  C_OR2		(C_OR  << 1)
#define  C_AND2		(C_AND << 1)


/* ================================================================== */
/*			 UTILITY MACROS				      */
/* ================================================================== */

#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#define IABS(x)		(((x) < 0) ?  -(x) : (x))

/*
 * exchange two integers
 */
#define EXCHG(a,b) do {							\
    int __temp__ = (a);							\
    (a) = (b);								\
    (b) = __temp__;							\
} while(0)

/*
 * sort two values
 */
#define SORT2(a,b) do {							\
    if((a) > (b)) EXCHG(a,b);						\
} while(0)

/*
 * conditionally remove/restore mouse cursor for drawing primitives
 */
#define MOUSE_FLAG  char _mouse_block_flag_ = FALSE

#define MOUSE_BLOCK(cxt,x1,y1,x2,y2) do {				\
    _mouse_block_flag_ = FALSE;						\
    if(_GrMouseCheck) {							\
	_mouse_block_flag_ = (*_GrMouseBlock)(cxt,x1,y1,x2,y2);		\
    }									\
} while(0);

#define MOUSE_UNBLOCK() do {						\
    if(_mouse_block_flag_) (*_GrMouseUnBlock)(_mouse_block_flag_);	\
    _mouse_block_flag_ = FALSE;						\
} while(0);


/* ================================================================== */
/*	       MODE-DEPENDENT LOW-LEVEL DRIVER FUNCTIONS	      */
/* ================================================================== */

/*
 * Share most S3 and 8514A low-level drivers for now. Later the S3 could
 * be optimized by using mixed direct frame buffer access and 8514/A-style
 * programming where appropriate. Example:
 *  _GrPSSetPixel  could be faster using direct access
 *  _GrPSDrawLine  is faster with 8514/A style programming
 */
void _GrP4Init(int memsize);
void _GrPXInit(int memsize);
void _GrP8Init(int accesstype);

int  _GrP1ReadPixel(GC *cxt,long addr);
int  _GrP4ReadPixel(GC *cxt,long addr);
int  _GrP8ReadPixel(GC *cxt,long addr);
int  _GrPHReadPixel(GC *cxt,long addr);
int  _GrPXReadPixel(GC *cxt,long addr);
int  _GrPIReadPixel(GC *cxt,long addr);
#define _GrPSReadPixel _GrPIReadPixel

void _GrP1SetPixel(long addr,int color);
void _GrP4SetPixel(long addr,int color);
void _GrP8SetPixel(long addr,int color);
void _GrPHSetPixel(long addr,int color);
void _GrPXSetPixel(long addr,int color);
void _GrPISetPixel(long addr,int color);
#define _GrPSSetPixel _GrPISetPixel

void _GrP1SetPixRow(long addr,int color,int width);
void _GrP4SetPixRow(long addr,int color,int width);
void _GrP8SetPixRow(long addr,int color,int width);
void _GrPHSetPixRow(long addr,int color,int width);
void _GrPXSetPixRow(long addr,int color,int width);
void _GrPISetPixRow(long addr,int color,int width);
#define _GrPSSetPixRow _GrPISetPixRow

void _GrP1SetPixColumn(long addr,int color,int height);
void _GrP4SetPixColumn(long addr,int color,int height);
void _GrP8SetPixColumn(long addr,int color,int height);
void _GrPHSetPixColumn(long addr,int color,int height);
void _GrPXSetPixColumn(long addr,int color,int height);
void _GrPISetPixColumn(long addr,int color,int height);
#define _GrPSSetPixColumn _GrPISetPixColumn

void _GrP1SetPixBlock(long addr,int color,int w,int h);
void _GrP4SetPixBlock(long addr,int color,int w,int h);
void _GrP8SetPixBlock(long addr,int color,int w,int h);
void _GrPHSetPixBlock(long addr,int color,int w,int h);
void _GrPXSetPixBlock(long addr,int color,int w,int h);
void _GrPISetPixBlock(long addr,int color,int w,int h);
#define _GrPSSetPixBlock _GrPISetPixBlock

void _GrP1DrawLine(long addr,int color,int dx,int dy);
void _GrP4DrawLine(long addr,int color,int dx,int dy);
void _GrP8DrawLine(long addr,int color,int dx,int dy);
void _GrPHDrawLine(long addr,int color,int dx,int dy);
void _GrPXDrawLine(long addr,int color,int dx,int dy);
void _GrPIDrawLine(long addr,int color,int dx,int dy);
#define _GrPSDrawLine _GrPIDrawLine

void _GrP1DrawChar(long addr,int w,int h,char far *bits,int fg,int bg);
void _GrP4DrawChar(long addr,int w,int h,char far *bits,int fg,int bg);
void _GrP8DrawChar(long addr,int w,int h,char far *bits,int fg,int bg);
void _GrPHDrawChar(long addr,int w,int h,char far *bits,int fg,int bg);
void _GrPXDrawChar(long addr,int w,int h,char far *bits,int fg,int bg);
void _GrPIDrawChar(long addr,int w,int h,char far *bits,int fg,int bg);
void _GrPSDrawChar(long addr,int w,int h,char far *bits,int fg,int bg);

void _GrP1PixCopy(GC *dst,long daddr,GC *src,long saddr,int w,int h,int op);
void _GrP4PixCopy(GC *dst,long daddr,GC *src,long saddr,int w,int h,int op);
void _GrP8PixCopy(GC *dst,long daddr,GC *src,long saddr,int w,int h,int op);
void _GrPHPixCopy(GC *dst,long daddr,GC *src,long saddr,int w,int h,int op);
void _GrPXPixCopy(GC *dst,long daddr,GC *src,long saddr,int w,int h,int op);
void _GrPIPixCopy(GC *dst,long daddr,GC *src,long saddr,int w,int h,int op);
#define _GrPSPixCopy _GrPIPixCopy

void _GrP1FillPattern(int x,int y,int width,GrPattern *p);
void _GrP4FillPattern(int x,int y,int width,GrPattern *p);
void _GrP8FillPattern(int x,int y,int width,GrPattern *p);
void _GrPHFillPattern(int x,int y,int width,GrPattern *p);
void _GrPXFillPattern(int x,int y,int width,GrPattern *p);
void _GrPIFillPattern(int x,int y,int width,GrPattern *p);
void _GrPSFillPattern(int x,int y,int width,GrPattern *p);


/* ================================================================== */
/*		SELECT LOW-LEVEL VIDEO ACCESS ROUTINES		      */
/* ================================================================== */

#if GRXPLANES == 1
# define _GrReadPixel(c,a)		_GrP1ReadPixel(c,a)
# define _GrSetPixel(a,p)		_GrP1SetPixel(a,p)
# define _GrSetPixRow(a,p,w)		_GrP1SetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrP1SetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrP1SetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrP1DrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrP1DrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrP1PixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrP1FillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#if GRXPLANES == 4
# define _GrReadPixel(c,a)		_GrP4ReadPixel(c,a)
# define _GrSetPixel(a,p)		_GrP4SetPixel(a,p)
# define _GrSetPixRow(a,p,w)		_GrP4SetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrP4SetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrP4SetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrP4DrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrP4DrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrP4PixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrP4FillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#if GRXPLANES == 8
# ifdef _INLINE256
#   define _GrReadPixel(c,a)		(*(unsigned char far *)(a))
#   define _GrSetPixel(a,p) do {					\
	register unsigned char far *_aa_ = (unsigned char far *)(a);	\
	register unsigned int _pp_ = (unsigned int)(p);			\
	switch(C_OPER(_pp_)) {						\
	    case C_XOR: *_aa_ ^= _pp_; break;				\
	    case C_OR:  *_aa_ |= _pp_; break;				\
	    case C_AND: *_aa_ &= _pp_; break;				\
	    default:	*_aa_  = _pp_; break;				\
	}								\
    } while(0)
# else
#   define _GrReadPixel(c,a)		_GrP8ReadPixel(c,a)
#   define _GrSetPixel(a,p)		_GrP8SetPixel(a,p)
# endif
# define _GrSetPixRow(a,p,w)		_GrP8SetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrP8SetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrP8SetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrP8DrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrP8DrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrP8PixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrP8FillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#if GRXPLANES == 16
# ifdef _INLINE32K
#   define _GrReadPixel(c,a)		(*(unsigned short far *)(a))
#   define _GrSetPixel(a,p) do {					\
	register unsigned short far *_aa_ = (unsigned short far *)(a);  \
	register unsigned int _pp_ = (unsigned int)(p);			\
	switch(C_OPER(_pp_)) {						\
	    case C_XOR: *_aa_ ^= _pp_; break;				\
	    case C_OR:  *_aa_ |= _pp_; break;				\
	    case C_AND: *_aa_ &= _pp_; break;				\
	    default:	*_aa_  = _pp_; break;				\
	}								\
    } while(0)
# else
#   define _GrReadPixel(c,a)		_GrPHReadPixel(c,a)
#   define _GrSetPixel(a,p)		_GrPHSetPixel(a,p)
# endif
# define _GrSetPixRow(a,p,w)		_GrPHSetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrPHSetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrPHSetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrPHDrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrPHDrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrPHPixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrPHFillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#if GRXPLANES == MODE_X
# define _GrReadPixel(c,a)		_GrPXReadPixel(c,a)
# define _GrSetPixel(a,p)		_GrPXSetPixel(a,p)
# define _GrSetPixRow(a,p,w)		_GrPXSetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrPXSetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrPXSetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrPXDrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrPXDrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrPXPixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrPXFillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#if GRXPLANES == MODE_8514A
# define _GrReadPixel(c,a)		_GrPIReadPixel(c,a)
# define _GrSetPixel(a,p)		_GrPISetPixel(a,p)
# define _GrSetPixRow(a,p,w)		_GrPISetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrPISetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrPISetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrPIDrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrPIDrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrPIPixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrPIFillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#if GRXPLANES == MODE_S3
# define _GrReadPixel(c,a)		_GrPSReadPixel(c,a)
# define _GrSetPixel(a,p)		_GrPSSetPixel(a,p)
# define _GrSetPixRow(a,p,w)		_GrPSSetPixRow(a,p,w)
# define _GrSetPixColumn(a,p,h)		_GrPSSetPixColumn(a,p,h)
# define _GrSetPixBlock(a,p,w,h)	_GrPSSetPixBlock(a,p,w,h)
# define _GrDrawLine(a,p,dx,dy)		_GrPSDrawLine(a,p,dx,dy)
# define _GrDrawChar(a,w,h,b,fg,bg)	_GrPSDrawChar(a,w,h,b,fg,bg)
# define _GrPixCopy(d,da,s,sa,w,h,o)	_GrPSPixCopy(d,da,s,sa,w,h,o)
# define _GrFillPattern(x,y,wdt,p)	_GrPSFillPattern(x,y,wdt,p)
# define _SINGLE_MODE_
#endif

#define ReadPixelIndex		0
#define SetPixelIndex		1
#define SetPixRowIndex		2
#define SetPixColumnIndex	3
#define SetPixBlockIndex	4
#define DrawLineIndex		5
#define DrawCharIndex		6
#define PixCopyIndex		7
#define FillPatternIndex	8

#define NumOfHandlers		9
typedef void (*handler)(void);

#ifndef _SINGLE_MODE_
  extern int  (*_GrReadPixel)(GC *,long);
  extern void (*_GrSetPixel)(long,int);
  extern void (*_GrSetPixRow)(long,int,int);
  extern void (*_GrSetPixColumn)(long,int,int);
  extern void (*_GrSetPixBlock)(long,int,int,int);
  extern void (*_GrDrawLine)(long,int,int,int);
  extern void (*_GrDrawChar)(long,int,int,char far *,int,int);
  extern void (*_GrPixCopy)(GC *,long,GC *,long,int,int,int);
  extern void (*_GrFillPattern)(int,int,int,GrPattern *);
  extern handler  _GrResetValues[];
  extern handler *_GrResetAddresses[];
#endif

#if (GRXPLANES & 1) && defined(WANT_TO_INCLUDE_INCOMPLETE_MODES)
# define USE_DRIVER_1(func)	func
#else
# define USE_DRIVER_1(func)	_GrVoidDriver
#endif

#if (GRXPLANES & 4)
# define USE_DRIVER_4(func)	func
#else
# define USE_DRIVER_4(func)	_GrVoidDriver
#endif

#if (GRXPLANES & 8)
# define USE_DRIVER_8(func)	func
#else
# define USE_DRIVER_8(func)	_GrVoidDriver
#endif

#if (GRXPLANES & 16)
# define USE_DRIVER_H(func)	func
#else
# define USE_DRIVER_H(func)	_GrVoidDriver
#endif

#if (GRXPLANES & MODE_X) && defined(WANT_TO_INCLUDE_INCOMPLETE_MODES)
# define USE_DRIVER_X(func)	func
#else
# define USE_DRIVER_X(func)	_GrVoidDriver
#endif

#if (GRXPLANES & MODE_8514A)
# define USE_DRIVER_I(func)	func
#else
# define USE_DRIVER_I(func)	_GrVoidDriver
#endif

#if (GRXPLANES & MODE_S3)
# define USE_DRIVER_S(func)	func
#else
# define USE_DRIVER_S(func)	_GrVoidDriver
#endif

#define HERC_DRIVER		1
#define VGA16_DRIVER		2
#define VGA256_DRIVER		3
#define VGA32K_DRIVER		4
#define MODE_X_DRIVER		5
#define IBM_8514A_DRIVER	6
#define S3_DRIVER		7


#endif /* whole file */

