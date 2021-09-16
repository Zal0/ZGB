# ZGB

ZGB is a Game Boy / Game Boy Color engine that lets you write your own games in C or asm.

It uses [GBDK 2020](https://github.com/Zal0/gbdk-2020) but expands it to give you some common functionallity for writing games such as: a main loop, sprites spawning, sprites life cycle, sprites animations, collison management, easily getting assets into the game, music, fx...

![gif](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/tuto.gif) ![gif](https://github.com/Zal0/bitbitjam2016/blob/develop/bitbit3/res/marketing/screenshots/pretty.gif?raw=true) ![gif](https://github.com/Zal0/gbjam2016/raw/develop/res/marketing/gifs/fly.gif?raw=true)

## Getting started
- Download the latest [release](https://github.com/Zal0/ZGB/releases)

- Run install.bat (this will create a new environment var ZGB_PATH pointing to %ZGB%/common)

- Download the [ZGB-template](https://github.com/Zal0/ZGB-template/archive/master.zip) and build it running build.bat 

- Follow the tutorial on the [wiki](https://github.com/Zal0/ZGB/wiki) to understand the basic concepts of the engine

- (**Optional**) Download [***Microsoft Visual Studio Community Edition***](https://www.visualstudio.com/downloads/) 
The [ZGB-template](https://github.com/Zal0/ZGB-template) comes with a solution customized for visual studio

## Documentation
Check the [wiki](https://github.com/Zal0/ZGB/wiki)

## Support
twitter: [Zal0](https://twitter.com/Zal0)

discord: [gbdk/zgb discord](https://discord.gg/XCbjCvqnUY)

## Features <a name="features"></a>

 - [Easy makefile support](#easy-makefile-support)
 - [Transparent asset management](#transparent-asset-management)
 - [Main Loop](#main-loop)
 - [Big maps scroll support](#big-maps-scroll-support)
 - [Metasprites](#metasprites)
 - [Sprites modes 8x8 and 8x16](#sprites-modes-8x8-and-8x16)
 - [Sprite animations](#sprite-animations)
 - [Collisions sprite vs sprite and sprite vs background](#collisions)
 - Almost transparent, bank management, now using bankpack
 - Fonts
 - Music support using [gbt-player](https://github.com/AntonioND/gbt-player) or [hUGETracker](https://github.com/SuperDisk/hUGETracker)
 - Sound Effects 

### Easy makefile support

In most cases you just need a small makefile like this

```
PROJECT_NAME = ZGB_TEMPLATE

all: build_gb

N_BANKS = 4

# Music player: HUGETRACKER(default) or GBT_PLAYER
MUSIC_PLAYER = GBT_PLAYER

# Default hardware sprites size: SPRITES_8x16(default) or SPRITES_8x8
DEFAULT_SPRITES_SIZE = SPRITES_8x16

include $(ZGB_PATH)/src/MakefileCommon
```
When you make any changes to any of the source files of your project, or any of the assets, only that file will be recompiled. The internal Makefile that comes with ZGB creates a list of dependencies and only compiles what is needed saving you a lot of time. It will also help you a lot if you work with a version control system, like git

### Transparent asset management

ZGB will automatically turn all your assets files into C data:
- Graphics
  - .gbr from Game Boy Tile Designer
  - .gbm from Game Boy Map Builder
  - .png than can be used for backgrounds or sprites
- Music
  - .mod for gbt-player
  - .uge for hUGETracker

### Main Loop

![gif](/doc%20files/readme/ZGB-loop.png)

#### States
All ZGB games must contain at least one State. This state must be assigned on ZGBMain.c
```C
UINT8 next_state = StateGame;
```
When ZGB starts it will call the **START** function of this State. Then on each frame it will call the **UPDATE** function until **SetState** is called to assign a new State

To create a new State:
1. Create a new file < YourNewState >.c containing this:
```C
#include "Banks/SetAutoBank.h"

void START() {
}

void UPDATE() {
}
```
2. Register this new State in ZGBMain.h
```C
#define STATES             \
...
_STATE(<YourNewState>)       \
STATE_DEF_END
````
Now, whenever you want to enter this new state you just need to call **SetState**(< YourNewState >)

#### Sprites

You can manually add Sprites calling **SpriteMangerAdd**(type, x, y). ZGB will call the **START** function of this Sprite first and then it will call **UPDATE** on each frame until the Sprite is removed. You can manually remove an Sprite with the function **SpriteManagerRemove** (faster) or **SpriteManagerRemoveSprite** and then the engine will call its **DESTROY** function. 

Sprites will also be removed when getting off screen limits. You can configure how far you allow them to go before the engine disposes them with the fields **lim_x** and **lim_y**

Usually you will create an Sprite in the START function of your State and assing it to scroll_target, so that the camera follows it
```C
void START() {
	scroll_target = SpriteManagerAdd(SpritePlayer, 50, 50);
	...
}
```

You can create your sprites with the Game Boy Tile Designer or you can use pngs. 
Create your sprites in the res/sprites folder so that the Makefile can identify them as sprites and pass the proper parameters to png2asset.

##### gbr sprites
Use Game Boy Tile Designer included in <ZGB_PATH>/env/tools/gbtd22/GBTD.exe to create gbr sprites. Don't worry too much about the TileSize choosen, remember that empty tiles will be discarded

The first time you compile the gbr a .meta will be created with the default params passed to png2asset. You may want to take a look specially at the collision info to adjust the collider
```
-px 0 -py 0 -pw 32 -ph 32
```
Check the png2asset params [here](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_toolchain_settings.html#png2mtspr-settings)

##### png sprites
GBTD has a few limitations:
- The maximum sprites size is 32x32
- It only lets you choose a palette for the whole metasprite

Luckily you can overcome these limitations by using your preferred pixel art software and then export your data as a spritesheet

It is important that you create a .meta and at least indicate the sprite dimensions (or it will be exported as a single sprite)
```
-sw 32 -sh 16 
```


The template already comes with a placeholder Sprite but you surely will need to add more. Yo do this by following the next 3 steps:
1. Create the sprite image data. 
2. Create a new file < YourNewSprite >.c containing this:
```C
#include "Banks/SetAutoBank.h"

void START() {
}

void UPDATE() {
}

void DESTROY() {
}
```
3. Register this new Sprite in ZGBMain.h
```C
#define SPRITES \
...
_SPRITE_DMG(<YourNewSprite>, <image>)\
SPRITE_DEF_END
```

### Big maps scroll support

ZGB support maps up to 16384 bytes with a maximum width or height of 255 tiles. The engine will take care of updating the native 32x32 background as the camera moves

![gif](/doc%20files/readme/scroll.gif)

Here is how you create a new map and load it into your game:
- Open Game Boy Tile Designer included in <ZGB_PATH>/env/tools/gbtd22 and create a new tile set
- Open Game Boy Map Builder included in <ZGB_PATH>/env/tools/gbmb18 and create a new map that uses the previous tile set
- In the START function of your state you should manually add the sprite that the camera will follow
- Import your map and Call InitScroll
```C
IMPORT_MAP(map); //This is the name of your map without the extension

void START() {
	scroll_target = SpriteManagerAdd(SpritePlayer, 50, 50);
	InitScroll(BANK(map), &map, 0, 0);
}
```
As the scroll updates new rows or columns it will call the function GetTileReplacement located in ZGBMain.c
The default behaviour of this function is to spawn sprites using sprite_tye = 255 - tile_id, but you can customize it for your custom needs

### Metasprites

Metasprites are sprites composed of 8x8 or 8x16 native sprites. 
The tool png2asset from GBDK is used to create the data that will end up in the final build:
- duplicated tiles will be only added once
- mirrored tiles will also count as duplicated
- empty tiles will be ignored
- palette info will be included

### Sprites modes 8x8 and 8x16

The Game Boy has native support for sprite sizes 8x8 and 8x16. You can use any of them to compose the metasprites in your game.

Choosing 8x8 size will make it easier to duplicate parts of the metasprite and will require less memory to store it but will take longer to render the final metasprite

The default Sprite mode is selected in your makefile
```
# Default hardware sprites size: SPRITES_8x16(default) or SPRITES_8x8
DEFAULT_SPRITES_SIZE = SPRITES_8x16
```

### Sprite animations

Animations in ZGB are defined by arrays of frames where the first element is the number of frames
```C
const UINT8 anim_walk[] = {4, 0, 1, 2, 1, 0};
```

Setting the current animation is done with **SetSpriteAnim**(sprite, animation, speed)

Instead of setting an animation you can Set the current frame manually by calling **SetFrame**(Sprite, frame_idx)

The Sprite field **anim_frame** contains the animation index if there is an animation running, or the frame index otherwise

### Collisions

All sprites have a rectangle collider that will be used to check collisions. By default it will be defined by metasprites width and height but you can adjust it on the sprite .meta file
```
-px 2 -py 0 -pw 12 -ph 19
```
![gif](/doc%20files/readme/collider.png)  ![gif](/doc%20files/readme/mirrors.gif)

This rectangle will remain constant when the sprite is flipped

#### Sprite vs Background

#### Sprite vs Sprite

## License

Released under the [MIT license](https://opensource.org/licenses/MIT)