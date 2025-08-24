// creature.c
#include "platform.h"
#ifdef __GBA__
#include "sound.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#endif
#include "sound_data.h"
#include "spelldata.h"
#include "creature.h"
#include "arena.h"
#include "chaos.h"
#include "wizards.h"
#include "casting.h"
#include "gfx.h"
#include "options.h"

void do_spread(unsigned char start, unsigned char target);
void uncover_square(unsigned char start, unsigned char target);

extern u8 game_frames;

#ifdef USE_8BIT_RAND
u32 g_rseed;
#else
u32 m_nRandom = 0x600d5eed;
#endif

void ChurnRand() {
#ifdef USE_8BIT_RAND
  unsigned char B = ((g_rseed>>7)&0xff)*2;
  unsigned char A = B + (g_rseed&0xff);
  g_rseed = (A<<8) | B;
#else
  m_nRandom *= 663608941; // churn the random number
#endif
}

  
int GetRand(int i) {
#ifdef USE_8BIT_RAND
  if (!g_rseed) {
    g_rseed = (game_frames*0x5d)&0xffff;
    g_rseed = (!g_rseed)?0x34d7:g_rseed;
  }
  
  unsigned char B = g_rseed>>7;
  unsigned char A = B + g_rseed;
  g_rseed = (A<<8) | B;
  int returnVal = i * A;
  return returnVal>>8;
#else
  unsigned long long nResult = i;
  nResult = nResult * m_nRandom;
  ChurnRand();
  return (u32)(nResult >> 32);
#endif
}

int GetSeed(void) {
#ifdef USE_8BIT_RAND
  return g_rseed;
#else
  return m_nRandom;
#endif
}
  
void setSeed(unsigned int newSeed) {
#ifdef USE_8BIT_RAND
  g_rseed = newSeed;
#else
  m_nRandom = newSeed; // churn the random number
#endif
}

void creature_spell_succeeds(unsigned char target) {
  
  if (!(current_spell == SPELL_GOOEY_BLOB || current_spell == SPELL_MAGIC_FIRE)) {
    arena[5][target] = arena[0][target];  // whatever is in this square, place in arena 5...
  }
  arena[0][target] = current_spell;
  arena[1][target] = 1;
  arena[2][target] = 0;
  arena[3][target] = current_player;
  if (temp_illusion_flag) {
    arena[3][target] |= 0x10; // set bit 4
  }
  
  u8 y,x;
  get_yx(target, &y, &x);
    
}


void clear_arena(void) {
  
  u8 x, y;
  for (x = 0; x < 15; x++){
    for (y = 0; y < 10; y++){
      clear_square(x,y);
    }
  }
  
}

// this should be called every now and again...
void draw_all_creatures(void) {
  u16 i;
  u8 x, y;
  for (i = 0; i < 0x9f; i++) {
    get_yx(i, &y, &x);
    if (arena[0][i] >= 2) {
      
      if (arena[0][i] < WIZARD_INDEX) {
        draw_creature(x-1, y-1, 
          arena[0][i],arena[2][i]);
      } else {
        draw_wizard(x-1, y-1,
          players[arena[0][i] - WIZARD_INDEX].image, arena[2][i], arena[0][i] - WIZARD_INDEX);
      }
    } else {
      if (x > 0 && x < 0x10 && y > 0 && y < 0xB)
        clear_square(x-1,y-1);
    }
    
  }
#ifdef __WINDOWS__
  draw_win_cursor();
#endif
}

void add_new_wizard(unsigned char index, unsigned char id) {
  
  u8 y,x;
  get_yx(index, &y, &x);
  
  // the indices are 1 based, but we always draw to a 0 based map
  draw_wizard(x-1, y-1,
      players[id].image, 0, id);
  
  arena[0][index] = id+WIZARD_INDEX;
  
}

