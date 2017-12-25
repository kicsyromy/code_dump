@ECHO OFF

REM ---------------------------------------------------------------------
REM TYGAME.BAT
REM Installation program for "Teach Yourself Game Programming in 21 Days"
REM ---------------------------------------------------------------------

REM --------------------------------------------------
REM Note: You must run INSTALL.BAT to properly execute
REM this batch file.
REM --------------------------------------------------

IF .%1 == . GOTO RUN_INSTALL
IF NOT EXIST %1\NUL GOTO NO_DRIVE

IF %2 == SOURCE GOTO SOURCE_INSTALL
IF %2 == BMASTER GOTO BMASTER_INSTALL
IF %2 == NEOPAINT GOTO NEOPAINT_INSTALL

CLS
ECHO ÿ
ECHO ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
ECHO บ "Teach Yourself Game Programming in 21 Days" Disc Installation บ
ECHO ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ECHO ÿ
ECHO   You have chosen to install the %4 %5 %6 
ECHO   software to your %1 hard drive. The files will take
ECHO   approximately %3 MB of disk space.
ECHO  ÿ
ECHO  ÿ
ECHO   If this is NOT what you want to do, press the Ctrl+C
ECHO   keys to end this batch program.
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
CLS

XCOPY GAMES\%2\*.* /s %1\%2\

CLS
ECHO 
ECHO ÿ
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO   The installation of %4 %5 %6 is complete!
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
ECHO   You'll find the files in the %1\%2 directory of your 
ECHO   hard drive. 
ECHO ÿ
ECHO   You'll now be returned to the install menu.                            
ECHO   Press Esc to exit the menu and return to DOS.  
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
GOTO FINISHED
REM ************************************************************************

:SOURCE_INSTALL
CLS
ECHO ÿ
ECHO ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
ECHO บ "Teach Yourself Game Programming in 21 Days" Disc Installation บ
ECHO ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ECHO ÿ
ECHO   You have chosen to install the book's source code  
ECHO   and utilities to your %1 hard drive. The files will take 
ECHO   approximately %3 MB of disk space.
ECHO  ÿ
ECHO  ÿ
ECHO   If this is NOT what you want to do, press the Ctrl+C
ECHO   keys to end this batch program.
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
CLS

XCOPY \SOURCE\*.* /s %1\TYGAME\SOURCE\

CLS
ECHO 
ECHO ÿ
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO   The installation of the source files is complete!
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
ECHO   You'll find the files in the %1\TYGAME\SOURCE directory of your hard
ECHO   drive, arranged by day. For example, the source code for example, 
ECHO   Day 16 is in the \TYGAME\SOURCE\DAY_16 subdirectory.
ECHO ÿ
ECHO   You'll now be returned to the install menu.                            
ECHO   Press Esc to exit the menu and return to DOS.  
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
GOTO FINISHED
REM ************************************************************************

:BMASTER_INSTALL
CLS
ECHO ÿ
ECHO ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
ECHO บ "Teach Yourself Game Programming in 21 Days" Disc Installation บ
ECHO ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ECHO ÿ
ECHO   You have chosen to install BLASTER Master 6.2 to your %1 hard 
ECHO   drive. The files will take approximately 600 KB of disk space.
ECHO  ÿ
ECHO  ÿ
ECHO   If this is NOT what you want to do, press the Ctrl+C
ECHO   keys to end this batch program.
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
CLS

XCOPY \BMASTER\*.* /s %1\BMASTER\

CLS
ECHO 
ECHO ÿ
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO   The installation of BLASTER Master is complete!
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
ECHO   You'll find the files in the %1\BMASTER directory of your drive.
ECHO ÿ
ECHO   You'll now be returned to the install menu.                            
ECHO   Press Esc to exit the menu and return to DOS.  
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
GOTO FINISHED
REM ************************************************************************

:NEOPAINT_INSTALL
CLS
ECHO ÿ
ECHO ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
ECHO บ "Teach Yourself Game Programming in 21 Days" Disc Installation บ
ECHO ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ECHO ÿ
ECHO   You have chosen to install NeoPaint to your %1 hard 
ECHO   drive. The files will take approximately 1 MB of disk space.
ECHO  ÿ
ECHO  ÿ
ECHO   If this is NOT what you want to do, press the Ctrl+C
ECHO   keys to end this batch program.
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
CLS

XCOPY \NEOPAINT\*.* /s %1\NPAINT\

CLS
ECHO 
ECHO ÿ
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO   The installation of NeoPaint is complete!
ECHO ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
ECHO ÿ Remember to change to %1\NPAINT and run INSTALL.EXE to install
ECHO ÿ NeoPaint.
ECHO ÿ
ECHO   You'll now be returned to the install menu.                            
ECHO   Press Esc to exit the menu and return to DOS.  
ECHO ÿ
ECHO ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
ECHO ÿ
PAUSE 
GOTO FINISHED
REM ************************************************************************

:NO_DRIVE
REM ************************************************************************
CLS
ECHO 
ECHO ÿ
ECHO ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ 
ECHO ณ Drive %1 is not available. Please select an available drive. ณ
ECHO ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
ECHO ÿ
PAUSE
GOTO FINISHED
REM ************************************************************************

REM ************************************************************************
CLS
ECHO 
ECHO ÿ
ECHO ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ 
ECHO ณ You must run the INSTALL.BAT program to properly install   ณ
ECHO ณ the "Teach Yourself Game Programming in 21 Days" software. ณ
ECHO ณ From the DOS prompt type INSTALL, press Enter, and the     ณ
ECHO ณ main installation menu will appear.                        ณ
ECHO ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
ECHO ÿ
PAUSE
REM ************************************************************************

:FINISHED
