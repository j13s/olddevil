Hi.

This is a short remark to well experienced Devil V2.0? + V1.02 users.
For further documentation look at the file readme.txt and the file
docs/userman.doc.

This is the full version of Devil V2.0h (V2.0g lived only one day).
This will be the final V2.00 release when all new/old bugs are removed.
 
Fixed problems/changes (V2.0f->V2.0h):
-The Mission Manager (finally replaced extract.exe/hogfile.exe):
  Well, this is some sort of HOG/MSN-file-generator. It should be quite
  clear how to use it.
  Two remarks: 
   -The level is always inserted before the first marked level in the 
    hog-file, if no level is marked, the level is inserted before the first
    secret level or at the end.
   -To insert a secret level, type the number of the level which has the
    alternate exit in the field 'Secret' and press return.
  I will modify it in the next version, so you can extract also the
  other files from the DESCENT.HOG file (in the moment you can only
  extract levels).
-Smoothing light works now like it should be. Fixed a few things
 with mine illuminating.
-Fixed problems with macro inserting and 'not find cubes' (wondering
 why this was doing anything of worth).
-The problem with texture2 and the huge number when you save the level.
-The statistics includes now the things dropped by robots (esp. the keys).
-Doors on a side of a cube which is deleted are now deleted automatically.
-Still must choose a texture that is one location above or below to insert
 onto multiple sides->Fixed.
-Deleting an cube inserted in the Side mode is now possible (->the cube
 is correct inserted)
-You can know insert a first wall/thing in a level by pressing 'Insert' in
 the window.
-Right click in a level window without a current level won't crash Devil
 anymore.
-Hopefully fixed the problems with the names of the macros. 
-Illuminating feature has now one more parameter: The minimal value for
 the light for one corner. If you want to have some sort of 'overall'
 brightness in your level, you can increase this value.
-It is possible to calculate inner cube and corner light seperate.
-You can now change the coordinates of several tagged points at once.
-When you change the mode via keyboard the window of the old mode is closed.
-A new menupoint for moving the current thing in the current cube exists.
-You can change the path while loading/saving.
-Fixed the thing with the right mouse button in the 'Move Buttons'-Window.
 
A BIG thanks to all guys at the mailing list and all others for bug reporting
and making suggestions.

This is still a prerelease, so be careful and make backups of your levels. 

If you have any questions, please mail them NOT to me directly but to the 
Devil Mailing List (sclouse@aol.com with subject starting with "DEVIL:") -
thanks to Steven P. Clouse. I am subscribed in this mailing list, too, so I
will receive everything you send to it (and a lot of other guys will receive
it also and can answer your questions).

The hotkeys:
 shift+esc: fast quit
 esc: quit
 tab: change movemode
 ctrl+c: cubemode
 ctrl+s: sidemode
 ctrl+p: pointmode
 ctrl+t: thingmode
 ctrl+w: wallmode
 F2: fast save
 M: set connected side/exit
 m: make macro
 g: grid on/off
 shift+del: delete tagged
 del: delete current
 backspace: delete special
 shift+ins: insert fast
 ins: insert
 5: beam to current object
 7: goto wall
 9: goto side cube
 L: previous level
 l: next level
 C: previous cube
 c: next cube
 S: previous side
 s: next side
 P: previous point
 p: next point
 T: previous thing
 t: next thing
 W: previous wall
 w: next wall
 ctrl+space: tag special
 shift+space: tag all
 space: tag
 shift+*: increase rotation angle
 shift+/: decrease rotation angle
 *: increase movefactor
 /: decrease movefactor
 A: increase visibility
 Y: decrease visibility
 Z: decrease visibility
 +: enlarge object
 -: shrink object
 y: move backward
 z: move backward
 N3: move backward
 N2: move down
 a: move forward
 N1: move forward
 N4: move left
 N8: move up
 N6: move right
 1: bank left
 8: turn down
 3: bank right 
 4: turn left
 2: turn up
 6: turn right

Have a good time,
Achim

PS: Next major new feature: Texture mapping (on one side) but this will take
 a while.
PSPS.: Searching someone who can translate a German manual in a English
 manual.

