#include "platform.h"

#ifdef __GBA__
#include "sound.h"
#include "screenmode.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#endif

#include "sound_data.h"
#include "magic.h"
#include "arena.h"
#include "chaos.h"
#include "wizards.h"
#include "casting.h"
#include "creature.h"
#include "computer.h"
#include "gfx.h"
#include "spelldata.h"
#include "options.h"
#include "text16.h"
#include "input.h"

void auto_cast_trees_castles(void);
void do_subversion(void);

void setup_wizard_spell(void);
void do_shield_cast(void);
void do_armour_cast(void);
void do_sword_cast(void);
void do_knife_cast(void);
void do_bow_cast(void);
void do_wings_cast(void);
void do_shadowform_cast(void);

void do_disbelieve_cast(void);

void cast_creature(void){  
  // if human just do the check on target_index, if CPU find the optimum square to cast to
  // if CPU and no square, set a target_square_found = 0 and return withough doing anything.

  temp_cast_amount = 1;  
  if (IS_CPU(current_player)) {
    // do the cpu creature cast ai routine 
    ai_cast_creature();
  } else {
    
    // call code at 9856 - validates player's spell cast
    if (!player_cast_ok()) {
      return;
    }
    
    //set_spell_success();
    spell_animation();
    if (temp_success_flag)
      creature_spell_succeeds(target_index);
    print_success_status();
    temp_cast_amount =0;
    draw_all_creatures();
    
    delay(30);
  }
  
}

void cast_fire_goo(void){
  // these are treated the same as normal creatures
  cast_creature();
}


// trees castles begins at 9add
void cast_trees_castles(void){
  if (temp_cast_amount == 0) {
    if (current_spell >= SPELL_MAGIC_CASTLE) {
      temp_cast_amount = 1;
    } else { 
      temp_cast_amount = 8;
    }
    
  }
  if (IS_CPU(current_player)) {
    // jump 9b00
    if (current_spell < SPELL_MAGIC_CASTLE) {
      // shadow wood or magic wood - uses the same routine
      // jump 9b1c (print player name and call magic wood casting...)
      temp_cast_amount = 8;
      print_name_spell();
      delay(80);
      auto_cast_trees_castles();
      
    } else {
      // check if spell is worth casting...
      if (arena[0][start_index] < SPELL_MAGIC_CASTLE || arena[0][start_index] > SPELL_DARK_CITADEL) {
        temp_cast_amount = 1;
        print_name_spell();
        delay(80);
        auto_cast_trees_castles();
        target_square_found = 1;
      } else {
        target_square_found = 0;
      }
    }
    
  } else {
    if (current_spell == SPELL_MAGIC_WOOD) {
      // do magic wood spell cast...
      temp_cast_amount = 8;
      auto_cast_trees_castles();
    } else {
      // do same checks as for creatures
      if (!player_cast_ok()) {
        return;
      }
      spell_animation();
      
      if (temp_success_flag) {
        creature_spell_succeeds(target_index);
        temp_cast_amount--;
      } else {
        temp_cast_amount = 0;
      }
      
      if (temp_cast_amount == 0)
        print_success_status();
      
      draw_all_creatures();
      delay(10);
    }
  } 
  
  
}



//
void cast_wall(void){
  if (temp_cast_amount == 0) 
    temp_cast_amount = 4;

  if (IS_CPU(current_player)) {
    // 9b85 = wall ai casting
    ai_cast_wall();
    if (target_square_found != 0) {
      print_success_status();
      delay(30);
    }
  }
  else {
    if (!player_cast_ok()) {
      return;
    }
   
    do_wall_cast();
    
    if (temp_cast_amount == 0) {
      print_success_status();
      delay(30);
    }
  }
}

