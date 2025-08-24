// computer.c

#include "platform.h"
#ifdef __GBA__
#include "screenmode.h"
#endif

#include "computer.h"
#include "movement.h"
#include "arena.h"
#include "wizards.h"
#include "magic.h"
#include "casting.h"
#include "creature.h"
#include "spelldata.h"
#include "chaos.h"
#include "gfx.h"

unsigned char target_square_found;
unsigned char move_table_created;
unsigned char has_wizard_moved;
unsigned char flying_target_found;
unsigned short LUT_index;
unsigned short priority_offset;

void do_flying_move(void);
void reset_priority_table(void);
void create_table_enemies(void);
void create_table_wizards(void);
void create_enemy_table_entry(u8 wizardid);

void do_this_movement(void);
void wizard_flying_move(u8 type);

u8 get_best_rangeattack(void);
int create_range_table(int target, int range);
u8 get_strongest_wizard(int attacker_index);
u8 get_priority_val(u8 index);
void setup_wizard_move(void);
void setup_creature_move(void);
void get_furthest_inrange(void);

// based on code at c64c
void order_table(u8 count, u8 * table) {
  // orders "count" entries in the given table by priority
  // table contains priority, index pairs
  
  u16 i, j; // loop counters
  u16 index = 0;
  u8 p1, p2, val1, val2; // priority 1, 2, value 1, 2
  for (i = 0; i < count; i++) {
    for (j = 0; j < count; j++) {
      p1 = table[index];
      index++;  
      val1 = table[index];
      index++;  
      p2 = table[index];
      index++;
      val2 = table[index];
      index--;   // points to prio 2
      if (p2 > p1) {
        index++;  // points to val 2
        table[index] = val1;
        index--;  // points to prio 2
        table[index] = p1;
        index--;  // points to val 1
        table[index] = val2;
        index--;  // points to prio 1
        table[index] = p2;
        index++;  // points to val 1
        index++;  // points to prio 2
      }
    }
      
    index = 0;
  }
}

// attack preference array...
// taken from c6d4
// except troll, which is new
const char attack_pref[48] = {
  0x08/*cobra*/,    0x0b/*wolf*/,    0x07/*goblin*/,    0x09/*croc*/,
  0x09/*troll*/,    0x09/*faun*/,    0x0f/*lion*/,      0x0e/*elf*/,
  0x06/*orc*/,      0x0e/*bear*/,    0x0b/*goril*/,     0x0a/*ogre*/,
  0x0d/*hydra*/,    0x09/*rat*/,     0x10/*giant*/,     0x0b/*horse*/,
  0x0f/*unicor*/,   0x0f/*cent*/,    0x0f/*peg*/,       0x10/*gryph*/,
  0x15/*manti*/,    0x0f/*bat*/,     0x19/*green*/,     0x19/*red*/,
  0x1b/*gold*/,     0x11/*harpy*/,   0x12/*eagle*/,     0x13/*vamp*/,
  0x0d/*ghost*/,    0x09/*spectre*/, 0x0b/*wraith*/,    0x07/*skeleton*/,
  0x04/*zombie*/,   0x04/*blob*/,    0x04/*fire*/,      0x00/*wood*/,
  0x01/*shadow*/,   0x00/*castle*/,  0x00/*cit*/,       0x00/*wall*/,
  /*wizards*/
  0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,};
  

// check the square in index to see if it contains a live enemy creature
// based on code at c67a
u8 contains_enemy(u16 index) {
  u8 creature = arena[0][index];
  if (creature < 2) {
    return 0; // jr c6d2 - was if creature == 0 jump, but that is buggy
  }
  
  if (arena[2][index] == 4) {
    // frame 4 showing, i.e. dead
    return 0; // jr c6d2
  }
  
  u8 owner = GET_OWNER(arena[3][index]); // mask lower 3 bits, the owner
  if (owner == current_player) {
    return 0; // jr c6d2
  }
  
  
  // got to here? contains a living enemy creature....
  // get the "creature attack preference" value
  u16 range;
  get_distance(index, start_index, &range);
  u16 pref = attack_pref[creature-2] + 4;

  // pref += var_cc55; // no idea what is stored here... 0x60 for lightning
  pref += priority_offset; // usually 0, but can be other values when we care about all creatures
  
  
  if ( (pref - range) < 0) {
    return 0;
  } 
  
  return (pref - range);
  
}


// create the priority table - stored at d3f2
// will contain {priority, index} for all living enemy creatures
unsigned char prio_table[320];
u8 target_count;

// based on code at c7bc
void create_table_enemies(void) {
  u8 i;
  
  start_index = wizard_index;
  u16 index = 0; // index to prio table
  target_index = 0; // the current "target" square
  target_count = 0;  // the number of targets
  reset_priority_table();
  
  u8 valid = 0;
  u16 range;
  for (i = 0; i < 0x9f; i++) {
    valid = contains_enemy(target_index);
    if (valid > 0) {
      target_count++;
      get_distance(target_index, start_index, &range);
      range = range >> 1;
      valid += 0x14;
      valid -= range;
      prio_table[index] = valid;
      index++;
      prio_table[index] = target_index;
      index++;
    }
    target_index++;
  }
}

