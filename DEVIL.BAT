@echo off
rem Available values are
rem vesa  --  This is the mode which should work everywhere
rem et4000  --  Tseng ET4000 chipset
rem cl5426  --  Cirrus Logic chipset
rem mach64  --  ATI
rem stdvga  --  DEVIL will work but you can't fit textures
rem s3svga  --  This is accelerated. If you've a S3-card give it a chance.
rem I don't know if they all will work, I've only tested vesa and s3svga
set grx20drv=s3svga
rem This is for key moving
mode CON: Rate=32 Delay=1
copy >nul levels\tmpdevil.msn .. 
d %1 %2
if errorlevel 1 goto done
:loop
copy >nul levels\tmpdevil.rdl ..\tmpdevil.rdl 
cd ..
call descent
cd devil
d tmpdevil
if not errorlevel 1 goto loop
:done
if errorlevel 2 goto ok
echo Devil ended with an error. Here's the file devil.err:
type devil.err |more
:ok
if exist levels\tmpdevil.rdl del levels\tmpdevil.rdl
if exist ..\tmpdevil.rdl del ..\tmpdevil.rdl
del ..\tmpdevil.msn
rem This is that you can type in the normal way
mode CON: Rate=21 Delay=2