// for lightning too...
void cast_magic_missile(void){
  temp_cast_amount = 1;
  if (IS_CPU(current_player)) {
    ai_cast_magic_missile();
  }
  else {
    // check this is a vlid square for human casting
    if (arena[0][target_index] == 0 || target_index == wizard_index ) {
      // if no creature, or attacking self, return
      return;
    }
    
    // in range?
    if (!is_spell_in_range(wizard_index, target_index, CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange)) {
      clear_message();
      print_text16("OUT OF RANGE", MESSAGE_X, MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(30,31,0)); // yellow
      return;
    }
    
    // do LOS check...
    if (los_blocked(target_index, 0)) {
      clear_message();
      print_text16("NO LINE OF SIGHT", MESSAGE_X, MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(31,30,0)); // yellow
      return;
    }
    
    // got to here? well cast the spell!
    do_magic_missile();
    delay(30);
    temp_cast_amount =0;
  }
  
}


void cast_subversion(void){
  temp_cast_amount = 1;
  if (IS_CPU(current_player)) {
    ai_cast_subversion();
    if (target_square_found) {
      do_subversion();
      temp_cast_amount = 0;
    }
  }
  else {
    // human...
    if (arena[0][target_index] >= SPELL_GOOEY_BLOB || arena[4][target_index] != 0 ||
    arena[0][target_index] == 0) {
      // can't cast here, as the creature is ridden by a wizard, 
      // is not a proper creature or there's nothing here
      return;
    }
    
    if (!is_spell_in_range(wizard_index, target_index, CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange)) {
      clear_message();
      print_text16("OUT OF RANGE", MESSAGE_X, MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(30,31,0)); // lblue
      return;
    }
    
    // do LOS check...
    if (los_blocked(target_index, 0)) {
      clear_message();
      print_text16("NO LINE OF SIGHT", MESSAGE_X, MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(31,30,0)); // lblue
      return;
    }
    
    // got to here then spell is valid
    temp_cast_amount = 0;
    do_subversion();
  }
  
}

void cast_chaos_law(void){
  // no need for AI on this one... the spell is really scraping the barrel!
  
  // if cpu, print player name and spell
  if (IS_CPU(current_player)) {
    print_name_spell();
    delay(80);
  }
  
  // do cast chance and update world chaos
  set_spell_success();
  
  // print spell success/fail message...
  print_success_status();
  
  target_square_found = 1;
  temp_cast_amount =0;
}

// casts justice, dark power etc
void cast_justice(void){
  if (temp_cast_amount == 0) {
    // setup cast amount
    temp_cast_amount = 1;
    if (current_spell >= SPELL_DARK_POWER) 
      temp_cast_amount = 3;
  }
  
  if (IS_CPU(current_player)) {
    // this will always be chosen, but might not actually be any good
    ai_cast_justice();
    
  }
  else {
    // code from 9df4 - spell must have passed initial cast rate to get this far
    clear_message();
    if (temp_success_flag == 0) {
      // not strictly needed ?
      temp_cast_amount =0;
      return;
    }
    
    // ok, got here then check spell is OK to cast here...
    if (arena[0][target_index] == 0)
      return;
    
    // 9e27 - only allow on proper creatures and wizards
    if (arena[0][target_index] < WIZARD_INDEX && arena[0][target_index] >= SPELL_GOOEY_BLOB)
      return;
    
    do_justice_cast();
    temp_cast_amount--;
    
  }
  
}

void cast_raise_dead(void){
  if (temp_cast_amount == 0) {
    // setup cast amount
    temp_cast_amount = 1;
  }
  
  if (IS_CPU(current_player)) {
    
    // checks if it is worht casting, and casts if so
    // otherwise sets relevent flags to ignore this spell
    ai_cast_raisedead();
    
  }
  else {
    // check that can be cast on this square
    // code from 861a
    if (arena[0][target_index] == 0) 
      return;
    if (arena[2][target_index] != 4) 
      return;
    
    if (!is_spell_in_range(wizard_index, target_index, CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange)) {
      clear_message();
      print_text16("OUT OF RANGE", MESSAGE_X, MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(30,31,0)); // lblue
      return;
    }
    
    // do LOS check...
    if (los_blocked(target_index, 0)) {
      clear_message();
      print_text16("NO LINE OF SIGHT", MESSAGE_X, MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(31,30,0)); // lblue
      return;
    }
    do_raisedead_cast();
    
    print_success_status();
    
  }
  
}

