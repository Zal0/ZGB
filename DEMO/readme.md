This is just a ZGB Engine demo for GBDK using all ZGB features.

I used maps from Secret Agent and Rick Dangerous, But this is never going to be a port of these games, it is just a test.

video https://www.youtube.com/watch?v=GWu-MOOYdj4Super Princess' 2092 Exodus will compile with this.

But it won't work well because I had to modify "ScrollSetMap" function.

The make.bat file has two settings to compile games: COLOR and CPLAYER.

COLOR = yes: enable game boy color features, ROM will only work well on GBC.

COLOR = no: disable game boy color features, ROM will only work well on GB.

CPlayer = yes: enable Carillon Player for music, gbt player is disabled. Only .sav files will be used.

CPlayer = no: enable GBT player for music, disable carillon player. Only .mod files will be used

