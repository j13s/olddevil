    Welcome to Devil, the Descent Editor for Vertices, Items and Levels.

  ____________________________________          _________________________
 /                /            /     /\        /\     \            \     \
/________________/____________/_____/  \      /  \_____\____________\_____\
\                \            \     \   \    /   /     /            /     /
 \____     ___    \     _______\     \   \  /   /     /___      ___/     /
      \    \  \    \    \/______\     \   \/   /     /   /    /   /     /
       \    \  \    \            \     \  ||  /     /   /    /   /     /____
      / \    \  \    \     _______\     \ || /     /\  /    /\  /     /     \
     /___\    \/_\    \    \/______\     \__/     /__\/    /__\/     /_______\
     \                 \            \            /            /              /
      \_________________\____________\__________/____________/______________/
            (thanks to Catalin-Andrei Stefanescu for the ASCII art)
					
                This is the README.TXT for version V2.10
         For further info refer to readme20.txt and docs/userman.doc
		
                Mail address: ubdb@rz.uni-karlsruhe.de
       Home page: http://wwwrzstud.rz.uni-karlsruhe.de/~ubdb/index.html 

Devil is free software, and you are welcome to redistribute it under certain
conditions, therefore Devil comes with ABSOLUTELY NO WARRANTY;
for details see the file 'COPYING'. Some things are copyright from DJ
Delorie, for these see 'COPYING.DJ'. The graphics library is copyright
by Csaba Biegel; see 'COPYING.CB'. The CWSDPMI is copyright by 
Charles W Sandmann, see 'CWSDPMI.DOC'. You get GNU-C with source-code on
nearly every ftp-site (for example ftp.uni-stuttgart.de).

!! Distribution with commercial or shareware programs is allowed but requires
!! notification of the author by mail or acknowledged email.

Devil is sponsored by the german magazine "PC PLAYER". They are
paying me some money to encourage further development of Devil :-)).
In return they asked me to be allowed to show up in the new
title screen. There is no other obligation. Devil IS STILL FREEWARE!

PC PLAYER can be reached by E-Mail at Devil@pcplayer.mhs.compuserve.com

ATTENTION:
There is a Devil Mailing list. Have a look at
 http://www.sky.net/~sclouse
