

ZGB
===

A little engine for creating games for the original Game Boy.

 1. [Features](#features)
 2. [Installation](#installation)
 3. [States and Sprites](#states-and-sprites)
 4. [Resource Guidelines](#resource-guidelines)
 5. [Banks](#banks)
 6. [Tutorial](#tutorial)
	 1. [Creating a new Project](#creating-a-new-project)
	 2. [Input](#input)
	 3. [Animations](#animations)
	 4. [Collisions](#collisions)
	 5. [Enemies](#enemies)
	 6. [Maps](#maps)
	 7. [Frame Skipping](#frame-skipping)
	 8. [Music](#music)
	 9. [Sounds](#sounds)
	 10. [Debugging](#debugging)
	 11. [Game Boy Color](#game-boy-color)

Features <a name="features"></a>
--------------------------------

 - Coding in C using [gbdk](http://gbdk.sourceforge.net/)
 - Based on States and Sprites
 - 4-directional scroll 
 - Support maps bigger than 32x32
 - Transparent sprites pool allowing 20 sprites 8x16 or 16x16
 - Sprite animations
 - Collisions sprite vs sprite and sprite vs background and configurable rectangle collider
 - Easy makefile support (just 3 lines for most projects)
 - Bank management
 - Fonts
 - Music support using [gbt-player](https://github.com/AntonioND/gbt-player)
 - Sound Effects

Installation <a name="installation"></a>
----------------------------------------

Download and install:

 - gbdk ([2.95-3](https://sourceforge.net/projects/gbdk/files/gbdk-win32/2.95-3/) at the moment of writing this) on c:\gbdk
 - [msys](https://sourceforge.net/projects/mingw-w64/files/External%20binary%20packages%20%28Win64%20hosted%29/MSYS%20%2832-bit%29/MSYS-20111123.zip/download) anywhere but add the folder msys/bin to your Path environment var
 - [Make for Windows](http://gnuwin32.sourceforge.net/packages/make.htm)* on C:\gbdk\bin\make-3.81-bin
 - [BGB](http://bgb.bircd.org/) on c:\gbdk\bin
 - [mod2gbt](https://github.com/AntonioND/gbt-player/raw/master/legacy_gbdk/mod2gbt/mod2gbt.exe) on c:\gbdk\bin
 - [Game Boy Map Builder](http://www.devrs.com/gb/hmgd/gbmb.html) on c:\gbdk\tools\gbmb18
 - [Game Boy Tile Designer](http://www.devrs.com/gb/hmgd/gbtd.html) on c:\gbdk\tools\gbtd22
 - This repo, anywhere but create an env var called ZGB_PATH pointing the common folder eg: c:\ZGB\common
 - [Microsoft Visual Studio Community Edition](https://www.visualstudio.com/downloads/) (optional)

*The make that comes with msys doesn't print any error messages and that's why you need to download make for windows too.

States and Sprites <a name="states-and-sprites"></a>
----------------------------------------------------

Before you start coding anything it is important that you understand what States and Sprites are for ZGB

 * **States**: you can think of States as the current main loop of your program (they are similar to Scenes in Unity3d). There can only be one of them running at a time and everytime you change from one to another all resources are discarded. Every declared State must contain 2 functions:
	 - ***Start***: that handles the initialization of the State. Here is where you should load all the resources you are going to need and perform all variables initialization
	 - ***Uptate***: called one per frame, for global interactions
 * **Sprites**: these are the objects that you see moving on screen (like GameObjects in Unity). Right now only 8x16 and 16x16 sprites are supported and  no matter the size you cannot have more than 20 sprites at a time or the rom will crash. Every declared Sprite must contain 3 functions:
	 * ***Start***: that handles the initialiation of the Sprite. It is called inmediatelly after the Sprite is created
	 * ***Update***: called one per frame
	 * ***Destroy***: called before deleting and Sprite

 every Sprite has a [struct](https://github.com/Zal0/ZGB/blob/master/common/include/Sprite.h) associated with it that contains its current data. This struct can be accesed using the **THIS** variable inside any of its functions and contains the next variables:
  * ***anim_data***: a pointer to an UINT8 array containing a list of frames for the current animation. The first element must be the number of frames
  * ***x, y***: current world position of the Sprite
  * ***flags***: right now only used for sprite mirroring, setting it to OAM_VERTICAL_FLAG will mirror the Sprite and setting it to 0 will leave it as it is
  * ***lim_x, lim_y***: by default sprites are destroyed when going offscreen. Using these vars you can control how far can they go without being destroyed
  * ***custom_data***: 8 bytes for storing custom information
 
 Sprites are managed by the **SpriteManager** which is responsible for the update of every one of them (you don't need to worry about that, it happens automatically). Some useful functions of the SpriteManager are:
	 - ***SpriteManagerAdd***: for adding new Sprites
	 - ***SpriteManagerRemove***: for removing them
	 - ***SPRITEMANAGER_ITERATE***: a useful macro for iterating through all active sprites

Resource guidelines <a name="resource-guidelines"></a>
-------------------
Some guidelines you must follow to make your life easier when developing for Game Boy using ZGB:

- **Tiles**:
	- Place them in the **res/** folder of the root directory of your project
	- In the Export To... set te filename as src/[tile name].[bank].c
	- When making changes besides saving your file remember to Export it 
- **Maps**:
	- Place them in the **res/** folder of the root directory of your project
	- When setting the tileset of a map (File->Map Properties) ensure the Filename is relative (it shouldn't contain any folders on the path, eg: "C:\Users\Zalo\ZGB-template\res\tiles.gbr" should be changed to just "tiles.gbr"). This will make your life easier if you are working on your game with more people
	- In the Export To... set te filename as src/[map name].[bank].c
	- In the Export To... ensure you have the Location Format like this
	![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/4-Location%20Format.PNG)
	- When making changes besides saving your file remember to Export it 
- Music:
	- Place your music mod files in the res/music folder
	- The makefile will handle the conversion automatically so you don't need to worry about it
	- For more info about music formats check the [gbt-player](https://github.com/AntonioND/gbt-player) docs

Banks <a name="banks"></a>
-----
It is very important that you understand the concept of banks. ZGB together with GBDK is taking most of the space in bank0 and gbt-player is using most of the space in bank1. That means you will have to put your code on bank 2 and further, and that you will have to use banks, whether you like it or not. Check the point 3 of my post [Game Boy Development Tips and Tricks (I)](http://zalods.blogspot.com.es/2016/07/game-boy-development-tips-and-tricks-i.html) for more info.

The good news is that you don't have to worry much about bank swapping since ZGB will take care of it in most situations (before calling start, update or destroy).

Bank management is managed in ZGB in 2 different ways
- **Setting the bank on the filename**: if you name your file [filename].[bank].c that will be compiled into the [bank] specified. All your maps, tiles and music should use this convention (eg: player.b3.c will be compiled into bank 3)
- **Setting the bank using #pragma bank=[bank]**: this is the standard way gbdk handles banks. You should use this in all your code files. Also for States and Sprites you need to declare an extra var so the engine will know where it has been placed (And it will swap banks before calling start, update or destroy automatically). The first 3 lines of your States and Sprites should look like:
```
#pragma bank=2
#include "StateGame.h"
UINT8 bank_STATE_GAME = 2;
```
 having both declarations together will make things easier when moving parts of your code from one bank to another. 

As a suggestion start placing all your code files into bank 2 and all your resources into bank 3. When you run out of space you can increase the number of banks in the Makefile of your project (also remember the number of banks must be a power of 4: 4, 8, 16, 32...)


Tutorial <a name="tutorial"></a>
--------
Here is a quick tutorial of how to make a simple game like this one

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/tuto.gif)

Creating a New Project <a name="creating-a-new-project"></a>
----------------------
Download [this template](https://github.com/Zal0/ZGB-template/archive/master.zip).

Try to build it by launching **build.bat** or loading the project into Visual Studio. If you have problems compiling check the installation list again. 

If everything goes ok **BGB** will be automatically launched after the build and you should see something like this

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/bgb00006.bmp)

The template contains the minimum files required for making a build:

 - **ZGBMain_Init**: The Main file for your project, all states an sprites declarations must be done in this file
 - **ZGBMain**: This containts the default method for replacing tiles when adding them to the map
 - **SpritePlayer**: this is the code of the Game Boy Sprite that you see on the screen. It does nothing, but has all the required methods
 - **StateGame**: the current State that makes a basic initialization:
	 - Loads all sprites graphics into memory
	 - Creates an sprite of type SPRITE_PLAYER and sets it as the scroll camera
	 - Loads all tile graphics into memory
	 - Initializes the scroll (this will center the camera on the sprite set as camera and fill the background)

Input <a name="input"></a>
----------------------
Let's make our sprite move a little bit:

 - Open **SpritePlayer.c** and include **Keys.h** and **SpriteManager.h**
 - The **KEY_PRESSED** macro tells us if a key is pressed or not. Write the next code in the Update function:
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

Animations <a name="animations"></a>
----------
Let's make our Sprite animate when walking

 - Open the file **player.bgr** with **Game Boy Tile Designer** (you should associate the extension *.gbr to be always opened whit this program if you haven't done it yet)
 - Change the aspect of the Sprite by a boy and add a couple of frames for the walking animation. Something like this 
![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/2-anim.PNG)
 - Now go to File->Export To... and Update the **number of frames** (From 0 to 2) and after that Save your file
 - If you Build your the project now you should see the new Sprite instead of the Game Boy
 - In **ZGBMain_Init.c** we need to update the number of frames of our sprite, the last parameter of **INIT_SPRITE** does exactly that: 
```
INIT_SPRITE(SPRITE_PLAYER, player, 3, FRAME_16x16, 3);
```
 - Animations are declared as arrays of UINT8, the first value is the number of frames. Declare the next arrays on **SpritePlayer.c**
```
const UINT8 anim_idle[] = {1, 0};
const UINT8 anim_walk[] = {2, 1, 2};
```
 - And now you can animate your character like this:
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

Collisions <a name="collisions"></a>
----------
Right now if you move the player you'll see it can go throught walls (also going offscreen will kill the sprite so we need to avoid this). Here is how to add collision checking with background tiles:

* Collisionable tiles are declared as an **array of UINT8 ended with 0**. If you open the file **tiles.gbr** you will see that we only have two tiles (the first one is empty and we don't want that to be collidable). Declare the next array on StateGame.c
```
UINT8 collision_tiles[] = {1, 0};
```
* This list is the 4th paramater passed to the **InitScroll** function on the Start method of **StateGame**. Right now it is set to 0 (null), change it into this:
```
InitScroll(mapWidth, mapHeight, map, collision_tiles, 0, 3);
```
* Now if you want the scroll to manage collisions with your sprite you need to use the function **TranslateSprite** instead of directly updating the x and y coordinates of the sprite. Here is how the Update method should look like now:
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
* There is still a small issue that can be fixed. Unless you did a square your Sprite might be stopping before touching the collidable tile

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/3-bad%20collision.PNG) 
 
 this can be fixed by adjusting the **collider** of the Sprite on the Start method. The collider of an Sprite is defined by an rectangle declared using **coll_x, coll_y, coll_w and coll_h** These values worked for me:
```
void Start_SPRITE_PLAYER() {
	THIS->coll_x = 2;
	THIS->coll_y = 0;
	THIS->coll_w = 12;
	THIS->coll_h = 16;
}
```

Enemies <a name="enemies"></a>
-------

Time to add a little bit of action to our game by adding an enemy.

* Open the **Game Boy Tile Designer** and create a new **16x16** Sprite for our enemy (View->Tile Size->16x16). This is what I came up with:

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/5-enemy.PNG)

 Save the file into the res/ folder and Set Export settings (File -> Export To...) as:
	*	FileName: src/enemy.b3.c
	*	Type: GBDK file
	*	Label: enemy
	*	From 0 to 1

 Save again

* Create a new file **SpriteEnemy.h** in the include folder with the next content:
```
#ifndef SPRITE_ENEMY_H
#define SPRITE_ENEMY_H

#include "main.h"

DECLARE_SPRITE(SPRITE_ENEMY);

#endif
```
 the macro **DECLARE_SPRITE** is just a shortcut for including the declaration of Start, Update and Destroy followed by the type of the Sprite (in this case Start_SPRITE_ENEMY, Update_SPRITE_ENEMY and Destroy_SPRITE_ENEMY)

* Create another file **SpriteEnemy.c** inside de src folder with the next content: 
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
* In **ZGB_Main.h** add the new type for the Sprite
```
typedef enum {
	SPRITE_PLAYER,
	SPRITE_ENEMY,

	N_SPRITE_TYPES
} SPRITE_TYPE;
```
* In **ZGB_Main.c**
	* Include "SpriteEnemy.h" and "../res/src/enemy.h"
	* Add its initialization, we stored it on bank 3, saved as 16x16 and created only 1 frame  so it should look like this
```
void InitSprites() {
	...
	INIT_SPRITE(SPRITE_ENEMY,  enemy,  3, FRAME_16x16, 1);
}
```
* In **StateGame.c** create an instance in 70, 50
```
void Start_STATE_GAME() {
	...
	SpriteManagerAdd(SPRITE_ENEMY, 70, 50);
}
```
  * If you build the game now you'll see your player and the new enemy. It does nothing and there aren't any collisions yet
 
 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/bgb00007.bmp)

* Write the next code on **SpriteEnemy.c**
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
 Here you can see how the field **custom_data** of the Sprites is mean to be used. Also **TranslateSprite** will return the index of the tile when a collision happens or 0 otherwise. In this case if there is a collision with the background the velocity of the sprite is negated making it changing its direction

- In order to collide with the enemy you have to add the next code at the end of the method Update of **SpritePlayer.c**
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
"ZGBMain.h" needs to be included in order to get access to all the sprite types. **SPRITEMANAGER_ITERATE** is a macro that iterates through all the active sprites, if the sprite is of type **SPRITE_ENEMY** then we use the function **CheckCollision** to check if we are colliding with it (do it in this order). If that happens then **SetState** is called unloading the current state and loading the state passed as paramenter (even if it is the current one, like in this case)

Maps <a name="maps"></a>
----
Let's now focus on the creation of our game level. 

- Open the file **res/map.bgm** with the **Game Boy Map Builder** and create your stage. This is how mine looks like

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/6-map.PNG)

 Export and Save and make a build. If you are unlucky something like this will happen

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/bgb00008.bmp)

 The enemy has been spawned in the middle of a wall

- Luckily ZGB will let you place enemies on the map using the Game Boy Map Builder. This thing is accomplished by the function **GetTileReplacement** in **ZGBMain.c**
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
- This function is called for any tile that its placed in the background. 
	- It receives a UINT8* pointing to the tile that will be placed 
	- It must return an Sprite_Type or 255 to spawn nothing. 
	- It must assign on tile the tile index that should go in the background (usually the same that was passed on tile_ptr except when spawning an enemy)
	- This default function will spawn and enemy of type 0 when the tile is 255, and enemy of type 1 when the tile is 254, and enemy of type 2 when the tile is 253... and so on

- Open the **tiles.bgr** with the **Game Boy Tile Designer** and go to View->Tile Count and set that to 255. Now **edit the tile 254** that corresponds to the sprite of type 1 (SPRITE_ENEMY) and create a thumbnail of the enemy

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/7-thumb.PNG)

 Export and save it.

- Open the **res/map.bgm** again and scroll down the list of tiles. You should see the thumbnail on the tile 254. Put it on some places of your map

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/8-mapenemies.PNG)

 Export and Save. 

- Build the program now and you should see enemies spawned on those places instead of the thumbnail

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/master/doc%20files/bgb00009.bmp)

 You will also see that the Sprite we were spawning by code is still there so make sure you delete that line

Frame Skipping <a name="frame-skipping"></a>
--------------
If you add too many enemies on your map you might notice a big slowdown that at some points can even make the game unplayable. This can be improved a little bit using the var delta_time that:

- Will be 0 when the frame rate is ~60fps
- Will be 1 otherwise

You can use this var to multiply the current amount of movement of your sprites (x <<  0 is the same as multiplying by 1 and x << 1 is the same as multiplying by 2). This is how your update method should look like on your SpritePlayer.c

```
void Update_SPRITE_PLAYER() {
	...
	if(KEY_PRESSED(J_UP)) {
		TranslateSprite(THIS, 0, -1 << delta_time);
		SetSpriteAnim(THIS, anim_walk, 15);
	} 
	if(KEY_PRESSED(J_DOWN)) {
		TranslateSprite(THIS, 0, 1 << delta_time);
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(KEY_PRESSED(J_LEFT)) {
		TranslateSprite(THIS, -1 << delta_time, 0);
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	if(KEY_PRESSED(J_RIGHT)) {
		TranslateSprite(THIS, 1 << delta_time, 0);
		SetSpriteAnim(THIS, anim_walk, 15);
	}
	...
}
```

and here is the same change on SpriteEnemy.c

```
void Update_SPRITE_ENEMY() {
	struct EnemyInfo* data = THIS->custom_data;
	if(TranslateSprite(THIS, 0, data->vy << delta_time)) {
		data->vy = -data->vy;
	}
}
```

For more info about frame skipping check this post on my blog [Game Boy Development Tips and Tricks (II)](http://zalods.blogspot.com.es/2016/07/game-boy-development-tips-and-tricks-ii.html)


Music <a name="music"></a>
-----
If you want to add any music

- Create the folder **res/music** and place your mods there.
- To select the **bank** where the music will be stored remember to include it between the filename and the extension (eg: game.b3.mod will go to bank 3)
- In **StateGame.c** declare a extern UINT8* array named **[filename]_mod_Data**. For example, if your file is named game.mod then you should declare it like this
```
extern UINT8* game_mod_Data[];
```
- At the end of the Start method of **StateGame.c** call **PlayMusic**
```
void Start_STATE_GAME() {
	...
	PlayMusic(level_mod_Data, 3, 1);
}
```

Sounds <a name="sounds"></a>
------
If you want to add sound to your game I recommend downloading the fixed version of the Sound Sample [here](https://github.com/Zal0/GBSoundDemo)

![scr1](https://github.com/Zal0/GBSoundDemo/blob/master/screenshots/bgb00011.png?raw=true) ![scr2](https://github.com/Zal0/GBSoundDemo/blob/master/screenshots/bgb00012.png?raw=true) ![scr3](https://github.com/Zal0/GBSoundDemo/blob/master/screenshots/bgb00013.png?raw=true) ![scr4](https://github.com/Zal0/GBSoundDemo/blob/master/screenshots/bgb00014.png?raw=true) ![scr5](https://github.com/Zal0/GBSoundDemo/blob/master/screenshots/bgb00015.png?raw=true)

Pressing select + A will output the current register values, then you just need to call PlayFx with the values you see there. For example you can add this sound when the enemy changes its direction on **SpriteEnemy.c**
```
void Update_SPRITE_ENEMY() {
	struct EnemyInfo* data = THIS->custom_data;
	if(TranslateSprite(THIS, 0, data->vy << delta_time)) {
		data->vy = -data->vy;
		PlayFx(CHANNEL_4, 4, 0x0c, 0x41, 0x30, 0xc0);
	}
}
```
and this one when an enemy kills the player on **SpritePlayer.c**
```
void Update_SPRITE_PLAYER() {
	...
	SPRITEMANAGER_ITERATE(i, spr) {
		if(spr->type == SPRITE_ENEMY) {
			if(CheckCollision(THIS, spr)) {
				PlayFx(CHANNEL_1, 10, 0x4f, 0xc7, 0xf3, 0x73, 0x86);
				SetState(STATE_GAME);
			}
		}
	}
	...
}
```
rememeber to include **Sound.h** in both files

If you are playing an fx at the same time the music is playing and the music is using that channel then you need to mute it for a moment (the second parameter in PlayFx is the number of frames you want that music channel to be muted)

If you don't have music on your game then you need to init register 5 (take a look [here](http://gbdev.gg8.se/wiki/articles/Sound_tutorial)). Usually this setup will work for you
```
	NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
	NR50_REG = 0x77; //Max volume
```

Debugging <a name="debugging"></a>
---------
I am not gonna lie to you, the **sdcc** that comes with the gbdk is far from perfect. It has a lot of issues that are hard to deal with ([take a look at this post](http://zalods.blogspot.com.es/2016/12/game-boy-development-tips-and-tricks-iii.html))

The **printf** function that comes with the gbdk is ok for small projects but as your code grows you won't be able to use it (actually if you use ZGB you can't use it because it takes a big part of bank 0 that is also required by printf)

Instead you can use the print functions available on Print.h

- INIT_CONSOLE will activate the Window and place it so that n number of lines ara available for printing (obviously this won't let you use the Window for other things)
- print_x and print_y will let you set the position where you want to write (take into account that after writing, these vars are automatically updated so you should reset then manually if you want to constantly print a value on the same position). You can update both using DPRINT_POS
- you will need a font. I have included one that can be used as a placeholder (and stored on bank 3). This font will take the last 45 tiles so if your vram is full you will have a conflict here too.
- These are the functions available:
	- INIT_CONSOLE: inits the console
	- DPrintf: similar to printf
	- DPRINT_POS: places the printing position
	- DPRINT: places the printing position and prints the string passed as parameter

If you want to debug the position of the SpritePlayer here is what you should do: 

- On StateGame.c:
```
#include "Print.h"
#include "../res/src/font.h"

...

void Start_STATE_GAME() {
	...
	INIT_CONSOLE(font, 3, 2);
}
```

- On SpritePlayer.c
```
#include "Print.h"

...

void Update_SPRITE_PLAYER() {
	...
	DPRINT_POS(0, 0);
	DPrintf("x:%d y:%d  ", THIS->x, THIS->y);
}
```

Leaving at least one space at the end of your text is recommented because the Window is never cleaned so previous values might overlap the current ones making it a bit confusing.

One last thing: If you are using **build.bat** (or your current target in Visual Studio is Release) you won't see anything. You need to compile using **build_Debug.bat** (or change the Target to Debug on VS). This will create a new rom ending in _Debug.gb. This will ensure none of this code will end in your final rom.


Game Boy Color <a name="game-boy-color"></a>
---------
In order to create a rom for the Game Boy Color you just need to change the target of your project to ReleaseColor or DebugColor and build it (or run build_Color.bat or build_DebugColor.bat). This new build will take advantage of the better processor that the Game Boy Color has and will run twice faster. By default it will load a black and white palette, so let's see how to add some color.

**Adding color to tiles**

 - Open the file tiles.bgr with the Gameboy Tile Designer and select View->Color Set->Gameboy Color. Everything will look the same but if you click on the palettes list you'll see there are now 8 palettes to choose from.

 ![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/9-paletteChoose.PNG)
 - Select the fist tile and click on the second palette. Now go to View->Palettes (or click control+A) and edit the second palette to add some colors

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/10-bgpaletteset.PNG)
 - Finally go to ***File->Export To*** and in the ***Advanced*** tab set ***Include palette colors*** and then select ***1 byte per entry*** on ***CGB palettes***. Click Ok to export everything.

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/11-exportsettings.PNG)

Now in your code:

 - Open the file StateGame.c and declare the palette for the background before the method Start_STATE_GAME like this
```
#include "Palette.h"
const UINT16 bg_palette[] = {PALETTE_FROM_HEADER(tiles)};
```
The macro ***PALETTE_FROM_HEADER*** will take the palette exported on one of the files. The parameter passed should be the same that you used on the ***Label*** on export settings:

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/12-labelName.PNG)

 - Load the palette inside Start_STATE_GAME using
```
void Start_STATE_GAME() {
...
SetPalette(BG_PALETTE, 0, 8, bg_palette, bank_STATE_GAME);
...
}
```

 - Change the line 
```
InitScrollTiles(0, 2, tiles, 3);
```
to
```
InitScrollTilesColor(0, 2, tiles, 3, tilesCGB);
```
this new parameter contains the palette info for each tile exported by the Tile Exporter. Build the project and you should see your background in color 

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/bgb00003.bmp)

**Adding color to sprites**
The process for sprites is very similar to what we did for the tiles

 - Add some color to player.bgr the same way we did with the tiles
	 - Select View->Color Set->Gameboy Color to set it as a Gameboy Color file
	 - Edit the Second palette
	 - Assign it to all the sprites
	 - Set ***Include palette colors*** and change CGB palettes to ***1 byte per entry*** in the ***Advanced*** tab in File->Export To...
	 - Save your file and export it

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/13-spritecolored.PNG)

Now in your code:

 - Open the file ZGBMain_b.c and change the INIT_SPRITE call for your sprite for INIT_SPRITE_COLOR, like this
```
void InitSprites() {
	INIT_SPRITE_COLOR(SPRITE_PLAYER, player, 3, FRAME_16x16, 3);
	...
}
```
 - Similar to what we did with the tiles we need to declare a palette for the sprites in StateGame.c. Do this next the background palette declaration
```
#include "..\res\src\player.h"
#include "Palette.h"
UINT16 bg_palette[] = {PALETTE_FROM_HEADER(tiles)};
UINT16 sprites_palette[] = {PALETTE_FROM_HEADER(player)};
```
Note we have also included ..\res\src\player.h since that is the file where the palette we want is (we didn't need to do this for the tiles since ..\res\src\tiles.h was already included)
 - Finally load the palette for the sprites inside Start_STATE_GAME
 ```
 void Start_STATE_GAME() {
 ...
 SetPalette(BG_PALETTE, 0, 8, bg_palette, bank_STATE_GAME);
	SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette, bank_STATE_GAME);
 ...
 }
 ```
 Build the project and your main character will appear in glorious 4 colors

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/bgb00004.bmp)
 
 Note: It seems that by default BGB doesn't show colors properly, so ensure you have GBC LCD colors disabled inside Options->Graphics

![enter image description here](https://raw.githubusercontent.com/Zal0/ZGB/feature/color/doc%20files/14-bgbsettings.PNG)