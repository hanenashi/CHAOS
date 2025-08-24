#include "platform.h"

#ifdef __GBA__
#include "keypad.h"
#include "screenmode.h"
#include "sound.h"
#elif defined(__WINDOWS__)
#include "winkeys.h"
#include "winsound.h"
#endif

#include "sound_data.h"
#include "spellselect.h"
#include "chaos.h"
#include "gfx.h"
#include "arena.h"
#include "examine.h"
#include "spelldata.h"
#include "casting.h"
#include "wizards.h"
#include "creature.h"
#include "string.h"
#include "text16.h"
#include "gamemenu.h"
#include "input.h"

char cast_chance_needed; 

void set_spell_palette(void);
void list_spells(void);

void select_spell(u8 item);
void deselect_spell(u8 item);

void print_spell_name(u8 spellid, u8 x, u8 y);
void generate_spell_string(char * str, u8 spellid);

u8 top_index;

////////////////////////////////////////

void show_spell_screen(void) {
  char str[30];
  
  top_index = 0;
  hilite_item = 0;
  
  // need to sort out the players spells here...
  // should remove all 0 spells and update spell count accordingly
  if (!IS_CPU(current_player))
    remove_null_spells();
  
  current_screen = SCR_SELECT_SPELL;
  clear_bg();
  clear_arena();
  clear_bg();
  clear_palettes();
  // set up the palette so that 0-5 are the spell colours
  set_spell_palette();
  
  // 6 can be the default colour, 10 is the selected colour
  strcpy(str, players[current_player].name);
  strcat(str, "'S SPELLS");
//  sprintf(str, "%s'S SPELLS", players[current_player].name);
// TESTED OK
  print_text16(str, 1,1, 6);  
  set_text16_colour(6, RGB16(0,30,30));
  
  // ask if they want an illusion and wait for response
  set_text16_colour(14, RGB16(21,21,29));
  print_text16("PRESS R TO", 18,4, 14);  
  print_text16("EXAMINE", 18,6, 14);
  
  // write all the spells
  list_spells();
  select_spell(0);
  
}


void spell_select_up(void) {
  // need to redo the spell list when moving up, possibly.
  // to move up, deselect the current spell
  // change the hilite_item
  // select the new hilite item
  // BUT if when you move up top_index > 0
  // check if you need to scroll the list a bit
  
  if (top_index == 0) {
    // no need to scroll
    deselect_spell(hilite_item);
    if (hilite_item > 0)
      hilite_item--;
    select_spell(hilite_item);
  } else {
    deselect_spell(hilite_item);
    if (hilite_item > 1)
      hilite_item--;
    else {
      top_index--;
      list_spells();
    }    
    select_spell(hilite_item);
  }
  
    
}

void spell_select_down(void) {
  if ( (top_index + 7) >= players[current_player].spell_count) {
    // no need to scroll
    deselect_spell(hilite_item);
    if ((hilite_item + top_index) < players[current_player].spell_count)
      hilite_item++;
    select_spell(hilite_item);
  } else {
    deselect_spell(hilite_item);
    if (hilite_item < 6)
      hilite_item++;
    else {
      top_index++;
      list_spells();
    }    
    select_spell(hilite_item);
  }
  
}

void spell_select_left(void) {
  // go to top of list
  deselect_spell(hilite_item);

  top_index = 0;
  hilite_item = 0;
  
  list_spells();
  select_spell(hilite_item);
}
void spell_select_right(void) {
  // go to end of list
  deselect_spell(hilite_item);
  if (  players[current_player].spell_count < 8) {
    hilite_item = players[current_player].spell_count-1;
  } else {
    top_index = players[current_player].spell_count - 8;
    hilite_item = 7;
  }
  list_spells();
  select_spell(hilite_item);
}

void spell_select_r(void) {
  
  cast_chance_needed = 1;
  u8 spellindex = hilite_item+top_index;
  u8 selected_spell = players[current_player].spells[spellindex*2+1];
  arena[0][15] = selected_spell;
  
  u8 ti = top_index;
  u8 hi = hilite_item;
  examine_spell(15);
  arena[0][15] = 0;
  fade_down();
  show_spell_screen();
  cast_chance_needed = 0;
  hilite_item = hi;
  top_index = ti;
  list_spells();
  select_spell(hi);
  fade_up();
}