///////////////  wizard effect spells ///////////////////////

// set up the spell, do the cast the effect
void setup_wizard_spell(void) {
  // code from 8357
  if (IS_CPU(current_player)) {
    print_name_spell();
    delay(80);
  }
  set_spell_success();
  set_current_player_index();
  remove_cursor();
  spell_animation();
#ifdef DEBUG_NET
  printf("Wizard spell %s succes %d seed %d\n", 
      CHAOS_SPELLS.pSpellDataTable[current_spell]->spellName,
      temp_success_flag, GetSeed());
#endif
      
}


void cast_magic_shield(void){
  if (IS_CPU(current_player)) {
    if ( (players[current_player].modifier_flag & 0x40) ) {
      // already have something similar...
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_shield_cast();
  }
  else {
    do_shield_cast();
    temp_cast_amount =0;
  }
  
}

void do_shield_cast(void) {
  
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].image = GFX_MAGIC_SHIELD;
    players[current_player].modifier_flag &= 0x7f;
    players[current_player].modifier_flag |= 0x40;
    delay(4);
  }
  print_success_status();
  
}


void cast_magic_armour(void){
  if (IS_CPU(current_player)) {
    if ( (players[current_player].modifier_flag & 0x80) ) {
      // already have something similar...
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_armour_cast();
  }
  else {
    do_armour_cast();
    temp_cast_amount = 0;
  }
}

void do_armour_cast(void) {
  
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].image = GFX_MAGIC_ARMOUR;
    players[current_player].modifier_flag |= 0xC0;
    delay(4);
  }
  print_success_status();
  
}

void cast_magic_sword(void){
  if (IS_CPU(current_player)) {
    if ( (players[current_player].modifier_flag & 0x04) ) {
      // already have something similar...
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_sword_cast();
  }
  else {
    do_sword_cast();
    temp_cast_amount = 0;
  }
}

void do_sword_cast(void) {
  
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].image = GFX_MAGIC_SWORD;
    players[current_player].modifier_flag &= 0xfc;      // 11111100  - low 2 bits set to 0
    players[current_player].modifier_flag |= 0x04;
    delay(4);
  }
  print_success_status();
  
}

void cast_magic_knife(void){
  if (IS_CPU(current_player)) {
    if ( (players[current_player].modifier_flag & 0x07) ) {
      // already have something similar...
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_knife_cast();
  }
  else {
    do_knife_cast();
    temp_cast_amount = 0;
  }
}

void do_knife_cast(void) {
  
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].image = GFX_MAGIC_KNIFE;
    players[current_player].modifier_flag &= 0xf8;      // 11111000  - low 3 bits set to 0
    players[current_player].modifier_flag |= 0x02;
    delay(4);
  }
  print_success_status();
  
}


void cast_magic_bow(void){
  if (IS_CPU(current_player)) {
    if ( players[current_player].ranged_combat ) {
      // already fitted with the bow
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_bow_cast();
  }
  else {
    do_bow_cast();
    temp_cast_amount = 0;
  }
}

// 8486
void do_bow_cast(void) {
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].image = GFX_MAGIC_BOW;
    players[current_player].ranged_combat = 3;
    players[current_player].range = 6;
    delay(4);
  }
  print_success_status();
  
}

void cast_magic_wings(void){
  if (IS_CPU(current_player)) {
    if ( players[current_player].modifier_flag & 0x20 ) {
      // already fitted with wings
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_wings_cast();
  }
  else {
    do_wings_cast();
    temp_cast_amount = 0;
  }
}

// 
void do_wings_cast(void) {
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].image = GFX_MAGIC_WINGS;
    players[current_player].modifier_flag |= 0x20;
    delay(4);
  }
  print_success_status();
  
}


