
#include "platform.h"
#ifdef __GBA__
#include "gba.h"
#include "screenmode.h"
#include "dma.h"
#endif

#include "arena.h"
#include "text16.h"
#include "string.h"
#include "gfx.h"

u16 bgmapoffset16; 
void reverse(char s[]);
#if defined(__WINDOWS__)
u32 g_text_tiles[WIN_SCRN_X*WIN_SCRN_Y];
void redraw_text(int pal);
#endif

// set up the 16 point letters in the tile memory - used for standard stuff...
// the letters are 4bpp
#if defined(__GBA__)  
void setup_text16(Bg* bg, u16 offset) {
  bgmapoffset16 = offset;
  u16 realos = bgmapoffset16*16; // each 8 by 8 tile takes up 16 bytes
  
  // the font consists of 84 letters
  // each one is 16*8, 4bpp = 16*4 pixels = 64 u8 transfers, 32 u16 transfers per char
  // total = 32*84
  REG_DMA3SAD = (u32)&text16_raw[0]; 
  REG_DMA3DAD = (u32)&bg->tileData[realos];
  REG_DMA3CNT = 32*MAX_LETTERS |DMA_16NOW;
  
  // the colour is set depending on the palette
  
}
#endif

// set the colour for the text in a given palette
void set_text16_colour(u8 pal, u16 col) {
#if defined(__GBA__)
  BGPaletteMem[16*pal+1] = col;
#elif defined(__WINDOWS__)
  BGPaletteMem[16*pal+1] = winCol(col);
  redraw_text(pal);
#endif
}

unsigned short get_text16_colour(u8 pal) {
  return BGPaletteMem[16*pal+1];
}



// print the 16 point text
// each letter is 2 tiles big. the first tile goes on row y,
// the second on row y+1
void print_text16(const char* text, u16 x, u16 y , /* Bg* bg,*/ u8 pal)
{

#ifdef __GBA__
  u32 indx=0;
  u32 newx = x;
	
  u32 letter;
  while( (text[indx]!= 0) ) 
  {
    letter = text[indx] - FIRST_CHAR_INDEX;
    // draw a letter...
    if (text[indx] != 32) {
      bg_text.mapData[newx+y*32] = letter*2+bgmapoffset16;
      bg_text.mapData[newx+(y+1)*32] = letter*2 + 1 + bgmapoffset16;
    }
    else {
      letter = TRANSP_CHAR_INDEX - FIRST_CHAR_INDEX;
      bg_text.mapData[newx+y*32] = letter*2+1+bgmapoffset16;//9+bgmapoffset16+97;
      bg_text.mapData[newx+(y+1)*32] = letter*2+1+bgmapoffset16;//9+bgmapoffset16+97;
    }
    // set the palette at these locations too
    bg_text.mapData[newx+y*32] &= ~0xF000; 
    bg_text.mapData[newx+y*32] |= (pal<<12)&0xF000;
    
    bg_text.mapData[newx+(y+1)*32] &= ~0xF000; 
    bg_text.mapData[newx+(y+1)*32] |= (pal<<12)&0xF000;
    
    newx++;
    indx++;
  }
#elif defined( __WINDOWS__  )
  u32 indx=0;
  u32 newx = x;
  if (y != MESSAGE_Y) {
    newx+=2;
    y++;
  }
  u32 letter;
  SDL_LockSurface(screen);
  while( (text[indx]!= 0) ) 
  {
    letter = text[indx] - FIRST_CHAR_INDEX;
    // draw a letter...
    setWinPalette(newx,y,pal);
    setWinPalette(newx,y+1,pal);
    if (text[indx] == 32) {
      letter = TRANSP_CHAR_INDEX - FIRST_CHAR_INDEX;
      //letter = 0;
    }
    draw8x8Tile(newx,y, &text16_raw[letter*8*8],0);
    draw8x8Tile(newx,y+1, &text16_raw[letter*8*8 + 4*8],0);
    g_text_tiles[newx+y*WIN_SCRN_X] = letter*2;
    g_text_tiles[newx+(y+1)*WIN_SCRN_X] = letter*2+1;
    newx++;
    indx++;
  }
  SDL_UnlockSurface(screen);
#endif
}


