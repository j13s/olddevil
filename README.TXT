Welcome to DEVIL, the Descent Editor for Vertices, Items and Levels at all.

	    __          __  ___    ___        ___   __    __
	   |  ||\      |  ||___\   \  \      /  /  |  |  |  |
	   |  ||  \    |  | ____    \  \    /  /   |  |  |  |
	   |  | \   \  |  ||____\    \  \  /  /    |  |  |  |
	   |  |___\  | |  |______     \  \/  /     |  |  |  |____
	   |_________| |_________\     \____/      |__|  |_______\
					
		This is the README.TXT for version V0.99


DEVIL is free software, and you are welcome to redistribute it under certain
conditions, therefore DEVIL comes with ABSOLUTELY NO WARRANTY;
for details see the file 'COPYING'. go32.exe is copyright by DJ Delorie
for copying this see 'COPYING.DJ'. You get GNU-C with source-code on nearly 
every ftp-site (for example ftp.uni-stuttgart.de).

I've tried to write this file in English because I think most of you
are better in reading very bad English than correct German, but I
would prefer if someone can correct this :-))).

Don't worry that you have not DEVIL V0.98 but V0.97. V0.98 never existed.
Sorry that I'm a bit late but I the work with making DEVIL portable has
taken some days and wasn't planned.
ATTENTION:
 If you think you've build a great macro, please mail it to me. I am still
 searching for a good idea for specifying macros.

  CONTENTS:
  
  [1] INSTALLING DEVIL
      [1a] What do I need to run DEVIL ?
      [1b] How to install this thing ?
      [1c] If one file is missing what do I have to do?
      [1d] DEVIL crashes before the screen appears. 
      [1e] DEVIL runs only in VGA-mode but I have a VESA-card.
      [1f] I've tried everything but DEVIL won't work. Is there any hope?

  [2] BUILDING LEVELS
      [2a] Building a level.
      [2b] Including the level in descent (easy way).
      [2c] Including the level in descent/Compiling your own HOG-files.
      [2d] Extracting the original levels.
      [2e] Extracting other files.
      [2f] Restoring the original descent.
      [2g] Copying macros.

  [3] BUGS AND OTHER WELL KNOWN FEATURES
      [3a] How many bugs I've to expect ?
      [3b] Bugtypes.
      [3c] Report a bug.

  [4] FURTHER PLANS
      [4a] Next version.
      [4b] Soon.
      [4c] Sometimes.
      [4d] Never.
      
  [5] ONE OR TWO WORDS TO THE SOURCECODE
      [5a] Why is DEVIL.INI so long?
      [5b] Sourcecode itself.
      [5c] Extending DEVIL.
      [5d] Recompiling DEVIL.

  [A] VERSIONS
  
  [B] Things I've tried without success.
      