void cast_shadow_form(void){
  if (IS_CPU(current_player)) {
    if ((players[current_player].modifier_flag & 0x8) == 0x8) {
      // already have
      target_square_found = 0;
      return;
    }
    target_square_found = 1;
    do_shadowform_cast();
  }
  else {
    do_shadowform_cast();
    temp_cast_amount = 0;
  }
}

void do_shadowform_cast( void) {
  setup_wizard_spell();
  if (temp_success_flag) {
    players[current_player].modifier_flag |= 0x8;
    delay(4);
  }
  print_success_status();
  
}


///////////////  end wizard effect spells ///////////////////////

void cast_disbelieve(void) {
  if (temp_cast_amount == 0)
    temp_cast_amount = 1;
  
  if (IS_CPU(current_player)) {
    
    ai_cast_disbelieve();
    
    if (target_square_found)
      do_disbelieve_cast();
    
  } else {
    wait_for_letgo();
    if (arena[0][target_index] == 0 || arena[0][target_index] >= SPELL_GOOEY_BLOB)
      return;
    
    do_disbelieve_cast();
    temp_cast_amount = 0;
    
  }
  
}

///////////// still to do ///////////////


void cast_turmoil(void){
  if (IS_CPU(current_player)) {
    
    ai_cast_turmoil();
    
    // // computer should never cast turmoil!
    // target_square_found = 0;
    // temp_cast_amount =0;
  } else {
    // humans can cast turmoil at their own risk!
    temp_cast_amount = 1;
    do_turmoil_cast();
  }
}

//void cast_teleport(void){}


/////////////////////////////////////////
// spell validation
void auto_cast_trees_castles(void) {
  
  u8 i;
  u8 current_location = wizard_index;
  u8 tmp_cast_range = 0xD;
  
  set_spell_success();
  LUT_index = create_range_table(current_location, tmp_cast_range);
  LUT_index = LUT_index*2+1;
  u8 x, y;
  u8 got_square = 0;
  // loop over all spells...
  for (i = 0; i < temp_cast_amount; i++) {
    
    got_square = 0;
    while (!got_square) {
      u8 flag = get_best_index();
      if (flag == 0x4b) {
        // no more squares to cast to
//          redraw_los_targets();
        i = temp_cast_amount;
        break;
      }
      
      get_yx(target_index, &y, &x);
      if (x == 0x10) {
        continue;
      }
      if (arena[0][target_index] != 0) {
        continue;
      }
      
      
      if (is_tree_adjacent(target_index) == 1) {
        continue;
      }
      
      if (is_wall_adjacent(target_index) == 1) {
        continue;
      }
      
      if (los_blocked(target_index, 0) == 1) {
        continue;
      }
      
      // do spell cast anim, etc
      spell_animation();
      // check success...
      if (temp_success_flag) {
        arena[0][target_index] = current_spell;
        arena[3][target_index] = current_player;
        delay(10);
      } else {
        i = temp_cast_amount;
      }
      got_square = 1;
      
    }
    
  }
  // assume for cpu that the spell was worth casting 
  // so cpu doesn't semi check for "line of sight" stuff when seeing if spell is worth casting
  target_square_found = 1;
  temp_cast_amount = 0;
  print_success_status();
}


void do_subversion(void) {
  // do the actual casting of subvert - target_index is valid 859c
  if (IS_CPU(current_player)) {
    print_name_spell();
    delay(80);    
  }
 
  spell_animation();
   
  temp_success_flag = 0;
  if (!(arena[3][target_index] & 0x10)) {
    // if BIT 4 is set, the spell would fail 
    // i.e. we are in (if !illusion)...
    
    u8 magic_res = CHAOS_SPELLS.pSpellDataTable[arena[0][target_index]]->magicRes;
    magic_res++;
    
    u8 r = GetRand(10);
    
    if (r < magic_res ) {
      // subvert SUCCEEDED!!!??! but the random value was less than magic resistance!
      // this is how it was done in the original though...
      u8 creature_val = arena[3][target_index];
      creature_val &= 0xF8; // mask lower 3 bits
      creature_val |= current_player;
      arena[3][target_index] = creature_val;
      temp_success_flag = 1;
    } 
  }
  print_success_status();
  
}

