/// arena.c
#include "platform.h"

#ifdef __GBA__
#include "screenmode.h"
#include "sound.h"
#include "sprite.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#endif
#include <stdlib.h>
#include "sound_data.h"
#include "arena.h"
#include "chaos_sprites.h"
#include "gfx.h"
#include "chaos.h"
#include "casting.h"
#include "creature.h"
#include "spelldata.h"
#include "wizards.h"
#include "options.h"
#include "text16.h"
#include "string.h"
unsigned char arena[6][160];  // the 6 arena tables... 960 bytes
unsigned char wizard_index; // index into arena of current player
unsigned char start_index;  // index into arena for start square of current spell
unsigned char target_index;  // index into arena for target square of current spell
unsigned char round_count; // index into arena of current player
signed char world_chaos;

void get_new_scroll_pos(u32 x, u32 y, s32 * bgx, s32 *bgy);
void display_message(const char * text);
void debug_cursor_pos(void);



void set_current_index(u8 index);
//////////////////////////////////////////// 
/*
    Cursor movement stuff.
    When the curosr moves to a position, the screen should automagically
    offset itself to be in sync with that location
    
    At x = 0, the scroll = 0
    at x = 14, the scroll should be 31
    
*/

void move_cursor_left(void) {
  if (cursor_x > 0) {
    u16 tmp_curs = cursor_x;
    tmp_curs--;
#ifdef __GBA__    
    // work out where the scroll should be...
    u8 top_bloc = bg_level.x_scroll>>4; // div 16
    s32 bgx = bg_level.x_scroll;
    if (top_bloc != 0 && (cursor_x - top_bloc <= 1)) {
      // need to scroll...
      bgx -=16;
    }
#endif
    SET_CURSOR_POSITION(tmp_curs, cursor_y, bgx, bg_level.y_scroll);
    cursor_x = tmp_curs;
  }
  debug_cursor_pos();
}

void move_cursor_right(void) {
  if (cursor_x < 14) {
    
    u16 tmp_curs = cursor_x;
    tmp_curs++;
#ifdef __GBA__
    u8 top_bloc = bg_level.x_scroll>>4; // div 16
    s32 bgx = bg_level.x_scroll;
    if ( (top_bloc + 12) != 14 && (cursor_x - top_bloc >= 11) ) {
      // need to scroll
      bgx+=16;
    }
#endif
    
    SET_CURSOR_POSITION(tmp_curs, cursor_y, bgx, bg_level.y_scroll);
    cursor_x = tmp_curs;
  }
  debug_cursor_pos();
}

void move_cursor_up(void) {
  if (cursor_y > 0) {
    // work out where the scroll should be...
    u16 tmp_curs = cursor_y;
    tmp_curs--;
#ifdef __GBA__
    u8 top_bloc = bg_level.y_scroll>>4; // div 16
    s32 bgy = bg_level.y_scroll;
    if (top_bloc != 0 && (cursor_y - top_bloc <= 1)) {
      // need to scroll...
      bgy -=16;
    }
#endif
    SET_CURSOR_POSITION(cursor_x, tmp_curs, bg_level.x_scroll, bgy);
    cursor_y = tmp_curs;
  }
  debug_cursor_pos();
}

void move_cursor_down(void) {
  if (cursor_y < 9) {
    // check if the screen scroll is needed...
    u16 tmp_curs = cursor_y;
    tmp_curs++;
#ifdef __GBA__
    u8 top_bloc = bg_level.y_scroll>>4; // div 16
    s32 bgy = bg_level.y_scroll;
    if ( (top_bloc + 6) != 9 && (cursor_y - top_bloc >= 5) ) {
      // need to scroll
      bgy+=16;
    }
#endif
    SET_CURSOR_POSITION(cursor_x, tmp_curs, bg_level.x_scroll, bgy);
    cursor_y = tmp_curs;
  }
  debug_cursor_pos();
}

void debug_cursor_pos(void) {
#ifdef DEBUG
  char str[30];
  sprintf (str, "%2d, %2d", cursor_x, cursor_y);
  print_text16(str, 0, 0, 0);
#endif
}

