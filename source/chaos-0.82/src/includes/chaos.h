// chaos.h

#ifndef CHAOS_H
#define CHAOS_H

#include "platform.h"
#ifdef __GBA__
#include "gba.h"
#elif defined( __WINDOWS__)
#include "SDL.h"
#endif

#define SCR_SPLASH               10
#define SCR_OPTIONS              20
#define SCR_CREATE_PLAYERS       30
#define SCR_CREATE_LOBBY         35
#define SCR_EDIT_NAME            40
#define SCR_GAME_MENU            50
#define SCR_SELECT_SPELL         60
#define SCR_EXAMINE_SPELL        70
#define SCR_EXAMINE_BOARD        80

#define SCR_CASTING              100
#define SCR_MOVEMENT             110
#define SCR_INFO_SCREEN          120
#define SCR_EXAMINE              130

#define WIZARD_INDEX              0x2A

// defines for status_flag
#define UNDEAD 1
#define ENGAGED 2
#define MOVELEFT 4
#define TURNOVER 8
#define ISHUMAN 16
#define SPELL_IS_ILLUSION 32
#define ISHIDDEN 64
#define ALIVE 128

extern const unsigned short int chaos_cols[];
extern unsigned char playercount;
extern unsigned char dead_wizards;

extern unsigned long int current_screen;
extern unsigned char hilite_item;
extern signed char anim_col;
extern signed char anim_col_grad;
extern unsigned char current_player;
extern u8 game_frames;
extern int g_icon;

extern const signed char surround_table[8][2];
extern const unsigned char key_table[8];

void enable_interrupts(void);
void disable_interrupts(void);
void delay (u8 time);

#if defined (__WINDOWS__)
void wait_vsync_int(void);
Uint32 callback_vblank(Uint32 interval, void* param); 
Uint32 callback_timer2(Uint32 interval, void* param);
extern int int_main;
#endif

#endif //CHAOS_H

