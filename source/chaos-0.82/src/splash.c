// splash.c
#include "platform.h"
#ifdef __GBA__
#include "screenmode.h"
#endif

#ifdef __WINDOWS__
#include "lobby.h"
#include "network.h"
#endif

#include "splash.h"
#include "options.h"
#include "players.h"
#include "gfx.h"
#include "text16.h"
#include "chaos.h"
#include "creature.h"

u8 splash_menu_on;
u8 anim_pal;

void select_splash_item(u8 item);

void show_splash(void) {
  clear_palettes();
  clear_bg();
  clear_arena();
  anim_col = -31;
  anim_col_grad = -8;
  hilite_item = 0;
  splash_menu_on = 0;
  current_screen = SCR_SPLASH;
  set_text16_colour(1, RGB16(31,0,31));
  print_text16("CHAOS -THE BATTLE OF WIZARDS", 1,2, 1);  
  set_text16_colour(2, RGB16(31,0,0));
  print_text16("By Julian Gollop", 7,4, 2);  
  
  set_text16_colour(3, RGB16(0,30,30));
#ifdef __GBA__
  print_text16("Gameboy Advance version" , 3,10, 3);  
#else
  print_text16("SDL version" , 9,10, 3);  
#endif
  print_text16("by Quirky", 10, 12, 3);
  
#ifdef DEBUG
  print_text16("DEBUG BUILD", 9, 12, 4);
  set_text16_colour(4, RGB16(0,28,0));
#endif

  set_text16_colour(12, RGB16(31,31,0));
  print_text16("Press START", 9,17, 12);  
  anim_pal = 12;
  draw_decor_border(15, RGB16(31,0,0),RGB16(31,0,31)); 
}


// start key pressed, for now start the game
void splash_start(void) {
  if (splash_menu_on == 0) {
    splash_menu_on = 1;
    hilite_item = 0;
    print_text16("                    ", 2,17, 12);  
    
    print_text16("START", 7,17, 12);  
    
    print_text16("OPTIONS", 15,17, 13);  
    
    select_splash_item(0);
    
  }
  else {
    select_splash_item(0);
    fade_down();
#ifdef __WINDOWS__
    // on windows, check what is going on with regards the net play
    if (ONLINE_MODE){
      show_create_lobby();
    } else {
      show_create_players();
    }
#else
    show_create_players();
#endif
    fade_up();
  }
}

//void splash_select(void) {
//  fade_down();
//  show_options();
//  fade_up();
//}

  
void splash_right(void) {
  if (splash_menu_on == 0)
    return;
    
  if (hilite_item == 0)  {
    hilite_item = 1;
    select_splash_item(hilite_item);
  }
  
}

void splash_left(void) {
  if (splash_menu_on == 0)
    return;
  
  if (hilite_item == 1) {
    hilite_item = 0;
    select_splash_item(hilite_item);
  }
  
}

void splash_a(void) {
  if (splash_menu_on == 0)
    return;
  
  if (hilite_item == 1) {
    fade_down();
    show_options();
    fade_up();
  } else {
    select_splash_item(0);
    fade_down();
#ifdef __WINDOWS__
    // on windows, check what is going on with regards the net play
    if (ONLINE_MODE) {
      show_create_lobby();
    } else {
      show_create_players();
    }
#else
    show_create_players();
#endif
    fade_up();
  }
    
}
  


void animate_splash_screen(void) {
  anim_selection(anim_pal, 31,31,0);
}


void select_splash_item(u8 item) {
  anim_pal = 12 + item;
  set_text16_colour(anim_pal, RGB16(31,31,0));
  set_text16_colour(13-item, RGB16(20,20,0));
}