void spell_select_a(void) {
  
  // store the spell...
  u8 spellindex = hilite_item+top_index;
  players[current_player].illusion_cast = 0;
  players[current_player].selected_spell = spellindex*2+1;
  // check for illusion...
  if (players[current_player].spells[players[current_player].selected_spell] < SPELL_KING_COBRA ||
    players[current_player].spells[players[current_player].selected_spell] >= SPELL_GOOEY_BLOB) {
    PlaySoundFX(SND_CHOSEN);
    fade_down();
    show_game_menu();
    fade_up();
    return;
  }

  // ask if they want an illusion and wait for response
  set_text16_colour(14, 0);  
  set_text16_colour(12, RGB16(31,31,31)); // white
  print_text16("ILLUSION?", 18,8, 12);  
  print_text16("(A=YES B=NO)", 17,10, 12);

  wait_for_letgo();
  while (1) {
    wait_vsync_int();
    anim_spell_select();
    UpdateKeys();
    if (KeyPressedNoBounce(KEY_A,50))
      break;
    if (KeyPressedNoBounce(KEY_B,50)) {
      PlaySoundFX(SND_CHOSEN);
      wait_for_letgo();
      fade_down();
      show_game_menu();
      fade_up();
      return;
    }
  }
  PlaySoundFX(SND_CHOSEN);
  wait_for_letgo();
  players[current_player].illusion_cast = 1;
  fade_down();
  show_game_menu();
  fade_up();
  
}

void spell_select_b(void) {
  fade_down();
  show_game_menu();
  fade_up();
}

const static u16 CastingChancePalette[6] = {
  RGB16(31,0,0),    //Red = 0-10%       p
  RGB16(31,0,31),   //purple = 20-30%   
  RGB16(0,31,0),    //Green = 40-50%    
  RGB16(0,31,31),   //LightBlue = 60-70%
  RGB16(31,31,0),   //Yellow = 80-90%   
  RGB16(31,31,31),  //White = 100%      
};

void set_spell_palette(void) {
  u8 loop; 
  for (loop = 0; loop < 6; loop++) {
     set_text16_colour(loop, CastingChancePalette[loop]);
  }
}

void list_spells(void) {
  // list the spells for current_player
  // this needed a rethink because 20 spells can't fit on screen
  // a list of 7 spells, "scroll" the list when you reach the top or bottom
  //  - left and right jump a page of spells up/down
  
  // need to know... current top index, spell count, selected index.
  // when press down, move down one, but don't scroll unless selected - top_index == 6 and we aren't at the end of the lsit
  // when up is pressed, move up one, but don't scroll unless selected - top_index == 1
  // also, I'll need arrows to show more spells are available off the end of the list
  wait_vsync_int();
  
  u16 loop = 0;
  // set the first spell index to the spell at the top of the list
  int spellindex = top_index;
  
  int index;
  int  y = 3;
  
  while (loop < 8 && spellindex < 20) {
    index = 1+spellindex*2;
    if (players[current_player].spells[index] != 0) {
      print_spell_name(players[current_player].spells[index], 1, y);
      y+=2;
      loop++;
    }
    spellindex++;
  }
  
  int x;
  // draw the up down arrows...
  // these are the first part of character "^" flipped vertically for "v"
  // a bit of a hack - it uses the top half of the law symbol!
  u8 letter = ('+' - FIRST_CHAR_INDEX);
  
  x = 16; y = 3;
#ifdef __WINDOWS__
  x = 18; y = 4;
#endif
  set_text16_colour(7, RGB16(31,0,0));

  if (top_index > 0) {
    put_char8(letter, x, y, 7, 0);
  } else {
    put_char8(0, x, y, 7, 0);
  }
  
  y = 17;
#ifdef __WINDOWS__
  y = 18;
#endif
  
  if ((top_index + 7) < players[current_player].spell_count) {
    // set the pallette to be 11, flashing red say...
    put_char8(letter, x, y, 7, TILE_FLIP_VERT);
  } else {
    put_char8(0, x, y, 7, 0);
  }
}

void print_spell_name(u8 spellid, u8 x, u8 y) {
  // parse the spell id into a proper spell name 
  // print it to the screen at the given location with the right colour
  char str[30];
  current_spell = spellid;
  set_current_spell_chance();
  generate_spell_string(str, spellid);
  current_spell_chance++;
  print_text16("              ", x, y,  current_spell_chance>>1);
  print_text16(str, x, y,  current_spell_chance>>1);
  
}