void get_new_scroll_pos(u32 x, u32 y, s32 * bgx, s32 *bgy) {
  if (x < 7) {
    // x in the actual middle means scrolling off the edge...
    *bgx = 0;
  } else {
    if (x > 8) {
      // scrolls off the other edge
      *bgx = 32;
    }
    else  {
      *bgx = (x-7)*16;
    }
  }
  
  // there are 8 visible squares vertically
  if ( y  < 4) {
    // y in the actual middle means scrolling off the edge...
    *bgy = 0;
  } else {
    if (y > 6) {
      // scrolls off the other edge
      *bgy = 48;
    }
    else
      *bgy = (y-4)*16;
  }
  
}

  

// move CURSOR to an arbitrary location
// handles screen scroll for you
void move_cursor_to(u32 x, u32 y) {
  // move so that the screen is centered on the given location
  // check if the screen scroll is needed...

  // now see where we would like the screen to be...
  // this is going to be so that the x, y position is in the middle of the screen
  // there are 14 visible squares at once horizontally
  
  s32 bgx, bgy;
  get_new_scroll_pos(x, y, &bgx, &bgy);
  
  SET_CURSOR_POSITION(x, y, bgx, bgy);
  
  cursor_x = x;
  cursor_y = y;
  SET_CURSOR_POSITION(x, y, bgx, bgy);
}

// moves screen to higlight a square
// doesn't move the cursor though
void move_screen_to(u8 index) {
#ifdef __GBA__
  u8 y, x;
  get_yx(index, &y, &x);
  // x and y in "chaos coords" need to -1 to get standard zero-indexed values
  y--;
  x--;
  
  remove_cursor();
  s32 bgx, bgy;
  get_new_scroll_pos(x, y, &bgx, &bgy);
  
  s8 dx = 0;
  s8 dy = 0;
  
  if (bgx < bg_level.x_scroll)
    dx = -4;
  else if (bgx > bg_level.x_scroll)
    dx = 4;
  
  if (bgy < bg_level.y_scroll)
    dy = -4;
  else if (bgy > bg_level.y_scroll)
    dy = 4;
  
  
//  bg_level.x_scroll = bgx;
//  bg_level.y_scroll = bgy;
  
//  UpdateBackground(&bg_level);
  while (dx != 0 || dy != 0) {  
    if (bg_level.x_scroll != bgx)
      bg_level.x_scroll+=dx; 
    else
      dx = 0;
    
    if (bg_level.y_scroll != bgy)
      bg_level.y_scroll+=dy; 
    else
      dy = 0;
    
    wait_vsync_int();
    UpdateBackground(&bg_level);
  }
  
  s16 y_offset = -bg_level.y_scroll + 16;
  s16 x_offset = -bg_level.x_scroll + 16;
  
  wait_vsync_int();
  MoveSprite(&sprites[CHAOS_SPRITES_ROOT.pSpriteTable[0].pSpriteMap->nSpriteID],
      x_offset+ cursor_x*16,y_offset+ cursor_y*16);
  CopyOAM();
#endif
}

  
// x is the arena x coord
// y is the arena y coord
#ifdef __GBA__
void set_cursor_position(u32 x, u32 y, s32 bgx, s32 bgy) 
#elif defined(__WINDOWS__)
void set_cursor_position(u32 x, u32 y) 
#endif
{
  // the cursor position depends on the screen scroll as well as x/y position
  // smooth scroll to the given bg x,y
  
#ifdef __GBA__  
  s8 dx = 0;
  s8 dy = 0;
  s32 y_offset;
  s32 x_offset;
  
  if (bgx < bg_level.x_scroll)
    dx = -4;
  else if (bgx > bg_level.x_scroll)
    dx = 4;
  
  if (bgy < bg_level.y_scroll)
    dy = -4;
  else if (bgy > bg_level.y_scroll)
    dy = 4;
  
  if (dx == 0 && dy == 0) {
    // halve the difference between cursor_x and x / cursor_y and y 
    // and scroll a small amount
    
    y_offset = -bg_level.y_scroll + 16;
    x_offset = -bg_level.x_scroll + 16;
    u8 cursorid = 0;
    s16 x_diff = x - cursor_x;
    s16 y_diff = y - cursor_y;
    
    wait_vsync_int();
    MoveSprite(&sprites[CHAOS_SPRITES_ROOT.pSpriteTable[cursorid].pSpriteMap->nSpriteID],
        x_offset+ cursor_x*16+ x_diff*8,y_offset+cursor_y*16+y_diff*8);
    CopyOAM();
    
    wait_vsync_int();
    MoveSprite(&sprites[CHAOS_SPRITES_ROOT.pSpriteTable[cursorid].pSpriteMap->nSpriteID],
        x_offset+ x*16,y_offset+ y*16);
    CopyOAM();
    
  }
  else {
    while (dx != 0 || dy != 0) {  
      if (bg_level.x_scroll != bgx)
        bg_level.x_scroll+=dx; 
      else
        dx = 0;
      
      if (bg_level.y_scroll != bgy)
        bg_level.y_scroll+=dy; 
      else
        dy = 0;
      
      wait_vsync_int();
      UpdateBackground(&bg_level);
    } 
  }
#elif defined(__WINDOWS__)
  // on windows, just draw the cursor and forget scrolling
  
#endif  
  // display the message for this square
  target_index = x+y*16;
  display_cursor_contents(target_index);

  
}