// based on code at cdaa
void reset_priority_table(void) {
  u16 index = 0;
  u8 i;
  for (i = 0; i < 0x9E; i++) {
    prio_table[index] = 0x00;
    index++;
    prio_table[index] = 0xFF;
    index++;
  }
}


// based on code at c955 
// returns squares in range...
//s16 create_range_table(u8 target, u8 range) 

int create_range_table(int target, int range)
{
  
  // fill the priority table with all squares in range 
  // distance is from target index
  u8 i;
  u16 tmprange;
  u8 range_count = 1;
  u8 pi = 0;
  reset_priority_table();
  
  for (i = 0; i < 0x9f; i++) {
    get_distance(i, start_index, &tmprange);
    if (range >= tmprange) {
      // square is in range
      get_distance(i, target, &tmprange);
      prio_table[pi] = tmprange;
      pi++;
      prio_table[pi] = i;
      pi++;
      range_count++;
    }
  }
  
  order_table(range_count, prio_table);
  return range_count;
  
}



// the AI spell casting is roughly based on code starting at 96f3
void do_ai_spell(void) {
  /*
    set spell 0 priorty = 0x0c + GetRand(10) (I imagine this is in case Disbelieve was top priority?)
    order the spell list by priority
    best spell = 0
    while (best spell < spell list length) {
      select the best spell 
      select the best square for this spell
      if no square is good
        best spell ++
      else
        break
    }
    
    if a spell has been chosen...
      cast spell at chosen square
      remove casted spell from list
    
    move on to next player...
  */
  
  if (IS_WIZARD_DEAD(players[current_player].modifier_flag)) {
    return;
  }
  
  temp_cast_amount = 0;
  players[current_player].spells[0] = 0x0c + GetRand(10);
  order_table(players[current_player].spell_count, players[current_player].spells);
  u8 best_spell = 0;
  while (best_spell < players[current_player].spell_count) {
    current_spell = players[current_player].spells[best_spell*2 + 1];
    // "cast" the spell... each casting routine has the CPU AI code for that spell built into it.
    // if no good square was found, then go to the next spell
    target_square_found = 0;
    if (current_spell != 0) {
      target_square_found = 1;
      set_current_spell_chance();
      CHAOS_SPELLS.pSpellDataTable[current_spell]->pFunc();

    }
    
    if (target_square_found) {
      
      // spell was cast succesfully
      if (current_spell != 0x1) {
        // spell used (if not disblv)
        players[current_player].spells[best_spell*2 + 1] = 0;
      }
//      break;
      return;
    } else {
      best_spell++;
    }
  }
  
}

// the computer tries to cast a creature..
// code based on 9984
void ai_cast_creature(void) {
  
  // get the square closest to the best target...
  u16 strongest_index;
  strongest_index = get_strongest_wizard(0xFF);
  
  start_index = wizard_index;
  target_square_found = 0;
  u8 range = 3;
  if (current_spell >= SPELL_GOOEY_BLOB)
    range = 13;
  
  s16 in_range = (s16)create_range_table(strongest_index, range);
  
  LUT_index = in_range*2+1;
  get_furthest_inrange();
  
  if (target_square_found) {
    print_name_spell();
    delay(80);
    spell_animation();
    temp_illusion_flag = 0;
    
    if ( (current_spell < SPELL_GOOEY_BLOB)) {
      // randomly cast illusions, but more chance of trying if the spell is tricky to cast
      if ( (current_spell_chance < 4 && GetRand(10) > 5) || (GetRand(10) > 7) )
        temp_illusion_flag = 1;
    }
    
    set_spell_success();
    
    
    if (temp_success_flag)
      creature_spell_succeeds(target_index);
    print_success_status();
    
    draw_all_creatures();
    delay(10);
  } // else no square found!
  
  temp_cast_amount = 0;
}

// c9dc
void get_furthest_inrange(void) {
  // get the furthest square away still in range...
  do {
    target_index = prio_table[LUT_index];
    
    delay(5);
    // check xpos < 10  - code at c63d
    u8 x,y;
    get_yx(target_index, &y,&x);
    if (x < 0x10) {
      // in range
      if (!los_blocked(target_index, 0)) {
        
        if (arena[0][target_index] == 0) {
          // nothing here
          target_square_found = 1;
        } else if (arena[2][target_index] == 4){
          // is the thing here dead?
          target_square_found = 1;
        }
      }
    }
    if (LUT_index - 2 < 0)
      LUT_index = 0;
    else
      LUT_index -=2;
  } while (LUT_index > 0 && (target_square_found == 0));
  
}

