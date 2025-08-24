/*

examine.c

arena viewer - used when "Examine Board" is clicked,
also has the spell/creature data displaying routines

*/
#include "platform.h"

#ifdef __GBA__
#include "screenmode.h"
#endif

#include "arena.h"
#include "spelldata.h"
#include "spellselect.h"
#include "casting.h"
#include "creature.h"
#include "examine.h"
#include "chaos.h"
#include "arena.h"
#include "wizards.h"
#include "gfx.h"
#include "chaos_sprites.h"
#include "gamemenu.h"
#include "input.h"
#include "text16.h"
#include "string.h"

void display_creature_data(u8 id, u8 arena4, u8 arena3);
void display_wizard_data(u8 playerid);
void display_spell_data(u8 id);
void draw_stats(const u8 * stat_pointer);

void examine_creature(unsigned char index);

void examine_board(void) {
  // "examine board" selected on the menu...

  disable_interrupts();
  display_arena();
  set_border_col(0);
  cursor_x = 7;
  cursor_y = 4;
  draw_cursor(CURSOR_NORMAL_GFX);
  SET_CURSOR_POSITION(cursor_x, cursor_y, 0,0);
  redraw_cursor();
  enable_interrupts();

  current_screen = SCR_EXAMINE_BOARD;
}

void examine_back(void) {
  fade_down();
  show_game_menu();
  remove_cursor();
  fade_up();
}

void examine_square(unsigned char index) {
  
  // return instantly if we examine an empty square
  if (arena[0][index] == 0) {
    return;
  }
#ifdef __GBA__
  s32 bgx = bg_level.x_scroll;
  s32 bgy = bg_level.y_scroll;
#endif
  examine_creature(index);
  fade_down();
  display_arena();
  SET_CURSOR_POSITION(cursor_x, cursor_y, bgx,bgy);
  clear_message();
  set_border_col(current_player);
  redraw_cursor();
  display_cursor_contents(index);
  fade_up();
}

void examine_spell(unsigned char index) {
  arena[4][index] = 0;
  if (arena[0][index] > SPELL_DISBELIEVE && arena[0][index] < SPELL_GOOEY_BLOB) {
    examine_creature(index);
  } else {
    wait_for_letgo();
    fade_down();
    remove_cursor();
    clear_arena();
    clear_bg();
    clear_palettes();
    display_spell_data(arena[0][index]);
    fade_up();
    wait_for_keypress();
    wait_for_letgo();
  }
  
  
}



void examine_creature(unsigned char index) {
  // examine the given square of the arena...
  // c3b3
  u8 arena0 = arena[0][index];
  u8 arena4 = arena[4][index];
  wait_for_letgo();

  if (arena0) {
    // clear arena screen...
    fade_down();
    remove_cursor();
    clear_arena();
    clear_bg();
    clear_palettes();
    // set up the text colours used
    set_text16_colour(1, RGB16(30,31,0)); // yel
    set_text16_colour(2, RGB16(0,31,31)); // light blue
    set_text16_colour(3, RGB16(30,31,31)); // white
    set_text16_colour(4, RGB16(30,0,31)); // purple
    set_text16_colour(5, RGB16(0,31,0)); // green
    
    display_creature_data(arena0, arena4, arena[3][index]);
    fade_up();
    wait_for_keypress();
    wait_for_letgo();
    if (arena4) {
      fade_down();
      display_creature_data(arena4, 0, arena[3][index]);
      fade_up();
      wait_for_keypress();
      wait_for_letgo();
    }
    
  }
}

const char * const stats_strings [] = {
  "COMBAT=",
  "RANGED COMBAT=",
  "RANGE=",
  "DEFENCE=",
  "MOVEMENT ALLOWANCE=",
  "MANOEUVRE RATING=",
  "MAGIC RESISTANCE=",
  "CASTING CHANCE=",      // 7
  "SPELLS=",              // 8
  "ABILITY=",             // 9
};

// define the positions of the stats
typedef struct statpos {
  u8 startx;
  u8 starty; 
  u8 statx;
} statpos;

const statpos stats_postions[] = {
  {2, 5, 9},      // combat
  {2, 7, 16},     // ranged combat
  {19, 7, 25},    // range
  {2, 9, 10},     // defence
  {2, 11, 21},     // movement allowance
  {2, 13, 19},     // manoeuvre rating
  {2, 15, 19},     // magic resistance
  {2, 17, 17},     // casting chance
  {2, 17, 9},    // spells
  {12, 17, 20},     // ability
};