// clear arena is in the creatures file...

void display_arena(void) {
  // redraw the arena...
  u16 i;
  clear_bg();
  clear_arena();
  clear_palettes();
  load_all_palettes();
#ifdef __GBA__
  u8 x,y;
  bg_level.x_scroll = 0;
  bg_level.y_scroll = 0;
  UpdateBackground(&bg_level);
  for (x = 0; x < 32; x++) {
    for (y = 0; y < 32; y++) {
      bg_level.mapData[x+y*32] = 0;
    }
  }
  
  i = 1;
  for (y=2; y < 22; y++) {
    for (x=2; x < 32; x++) {
      bg_level.mapData[x+y*32] = i;
      i++;
    }
  }
#endif
  
  draw_game_border();
  // draw all the creatures in the arena array
  draw_all_creatures();
  
  for (i = 0; i < playercount; i++) {
    set_player_col(i, players[i].colour);
  }
  
}


// this is a dumb display message routine
// need one that can draw parts of messages in different colours...
void display_message(const char * text) {
  
  print_text16("                               ", MESSAGE_X,MESSAGE_Y, 12);
  print_text16(text, MESSAGE_X,MESSAGE_Y, 12);
  set_text16_colour(12, RGB16(0,30,30));
}

void clear_message(void) {
  print_text16("                               ", MESSAGE_X,MESSAGE_Y, 12);
  
}

const u8 anim_speed_data[] = {
  /*KING COBRA   */ 0x1e,
  /*DIRE WOLF    */ 0x0c,
  /*GOBLIN       */ 0x0c,
  /*CROCODILE    */ 0x22,
  /*FAUN         */ 0x14,
  /*LION         */ 0x26,
  /*ELF          */ 0x1a,
  /*ORC          */ 0x15,
  /*BEAR         */ 0x17,
  /*GORILLA      */ 0x12,
  /*OGRE         */ 0x17,
  /*HYDRA        */ 0x24,
  /*GIANT RAT    */ 0x0d,
  /*GIANT        */ 0x17,
  /*HORSE        */ 0x15,
  /*UNICORN      */ 0x10,
  /*CENTAUR      */ 0x17,
  /*PEGASUS      */ 0x10,
  /*GRYPHON      */ 0x0a,
  /*MANTICORE    */ 0x0d,
  /*TROLL        */ 0x16,
  /*BAT          */ 0x08,
  /*GREEN DRAGON */ 0x20,
  /*RED DRAGON   */ 0x22,
  /*GOLDEN DRAGON*/ 0x1b,
  /*HARPY        */ 0x0d,
  /*EAGLE        */ 0x0e,
  /*VAMPIRE      */ 0x28,
  /*GHOST        */ 0x0f,
  /*SPECTRE      */ 0x0f,
  /*WRAITH       */ 0x0a,
  /*SKELETON     */ 0x11,
  /*ZOMBIE       */ 0x19,
  /*BLOB         */ 0x28,
  /*FIRE         */ 0x0c,
  /*MAGIC WOOD   */ 0xfa,
  /*SHADOW WOOD  */ 0x1e,
  /*MAGIC CASTLE */ 0x32,
  /*DARK CITADEL */ 0x32,
  /*WALL         */ 0x1e,
  /*JULIANLL     */ 0x1e,
  /*GANDALFRT    */ 0x1e,
  /*GREATFOGEY   */ 0x1e,
  /*DYERARTI     */ 0x1e,
  /*GOWIN        */ 0x1e,
  /*MERLIN       */ 0x1e,
  /*ILIAN RANE   */ 0x1e,
  /*ASIMONO ZARK */ 0x1e,
};