// based on code at c78d
// additionally, pass in the attakcer - or 0xff if no attacker ready 
// and check "current spell" value
u8 get_strongest_wizard(int attacker_index) {
  if (players[current_player].timid < 0xA) {
    // if we have less than 10 in this value, 
    // go for the wizard who's creature is closest to us
    create_table_enemies();
    
    // the best value will now be the index of the creature closest to us
    // or the wizard closest to us, as they are treated the same in the create_table_enemies routine
    u8 enemy_creature = prio_table[0];
    
    if (enemy_creature == 0) {
      
      // no dangerous creature found..
      // create the priority table based on "strongest wizard" - one with most/best creatures
      create_table_wizards();
      target_index = prio_table[1];
      
      
      // in the actual game it does a pointless thing here...
      return target_index;
      
    } else {
      // first of all, get the most dangerous creature... regardless of whether we can kill it or not
      // if we can't kill it, later we will attack its owner to do away with it
      order_table(target_count, prio_table);
      
      enemy_creature = prio_table[1];
      
      u8 enemy_wizard = GET_OWNER(arena[3][enemy_creature]); 
      
      u8 i;
      for (i = 0; i < 0x9f; i++) {
        if (arena[0][i] - WIZARD_INDEX == enemy_wizard) {
          enemy_wizard = i;
          break;
        }
        if (arena[4][i] - WIZARD_INDEX == enemy_wizard) {
          enemy_wizard = i;
          break;
        }
      }
      // enemy wizard = index to the enemy wizard attacking us
      // enemy creature = index to the enemy creature attacking us
      // now see which is closer and attack them
      u16 creature_range, wizard_range;
      get_distance(enemy_creature, start_index, &creature_range);
      get_distance(enemy_wizard, start_index, &wizard_range);
      
      /* 
        Magic Fire close to a wizard confuses the enemy creatures
        Try this out, so that fire is not attractive
        Also, undead creatures should not be attractive to living ones
      */
      u8 attacker_undead = 0;
      if (attacker_index == 0xff) {
        // a creature spell to cast... so use the spellid
        attacker_undead = (current_spell >= SPELL_VAMPIRE  &&
                            current_spell <= SPELL_ZOMBIE );
      } else {
        // is a valid square
        // see if the attacking square is undead
        attacker_undead = IS_UNDEAD(arena[3][attacker_index]) ||
                          (arena[0][attacker_index] >= SPELL_VAMPIRE  &&
                            arena[0][attacker_index] <= SPELL_ZOMBIE );
      }
      
      u8 defender_undead = 0;
      
      int tmp_prio_index = 1;
      while (enemy_creature != 0xFF) {
        enemy_creature = prio_table[tmp_prio_index];
        if (arena[0][enemy_creature] != SPELL_MAGIC_FIRE) {
          // not magic fire so check undeadness
          // remember that we are still scared of magic fire and undead creatures
          // just that we are not worried about attacking it
          defender_undead = (arena[0][enemy_creature] >= SPELL_VAMPIRE  &&
                            arena[0][enemy_creature] <= SPELL_ZOMBIE ) || 
                            IS_UNDEAD(arena[3][enemy_creature]);
          if ( !defender_undead || attacker_undead) { 
            
            break;
          } 
        }
        
        
        tmp_prio_index += 2;
      }
      
      if (enemy_creature == 0xFF) {
        return enemy_wizard;
      }
      
      if (creature_range < wizard_range) {
        return enemy_creature;
      }
      return enemy_wizard;
    }
    
  } else {
    // create the priority table based on "strongest wizard" - one with most/best creatures
    create_table_wizards();
    target_index = prio_table[1];
    
    // in the actual game it does a pointless thing here...
    return target_index;
  }
  
}

// create a table of the strongest wizards, based on quantity/wuality of creatures

u8 prio_table_index;

void create_table_wizards(void) {
  // code is from c825 
  reset_priority_table();
//  char str[20];
  
  prio_table_index = 0;
  u8 i;
  target_index = 0;
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][target_index] >= WIZARD_INDEX) {
      create_enemy_table_entry(arena[0][i]-WIZARD_INDEX);
    }
    if (arena[4][target_index] >= WIZARD_INDEX) {
      create_enemy_table_entry(arena[4][i]-WIZARD_INDEX);
    }
    target_index++;

  }
  
  order_table(7, prio_table);
  
  
}

// create an enemy table entry for this wizard
// code at c859
void create_enemy_table_entry(u8 wizardid) {
  if (wizardid == current_player) 
    return;
  
  prio_table[prio_table_index+1] = target_index;
  u8 i;
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] > 1 && arena[0][i] < 0x26) {
      if (GET_OWNER(arena[3][i]) != wizardid) {
        prio_table[prio_table_index] = (attack_pref[arena[0][i]]>>2) + prio_table[prio_table_index] 
                                      + priority_offset;
        
      }
    }
    
  }
  prio_table_index +=2;
    
}

// code based on c8c7
// get the priorities of squares surrounding "index" based on distance from "strongest"
void get_surrounding_square_prios(u8 index, u8 strongest) {
  
  reset_priority_table();
  u8 i;
  u8 surround_index = 0;
  u16 range;
  u8 lookat_i;
  LUT_index = 0;
  for (i = 0; i < 8; i++) {
    lookat_i = apply_position_modifier(index, surround_index);
    surround_index++;
    if (lookat_i == 0) {
      // out of bounds
      continue;
    }
    lookat_i--;
    
    get_distance(strongest, lookat_i, &range);
    // add a check in here to make sure we don't
    // tread on a magic wood. This table is ordered largest
    // first. But the potential squares are read starting 
    // at the end (LUT_index, then LUT_index-2, etc)
    if (arena[0][lookat_i] == SPELL_MAGIC_WOOD)
      range += 1;  // "increase" the distance so it isn't as appealing
                   // careful not to increase too much, may go backwards!
    
    prio_table[LUT_index++] = range;
    prio_table[LUT_index++] = i;
    
  }
  order_table(7, prio_table);
  LUT_index = 0xF;
  
}