to subscribe.
I'm subscribed in this list, too, so please mail any questions about how to 
use Devil to this list and not directly to me. I WON'T ANSWER ANY MAILS
EXCEPT FOR (USEFUL) BUG REPORTS!!! This is not very friendly but I get a lot 
of mails like "I don't know how it works, please help me."...:-(

  CONTENTS:
  
  [0] THE VERSION NUMBERS.
  
  [1] INSTALLING Devil
      [1a] What do I need to run Devil ?
      [1b] How to install this thing ?
      [1c] Change your configuration
      [1d] Devil crashes before the titlescreen appears. 
      [1e] Devil crashes after the titlescreen has appeared.

  [2] BUILDING LEVELS
      [2a] Building a level.
      [2b] Including the level in Descent (easy way).
      [2c] Including the level in Descent/Compiling your own HOG-files.
      [2d] Extracting the original levels.
      [2e] Restoring the original Descent.
      [2f] Copying macros.

  [3] BUGS AND OTHER WELL KNOWN FEATURES
      [3a] How many bugs I've to expect ?
      [3b] Report a bug.

  [4] FURTHER PLANS
      [4a] Next weeks.
      [4b] Next months.
      [4c] Never.
      
  [5] ONE OR TWO WORDS TO THE SOURCECODE
      [5a] Why is Devil.INI so long?
      [5b] Sourcecode itself.
      [5c] Extending Devil.
      [5d] Recompiling Devil.

  [A] UPDATES
  
[0] THE VERSION NUMBERS.
 Since version V1.0a Devil is numbered in the following way:
 The full working, official release is called V1.0, V1.1 and so on.
 The beta-testing versions are called V1.0a, V1.0b, V1.0c and so on.
 Please spread this thing as wide as you can.
 Minor updates and bugfixes will be called V1.01, V1.02...

[1] INSTALLING Devil

 [1a] What do I need to run Devil ?
  To run Devil you need the following things: 
   1. Descent 1 or 2 registered version (I won't release a editor for the 
      shareware version because the guys from Parallax don't want this).
   2. A fast computer with coprocessor (the program isn't optimized in
      any way yet, especially I do all the calculations needed for
      plotting with floating point, so without a copro it's very very very
      very slow, but you can try it with emu387 (is not included).
      On a 486/66 it should be quite good).
   3. A VESA-compatible graphics card.
      
 [1b] How to install this thing ?
  If you have the DevilXXX.zip file just pkunzip it with '-d' (IMPORTANT!!)
  anywhere you want. This will create a directory Devil with several
  subdirectories. Then start the editor from the Devil directory with
  "Devil". Then you get in a self-explaining (well, at least I hope it)
  configuration routine where you must enter your Descent 1/2 path and
  choose your graphics card/resolution.
  
 [1c] Change your configuration
  To change the resolution and the other settings, start Devil with
  "Devil /config".
  
 [1d] Devil crashes before the titlescreen appears. Help?
  If Devil crashes with a message like General Protection Fault or something
  like this, try a clean configuration only with HIMEM.SYS from MSDOS. This
  should work. 
  
  I don't know how to use Win95/Windows 3.1 and Devil but I know
  that there are some guys who have managed to do this. Try a short notice
  in the mailing list for help.
  
  If you get no message and Devil just quits after you started it, it is
  possible that Devil couldn't initialize the graphics mode. Then have a
  look at the Devil.BAT file. There are some drivers mentioned. Perhaps you
  can find one with a name which is similar to your card. Then change the line
    set grx20drv=vesa
  into
    set grx20drv=<drivername>
  Of course you must replace <drivername> with the name of the driver
  (i.e. cl5426)
  You can try to load a resident VESA-bios, too.
  
  It is also possible that your graphics card cannot handle the resolution
  which is currently set in the Devil.ini file.

 [1e] Devil crashes after the screen has appeared.
  Perhaps your level-file is corrupted. Try starting Devil with
   Devil /new

[2] BUILDING LEVELS
 [2a] Building a level.
  Start Devil.bat. If it does some output and then returns to the DOS-prompt
  see the previous sections. 
  
  If Devil has started, build an level. This is not an easy task, but 
  I'm sure you will see how it works.
  See the doc-files for documentation. You can find them in the Docs 
  directory. The saved levels will be in the levels-directory (that's not very
  surprising, I suppose).

 [2b] Including the level in Descent (easy way).
  Just copy your levelfile in the Descent directory and name it
  LEVEL01.RDL (the 01 can be replaced by 01-27,S1-S3) (D2LEVA-1.RL2
  for Descent 2). Descent will load this level automatically and replace 
  the original level. 
  
 [2c] Including the level in Descent/Compiling your own HOG-files.
  If you want to produce your own missions (compilations of levels) you can
  make your own HOG-files with the Mission Manager in Devil. You must store
  the MSN/HOG-file produced from Devil in your Descent directory.
  Descent will recognize these files and ask you which mission you want to
  play.
  
 [2d] Extracting the original levels.
  Also done with the Mission Manager.

 [2e] Copying macros.
  If your friend has build some macros you want to include, just copy
  them in your macros directory and start Devil.

[3] BUGS AND OTHER WELL KNOWN FEATURES
 
 [3a] How many bugs I've to expect ?
  Tip: Save early and save often (as you know it from several games)
  and sometimes with different filenames. 
  I hope there are not so many bugs that are destroying your savefile or
  make Devil not to read saved files, but I guarantee for nothing. If
  you've a really, really great level that you've build for hours and hours
  and Devil won't load him anymore, send me an EMail with a short message
  what you've done last and what's in the file Devil.err. It's always
  a good idea to look in the file Devil.err, perhaps you can do
  some repair for yourself.
  
 [3b] Report a bug.
  Report a bug to: ubdb@rz.uni-karlsruhe.de
  Please, please make a description what you have done when Devil has
  crashed and mail the level you are working on with your bug report.
  Remember: I must be able to reproduce the bug to fix it!

[4] Further plans

 [4a] Next weeks 
  Making seven cubes out of one.
  Texture mapping for one side.
  Special Devil ASCII file format.
  
 [4b] Next months
  Some corridor generators.
  A cube move mode.
  An Undo feature (perhaps this goes together with some sort of programming
   language)
  A setup program for Devil.INI, so you can change keys and menus and so on. 
  
 [4c] Never
  A bitmap editor.
  An editor for sounds or decoding the soundfile (because of lack of
   knowledge).
  
[5] ONE OR TWO WORDS TO THE SOURCECODE

 [5a] Why is Devil.INI so long?
  I've tried to keep as much data as possible out of the real sourcecode,
  especially the data which refers to the keys, the menustructure and
  the Descent data, so you can edit or complete it without compiling
  Devil once again. I will release a setup program for adding something
  to Devil.INI in a few years :-) but if you're a tough guy, you can try to
  understand and modify it with a normal editor (there are some comments in
  it).
  
 [5b] Sourcecode itself.
  If you're trying to understand the sourcecode I must give you one hint:
  The WALLS are named DOORS in the sourcecode and the SIDES are called
  WALLS. It's bit confusing, but has historical reasons (I first thought
  the WALLS are only DOORS). I suppose you'll be astonished at the do_event.c
  file especially the function array do_event. I've done it this way and
  not with a switch() because I think in this way it's easier to extend.
  Just put your procedure name at the end of this array, an corresponding
  entry in the enum evcodes in do_event.h, and the number of this
  evcode as an entry in Devil.INI/MENU.INI.
  
 [5c] Extending Devil.  
  In my opinion, the best way to extend Devil is to send me your sourcecode
  (of course). I will put a node in the credits etc.
  
 [5d] Recompiling Devil.
  If you want to recompile Devil on another compiler, just have a look at
  the w_system.c/w_system.h files. They contain all non-ANSI-C-routines Devil
  needs. The rest of Devil is strict ANSI-C. The only non-ANSI-C
  convention is that an 'int' is 4 bytes long.
  Please send me an E-Mail if you recompile Devil on another compiler so
  I can always give you the newest source-code.
  
