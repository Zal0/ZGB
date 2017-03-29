@echo off
rem --------------------------------------
rem --AUTOMATIC COMPILLER FOR ZGB ENGINE--
rem --------------------------------------

rem		CHANGE OPTIONS (yes/no)
rem		-----------------------
rem		COLOR: 
rem 		yes = enable game boy color features, ROM will only work well on GBC
rem 		no = disable game boy color features, ROM will only work well on GB
rem		CPlayer: 
rem 		yes = enable Carillon Player for music, gbt player is disabled
rem			Only .sav files will be used
rem 		no = enable GBT player for music, disable carillon player
rem			Only .mod files will be used

set "COLOR=no"
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