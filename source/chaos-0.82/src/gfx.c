#include "platform.h"

#ifdef __GBA__
// gba library includes
#include "dma.h"
#include "sprite.h"
#include "screenmode.h"
#define WIN_X_OFFSET   0
#define WIN_Y_OFFSET   0
#endif

#ifdef __WINDOWS__
#include "SDL.h"

// the start of the arena is 16 to the right and 16 down
// from the real "top" of the screen - that is 2 tiles
#define WIN_X_OFFSET   2
#define WIN_Y_OFFSET   2

int gfx_scale = 1;
#endif
#include "gfx.h"
// specific includes
#include "spelldata.h"
#include "arena.h"
#include "creature.h"
#include "chaos_sprites.h"
#include "chaos.h"
#include "wizards.h"
#include "text16.h"

#if !defined( __MINGW32__)
#define FIGHT_GFX_DATA _binary_fight_raw_start
#else
#define FIGHT_GFX_DATA binary_fight_raw_start
#endif
extern u8 FIGHT_GFX_DATA[];
#if !defined( __MINGW32__)
#define FIGHT_MAP_DATA _binary_fight_map_start
#else
#define FIGHT_MAP_DATA binary_fight_map_start
#endif
extern u8 FIGHT_MAP_DATA[];

#if !defined( __MINGW32__)
#define SPELL_GFX_DATA _binary_spell_raw_start
#else
#define SPELL_GFX_DATA binary_spell_raw_start
#endif
extern u8 SPELL_GFX_DATA[];
#if !defined( __MINGW32__)
#define SPELL_MAP_DATA _binary_spell_map_start
#else
#define SPELL_MAP_DATA binary_spell_map_start
#endif
extern u8 SPELL_MAP_DATA[];

#if !defined( __MINGW32__)
#define SPLAT_GFX_DATA _binary_bolt_anim_raw_start
#else
#define SPLAT_GFX_DATA binary_bolt_anim_raw_start
#endif
extern u8 SPLAT_GFX_DATA[];
#if !defined( __MINGW32__)
#define SPLAT_MAP_DATA _binary_bolt_anim_map_start
#else
#define SPLAT_MAP_DATA binary_bolt_anim_map_start
#endif
extern u8 SPLAT_MAP_DATA[];

#if !defined( __MINGW32__)
#define DRAGONBREATH_GFX_DATA _binary_fire_anim_raw_start
#else
#define DRAGONBREATH_GFX_DATA binary_fire_anim_raw_start
#endif
extern u8 DRAGONBREATH_GFX_DATA[];
#if !defined( __MINGW32__)
#define DRAGONBREATH_MAP_DATA _binary_fire_anim_map_start
#else
#define DRAGONBREATH_MAP_DATA binary_fire_anim_map_start
#endif
extern u8 DRAGONBREATH_MAP_DATA[];

#if !defined( __MINGW32__)
#define POP_GFX_DATA _binary_disappear_raw_start
#else
#define POP_GFX_DATA binary_disappear_raw_start
#endif
extern u8 POP_GFX_DATA[];
#if !defined( __MINGW32__)
#define POP_MAP_DATA _binary_disappear_map_start
#else
#define POP_MAP_DATA binary_disappear_map_start
#endif
extern u8 POP_MAP_DATA[];


#if !defined( __MINGW32__)
#define PALETTE0_DATA _binary_palette0_pal_start
#define PALETTE1_DATA _binary_palette1_pal_start
#define PALETTE2_DATA _binary_palette2_pal_start
#define PALETTE3_DATA _binary_palette3_pal_start
#define PALETTE4_DATA _binary_palette4_pal_start
#define PALETTE5_DATA _binary_palette5_pal_start
#define PALETTE6_DATA _binary_palette6_pal_start
#define PALETTE7_DATA _binary_palette7_pal_start
#define PALETTE8_DATA _binary_palette8_pal_start
#define PALETTE9_DATA _binary_palette9_pal_start
#define SPRITEPALETTE_DATA _binary_paletteb_pal_start
#define SPRITEPALETTE_DATA_END _binary_paletteb_pal_end
#define SPRITEPALETTE_SIZE (_binary_paletteb_pal_start - _binary_paletteb_pal_end)
#else
#define PALETTE0_DATA binary_palette0_pal_start
#define PALETTE1_DATA binary_palette1_pal_start
#define PALETTE2_DATA binary_palette2_pal_start
#define PALETTE3_DATA binary_palette3_pal_start
#define PALETTE4_DATA binary_palette4_pal_start
#define PALETTE5_DATA binary_palette5_pal_start
#define PALETTE6_DATA binary_palette6_pal_start
#define PALETTE7_DATA binary_palette7_pal_start
#define PALETTE8_DATA binary_palette8_pal_start
#define PALETTE9_DATA binary_palette9_pal_start
#define SPRITEPALETTE_DATA binary_paletteb_pal_start
#define SPRITEPALETTE_DATA_END binary_paletteb_pal_end
#define SPRITEPALETTE_SIZE (binary_paletteb_pal_start - binary_paletteb_pal_end)
#endif

extern u16 PALETTE0_DATA[];
extern u16 PALETTE1_DATA[];
extern u16 PALETTE2_DATA[];
extern u16 PALETTE3_DATA[];
extern u16 PALETTE4_DATA[];
extern u16 PALETTE5_DATA[];
extern u16 PALETTE6_DATA[];
extern u16 PALETTE7_DATA[];
extern u16 PALETTE8_DATA[];
extern u16 PALETTE9_DATA[];

const u16 * PALETTES [10] = {
   PALETTE0_DATA,
   PALETTE1_DATA,
   PALETTE2_DATA,
   PALETTE3_DATA,
   PALETTE4_DATA,
   PALETTE5_DATA,
   PALETTE6_DATA,
   PALETTE7_DATA,
   PALETTE8_DATA,
   PALETTE9_DATA,
  };
               
extern u8 SPRITEPALETTE_DATA[];
extern u8 SPRITEPALETTE_DATA_END[];
               
               
extern u8 _binary_bg_raw_start[];
#ifdef __GBA__               
Bg bg_level;   
Bg bg_text;    
Bg bg_rotate; 
#elif defined(__WINDOWS__)
// 8*8 tile palette emulation
int g_tilePalette8[WIN_SCRN_X*WIN_SCRN_Y];
int g_cursor_type = 0;
int g_cursor_on = 0;
// emulates the BG palette
u32 BGPaletteMem[256];
u32 SpritePaletteMem[16];
SDL_Surface *screen;

void draw8x8SpriteTile(int x, int y, const u8 * gfx, int flags);
#endif
               
//unsigned char bg_colour; // the colour of the pixel to draw in the rotation layer

unsigned char cursor_x = 0;
unsigned char cursor_y = 0;

unsigned char fadedup;
#ifdef __GBA__
#define IN_EWRAM __attribute__ ((section (".ewram")))
OAMEntry sprites[128] IN_EWRAM;
#endif


void load_sprite_data(u32 spriteid, u32 nFrame);
void load_sprite(u32 spriteid, u32 copyOAM);
void set_palette(unsigned short x, unsigned short y, unsigned char palette);

void draw_creature(unsigned char x, unsigned char y, unsigned char i,unsigned char f) {
  // draw the creature i at position x,y with frame f
    set_palette(x,y,CHAOS_SPELLS.pSpellDataTable[i]->palette);
    
    draw_gfx(CHAOS_SPELLS.pSpellDataTable[i]->pGFX,
             CHAOS_SPELLS.pSpellDataTable[i]->pGFXMap,
             x, y, f);
    
}
void draw_creature8(unsigned char x, unsigned char y, unsigned char i,unsigned char f) {
  // draw the creature i at position x,y with frame f
#ifdef __GBA__
  set_palette8(x,y,CHAOS_SPELLS.pSpellDataTable[i]->palette);
    
  draw_gfx8(CHAOS_SPELLS.pSpellDataTable[i]->pGFX,
      CHAOS_SPELLS.pSpellDataTable[i]->pGFXMap,
      x, y, f);
#else
  draw_creature(x, y, i,f);
#endif
    
}
#if defined( __WINDOWS__ )
void clear_square8(unsigned char x, unsigned char y) {
  Uint32 color = SDL_MapRGB (screen->format, 0, 0, 0);
  SDL_Rect rect;
  rect.w = 16*gfx_scale; 
  rect.h = 16*gfx_scale;
  rect.x = (WIN_X_OFFSET + x)*8*gfx_scale;
  rect.y = (WIN_Y_OFFSET + y)*8*gfx_scale;
  SDL_FillRect (screen, &rect, color);
}
#endif
  
