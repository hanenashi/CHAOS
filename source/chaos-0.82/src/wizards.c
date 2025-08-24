#include "platform.h"

#ifdef __GBA__
#include "screenmode.h"
#include "gba.h"
#include "sound.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#include "network.h"
#include "options.h"
#endif

#include "sound_data.h"
#include "wizards.h"
#include "arena.h"
#include "gfx.h"
#include "chaos.h"
#include "spelldata.h"

// player data
player_data players[8];
//unsigned char current_player;

// structures to make the graphics easier to handle
const static struct WIZARD_DATA _wiz1 = 
{
  WIZ1_DATA,
  WIZ1_MAP_DATA,
};

const static struct WIZARD_DATA _wiz2 = 
{
  WIZ2_DATA,
  WIZ2_MAP_DATA,
};

const static struct WIZARD_DATA _wiz3 = 
{
  WIZ3_DATA,
  WIZ3_MAP_DATA,
};

const static struct WIZARD_DATA _wiz4 = 
{
  WIZ4_DATA,
  WIZ4_MAP_DATA,
};

const static struct WIZARD_DATA _wiz5 = 
{
  WIZ5_DATA,
  WIZ5_MAP_DATA,
};

const static struct WIZARD_DATA _wiz6 = 
{
  WIZ6_DATA,
  WIZ6_MAP_DATA,
};

const static struct WIZARD_DATA _wiz7 = 
{
  WIZ7_DATA,
  WIZ7_MAP_DATA,
};

const static struct WIZARD_DATA _wiz8 = 
{
  WIZ8_DATA,
  WIZ8_MAP_DATA,
};


// SPELL_MAGIC_SHIELD
// SPELL_MAGIC_ARMOUR
// SPELL_MAGIC_SWORD 
// SPELL_MAGIC_KNIFE 
// SPELL_MAGIC_BOW
// SPELL_MAGIC_WINGS 

const static struct WIZARD_DATA _shield = 
{
  SHIELD_DATA,
  SHIELD_MAP_DATA,
};

const static struct WIZARD_DATA _armour = 
{
  ARMOUR_DATA,
  ARMOUR_MAP_DATA,
};

const static struct WIZARD_DATA _sword = 
{
  SWORD_DATA,
  SWORD_MAP_DATA,
};

const static struct WIZARD_DATA _knife = 
{
  KNIFE_DATA,
  KNIFE_MAP_DATA,
};

const static struct WIZARD_DATA _bow = 
{
  BOW_DATA,
  BOW_MAP_DATA,
};

const static struct WIZARD_DATA _wings = 
{
  WINGS_DATA,
  WINGS_MAP_DATA,
};


const struct WIZARD_DATATABLE WizardGFX [] = {
   {&_wiz1},
   {&_wiz2},
   {&_wiz3},
   {&_wiz4},
   {&_wiz5},
   {&_wiz6},
   {&_wiz7},
   {&_wiz8},
  
   {&_shield},
   {&_armour},
   {&_sword},
   {&_knife},
   {&_bow},
   {&_wings},
};

// update the player's creature count table - 7d04
void update_creaturecount(void) {
  
  if (round_count < 6) {
    players[current_player].timid = 0;
    return;
  }
  
  u8 i, owner;  
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] >= SPELL_KING_COBRA
       	&& arena[0][i] < SPELL_GOOEY_BLOB) 
    {
      // is a creature
      if (arena[4][i] != WIZARD_INDEX+current_player) {
        // is not a ridden creature
        owner = arena[3][i]&0x7;
        if (owner == current_player) {
          players[current_player].timid = 0;
          return;
        }
      }
    }
    
  }
  
  // if we are past round 6 and have no creatures
  // then "be brave"...
  players[current_player].timid = 0x14;
  
}