void display_creature_data(u8 id, u8 arena4, u8 arena3) {
  if (id != 0) {
    // clear text screen...
    clear_bg();
    draw_decor_border(15, RGB16(0,31,0),0); 
    if (id < WIZARD_INDEX) {
      // creature
      // c479
      // print the creature's name
      print_text16(CHAOS_SPELLS.pSpellDataTable[id]->spellName, 2,1, 1);
      
      // its chaos/law type
      if (CHAOS_SPELLS.pSpellDataTable[id]->chaosRating != 0) {
        u8 screen_x = 3+strlen(CHAOS_SPELLS.pSpellDataTable[id]->spellName);
        u8 col = 1;
        char str[30];
        if (CHAOS_SPELLS.pSpellDataTable[id]->chaosRating < 0) {
          // chaos value, drawn in purple
          col = 4; // purp
          print_text16("(CHAOS ", screen_x,1, col); 
          screen_x += 7;
          int2a((CHAOS_SPELLS.pSpellDataTable[id]->chaosRating*-1), str, 10);
          
          print_text16(str, screen_x++,1, col); 
          
        } else {
          // law, drawn in light blue
          col = 2; // l blue
          print_text16("(LAW ", screen_x,1, col); 
          screen_x += 5;
          
          int2a(CHAOS_SPELLS.pSpellDataTable[id]->chaosRating, str, 10);
          
          print_text16(str, screen_x++,1, col); 
          
        }
        print_text16(")", screen_x,1, col); 
        
      } // end chaos / law type display
      
      u8 need_comma = 0;
      u8 x = 2;
      // creature's special skills
      if (id >= SPELL_HORSE && id <= SPELL_MANTICORE) {
        // mount...
        char * str = "MOUNT";
        if (need_comma) {
          // draw a comma...
          print_text16(",", x,3, 5);
          x++;
        }
        // write value
        print_text16(str, x,3, 5);
        x+=strlen(str);
        need_comma = 1;
      }
      // draw any "inside" wizards - for mounts or trees/castles
      if (arena4 >= WIZARD_INDEX) {
        print_text16("(", x,3, 5);
        x++;
        print_text16(players[arena4-WIZARD_INDEX].name, x,3, 5);
        int namelength = strlen(players[arena4-WIZARD_INDEX].name);
        if ((id >= SPELL_PEGASUS && id <= SPELL_GHOST) && (arena3 & 0x40)
        && namelength > 6) { 
          // undead flying mount - need to save space...
          x+=6;
        }
        else
          x+=namelength;
        print_text16(")", x,3, 5);
        x++;
      }
      
      if (id >= SPELL_PEGASUS && id <= SPELL_GHOST) {
        // flying
        char * str = "FLYING";
        if (need_comma) {
          // draw a comma...
          print_text16(",", x,3, 5);
          x++;
        }
        // write value
        print_text16(str, x,3, 5);
        x+=strlen(str);
        need_comma = 1;
      }
      
      if ( (arena3 & 0x40) ||  (id >= SPELL_VAMPIRE && id <= SPELL_ZOMBIE)){
        // undead...
        char * str = "UNDEAD";
        if (need_comma) {
          // draw a comma...
          print_text16(",", x,3, 5);
          x++;
        }
        // write value
        print_text16(str, x,3, 5);
        x+=strlen(str);
        need_comma = 1;
      }
      
      draw_stats(&CHAOS_SPELLS.pSpellDataTable[id]->combat);
      
      // draw casting chance too if needed...
      if (cast_chance_needed) {
        char statval[10];
        u8 stat = 7;
        print_text16(stats_strings[stat], stats_postions[stat].startx,stats_postions[stat].starty, 2);
        
        int2a(current_spell_chance*10, statval, 10);
        strcat(statval, "/");
        
        
        print_text16(statval, stats_postions[stat].statx, stats_postions[stat].starty, 2);
      }
      
    } else {
      // wizard
      // c419
      display_wizard_data(id - WIZARD_INDEX);
    }
  }
}

