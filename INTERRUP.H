/**
 ** INTERRUP.H
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

#ifdef __TURBOC__
# include <dos.h>
# define  int10(regs)	intr(0x10,regs)
# define  int33(regs)	intr(0x33,regs)
# define  REGISTERS	struct REGPACK
#endif

#ifdef __GNUC__
# include <sys/registers.h>
# define  r_ax  ax
# define  r_bx  bx
# define  r_cx  cx
# define  r_dx  dx
# define  r_bp  bp
# define  r_si  si
# define  r_di  di
  extern  void  int10(REGISTERS *regs);
  extern  void  int33(REGISTERS *regs);
#endif