// counts down the arena[1] values and updates the arena 2 val if needed
void countdown_anim(void) {
  int i;  
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] >= 2) {
      if ( (--arena[1][i]) == 0) {
        arena[2][i]++;
        if (arena[2][i] == 4) {
          arena[2][i] = 0;
        }
        if (arena[2][i] == 5) {
          arena[2][i]--;
        }

      	// each spell has a "speed" count
	arena[1][i] = anim_speed_data[arena[0][i]-2];
      }
    }
  }
}


void animate_arena(void) {
  draw_all_creatures();

}

void get_cursor_pos(unsigned char * x, unsigned char * y) {
  *x = cursor_x;
  *y = cursor_y;
}
 



// calculate the distance between 2 squares in the arena in "chaos range" terms (roughly 2*distance)
// the squares should be indices into the arena (i.e. 0,0 is 0, 1,1 is 1+1*16, etc)
void get_distance(u16 square1, u16 square2, u16 * distance) {
  // based on code at 9786 
  u8 x1, y1, x2, y2;
  u16 diff;
  get_yx(square1, &y1, &x1);
  get_yx(square2, &y2, &x2);
  
  /* calculate 2*(larger of xposdiff and yposdiff) + (the smaller of the 2)*/
  get_chaosdistance(x1,y1,x2,y2,&diff);
  *distance = diff;
  
  // further check in here, compare to cast range
//  return spell_list[current_spell].cast_range - distance;
  
  
}

// calculate 2*(larger of xposdiff and yposdiff) + (the smaller of the 2)
// based on code at 0xbeef
void get_chaosdistance(u8 x1,u8 y1,u8 x2,u8 y2,u16 *diff) {
  u16 xdiff, ydiff;
  u16 distance;
   
  xdiff = abs(x2-x1);
  ydiff = abs(y2-y1);
  
  if (xdiff < ydiff) {
    distance = ydiff<<1;
    distance += xdiff;
  } else {
    distance = xdiff<<1;
    distance += ydiff;
  }

  *diff = distance;
  
}



void get_yx(u16 arena_index, u8 * H, u8 *L) {
  
  u8 var1, var2;
  
  get_yx2(arena_index, &var1, &var2);
  var1>>=1;
  var1++;
  var2>>=1;
  var2++;
  *H = var1;
  *L = var2;
  
}


void get_yx2(u16 arena_index, u8 * H, u8 *L) {
    int val1 = arena_index & 0xF0;      
    int val2 = arena_index;// (& 0xFF);
    val1 = val1>>3;
    
    val2 = val2<<1;
    val2 = val2 & 0x1e;
    
    val1++;
    val2++;
    
    *H = val1;
    *L = val2;
}                              

void init_arena_tables(void) {
  // initialises the arena tables - code based on that at c0dd
  int i;
  disable_interrupts();
  for (i = 0; i < 0xA0; i++) {
    
    if ((i + 1) & 0xF) {
      if (arena[0][i] == 0) {
        arena[0][i] = 0; // set to one in the actual game...
        arena[2][i] = 0;        
      }      
    }
    arena[1][i] = 1;
  }
  enable_interrupts();  
}

// code from 97d1 - sets the index of the current player for spell casting
void set_current_player_index(void) {
  u8 i;
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] - WIZARD_INDEX == current_player) {
        set_current_index(i);
        return;         
    }
    if (arena[4][i] - WIZARD_INDEX == current_player) {
      set_current_index(i);
      return;
    }
  }
}

void set_current_index(u8 index) {
  wizard_index = index;
  start_index = index;
  target_index = index;
}

