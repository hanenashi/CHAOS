// casting.c
#include "platform.h"

// called after the arena is drawn...
#ifdef __GBA__
#include "agbprint.h"
#include "sound.h"
#include "screenmode.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#include "remote.h"
#include "network.h"
#include "options.h"
#endif
#if defined (WIN32) || defined(__GBA__)
#include <stdlib.h>
#endif
#include "sound_data.h"
#include "string.h"
#include "casting.h"
#include "chaos_sprites.h"
#include "arena.h"
#include "spelldata.h"
#include "chaos.h"
#include "gfx.h"
#include "wizards.h"
#include "creature.h"
#include "computer.h"
#include "movement.h"
#include "text16.h"
#include "input.h"
//int abs(int);
unsigned char current_spell;
signed char current_spell_chance;
unsigned char temp_illusion_flag;
unsigned char temp_cast_amount;
unsigned char temp_success_flag;


void setup_human_player_cast(void);

void remove_pixel(unsigned short x, unsigned short y);
void draw_line_segment(unsigned short x, unsigned short y);
void set_line_segment(unsigned short x, unsigned short y);
void set_pixel(unsigned short x, unsigned short y);

//////////////////////////////////////////////////

// approximately at 95ee
void start_cast_round(void) {
  if (current_player < playercount) {
    temp_cast_amount = 0;
    if (IS_WIZARD_DEAD(players[current_player].modifier_flag)) {
      // player is dead...
      update_creaturecount();
      current_player++;
      start_cast_round();
    } else {
      current_screen = SCR_CASTING;
      hilite_item = 0;
      // this moves to the player, even if they don't have a spell to cast
      // need to rethink this....
      set_current_player_index();
      if (IS_CPU(current_player)) {
        // cpu spell casting...
        // jump 96f3
        remove_cursor();
        do_ai_spell();
        delay(10);
        update_creaturecount();
        current_player++;
        start_cast_round();
      } else {
#ifdef __WINDOWS__
	if (ONLINE_MODE && IS_REMOTE(current_player)) {
#ifdef DEBUG_NET
          printf("ONLINE MODE and remote %d\n", current_player);
#endif
          current_spell = players[current_player].spells[players[current_player].selected_spell];
	  if (current_spell != 0) {
	    set_current_spell_chance();
	    do_remote_spell();
	  }
	  update_creaturecount();
	  current_player++;
	  start_cast_round();
	} else {
#endif	
        current_spell = players[current_player].spells[players[current_player].selected_spell];
        if (current_spell == 0) {
          update_creaturecount();
          current_player++;
          start_cast_round();
        } else {
          
          if (current_spell != SPELL_DISBELIEVE) {
            // set the current spell to 0 if it isn't disblv.
            players[current_player].spells[players[current_player].selected_spell] = 0;
          }
          players[current_player].selected_spell = 0; // set to 0 for "no spell selected"
          
          // sets the success flag, etc
          setup_human_player_cast();
          
          // auto cast certain spells...
          if (current_spell == SPELL_MAGIC_WOOD || current_spell == SPELL_TURMOIL ||
          (current_spell >= SPELL_MAGIC_SHIELD && current_spell <= SPELL_SHADOW_FORM) ) {
            remove_cursor();
#ifdef __WINDOWS__
	    if (ONLINE_MODE /*&& IS_LOCAL(current_player)*/) {
	      // send a message to everyone saying what we are up to
	      // this is because this spell is "autocast" by pressing any key
	      send_casting_a();
            }
#endif 
            CHAOS_SPELLS.pSpellDataTable[current_spell]->pFunc();
            next_player_cast();
            
          } else if (current_spell >= SPELL_VENGEANCE && current_spell <= SPELL_JUSTICE) {
            // set up the casting chance first... if fails go to the next player
            set_spell_success();
#ifdef __WINDOWS__
	    if (ONLINE_MODE /*&& IS_LOCAL(current_player)*/) {
	      // send a message to everyone saying what we are up to
	      // this is because this spell is "autocast" by pressing any key
	      send_casting_a();
	    }
#endif 
            if (temp_success_flag == 0) {
              // print spell success/fail message...
              print_success_status();
              delay(20);
              temp_cast_amount = 0;
              next_player_cast();
            } 
          } else {
            draw_cursor(CURSOR_SPELL_GFX);
            redraw_cursor();
          }
        }
        
#ifdef __WINDOWS__
	// close "else not online or is local" 
	}
#endif
      }
    } 
  } else {
    // start movement round..
    unset_moved_flags();
    round_count++;
    clear_message();
    current_player = 0;
    spread_fire_blob();
    destroy_castles();
    random_new_spell();
    // here, check that there are enough wizards left to carry on
    
    if (dead_wizards == (playercount-1)) {
      // uh oh -  no wizards left, do winner screen
      win_contest();
      return;
    }
    
    start_movement_round();
  }
  
}

