

ZGB
===

A little engine for creating games for the original Game Boy.

Features:
---------

 - Coding in C using [gbdk](http://gbdk.sourceforge.net/)
 - Based on States and Sprites
 - 4-directional scroll 
 - Support for maps bigger than 32x32
 - Transparent sprites pool allowing 20 sprites 8x16 or 16x16
 - Sprite animations
 - Collisions sprite vs sprite and sprite vs background and configurable rectangle collider
 - Easy makefile support (just 3 lines for most projects)
 - Bank management
 - Fonts
 - Music support using [gbt-player](https://github.com/AntonioND/gbt-player)

Installation
------------
Download and install:

 - gbdk ([2.95-3](https://sourceforge.net/projects/gbdk/files/gbdk-win32/2.95-3/) at the moment of writing this) on c:\gbdk
 - [msys](https://sourceforge.net/projects/mingw-w64/files/External%20binary%20packages%20%28Win64%20hosted%29/MSYS%20%2832-bit%29/MSYS-20111123.zip/download) anywhere but add folder msys/bin to your Path
 - [Make for Windows](http://gnuwin32.sourceforge.net/packages/make.htm)* on C:\gbdk\bin\make-3.81-bin
 - [BGB](http://bgb.bircd.org/) on c:\gbdk\bin
 - [mod2gbt](https://github.com/AntonioND/gbt-player/raw/master/legacy_gbdk/mod2gbt/mod2gbt.exe) on c:\gbdk\bin
 - [Game Boy Map Builder](http://www.devrs.com/gb/hmgd/gbmb.html) on c:\gbdk\tools\gbmb18
 - [Game Boy Tile Designer](http://www.devrs.com/gb/hmgd/gbtd.html) on c:\gbdk\tools\gbtd22
 - This repo, anywhere but create an env var called ZGB_PATH pointing the common folder eg: c:\ZGB\common
 - [Microsoft Visual Studio Community Edition](https://www.visualstudio.com/downloads/) (optional)

*The make that comes with devkitpro doesn't print any error messages and that's why you need to download make for windows too.

States and Sprites
------------------
Before you start coding anything it is important that you understand what States and Sprites are for ZGB

 * **States**: you can think on States as the current main loop of your program. There can only be one of them running at a time and everytime you change from one to another all resources are discarded (they are similar to Scenes in Unity3d). Every declared State must contain 2 functions:
	 - ***Start***: that handles the initialization of the State. Here is where you should load all the resources you are going to need and perform all variables initialization
	 - ***Uptate***: called one per frame, for global interactions
 * **Sprites**: these are the objects that you see moving on screen (like GameObjects in Unity). Right now only 8x16 and 16x16 sprites are supported and  no matter the size you cannot have more than 20 sprites at a time or the rom will crash. Every declared Sprite must contain 3 functions:
	 * ***Start***: that handles the initialiation of the Sprite. It is called inmediatelly after the Sprite is created
	 * ***Update***: called one per frame
	 * ***Destroy***: called before deleting and Sprite

 every Sprite has a [struct](https://github.com/Zal0/ZGB/blob/master/common/include/Sprite.h) associated with him that contains its current data. This struct can be accesed using the THIS variable inside any of its functions and contains the next variables:
  * *anim_data*: a pointer to an UINT8 array containing a list of frames for the current animation. The first element must be the number of frames
  * *x, y*: current world position of the Sprite
  * *flags*: right now only used for sprite mirroring, setting it to OAM_VERTICAL_FLAG will mirror the Sprite and setting it to 0 will leave it as it is
  * *lim_x, lim_y*: by default sprites are destroyed when going offscreen. Using these vars you can control how far can they go without being destroyed
  * *custom_data*: 8 bytes for storing custom information
 
 Sprites are managed by the **SpriteManager** which is responsible for the update of every one of them (you don't need to worry about that, it happens automatically). Some useful functions of the SpriteManager are:
	 - SpriteManagerAdd: for adding new Sprites
	 - SpriteManagerRemove: for removing them
	 - SPRITEMANAGER_ITERATE: a useful macro for iterating through all active sprites

Resource guidelines
-------------------
Some guidelines you must follow to make your life easier when developing for Game Boy using ZGB:

- Tiles:
	- Place them in the res/ folder of the root directory of your project
	- In the Export To... set te filename as src/[tile name].[bank].c
	- When making changes besides saving your file remember to Export it 
- Maps:
	- Place them in the res/ folder of the root directory of your project
	- When setting a map (File->Map Properties) ensure the Filename contains no folders on the path (this will make your life easier when working with more people)
	- In the Export To... set te filename as src/[map name].[bank].c
	- In the Export To... ensure you have the Location Format like this
	![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/4-Location%20Format.PNG)
	- When making changes besides saving your file remember to Export it 
- Music:
	- Place your music mod files in the res/music folder
	- The makefile will handle the converion automatically so you don't need to worry about it
	- For more info about music formats check the [gbt-player](https://github.com/AntonioND/gbt-player) docs

Banks
-----
It is very important that you understand the concept of banks. ZGB together with GBDK is taking most of the space in bank0 and gbt-player is using most of the space in bank1. That means you will have to put your code on bank 2 and further, and that you will have to use banks, whether you like it or not. Check the point 3 of my post [Game Boy Development Tips and Tricks (I)](http://zalods.blogspot.com.es/2016/07/game-boy-development-tips-and-tricks-i.html) for more info.

The good news is that you don't have to worry much about bank swapping since ZGB will take care of it in most situations.

Bank management is managed in ZGB in 2 different ways
- **Setting the bank on the filename**: if you name your file [filename].[bank].c that will be compiled into the [bank] specified. All your maps and tiles should use this convention (eg: player.b3.c will be compiled into bank 3)
- **Setting the bank using #pragma bank=[bank]**: this is the standard way gbdk handles banks. You should use this in all your code files. Also for States and Sprites you need to declare an extra var so the engine will know where it has been placed (And it will swap banks before calling start, update or destroy automatically). The first 3 lines of your States and Sprites should look like:
```
#pragma bank=2
#include "StateGame.h"
UINT8 bank_STATE_GAME = 2;
```
 having both declarations together will make things easier when moving parts of your code from one bank to another. 

As a suggestion start placing all your code files into bank 2 and all your resources into bank 3. When you run out of space you can increase the number of banks in the Makefile of your project (also remember the number of banks must be a power of 4: 4, 8, 16, 32...)


Tutorial
--------
Here is a quick tutorial of how to make a simple game

Creating a New Project
----------------------
Download [this template](https://github.com/Zal0/ZGB-template/archive/master.zip) for starting a new project. 

Try to build it by launching build.bat or loading the project into Visual Studio. If you have problems compiling check the installation list again. 

If everything goes ok BGB will be automatically launched after the build and you should see something like this
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/bgb00006.bmp)

The template contains the minimum files required for making a build:

 - ZGBMain: The Main file for your project: all states an sprites declarations must be done in this file
 - SpritePlayer: this is the code of the Game Boy Sprite that you can see on the screen. It does nothing, but has all the required methods
 - StateGame: the current State that makes a basic initialization:
	 - Loads all sprites graphics into memory
	 - Creates an sprite of type SPRITE_PLAYER and sets it as the scroll camera
	 - Loads all tile graphics into memory
	 - Initializes the scroll (this will center the camera on the sprite set as camera and fill the background)

Input
----------------------
Let's make out sprite to move a little bit:

 - Open SpritePlayer.c and include Keys.h and SpriteManager.h
 - The KEY_PRESSED macro tells us if a key is pressed or not. Write the next code in the Update function:
```
if(KEY_PRESSED(J_UP)) {
	THIS->y --;
}
```
 - Compile and you should see your sprite moving upwards when pressing the up button
 - Now, to do the full movement, this is how your SpritePlayer.c should look:
```
#pragma bank=2
#include "SpritePlayer.h"
UINT8 bank_SPRITE_PLAYER = 2;

#include "Keys.h"
#include "SpriteManager.h"

void Start_SPRITE_PLAYER() {
}

void Update_SPRITE_PLAYER() {
	if(KEY_PRESSED(J_UP)) {
		THIS->y --;
	} 
	if(KEY_PRESSED(J_DOWN)) {
		THIS->y ++;
	}
	if(KEY_PRESSED(J_LEFT)) {
		THIS->x --;
	}
	if(KEY_PRESSED(J_RIGHT)) {
		THIS->x ++;
	}
}

void Destroy_SPRITE_PLAYER() {
}
```

Animations
----------
Let's make our Sprite do some animation when walking

 - Open the file player.bgr with Game Boy Tile Designer (you should associate *.gbr to be always opened whit this program if you haven't done it yet)
 - Let's change the aspect of the Sprite by a boy and add a couple of frames for the walking animation like this ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/2-anim.PNG)
 - Now go to File->Export To... and Update the number of frames (From 0 to 2) and after that Save your file
 - If you Build your the project now you should see the new Sprite instead of the Game Boy
 - In ZGBMain.c we need to update the number of frames of our sprite, the last parameter of INIT_SPRITE does exactly that: 
```
INIT_SPRITE(SPRITE_PLAYER, player, 3, FRAME_16x16, 3);
```
 - Animations are declared as arrays of UINT8, the first value is the number of frames. Declare the next arrays on SpritePlayer.c
```
const UINT8 anim_idle[] = {1, 0};
const UINT8 anim_walk[] = {2, 1, 2};
```
 - And now we can animate our character like this:
```
void Update_SPRITE_PLAYER() {
	if(KEY_PRESSED(J_UP)) {
		THIS->y --;
		SetSpriteAnim(THIS, anim_walk, 15);
	} 
	if(KEY_PRESSED(J_DOWN)) {
		THIS->y ++;
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(KEY_PRESSED(J_LEFT)) {
		THIS->x --;
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(KEY_PRESSED(J_RIGHT)) {
		THIS->x ++;
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(keys == 0) {
		SetSpriteAnim(THIS, anim_idle, 15);
	}
}
```

Collisions
----------
Let's focus for now on the collisions with the background. Right now if you move the player you'll see it can go throught walls (also going offscreen will kill the sprite so we need to avoid this)

* Collisionable tiles are declared as an array of UINT8 ended with 0. So far we only have two tiles (the first one is empty and we don't want that to be collidable). Declare the next array on StateGame.c
```
UINT8 collision_tiles[] = {1, 0};
```
* This list is the 4th paramater passed to the InitScroll function. Right now is set as 0 (null), so let's change it into this
```
InitScroll(mapWidth, mapHeight, map, collision_tiles, 0, 3);
```
* Now if we want the scroll to manage collisions with our sprite we need to used the function TranslateSprite instead of directly updating the x and y coordinates of our sprite. Here is how our Update method should look like now:
```
void Update_SPRITE_PLAYER() {
	if(KEY_PRESSED(J_UP)) {
		TranslateSprite(THIS, 0, -1);
		SetSpriteAnim(THIS, anim_walk, 15);
	} 
	if(KEY_PRESSED(J_DOWN)) {
		TranslateSprite(THIS, 0, 1);
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(KEY_PRESSED(J_LEFT)) {
		TranslateSprite(THIS, -1, 0);
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(KEY_PRESSED(J_RIGHT)) {
		TranslateSprite(THIS, 1, 0);
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(keys == 0) {
		SetSpriteAnim(THIS, anim_idle, 15);
	}
}
```
* There is still a small issue we can fix. Unless you did a square your Sprite might be stopping before touching the collidable tile
 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/3-bad%20collision.PNG) 
we can fix this adjusting the collider of the Sprite on the Start method. The collider of an Sprite is defined by an rectangle declared using coll_y, coll_x, coll_w and coll_h These values worked for me:
```
void Start_SPRITE_PLAYER() {
	THIS->coll_x = 2;
	THIS->coll_y = 0;
	THIS->coll_w = 12;
	THIS->coll_h = 16;
}
```

Enemies
-------

Let's add a little bit of action to our game by adding an enemy.

* Open the Game Boy Tile designer and create a new 16x16 Sprite for our enemy (View->Tile Size->16x16). This is what I came up with:
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/5-enemy.PNG)
Set Export settings (File->Export To...) like:
	*	FileName: src/enemy.b3.c
	*	Type: GBDK file
	*	Label: enemy
	*	From 0 to 1

 And Save the file into de res folder

* Create a new file SpriteEnemy.h in the include folder with the next content:
```
#ifndef SPRITE_ENEMY_H
#define SPRITE_ENEMY_H

#include "main.h"

DECLARE_SPRITE(SPRITE_ENEMY);

#endif
```
 the macro DECLARE_SPRITE is just a shortcut for including the declaration of Start, Update and Destroy followed by the type of the Sprite (in this case Start_SPRITE_ENEMY, Update_SPRITE_ENEMY and Destroy_SPRITE_ENEMY)

* Create another file SpriteEnemy.c inside de src folder with the next content: 
```
#pragma bank=2
#include "SpriteEnemy.h"
UINT8 bank_SPRITE_ENEMY = 2;

void Start_SPRITE_ENEMY() {
}

void Update_SPRITE_ENEMY() {
}

void Destroy_SPRITE_ENEMY() {
}
```
* In ZGB_Main.h add the new type for the Sprite
```
typedef enum {
	SPRITE_PLAYER,
	SPRITE_ENEMY,

	N_SPRITE_TYPES
} SPRITE_TYPE;
```
* In ZGB_Main.c
	* Include "SpriteEnemy.h" and "../res/src/enemy.h"
	* Add its initialization, we stored it on bank 3, saved as 16x16 and created only 1 frame  so it should looklike this
```
void InitSprites() {
	...
	INIT_SPRITE(SPRITE_ENEMY,  enemy,  3, FRAME_16x16, 1);
}
```
* In StateGame.c let's create an instance in 70, 50
```
void Start_STATE_GAME() {
	...
	SpriteManagerAdd(SPRITE_ENEMY, 70, 50);
}
```
  * If you build the game now you'll see our player and the new enemy. It does nothing and we don't have any collisions with it yet
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/bgb00007.bmp)
* let's add some movement to our enemy, something like this:
```
#pragma bank=2
#include "SpriteEnemy.h"
UINT8 bank_SPRITE_ENEMY = 2;

#include "SpriteManager.h"

struct EnemyInfo {
	INT8 vy;
};

void Start_SPRITE_ENEMY() {
	struct EnemyInfo* data = THIS->custom_data;
	data->vy = 1;
}

void Update_SPRITE_ENEMY() {
	struct EnemyInfo* data = THIS->custom_data;
	if(TranslateSprite(THIS, 0, data->vy)) {
		data->vy = -data->vy;
	}
}

void Destroy_SPRITE_ENEMY() {
}
```
Here you can see how the field custom_data on the Sprites is mean to be used. Also TranslateSprite will return the index of the tile when a collision happens or 0 otherwise. In this case if there is a collision with the background the velocity of the sprite is negated making it changing its direction

- In order to collide with the enemy we have to add the next code on the method Update of SpritePlayer.c
```
#include "ZGBMain.h"
...

void Update_SPRITE_PLAYER() {
	UINT8 i;
	struct Sprite* spr;
	...

	SPRITEMANAGER_ITERATE(i, spr) {
		if(spr->type == SPRITE_ENEMY) {
			if(CheckCollision(THIS, spr)) {
				SetState(STATE_GAME);
			}
		}
	}
}
```
"ZGBMain.h" needs to be included in order to get access to all the sprite types. SPRITEMANAGER_ITERATE is a macro that iterates through all the active sprites, if the sprite is of type SPRITE_ENEMY then we use the function CheckCollision to check if we are colliding with it (do it in this order). If so we call SetState that unloads the current state and loads the state passed as paramenter (even if it is the current one, like in this case)

Maps
----
Let's now focus on the creation of our game level. Open the file res/map.bgm with the Game Boy Map Builder and create your stage. This is how mine looks like
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/6-map.PNG)
Export and Save and make a build and it should have been updated.

If you are unlucky as I've been somethign like this will happen
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/bgb00008.bmp)
The enemy has been spawned in the middle of a wall

Luckily ZGB will let you place enemies on the map using the Game Boy Map Builder. This thing is accomplished by the function GetTileReplacement in ZGBMain.c
```
UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if(current_state == STATE_GAME) {
		if(U_LESS_THAN(255 - (UINT16)*tile_ptr, N_SPRITE_TYPES)) {
			*tile = 0;
			return 255 - (UINT16)*tile_ptr;
		}

		*tile = *tile_ptr;
	}

	return 255u;
}
```
This function is called when the sprite is updating any tile in the background. It receives a pointer to a UNIT8 that contains the tile and it must return an Sprite_Type or 255 spawn is done. Also it must assign on tile_ptr the tile that should go there allowing us to replace it by another one

This default function will spawn and enemy of type 0 when the tile is 255, and enemy of type 1 when the tile is 254, and enemy of type 2 when the tile is 253... and so on

Open the tiles.bgr with the Game Boy Tile Designer and go to View->Tile Count and set that to 255. Now edit the tile 254 that corresponds to the sprite of type 1 (SPRITE_ENEMY) and create a thumbnail of our enemy
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/7-thumb.PNG)
Export and save it.

Now open the res/map.bgm again and scroll down the list of tiles. You should see the thmbnail. Put it on some places on your map and when you are done Export and Save. 
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/8-mapenemies.PNG)

If you build the program now you should see enemies spawned on those places instead of the thumbnail
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/postGBJAM5_Improvements/doc%20files/bgb00009.bmp)
You will also see that the Sprite we were spawning by code is still there so make sure you delete that line

Music
-----
If you want to add any music
- Create a folder music into the res folder and place your mods there.
- To select the bank where the music will be stored remember to include it between the filename and the extension (eg: game.b3.mod will go to bank 3)
- In StateGame declare a extern UINT8* array named [filename]_mod_Data. For example, if your file is named game.mod then you should declare it like this
```
extern UINT8* game_mod_Data[];
```
- At the end of the Start method of StateGame call PlayMusic
```
void Start_STATE_GAME() {
	...
	PlayMusic(level_mod_Data, 3, 1);
}
```