// the implementation of a magic missile
// 9ca9 
void do_magic_missile(void) {
  // wait for redraw
  delay(4); 
  
  // spell anim
  u8 anim_type = 5;
  if (current_spell == SPELL_LIGHTNING)
    anim_type = 6;
  PlaySoundFX(SND_BEAM);
  los_blocked(target_index, anim_type);
  
  if (current_spell == SPELL_LIGHTNING) {
    PlaySoundFX(SND_ELECTRO);
  } else {
    // a splat sound?
    PlaySoundFX(SND_SPELLSUCCESS);
  }
  
  // do "splat" animation
  splat_animation();
  
  if (arena[0][target_index] == SPELL_MAGIC_FIRE ||
    ( arena[0][target_index] >= SPELL_MAGIC_CASTLE && arena[0][target_index] <= SPELL_WALL)) {
    // no effect
    return;
  }
  
  // get the defence of the creature here
  u8 defence;
  if (arena[0][target_index] >= WIZARD_INDEX) {
    // wiz
    u8 plyr = arena[0][target_index] -WIZARD_INDEX;
    defence = players[plyr].defence;
    if ((players[plyr].modifier_flag & 0xc0) == 0x40) {
      // has magic shield
      defence += 2;
    } else
    if ((players[plyr].modifier_flag & 0xc0) == 0x80) {
      // has magic armour
      defence += 4;
    }
    
  } else {
    // creature
    defence = CHAOS_SPELLS.pSpellDataTable[arena[0][target_index]]->defence;
  }
  
  defence += GetRand(10);
  u8 attack = 3 + GetRand(10);
  
  if (current_spell == SPELL_LIGHTNING) {
    attack += 3;
   
  }
  
  
  if (attack < defence)
    return;
  
  wait_vsync_int();
  wait_vsync_int();
  
  // do the pop animation...
  pop_animation();
  
  // new code...
  if (arena[4][target_index] == 0) {
    // nothing in arena 4...
    if (arena[0][target_index] >= WIZARD_INDEX) {
      // was a wizard, do wizard death anim...
      kill_wizard();
    }  else {
      // remove the creature
      arena[0][target_index] = 0;
      if (!Options[OPT_OLD_BUGS] && arena[5][target_index] != 0) {
        // what about dead bodies?
        // but only if arena 4 was empty
        // bug fix v0.7a (disbelieve failed with old bugs turned off, fixed here too)
        arena[0][target_index] = arena[5][target_index]; //creature in arena 5
        arena[2][target_index] = 4; // dead
        arena[5][target_index] = 0; //clear creature in arena 5 
      }
    }
    
  } else {
    // arena 4 had something in it
    u8 arena4 = arena[4][target_index];
    arena[4][target_index] = 0;
    if (!Options[OPT_OLD_BUGS]) {
      // an old bug was destroying gooey blob results in creature under blob 
      // taking the same owner as the blob... 
      // e.g. wizard 1 blob covers wizard 2 creature
      // someone kills the blob, wizard 2's creature now belongs to wizard 1!
      if (arena[0][target_index] == SPELL_GOOEY_BLOB) {
        arena[3][target_index] = arena[5][target_index];
      }
      else {
        // the famous "undead wizard" bug is caused by not updating the arena[3] flag properly
        if (arena4 >= WIZARD_INDEX) {
          arena[3][target_index] = arena4-WIZARD_INDEX;
        }
      }
    }
    
    arena[0][target_index] = arena4;
    arena[5][target_index] = 0;
    
  }
  
  /* old code
  if (arena[0][target_index] >= WIZARD_INDEX) {
      // jump to wizard death stuff..
    kill_wizard();
  }
  else {
    arena[0][target_index] = arena[4][target_index];
    arena[4][target_index] = 0;
    if (!Options[OPT_OLD_BUGS] && arena[5][target_index] != 0
    && arena[0][target_index] == 0) {
      // what about dead bodies?
      // but only if arena 4 was empty
      // bug fix v0.7a (disbelieve failed with old bugs turned off, fixed here too)
      arena[0][target_index] = arena[5][target_index]; //creature in arena 5
      arena[2][target_index] = 4; // dead
      arena[5][target_index] = 0; //clear creature in arena 5 
    }
    
  }
  */
}