[A] UPDATES
 The latest release is always available at
  http://wwwrzstud.rz.uni-karlsruhe.de/~ubdb/index.html (my site)
  http://www.interplay.com/website/Devil.html (official US site)
  http://www.sky.net/~sclouse (faster updated than Interplay)
  US: The Integrated Connection BBS: 317-921-8619
  UK: The Shed BBS: 
  Australia: Southern Acces BBS: 61-53-428357
  
 0.95b - first release, a bit chaotic, COPYING files missing (in some cases),
	 wrong DOC file.
 0.96b - Changed tiny bug with alternate exit, switch for doors in Devil.ini
	 renamed, correct DOC file, complete README.TXT.
 0.96b2  Fixed a lot of bugs: 
       - Connecting cube with itself is no longer possible.
       - Killing the status line sometimes.
       - Choosing does not implement beaming anymore.
       - Default filename (also change in Devil.ini)
       - Deleting is now possible with the current object if it's tagged.
	 The current object is just set to the previous (or next) object.
       - Saving is no longer aborted, if a thing is out of bounds, but
	 a warning is printed if you want to ignore this. Also the exit side.
       - If you enter something via keybord you can't delete too much anymore.
       - Fixed bug entering strings: now it's possible to use spaces.
       - Status line strings are now shortened if too long.
       - No "General Protection Fault" if you tag a non existing wall.
       - tuned bitmap routine (wasn't that difficult)
	 introduced: g, G for fitting to bitmap/side
       - Numlock is now turned on at start.
       - Normal quit made.
       - Fixed bug in initio with doortextures
       - Tried to fixed bug with macro saving but..-
     KNOWN BUGS:
       - Numlock on/off with keyboard LED does not work (look at the screen)
       - Saving the same macro under several filenames will rename the first
	 saved macro (but on the disk there are two files, I hope). 
	 Try exiting and loading Devil again.
 0.97  - Fixed the damned bug with the macros.
       - In wallmode now the texture for the wall side is shown.
       - Fixed the "General Protection Fault" bug with producers that
	 produce nothing.
       - Fixed another "GPF" when you run at the beginning of the doortypes.
       - Introduced texture1&texture2 in wall menu.
       - A normal texture2 is now also a wall texture 1.
     KNOWN BUGS:
       - Numlock on/off with keyboard LED still does not work 
	 (look at the screen)
       - Changing textures with tagged walls in wall mode doesn't work.
 0.99  - Made the SOURCE-code portable (with system.c/system.h)
       - VGA-version is now built-in.
       - fixed a bug in door textures.
       - corrected the calculation of the texture coords
         (works now even with very weird wall).
       - fixed bug in the convex/weird-routine.
       - added align to axis.
       - you can't set the exit on a wall anymore.
       - improved the helptxt.
       - changed violet lines in texture/plot window.
       - added "make room"-feature.
       - fixed bug with left upper movebutton.
       - fixed bug moving tagged cubes: they're now first all moved and then
         checked, not moved and checked one by one.
       - added "special tag" for points.
       - made hogfile-sampler.
       - changed drawing of tagged lines to one-point-thick lines
         because of system.c/system.h.
       - introduced changing rotating angle for fitbitmap.
       - fixed bug that produced a 0-Byte file if you aborted saving macros.
       - added the "align bitmaps"-feature.
       - added an other rotating mode for cubes (with 4 & 6) where the current
         cube is not rotated.
       - changed the displaying of light&coords.
       - Internal menu is now working completely.
       - added scaling of macros&textures.
       - introduced seperate rotation angle for things.
       - you can insert now a macro on the exit.
       - current position&direction is shown permanently.
       - added growing&shrinking cubes and walls.
       KNOWN BUGS: more I can describe.
 1.0a  - added button for calculating textures.
       - fixed the crash if you try to move a wall.
       - now showing hotkeys in menus.
       - connections for macros are now aligned vice versa 
         (the macro is aligned to the current side).
       - fixed the bug with the orientation of the second texture.
       - crash if changing in wall mode.
       - movebuttons won't work with mouse
       - thing texture
       - door texture
       - killing of texture
       - moving down in menu
       - growing&shrinking now calculating textures and door grfx
       - Clicking on scrolled data field is possible now.
       - The doors are now correct changed if you change the animation.
       - Setting door open - switch is now working without an error message.
       - The items/hostages are now shown yellow not blue like the tagged 
         objects.
       - A blowdoor is now initialized with correct values&texture.
       - You can now stretch the bitmap in 'fit bitmap' in x/y direction
         (try switching numlock on/off).
       - A thing is now inserted with the top to the current wall.
       - Connections are now done with the variable UConn. in Internal menue.
       - Hostages work now.
       - Cubesize is now restricted (cause of the textures).
       - Textures can now be selected with the mouse.
       - Level reading tolerates now minor bugs.
       - I hope the open-at-end bug is fixed.
       - GPF if changing cube from producer to normal type is fixed.
       - Arrange bitmaps works now.
       - Source code is now strict ANSI-C.
       - Fixed bug with Ins in side mode if a cube is surrounded by other 
         cubes.
       - Changed the read level routine a bit (can now read level26).
       - Better map (try it, with lines of in Drawing Menu).
       - Font is changed (hope that fixes the problems with several grfx 
         cards).
       - Added shift+left mousebutton for tagging several objects.
       - Added tag box for tagging several objects
       - Saving now checks for duplicate reactors and keys and starts with
         illegal numbers. Also for reactors in no 'reactor'-cubes.
       - You can change the level-name shown in Descent.
       - Light shading feature added.
       - Insert points.
       - Insert&Delete sides with tagged walls.
       - Shrink&grow now work with grid.
       - You can lock side, so their texture is not calculated if you move
         their points.
       - Fit bitmap is now with&without tagged sides.
       - Size of things is now correct (you won't see a difference).
       - You can use the cube/side tags in side/point mode.
       - If you leave Devil it starts with the same level on the same
         position.
       - You're asked is you want to leave Devil without saving changed
         you've made.
       - The play level feature is added.
 1.0b  - Fixed little bug with text in the left upper corner by fit bitmap.
       - Changed exit codes for handling GPFs.
       - Changed name of the exe-file.
       - works only with V1.4
 1.0c  - works now with V1.0 and V1.4
       - if macro saving is aborted, no file is created anymore.
       - in macro choosing text is now correct even if you've overwritten
         a macro.
       - 'side lock' is now copied in macros & saved with levels.
       - fixed a bug in savecube with the producers.
       - size of things in old levels is now set & plot correctly.
 1.0   - fixed the bugs with tagging.
       - you can switch now between x-tagged display mode and normal tagged
         display mode.
       - introduced the number of tagged objects in the upper right corner.
       - fixed the bug with updating the texture window.
       - fixed the mouse bug if you click buttons rapidly.
       - Devil now changes to cube mode after making a macro.
       - Devil works with DPMI
       - S3 cards can use the accelerated line function of the card
         (have a look in the Devil.bat file)
       - Fit bitmap in VGA mode won't work.
 1.01  - fixed some more bugs with tagging (when connecting cubes,
         light shading and align bitmaps).
       - changed the shape of the hostages.
       - S3 cards can now use the accelerated rectangle fill function also.
       - fixed the bug with the producers who forget their data.
       - you can now make a macro with a connected side as reference.
       - the old connect cubes feature is now available with Shift+Del.
         The delete all feature is only available over the menu.
       - Fixed the bug with 'Bitmaptype 13 not implemented'.
       - The whole file is now a bit more strict ANSI:
         <string.h> is included.
	 Constant strings are no longer changed.
	 connect() is now called connectsides() because it collides with
	  a Linux function.
       - The z-key in Modify-Mode now works.
       - A simple statistic is included.
 1.02  - fixed the GPF with the animation of the doors.
       - the new connect cubes feature is now working (with Del).
       - a little coordinate system is shown.
       - fixed the GPF when changing 'no switch' to 'no switch'.
       - walls with 'nothing but switch' have a texture 'Nothing' now.
       - fixed the problem with the tagging with the mouse with the box
         and the white lines.
       - now Devil know the Descent limits and gives warnings.
       - hope I fixed the bug with the play-level feature.
 2.00 (never completed, only major changes are listed)
       - new graphics interface
       - macro viewer & changer
       - loading&handling several levels        
       - mine illuminating
       - Mission Manager
 2.10 (not yet completed)
       - Descent 2 support
       - Texture List
       - Configuration utility
       
Have a good time, build many levels, bomb all enemies and send me an email

Achim (ubdb@rz.uni-karlsruhe.de)

The Tao of programming
----------------------

 Master : The Tao of programming is in every program,
          no matter of how insignificant it is.
 Student: Really in every program?
 Master : Yes, in every program.
 Student: Is the Tao of programming in a handheld-calculator?
 Master : It is in a handheld-calculator.
 Student: Is the Tao even in a digital wrist watch?
 Master : It is even in a digital wrist watch.
 Student: And is the Tao of programming in a DOS for a PC?
 The master coughed, shifted his position slightly and said:
          'The lesson is over for today.'