void casting_r(void) {
  // move to the next player...
  if (hilite_item < playercount-1)
    hilite_item++;
  else 
    hilite_item = 0;
    
  // move cursor to the hilited player
//  move_cursor_to(all_creatures[hilite_item+1].xpos, all_creatures[hilite_item+1].ypos);
}

void casting_a(void) {
  
  // cast the spell 
  u8 x, y;
  get_cursor_pos(&x, &y);
  u16 spellid = current_spell;
  
#ifdef __WINDOWS__
  if (ONLINE_MODE && IS_LOCAL(current_player)) 
    send_casting_a();
#endif 

  remove_cursor();
  CHAOS_SPELLS.pSpellDataTable[spellid]->pFunc();
  redraw_cursor();
  
  if (temp_cast_amount == 0 ) {
    // next players turn
    next_player_cast();
  }
  
}

void casting_b(void) {
  // cancel the spell
  temp_cast_amount = 0;
  next_player_cast();
}


// set up the next player ready for casting a spell
void next_player_cast(void) {
  // need checks here for if CPU, if less than playercount, etc,
  if (current_player < playercount) {
    update_creaturecount();
    current_player++;
    draw_cursor(CURSOR_NORMAL_GFX);
    start_cast_round();
  } else {
    // end round
    
  }
  
  
}


// 92f9
void set_current_spell_chance(void) {
  // sets the current spell chance based on default value, world chaos and wizard ability
  // just set to default for now
  s8 current_spell_chaos = CHAOS_SPELLS.pSpellDataTable[current_spell]->chaosRating;
  current_spell_chance = CHAOS_SPELLS.pSpellDataTable[current_spell]->castChance;
  if (world_chaos > 0 && current_spell_chaos > 0) {
    current_spell_chance += (world_chaos)>>2;
  } else if (world_chaos < 0 && current_spell_chaos < 0) {
    current_spell_chance += (world_chaos*-1)>>2;
  }
  // 9353 - add ability
  current_spell_chance += players[current_player].ability;
  
  if (current_spell_chance >= 0x0a) {
    current_spell_chance = 9;
  }
  
  if (current_spell_chance < 0) {
    current_spell_chance = 0;
  }
  
}


// based on 9786
unsigned char is_spell_in_range(unsigned char index1, unsigned char index2, unsigned char range) {
  
  u16 distance;
  get_distance(index1, index2, &distance);
  
  if (range >= distance) {
    return 1;
  }
  return 0;
  
}

// code based on that at 9856 - after S is pressed it validates the players spell cast choice
unsigned char player_cast_ok(void) {
  // check spell is in range .... call 9786
  if (!is_spell_in_range(wizard_index, target_index, CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange)) {
    clear_message();
    print_text16("OUT OF RANGE", MESSAGE_X, MESSAGE_Y, 12);
    set_text16_colour(12, RGB16(30,31,0)); // yel
    return 0;
  }
  // in range, so do some more checks 
  // 9877...
  if (arena[0][target_index] != 0) {
    if (current_spell >= SPELL_MAGIC_WOOD) {
      return 0;
    }
    if (arena[2][target_index] != 4) {
      // creature is not dead, can't cast here.
      return 0;
    }
    
  }
  // 9892... 
  // do tree check
  if (is_tree_adjacent(target_index)) {
    return 0;
  }
  // do wall check
  if (is_wall_adjacent(target_index)) {
    return 0;
  }
  
  // do LOS check...
  if (los_blocked(target_index, 0)) {
    clear_message();
    print_text16("NO LINE OF SIGHT", MESSAGE_X, MESSAGE_Y, 12);
    set_text16_colour(12, RGB16(31,30,0)); // lblue
    return 0;
  }
  // call spell anim, etc...
  return 1;
}