void generate_spell_string(char * str, u8 spellid) {
  char chaoslvl = '-';
  if (CHAOS_SPELLS.pSpellDataTable[spellid]->chaosRating > 0)
    chaoslvl = '+';
  else if (CHAOS_SPELLS.pSpellDataTable[spellid]->chaosRating < 0)
    chaoslvl = '*';
  str[0] = chaoslvl;
  str[1] = 0;
  strcat(str,CHAOS_SPELLS.pSpellDataTable[spellid]->spellName);
}



void select_spell(u8 item) {
  // select the spell at index "item"
  
  // get the actual spell id index
  u8 spellindex = item+top_index;
  
  char str[30];
  
  generate_spell_string(str, players[current_player].spells[spellindex*2+1]);
/*
#if defined (__WINDOWS__)
  // the problem here is that just redefining the palette is not enough
  // will have to redraw the text too... and that is too awful to think of
  set_text16_colour(10, RGB16(31,0,0));
#endif
*/

  print_text16(str, 1,3+(item*2), 10);
  
  current_spell = players[current_player].spells[spellindex*2+1];
  set_current_spell_chance();
  current_spell_chance++;
  
  print_text16("CASTING", 18,14, 2);
  print_text16("CHANCE=    ", 18,16, 2);
  int2a(current_spell_chance*10, str, 10);
  strcat(str,"/");
  print_text16(str, 25,16, 5/*current_spell_chance>>1*/);
  
//#ifdef __GBA__  
  u8 r = GetRed(CastingChancePalette[current_spell_chance>>1]);
  u8 g = GetGreen(CastingChancePalette[current_spell_chance>>1]);
  u8 b = GetBlue(CastingChancePalette[current_spell_chance>>1]);
/*#elif defined (__WINDOWS__)
  Uint32 col = winCol((CastingChancePalette[current_spell_chance>>1]));
  Uint8 r, g, b;
  SDL_GetRGB(col, screen->format, &r, &g, &b);
#endif*/
  r = r + 31;
  g = g + 31;
  b = b + 31;
  
  r = r>31?31:r;
  g = g>31?31:g;
  b = b>31?31:b;
  
  set_text16_colour(10, RGB16(r,g,b));

  anim_col = -15;
  anim_col_grad = -8;
//  u8 x, y;
//  x=10; y=1; 
//  bg_level.mapData[x+y*32] = 49;
//  x++;
//  bg_level.mapData[x+y*32] = 50;
//  x--; y++;
//  bg_level.mapData[x+y*32] = 79;
//  x++;
//  bg_level.mapData[x+y*32] = 80;
//  if (current_spell >= SPELL_KING_COBRA && current_spell <= SPELL_WALL) {
//    draw_creature(9,0, current_spell, 0);
//  } else {
//    clear_square(9,0);
//  }
  
}

void deselect_spell(u8 item) {
  
  // get the actual spell id index
  u8 spellindex = item+top_index;
  
  char str[30];
  
  generate_spell_string(str, players[current_player].spells[spellindex*2+1]);
  
  
  current_spell = players[current_player].spells[spellindex*2+1];
  set_current_spell_chance();
  current_spell_chance++;
  print_text16(str, 1,3+(item*2), current_spell_chance>>1);
  
}

void anim_spell_select(void) {
  
  u8 spellindex = hilite_item+top_index;
  current_spell = players[current_player].spells[spellindex*2+1];
  set_current_spell_chance();
  current_spell_chance++;
  
  u8 r = GetRed(CastingChancePalette[current_spell_chance>>1]);
  u8 g = GetGreen(CastingChancePalette[current_spell_chance>>1]);
  u8 b = GetBlue(CastingChancePalette[current_spell_chance>>1]);  
  
  anim_selection(10, r, g, b);

}

// new code, this isn't from speccy chaos but
// helps when dealing with the list of spells
void remove_null_spells(void) {
  u8 i;
  u8 new_count = 0;
  u8 new_spells[20];
  for (i = 0; i < 20; i++) {
    if (players[current_player].spells[i*2+1] != 0) {
      new_spells[new_count++] = players[current_player].spells[i*2+1];
    }
  }
  // copy the valid spells back again
  for (i = 0; i < 20; i++) {
    if (i < new_count) {
      players[current_player].spells[i*2]   = 0x12; // default priority...
      players[current_player].spells[i*2+1] = new_spells[i];
    } else {
      players[current_player].spells[i*2+1] = 0;
    }
  }
  players[current_player].spells[0] = 0;
  players[current_player].spell_count = new_count-1;
  
}