void display_cursor_contents(u8 index) {
  // dispay the creature name, etc in the right colours...
  // taken from bd18 onwards
  // clear the message box
  u8 creature, x;
  char str [30];

  clear_message();
  creature = arena[0][index];
  
  if (creature == 0) 
    return;
  
  set_text16_colour(12, RGB16(0,28,31)); // light blue
  x = MESSAGE_X;
  if (creature >= WIZARD_INDEX) {
    // wizard... write wiz name
    print_text16(players[creature-WIZARD_INDEX].name, x,MESSAGE_Y, 12);
    x += strlen(players[creature-WIZARD_INDEX].name);
  } else {
    // creature... print name and owner
    print_text16(CHAOS_SPELLS.pSpellDataTable[creature]->spellName, x,MESSAGE_Y, 12);
    x += strlen(CHAOS_SPELLS.pSpellDataTable[creature]->spellName);
  }
  
  // check underneath
  if (arena[4][index] != 0) {
    // underneath colour 47 (white?) 
    set_text16_colour(14, RGB16(30,30,30)); // white  
  } else if (arena[5][index] != 0) {

    set_text16_colour(14, RGB16(27,4,28)); // purple?
  } else {
    set_text16_colour(14, RGB16(0,0,0)); // purple?
  }
  
  // the character # is actually a \ in my character array as I don't have many entries.
  str[0] = '\\';
  str[1] = 0;
  print_text16(str, x,MESSAGE_Y, 14);
  x++;
  
  if (creature < WIZARD_INDEX) {
    // print the creature owner or status...
    if (arena[2][index] == 4) {
      // dead
      set_text16_colour(13, RGB16(0,30,0)); // green
      strcpy(str, "(DEAD)");
      
      
      print_text16(str, x,MESSAGE_Y, 13);
      x = strlen(str);
    } else {
      u8 own = arena[3][index] & 0x7;
      set_text16_colour(13, RGB16(30,30,0)); // yellow
      
      str[0] = '(';
      str[1] = 0;
      strcat(str, players[own].name);
      strcat(str, ")");
      
      print_text16(str, x,MESSAGE_Y, 13);
      x = strlen(str);
    }
    
  }
  
  
}

// called at the end of the moves round
// resets the movement flags so that the creatures can move next time
void unset_moved_flags(void) {
  unsigned char i;
  for (i = 0; i < 0x9f; i++) {
    arena[3][i] &= 0x7F;  // unset bit 7
  }
}



u8 apply_position_modifier(u8 square, u8 i) {
  // use the look up table to convert the square to a new one 
  unsigned char y, x, tmp;
  get_yx(square,&y,&x);
  
  y = y + surround_table[i][0];
  if (y == 0 || y == 0xB) {
    return 0;
  }
  
  x = x + surround_table[i][1];
  if (x == 0 || x == 0x10) {
    return 0;
  }
  
  x--;
  y--;
  
  tmp = ((y<<4)+x)+1;
  
  // Add the "3 trees in top corner" bug...
  if (Options[OPT_OLD_BUGS] && tmp == 2 && current_spell == SPELL_MAGIC_WOOD)
    tmp = 0;
    
  return tmp;
  
}

// based on ba77  
void remove_los_targets(unsigned char index) {
  // remove fire, ghost wraiths, dead creatures, target squ, casting wizard
  // u16 i;
  u8 x, y;

  get_yx(index, &y, &x);
  clear_square(x-1,y-1);
  get_yx(start_index, &y, &x);
  clear_square(x-1,y-1);
  
#if 0
  for (i = 0; i < 0x9F; i++) {
    if (/*arena[0][i] == SPELL_MAGIC_FIRE || arena[0][i] == SPELL_GHOST
    || arena[0][i] == SPELL_WRAITH || arena[2][i] == 4  || */ /*dead*/ 
     i == index || i == start_index) {
      // get the x, y coords
      get_yx(i, &y, &x);
      clear_square(x-1, y-1);
    }
  }
#endif
}


void get_yx_upper(unsigned char arena_index, unsigned char *H, unsigned char *L) {
  u8 x, y;
  get_yx(arena_index, &y, &x);
  *L = x<<4;
  *H = y<<4;
}

// taken from b4fa  
// destroys this wizard's creations
void destroy_all_creatures(unsigned char playerid) {
  
  u8 i, y, x;
  u8 frame;
  u8 sample_played = 0;
  for (frame = 0; frame < 7; frame++) {
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    for (i = 0; i < 0x9f; i++) {
      if (arena[0][i] == 0)
        continue;   
      if(arena[0][i] >= WIZARD_INDEX)
        continue;
      if (arena[2][i] == 4 /* && !IS_ASLEEP(arena[3][i] & 0x8) */ ) {
        // is dead..
        continue;
      }
      // check the owner
      if ( (arena[3][i] & 0x7) != playerid) {
        // chek if it is a blob
        if (arena[0][i] != SPELL_GOOEY_BLOB)
          continue;
        
        // blob... is anything under it?
        if (arena[4][i] == 0)
          continue;
        // is the trapped creature the effected player?
        if ((arena[5][i] & 7) != playerid)
          continue;
      }
      // draw the pop frame at this creatures location...
      get_yx(i, &y, &x);
      draw_splat_frame(x-1, y-1, frame);
      
      // do a sound effect, once only
      if (!sample_played) {
        PlaySoundFX(SND_SPELLSUCCESS);
        sample_played = 1;
      }
      
      // check if we have finished the anim...
      if (frame == 6) {
        // get rid of the creature
        if ( (arena[3][i] & 0x7) != playerid) {
          arena[4][i] = 0;
          arena[5][i] = 0;
        } else {
          // same owner as effected wiz
          if (arena[0][i] == SPELL_GOOEY_BLOB && arena[4][i] != 0) {
            // b5a9
            arena[0][i] = arena[4][i];
            arena[4][i] = 0;
            arena[3][i] = arena[5][i];
            arena[5][i] = 0;
            continue;
          }
          // b5be
          u8 arena4 = arena[4][i];
          arena[4][i] = 0;
          if (arena4 != 0) {
            arena[0][i] = arena4;
            continue;
          }
          // b5d1
          //u8 arena5 = arena[5][i];
          arena[0][i] = arena[5][i];
          if (arena[5][i] != 0) {
            arena[2][i] = 4;
            arena[5][i] = 0;
          }
        }
      }
    }
  }
  
}

