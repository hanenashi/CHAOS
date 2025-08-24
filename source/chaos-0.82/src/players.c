// players.c
// create the players
#include "platform.h"
#ifdef __GBA__
#include "gba.h"
#include "sound.h"
#include "screenmode.h"
#include "keypad.h"
#define PLAYER_WIZ_Y 1
#elif defined(__WINDOWS__)
#include "winkeys.h"
#include "winsound.h"
#define PLAYER_WIZ_Y 2
#endif

#include "sound_data.h"
#include "players.h"
#include "gfx.h"
#include "chaos.h"
#include "gamemenu.h"
#include "wizards.h"
#include "creature.h"
#include "arena.h"
#include "spelldata.h"
#include "chaos_sprites.h"
#include "computer.h"
#include "editname.h"

#include "text16.h"
#include "string.h"
#include "input.h"
#include "splash.h"
#include "creature.h"
/////////////////////////////////////////////////////////


int hilite_wizard_item;



/////////////////////////////////////////////////////////

void select_player_item(u8 item);
void deselect_player_item(u8 item);
void create_default_wizs(void);


void update_players(void);
void init_players(void);
unsigned char array_contains(const unsigned char * array, unsigned char val);

/////////////////////////////////////////////////////////

const char* const namesData[22] = {
  "Rich Q  ", "Fat Matt", "Colin   ", "Rich B  ", "Leek    ", "Dan     ",
  "Marce   ", "Smithy  ", "Kiff    ", "Ben     ", "Nick    ", "Tim     ",
  "Gandalf ", "H.Potter", "Houdini ", "Merlin  ", "Grotbags", "Venger  ",
  "Presto  ", "Saruman ", "Sooty   ", "Paul D  ", 
};
/////////////////////////////////////////////////////////

void show_create_players(void) {
  current_screen = SCR_CREATE_PLAYERS;
  clear_bg();
  clear_palettes();
  load_bg_palette(9,9);
  playercount = 2;
  hilite_item = 0;
  anim_col = 30;
  hilite_wizard_item = 0;
  
  print_text16("How many players?", 4,1, 10);
  set_text16_colour(10, RGB16(31,30,30));
  
  // create the default start wizards
  create_default_wizs();
  
#ifdef __GBA__
  u8 x,y;
  u16 i;
  i = 1;
  for (y=2; y < 22; y++) {
    for (x=2; x < 32; x++) {
      bg_level.mapData[x+y*32] = i;
      i++;
    }
  }
  
#endif
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 

  update_players();
  
  select_player_item(0);
  
}

// used to just redraw, not reinit players
void redraw_create_players(void) {
  current_screen = SCR_CREATE_PLAYERS;
  clear_bg();
  clear_palettes();
  load_bg_palette(9,9);
  hilite_item = 0;
  anim_col = 30;
  hilite_wizard_item = 0;
  
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
  
  print_text16("How many players?", 2,1, 10);
  set_text16_colour(10, RGB16(0,30,30));
  update_players();
  
  hilite_item = current_player+1;
  select_player_item(hilite_item);
  current_player = 0;
}

  

void create_players_up(void) {
  wait_vsync_int();
  deselect_player_item(hilite_item);
  anim_col = 30;
  if (hilite_item > 0)
    hilite_item--;
    
  select_player_item(hilite_item);
}


void create_players_down(void) {
  wait_vsync_int();
  deselect_player_item(hilite_item);
  anim_col = 30;
  if (hilite_item < playercount)
    hilite_item++;
  select_player_item(hilite_item);
}

void select_player_item(u8 item) {
  anim_col = -31;
  anim_col_grad = -8;
  
  if (item == 0)
    set_text16_colour(10, RGB16(31,30,30));
  else
    set_text16_colour(item-1, RGB16(31,30,30));
}

void deselect_player_item(u8 item) {
  if (item == 0)
    set_text16_colour(10, RGB16(0,30,30));
  else
    set_text16_colour(item-1, RGB16(0,30,30));
}



void create_players_left(void) {
  if (hilite_item == 0) {
      
    if (playercount > 2) {
      playercount--;
      PlaySoundFX(SND_MENU);
    }
  } else {
    if (IS_CPU(hilite_item-1)) 
    {
      // get the level - stored in upper 4 bits
      int level = players[hilite_item-1].plyr_type>>4;
      players[hilite_item-1].plyr_type &= 0xf;
      level--;
      players[hilite_item-1].plyr_type |= (level<<4);
      if (!level)
        players[hilite_item-1].plyr_type = PLYR_HUMAN;
    }
    PlaySoundFX(SND_MENU);
  }
  update_players();
  
}

void create_players_right(void) {
  if (hilite_item == 0) {
    if (playercount < 8) {
      playercount++;
      PlaySoundFX(SND_MENU);
    }
  } else {
    int level = players[hilite_item-1].plyr_type>>4;
    if (level < 8) {
      level++;
      players[hilite_item-1].plyr_type = PLYR_CPU | (level<<4);
    }
    PlaySoundFX(SND_MENU);
  }
  update_players();
}

