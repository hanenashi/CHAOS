#include "platform.h"

#ifdef __GBA__
#include "gba.h"
#include "keypad.h"
#include "screenmode.h"
#elif defined(__WINDOWS__)
#include "winkeys.h"
#include "lobby.h"
#endif

#include "input.h"
#include "gamemenu.h"
#include "arena.h"
#include "wizards.h"
#include "gfx.h"
#include "examine.h"
#include "options.h"
#include "editname.h"
#include "movement.h"
#include "splash.h"
#include "players.h"
#include "spellselect.h"
#include "casting.h"

// also need to include all the screens, 
// as we use calls to the specific up, down, etc here
#include "chaos.h"


#define CVK_RIGHT       0
#define CVK_LEFT        1 
#define CVK_UP          2
#define CVK_DOWN        3
#define CVK_A           4  
#define CVK_B           5  
#define CVK_L           6  
#define CVK_R           7  

#define CVK_COUNT       8

#define CVK_MAX              128
#define CVK_KEY_REPEAT_DEFAULT       20


#define CVK_KEY_PRESSED(n)      ( (virtual_keys[n]==1)|| (virtual_keys[n]>CVK_KEY_REPEAT))
unsigned char virtual_keys[CVK_COUNT] = {};
const int key_map[] = {
  KEY_RIGHT,
  KEY_LEFT,
  KEY_UP,
  KEY_DOWN,
  KEY_A,
  KEY_B,
  KEY_L,
  KEY_R,
};


s32 x_max = 0;
s32 y_max = 0;

u8 rotate_flag = 0;
u16 angle = 0;

void up_pressed(void);
void down_pressed(void);
void left_pressed(void);
void right_pressed(void);
void a_pressed(void);
void b_pressed(void);
void l_pressed(void);
void r_pressed(void);
void start_pressed(void);
void select_pressed(void);



void get_key_presses(void) {
  UpdateKeys();
  int i;
  for (i = 0; i < CVK_COUNT; i++) {
    if (KeyPressed(key_map[i])) {
      virtual_keys[i]++;
      if (virtual_keys[i] > 128)
	virtual_keys[i] = 128;
    } else {
      virtual_keys[i] = 0;
    }
  }
}

void handle_keys (void) {
  get_key_presses();
  int CVK_KEY_REPEAT = CVK_KEY_REPEAT_DEFAULT;
  // cut the delay for the game board
  if (current_screen == SCR_CASTING ||
    current_screen == SCR_MOVEMENT ||
    current_screen == SCR_EXAMINE_BOARD ) {
    CVK_KEY_REPEAT /= 2;
  }

  if (CVK_KEY_PRESSED(CVK_UP)) {
    up_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_DOWN)) {
    down_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_LEFT)) {
    left_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_RIGHT)) {
    right_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_A)) {
    a_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_B)) {
    b_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_L)) {
    l_pressed();
  } 
  if (CVK_KEY_PRESSED(CVK_R)) {
    r_pressed();
  } 
  
  
  if (KeyPressedNoBounce(KEY_START,1200)) {
    start_pressed();
  }
  
  if (KeyPressedNoBounce(KEY_SELECT,1200)) {
    select_pressed();
  }
}



