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
echo Devil ended with an error. Here's the file devil.err:
type devil.err |more
if exist config\last.cfg del >nul config\last.cfg
:ok
if not exist %MISSIONPATH%\tmpdevil.msn goto no_d1_files
del %MISSIONPATH%\tmpdevil.msn >nul
del config\*.rdl >nul 
:no_d1_files 
if not exist %MISSIONPATH%\tmpdevil.mn2 goto no_d2_files
del %MISSIONPATH%\tmpdevil.mn2 >nul
del config\*.rl2 >nul
:no_d2_files
if exist %MISSIONPATH%\tmpdevil.hog del %MISSIONPATH%\tmpdevil.hog >nul
rem This is that you can type in the normal way
mode CON: Rate=21 Delay=2