void do_ai_movement(void) {
  /*
   if wiz has creatures to do his bidding...

     get current position, set up relative
     movement advantages for squares we can go to

     if we are in a wood or castle, stay there  
     else ...
     select the wizard
     do the movement
   
   end if
   
   for all squares on the board...
    if we have a creature here
    then try selecting it
      if we selected it successfully (it hadn't moved already)
        then do the movement for this creature
   end for

   so if the wizard is creatureless, he himself acts like a normal creature
   */
  
  // the computer movement stuff...
  if (players[current_player].timid == 0) {
    //  ca92...
    setup_wizard_move();
    
    if (arena[0][start_index] == SPELL_MAGIC_WOOD ||
    arena[0][start_index] == SPELL_MAGIC_CASTLE ||
    arena[0][start_index] == SPELL_DARK_CITADEL) {
      // set has moved...
      has_wizard_moved = 1;
      arena[3][start_index] = arena[3][start_index] | 0x80;
    } else {
      // move wizard...
      // select the wizard...
      has_wizard_moved = 0;
      // and we have the move table in memory..
      move_table_created = 1;
      
      movement_a(); // select the wizard...
      delay(30);
      // now try moving it to the target square...
      do_this_movement();
      
      delay(10);
    }
  }
  // here? move creatures
  // we have already dealt with the wizard...
  has_wizard_moved = 1;
  
  u8 i;
  for (i = 0; i < 0x9f; i++) {
    target_index = i;
    start_index = i;
    movement_a(); // select the creature...
    if (selected_creature != 0) {
      delay(30);
      do_this_movement();
    }
    
  }
  delay(20);
}

void do_this_movement(void) {
  // this is effectively the first half of the movement polling
  // the code in Chaos starts at ae50 and is only called if a 
  // creature is successfully selected...
  
  while (selected_creature != 0) {
    
    if (tmp_is_flying) {
      // jump b0a8
      
      do_flying_move();
      // what if it becomes engaged?
      // in chaos code, jumps to ae7a
      if (tmp_range_attack != 0) {
        goto range_attacking;
      }
      
    }
    else {
      // walking creature...
      if (!has_wizard_moved) {
        // ae67 - wizard has not moved yet...
        if (!move_table_created) {
          // need to create the wizard movement table
          setup_wizard_move();
        }
      } else {
        // wizard has moved already.. 
        // ae70
        if (!move_table_created) {
          // need to create the creature movement table
          setup_creature_move();
        }
      }  
      
      // get the first best square to move to...
      while(prio_table[LUT_index] == 0xFF) {
        if (LUT_index > 2) {
          LUT_index -= 2;
        } else {
          movement_b();
          // if we have range attack after our movement, make sure it is handled properly
          // best solution is a goto here... otherwise I'd have a load of confusing ifs
          if (tmp_range_attack != 0) {
            goto range_attacking;
          }
          
          return;
        }
        
      }
      
      target_index = apply_position_modifier(start_index, prio_table[LUT_index]);
      target_index--;

      clear_message();
      movement_a(); 
      
      delay(4);
      
      if (tmp_range_attack != 0) {
        // could have used break instead of goto
        // but goto makes the intention clearer
        goto range_attacking;
      }
      
      // get the next square to move to
      // note that movement_a() sets "move_table_created" to 0, 
      // so after a move, we need to remake the table.
      if (LUT_index > 2) {
        LUT_index -= 2;
      } else {
	// no good square to go to..
        movement_b();
        
        if (tmp_range_attack != 0) {
          // again, could have used break instead of goto
          // but goto makes the intention clearer
          goto range_attacking;
        }
        return;
      }

    }
    
  }
  
// GOTO label - for range attacks jump here
range_attacking:
  while (tmp_range_attack != 0) {  
    
    
    // range attack... this is identical for flying or not, wiz or not...
    // get the best value (call cccb)
    u8 ra = get_best_rangeattack();
    if (ra == 0x4b) {
      // the best square is crap
      movement_b();
      return;
    } else {
      // the target square is good
      movement_a();
    }
    
  }
  delay(20);
}

// defines for the special moves the wizard can make, with wings or on a mount
#define SPECIAL_MOVE_MOUNT 1
#define SPECIAL_MOVE_WINGS 2