// do the tree check.. code from 98f1
int is_tree_adjacent(int index) {
  if (current_spell < SPELL_MAGIC_WOOD || current_spell >= SPELL_MAGIC_CASTLE)
    return 0;
  
  u8 surround_index = 0;
  u8  i = 0;
  u8  lookat_i;
  u8  found = 0;
  for (i = 0; i < 8; i++) {
    lookat_i = apply_position_modifier(index, surround_index);
    surround_index++;
    if (lookat_i == 0) {
      // out of bounds
      continue;
    }
    lookat_i--;
    if (arena[0][lookat_i] < SPELL_MAGIC_WOOD || arena[0][lookat_i] >= SPELL_MAGIC_CASTLE) {
      // not a wood
      continue;
    } else {
      found = 1;
    }
  }
  return found;
}

// 9c0f code - check that the casting wizard isn't next to the wall target square
int is_wall_adjacent(int index) {
  if (current_spell != SPELL_WALL)
    return 0;
  
  u8 surround_index = 0;
  u8 i = 0;
  u8 lookat_i;
  u8 found = 0;
  for (i = 0; i < 8; i++) {
    lookat_i = apply_position_modifier(index, surround_index);
    surround_index++;
    if (lookat_i == 0) {
      // out of bounds
      continue;
    }
    lookat_i--;
    if (arena[0][lookat_i] == (WIZARD_INDEX + current_player)) {
      // casting wizard is adjacent
      found = 1;
    } /*else if (arena[0][lookat_i] < WIZARD_INDEX && arena[4][lookat_i] != 0) {
      // casting wizard is adjacent and in a wood/mount etc
      if (arena[4][lookat_i] == (WIZARD_INDEX + current_player))
        found = 1;
    }*/
  }
  return found;
  
}




int line_type;
int line_table_offset;
int segment_count;
int line_length;
int tmp_gfx;
int line_end;
int colour;
// check line of sight between start_index and the given index
unsigned char los_blocked(unsigned char index, unsigned char gfx) {
  
  // remove fire, ghost wraiths, dead creatures, target squ, casting wizard
  // but only if we are doing the LOS check.. otherwise scroll the screen to the target square
  //if (gfx == 0) 
  //  remove_los_targets(index);
  if (gfx != 0)
    move_screen_to(index);
    
  switch (gfx) {
    case 0: colour = 0; line_table_offset = 0;    line_type = 0; break;
    case 1: colour = RGB16(31,31,31); line_table_offset = 0x2A; line_type = 1; break; // arrow
    case 2: colour = RGB16(31,31,0); line_table_offset = 0x0E; line_type = 2; break; // short arrow attack
    case 3: colour = RGB16(0,31,31); line_table_offset = 0x3C; line_type = 3; break; // spell
    case 4: colour = RGB16(31,0,0); line_table_offset = 0x64; line_type = 3; break; // dragon
    case 5: colour = RGB16(0,31,10); line_table_offset = 0x08; line_type = 3; break; // bolt
    case 6: colour = RGB16(31,25,10); line_table_offset = 0x64; line_type = 3; break; // lightning
    default: line_type = 0x0; break; // arrow
  }
  
  // init segment counter
  segment_count = 0;
  line_length = 0;
  tmp_gfx = gfx;
  // check from start_index to index if LOS
  
  u8 los = 0; // line of sight OK
  
  
  u16 deltax, deltay, numpixels;
  s16 i,d, dinc1, dinc2;
  s16 xinc1, xinc2;
  s16 yinc1, yinc2;
  u8 currentx,currenty;
  u8 startx, starty, targetx, targety;
  
  get_yx_upper(start_index, &starty, &startx);
  startx-=8; starty-=8;
  
  get_yx_upper(index, &targety, &targetx);
  targetx-=8; targety-=8;
  
  
  deltax = abs(startx - targetx);
  deltay = abs(starty - targety);
  
  
  if(deltax >= deltay) {
    //If x is independent variable
    numpixels = deltax + 1;
    d = (2 * deltay) - deltax;
    dinc1 = deltay << 1;
    dinc2 = (deltay - deltax) << 1;
    xinc1 = 1;
    xinc2 = 1;
    yinc1 = 0;
    yinc2 = 1;
  }
  else
  {
    //If y is independant variable
    numpixels = deltay + 1;
    d = (2 * deltax) - deltay;
    dinc1 = deltax << 1;
    dinc2 = (deltax - deltay) << 1;
    xinc1 = 0;
    xinc2 = 1;
    yinc1 = 1;
    yinc2 = 1;
  }
  
  //Move the right direction
  if(startx > targetx)
  {
      xinc1 = -xinc1;
      xinc2 = -xinc2;
  }
  if(starty > targety)
  {
      yinc1 = -yinc1;
      yinc2 = -yinc2;
  }
  
  currentx = startx;
  currenty = starty;
  
  line_end = targetx+targety*16;
  for(i = 0; i < numpixels; i++)    {
    if (gfx == 0) {
      if (is_pixelset(currentx, currenty, index, start_index) != 0) {
	// could add the check for los creatures here
	// then if it is eg. a tile with a transparent creature, no los set
        los = 1;
	//return los;
      } 
    } else {
      // not the LOS check... do the line drawing
      draw_line_segment(currentx, currenty);  
    }
    
    if(d < 0) {
      d = d + dinc1;
      currentx = currentx + xinc1;
      currenty = currenty + yinc1;
    }
    else {
      d = d + dinc2;
      currentx = currentx + xinc2;
      currenty = currenty + yinc2;
    }
  }
  if (gfx != 0)
    load_all_palettes();
    
  return los;
}

