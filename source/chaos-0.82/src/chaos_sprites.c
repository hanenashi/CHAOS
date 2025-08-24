// the chaos_sprites.c file...
#include "platform.h"
#include"chaos_sprites.h"
#ifdef __GBA__
#include "gba.h"
#include "sprite.h"

#elif defined( __WINDOWS__)
#define COLOR_256 0
#define SQUARE 0
#define SIZE_16 0
#endif
// CURSOR_SPELL_GFX
const static struct QGFX_SPRITEMAP CYC_SpriteMap_0 = {
  16,16,  // width, height in pixels
  0,106,  // x,y screen pos
  COLOR_256 | SQUARE , // attrib 0
  SIZE_16 , // attrib 1
  0, // OAM value
  0, // spriteID
  1,
  binary_cursor_spell_raw_start, // pointer to graphics 
  binary_cursor_spell_map_start, // pointer to map 
};
 
// CURSOR_ENGAGED_GFX
const static struct QGFX_SPRITEMAP CYC_SpriteMap_1 = {
  16,16,  // width, height in pixels
  0,106,  // x,y screen pos
  COLOR_256 | SQUARE , // attrib 0
  SIZE_16 , // attrib 1
  0, // OAM value
  0, // spriteID
  1,
  binary_cursor_engaged_raw_start, // pointer to graphics 
  binary_cursor_engaged_map_start, // pointer to map 
};
 
// CURSOR_FIRE_GFX
const static struct QGFX_SPRITEMAP CYC_SpriteMap_2 = {
  16,16,  // width, height in pixels
  0,106,  // x,y screen pos
  COLOR_256 | SQUARE , // attrib 0
  SIZE_16 , // attrib 1
  0, // OAM value
  0, // spriteID
  1,
  binary_cursor_fire_raw_start, // pointer to graphics 
  binary_cursor_fire_map_start, // pointer to map 
};
 
// CURSOR_FLY_GFX
const static struct QGFX_SPRITEMAP CYC_SpriteMap_3 = {
  16,16,  // width, height in pixels
  0,106,  // x,y screen pos
  COLOR_256 | SQUARE , // attrib 0
  SIZE_16 , // attrib 1
  0, // OAM value
  0, // spriteID
  1,
  binary_cursor_fly_raw_start, // pointer to graphics 
  binary_cursor_fly_map_start, // pointer to map 
};
 
// CURSOR_GROUND_GFX
const static struct QGFX_SPRITEMAP CYC_SpriteMap_4 = {
  16,16,  // width, height in pixels
  0,106,  // x,y screen pos
  COLOR_256 | SQUARE , // attrib 0
  SIZE_16 , // attrib 1
  0, // OAM value
  0, // spriteID
  1,
  binary_cursor_ground_raw_start, // pointer to graphics 
  binary_cursor_ground_map_start, // pointer to map 
};
 
// CURSOR_NORMAL_GFX
const static struct QGFX_SPRITEMAP CYC_SpriteMap_5 = {
  16,16,  // width, height in pixels
  0,106,  // x,y screen pos
  COLOR_256 | SQUARE , // attrib 0
  SIZE_16 , // attrib 1
  0, // OAM value
  0, // spriteID
  1,
  binary_cursor_raw_start, // pointer to graphics 
  binary_cursor_map_start, // pointer to map 
};
 
const static struct QGFX_SPRITETABLE  QMAP_Cyc_SpriteTable []= {
	{&CYC_SpriteMap_0},
	{&CYC_SpriteMap_1},
	{&CYC_SpriteMap_2},
	{&CYC_SpriteMap_3},
	{&CYC_SpriteMap_4},
	{&CYC_SpriteMap_5},
};
 
const struct QGFX_ROOT CHAOS_SPRITES_ROOT = {
    6, // number of sprites 
    QMAP_Cyc_SpriteTable,
};
// end of file
 