// ca92.
void setup_wizard_move(void) {
  reset_priority_table();
  // get the arena square with the wizard in it...
  u8 i;
  u8 tmp_wiz = 0;
  for (i = 0; i < 0x9f; i++) {
    if (arena[0][i] >= WIZARD_INDEX && (arena[0][i] - WIZARD_INDEX) == current_player) {
      tmp_wiz = i;
      break;
    }
    if (arena[4][i] >= WIZARD_INDEX && (arena[4][i] - WIZARD_INDEX) == current_player) {
      tmp_wiz = i;
      break;
    }
  }
  target_index = tmp_wiz;
  // check the value in arena 0 at the wizard's location
  if (arena[0][target_index] >= SPELL_PEGASUS && arena[0][target_index] <= SPELL_MANTICORE) {
    // wizard on a flying mount...
    // cade...
    // call cbb9
    wizard_flying_move(SPECIAL_MOVE_MOUNT);
    has_wizard_moved = 1;
    return;
  } else {
    // not on a flying mount (may be on a regular one though)
    
    if (arena[0][target_index] >= WIZARD_INDEX 
	&& HAS_MAGICWINGS(players[current_player].modifier_flag) 
      /* bug fix 26/12/04 
       * If wiz is flying and engaged to undead, causes problems
       */
      && tmp_is_flying)
    {
      // has magic wings and is in the open
      // cafb....
      // call cbc7
      wizard_flying_move(SPECIAL_MOVE_WINGS);
      has_wizard_moved = 1;
      return;
    } else {
      // cb06...
      u8 surround_index = 0;
      u8 lookat_i;
      LUT_index = 0;
      for (i = 0; i < 8; i++) {
        lookat_i = apply_position_modifier(target_index, surround_index);
        surround_index++;
        if (lookat_i == 0) {
          // out of bounds
          continue;
        }
        lookat_i--;
        if (arena[0][lookat_i] >= SPELL_MAGIC_CASTLE 
	    && arena[0][lookat_i] <= SPELL_DARK_CITADEL) 
	{
          prio_table[LUT_index++] = 0;
          prio_table[LUT_index++] = i;
          target_square_found = 1;
        } else
        if (arena[0][lookat_i] == SPELL_MAGIC_WOOD) {
          prio_table[LUT_index++] = 1;
          prio_table[LUT_index++] = i;
          target_square_found = 1;
        } else 
        if (arena[0][lookat_i] >= SPELL_HORSE && arena[0][lookat_i] <= SPELL_MANTICORE
        && (arena[3][lookat_i] & 7) == current_player  && arena[4][lookat_i] == 0) {
          // is a mount and is ours and no one is on it 
          prio_table[LUT_index++] = 2;
          prio_table[LUT_index++] = i;
          target_square_found = 1;
        } else {
          // is none of the above
          prio_table[LUT_index++] = get_priority_val(lookat_i)+3;
          prio_table[LUT_index++] = i;
        }
          
      }
      // cb8b
      order_table(7, prio_table);
      start_index = tmp_wiz;
      target_index = tmp_wiz;
      LUT_index = 0xF;
    } // end if has magic wings/out in open
  
  } // end if wizard on mount
  // cba2
  move_table_created = 1;
}

// used for wizards on flying mounts or with wings
void wizard_flying_move(u8 type) {
  // based on code at cbb9/cbc7
  if (type == SPECIAL_MOVE_MOUNT) {
    tmp_movement_allowance = 1+
      (CHAOS_SPELLS.pSpellDataTable[arena[0][target_index]]->movement<<1); 
  } else {
    // wings
    tmp_movement_allowance = 0xd;
  }
  
  reset_priority_table();
  
  //cbcf
  target_count = 1;
  LUT_index = 0;
  u8 tmp_wiz = target_index;
  u8 i, x, y;
  u16 tmp_dist;
  u8 tmp_prio_val;
  // loop over arena to get priority of all squares in range
  for (i = 0; i < 0x9e; i++) {
    get_yx(i, &y, &x);
    if (x < 0x10) {
      // in bounds
      // get the distance from wizard start square to this square (i)
      get_distance(tmp_wiz, i, &tmp_dist);
      if (tmp_movement_allowance >= tmp_dist) {
        // is in range of the wiz start square...
        // find the distance/danger of all enemies relative to this square  (call cd92)
        // store as priority // code to here is at cc0d
        tmp_prio_val = get_priority_val(i);
        prio_table[LUT_index++] = tmp_prio_val;
        prio_table[LUT_index++] = i;
        target_count++;
      }
    }  
  }
  // cc31
  order_table(target_count, prio_table);
  start_index = tmp_wiz;
  LUT_index--;
}


// taken from cd92 - gets the "danger" of the square at i
// this is calculated based on enemy creature distance and how dangerous that creature is
u8 get_priority_val(u8 index) {
  u8 i;
  u8 total = 0;
  start_index = index;
  for (i = 0; i < 0x9e; i++) {
    total += contains_enemy(i); // gets the priority val for this square from start_index
  }
  return total;
}

// code based on that at c8c7...
void setup_creature_move(void) {
  reset_priority_table();
  u8 current_index = target_index; // save the current square, just in case
  u16 strongest_index;
  strongest_index = get_strongest_wizard(current_index);
  get_surrounding_square_prios(current_index, strongest_index);
  
  start_index = current_index;
  target_index = current_index;
  
  move_table_created = 1;
}