void up_pressed(void) {
  switch (current_screen) {
    
    case SCR_EDIT_NAME: edit_name_up(); break;
    case SCR_CREATE_PLAYERS: create_players_up(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_up(); break;
#endif
    case SCR_GAME_MENU: game_menu_up(); break;
    case SCR_SELECT_SPELL: spell_select_up(); break;
    case SCR_OPTIONS:options_up();break;
    
    case SCR_CASTING : 
    case SCR_MOVEMENT :
    case SCR_EXAMINE_BOARD :   
      move_cursor_up(); break;
  }  
}

void down_pressed(void) {
  switch (current_screen) {
    
    case SCR_EDIT_NAME: edit_name_down(); break;
    case SCR_SELECT_SPELL: spell_select_down(); break;
    case SCR_CREATE_PLAYERS: create_players_down(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_down(); break;
#endif
    case SCR_GAME_MENU: game_menu_down(); break;
    
    case SCR_OPTIONS:options_down();break;
    
    case SCR_CASTING : 
    case SCR_MOVEMENT :
    case SCR_EXAMINE_BOARD : 
      move_cursor_down(); break;
    
  }
}

void left_pressed(void) {
  switch (current_screen) {
//    case SCR_GAME_BOARD: move_cursor_left(); break;  
    case SCR_SPLASH: splash_left(); break;
    case SCR_CREATE_PLAYERS: create_players_left(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_left(); break;
#endif
    case SCR_EDIT_NAME: edit_name_left(); break;
    case SCR_SELECT_SPELL:spell_select_left();break;
    
    case SCR_OPTIONS:options_left();break;
    
    case SCR_CASTING : 
    case SCR_MOVEMENT :
    case SCR_EXAMINE_BOARD :  
      move_cursor_left(); break;
  }
}

void right_pressed(void) {
  switch (current_screen) {
//    case SCR_GAME_BOARD: move_cursor_right(); break;  
    case SCR_SPLASH: splash_right(); break;
    case SCR_CREATE_PLAYERS: create_players_right(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_right(); break;
#endif
    
    case SCR_EDIT_NAME: edit_name_right(); break;
    
    case SCR_SELECT_SPELL:spell_select_right();break;
    
    case SCR_OPTIONS:options_right();break;
    
    case SCR_EXAMINE_BOARD :
    case SCR_CASTING : 
    case SCR_MOVEMENT :
       move_cursor_right(); break;
  }
}

void a_pressed(void) {
  switch (current_screen) {
    case SCR_SPLASH: splash_a(); break;
    case SCR_CREATE_PLAYERS: create_players_accept(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_accept(); break;
#endif
    case SCR_GAME_MENU: game_menu_a(); break;
    case SCR_SELECT_SPELL: spell_select_a(); break;
    case SCR_EDIT_NAME: edit_name_a(); break;

    case SCR_OPTIONS: options_a(); break;

    case SCR_CASTING : casting_a(); break;
    case SCR_MOVEMENT : movement_a(); break;
  }
  
}

void b_pressed(void) {
  switch (current_screen) {

   
   case SCR_SELECT_SPELL: spell_select_b(); break;
   case SCR_EDIT_NAME: edit_name_b(); break;
   case SCR_CASTING : casting_b(); break;
   
   case SCR_OPTIONS: options_back(); break;
   
   case SCR_MOVEMENT : movement_b(); break;
   
   case SCR_EXAMINE_BOARD : 
      examine_back();
      break;
   
  }
  
}

void l_pressed(void) {
  switch (current_screen) {    
    case SCR_EXAMINE_BOARD :
    case SCR_MOVEMENT : 
    case SCR_CASTING : movement_l(); break;
    
    case SCR_CREATE_PLAYERS: create_players_l(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_l(); break;
#endif
    
  }
  
}


void r_pressed(void) {
  switch (current_screen) {
    case SCR_SELECT_SPELL: spell_select_r(); break;
    
    case SCR_CREATE_PLAYERS: create_players_r(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_r(); break;
#endif
    
    case SCR_EXAMINE_BOARD :
    case SCR_MOVEMENT : 
    case SCR_CASTING : examine_square(target_index); break;
    
  }
  
}


void start_pressed(void) {
  switch (current_screen) {
    case SCR_SPLASH: splash_start(); break;
    case SCR_MOVEMENT : movement_start(); break;
    case SCR_CREATE_PLAYERS: create_players_start(); break;
#if defined( __WINDOWS__ ) && defined(NETWORK)
    case SCR_CREATE_LOBBY: lobby_start(); break;
#endif
    case SCR_EDIT_NAME: edit_name_start(); break;
//    case SCR_OPTIONS: options_start(); break;
    
    
  }
}

void select_pressed(void) {
  switch (current_screen) {
    case SCR_MOVEMENT : movement_select(); break;
  }
}

// wait here for the player to press ANY key...
// this is from code at bbb0
void wait_for_keypress(void) {
  if (IS_CPU(current_player))
    return;
  u8 prsd = 0;
  while (!prsd) {
    UpdateKeys();
    wait_vsync_int();
    if (  KeyPressed(KEY_A) || KeyPressed(KEY_B) || KeyPressed(KEY_L) || KeyPressed(KEY_R)
        || KeyPressed(KEY_UP) || KeyPressed(KEY_DOWN) || KeyPressed(KEY_LEFT) || KeyPressed(KEY_RIGHT)
        || KeyPressed(KEY_START) || KeyPressed(KEY_SELECT) )
        prsd = 1;
  }
  
}

// wait here for the player to let go of all keys
#define ANIM_VSYNCS    9
void wait_for_letgo(void) {
  if (IS_CPU(current_player))
    return;
  int prsd = 1;
  int counter  = 0;
  while (prsd) {
    // fix for highlight player's creations non-animation:
    if (current_screen == SCR_EXAMINE_BOARD || 
	current_screen == SCR_CASTING || 
	current_screen == SCR_MOVEMENT) 
    {
      if (g_highlight_creations < 9) {
        counter ^= 1;
        wait_vsync_int();
        // without this wait, the LCD doesn't have time to change and
        // the gfx appear kludgely alpha blended!
        int i;
        for ( i = 0; i < ANIM_VSYNCS; ++i) {
          if (counter)
            highlight_players_stuff(g_highlight_creations);
          else
            animate_arena();
          wait_vsync_int();
        }
      }
    }
    // end fix
    UpdateKeys();
    wait_vsync_int();
    if (  KeyPressed(KEY_A) || KeyPressed(KEY_B) || KeyPressed(KEY_L) || KeyPressed(KEY_R)
        || KeyPressed(KEY_UP) || KeyPressed(KEY_DOWN) || KeyPressed(KEY_LEFT) || KeyPressed(KEY_RIGHT)
        || KeyPressed(KEY_START) || KeyPressed(KEY_SELECT) )
        prsd = 1;
    else
        prsd = 0;
  }
  
}