void create_players_l(void) {
  // change colour
  if (hilite_item ==0)
    return;
  if (players[hilite_item-1].colour < 8) {
    players[hilite_item-1].colour++;
  }
  else {
    players[hilite_item-1].colour = 0;
  }
  update_players();
}

void create_players_r(void) {
  // change image
  if (hilite_item ==0)
    return;
  if (players[hilite_item-1].image < 7) {
    players[hilite_item-1].image++;
  }
  else {
    players[hilite_item-1].image = 0;
  }
  update_players();
}


void create_players_accept(void) {
  
  if (hilite_item == 0) {
    // A prssed on the "How many players?" bit
    // reset the players!  
    if (KeyPressed(KEY_L) && KeyPressed(KEY_R)) {
      create_default_wizs();
      
      clear_bg();
      print_text16("How many players?", 4,1, 10);
      set_text16_colour(10, RGB16(0,30,30));
#ifdef __WINDOWS__ 
      draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
#endif
      update_players();
    }
  
  } else {
    // go to the edit name screen
    current_player = hilite_item -1;
    fade_down();
    show_editname();
    fade_up();
  }
  
    
}

void create_players_start(void) {
  PlaySoundFX(SND_CHOSEN);
  init_players();
  current_player = 0;
  round_count = 0;
  // check if we need to show game menu...
  if (IS_CPU(0)) 
    current_player = get_next_human(0);
  else
    current_player = 0;
  
  if (current_player == 9) {
    // there is no human player!
    
    continue_game();
  } else {
    fade_down();
    show_game_menu();
    fade_up();
  }
  
}


unsigned char get_next_human(unsigned char id) {
  u8 i;
  for (i = id+1; i < playercount; i++) {
    if (!IS_CPU(i) && !IS_WIZARD_DEAD(players[i].modifier_flag))
      return i;
  }
  return 9;
}


void update_players(void) {
  // use the global variables to update the amount of players shown...
  u8 i;
  char str[30];
  wait_vsync_int();
  clear_arena(); 
  // clear the arena and redraaw it...
  for (i = 0; i < 8; i++) {
    // draw a wizard at this screen position
    if (i < playercount) {
      set_player_col(i, chaos_cols[players[i].colour]);
      draw_wizard8(0,(i<<1)+PLAYER_WIZ_Y,players[i].image,0,i);
      
      if (hilite_item -1 == i)
        set_text16_colour(i, RGB16(31,30,30));
      else
        set_text16_colour(i, RGB16(0,30,30));
      print_text16(players[i].name, 6,3+i*2, i);
          
      if (IS_CPU(i)) {
        strcpy(str, "Computer ");
        char istr[3];
        int level = players[i].plyr_type>>4;
        int2a(level, istr, 10);
        strcat(str, istr);
        print_text16(str, 16,3+i*2, i);
      }
      else {
        print_text16("Human     ", 16,3+i*2, i);
      }
      
      
    } else {
      set_text16_colour(i, RGB16(0,0,0));
#if defined(__WINDOWS__)
      clear_square8(0,(i<<1)+PLAYER_WIZ_Y);
#endif
    }
    
  }
  
  set_text16_colour(10, RGB16(0,30,30));
  int2a(playercount, str, 10);
  print_text16(str, 22,1, 10);
  
}
  
// need to select/ unselect items
// hilite_item contains current selected item
// hilite_item:
// PLayercount (0)
// CPU level (1)
// wizard 1 (2)
// wizard 2 (3)
// ...
// wizard 8 (9)

// currently selected item should flash 
void animate_player_screen(void){
  
  
  if (hilite_item == 0)
    anim_selection(10, 0, 31, 31);
  else 
    anim_selection(hilite_item-1, 0, 31, 31);
    
  
}

unsigned char array_contains(const unsigned char * array, unsigned char val) {
  // check the 8 long array to see if it contains val
  // return 1 if it does contain the val, return 0 if not
  u8 i;
  for (i = 0; i < 8; i++) {
    if (array[i] == val)
      return 1;
  }
  return 0; 
}


void create_default_wizs(void) {
  // creates 8 default wizards with sensible colours
  // names, type and colour are random and not repeated
  u8 i;
  u8 names[8];
  u8 colours[8];
  u8 images[8];
  u8 tmp;
  for (i = 0; i < 8; i++) {
    names[i] = 0;
    colours[i] = 0;
    images[i] = 0;
  }
  
  
  for (i = 0; i < 8; i++) {
    tmp = 1+GetRand(22);
    while (array_contains(names, tmp) ) {
      tmp = 1+GetRand(22);
    }
    names[i] = tmp;
    strcpy(players[i].name , namesData[names[i]-1]);
    trim_whitespace(players[i].name);

    tmp = 1+GetRand(8);
    while (array_contains(images, tmp) ) {
      tmp = 1+GetRand(8);
    }
    images[i] = tmp;
    players[i].image = images[i]-1;
    
    tmp = 1+GetRand(9);
    while (array_contains(colours, tmp) ) {
      tmp = 1+GetRand(9);
    }
    colours[i] = tmp;
    players[i].colour = colours[i]-1; //chaos_cols[colours[i]-1];
    
    
    players[i].modifier_flag = 0;
    players[i].plyr_type = PLYR_CPU | ( 4 << 4);

  }
  
  players[0].plyr_type = PLYR_HUMAN;
}


