@echo off
if exist config\cfg.bat call config\cfg.bat
rem This is for fast key moving
mode CON: Rate=28 Delay=1
config\d %1 %2 %3
if errorlevel 1 goto done
:loop
%DESCENTDRIVE%:
cd %DESCENTPATH%
call %DESCENTSTART%
%DEVILDRIVE%:
cd %DEVILPATH%
config\d /NOTITLE
if not errorlevel 1 goto loop
:done
if not errorlevel 2 goto ok
set devilerror=1
if not exist devil.err goto nodeverr
echo Devil ended with an error. Here's the file devil.err:
type devil.err |more
goto errclean
:nodeverr
echo If you can't start Devil, check your choice for the grfx card.
echo To do that, start Devil with "devil /config".
:errclean
if exist config\last.cfg del >nul config\last.cfg
:ok
if not exist %MISSIONPATH%\tmpdevil.msn goto no_d1_files
del %MISSIONPATH%\tmpdevil.msn >nul
if %devilerror%==1 goto dontkilllevel
if exist config\*.rdl del config\*.rdl >nul
:no_d1_files 
if not exist %MISSIONPATH%\tmpdevil.mn2 goto no_d2_files
del %MISSIONPATH%\tmpdevil.mn2 >nul
if %devilerror%==1 goto dontkilllevel
if exist config\*.rl2 del config\*.rl2 >nul
:no_d2_files
:dontkilllevel
if exist %MISSIONPATH%\tmpdevil.hog del %MISSIONPATH%\tmpdevil.hog >nul
set devilerror=
rem This is that you can type in the normal way
mode CON: Rate=21 Delay=2
