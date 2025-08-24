#include "platform.h"

#ifdef __GBA__
#include "gba.h"
#include "screenmode.h"
#include "sleep.h"
#include "sound.h"
#define MAX_GAME_MENU 3
#elif defined __WINDOWS__
#define MAX_GAME_MENU 2
#include "winsound.h"
#include "network.h"
#include "remote.h"
#include "options.h"
#endif

#include "sound_data.h"
#include "text16.h"
#include "gamemenu.h"
#include "examine.h"
#include "players.h"
#include "casting.h"
#include "creature.h"
#include "wizards.h"
#include "chaos.h"
#include "gfx.h"
#include "arena.h"

#include "chaos_sprites.h"
#include "spellselect.h"



void deselect_game_item(u8 item);
void select_game_item(u8 item);

//////////////////////////////////////////////////////////

#define MENU_XPOS  4
#define MENU_YPOS  8
#define CHAOS_GAUGE_YPOS  4

void show_game_menu(void) {

  u16 i;
  hilite_item = 0;
  current_screen = SCR_GAME_MENU;
  clear_bg();
  clear_arena();
  clear_palettes();
  
  set_text16_colour(12, RGB16(30,31,0));
  print_text16(players[current_player].name, 4,2, 12);  
  
  draw_decor_border(15, RGB16(31,0,0),RGB16(31,31,0)); 
  
  // draw chaos/law val - colour 13
  if (world_chaos != 0) {
    u8 screen_x = MENU_XPOS;
    set_text16_colour(13, RGB16(31,30,0));
    
    if (world_chaos < 0) {
      // chaos
      print_text16("(CHAOS ", screen_x,CHAOS_GAUGE_YPOS, 13); 
      screen_x += 7;
      u8 A = world_chaos*-1;
      A >>= 2;
      
      if (A != 0) {
        for (i = 0; i < A; i++) {
          print_text16("*", screen_x++,CHAOS_GAUGE_YPOS, 13); 
        } 
      }
      
    } else {
      // law
      print_text16("(LAW ", screen_x,CHAOS_GAUGE_YPOS, 13); 
      screen_x += 5;
      u8 A = world_chaos;
      A >>= 2;
      
      if (A != 0) {
        for (i = 0; i < A; i++) {
          print_text16("+", screen_x++,CHAOS_GAUGE_YPOS, 13); 
        } 
      }
      
    }
    print_text16(")", screen_x,CHAOS_GAUGE_YPOS, 13); 
    
  }
  
  deselect_game_item(0);
  deselect_game_item(1);
  deselect_game_item(2);
  deselect_game_item(3);
  
  select_game_item(hilite_item);
  
  print_text16("1. SELECT SPELL", MENU_XPOS,MENU_YPOS, 0);  
  print_text16("2. EXAMINE BOARD", MENU_XPOS,MENU_YPOS+3, 1);  
  print_text16("3. CONTINUE WITH GAME", MENU_XPOS,MENU_YPOS+6, 2);  
#ifdef __GBA__
  print_text16("4. ENTER SLEEP MODE", MENU_XPOS,MENU_YPOS+9, 3);  
#endif

}

void game_menu_up(void) {
  deselect_game_item(hilite_item);
  if (hilite_item > 0) {
    hilite_item--;
    PlaySoundFX(SND_MENU);
  }
  
    
  select_game_item(hilite_item);
}

void game_menu_down(void) {
  deselect_game_item(hilite_item);
  if (hilite_item < MAX_GAME_MENU) {
    hilite_item++;
    PlaySoundFX(SND_MENU);
  }
  
  select_game_item(hilite_item);
}

void game_menu_a(void) {
  // get the hilited item...
  PlaySoundFX(SND_CHOSEN);
  switch (hilite_item) {
    case 0: // view spells
        fade_down();
        show_spell_screen();
        fade_up();
        break;
    case 1: // view arena
        fade_down();
        examine_board();    
        fade_up();
        break;
    case 2: // continue game
        continue_game();
        break;
#ifdef __GBA__
    case 3: // sleep mode
        Sleep_Mode(1);
        break;
#endif
    default: break;    
  }
}

void continue_game(void) {
#ifdef __WINDOWS__
  if (ONLINE_MODE) {
    // online mode carry on...
    // this assumes just 1 local player 
    // 1. send chosen spell to server
    //      server keeps track of who has sent what...
    // 2. wait for everyone to respond
    //      server waits for everyone to respond, then tells
    //      all clients who cast what so they can carry on
    //    if we are the host, when everyone finishes, send the choices on 
    //    to everyone else
    // 3. start game as per offline mode 

    // 1.
    send_spellchoice(current_player);
    // 2. 
    show_spellwaiting_screen();
    // 3.  copy + paste..
    fade_down();
    init_arena_tables();
    display_arena();
    current_player = 0;
    
    set_border_col(0);
    draw_game_border();
    draw_cursor(CURSOR_NORMAL_GFX);
    cursor_x = 0;
    cursor_y = 0;
    SET_CURSOR_POSITION(cursor_x, cursor_y, 0,0);
    fade_up();
    start_cast_round();

  }
  else {
#endif
  u8 next_player = get_next_human(current_player);
  if (next_player == 9) {
    // continue game after spell selection... this is roughly at 95c7
    fade_down();
    init_arena_tables();
    display_arena();
    current_player = 0;
    
    set_border_col(0);
    draw_game_border();
    draw_cursor(CURSOR_NORMAL_GFX);
    cursor_x = 0;
    cursor_y = 0;
    SET_CURSOR_POSITION(cursor_x, cursor_y, 0,0);
    fade_up();
    start_cast_round();
  } else {
    current_player = next_player;    
    fade_down();
    show_game_menu();
    fade_up();
  }
#ifdef __WINDOWS__
  }  
#endif
}


void deselect_game_item(u8 item) {
  set_text16_colour(item, RGB16(0,16,16));
}

void select_game_item(u8 item) {
  set_text16_colour(item, RGB16(0,31,31));
}
