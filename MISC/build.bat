@echo off
rem --------------------------------------
rem --AUTOMATIC COMPILLER FOR ZGB ENGINE--
rem --------------------------------------

rem		CHANGE OPTIONS (yes/no)
rem		-----------------------
rem		COLOR: enables game boy color features
rem		CPlayer: enables Carillon Player for music

set "COLOR=yes"
set "CPlayer=yes"


rem ///////////////////////////////////
rem ///////////////////////////////////
rem ///////////////////////////////////

@echo start
echo COLOR MODE
echo %COLOR%
echo CARILLON PLAYER MODE
echo %CPLAYER%


C:\gbdk\bin\make-3.81-bin\bin\make -C %ZGB_PATH%\src CGB=%COLOR% CP=%CPLAYER%
cd src
C:\gbdk\bin\make-3.81-bin\bin\make run CGB=%COLOR% CP=%CPLAYER%
cd ..
pause