// called at the end of the casting round
// based on code at 9f50.. 
void spread_fire_blob(void) {
  unsigned char i;
  unsigned char current_index = 0;
  target_index = 0;
  unset_moved_flags();
  unsigned char this_owner;
  unsigned char target_owner;
  unsigned char r;
  unsigned char creature;
  unsigned char target_creature;
  u16 dist;
  for (i = 0; i < 0x9f; i++) {
    target_index = current_index;
    
    if (HAS_MOVED(arena[3][current_index])) {
      current_index++;
      continue;
    }
    creature = arena[0][target_index];
    if (creature < SPELL_GOOEY_BLOB) {
      current_index++;
      continue;
    }
    if (creature >= SPELL_MAGIC_WOOD) {
      current_index++;
      continue;
    }
    this_owner = arena[3][target_index] & 0x7;
    r = GetRand(10);
    
    if (r < 0x9) {
      if (creature != SPELL_MAGIC_FIRE) {
        r = GetRand(10);
        if (r > 0x8) {
          // jump to a0c6...
          // do uncover creature routine
          uncover_square(current_index, target_index);
          current_index++;
          continue;
        }
      }
      r = GetRand(10);
      while (r >= 0x8) {
        r = GetRand(10);
      }
      target_index = apply_position_modifier(target_index, r);
      r++;
      if (target_index == 0) {
        current_index++;
        continue;
      } else 
        target_index --;
      
      // do in range check... 
      get_distance(current_index, target_index, &dist);
      if (dist > 4) {
        current_index++;
        continue;
      }
      
      target_creature = arena[0][target_index];
      if (target_creature != 0) {
        if (target_creature < WIZARD_INDEX) {
          // is NOT a wizard...
          if (target_creature >= SPELL_MAGIC_CASTLE || target_creature == SPELL_MAGIC_WOOD ) {
            // a magic wood or castle, etc
            // jump to a0c6...
            // do uncover creature routine
            uncover_square(current_index, target_index);
            current_index++;
            continue;
          }
          
          if (arena[2][target_index] != 4) {
            // if not dead
            target_owner = arena[3][target_index] & 0x7;
            if (target_owner == this_owner) {
              current_index++;
              continue;  
            }
            if (target_creature <= SPELL_MANTICORE && target_creature >= SPELL_HORSE
                && arena[4][target_index] >= WIZARD_INDEX) {
              // if the creature is a mount and has a wizard on it
              // a011...
              if ((arena[4][target_index] - WIZARD_INDEX) == this_owner) { 
                // do nothing - own blob can't kill
                current_index++;
                continue;
              }
              // clear arena 4 (wizard) and place wizard in arena 0...
              arena[0][target_index] = arena[4][target_index];
              arena[4][target_index] = 0x00;
              
              // a033... kill wizard
              kill_wizard();
              // jump to a09b 
              do_spread(current_index, target_index);
              current_index++;
              continue;
              
            } 
            // a03d
            if (creature == SPELL_MAGIC_FIRE) {
              // jump a075 - for fire
              // get defence of target creature
              unsigned char defence = CHAOS_SPELLS.pSpellDataTable[target_creature]->defence + GetRand(10);
              unsigned char attack  = GetRand(10) + 5;
              
              if (attack > defence ) {
                // jump a0c6
                uncover_square(current_index, target_index);
                current_index++;
                continue;
              } 
              // jump a09b...
            } else { //if (creature == SPELL_GOOEY_BLOB) {
              // gooey blob
              if (target_creature >= SPELL_MAGIC_FIRE) {
                // jump a0c6 - uncover creature routine
                uncover_square(current_index, target_index);
                current_index++;
                continue;
              } 
              if (arena[2][target_index] != 4) {
                // creature alive.. set arena 4 with the current creature and cover it
                arena[4][target_index] = arena[0][target_index];
                if (Options[OPT_OLD_BUGS]) {
                  // old bugs on...
                  // a066
                  // I think the plan here was to set the creature's owner, the first 3 bits
                  arena[5][target_index] = (arena[3][target_index]&0x7); // bug here
                }
                else {
                  // old bug removed here...
                  // what about undead and illusionary flags?
                  arena[5][target_index] = arena[3][target_index];
                }
                
              } // else jump to a09b - target dead
            }
            
          } // else target is dead
          // jump to a09b
        } else {
          // "jump a011" - target creature is a wizard
          
          if ( (target_creature - WIZARD_INDEX) == this_owner) { 
            // do nothing - own blob can't kill
            current_index++;
            continue;
          }
          
          // a033 - kill wizard
          kill_wizard();
          // jmp a09b
        }
      } // else  target creature == 0
      // jump a09b
      do_spread(current_index, target_index);
    } else {
      // jump to a0c6...
      // do uncover creature routine
      uncover_square(current_index, target_index);
    }
    current_index++;
  }
  
  
}


void uncover_square(unsigned char start, unsigned char target) {
  unsigned char r = GetRand(10);
  if (r > 2) {
    return;
  }
  unsigned char target_creature = arena[0][target];
  
  if (target_creature != SPELL_MAGIC_FIRE) {
    r = GetRand(10);
    if (r > 3) {
      return;
    }
  }
  // remove the blob and show the creature underneath...
  if (arena[4][start] == 0) {
    
    arena[0][start] = 0;
    arena[5][start] = 0;
    
  } else {
    arena[0][start] = arena[4][start];
    arena[4][start] = 0;
    arena[3][start] = arena[5][start];
    arena[5][start] = 0;
  }
  delay(24);
}

// code from a09b
void do_spread(unsigned char start, unsigned char target) {
  unsigned char start_creature = arena[0][start];
  
  arena[0][target] = start_creature;
  arena[2][target] = 0; // frame
  arena[3][target] = arena[3][start]; // set owner
  
  arena[3][target] |= 0x80; // set spread flag
  arena[3][start]  |= 0x80; // set spread flag
  
  move_screen_to(target);
  if (start_creature == SPELL_GOOEY_BLOB) {
    PlaySoundFX(SND_GOOEY);
  } else if (start_creature == SPELL_MAGIC_FIRE) {
    PlaySoundFX(SND_FIRE);
  }
  
  delay(24);

  // at the end of everything, randomly uncover the "spawn" square
  uncover_square(start,start);
}


