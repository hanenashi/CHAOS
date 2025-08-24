// chaos_sprites.h header file...
#ifndef  chaos_sprites_h_seen
#define  chaos_sprites_h_seen
#define CURSOR_SPELL_GFX 0
#define CURSOR_ENGAGED_GFX 1
#define CURSOR_FIRE_GFX 2
#define CURSOR_FLY_GFX 3
#define CURSOR_GROUND_GFX 4
#define CURSOR_NORMAL_GFX 5
#define END_GFX 128

#if !defined( __MINGW32__)
#define binary_cursor_spell_raw_start _binary_cursor_spell_raw_start
#define binary_cursor_spell_map_start _binary_cursor_spell_map_start
#define binary_cursor_engaged_raw_start _binary_cursor_engaged_raw_start
#define binary_cursor_engaged_map_start _binary_cursor_engaged_map_start
#define binary_cursor_fire_raw_start _binary_cursor_fire_raw_start
#define binary_cursor_fire_map_start _binary_cursor_fire_map_start
#define binary_cursor_fly_raw_start _binary_cursor_fly_raw_start
#define binary_cursor_fly_map_start _binary_cursor_fly_map_start
#define binary_cursor_ground_raw_start _binary_cursor_ground_raw_start
#define binary_cursor_ground_map_start _binary_cursor_ground_map_start
#define binary_cursor_raw_start _binary_cursor_raw_start
#define binary_cursor_map_start _binary_cursor_map_start
#endif
extern unsigned char binary_cursor_spell_raw_start[];
extern unsigned char binary_cursor_spell_map_start[];
extern unsigned char binary_cursor_engaged_raw_start[];
extern unsigned char binary_cursor_engaged_map_start[];
extern unsigned char binary_cursor_fire_raw_start[];
extern unsigned char binary_cursor_fire_map_start[];
extern unsigned char binary_cursor_fly_raw_start[];
extern unsigned char binary_cursor_fly_map_start[];
extern unsigned char binary_cursor_ground_raw_start[];
extern unsigned char binary_cursor_ground_map_start[];
extern unsigned char binary_cursor_raw_start[];
extern unsigned char binary_cursor_map_start[];
 
struct QGFX_SPRITEMAP
{
    const unsigned char nWidth;  // width in pixels
    const unsigned char nHeight;  // height in pixels
    const unsigned char nX;  // screen pos x
    const unsigned char nY;  // screen pos y
    const unsigned short nAttribute0;  // flags to be used for attrib 0 (COLOR_256 | SQUARE | ROTATION_FLAG)
    const unsigned short nAttribute1;  // flags to be used for attrib 1 (SIZE_32 |ROTDATA(1))
    const unsigned short nOAM; // the OAM value to store the gfx
    const unsigned short nSpriteID;
    const unsigned short nSpriteCount; // number of HW sprites to use
    const unsigned char *pGFX;  // pointer to the graphics array (extern u8 _binary_GFX_raw_start[];)
    const unsigned char *pGFXMap; // pointer to the map file that makes up the graphics (extern u8 _binary_GFX_map_start[];)
};
 
struct QGFX_SPRITETABLE 
{
  const struct QGFX_SPRITEMAP *pSpriteMap;
};
 
struct QGFX_ROOT
{
    const unsigned char nGFXs; // number of peices of gfx data
    const struct QGFX_SPRITETABLE *pSpriteTable;
};
 
extern const struct QGFX_ROOT CHAOS_SPRITES_ROOT;
 
// end of file
#endif

