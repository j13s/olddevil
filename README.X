Hastrup August 1998

This is a modified version of Devil based on Devil 2.2l.

You run this modified version by typing DEVILX on your commandline.

This modified version of Devil is NOT supported by Achim Stremplat,
so don't bother him with bugs you find in the modified version.

If you encounter a bug using this modified version, then reconstruct
it with the original version of Devil (2.2l) before you submit a bug
report to Achim (on DDL, see http://www.descent2.com/ddl/).
If you can't reconstruct the bug, then it is probably a bug I have
introduced while making this version.

I do not offer any support at all, but you can try to reach me at DDL
(see http://www.descent2.com/ddl/).
I just made this version to make my own level building easier.

Read readme.txt for info about Copyright & Warranty.

============== Devil 2.2LC2 =========================

Bugs fixed:
- The UVL fields in the Edge (corner) box, now works.

Things changed:
- Devil will, in Descent 1 mode, now search for a DTX 1 PIG patch file in
  the same directory as the loaded level.
  It will first look for a DTX or PG1 file with the same name as the
  (RDL) level file, if non is found it will look for a file called
  DEVIL.DTX or DEVIL.PG1.
  If a PIG patch file is found it is used to display the modified textures
  in the level just like when a POG file is used when making Descent 2 levels.
  If no PIG patch file is found the normal Descent 1 textures are displayed.
  This will allow you to develope levels using D1X with custom textures.
- When "File->Play Level" or "File->Save to Descent Directory" is used, Devil will 
  automaticly add any PG1/DTX and HX1 files belonging to the level which is
  found in the level's directory.
  Example:
    The files:
    MYLEVEL.RDL
    MYLEVEL.PG1 (or MYLEVEL.DTX or DEVIL.PG1 or DEVIL.DTX)
    MYLEVEL.HX1 (DEVIL.HX1)
    will all be added to the HOG file if they are pressent in the same
    directory.
  This will allow you to develope levels using D1X with custom textures and
  customize robots.


============== Devil 2.2LC1 =========================

Bugs fixed:
- Deleting cubes do not cause a crash later when the level is
  re-illuminated (D2).
- Unknown flags for sdoors (found in levels made with DMB2)
  do not cause an assertion anymore (D1).
- Deleting a wall with a lightsource do not cause a crash anymore (D2).
- Inserting a modified macro now works without generation a
  assertion (D1 & D2).

I have made a few changes too. These are optional and can be controlled with
 by changing the config\Devilx.ini file.

Things changed:
- F2 saves the level with full lightinfo (D2).
  F2FullSave=1 in Devilx.ini.
- MAX_LIGHT value is set to 199%. This allows DMB2 to read the
  level illuminated with Devil without problems (D2).
  MaxLightValue=199 in Devilx.ini.
- Deltalight values less than 4% isn't saved (D2).
  MinDeltaLight=4 in Devilx.ini.
- Alway smoothing when illuminating a mine (D1 & D2).
  AlwaysSmooth=1 in Devilx.ini
- Disabling of the 'Change cube' move mode. So that you only
  togles between 'move you' and 'move object' with <TAB>.
  ChangeCube=0 in Devilx.ini
- Devil now accepts a filename on the commandline.
- New title picture, rendered by Alan "Zonerie" Fork from the Descent Network
  Team.
- Now distributed as self-installing EXE, script written by Heiko "HH-Software
  Images" Herrmann from Descent Network Team, also looks if Descent Manager
  is installed, and if so, automatically uses its Descent directory
  information.

Lars Christensen
larsch@geocities.com
http://hjem.get2net.dk/dittehus/lac/