[1] INSTALLING DEVIL

 [1a] What do I need to run DEVIL ?
  To run DEVIL you need the following things: 
   1. DESCENT registered version (I won't release a editor for the shareware
      version because the guys from Parallax don't want this).
   2. A fast computer with coprocessor (the program isn't optimized in
      any way yet, especially I do all the calculations needed for
      plotting with floating point, so without a copro it's very very very
      very slow, but you can try it with emu387 (is not included).
      On a 486/66 it should be quite good).
   3. A VESA-compatible graphics card is recommended for 640*480*256.
      Some problems have occured with the built-in VESA driver from go32
      with the last version, so I have included this time all drivers
      I have.
   4. The following files: devil.exe, devil.ini, extract.exe or
      (new.rdl (or any other level) and palette.256).
      
 [1b] How to install this thing ?
  If you have the descedit.zip file just pkunzip it with -d in the descent
  directory. Then copy the descent.hog file to a save place (inclevel.exe
  will modify it). Since you read this file you've done this before, I
  suppose? In your DESCENT directory is now a directory called DEVIL
  with the three subdirectories LEVELS, MACROS and DRIVERS. If DEVIL
  works normally in SVGA-mode (that is when you have a texture in the
  lower right corner) you can kill DRIVERS, otherwise look at the
  sections [1e] and following.
  
  If you've several single files, first look if you've all files mentioned
  in [1a] if not see [1c]. If you've all files you must do the following
  things:
   1. make an subdirectory called "DEVIL" (the name is not important)
      in your descent directory (this is important). In this
      directory you make two subdirectories called "MACROS" and "LEVELS"
      (name and position important). The .doc files you can copy 
      whereever you want.
   2. copy devil.exe, devil.ini, readme.txt and extract.exe in the "DEVIL" 
      directory.
   3. copy any *.rdl file you have in the LEVELS directory.
   4. copy any *.rmc file you have in the MACROS directory.
   
 [1c] If one file is missing what do I have to do?
  If you've no Internet access you've a problem. Otherwise look
  at the following sites:
   - descent, registered version: buy it.
   - go32.exe,emu387: look for djgpp (gnu-c for msdos from DJ Delorie). This
      is nearly on every server.
   - palette.256, a .rdl file: if you've extract.exe you can use this
      to extract files from the hogfile.
   - devil.exe,devil.ini,extract.exe: I will try to copy these files on the
      following sites (I suppose you must look in incoming directories
      to find it):
     ftp.cdrom.com         (/pub/games/descent/incoming)
     ftp.uni-heidelberg.de (/incoming) 
     
 [1d] DEVIL crashes before the screen appears. Help?
  If DEVIL crashes with a message like General Protection Fault or something
  like this, try a clean configuration only with HIMEM.SYS from MSDOS.
  (QEMM won't work, OS/2 won't work, Windows won't work as fas as I know).
  If this also crashes, you have a system which go32 thinks is not worthy
  running DEVIL. Sorry about this, have a look at the next section.
  
  If DEVIL crashes with the message "Unknown adapter type". You have two
  chances: First look at the directory DRIVERS if you find a driver with
  a name which is similar to your card (you can also try VESADRV.VDR) and
  type before starting DEVIL
    set go32=driver drivers/<drivername>
  of course replace <drivername> with the full name of the driver
  (i.e. vesadrv.vdr).
  Also you can try to load a resident VESA-bios. Perhaps you try different
  BIOS's, not every BIOS will work with you graphics card&go32.

  If you've tried all drivers and DEVIL still prints this idiotic message
  you must use VGA - sorry. Do this with
    set go32=driver drivers/stdvga.grn
  and then start DEVIL. This should work but it's only thought as provisional
  solution (see section [1f]).
  
 [1e] DEVIL runs only in VGA-mode but I have a VESA-card.
  Try the drivers in the drivers directory. See previous section.
  
 [1f] I've tried everything but DEVIL won't work. Is there any hope?
  I hope there will be one version compiled with Borland-C++ and one compiled 
  with the emx-extender. One of this versions will hopefully run on nearly
  every configuration.  
  If you are fit in C and have enough time, look at section [5d].
   
[2] BUILDING LEVELS
 [2a] Building a level.
  Start devil.exe. If it does some output and then returns to the DOS-prompt
  look in the file devil.err and see the previous sections. 
  
  If DEVIL has started, build an level. This is not an easy task, but 
  I'm sure you will see how it works.
  See the doc-files for documentation. The saved levels will be in
  the levels-directory (that's not very surprising I suppose).

 [2b] Including the level in DESCENT (easy way).
  Just copy your levelfile in the DESCENT directory and name it
  LEVEL01.RDL (the 01 can be replaced by 01-27,S1-S3). DESCENT will
  load this level automatically and replace the original level. You can
  also use the batch file launch.bat like
   c:\descent\devil> launch my_level
  This will copy levels\my_level.rdl to ..\level01.rdl (if you're in the 
  devil directory) and start DESCENT.
  
 [2c] Including the level in descent/Compiling your own HOG-files.
  If you want to produce your own missions you can compile your own
  HOG-files with hogfile.exe. It will ask you all necessary data
  and produce the specified HOG-file. If this HOG-file and the corresponding
  MSN-file is in the DESCENT-directory, DESCENT will recognize this and
  ask you which mission you want to play.
  
 [2d] Extracting the original levels.
  This is done with extract.exe. Assuming that you will extract
  the level 01, type in the levels-directory (!):
   C:\DESCENT\DEVIL\LEVELS> ..\extract.exe level01.rdl ..\..\descent.hog
  Instead of 01 you can write 01-27, S1-S3 (S must be uppercase).
  The last parameter is the hog-file from which you want to 
  extract, if it's '..\..\descent.hog' you can omit it.
  The HOG-file is not changed.

 [2e] Extracting other files.
  The same thing as extracting the levels. extract.exe can use wildcards
  so perhaps you wanna extract all files in a seperate directory with *.*.
  Extract copies the files in the current directory.

 [2f] Restoring the original DESCENT.
  Delete all level??.rdl files from your DESCENT directory.

 [2g] Copying macros.
  If your friend has build some macros you want to include, just copy
  them in your macros directory and start DEVIL.

[3] BUGS AND OTHER WELL KNOWN FEATURES
 
 [3a] How many bugs I've to expect ?
  First I want to apologize for all those bugs you will meet (because
  there will be a lot) and thank all people who mail me bugs.
  Save early and save often (as you know it from several games). 
  I hope there are not so many bugs that are destroying your savefile or
  make DEVIL not to read saved files, but I guarantee for nothing. If you've
  a really, really great level that you've build for hours and hours and DEVIL
  won't load him anymore, write me an EMail with a short message
  what you've done last and what's in the file devil.err. It's always
  a good idea to look in the file devil.err, perhaps you can do
  some repair for yourself.
  
 [3b] Bugtypes.
  I distinguish the following bugtypes:
   A. Real, heavy bug: DEVIL crashes down with a message like
       General Protection Fault
      and a lot of registers and numbers and so on.
      If you've discoverd such a bug I hope you've saved your work so
      far. Please send me the clear readable message and the last few
      steps you've made before the crash, so I can see what to do.
   B. Middle bug: DEVIL crashes not really down, but goes to the DOS-prompt.
      In this case it's a good idea to look in the file DEVIL.ERR.
      Please send me the text written in there and the description of
      your last few actions.
   C. Weird bug: DESCENT won't load the level or crashes loading the level.
      Also report to me (but first look at the documentation for possible
      reasons). 
   D. No bug: Suggestions to improve DEVIL. Also report to me.

 [3c] Report a bug.
  Report a bug to: ubdb@rz.uni-karlsruhe.de
       or          ub0u@rz.uni-karlsruhe.de
  Just send one of the four topics under [3b] as head of your email so we
  can sort it.

[4] Further plans

 [4a] Definitely next version (available in two weeks or so).
  Inserting a cube between two other.
  Fully implement the DESCENT - specs.
  A light shading funcion over several cubes (thanks to Pat Johnson for the
                                              idea)
  Better map (don't draw the lines not necessary, like DESCENT does).
  Brooding over a better way to mark tagged objects.
  
 [4b] Next weeks (After the next version it will take a while because
                  the holidays are over then so I can't program that much
		  anymore)
  A setup program for DEVIL.INI, so you can change keys and menus and so on. 
  A macro previewer. 
  Making real descent specs if no one does it.
  Describing exactly what to do to extend DEVIL.
  Tune the plot-routine especially avoid the copro. 
  Cleaning up the program code.
  Perhaps make some sort of DEVIL-language.
  
 [4c] Sometimes
  If I get an idea how to do it, texture-mapping on one cube, so you
  can see it from the inside (??). 
  Decoding POF-files and making an editor for them.
  
 [4d] Never
  A bitmap editor.
  An editor for sounds or decoding the soundfile (because of lack of
   knowledge).
  
[5] ONE OR TWO WORDS TO THE SOURCECODE

 [5a] Why is DEVIL.INI so long?
  I've tried to keep as much data as possible out of the real sourcecode,
  especially the data which refers to the keys, the menustructure and
  the DESCENT data, so you can edit or complete it without compiling
  DEVIL once again. I will release a setup program for adding something
  to DEVIL.INI but if you're a tough guy, you can try to understand and
  modify it with a normal editor (I put some comments in it, but I don't
  know if they're up to date).
  
 [5b] Sourcecode itself.
  If you're trying to understand the sourcecode I must give you one hint:
  The WALLS are named DOORS in the sourcecode and the SIDES are called
  WALLS. It's bit confusing, but has historical reasons (I first thought
  the WALLS are only DOORS). I suppose you'll be astonished at the do_event.c
  file especially the function array do_event. I've done it this way and
  not with a switch() because I think in this way it's easier to extend.
  Just put your procedure name at the end of this array, an corresponding
  entry in the enum evcodes in do_event.h, and the number of this
  evcode as an entry in DEVIL.INI.
  
 [5c] Extending DEVIL.  
  In my opinion, the best way to extend DEVIL is to send me your sourcecode
  (of course). I will put a node in the credits and make an event in
  DEVIL for it. If you want to do this for yourself, look at [5b] or wait
  some time until I've released a setup program and fully information
  about how extending DEVIL.
  
 [5d] Recompiling DEVIL.
  If you want to recompile DEVIL on another compiler, just have a look at
  the system.c/system.h files. They contain all non-ANSI-C-routines DEVIL
  needs. The rest of DEVIL is fully ANSI-C with two exceptions:
   1. Declaring a local array with variable length is allowed.
   2. Empty braces are allowed as initialisiers of arrays (look at stdtypes.h)
  Please send me an E-Mail if you recompile DEVIL on another compiler so
  I can always give you the newest source-code.
  
[A] VERSIONS
 0.95b - first release, a bit chaotic, COPYING files missing (in some cases),
	 wrong DOC file.
 0.96b - Changed tiny bug with alternate exit, switch for doors in devil.ini
	 renamed, correct DOC file, complete README.TXT.
 0.96b2  Fixed a lot of bugs: 
       - Connecting cube with itself is no longer possible.
       - Killing the status line sometimes.
       - Choosing does not implement beaming anymore.
       - Default filename (also change in devil.ini)
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
	 Try exiting and loading devil again.
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
       - added button for calculating textures.
       - fixed the crash if you try to move a wall.
       - now showing hotkeys in menus.
       - connections for macros are now aligned vice versa 
         (the macro is aligned to the current side).
       - fixed the bug with the orientation of the second texture.

[B] Things I've tried without success.
 Hope this section won't get very long:
  - Drawing a grid. Well, this is a difficult point. I've tried it with
    points&lines and with different distances and it never looks like
    this can help anyone to make a level. If someone has a good idea how
    to draw a three dimensional grid, mail me.
       
Have a good time, build many levels, bomb all enemies, send me an email

Achim (ubdb@rz.uni-karlsruhe.de)