u8 line_table[0x68];  // the line table stores the current positions...

// based on code at b7d3 and b84d
void draw_line_segment(unsigned short x, unsigned short y) {
  s16 i;
  line_table[0] = (u8)x;
  line_table[1] = (u8)y;

  /* great big hack coming up.... 
   * 
   * regarding the ifdef's around the wait for vsync bits: This is really
   * needed for the gba version, but not so much for the pc versions.  
   *
   * On gba it takes at least 2 frames to really notice the new gfx updates,
   * but waiting every frame makes the line go too slow.  Dragon's breath (3)
   * takes less time to draw, so need to wait a bit more to actually see it.
   *
   * on the pc, waiting the same amount as on the gba makes the line too slow,
   * so wait less.
   * 
   * all this goes to pot if we change the compilation to use more or less
   * optimisation! -O1 is the plan.
   */ 
  set_line_segment(x,y);
  // if reach the end of line, jump...
  if ( (x+(y*16)) != line_end) {
#ifdef __WINDOWS__
    if ((segment_count & 15) == 1) {
#else
    if ( ((segment_count & 1) == 0)) { 
#endif
      wait_vsync_int();
    }
    for (i = 0x64; i >= 0; i--) {
      line_table[i+2] = line_table[i];
    }
    
    if (line_length < line_table_offset) {
      line_length += 2;
    }
    u8 newx, newy;
    newx = line_table[line_length];
    newy = line_table[line_length+1];
    
    line_type |= 0x80;
    set_line_segment(newx,newy);
    
    line_type = line_type & 0x7f;
  }  else {
    // end of line reached
    u8 A = (line_length>>1) + 2;
    u8 index = line_length;
    for (i = 0; i < A; i++) {
      u8 newx, newy;
      newx = line_table[index];
      newy = line_table[index+1];
      line_type |= 0x80;
#ifdef __WINDOWS__
      if ((i & 3) == 1) {
#else
      if ( ((i & 1) == 0) || line_type == 3) {
#endif
        wait_vsync_int();
      }
      set_line_segment(newx,newy);
      
      index -= 2;
    }
    
  }
  
  
}

void set_line_segment(unsigned short x, unsigned short y) {
  u8 A = line_type & 0x7f;
  if (A == 3 ) {
    // b85d...
    set_pixel(x,y);
    set_pixel(x,y+1);
    set_pixel(x,y-1);
    set_pixel(x+1,y);
    set_pixel(x-1,y);
    
    if ( (tmp_gfx & 0x4) == 0x4) {
      set_pixel(x-2,y+2);
      set_pixel(x,y+3);
      set_pixel(x+2,y+2);
      set_pixel(x+3,y);
      set_pixel(x+2,y-2);
      set_pixel(x,y-3);
      set_pixel(x-2,y-2);
      set_pixel(x-3,y);
      
    }
    
  } else {
    // draw pixel at x,y and return
    set_pixel(x,y);
  }
  segment_count++;
}


// sets or unsets pixel depending on line segment and other things
// b8bd...
void set_pixel(unsigned short x, unsigned short y) {
  
  if ( (line_type & 0x80) == 0x80) {
    // remove pixel here
    remove_pixel(x,y);
    return;
  }
  if ( (tmp_gfx & 0x4) != 0) {
    if ( (segment_count & 0x7) != 0 ) 
      return;
  }
  
  
#ifdef __GBA__
  draw_pixel_4bpp(x, y);
  u16 tile = ((x/8)+1) + ((y/8)+1)*32;
  //u16 tile = 1 + (x>>3) + ((y>>3)*30);
  // get current palette... copy here..
  u8 pal = (bg_level.mapData[33+tile] & 0xF000)>>12;
  BGPaletteMem[pal*16 + 0xF] = colour;
#elif defined(__WINDOWS__)
  
  int tile = get_win_tile(x,y);
  int pal = g_tilePalette8[tile];
  BGPaletteMem[pal*16 + 0xF] = winCol(colour);
  draw_pixel_4bpp(x, y);
#endif
  
}



void remove_pixel(unsigned short x, unsigned short y) {
  // remove the pixel at x, y
#ifdef __GBA__
  // completely hardware dependent this...
  u16 tile = 1 + (x>>3) + ((y>>3)*30);
  // then from here it's easy (ish) just work out the remainder from /8
  u8 xrem = x&0x7;
  u8 yrem = y&0x7;
  
  u16 address = tile*0x10 /*16 16-bit vals per tile*/ + (xrem>>2) + (yrem<<1);
  
  u16 pixel = bg_level.tileData[address];
  
  if ((x & 3) == 0) {
    pixel &= 0xFFF0;
  } else if ((x & 3) == 1) {
    pixel &= 0xFF0F;
  } else if ((x & 3) == 2) {    
    pixel &= 0xF0FF;
  } else if ((x & 3) == 3) {    
    pixel &= 0x0FFF;
  }
  
  bg_level.tileData[address] = pixel;
#elif defined(__WINDOWS__)
  // do some SDL stuff
  clear_pixel(x, y);
#endif
}


// do a spell anim from start_index to target_index
// delay (4) first to make sure we are drawn..
// based on code at a18a

u8 spellframetable[0x12] = {
  0, 1, 2, 3,
  0, 1, 2, 3,
  0, 1, 2, 3,
  4, 5, 6, 7,
  8, 8,
};


// do the spell animation between start_index and target_index
void spell_animation(void) {
  
  delay(4);
  // don't do spell line anim for wizard modifier spells...
  if (CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange != 0 
  || current_spell == SPELL_TURMOIL) {
    PlaySoundFX(SND_BEAM);
    los_blocked(target_index, 3);
    // wait for redraw..
    delay(4);
  }
  // draw the spell animation gfx...
  u8 i;
  u8 x, y;
  get_yx(target_index, &y, &x);
  x--;
  y--;
  PlaySoundFX(SND_SPELLSUCCESS);
  //char str[20];
  for (i = 0; i < 0x12; i++) {
    wait_vsync_int();
    draw_spellcast_frame(x, y, spellframetable[i]);
  }
  
}

// based on code at 9760
// only used for cpu player - and for chaos/law spells
void set_spell_success(void) {
  temp_success_flag = 0;
  if (temp_illusion_flag) {
    temp_success_flag = 1;
  } else {
    u8 r = GetRand(10);
    if (r < (current_spell_chance + 1) ) {
      temp_success_flag = 1;
      world_chaos += CHAOS_SPELLS.pSpellDataTable[current_spell]->chaosRating;
    } 
  } 
#ifdef DEBUG
  temp_success_flag = 1;
#endif
#if 0
#ifdef DEBUG
  temp_success_flag = 1;
#else
  temp_success_flag = 1;
  if (!temp_illusion_flag) {
    u8 r = GetRand(10);
    u8 chance = current_spell_chance+1;
    if (r >= chance) {
      temp_success_flag = 0;
    } 
  } 
  // 9778
  //temp_success_flag = 1;
#endif
  if (temp_success_flag)
    world_chaos += CHAOS_SPELLS.pSpellDataTable[current_spell]->chaosRating;
  
#ifdef __WINDOWS__
  if (ONLINE_MODE && IS_LOCAL(current_player)) {
    // send a message to everyone saying what we are up to
    send_casting_a();
  }
#endif 
#endif

}

// called at the start of the human player spell casting
void setup_human_player_cast(void) {
  set_current_spell_chance();
  // print player name and spell...
  remove_cursor();
  print_name_spell();
  
  wait_for_keypress();
  redraw_cursor();
  // clear message display
  clear_message();
  
  temp_illusion_flag = players[current_player].illusion_cast;

  // NB: Spell Success is calculated twice for some spells!
  // not sure if that is a bug or not
  // e.g. for creatures, this code is called once -> here
  // Magic Wood, it is called here and again just before the cast
  set_spell_success();
  
}


void print_success_status(void) {
  // based on 97a3
  clear_message();
  if (temp_success_flag == 0) {
    // print spell fails in purple
    
    print_text16("SPELL FAILS", MESSAGE_X, MESSAGE_Y, 12);
    set_text16_colour(12, RGB16(31,0,30)); // purple
    
  } else {
    
    print_text16("SPELL SUCCEEDS", MESSAGE_X, MESSAGE_Y, 12);
    set_text16_colour(12, RGB16(31,31,31)); // white
  }
  
  delay(30);
  
}

void print_name_spell(void) {
  // print player name, the spell and the range - code from 967a
  
  u8 x,y;
  get_yx(wizard_index,&y,&x);
  if (IS_CPU(current_player))
    move_screen_to(wizard_index);
  else
    move_cursor_to(x-1,y-1);
  set_border_col(current_player);
  clear_message();
  x = 0;
  // player name in yellow
  print_text16(players[current_player].name, x,MESSAGE_Y, 12);
  set_text16_colour(12, RGB16(31,30,0));
  // do a sound effect...
  PlaySoundFX(SND_SPELLSTEP);
  delay(8);
  x += strlen(players[current_player].name) + 1;
  
  // spell in green
  print_text16(CHAOS_SPELLS.pSpellDataTable[current_spell]->spellName, x,MESSAGE_Y, 13);
  set_text16_colour(13, RGB16(0,30,0));
  x += strlen(CHAOS_SPELLS.pSpellDataTable[current_spell]->spellName) + 1;
  // do a sound effect...
  PlaySoundFX(SND_SPELLSTEP);
  delay(8);
  // range in white
  u8 rng = CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange>>1;
  if (rng >= 10) {
    rng = 20;
  }
  
  char str[3];
  int2a(rng, str, 10);
  
  print_text16(str, x,MESSAGE_Y, 15);
  set_text16_colour(15, RGB16(31,30,31));
  // do a sound effect...
  PlaySoundFX(SND_SPELLSTEP);
  delay(8);
}

void splat_animation(void) {
  u8 x, y, i;
  get_yx(target_index, &y, &x);
  x--;
  y--;
  for (i = 0; i < 8; i++) {
    wait_vsync_int();
    wait_vsync_int();
//    wait_vsync_int();
    
  
    draw_splat_frame(x, y, i);
  }
}


void pop_animation(void) {
  
  u8 x, y, i;
  get_yx(target_index, &y, &x);
  PlaySoundFX(SND_SPELLSUCCESS);
  x--;
  y--;
  for (i = 0; i < 7; i++) {
    wait_vsync_int();
    wait_vsync_int();
//    wait_vsync_int();
    
    draw_pop_frame(x, y, i);
  }
}
