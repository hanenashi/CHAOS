#ifndef _platform_h
#define _platform_h

/*
  Define which platform we are using
  __WINDOWS__ for Windows
  __GP32 __   for GP32
  __GBA__     for Game Boy Advance (default)
*/

#if defined(_WIN32) || defined(WIN32)
    #if !defined(__WINDOWS__)
        #define __WINDOWS__
    #endif
#endif
#if defined(linux)
        #define __WINDOWS__
#endif

#if !defined(__WINDOWS__) && !defined(__GP32__)
  #if !defined(__GBA__)
    #define __GBA__
  #endif
#endif

#ifndef __GBA__
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef void (*fp)(void);

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

typedef unsigned char byte;
typedef unsigned short hword;
typedef unsigned long word;

typedef unsigned char soundtype_t;

#define BIT00 1
#define BIT01 2
#define BIT02 4
#define BIT03 8
#define BIT04 16
#define BIT05 32
#define BIT06 64
#define BIT07 128
#define BIT08 256
#define BIT09 512
#define BIT10 1024
#define BIT11 2048
#define BIT12 4096
#define BIT13 8192
#define BIT14 16384
#define BIT15 32768

//this converts a color value to 15 bit BGR value used by GBA
#define RGB16(r,g,b)  ((r)+(g<<5)+(b<<10))  
#define TILE_FLIP_VERT 1
#define TILE_FLIP_HORZ 2
#define GetRed(col)  ((col)&0x1f)
#define GetGreen(col) (((col)>>5)&0x1f)
#define GetBlue(col)  ( ((col)>>10)&0x1f)
#endif

#endif