void clear_square(unsigned char x, unsigned char y) {
#ifdef __GBA__
  // clears a square in the arena
  static u16 zero = 0;
  u16 startOAM = 16+x*2*16+2*y*30*16;
  // bug here in -O3 ? 
  // clear 
  
  REG_DMA3SAD = (u32)&zero; // 16 bit reads into VRAM
  REG_DMA3DAD = (u32)&bg_level.tileData[startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_SOURCE_FIXED|DMA_16NOW; // 16 bit transfers from gfx array via DMA  
  
  startOAM = 16+x*2*16+(2*y+1)*30*16; // x*2*16+(2*y+1)*30*16;
  REG_DMA3SAD = (u32)&zero; // 16 bit reads into VRAM
  REG_DMA3DAD = (u32)&bg_level.tileData[startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_SOURCE_FIXED|DMA_16NOW; // 16 bit transfers from gfx array via DMA  
#elif defined( __WINDOWS__ )
  // clear the square 
  clear_square8(x*2, y*2);
#endif  
  
}



#ifdef __GBA__
#define BG_CORNER_TILE 601
#define BG_VERT_EDGE_TILE 602
#define BG_HORZ_EDGE_TILE 603
#define BG_SOLID_TILE 604
#define BG_PALETTE (11<<12)
#endif  
#ifdef __WINDOWS__
#define BG_CORNER_TILE 0
#define BG_VERT_EDGE_TILE 1
#define BG_HORZ_EDGE_TILE 2
#define BG_SOLID_TILE 3
#define BG_PALETTE (11)

#ifdef __MINGW32__
#define tileData binary_bg_raw_start
#else
#define tileData _binary_bg_raw_start
#endif
extern u8 tileData[]; 
#endif  
 
void draw_game_border(void) {
  
  u16 x, y;
#ifdef __GBA__  
  // the second bank of 256*256 start at tile 32*32...
  u16 SECOND_TILEBANK_0 = 32*32;
  
  // draws the game border, which is tiles in bg
  // place the tiles at the end of the creature data, i.e. 150 creatures, 4 tiles each = 600
  // use tile 600 to 607 = 0x4b00 19200 to 19392 (= 32 bytes per tile
  // 
  REG_DMA3SAD = (u32)_binary_bg_raw_start;
  REG_DMA3DAD = (u32)&bg_level.tileData[16+(19200>>1)];
  REG_DMA3CNT = 64 |DMA_16NOW;
  
  // top left corner
  bg_level.mapData[0] = BG_CORNER_TILE|BG_PALETTE;
  // top right corner
  bg_level.mapData[SECOND_TILEBANK_0+1] = BG_CORNER_TILE|TILE_FLIP_HORZ|BG_PALETTE;
  // bottom left corner
  bg_level.mapData[23*32] = BG_CORNER_TILE|TILE_FLIP_VERT|BG_PALETTE;
  // bottom right corner
  bg_level.mapData[SECOND_TILEBANK_0+1+23*32] = BG_CORNER_TILE|TILE_FLIP_HORZ|TILE_FLIP_VERT|BG_PALETTE;
  
  // now fill in the sides
  // top / bottom edges
  for (x=1; x < 32; x++) {
    // top
    bg_level.mapData[x] = BG_HORZ_EDGE_TILE|BG_PALETTE;
    bg_level.mapData[x+32] = BG_SOLID_TILE|BG_PALETTE;
    // bottom edge
    bg_level.mapData[x+23*32] = BG_HORZ_EDGE_TILE|TILE_FLIP_VERT|BG_PALETTE;
    bg_level.mapData[x+22*32] = BG_SOLID_TILE|BG_PALETTE;
  }
  // left over top edge tile...
  bg_level.mapData[SECOND_TILEBANK_0+0] = BG_HORZ_EDGE_TILE|BG_PALETTE;
  // left over bottom edge tile...
  bg_level.mapData[SECOND_TILEBANK_0+23*32] = BG_HORZ_EDGE_TILE|TILE_FLIP_VERT|BG_PALETTE;
  
  // left and right edges
  for (y=1; y < 23; y++) {
    // left
    bg_level.mapData[y*32] = BG_VERT_EDGE_TILE|BG_PALETTE;
    bg_level.mapData[1+y*32] = BG_SOLID_TILE|BG_PALETTE;
    
    // right edge
    bg_level.mapData[SECOND_TILEBANK_0+1+y*32] = BG_VERT_EDGE_TILE|TILE_FLIP_HORZ|BG_PALETTE;
    bg_level.mapData[SECOND_TILEBANK_0+y*32] = BG_SOLID_TILE|BG_PALETTE;
    
  }
#elif defined( __WINDOWS__  )
  // for windows, draw the bg_raw to the screen directly 
  // 64*16 = 128*8 = 256*4bits  = 256 pixels = 4 tiles
  // the tiles are corner, vert, horiz, solid
  // need to explicitly set all palettes first
  if ( SDL_MUSTLOCK(screen) ) 
     SDL_LockSurface(screen);
  // top left corner
  setWinPalette(0,0,BG_PALETTE);
  draw8x8Tile(0,0,&tileData[BG_CORNER_TILE*32],0);  
  // top right corner
  setWinPalette(33,0,BG_PALETTE);
  draw8x8Tile(33,0,&tileData[BG_CORNER_TILE*32],TILE_FLIP_HORZ);  
  // bottom left corner
  setWinPalette(0,23,BG_PALETTE);
  draw8x8Tile(0,23,&tileData[BG_CORNER_TILE*32],TILE_FLIP_VERT);  
  // bottom right corner
  setWinPalette(33,23,BG_PALETTE);
  draw8x8Tile(33,23,&tileData[BG_CORNER_TILE*32],TILE_FLIP_HORZ|TILE_FLIP_VERT);  
  
  // now fill in the sides
  // top / bottom edges
  for (x=1; x < 32; x++) {
    // top
    setWinPalette(x,0,BG_PALETTE);
    setWinPalette(x,1,BG_PALETTE);
    draw8x8Tile(x,0,&tileData[BG_HORZ_EDGE_TILE*32],0);  
    draw8x8Tile(x,1,&tileData[BG_SOLID_TILE*32],0);  
    // bottom edge
    setWinPalette(x,23,BG_PALETTE);
    setWinPalette(x,22,BG_PALETTE);
    draw8x8Tile(x,23,&tileData[BG_HORZ_EDGE_TILE*32],TILE_FLIP_VERT);  
    draw8x8Tile(x,22,&tileData[BG_SOLID_TILE*32],0);  
  }
  // left over top edge tile...
  setWinPalette(32,0,BG_PALETTE);
  draw8x8Tile(32,0,&tileData[BG_HORZ_EDGE_TILE*32],0);  
  // left over bottom edge tile...
  setWinPalette(32,23,BG_PALETTE);
  draw8x8Tile(32,23,&tileData[BG_HORZ_EDGE_TILE*32],TILE_FLIP_VERT);  
  
  // left and right edges
  for (y=1; y < 23; y++) {
    // left
    setWinPalette(0,y,BG_PALETTE);
    setWinPalette(1,y,BG_PALETTE);
    draw8x8Tile(0,y,&tileData[BG_VERT_EDGE_TILE*32],0);  
    draw8x8Tile(1,y,&tileData[BG_SOLID_TILE*32],0);  
    
    // right edge
    setWinPalette(33,y,BG_PALETTE);
    setWinPalette(32,y,BG_PALETTE);
    draw8x8Tile(33,y,&tileData[BG_VERT_EDGE_TILE*32],TILE_FLIP_HORZ);  
    draw8x8Tile(32,y,&tileData[BG_SOLID_TILE*32],0);  
    
  }
  
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_UnlockSurface(screen);
#endif  
  
}


void clear_game_border(void) {
#ifdef __GBA__
  u16 x, y;
  // the second bank of 256*256 start at tile 32*32...
  u16 SECOND_TILEBANK_0 = 32*32;
  
  // clear the game border from the map data
  
  // top left corner
  bg_level.mapData[0] = 0;
  // top right corner
  bg_level.mapData[SECOND_TILEBANK_0+1] = 0;
  // bottom left corner
  bg_level.mapData[23*32] = 0;
  // bottom right corner
  bg_level.mapData[SECOND_TILEBANK_0+1+23*32] = 0;
  
  // now fill in the sides
  // top/bottom edge
  for (x=1; x < 32; x++) {
    //top
    bg_level.mapData[x] = 0;
    bg_level.mapData[x+32] = 0;
    //bottom
    bg_level.mapData[x+23*32] = 0;
    bg_level.mapData[x+22*32] = 0;
  }
  // left over top edge tile...
  bg_level.mapData[SECOND_TILEBANK_0+0] = 0;
  // left over bottom edge tile...
  bg_level.mapData[SECOND_TILEBANK_0+23*32] = 0;
  
  // left and right edge
  for (y=1; y < 23; y++) {
    //l
    bg_level.mapData[y*32] = 0;
    bg_level.mapData[1+y*32] = 0;
    //r
    bg_level.mapData[SECOND_TILEBANK_0+1+y*32] = 0;
    bg_level.mapData[SECOND_TILEBANK_0+y*32] = 0;
  }
#endif
}

#if defined(__GBA__)
#define X_LIMIT 29
#define Y_LIMIT 19
#elif defined(__WINDOWS__)
#define X_LIMIT 33 
#define Y_LIMIT 23
#endif

// draw a "decorative" border... within the given palette with the given colours
void draw_decor_border(unsigned char pal, unsigned short col1,  unsigned short col2) {
  
  // this border is smaller than the game border, goes from 0 to 30 and is only 1 deep
  
  u16 x, y;
  // set the colours
#ifdef __WINDOWS__ 
  BGPaletteMem[16*pal+2] = winCol(col1);
  BGPaletteMem[16*pal+1] = winCol(col2);
#endif
#ifdef __GBA__
  BGPaletteMem[16*pal+2] = col1;
  BGPaletteMem[16*pal+1] = col2;
  REG_DMA3SAD = (u32)_binary_bg_raw_start;
  REG_DMA3DAD = (u32)&bg_level.tileData[16+(19200>>1)];
  REG_DMA3CNT = 64 |DMA_16NOW;
  wait_vsync_int();
  // top left corner
  bg_level.mapData[0] = BG_CORNER_TILE|(pal<<12);
  // top right corner
  bg_level.mapData[X_LIMIT] = BG_CORNER_TILE|TILE_FLIP_HORZ|(pal<<12);
  // bottom left corner
  bg_level.mapData[Y_LIMIT*32] = BG_CORNER_TILE|TILE_FLIP_VERT|(pal<<12);
  // bottom right corner
  bg_level.mapData[X_LIMIT+Y_LIMIT*32] = BG_CORNER_TILE|TILE_FLIP_HORZ|TILE_FLIP_VERT|(pal<<12);
  // now fill the sides
  // top edge
  for (x=1; x < X_LIMIT; x++) {
    bg_level.mapData[x] = BG_HORZ_EDGE_TILE|(pal<<12);
  }
  
  // bottom edge
  for (x=1; x < X_LIMIT; x++) {
    bg_level.mapData[x+Y_LIMIT*32] = BG_HORZ_EDGE_TILE|TILE_FLIP_VERT|(pal<<12);
    
  }
  
  // left edge
  for (y=1; y < Y_LIMIT; y++) {
    bg_level.mapData[y*32] = BG_VERT_EDGE_TILE|(pal<<12);
  }
  
  // right edge
  for (y=1; y < Y_LIMIT; y++) {
    bg_level.mapData[X_LIMIT+y*32] = BG_VERT_EDGE_TILE|TILE_FLIP_HORZ|(pal<<12);
  }
  bg_level.x_scroll = 0;
  bg_level.y_scroll = 0; 
  UpdateBackground(&bg_level);
  
#elif defined(__WINDOWS__)
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_LockSurface(screen);
  // top left corner
  setWinPalette(0,0,pal);
  draw8x8Tile(0,0,&tileData[BG_CORNER_TILE*32],0);  
  // top right corner
  setWinPalette(X_LIMIT,0,pal);
  draw8x8Tile(X_LIMIT,0,&tileData[BG_CORNER_TILE*32],TILE_FLIP_HORZ);  
  // bottom left corner
  setWinPalette(0,Y_LIMIT,pal);
  draw8x8Tile(0,Y_LIMIT,&tileData[BG_CORNER_TILE*32],TILE_FLIP_VERT);  
  // bottom right corner
  setWinPalette(X_LIMIT,Y_LIMIT,pal);
  draw8x8Tile(X_LIMIT,Y_LIMIT,&tileData[BG_CORNER_TILE*32],TILE_FLIP_HORZ|TILE_FLIP_VERT);  
  
  // now fill in the sides
  // top/bottom edges
  for (x=1; x < X_LIMIT; x++) {
    // top
    setWinPalette(x,0,pal);
    draw8x8Tile(x,0,&tileData[BG_HORZ_EDGE_TILE*32],0);  
    // bottom edge
    setWinPalette(x,Y_LIMIT,pal);
    draw8x8Tile(x,Y_LIMIT,&tileData[BG_HORZ_EDGE_TILE*32],TILE_FLIP_VERT);  
  }
  
  // left and right edges
  for (y=1; y < Y_LIMIT; y++) {
    // left
    setWinPalette(0,y,pal);
    draw8x8Tile(0,y,&tileData[BG_VERT_EDGE_TILE*32],0);  
    
    // right edge
    setWinPalette(X_LIMIT,y,pal);
    draw8x8Tile(X_LIMIT,y,&tileData[BG_VERT_EDGE_TILE*32],TILE_FLIP_HORZ);  
    
  }
  
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_UnlockSurface(screen);
#endif  
}
  
void load_all_palettes(void) {
  u8 i;
  
  for (i = 0; i < 10; i++) {
    load_bg_palette(i, i);
  }
  for (i = 0; i < 8; i++) {
    set_player_col(i, players[i].colour);  
  }
  
  BGPaletteMem[0] = 0;
  
}


void load_bg_palette(u8 topal, u8 frompal) {
  u16 palindex = topal*16;
#ifdef __GBA__  
  REG_DMA3SAD = (u32)PALETTES[frompal];
  REG_DMA3DAD = (u32)&BGPaletteMem[palindex];
  REG_DMA3CNT = 16 |DMA_16NOW;
#endif  
  
#ifdef __WINDOWS__ 
  int i;
  for (i = 0; i < 16; i++) {
    BGPaletteMem[palindex + i] = winCol(PALETTES[frompal][i]);
  } 
#endif  
}


void load_sprite_palette(void) {
  int i;
#ifdef __GBA__
  REG_DMA3SAD = (u32)&SPRITEPALETTE_DATA[0];
  REG_DMA3DAD = (u32)&OBJPaletteMem[0];
  REG_DMA3CNT = 128 |DMA_16NOW;
  for (i = 0; i < 128; i++)
    remove_sprite(i);
    
#elif defined (__WINDOWS__)
  int col;
  for (i = 0; i < 16; i++) { 
    col = SPRITEPALETTE_DATA[i*2]|(SPRITEPALETTE_DATA[1+i*2]<<8);
    SpritePaletteMem[i] = winCol(col);
  }
#endif
  
}


void load_sprite_data(u32 spriteid, u32 nFrame) {
  // load a sprite from the sprite arrays
  // check we are in bounds
  
#ifdef __GBA__
  const struct QGFX_ROOT* root = &CHAOS_SPRITES_ROOT;
  if (spriteid > root->nGFXs)
    return;
  
  u16 i;
  // get the size of the object and from this work out how many tiles need to be read in
  //k = <start of frame in map file> = Frame*number of tiles per sprite
  // tile count = (sprites*width*height / 64)
  u16 nTileCount = (root->pSpriteTable[spriteid].pSpriteMap->nSpriteCount *
      root->pSpriteTable[spriteid].pSpriteMap->nWidth *
      root->pSpriteTable[spriteid].pSpriteMap->nHeight) >> 6;


  u16 k = nFrame*nTileCount*2;  // 2 because the map is 16 bit and we need to skip every other value

  u16 id;
  u16 index = k;

  // need to load it in blocks of 64...
  for (i = 0; i < nTileCount; i++) {
    // id = <the tile id in the map file at this position>
    id = root->pSpriteTable[spriteid].pSpriteMap->pGFXMap[index];
    index += 2;
    REG_DMA3SAD = (u32)&(root->pSpriteTable[spriteid].pSpriteMap->pGFX[id*64]); // 8 bit reads into gfx array
    REG_DMA3DAD = (u32)&OAMData[root->pSpriteTable[spriteid].pSpriteMap->nOAM+i*32]; // 16 bit reads into VRAM
    REG_DMA3CNT = 32 |DMA_16NOW; // 16 bit transfers from gfx array via DMA

  }
  
#endif
}

void load_sprite(u32 spriteid, u32 copyOAM) {
  // create the sprites .. start at the offset given by the OAM value/16...
#ifdef __GBA__
  const struct QGFX_ROOT* root = &CHAOS_SPRITES_ROOT;
  
  u16 i;
  u16 offset = root->pSpriteTable[spriteid].pSpriteMap->nOAM >> 4;
  s16 temp_x = 0;
  s16 temp_y = 0;

  u16 max_temp_x = 0;
  u16 increment = 0;
  if (root->pSpriteTable[spriteid].pSpriteMap->nSpriteCount > 1) {
    max_temp_x = root->pSpriteTable[spriteid].pSpriteMap->nWidth*
    root->pSpriteTable[spriteid].pSpriteMap->nSpriteCount>>1;
    increment = root->pSpriteTable[spriteid].pSpriteMap->nWidth;
  }

  if (root->pSpriteTable[spriteid].pSpriteMap->nSpriteCount == 1) {
    temp_x = 0;
    temp_y = 0;
  }
  // how many 8 by 8 tiles make up each sprite?
  u16 tilecount88 = root->pSpriteTable[spriteid].pSpriteMap->nWidth *
      root->pSpriteTable[spriteid].pSpriteMap->nHeight >> 6;

  for (i = 0; i < root->pSpriteTable[spriteid].pSpriteMap->nSpriteCount; i++) {
    sprites[root->pSpriteTable[spriteid].pSpriteMap->nSpriteID+i].attribute0 = root->pSpriteTable[spriteid].pSpriteMap->nAttribute0;
    sprites[root->pSpriteTable[spriteid].pSpriteMap->nSpriteID+i].attribute1 = root->pSpriteTable[spriteid].pSpriteMap->nAttribute1;

    sprites[root->pSpriteTable[spriteid].pSpriteMap->nSpriteID+i].attribute2 = PRIORITY(1)|(offset+i*2*tilecount88);
    MoveSprite(&sprites[root->pSpriteTable[spriteid].pSpriteMap->nSpriteID+i],
      root->pSpriteTable[spriteid].pSpriteMap->nX + temp_x,
      root->pSpriteTable[spriteid].pSpriteMap->nY+ temp_y);

    if (temp_x > max_temp_x) {
      temp_x = 0;
      temp_y +=root->pSpriteTable[spriteid].pSpriteMap->nHeight;
    }
    else
      temp_x += increment;
  }
  if (copyOAM)
    CopyOAM();/*root->pSpriteTable[spriteid].pSpriteMap->nSpriteID, 
                    root->pSpriteTable[spriteid].pSpriteMap->nSpriteID
                    +root->pSpriteTable[spriteid].pSpriteMap->nSpriteCount);*/
#endif
}


#ifdef __GBA__
#if 0
void CopyOAM(unsigned char from, unsigned char to)
{
// some error checking?
//  signed short int* temp;
//  temp = (unsigned short int*)sprites;
//
  unsigned short int loop;
  unsigned short int* temp;
  temp = (unsigned short int*)sprites;
  for(loop = 0; loop < 4*4; loop++)
  {
  	OAM[loop] = temp[loop];
  }

}

void MoveSprite(OAMEntry* sp, signed short int x, signed short int y)
{
    if(x < 0)			//if it is off the left corect
		x = 512 + x;
	if(y < 0)			//if off the top corect
		y = 256 + y;

    /* because x is 9 bits, we need to clear out bits 10-15,
	 so we dont mess up any rotation data */
    x &= 0x01FF;
	sp->attribute1 = sp->attribute1 & 0xFE00;  //clear the old x value
	sp->attribute1 = sp->attribute1 | x;

	sp->attribute0 = sp->attribute0 & 0xFF00;  //clear the old y value
	sp->attribute0 = sp->attribute0 | y;

}
#endif
#endif

void draw_cursor(u32 cursorid) {
  // draw the given cursor at the given coordinates...
  load_sprite_data(cursorid, 0);
  load_sprite(cursorid, 0);
#ifdef __WINDOWS__
  g_cursor_type = cursorid;
#endif
}


//////////////////////////////////////////// 


void draw_wizard8(unsigned long x, unsigned long y, unsigned long wizardid, 
  unsigned long frame, unsigned long playerid) {
  set_palette8(x,y,9);
  
  
  u8 pNewGfx[128];
  
  u16 nTileCount = 4; // 4 tiles for each wizard
  u16 k = frame*nTileCount*2;  // 2 because the map is 16 bit and we need to skip every other value
  
  u16 id;
  u16 index = k;
  
  // need to load it in blocks of 16 for 4bpp gfx
  u16 loop = 0;
  
  // copy the actual graphics into the temp array
  for (loop = 0; loop < 4; loop++) {
    // id = <the tile id in the map file at this position>
    id = WizardGFX[wizardid].pWizardData->pMap[index];
    index += 2;    
#ifdef __GBA__
    REG_DMA3SAD = (u32)&(WizardGFX[wizardid].pWizardData->pGFX[id*32]); // 8 bit reads into gfx array
    REG_DMA3DAD = (u32)&(pNewGfx[loop*32]); // 16 bit reads into VRAM
    REG_DMA3CNT = 16 |DMA_16NOW; // 16 bit transfers from gfx array via DMA 
#elif defined(__WINDOWS__)
    for (k = 0; k < 32; k++)
      pNewGfx[(loop*32)+k] = WizardGFX[wizardid].pWizardData->pGFX[k+(id*32)];
#endif
  }
  
  // replace the temp array index WIZARD_COLOUR (which is the colour that should be replaced)
  // with index playerid, which is the colour index for this player.
  u8 thisVal;
  u8 defaultCol_l = WIZARD_COLOUR;
  u8 defaultCol_h = WIZARD_COLOUR<<4;
  u8 newCol_l = (u8)playerid+8;
  u8 newCol_h = (u8)(playerid+8)<<4;
  for (loop = 0; loop < 128; loop++) {
    thisVal = pNewGfx[loop];
    
    // 4 bits per pixel...
    // mask the lower 4 bits
    if ((thisVal & 0x0f) == defaultCol_l) {
      thisVal &= ~defaultCol_l;
      thisVal |= newCol_l;
    }
    // mask the higher 4 bits
    if ((thisVal & 0xf0) == defaultCol_h) {
      thisVal &= ~defaultCol_h;
      thisVal |= newCol_h;
    }
    pNewGfx[loop] = thisVal;
  }
#ifdef __GBA__
  // each 8*8 tile takes up 32 bytes.
  u16 startOAM = 16+x*16+y*30*16;
  REG_DMA3SAD = (u32)&(pNewGfx[0]); // 8 bit reads into gfx array
  REG_DMA3DAD = (u32)&bg_level.tileData[startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  
  startOAM = 16+x*16+(y+1)*30*16 - 32; // x*2*16+(2*y+1)*30*16;
  REG_DMA3SAD = (u32)&(pNewGfx[64]); // 8 bit reads into gfx array
  REG_DMA3DAD = (u32)&bg_level.tileData[32+startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  
#elif defined(__WINDOWS__)
  draw8x8Tile(WIN_X_OFFSET+x, WIN_Y_OFFSET+y,     &pNewGfx[0], 0);
  draw8x8Tile(WIN_X_OFFSET+x+1, WIN_Y_OFFSET+y,   &pNewGfx[32], 0);
  draw8x8Tile(WIN_X_OFFSET+x, WIN_Y_OFFSET+y+1,   &pNewGfx[64], 0);
  draw8x8Tile(WIN_X_OFFSET+x+1, WIN_Y_OFFSET+y+1, &pNewGfx[96], 0);
  
#endif
  
}
  

void draw_wizard(unsigned long x, unsigned long y, unsigned long wizardid, 
  unsigned long frame, unsigned long playerid) {
  // draw the given wizard id... 
  // take from list of wizard gfx pointers
  // WizardGFX[wizardid].pWizardData->pGFX is the pointer to the graphics
  // WizardGFX[wizardid].pWizardData->pMap is the pointer to the map file
  // to draw a given frame use similar code to the creature drawing routine
  
  if ( (players[playerid].modifier_flag & 0x8) && frame == 3) {
    // shadow form, do not draw this frame
    clear_square(x,y);
    return;
  }
  
  draw_wizard8(x<<1, y<<1, wizardid,frame,playerid);
  
}


void set_palette(unsigned short x, unsigned short y, unsigned char palette) {
  // sets the arena square's palette index...
  // mask out the old palette and set the current one
  // get the tile ids for the 4 gba tiles at the ARENA x,y
  
  set_palette8(x*2, y*2, palette);

      
}

void set_palette8(unsigned short x, unsigned short y, unsigned char palette) {
  // 8 bit resolution, 4 square size setting...
  // mask out the old palette and set the current one
  // get the tile ids for the 4 gba tiles at the ARENA x,y
  u16 tile[4];
#ifdef __GBA__

  
  // 66 comes from the fact that there are 2 rows before hand (32 *2) and 2 blocks
  // at the start of each row
  // x = 0 = 132
  tile[0] = 66+x+y*32;        tile[1] = 66+1+x+y*32;
  tile[2] = tile[0]+32;         tile[3] = tile[1]+32;
  
  bg_level.mapData[tile[0]] &= ~0xF000; bg_level.mapData[tile[0]] |= (palette<<12)&0xF000;
  bg_level.mapData[tile[1]] &= ~0xF000; bg_level.mapData[tile[1]] |= (palette<<12)&0xF000;
  bg_level.mapData[tile[2]] &= ~0xF000; bg_level.mapData[tile[2]] |= (palette<<12)&0xF000;
  bg_level.mapData[tile[3]] &= ~0xF000; bg_level.mapData[tile[3]] |= (palette<<12)&0xF000;
#elif defined(__WINDOWS__)
  x+=WIN_X_OFFSET;
  y+=WIN_Y_OFFSET;
  tile[0] = x+y*WIN_SCRN_X;        tile[1] = 1+x+y*WIN_SCRN_X;
  tile[2] = tile[0]+WIN_SCRN_X;         tile[3] = tile[1]+WIN_SCRN_X;
  
  g_tilePalette8[tile[0]] = palette;
  g_tilePalette8[tile[1]] = palette;
  g_tilePalette8[tile[2]] = palette;
  g_tilePalette8[tile[3]] = palette;

#endif
      
}


void set_player_col(unsigned char playerid, unsigned short colour) 
{
  // set a player to be this colour
  if (playerid < 8) {
#ifdef __GBA__
    BGPaletteMem[16*9+playerid+8] = colour;
#elif defined( __WINDOWS__  )
    BGPaletteMem[16*9+playerid+8] = winCol(colour);
#endif  
  }
}

void set_player_col_RGB(unsigned char playerid, unsigned short red, unsigned short green, unsigned short blue) {
  if (playerid < 8) {
    set_player_col(playerid, RGB16(red, green, blue));
  }
}


void set_border_col(unsigned char playerid) {
  // set the border colour based on the player colour
  if (playerid < 8) {
#ifdef __GBA__
    u16 col = BGPaletteMem[16*9+playerid+8];
    s8 red = col & 0x1F;
    s8 green = (col>>5) & 0x1F;
    s8 blue = (col>>10) & 0x1F;
    red -= 8;
    green -= 8;
    blue -= 8;
    if (red < 0) red = 0;
    if (green < 0) green = 0;
    if (blue < 0) blue = 0;
    
#elif defined( __WINDOWS__)
    u32 col = BGPaletteMem[16*9+playerid+8];
    Uint8 red, green, blue;
    SDL_GetRGB(col, screen->format, &red, &green, &blue);
    if ( (red - 64) > 0) red -= 64;
    if ( (green - 64) > 0) green -= 64;
    if ( (blue - 64) > 0) blue -= 64;
#endif
    
    
#ifdef __GBA__
    BGPaletteMem[16*11+2] = col;
    BGPaletteMem[16*11+1] = RGB16(red,green, blue);
#elif defined (__WINDOWS__)
    BGPaletteMem[16*11+2] = col;
    BGPaletteMem[16*11+1] = SDL_MapRGB (screen->format, red,green, blue);
    
    draw_game_border();
#endif
    
  }
    
}

void clear_palette(unsigned char pal) {
  if (pal > 15)
    return;
  u8 i;
  for (i = 0; i < 16; i++) {
    BGPaletteMem[pal*16 + i] = 0x0000;
  }
}


void clear_palettes(void) {
  u16 i;
  for (i = 0; i < 16; i++) {
    clear_palette(i);
  }
}

// this is a bit of a problem... just changes the palette
// on win, how to update the text?
// could search palette info, get the square with this palette
// then redraw it in place...
void anim_selection(u8 pal, u8 red, u8 green, u8 blue) {
  s8 r = red;
  s8 g = green;
  s8 b = blue;
  
  r = r + anim_col;
  g = g + anim_col;
  b = b + anim_col;
  
  r = r>31?31:r;
  g = g>31?31:g;
  b = b>31?31:b;
  
  r = r<0?0:r;
  g = g<0?0:g;
  b = b<0?0:b;
  
  
  set_text16_colour(pal, RGB16(r,g,b));
  
  anim_col += anim_col_grad;
  
  if (anim_col >= 31) {
    anim_col_grad = -8;
    anim_col = 31;
  } else if (anim_col <= -15) {
    anim_col_grad = 8;
    anim_col = -15;
  }
  
}


#ifdef __WINDOWS__  
#define ROW_WIDTH 300
#endif 
void draw_gfx8(const u8 * gfx, const u8 * map, u8 x, u8 y, u8 frame) {
  // draws graphics on the 8 by 8 tile boundaries
  // ie, to draw on 16*16 tiles, multiply x, y by 2
  u16 nTileCount = 4; // 4 tiles for each 16*16 gfx 
  u16 index = frame*nTileCount*2;  // 2 because the map is 16 bit and we need to skip every other value
  u16 id;
  u8 loop = 0;
#ifdef __GBA__  
  // each 8*8 tile takes up 32 bytes.
  u16 startOAM = 16+x*16+y*30*16;
#endif  
  // get start address in screen
#ifdef __WINDOWS__  
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_LockSurface(screen);
#endif    
  for (loop = 0; loop < 2; loop++) {
    // id = <the tile id in the map file at this position>
    id = map[index];
    index += 2;    
#ifdef __WINDOWS__  
    draw8x8Tile(WIN_X_OFFSET+x+loop, WIN_Y_OFFSET+y, &gfx[id*32], 0); 

#elif defined(__GBA__)
    REG_DMA3SAD = (u32)&(gfx[id*32]); // 8 bit reads into gfx array
    REG_DMA3DAD = (u32)&bg_level.tileData[loop*16+startOAM]; // 16 bit reads into VRAM
    REG_DMA3CNT = 16 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  

#endif  

  }
  
#ifdef __GBA__  
  startOAM = 16+x*16+(y+1)*30*16 - 32;
#endif  
  for (loop = 2; loop < 4; loop++) {
    // id = <the tile id in the map file at this position>
    id = map[index];
    index += 2;
#ifdef __WINDOWS__  
    draw8x8Tile(WIN_X_OFFSET+x+(loop-2), WIN_Y_OFFSET+y+1, &gfx[id*32], 0); 
#elif defined ( __GBA__ )
    REG_DMA3SAD = (u32)&(gfx[id*32]); // 8 bit reads into gfx array
    REG_DMA3DAD = (u32)&bg_level.tileData[loop*16+startOAM]; // 16 bit reads into VRAM
    REG_DMA3CNT = 16 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  
#endif
  }
#ifdef __WINDOWS__  
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_UnlockSurface(screen);
#endif  
  
}

 
void draw_gfx(const u8 * gfx, const u8 * map, u8 x, u8 y, u8 frame) {
  draw_gfx8(gfx, map, x*2, y*2, frame);

  
}


void draw_fight_frame(u8 x, u8 y, u8 frame) {
  
  set_palette(x,y,8);
  draw_gfx(FIGHT_GFX_DATA, FIGHT_MAP_DATA, x, y, frame);
//  BGPaletteMem[16*10+1] = RGB16(30,30,0);
  
  
}


void draw_spellcast_frame(u8 x, u8 y, u8 frame) {
  
  set_palette(x,y,0);
  draw_gfx(SPELL_GFX_DATA, SPELL_MAP_DATA, x, y, frame);
  
//  BGPaletteMem[16*10+1] = RGB16(30,30,0);
  
}


void draw_splat_frame(u8 x, u8 y, u8 frame) {
    
  set_palette(x,y,8);
  draw_gfx(SPLAT_GFX_DATA, SPLAT_MAP_DATA, x, y, frame);
  
}

void draw_pop_frame(u8 x, u8 y, u8 frame) {
    
  set_palette(x,y,8);
  draw_gfx(POP_GFX_DATA, POP_MAP_DATA, x, y, frame);
  
}

void draw_breath_frame(u8 x, u8 y, u8 frame) {
    
  set_palette(x,y,8);
  draw_gfx(DRAGONBREATH_GFX_DATA, DRAGONBREATH_MAP_DATA, x, y, frame);
  
}


void remove_cursor(void) {

#ifdef __GBA__
  REG_DISPCNT &= ~OBJ_ENABLE;
#elif defined(__WINDOWS__)

  g_cursor_on = 0;
  
#endif
}

void redraw_cursor(void) {
#ifdef __GBA__
  wait_vsync_int();
  REG_DISPCNT |= OBJ_ENABLE;
#elif defined (__WINDOWS__)
  g_cursor_on = 1;
#endif

}

#if defined (__WINDOWS__)
void draw_win_cursor(void) {
  // draw the cursor gfx!
  const struct QGFX_ROOT* root = &CHAOS_SPRITES_ROOT;
  if ((g_cursor_type > root->nGFXs) || !g_cursor_on)
    return;
  
  int i, id;
  int index = 0;
  // get the size of the object and from this work out how many tiles need to be read in
  //k = <start of frame in map file> = Frame*number of tiles per sprite
  // tile count = (sprites*width*height / 64)
  /*
  // attempt to emulate colour clash...   
  if (arena[0][cursor_x+cursor_y*16] > 2) {
    int tile = 2*cursor_x+WIN_X_OFFSET + (2*cursor_y+WIN_Y_OFFSET)*WIN_SCRN_X;
    int pal = g_tilePalette8[tile];
    for (i =0; i < 16; i++) 
      SpritePaletteMem[i] = BGPaletteMem[16*pal + i];
  } else {
    load_sprite_palette();
  }
  */
  for (i = 0; i < 2; i++) {
    // id = <the tile id in the map file at this position>
    id = root->pSpriteTable[g_cursor_type].pSpriteMap->pGFXMap[index];
    index += 2;
    draw8x8SpriteTile(2*cursor_x+WIN_X_OFFSET+i, 2*cursor_y+WIN_Y_OFFSET, 
        &(root->pSpriteTable[g_cursor_type].pSpriteMap->pGFX[id*64]),0);
      
  }
  for (i = 0; i < 2; i++) {
    // id = <the tile id in the map file at this position>
    id = root->pSpriteTable[g_cursor_type].pSpriteMap->pGFXMap[index];
    index += 2;
    draw8x8SpriteTile(2*cursor_x+WIN_X_OFFSET+i, 2*cursor_y+WIN_Y_OFFSET+1, 
        &(root->pSpriteTable[g_cursor_type].pSpriteMap->pGFX[id*64]),0);
      
  }
}
#endif


#ifdef __WINDOWS__
int get_win_tile(int x, int y) {
  // 120 67
  // 15  8
  // 17  10
  return (x>>3)+WIN_X_OFFSET + ( (y>>3)+WIN_Y_OFFSET)*WIN_SCRN_X;
}
void clear_pixel(int x, int y) {
  if ( (x > WIN_SCRN_X*8) || (y > WIN_SCRN_Y*8))
    return;
/*
  u8 * pixels = (u8*) screen->pixels;
  u16* vidPtr = (u16 *)&pixels[(x+WIN_X_OFFSET*8)*2 + (y+WIN_Y_OFFSET*8)*screen->w*2];
  *vidPtr = 0;
*/
  SDL_Rect rect;
  rect.w = 1*gfx_scale;
  rect.h = 1*gfx_scale;
  rect.x = (x+8*WIN_X_OFFSET)*gfx_scale;
  rect.y = (y+8*WIN_Y_OFFSET)*gfx_scale;
  SDL_FillRect (screen, &rect, BGPaletteMem[0]); 
}
#endif


void draw_pixel_4bpp(unsigned short x, unsigned short y) {
  // draw a pixel to the non rot bg, assume the tiles are set up already
  // use "1" as the colour
  
#ifdef __GBA__
  // convert the x position to the x,y position to the 8*8 tile
  // each tile is 8 pixels wide, but these are stored in quadruples 
  // (pairs of 8 bit values, each 8 bit value holds 2 pixels)
  // therefore each 8*8 block is just 16 "reads" big.
  // 16 * 16 = 256 bits. We have 4 bits per pixel, so 256 bits = 64 pixels
  
  // the pixel could be either one of the 4 4bit blocks 
  //       1111 0000 0000 0000    F000 -> 0FFF
  //       0000 1111 0000 0000    0F00 -> F0FF
  //       0000 0000 1111 0000    00F0 -> FF0F
  //       0000 0000 0000 1111    000F -> FFF0
  // to determine the 4 bit block, need to think about the pixel.
  // pixel 0 4 is in lower 4 bits   00  100     &3 == 0
  // pixel 1 5 is in next 4 bits    01  101     &3 == 1
  // pixel 2 6 is in next 4 bits    10  110     &3 == 2
  // pixel 3 7 is in next 4 bits    11  111     &3 == 3
  
  /* tile     add           x at top left corner      y
      1         20          0                         0
      2         40          8                         0
      3         60...       16                        0 
      31        3E0         0                         8
      32        400         8                         8....
      
      so to get tile from x,y... need to 1 + (y/8 * 30 + x/8)
  */
  
  u16 tile = 1 + (x/8) + ((y/8)*30);
  // then from here it's easy (ish) just work out the remainder from /8
  //u8 xrem = x - ((x>>3)<<3); //&7!
  //u8 yrem = y - ((y>>3)<<3); //&7!

  int xrem = x&7;
  int yrem = y&7;
  
  u16 address = tile*0x10 /*16 16-bit vals per tile*/ + (xrem/4) + (yrem*2);
  
  u16 pixel = bg_level.tileData[address];
  if ((x & 3) == 0) {
    // lower bits are removed...
//    pixel &= 0xFFF0;
    pixel |= 0x000F;
  } else if ((x & 3) == 1) {
    
//    pixel &= 0xFF0F;
    pixel |= 0x00F0;
  } else if ((x & 3) == 2) {    
//    pixel &= 0xF0FF;
    pixel |= 0x0F00;
  } else if ((x & 3) == 3) {    
//    pixel &= 0x0FFF;
    pixel |= 0xF000;
  }
  
  bg_level.tileData[address] = pixel;
#elif defined (__WINDOWS__)
  //u8 * pixels = (u8*)screen->pixels;
  //u16* vidPtr = (u16 *)&pixels[(x+WIN_X_OFFSET*8)*2 + (y+WIN_Y_OFFSET*8)*screen->w*2];
  // which tile is that?
  int tile = get_win_tile(x,y);
  //printf("x %d y %d tile %d \n", x, y, tile);
  int pal = g_tilePalette8[tile];
  //*vidPtr = BGPaletteMem[16*pal + 0xf];
  SDL_Rect rect;
  rect.w = 1*gfx_scale;
  rect.h = 1*gfx_scale;
  rect.x = (x+8*WIN_X_OFFSET)*gfx_scale;
  rect.y = (y+8*WIN_Y_OFFSET)*gfx_scale;
  SDL_FillRect (screen, &rect, BGPaletteMem[16*pal + 0xf]); 
#endif
}


unsigned char is_pixelset(unsigned short x, unsigned short y, int target, int start) {
  // check a pixel is set in the non rot bg

  // fix for disappearing fire/ghost/wraith
  int chaos_tile = x/16 + (y/16)*16;
  int creature = arena[0][chaos_tile];
  int dead = (arena[2][chaos_tile] == 4);
  if (chaos_tile == target || chaos_tile == start ||
      creature < SPELL_KING_COBRA ||
      creature == SPELL_WRAITH || creature == SPELL_GHOST ||
      creature == SPELL_MAGIC_FIRE || dead) {
    return 0;
  }
  
#ifdef __GBA__
  u16 tile = 1 + (x/8) + ((y/8)*30);
  // then from here it's easy (ish) just work out the remainder from /8
  u8 xrem = x&0x7;
  u8 yrem = y&0x7;
  
  u16 address = tile*0x10 /*16 16-bit vals per tile*/ + (xrem/4) + (yrem*2);
  
  u16 pixel = bg_level.tileData[address];
  if ((x & 3) == 0) {
    return  ((pixel & 0x000F) != 0);
  } else if ((x & 3) == 1) {
    return  ((pixel & 0x00F0) != 0);
  } else if ((x & 3) == 2) {    
    return  ((pixel & 0x0F00) != 0);
  } else { //if ((x & 3) == 3) {    
    return  ((pixel & 0xF000) != 0);
  }
#elif defined(__WINDOWS__)
  // no other way to do this but check "video ram"
  u8* pixels = (u8*)screen->pixels; 
  u16* vidPtr = (u16 *)&pixels[(x+WIN_X_OFFSET*8)*2*gfx_scale + (y+WIN_Y_OFFSET*8)*screen->w*2*gfx_scale];
  return (*vidPtr != 0);
#endif
}

// draw a silhoutte style gfx thing at the given location
// used for the justice cast
// passing in 1 for negative draws in the reversed "hilighted" style
void draw_silhouette_gfx(u8 arena_index, const u8 * gfx, const u8 * map,
    u16 col, int palette,
    u8 negative) {
  u8 x,y;
  get_yx(arena_index, &y, &x);
  x--;
  y--;
  
  
  
  u8 pNewGfx[128];
  
  u16 nTileCount = 4; // 4 tiles for each gfx
  
  u16 id;
  u16 index = arena[2][arena_index]*nTileCount*2; // set the frame...
  if (palette == -1) {
    palette = CHAOS_SPELLS.pSpellDataTable[arena[0][arena_index]]->palette;
  }
  
  set_palette(x,y,palette);
  if (palette == 11) {
#ifdef __GBA__
    BGPaletteMem[16*11+0xf] = col;
#elif defined (__WINDOWS__)
    BGPaletteMem[16*11+0xf] = winCol(col);
#endif
    col = 0xf;
  }
  // need to load it in blocks of 16 for 4bpp gfx
  int loop = 0;
  // copy the actual graphics into the temp array
  for (loop = 0; loop < 4; loop++) {
    // id = <the tile id in the map file at this position>
    id = map[index];
    index += 2;   
#ifdef __WINDOWS__
    int i;
    for (i = 0; i < 32; i++)
      pNewGfx[loop*32+i] = gfx[id*32+i];
#elif defined(__GBA__)
    REG_DMA3SAD = (u32)&(gfx[id*32]); // 8 bit reads into gfx array
    REG_DMA3DAD = (u32)&(pNewGfx[loop*32]); // 16 bit reads into VRAM
    REG_DMA3CNT = 16 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  
#endif
  }
  
  // replace in the temp array any non zero gfx values with the silhoutte value F
  // use palette 11 - the border palette, but the Fth entry isn't used
  
  u8 thisVal;
  int lowMask =  col&0xf;
  int hiMask =  (col&0xf)<<4;
  
  for (loop = 0; loop < 128; loop++) {
    thisVal = pNewGfx[loop];
    
    // 4 bits per pixel...
    // mask the lower 4 bits
    if (negative == 0) {
      // draw solid withno pixel detail
      if ((thisVal & 0x0f) != 0) {
        thisVal |= lowMask;
      }
    } else {
      // draw inversed
      if ((thisVal & 0x0f) == 0) {
        thisVal |= lowMask;
      }else if ((thisVal & 0x0f) != 0) {
        thisVal &= 0xf0;
      }
    }
    
    
    // mask the higher 4 bits
    if (negative ==0) {
      // draw solid withno pixel detail
      if ((thisVal & 0xf0) != 0) {
        thisVal |= hiMask;
      }
    } else {
      // draw inversed
      if ((thisVal & 0xf0) == 0) {
        thisVal |= hiMask;
      } else if ((thisVal & 0xf0) != 0) {
        thisVal &= 0x0f;
      }
    }
    
    pNewGfx[loop] = thisVal;
  }
  
#ifdef __GBA__ 
  // each 8*8 tile takes up 32 bytes.
  u16 startOAM = 16+x*2*16+2*y*30*16;
  
  REG_DMA3SAD = (u32)&(pNewGfx[0]); // 8 bit reads into gfx array
  REG_DMA3DAD = (u32)&bg_level.tileData[startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  
  
  
  startOAM = 16+x*2*16+(2*y+1)*30*16 - 32; // x*2*16+(2*y+1)*30*16;

  REG_DMA3SAD = (u32)&(pNewGfx[64]); // 8 bit reads into gfx array
  REG_DMA3DAD = (u32)&bg_level.tileData[32+startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_16NOW; // 16 bit transfers from gfx array via DMA  
#elif defined(__WINDOWS__)
  draw8x8Tile(WIN_X_OFFSET+x*2, WIN_Y_OFFSET+y*2,     &pNewGfx[0], 0);
  draw8x8Tile(WIN_X_OFFSET+x*2+1, WIN_Y_OFFSET+y*2,   &pNewGfx[32], 0);
  draw8x8Tile(WIN_X_OFFSET+x*2, WIN_Y_OFFSET+y*2+1,   &pNewGfx[64], 0);
  draw8x8Tile(WIN_X_OFFSET+x*2+1, WIN_Y_OFFSET+y*2+1, &pNewGfx[96], 0);
#endif
}

int getMainColour(int creatid, int frame) {
  // get the main colour for this spell
  if (creatid > SPELL_WALL || creatid < SPELL_KING_COBRA) {
    return 0;
  }
  if (frame > 3) {
    frame = 0;
  }
  int m, i, id;
  u16 pixelPair;
  
  u16 used[16];
  for (i = 0; i < 16; i++)
    used[i] = 0;
  // use the frame to get the current colour..
  const u8 * gfx = CHAOS_SPELLS.pSpellDataTable[creatid]->pGFX; 
  const u8 * map = CHAOS_SPELLS.pSpellDataTable[creatid]->pGFXMap;
  for (m = frame*8; m < ((frame+1)*8); m+=2) { 
    id = map[m];
    for (i = 0; i < 32; i++) {
      pixelPair = gfx[id*32+i];
      used[pixelPair&0xf]++;
      // half the work, same effect :)
      // Uncomment next line for more accuracy 
      // used[(pixelPair>>4)&0xf]++;
    }
  }
  int maxUsed = 0;
  int maxIndex = 1;
  // don't count 0, as that is transparent and most likely max used
  for (i = 1; i < 16; i++) {
    if (used[i] > maxUsed) {
      maxIndex = i;
      maxUsed = used[i];
    }
  }
  return maxIndex;
  
}


void fade_up(void) {
#ifdef __GBA__
  u8 loop;
  
  REG_BLDMOD = BLDMOD_FIRST_OBJ | BLDMOD_FIRST_BG1 | 
               BLDMOD_FIRST_BG0 | BLDMOD_SECOND_BD | BLDMOD_MODE(3);
  
  for (loop = 0; loop < 17; loop+=2) {
    wait_vsync_int();
    REG_COLEY = (16-loop);
  }
  wait_vsync_int();
  REG_COLEY = 0;
#endif
  fadedup = 1;
}     

void fade_down(void) {
#ifdef __GBA__
  u8 loop;
  REG_BLDMOD = BLDMOD_FIRST_BG1 | BLDMOD_FIRST_BG0 | 
               BLDMOD_FIRST_OBJ | BLDMOD_SECOND_BD | BLDMOD_MODE(3);
  
  for (loop = 0; loop < 17; loop+=2) {
    wait_vsync_int();
    REG_COLEY = loop;
  }
  wait_vsync_int();
  REG_COLEY = 16;
#else
#if 0
  int width = screen->w;
  int height = screen->h;
  Uint8 r, g, b
  Uint32 c;
  SDL_Rect rect;
  rect.w = gfx_scale;
  rect.h = gfx_scale;
  // for all pixels
  // get colour
  // make darker
  // put colour 
  int x, y;
  for (x = 0; x < width; x+= gfx_scale) {
    for (y = 0; y < width; y+= gfx_scale) {
    c = pixels[x+y*width];
    r = GetRed(c);
    g = GetGreen(c);
    b = GetBlue(c);
    if (r > 2*i)
      r -= 2*i;
    else
      r = 0;
    if (g > 2*i)
      g -= 2*i;
    else
      g = 0;
    if (b > 2*i)
      b -= 2*i;
    else
      b = 0;
    c = SDL_MapRGB(screen->format, r, g, b);
    rect.x = x;
    rect.y = y;
    SDL_FillRect (screen, &rect, c); 
  } 
  
#endif
  
#endif
  fadedup = 0;
}

// clears out the video memory, which may have been set already by the boot menu
#if 0
void clear_vram(void) {
#ifdef __GBA__
  u16 i;
  u8 zero = 0;
  // fade down
  REG_BLDMOD = BLDMOD_FIRST_BG1 | BLDMOD_FIRST_BG0 | 
               BLDMOD_FIRST_OBJ | BLDMOD_SECOND_BD | BLDMOD_MODE(3);
  REG_COLEY = 16;
  
  REG_DMA3SAD = (u32)&zero;
  REG_DMA3DAD = (u32)&(VideoBuffer[0]); // VRAM
  REG_DMA3CNT = 0x3100 |DMA_SOURCE_FIXED|DMA_16NOW; // 16 bit transfers from gfx array via DMA  
  
  for (i = 0; i < 256; i++) 
    BGPaletteMem[i] = 0;
#endif
  
  
}
#endif

#ifdef __WINDOWS__
void remove_win_cursor(void) {
  int i = cursor_x+cursor_y*16;

  if (arena[0][i] >= 2) {

    if (arena[0][i] < WIZARD_INDEX) {
      draw_creature(cursor_x,cursor_y, 
          arena[0][i],arena[2][i]);
    } else {
      draw_wizard(cursor_x, cursor_y,
          players[arena[0][i] - WIZARD_INDEX].image, 
          arena[2][i], arena[0][i] - WIZARD_INDEX);
    }
  } else {
      if (cursor_x > 0 && cursor_x < 0x10
          && cursor_y > 0 && cursor_y < 0xB)
        clear_square(cursor_x, cursor_y);
    }
}

void DrawPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(screen->format, R, G, B);
  switch (screen->format->BytesPerPixel)
  {
    case 1: // Assuming 8-bpp
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
        *bufp = color;
      }
      break;
    case 2: // Probably 15-bpp or 16-bpp
      {
        Uint16 *bufp;
        bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *bufp = color;
      }
      break;
    case 3: // Slow 24-bpp mode, usually not used
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        {
          bufp[0] = color;
          bufp[1] = color >> 8;
          bufp[2] = color >> 16;
        } else {
          bufp[2] = color;
          bufp[1] = color >> 8;
          bufp[0] = color >> 16;
        }
      }
      break;
    case 4: // Probably 32-bpp
      {
        Uint32 *bufp;
        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        *bufp = color;
      }
      break;
  }
}

void draw8x8TileTo(int x, int y, const u8 * gfx, int flags, SDL_Surface * sfc) {
  // draws a 8*8 tile to the given x,y tile position
/*
  Uint32 width = sfc->w;
  u8* pixels = (u8*)sfc->pixels;
  u16* vidPtr = (u16 *)&pixels[x*2*8 + y*8*width*2];
  if (flags & TILE_FLIP_VERT)
    vidPtr = (u16 *)&pixels[x*2*8 + ((y*8)+7)*width*2];
  if (flags & TILE_FLIP_HORZ)
    vidPtr += 7;
  int scanline, pix, pixelPair;
  int palette = g_tilePalette8[x+y*WIN_SCRN_X];
  for (scanline = 0; scanline < 8; scanline++) { 
    // copy gfx to vidPtr - scanline of gfx to vidPtr then increase row
    for (pix  = 0; pix < 4; pix++) {
      // there are 2 pixels per byte in the gfx array...
      pixelPair = *gfx++;
      if (flags & TILE_FLIP_HORZ) {
        if ((pixelPair&0xf0))
          *vidPtr-- = BGPaletteMem[palette*16 + ((pixelPair>>4)&0xf)];
        else
          *vidPtr-- = BGPaletteMem[0];

        if ((pixelPair&0xf))
          *vidPtr-- = BGPaletteMem[palette*16 + (pixelPair&0xf)];
        else
          *vidPtr-- = BGPaletteMem[0];
      } else {
        if ((pixelPair&0xf))
          *vidPtr++ = BGPaletteMem[palette*16 + (pixelPair&0xf)];
        else
          *vidPtr++ = BGPaletteMem[0];

        if ((pixelPair&0xf0))
          *vidPtr++ = BGPaletteMem[palette*16 + ((pixelPair>>4)&0xf)];
        else
          *vidPtr++ = BGPaletteMem[0];
      }
    }
    if (flags & TILE_FLIP_HORZ) {
      if (flags & TILE_FLIP_VERT)
        vidPtr-= (width-8);
      else
        vidPtr+= width+8;
    } else {
      if (flags & TILE_FLIP_VERT)
        vidPtr-= (width+8);
      else
        vidPtr+= width-8;
    }
    
  }
*/

  SDL_Rect rect;
  rect.w = 1*gfx_scale;
  rect.h = 1*gfx_scale;
  int pixelPair, i, j;
  int palette = g_tilePalette8[x+y*WIN_SCRN_X];
  for (j = 0; j < 8; j++) {
    for (i = 0; i < 4; i++) {
      rect.x = i*gfx_scale*2 + x*gfx_scale*8;
      if (flags & TILE_FLIP_VERT) {
	rect.y = (7-j)*gfx_scale + y*gfx_scale*8;
      }else {
	rect.y = j*gfx_scale + y*gfx_scale*8;
      }
      pixelPair = *gfx++;
      if (flags & TILE_FLIP_HORZ) {
	// horizontal tiles start at the end and work back
	rect.x = (3-i)*gfx_scale*2 + x*gfx_scale*8 + gfx_scale;
	if ((pixelPair&0xf0))
	  SDL_FillRect (sfc, &rect, BGPaletteMem[palette*16 + ((pixelPair>>4)&0xf)]); 
	else
	  SDL_FillRect (sfc, &rect, BGPaletteMem[0]); 
	rect.x-=gfx_scale;
	if ((pixelPair&0xf))
	  SDL_FillRect (sfc, &rect, BGPaletteMem[palette*16 + (pixelPair&0xf)]); 
	else
	  SDL_FillRect (sfc, &rect, BGPaletteMem[0]); 
      } else {

	if ((pixelPair&0xf))
	  SDL_FillRect (sfc, &rect, BGPaletteMem[palette*16 + (pixelPair&0xf)]); 
	else
	  SDL_FillRect (sfc, &rect, BGPaletteMem[0]); 
	rect.x+=gfx_scale;
	if ((pixelPair&0xf0))
	  SDL_FillRect (sfc, &rect, BGPaletteMem[palette*16 + ((pixelPair>>4)&0xf)]); 
	else
	  SDL_FillRect (sfc, &rect, BGPaletteMem[0]); 
      }
    }
  }
  
}
// remember - lock the screen before calling this!
void draw8x8Tile(int x, int y, const u8 * gfx, int flags) {
  // draws a 8*8 tile to the given x,y tile position
  draw8x8TileTo(x,y, gfx, flags, screen);
}

void draw8x8SpriteTile(int x, int y, const u8 * gfx, int flags) {
  // draws a 8*8 tile to the given x,y tile position
  /*
  Uint32 width = screen->w;
  u8* pixels = (u8*)screen->pixels;
  u16* vidPtr = (u16 *)&pixels[x*2*8 + y*8*width*2];
  int scanline, pix, pixelPair;
  for (scanline = 0; scanline < 8; scanline++) { 
    // copy gfx to vidPtr - scanline of gfx to vidPtr then increase row
    for (pix  = 0; pix < 8; pix++) {
      pixelPair = *gfx++;
      if ((pixelPair&0xf))
	*vidPtr++ = SpritePaletteMem[(pixelPair&0xf)];
      else
        vidPtr++;
       
    }
    vidPtr+= width-8;
  }
*/

  SDL_Rect rect;
  rect.w = 1*gfx_scale;
  rect.h = 1*gfx_scale;
  int pixelPair, i, j;
  for (j = 0; j < 8; j++) {
    for (i = 0; i < 8; i++) {
      rect.x = i*gfx_scale + x*gfx_scale*8;
      rect.y = j*gfx_scale + y*gfx_scale*8;
      pixelPair = *gfx++;
      if ((pixelPair&0xf))
        SDL_FillRect (screen, &rect, SpritePaletteMem[(pixelPair&0xf)]);
    }
  }
}

void clear_win(void) {
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_LockSurface(screen);
  SDL_Rect rect = {0,0, screen->w, screen->h};
  Uint32 color = SDL_MapRGB(screen->format, 0, 0, 0);
  SDL_FillRect (screen, &rect, color);
  if ( SDL_MUSTLOCK(screen) ) 
    SDL_UnlockSurface(screen);
  
}

SDL_Surface * get_icon(int gfxid) {
  SDL_Surface * icon_sfc = SDL_CreateRGBSurface (SDL_SWSURFACE, 32,32,
                              screen->format->BitsPerPixel,
                              screen->format->Rmask,
                              screen->format->Gmask,
                              screen->format->Bmask,
                              screen->format->Amask
                              );
  if (!icon_sfc) {
    printf("Error creating icon\n");
    return NULL;
  }
  int tmp_scale = gfx_scale;
  gfx_scale = 2;
  // fill with gfx data for creature gfxid
  load_bg_palette(CHAOS_SPELLS.pSpellDataTable[gfxid]->palette,
      CHAOS_SPELLS.pSpellDataTable[gfxid]->palette);

  setWinPalette(0,0,CHAOS_SPELLS.pSpellDataTable[gfxid]->palette);
  setWinPalette(1,0,CHAOS_SPELLS.pSpellDataTable[gfxid]->palette);
  setWinPalette(0,1,CHAOS_SPELLS.pSpellDataTable[gfxid]->palette);
  setWinPalette(1,1,CHAOS_SPELLS.pSpellDataTable[gfxid]->palette);

  int frame = 0; 
  const u8* gfx = CHAOS_SPELLS.pSpellDataTable[gfxid]->pGFX;
  const u8* map = CHAOS_SPELLS.pSpellDataTable[gfxid]->pGFXMap;
  u16 nTileCount = 4; // 4 tiles for each 16*16 gfx 
  u16 index = frame*nTileCount*2;  // 2 because the map is 16 bit and we need to skip every other value
  u16 id;
  u8 loop = 0;
  for (loop = 0; loop < 2; loop++) {
    id = map[index];
    index += 2;    
    draw8x8TileTo(loop, 0, &gfx[id*32], 0, icon_sfc);
  }

  for (loop = 0; loop < 2; loop++) {
    id = map[index];
    index += 2;
    draw8x8TileTo(loop, 1, &gfx[id*32], 0, icon_sfc); 
  }
  gfx_scale = tmp_scale;
  return icon_sfc;
}


#endif

