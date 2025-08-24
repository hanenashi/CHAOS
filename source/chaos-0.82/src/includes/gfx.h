#ifndef GFX_H
#define GFX_H
#include "platform.h"

#ifdef __GBA__
#include "backgrounds.h"
#include "gba.h"
#include "sprite.h"
#endif

#ifdef __GBA__
extern Bg bg_level;
extern Bg bg_text;
extern Bg bg_rotate;
#endif

#if defined(__WINDOWS__)
#include "SDL.h"
extern SDL_Surface *screen;
extern int gfx_scale;

// these values are derived from the screen size
// the max width is: border + 15*2 + border = 2+30+2 = 34
// the max height is: border+ 10*2 + border + text = 2+20+2+2 = 26
#define WIN_SCRN_X  34
#define WIN_SCRN_Y  26

// 8*8 tile palette emulation
extern int g_tilePalette8[WIN_SCRN_X*WIN_SCRN_Y];
extern u32 BGPaletteMem[256];
#define setWinPalette( x,  y,  palette) g_tilePalette8[(x)+(y)*WIN_SCRN_X] = palette
// convert from 5 bits per colour to 8 bits per colour
#define winCol(rgb16) (SDL_MapRGB( screen->format, (rgb16&0x1f)*8, \
      ((rgb16>>5)&0x1f)*8, ((rgb16>>10)&0x1f)*8))
void draw8x8Tile(int x, int y, const u8 * gfx, int flags);
void clear_win(void);
void clear_square8(unsigned char x, unsigned char y);
void draw_win_cursor(void);
int get_win_tile(int x, int y) ;
void clear_pixel(int x, int y) ;
SDL_Surface * get_icon(int);
void remove_win_cursor(void);
#endif

extern unsigned char cursor_x;
extern unsigned char cursor_y;
extern unsigned char fadedup;

// a silly way to draw graphics...
// can offset them by 1 by doing x*2 +1, etc
// used for the wizard deaths...
void draw_gfx(const u8 * gfx, const u8 * map, u8 x, u8 y, u8 frame);
void set_palette(unsigned short x, unsigned short y, unsigned char palette) ;
void draw_gfx8(const u8 * gfx, const u8 * map, u8 x, u8 y, u8 frame);
// similar for paletes..
void set_palette8(unsigned short x, unsigned short y, unsigned char palette);

void draw_creature(unsigned char x, unsigned char y, unsigned char i,unsigned char f);
void draw_wizard(unsigned long x, unsigned long y, unsigned long wizardid, 
  unsigned long frame, unsigned long playerid);
// a hacky test for now, tidy up later
void draw_wizard8(unsigned long x, unsigned long y, unsigned long wizardid, 
  unsigned long frame, unsigned long playerid);
  
void clear_square(unsigned char x, unsigned char y);

void set_player_col_RGB(unsigned char playerid, unsigned short red, unsigned short green, unsigned short blue);
void set_player_col(unsigned char playerid, unsigned short colour);

void set_border_col(unsigned char playerid);

void load_bg_palette(u8 topal, u8 frompal);
void load_all_palettes(void);
void load_sprite_palette(void);

void draw_game_border(void);
void clear_game_border(void);
void draw_decor_border(unsigned char pal, unsigned short col1,  unsigned short col2);

void scroll_arena_up(void);
void scroll_arena_down(void);
void scroll_arena_left(void);
void scroll_arena_right(void);

void fg_set_colour(unsigned char col);
//void draw_pixel(unsigned char x, unsigned char y);
//void gfx_slow_line(int x1, int y1, int x2, int y2);
//void draw_lightning(unsigned short x1, unsigned short x2);
//void clear_fg(void);

int getMainColour(int creatid, int frame) ;

void draw_cursor(u32 cursorid); 


void clear_palettes(void);
void clear_palette(unsigned char pal);

void anim_selection(u8 pal, u8 red, u8 green, u8 blue);
void draw_fight_frame(u8 x, u8 y, u8 frame);

void remove_cursor(void);
void redraw_cursor(void);
//void set_cursor_position(u32 x, u32 y);

// check pixel set...
unsigned char is_pixelset(unsigned short x, unsigned short y, int, int);
void draw_pixel_4bpp(unsigned short x, unsigned short y);

void draw_spellcast_frame(u8 x, u8 y, u8 frame);
void draw_splat_frame(u8 x, u8 y, u8 frame);
void draw_pop_frame(u8 x, u8 y, u8 frame);
void draw_breath_frame(u8 x, u8 y, u8 frame);

void draw_silhouette_gfx(u8 arena_index, const u8 * gfx, const u8 * map,
    u16 col, int palette,
    u8 negative);

void fade_up(void);
void fade_down(void);

void clear_vram(void);
#ifdef __GBA__
/*void CopyOAM(unsigned char from, unsigned char to);
void MoveSprite(OAMEntry* sp, signed short int x, signed short int y);*/
#endif

#endif

