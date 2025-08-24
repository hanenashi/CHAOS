#ifndef TEXT16_H
#define TEXT16_H

#ifdef __GBA__
#include"backgrounds.h"
#endif

#if !defined( __MINGW32__)
#define text16_raw _binary_chaosfont_raw_start
#else
#define text16_raw binary_chaosfont_raw_start
#endif

// gfx2gba -t8 -M -F -D -c8 chaosfont.pcx
extern u8 text16_raw [ ];
extern u16 bgmapoffset16;
/*

The map of the characters is as follows:

' ( ) * + , - . / 0-9 : ; < = > ? @ A-Z [ \ ] ^ _ ` a-z 

The changes are:

    ASCII   |   Chaos
------------+-------------
            |
     *      |   CHAOS symbol
     +      |   LAW symbol
     /      |   %
     <      |   back arrow
     >      |   return/enter arrow
     \      |   #

e.g. "\\<>" would print "#" followed by back arrow, return arrow on screen


*/

#define MAX_LETTERS       85
#define FIRST_CHAR_INDEX  '\''
#define FILLED_CHAR_INDEX  ('z'+2)
#define TRANSP_CHAR_INDEX  ('z'+1)

#ifdef __GBA__
void setup_text16(Bg* bg, u16 offset);
#endif

void print_text16(const char* text, u16 x, u16 y , u8 pal);
void put_char8(int ch, int x, int y, int pal, u32  extraFlags);
void set_text16_colour(u8 pal, u16 col);
unsigned short get_text16_colour(u8 pal);
void clear_bg();
char * trim_whitespace(char * str);
void int2a(int n, char s[], int b);

#endif //TEXT16_H

