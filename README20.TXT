Hi.

This is the full version of Devil V2.0c. 
 
New features (V2.0b->V2.0c):
-Play level feature & Config-File
-Reading old Devil V1.02 macros.
-More than one macro is possible and you can modify&view macros.

This is a prerelease. It has all functions from V1.02 and some minor
extensions.

This is a non-tested (well, nearly) version at all. Also there is no new
documentation material except for this file and I will only explain the new
things but I think every experienced Devil V1.02 user shouldn't have great
problems. If you have any questions, please mail them NOT to me directly but
to the new Devil Mailing List (DevilML@aol.com) - thanks to Steven P. Clouse.
I am subscribed in this mailing list, too, so I will receive everything
you send to it (and a lot of other guys will receive it also and can
answer your questions).

The basics of V2.0:

1. This thing is window-driven. Each window has two buttons in its headline:
a small one left (closebutton) and a large one right (dragbutton) with the
following functions which don't work all on every window but you'll see:
closebutton, left mouse button: close this window
closebutton, right mouse button: enlarge/shrink this window
dragbutton, left mouse button: move window
dragbutton, right mouse button: resize window
You can get a window to front with clicking on it.

2. The option windows replace the text on the right upper side in Devil V1.02.
If you press the left mousebutton you change the current object, if you
press the right mousebutton you change the tagged objects (without
the current object). In the textbuttons you can delete the contents of
this button with Shift+Del, the other keys work as you would expect (I hope).
The bitmap in the Side window is also a button. It activates the Fit Bitmap
function.

3. The Fit Bitmap functions: Try the movekeys with movemode 'texture', you 
will see what happens. Try also left and right mouseclick in the 
Fit Bitmap window.

4. The current level is shown in the upper right corner. You always move
yourself or an object in the current level. You can change the current level
with clicking on it or by pressing L or l. You select the current
object with click on the left button and you can tag several things with the
right button and dragging the tag box.

5. You change the current button (marked with a red edge) with the cursor
up and cursor down keys. Pressing return=left mouse button, pressing
shift+return=right mouse button. You can change the window with
'ctrl+the first letter of the title of the window'. You can use the
alt-key to gain access to the menu via the keyboard.

6. The macros: The new macros are used the same way as the macros in
the previous Devil releases. The new thing is that you can modify&view
the macros like levels and that you can change the side which will be
connected in the macro. To do so, press 'M' or select in the menu 'edit'
'set connect side/exit'. The side will be marked with three red lines in
the middle and a fourth line at the point which will be layed on the
current point when you insert the macro (just try it). This connect side
is identical with the former 'exit side', so if you use a level as
a macro the exit side is the connect side and if you use a macro as a level
the connect side is the exit side.

7. The hotkeys:
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
 F3: make room
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

Have fun,
Achim

PS.: Next release:
 - automatic mine illuminating
 - save camera positions
 - perhaps more than one window per level (to have i.e. two perspectives)