// c419
// to get everything, POKE ac16,FE or POKE 44054,254
void display_wizard_data(u8 playerid) {
  // print name in yellow
  print_text16(players[playerid].name, 2,1, 1);
  u8 need_comma = 0;
  u8 x = 2;
  if (players[playerid].modifier_flag & 0x2) {
    // bit 1 set, "KNIFE"
    char * str = "KNIFE";
    if (need_comma) {
      // draw a comma...
      print_text16(",", x,3, 1);
      x++;
    }
    // write value
    print_text16(str, x,3, 1);
    x+=strlen(str);
    need_comma = 1;
  }
  if (players[playerid].modifier_flag & 0x4) {
    // bit 2 set,
    char * str = "SWORD";
    if (need_comma) {
      // draw a comma...
      print_text16(",", x,3, 1);
      x++;
    }
    // write value
    print_text16(str, x,3, 1);
    x+=strlen(str);
    need_comma = 1;
  }

  if ((players[playerid].modifier_flag & 0xC0) == 0xC0) {
    // "ARMOUR"
    char * str = "ARMOUR";
    if (need_comma) {
      // draw a comma...
      print_text16(",", x,3, 1);
      x++;
    }
    // write value
    print_text16(str, x,3, 1);
    x+=strlen(str);
    need_comma = 1;
  }
  if ((players[playerid].modifier_flag & 0xC0) == 0x40) {
    // "SHIELD"
    char * str = "SHIELD";
    if (need_comma) {
      // draw a comma...
      print_text16(",", x,3, 1);
      x++;
    }
    // write value
    print_text16(str, x,3, 1);
    x+=strlen(str);
    need_comma = 1;
  }

  if (HAS_MAGICWINGS(players[playerid].modifier_flag)) {
    // bit 5
    // FLYING
    char * str = "FLYING";
    if (need_comma) {
      // draw a comma...
      print_text16(",", x,3, 1);
      x++;
    }
    // write value
    print_text16(str, x,3, 1);
    x+=strlen(str);
    need_comma = 1;
  }

  if (HAS_SHADOWFORM(players[playerid].modifier_flag)) {
    // bit 3
    // SHADOW
    char * str = "SHADOW";
    if (need_comma) {
      // draw a comma...
      print_text16(",", x,3, 1);
      x++;
    }
    // write value
    print_text16(str, x,3, 1);
    x+=strlen(str);
    need_comma = 1;
  }

  // now draw the actual stats...
  draw_stats(&players[playerid].combat);
  
  // spells
  char statval[3];
  u8 stat = 8;
  print_text16(stats_strings[stat], stats_postions[stat].startx,stats_postions[stat].starty, 2);
  int2a(players[playerid].spell_count, statval, 10);
  
  print_text16(statval, stats_postions[stat].statx, stats_postions[stat].starty, 3);
  
  // and ability...
  stat = 9;
  print_text16(stats_strings[stat], stats_postions[stat].startx,stats_postions[stat].starty, 2);
  int2a(players[playerid].ability, statval, 10);
  
  print_text16(statval, stats_postions[stat].statx, stats_postions[stat].starty, 3);
  
}

void display_spell_data(u8 id) {
  // this is used for displaying a spell sheet...
  // set up the colours
  set_text16_colour(1, RGB16(30,31,0)); // yel
  set_text16_colour(2, RGB16(0,31,31)); // light blue
  set_text16_colour(3, RGB16(30,31,31)); // white
  set_text16_colour(4, RGB16(30,0,31)); // purple
  set_text16_colour(5, RGB16(0,31,0)); // green
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
  // code from 94e2
  // write spell name
  u8 start_x = 5;
  u8 start_y = 4;
  print_text16(CHAOS_SPELLS.pSpellDataTable[id]->spellName, start_x,start_y, 1);
  start_y += 2;
  // its chaos/law type
  if (CHAOS_SPELLS.pSpellDataTable[id]->chaosRating != 0) {
    u8 screen_x = start_x;
    u8 col = 1;
    char str[30];
    if (CHAOS_SPELLS.pSpellDataTable[id]->chaosRating < 0) {
      // chaos value, drawn in purple
      col = 4; // purp
      print_text16("(CHAOS ", screen_x,start_y, col); 
      screen_x += 7;
      
      int2a((CHAOS_SPELLS.pSpellDataTable[id]->chaosRating*-1), str, 10);
      print_text16(str, screen_x++,start_y, col); 
      
    } else {
      // law, drawn in light blue
      col = 2; // l blue
      print_text16("(LAW ", screen_x,start_y, col); 
      screen_x += 5;
      
      int2a(CHAOS_SPELLS.pSpellDataTable[id]->chaosRating, str, 10);
      print_text16(str, screen_x++,start_y, col); 

      
    }
    print_text16(")", screen_x,start_y, col); 
    
  } // end chaos / law type display
  start_y += 4;
  // casting chance...
  print_text16(stats_strings[7], start_x,start_y, 5);
  char statval[10];
  int2a(current_spell_chance*10, statval, 10);
  strcat(statval, "/");
  
  print_text16(statval, start_x+15, start_y, 1);
  start_y+=4;
  // casting range
  print_text16(stats_strings[2], start_x,start_y, 5);
  u8 A = CHAOS_SPELLS.pSpellDataTable[id]->castRange>>1;
  if (A > 10)
    A = 20;
  int2a(A, statval, 10);
  
  
  print_text16(statval, start_x+6, start_y, 1);
}


void draw_stats(const u8 * stat_pointer) {
  u8 i;
  // u8 stat;
  // loop over the 7 stats... this is a nice way to do it as it works for creatures and wizards, 
  // but it relies on structures being unpadded (devkitadvance doesn't pad to 4 byte boundaries)
  char statval[3];
  
  for (i = 0; i < 7; i++) {
    // write the description in lblue
    print_text16(stats_strings[i], stats_postions[i].startx,stats_postions[i].starty, 2);
    // write the stat in white
    int2a(*stat_pointer, statval, 10);
    
    print_text16(statval, stats_postions[i].statx, stats_postions[i].starty, 3);
    
    // point to next stat
    stat_pointer++;
  }


}