// randomly destroy castles at the end of each casting round
// code originally followed on from gooey blob, but it's here
// to make it clearer what's going on 
// a120
void destroy_castles(void) {
  u8 i;
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] >= SPELL_MAGIC_CASTLE && arena[0][i] <= SPELL_DARK_CITADEL) {
      // is a castle...
      if (GetRand(10) < 8)
        continue;
      
      // got here? then the castle is to be destroyed
      move_screen_to(i);
      arena[0][i] = arena[4][i];
      arena[4][i] = 0;
      target_index = i;
      pop_animation();
      delay(4);
    }
    
  } 
  delay(20);
}

// give a player a spell from the magic wood
void random_new_spell(void) {
  u8 i,j,k;
  char str[30];
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] == SPELL_MAGIC_WOOD && arena[4][i] != 0) {
      // is a wood with someone inside
      if (GetRand(10) <= 6)
        continue;
      
      // got here? then the wood has given us a spell!
      u8 lucky_player = arena[4][i] - WIZARD_INDEX;
      strcpy(str,"NEW SPELL FOR ");
      strcat(str,players[lucky_player].name);
      print_text16(str, MESSAGE_X,MESSAGE_Y, 13);
      set_text16_colour(13, RGB16(30,30,0)); // yellow
      delay(100);
      
      // generate new spell...
      clear_message();
      u8 randspell = GetRand(127);
      while (randspell < SPELL_KING_COBRA || randspell > SPELL_TURMOIL) {
        randspell = GetRand(127);
      }
      
      // find an empty slot for the spell
      k = 3;
      for (j = 0; j < 0x13; j++) {
        if (players[lucky_player].spells[k] == 0) {
          players[lucky_player].spells[k] = randspell;
          players[lucky_player].spell_count++;
          break;
        }
        k+=2;
      }

      arena[0][i] = arena[4][i];
      arena[4][i] = 0;
      arena[3][i] = lucky_player;   // bug here, what if the wizard was undead? ;)
      delay(20);
    }
    
  }
}

// called at the end of the game... resets the tables to the initial state
void reset_arena_tables(void) {
  u8 i, j;
  for (j = 0; j < 6; j++) {
    for (i = 0; i < 0x9f; i++) {
      arena[j][i] = 0;
    }
  }
}

void highlight_players_stuff(unsigned char playerid) {
  // highlight the given players stuff
  int i;
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] == 0) 
      continue;
    
    if (arena[0][i] >= WIZARD_INDEX) {
      // wizard
      if ( (arena[0][i] - WIZARD_INDEX) == playerid) {
        // highlight the player
        draw_silhouette_gfx(i, 
          WizardGFX[players[arena[0][i] - WIZARD_INDEX].image].pWizardData->pGFX, 
          WizardGFX[players[arena[0][i] - WIZARD_INDEX].image].pWizardData->pMap,
          players[playerid].colour, 11, 1); 
      }
      
    }
    else if (arena[2][i] != 4) {
      // creature, not dead
      if (GET_OWNER(arena[3][i]) == playerid) {
	    int col = getMainColour(arena[0][i],arena[2][i]);
	    
        draw_silhouette_gfx(i, 
          CHAOS_SPELLS.pSpellDataTable[arena[0][i]]->pGFX, 
          CHAOS_SPELLS.pSpellDataTable[arena[0][i]]->pGFXMap,
          //players[playerid].colour,
      	  col, -1,
      	  1);
      }
      
    }
  }
  
}