// taken from b0a8
void do_flying_move(void) {

  //char str[30];
  u8 current_index = start_index; // save the current square, just in case
  u16 strongest_index;
  /* bug fix 26/12/04 
   * If wiz is flying and engaged to undead, causes problems
   */
  if (tmp_engaged_flag) {
    tmp_movement_allowance = 3;
  }
  if (!flying_target_found) {
    
    if (!has_wizard_moved) {
      // ae67 - wizard has not moved yet...
      if (!move_table_created) {
        // need to create the wizard movement table
        setup_wizard_move();
      }
      // code here really follows after cc31
      // gets the "worst" square for the wizard (i.e. least dangerous)
      while(prio_table[LUT_index] == 0xFF) {
        if (LUT_index > 2) {
          LUT_index -= 2;
        } else {
          movement_b();
          return;
        }
        
      }
      strongest_index = prio_table[LUT_index];
    } else {
      // wizard has moved already.. 
      // need to create the creature movement table
      reset_priority_table();
      strongest_index = get_strongest_wizard(current_index);
      
    }
    start_index = current_index;
    // for creatures, si will be the one closest to the "best" enemy
    // for wizards, will be the safest square in range
    s16 in_range = create_range_table(strongest_index, tmp_movement_allowance);
    LUT_index = in_range*2 + 1;
    flying_target_found = 1;
    
  }
  
  u8 bi = get_best_index();
  
  if (bi == 0x53) {
    start_index = current_index;
    // found one! target_index contains the target square...
    
    flying_target_found = 1;
    // move_screen_to(target_index);
    clear_message();
    movement_a();
    delay(30);
  } else {
    // ran out...
    movement_b();
    return;
  }
 
}


// based on code around c9dc
int get_best_index(void) {
  // get the best index of the prio table.. will be the one 
  // get the furthest square away still in range...
  u8 y,x;
  do {
    if (LUT_index -1 <= 0) 
      return 0x4b;
    if (prio_table[LUT_index] == 0xFF) {
      LUT_index--;
      LUT_index--;
    } else {
      target_index = prio_table[LUT_index];
      // check xpos < 10
      
      get_yx(target_index, &y, &x);
      if (x < 0x10) {
        // in range
         
        LUT_index--;
        LUT_index--;
        return 0x53;
      } else {
        LUT_index--;
        LUT_index--;
      }
    }
  } while (LUT_index > 0);
  return 0x4b;
}

// called at the start of lightning casting amongst other stuff...
// creates the priority table based on all enemy creatures and wizards
// wizards are rated higher than creatures and table is ordered and 
// the first value in the table is pointed to by value in cd86
void create_all_enemies_table(void) {
  // based on cc56
  u8 i;
  priority_offset = 0x3c;
  // create the priority table based on "strongest wizard"
  create_table_wizards();
  priority_offset = 0x20;
  LUT_index = 14;
  u8 table_size = 9;
  u8 tmp;
  // now create the rest of the table, for the enemy creatures
  for (i = 0; i < 0x9e; i++) {
    // call c67a
    tmp = contains_enemy(i);
    
    if (tmp != 0) {
      table_size++;
      prio_table[LUT_index++] = tmp;
      prio_table[LUT_index++] = i;
    }
  }
  order_table(table_size,prio_table);
  LUT_index = 1;
  
  priority_offset = 0;
}



// used for lightning and magic bolt
// code from 9d8a
void ai_cast_magic_missile(void) {
  
  create_all_enemies_table();
  
  target_index = prio_table[LUT_index];
  while (target_index != 0xFF) {
    target_index = prio_table[LUT_index];
    if (arena[0][target_index] == SPELL_SHADOW_WOOD || arena[0][target_index] >= WIZARD_INDEX
    || arena[0][target_index] < SPELL_MAGIC_FIRE ) {
      // see if in range...
      if (is_spell_in_range(wizard_index, target_index, CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange)) {
        if (!los_blocked(target_index, 0)) {
          // spell is good!
          target_square_found = 1;
          temp_cast_amount = 0;
          // cas tthe actual spell...
          print_name_spell();
          delay(80);
          do_magic_missile();
          return;
        } 
      }
    }
    // got to here? then the spell is not valid... look at the next index
    LUT_index += 2;
  }
  
  target_square_found = 0;
  temp_cast_amount = 0;
}

// code based on 853b
void ai_cast_subversion(void) {
  // ths usual, store start pos, create table of all enemies, retrieve start pos and order table
  u8 current_index = start_index;
  create_table_enemies();
  start_index = current_index;
  order_table(target_count, prio_table);
  
  // get the best target square
  LUT_index = 1;
  u8 creature;
  u16 tmp_dist;
  u8 tmp_cast_range = CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange;
  temp_cast_amount = 1;
  while (prio_table[LUT_index] != 0xFF) {
    creature = arena[0][prio_table[LUT_index]];
    if (arena[4][prio_table[LUT_index]] == 0 && creature < SPELL_GOOEY_BLOB && creature != 0) {
      // a valid creature for subversion...
      // compare the start and end locations to see if in spell range
      get_distance(start_index, prio_table[LUT_index], &tmp_dist);
      if (tmp_cast_range >= tmp_dist && !los_blocked(prio_table[LUT_index], 0) && 
      (arena[3][prio_table[LUT_index]] & 0x20) == 0 ) {
        // in rangem, have los and is not an illusion (because some one has cast disbeilve on it)
        target_index = prio_table[LUT_index];
        target_square_found = 1;
        return;
      }
      
    }
    LUT_index += 2;
  }
  
  if (prio_table[LUT_index] == 0xFF) {
    // no decent square
    target_square_found = 0;
    temp_cast_amount =0;
    return;
  }
  
  target_square_found = 0;
  temp_cast_amount =0;
}