const u8 position_table[56] = {

  0x41,0x4d,0x00,0x00,0x00,0x00,0x00,0x00,
  0x17,0x81,0x8d,0x00,0x00,0x00,0x00,0x00,
  0x11,0x1d,0x81,0x8d,0x00,0x00,0x00,0x00,
  0x07,0x30,0x3e,0x93,0x9b,0x00,0x00,0x00,
  0x07,0x10,0x1e,0x80,0x97,0x8e,0x00,0x00,
  0x07,0x11,0x1d,0x60,0x6e,0x94,0x9a,0x00,
  0x00,0x07,0x0e,0x40,0x4e,0x90,0x97,0x9e
};

void init_players(void) {
  u8 loop;
  u8 k = 0;
  u8 cpulvl = 0;
  u8 offset = (playercount-2)*8;
  u8 square;
  // in speccy chaos, the spell data for players starts at 7f47. 
  // For player 1, it goes 7f47 - 7f6e (inclusives) = 0x28 = 40 (20 spells)
  for (loop = 0; loop < playercount; loop++) {
    
    cpulvl = players[loop].plyr_type>>4; //cpu level, or 0 for humans
    
    // generate the player stats
    players[loop].combat = (GetRand(10)>>1) + 1 + (cpulvl>>1);
    players[loop].ranged_combat = 0;
    players[loop].range = 0;
    players[loop].defence = (cpulvl>>1) + (GetRand(10)>>1) + 1;
    players[loop].movement_allowance = 1;
    players[loop].manoeuvre_rating = (cpulvl>>2) + (GetRand(10)>>1) + 3;
    players[loop].magic_resistance = (GetRand(10)>>1) + 6;
    players[loop].spell_count = cpulvl + (GetRand(10)>>1) + 0xB;   
    if (players[loop].spell_count > 20)
      players[loop].spell_count = 20;
    

    
    players[loop].ability = 0;
    if ( (5 - (cpulvl>>1)) <= GetRand(10))
      players[loop].ability = GetRand(10)>>2;     
    
    // change from index representation to the actual 15 bit colour value
    players[loop].colour = chaos_cols[players[loop].colour];
     
    // generate the spells...
    u8 spellindex = 0;
    u8 spellid = 0x01;
    if (IS_CPU(loop)) {
      players[loop].spells[spellindex] = GetRand(10)+0x0c; // disblv, random priority
    } else {
      players[loop].spells[spellindex] = 0; // set to 0 for "no spell selected"
      players[loop].selected_spell = 0; // set to 0 for "no spell selected"
    }
    
    spellindex++;
    players[loop].spells[spellindex] = spellid;
    spellindex++;

    for (k = 1; k < players[loop].spell_count; k++) {
      spellid = GetRand(255) & 0x3F;
      while (spellid < SPELL_KING_COBRA || spellid > SPELL_RAISE_DEAD) {
        spellid = GetRand(255) & 0x3F;
      }
       
      if (spellindex > 39) {
        print_text16("Spell overflow", 1,12, 10);
        set_text16_colour(10, RGB16(0,30,30));
        wait_for_letgo();
        wait_for_keypress();
      }
      players[loop].spells[spellindex] =
       	CHAOS_SPELLS.pSpellDataTable[spellid]->castPriority; // priority
      spellindex++;
      if (spellindex > 39) {
        print_text16("Spell overflow", 1,12, 10);
        set_text16_colour(10, RGB16(0,30,30));
        wait_for_letgo();
        wait_for_keypress();
      }
      players[loop].spells[spellindex] = spellid;
      spellindex++;
    }
#if 0
    spellindex++; 
    spellid = SPELL_MAGIC_WINGS;
    players[loop].spells[spellindex] = spellid;
    players[loop].spell_count++;
    spellindex++; 

    spellindex++; 
    spellid = SPELL_WRAITH;
    players[loop].spells[spellindex] = spellid;
    players[loop].spell_count++;
    spellindex++;
#endif
    players[loop].spells[40] = 0;
    players[loop].spells[41] = 0;
    if (IS_CPU(loop)) {
      // order the spells by priority  
      order_table(20, players[loop].spells);
    }
    
    // set the player positions..
    square = position_table[offset+loop];
    arena[0][square] = loop+WIZARD_INDEX;
    arena[3][square] = loop;
    
  }
  
  dead_wizards = 0;
  
}