void do_wall_cast(void) {
  spell_animation();
  if (temp_success_flag)
    creature_spell_succeeds(target_index);
  else 
    temp_cast_amount = 0;    
  if (temp_cast_amount > 0)
    temp_cast_amount--;
  draw_all_creatures();
  wait_vsync_int();
  load_all_palettes();
}


void do_justice_cast(void) {
  // code from 9e38 onwards
  // do sound fx, flashing graphics
  move_screen_to(target_index);
  
  u8 i,j,k;
  PlaySoundFX(SND_JUSTICE);
  for (i = 0; i < 3; i++) {
    for (k = 0; k < 3; k++) {
      for (j = 0; j < 8; j++) {
        // do the flashing gfx...
        wait_vsync_int();
        wait_vsync_int();
        if (arena[0][target_index] >= WIZARD_INDEX) {
          draw_silhouette_gfx(target_index, 
          WizardGFX[players[arena[0][target_index] - WIZARD_INDEX].image].pWizardData->pGFX, 
          WizardGFX[players[arena[0][target_index] - WIZARD_INDEX].image].pWizardData->pMap,
          chaos_cols[j], 11, 0); //  final 0 is to draw the gfx "positive"
        }
        else {
          draw_silhouette_gfx(target_index, 
          CHAOS_SPELLS.pSpellDataTable[arena[0][target_index]]->pGFX, 
          CHAOS_SPELLS.pSpellDataTable[arena[0][target_index]]->pGFXMap,
          chaos_cols[j],11, 0); //  final 0 is to draw the gfx "positive"
        }
        
      }
      
    }
    
    
  }
  
  
  // 9e96
  // refresh arena
  // set the has cast spell flag
  
  // get magic resistance of creature...
  u8 magres;
  if (arena[0][target_index] >= WIZARD_INDEX) {
    // wizard res
    magres = players[arena[0][target_index] - WIZARD_INDEX].magic_resistance;
  } else {
    // creature res
    magres = CHAOS_SPELLS.pSpellDataTable[arena[0][target_index]]->magicRes;
  }
  magres++;
  
  u8 r = GetRand(10);
  
  if (r >= magres) {
    // spell succeeded...
    // do pop anim
    
    pop_animation();
    
    if (arena[0][target_index] >= WIZARD_INDEX) {
      // wizard - destroy all his creations!
      delay(4);
      destroy_all_creatures(arena[0][target_index] - WIZARD_INDEX);
      
    }
    else {
      // single creature only...
      // there's the famous "rise from the dead" bug here...
      arena[0][target_index] = 0;
      if (arena[4][target_index] != 0) {
        arena[0][target_index] = arena[4][target_index];
        arena[4][target_index] = 0;
      } else if (arena[5][target_index] != 0) {
        arena[0][target_index] = arena[5][target_index];
        arena[5][target_index] = 0;
        if (!Options[OPT_OLD_BUGS]) {
          // make sure this creature is dead, as arena 5 creatures are dead bodies
          arena[2][target_index] = 4;
        } 
      }
    }
    delay(4);
  }
  
}