// works a treat this ....
void ai_cast_wall(void) {
  // 9b85
  // CALL c7bc
  temp_cast_amount = 4;
  u8 current_index = start_index;
  create_table_enemies();
  start_index = current_index;
  order_table(target_count, prio_table);
  
  LUT_index = 1;
  
  while (prio_table[LUT_index] != 0xFF) {
    if (arena[0][prio_table[LUT_index]] >= SPELL_SPECTRE )
      break;
    if (arena[0][prio_table[LUT_index]] < SPELL_PEGASUS )
      break;
    LUT_index += 2;
  }
  
  if (prio_table[LUT_index] == 0xFF) {
    // no decent square
    target_square_found = 0;
    temp_cast_amount =0;
    return;
  }
  
  // got to here? OK, a "good" square was found for wall.
  // calculate spell success and update world chaos ...
  set_spell_success();
  print_name_spell();
  delay(80);
  s16 in_range = create_range_table(prio_table[LUT_index], 0x9);
  clear_message();
  
  LUT_index = 2*in_range+1;
  u8 y,x;
  u8 tmp_square_found = 0;
  target_square_found = 0;
  
  // the global target_square_found is used as a temporary flag here
  // tmp_square_found indicates if we find any castable-to squares for this spell at all
  while (temp_cast_amount) {
    
    target_square_found = 0;
    get_furthest_inrange();
    // if a suitable square was found, target_sq_found will be 1
    if (target_square_found == 0) {
      // we have run out of chackable squares!
      break;
    }
    
    // check that this is a valid square for walls...
    get_yx(target_index, &y, &x);
    if (x >= 0x10) {
      continue;
    }
    if (arena[0][target_index] != 0){
      continue;
    }
    if (is_wall_adjacent(target_index)){
      continue;
    }
    if (los_blocked(target_index, 0)){
      continue;
    }
    
    // we have found a suitable square...
    tmp_square_found = 1;
    do_wall_cast();
    
  }
  temp_cast_amount = 0;
  target_square_found = tmp_square_found;
}


// get the best square for range attack
u8 get_best_rangeattack(void) {
  // cccb
  u8 creature;
  while (prio_table[LUT_index] != 0xFF) {
    
    // check the value at this index...
    creature = arena[0][prio_table[LUT_index]];
    if (creature >= WIZARD_INDEX || (creature == SPELL_MAGIC_WOOD && arena[4][prio_table[LUT_index]] != 0) 
    || creature < SPELL_MAGIC_FIRE || creature == SPELL_SHADOW_WOOD ) {
      // wizard, or wizard in wood, shadow wood or an actual creature
      // anything >= magic fire will be ignored
      
      // ccfd
      // check if we are trying to attack an undead creature...
      // original chaos didn't check the arena[3] value for raise deaded creatures
      // wizards can always attack undead with their magic bow range attacks
      u8 attacker_undead = IS_UNDEAD(arena[3][start_index]) || (selected_creature >= WIZARD_INDEX);
      u8 defender_undead = (creature >= SPELL_VAMPIRE  &&
                            creature <= SPELL_ZOMBIE ) || 
                            IS_UNDEAD(arena[3][prio_table[LUT_index]]);
      if ( !defender_undead || attacker_undead)
      {
        target_index = prio_table[LUT_index];
        LUT_index++;
        LUT_index++;
        return 0x53;
      } // else, trying to range attack undead with a living creature, so get next value... 
        
    } 
    LUT_index++;
    LUT_index++;
  }
   
  return 0x4b;
  
}

// from 9ef9
void ai_cast_justice(void) {
  // print name and spell
  target_square_found = 1;
  print_name_spell();
  delay(80);
  set_spell_success();
  if (!temp_success_flag) {
    temp_cast_amount =0;
    print_success_status();
    return;
  }
  u8 tmp_start = wizard_index;
  u8 creature;
  while (temp_cast_amount != 0) {
    start_index = tmp_start;
    target_index = start_index;
    create_all_enemies_table();
    LUT_index = 1;
    
    // get the best index for each cast...
    while (prio_table[LUT_index] != 0xFF) {
      // check the creature at this index...
      creature = arena[0][prio_table[LUT_index]];
      if (creature >= WIZARD_INDEX || (creature < SPELL_GOOEY_BLOB && creature != 0) ) {
        // wizard or proper creature
        target_index = prio_table[LUT_index];
        LUT_index++;
        LUT_index++;
        break;
      } 
      LUT_index++;
      LUT_index++;
    }
    if (prio_table[LUT_index] == 0xFF) {
      return;
    }
    delay(60);
    do_justice_cast();
    temp_cast_amount--;
  }
  
}

