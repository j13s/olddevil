@echo off
rem Available values are
rem vesa  --  This is the mode which should work everywhere
rem et4000  --  Tseng ET4000 chipset
rem cl5426  --  Cirrus Logic chipset
rem mach64  --  ATI
rem s3vesa  --  This is accelerated. If you've a S3-card give it a chance.
rem I don't know if they all will work, I've only tested vesa and s3vesa
set grx20drv=vesa
rem This is for fast key moving
mode CON: Rate=32 Delay=1
copy >nul config\tmpdevil.msn .. 
config\d %1 %2
if errorlevel 1 goto done
:loop
copy >nul config\tmpdevil.rdl ..\tmpdevil.rdl 
cd ..
call descent
cd devil20h
config\d /NOTITLE
if not errorlevel 1 goto loop
:done
if not errorlevel 2 goto ok
echo Devil ended with an error. Here's the file devil.err:
type devil.err |more
:ok
if exist config\tmpdevil.rdl del >nul config\*.rdl
if exist ..\tmpdevil.rdl del ..\tmpdevil.rdl 
if exist ..\tmpdevil.msn del ..\tmpdevil.msn >NUL
rem This is that you can type in the normal way
mode CON: Rate=21 Delay=2