// kill the wizard at the target_index
void kill_wizard(void) {
  // code from b3c9
  remove_cursor();
  delay(4);
  dead_wizards++;
  // do a sound fx..
  u8 deadid = arena[0][target_index]-WIZARD_INDEX;
  move_screen_to(target_index);
  // convert the x, y target location to y, x tile locations
  u8 x, y, i, j;
  u8 pal = 9;
  // load the wizard pal into 10 too...
  load_bg_palette(10, 9);
  s8 y2_1, y2_2, x2_1, x2_2;
  get_yx(target_index, &y, &x);
  clear_square(x-1, y-1);
  
#ifdef __GBA__
  PlayLoopedSoundFX(SND_SCREAM, 8);
  // FIXME: Looped sounds are not too good...
#else
  PlaySoundFX(SND_SCREAM);
#endif
  
  for (j = 0; j < 0x8; j++) {
    
    get_yx2(target_index, &y, &x);
    y2_1 = y;
    y2_2 = y;
    x2_1 = x;
    x2_2 = x;
    // loop over 29 frames and draw the wizard "breaking"
    
    // set the player colour...
    if (j == 7) {
      // clear palette 10 to make the end of run gfx effect
      // palette 10 is the final one
      clear_palette(10);
    } else {
#ifdef __WINDOWS__
      BGPaletteMem[16*pal+WIZARD_COLOUR] = winCol(chaos_cols[j]);
#else
      BGPaletteMem[16*pal+WIZARD_COLOUR] = chaos_cols[j];
#endif
    }
    for (i = 0; i < 0x1D; i++) {
      
      if (y2_1-1 != 0) {
        // draw the wiz line upwards...
        y2_1--;
        set_palette8(x-1, y2_1-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x-1, y2_1-1, 0);
      }
      if (y2_2+1 != 0x14) {
        // draw the wiz line downwards...
        y2_2++;
        set_palette8(x-1, y2_2-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x-1, y2_2-1, 0);
      }
      if (x2_1-1 != 0) {
        // draw the wiz line left
        x2_1--;
        set_palette8(x2_1-1, y-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x2_1-1, y-1, 0);
      }
      if (x2_2+1 != 0x1E) {
        // draw the wiz line right
        x2_2++;
        set_palette8(x2_2-1, y-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x2_2-1, y-1, 0);
      }
      if (x2_1 != 1 && y2_1 != 1) {
        set_palette8(x2_1-1, y2_1-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x2_1-1, y2_1-1, 0);
      }
      if (x2_2 != 0x1D && y2_1 != 1) {
        set_palette8(x2_2-1, y2_1-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x2_2-1, y2_1-1, 0);
      }
      
      if (x2_1 != 1 && y2_2 != 0x13) {
        set_palette8(x2_1-1, y2_2-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x2_1-1, y2_2-1, 0);
      }
      
      if (x2_2 != 0x1D && y2_2 != 0x13) {
        set_palette8(x2_2-1, y2_2-1, pal);
        draw_gfx8(WizardGFX[players[deadid].image].pWizardData->pGFX,
            WizardGFX[players[deadid].image].pWizardData->pMap, x2_2-1, y2_2-1, 0);
      }
      if (i & 1)
        wait_vsync_int();
    }
    
    if (pal == 9)
      pal = 10;
    else
      pal = 9;
  }
  PlaySoundFX(SND_URGH);
  clear_palettes();
  load_all_palettes();
  set_border_col(current_player);
  
  players[arena[0][target_index] - WIZARD_INDEX].modifier_flag |= 0x10;
  
  arena[0][target_index] = arena[5][target_index];
  arena[5][target_index] = 0;
  
  delay(10);
  
  destroy_all_creatures(deadid);
  if (!IS_CPU(current_player)
#ifdef __WINDOWS__
      && ! (ONLINE_MODE && IS_REMOTE(current_player))
#endif
      )
    redraw_cursor();
}


void reset_players(void) {
  u8 i, s;
  for (i = 0; i < 8; i++) {
//    players[i].name[0] = 0;
    players[i].combat = 0;
	players[i].ranged_combat = 0;
	players[i].range = 0;
	players[i].defence = 0;
	players[i].movement_allowance = 0;
	players[i].manoeuvre_rating = 0;
	players[i].magic_resistance = 0;
	players[i].spell_count = 0;
	
	for (s = 0; s < 40; s++) {
      players[i].spells[s] = 0;
    }
    
	players[i].ability = 0;
	players[i].image = 0;
	players[i].colour = 0;
	players[i].plyr_type = PLYR_HUMAN;
	players[i].modifier_flag = 0;
	players[i].illusion_cast = 0;
	players[i].selected_spell = 0;
	players[i].timid = 0;
  }
}