// taken from 8639
void ai_cast_raisedead(void) {
  int i;
  u8 id;
  target_square_found = 0;
  // make the byte pair table...
  reset_priority_table();
  target_count = 0;
  LUT_index = 0;
  for (i = 0; i < 0x9f; i++) {
    id = arena[0][i];
    if (arena[2][i] == 4) {
      // is dead, store the priority for this creature
      prio_table[LUT_index++] = attack_pref[id];
      prio_table[LUT_index++] = i;
      target_count++;
    }
  }
  order_table(target_count, prio_table);
  
  // get the best value and try and cast it...
  // get the best index for each cast...
  LUT_index = 1;
  u8 creature;
  while (temp_cast_amount != 0) {
    while (prio_table[LUT_index] != 0xFF) {
      // check the creature at this index...
      creature = arena[0][prio_table[LUT_index]];
      if (creature >= WIZARD_INDEX || (creature < SPELL_GOOEY_BLOB && creature != 0) ) {
        // wizard or proper creature
        target_index = prio_table[LUT_index];
        LUT_index++;
        LUT_index++;
        break;
      } 
      LUT_index++;
      LUT_index++;
    }
    if (prio_table[LUT_index] == 0xFF) {
      return;
    }
    // ok, got a square to cast to... is it in range, los, etc?
    if (arena[0][target_index] == 0 || arena[2][target_index] != 4) 
      continue;
    if (!is_spell_in_range(wizard_index, target_index, CHAOS_SPELLS.pSpellDataTable[current_spell]->castRange)) 
      continue;
    if (los_blocked(target_index, 0))
      continue;
    
    // got to here? good, print name and spell and do cast
    print_name_spell();
    delay(80);
    do_raisedead_cast();
    temp_cast_amount = 0;
  }
  target_square_found = 1;
}

// based on code at 9a95
// should set target square flag and the target index
// when returns, the disbelieve spell is cast based on these
void ai_cast_disbelieve(void) {
  u8 current_index = start_index;
  create_all_enemies_table();
  start_index = current_index;
  while (prio_table[LUT_index] != 0xFF) {
    target_index = prio_table[LUT_index];
    if (arena[0][target_index] < SPELL_GOOEY_BLOB && !(arena[3][target_index] & 0x20)  ) {
      // is a creature and has not had disbeleive cast on it yet...
      target_square_found = 1;
      print_name_spell();
      delay(80);
      return;
    }
    
    LUT_index++;
    LUT_index++;
  }
  // got to here? must have run out of decent targets
  target_index = current_index;
  target_square_found = 0;
  
}

// code at 86f9
void ai_cast_turmoil(void) {
  
  // CALL c7bc
  u8 current_index = start_index;
  create_table_enemies();
  start_index = current_index;
  order_table(target_count, prio_table);
  
  // this first part depends not on the index values, like most other spells.
  // it instead depends on the priority value... the thinking behind this is: 
  // if there aren't enough enemy creatures really close, it isn't worth casting turmoil
  LUT_index = 0;
  u8 total_danger = 0;
  while (prio_table[LUT_index] != 0) {
    
    total_danger += prio_table[LUT_index];
    LUT_index += 2;
    if (total_danger >= 0x1E) {
      break;
    }
  }
  
  if (prio_table[LUT_index] == 0) {
    // no decent square
    target_square_found = 0;
    temp_cast_amount =0;
    return;
  }
  
  // the spell is worth casting
  target_square_found = 1;
  temp_cast_amount = 1;
  do_turmoil_cast();
}

// check if the cpu could dismount (traditionally, the cpu never dismounts)
// return true to dismount
// return false to stay mounted
int cpu_dismount(void)
{
  u8 surround_index = 0;
  u8 lookat_i;
  // type records type of thing that attracts us
  int type = 0xff;
  int i;
  // which type of mount are we on?
  if (arena[0][target_index] >= SPELL_PEGASUS
      && arena[0][target_index] <= SPELL_MANTICORE )
  {
    // too complex to work out...
    return 0;
  } 

  for (i = 0; i < 8; i++) {
    lookat_i = apply_position_modifier(target_index, surround_index);
    surround_index++;
    if (lookat_i == 0) {
      // out of bounds
      continue;
    }
    lookat_i--;

    // rules for leaving our mount are consistent with the
    // movement patterns when not mounted, i.e. prefer
    // to move to a castle or tree before getting on a horse.
    
    if (arena[0][lookat_i] >= SPELL_MAGIC_CASTLE 
	&& arena[0][lookat_i] <= SPELL_DARK_CITADEL
//	&& (players[current_player].timid == 0)  // is this right?
	) 
    {
      type = 0;
    } else
    if (arena[0][lookat_i] == SPELL_MAGIC_WOOD) 
    {
      if (type > 1) {  
	type = 1;
      }
    } else
    if (arena[0][lookat_i] >= SPELL_PEGASUS
      && arena[0][lookat_i] <= SPELL_MANTICORE 
      && (arena[3][lookat_i] & 7) == current_player 
      && arena[4][lookat_i] == 0)
    {
      /*
       * next to us stands a winged mount, we are on a walking mount so swap!
       * Really, we should change to any better mount but that is quite a
       * complex operation - the cpu move table would need resetting and extra
       * checks put in all over the place. Not really worth it.
       */ 
      if (type > 2) {  
	type = 2;
      }
    }
  }
  return type  != 0xff;

}
  