// code from 86c3
void do_raisedead_cast(void) {
  // implementation of the raise dead spell
  // used by human and cpu players
  
  // spell animation to target square
  spell_animation();
  set_spell_success();
    
  if (!temp_success_flag) {
    temp_cast_amount = 0;
    return;
  }
  // set target frame to 0
  arena[2][target_index] = 0;
  u8 flag = 0x60; // bits 5 & 6 - is real, is known to the ai to be real and is undead
  flag |= current_player;
  // update this creature's flag val so it is undead and this player's
  arena[3][target_index] = flag;
  arena[5][target_index] = 0; // just in case
  temp_cast_amount--;
}

// implements the casting of disb... 
// code from 9a21 and used by AI and humans
void do_disbelieve_cast(void) {
  
  spell_animation();
  temp_success_flag = 0;
  wait_vsync_int();
  wait_vsync_int();
  wait_vsync_int();
  wait_vsync_int();
  // check arena 3 value, bit 4
  if (IS_ILLUSION(arena[3][target_index])) {
    // place the arena 4 riding wizard, or 0, in arena 0
    arena[0][target_index] = arena[4][target_index];
    arena[4][target_index] = 0;
    if (!Options[OPT_OLD_BUGS] && arena[5][target_index] != 0
    && arena[0][target_index] == 0) {
      // what about dead bodies?
      // but only if arena 4 was empty
      // bug fix v0.7a (disbelieve failed with old bugs turned off)
      arena[0][target_index] = arena[5][target_index]; //creature in arena 5
      arena[2][target_index] = 4; // dead
      arena[5][target_index] = 0; //clear creature in arena 5 
    }
    
    
    pop_animation();
    temp_success_flag = 1;
    
  }
  delay(10);
  print_success_status();
  // set bit 5 for this creature (won't matter if disbelieved already...)
  // bit 5 is "has been disbelieved" and is used for the AI to know which creatures
  // are known to be real and which are not known... 
  arena[3][target_index] |= 0x20;
  
}

void do_turmoil_cast(void) {
  int i;
  u8 tsi = start_index;
  set_spell_success();
  disable_interrupts();
  temp_success_flag = 1;
  if (temp_success_flag) {
    // unset bit 7 of all things
    unset_moved_flags();
    u8 arena0, arena2, arena3, arena4, arena5;
    u8 r,x,y;
    for (i = 0; i < 0x9f; i++) {
      // loop over the whole arena
      if (arena[0][i] == 0)
        continue;
      if (HAS_MOVED(arena[3][i])) // moved and turmoil effect the same BIT
        continue;
      // store the start square data
      arena0 = arena[0][i];
      arena2 = arena[2][i];
      arena3 = arena[3][i];
      arena4 = arena[4][i];
      arena5 = arena[5][i];
      // get a new square to send this baby to
      r = GetRand(10) + GetRand(255);
      if (r < 0x9f) { 
        get_yx(r,&y,&x);
      } else {
        // make sure we get another val
        x = 0x10;
      }
      
      /*
        All these squares are full:
        
        1  2  3 
        4  5  6
        7  8  9
        
        Start at 0
        get random, eg "3"
        Aha! can't go there, get new random square...
        Whatever square we get is full
        and so on for ever...
      */
      
      while (r >= 0x9f || x >= 0x10 || arena[0][r] != 0) {
        r = GetRand(10) + GetRand(255);
        if (r < 0x9f) {
          get_yx(r,&y,&x);
        } else {
          x = 0x10;
        }
        
      }
      
      // got a new square to cast to
      target_index = r;
      start_index = i;
      spell_animation();
      
      arena[0][target_index] = arena0;
      arena[2][target_index] = arena2;
      arena[3][target_index] = arena3 | 0x80;   // moved/turmoiled
      arena[4][target_index] = arena4;
      arena[5][target_index] = arena5;
      
      arena[0][i] = 0;
      arena[1][i] = 1;
      arena[4][i] = 0;
      arena[5][i] = 0;
      
      enable_interrupts();
      delay(4);
      disable_interrupts();
    }
  }
  enable_interrupts();
  start_index = tsi;
  print_success_status();
  
  temp_cast_amount = 0;
  
}