// place a single character from the alphabet at position x, y
// this is used to draw, eg. the ^/v arrow thing
void put_char8(int ch, int x, int y, int pal, u32  extraFlags) {
  // put the character in spot x, y with palette pal
  if (ch != 0) {
#ifdef __GBA__
    bg_text.mapData[x+y*32] = ch*2 + bgmapoffset16;
#elif defined(__WINDOWS__)
    setWinPalette(x,y,pal);
    draw8x8Tile(x,y, &text16_raw[ch*8*8],extraFlags);
#endif
  } else {
#ifdef __GBA__
    bg_text.mapData[x+y*32] = 0;
#elif defined(__WINDOWS__)
    ch = TRANSP_CHAR_INDEX - FIRST_CHAR_INDEX;
    setWinPalette(x,y,pal);
    draw8x8Tile(x,y, &text16_raw[ch*8*8],extraFlags);
#endif
  } 
#ifdef __GBA__
  bg_text.mapData[x+y*32] &= ~0xF000; 
  bg_text.mapData[x+y*32] |= (pal<<12)&0xF000;
  bg_text.mapData[x+y*32] |= extraFlags; 
#endif
}

void clear_bg()  {
  int x, y;
#ifdef __GBA__
  
  for (x = 0; x < 32; x++) {
    for (y = 0; y < 32; y++) {
      bg_text.mapData[x+y*32] = 0; 
    }
  }
#elif defined(__WINDOWS__)
  
  for (x = 0; x < WIN_SCRN_X; x++) {
    for (y = 0; y < WIN_SCRN_Y; y++) {
      g_text_tiles[x+y*WIN_SCRN_X] = (unsigned long)-1;
    }
  }
  clear_win();
#endif
}

#ifdef __WINDOWS__
void redraw_text(int pal) {
  // scan for pal in the g_tilePalette8[] then redraw text there
  int x, y, tile;
  for (x = 0; x < WIN_SCRN_X; x++) {
    for (y = 0; y < WIN_SCRN_Y; y++) {
      if (g_tilePalette8[x+y*WIN_SCRN_X] == pal) {
        tile = g_text_tiles[x+y*WIN_SCRN_X];
        if (tile>=0)
          draw8x8Tile(x,y, &text16_raw[g_text_tiles[x+y*WIN_SCRN_X]*4*8],0);
        //put_char8(g_text_tiles[x+y*WIN_SCRN_X],x , y, pal, 0);
      }
    }
  }

}
#endif


/*
 *  remove whitespace at the end and the beginning of a char array
 *  the char array is used for the name, so max length is 11
 */
#ifdef __cplusplus
extern "C" {
#endif
  int isspace(char); 
#ifdef __cplusplus
}
#endif
char * trim_whitespace(char * str) {
  char *s = str;
  char *d = str;
  
  // eat trailing white space
  while (*s)
    s++;
  while (str < s && isspace(*(s-1)))
    *s-- = '\0';
  
  *s = '\0';
  
  s = str;
  // has leading space?
  if (isspace(*s))
  {
    // eat white space 
    while (isspace(*s))
      s++;
    
    // copy
    while (*s)
      *d++ = *s++;
    *d = '\0';
  } 
  return str;
}

void int2a(int n, char s[], int b) {
    static char digits[17] = "0123456789ABCDEF";
    int i, sign;
   
    if ( b < 2 || b > 36 ) {
        return;
    }
   
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = digits[n % b];
    } while ((n /= b) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


/*  Reverses string s[] in place  */

void reverse(char s[]) {
    int c, i, j;
    for ( i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

