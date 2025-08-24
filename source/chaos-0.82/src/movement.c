// movement.c
#include "platform.h"

#ifdef __GBA__
#include "sound.h"
#include "screenmode.h"
#include "keypad.h"
#elif defined(__WINDOWS__)
#include "winkeys.h"
#include "winsound.h"
#include "remote.h"
#include "network.h"
#include "options.h"
#endif
#include "sound_data.h"
#include "string.h"
#include "text16.h"
#include "movement.h"
#include "chaos.h"
#include "wizards.h"
#include "computer.h"
#include "gfx.h"
#include "arena.h"
#include "spelldata.h"
#include "chaos_sprites.h"
#include "creature.h"
#include "players.h"
#include "gamemenu.h"
#include "input.h"
#include "casting.h"
#include "splash.h"
#include "options.h"


unsigned char selected_creature;
unsigned char tmp_range_attack;
unsigned char tmp_movement_allowance;
unsigned char tmp_is_flying;
unsigned char tmp_engaged_flag;
unsigned char tmp_creature_id;
unsigned char tmp_range_attack_val;
unsigned char tmp_wizard_movement_allowance;
unsigned char attacker;

void get_owner_at(u8 x, u8 y, u8 *surround_creature);
void set_engaged_to_enemy(void);
void check_engaged(u8 index);
void select_creature(void);
void do_successful_move(u8 distance_moved);

void move_walking_creature(void);
void move_flying_creature(void);
void move_creature(u8 distance_moved);

void end_movement(void);
void make_attack(void);
void do_range_attack(void);
void end_game_option(void);

void remove_creature(u8 creature);
void do_attack_anim(u8 index);
void end_game(void);

void start_movement_round(void) {
  // start the movement round
  // code is very roughly based on speccy chaos...
  // code begins at ac36
  selected_creature = 0;
  attacker = 0;
  tmp_range_attack = 0;
  tmp_engaged_flag = 0;
  tmp_creature_id = 0;
  tmp_range_attack_val = 0;
  tmp_movement_allowance = 0;
  tmp_is_flying = 0;
  tmp_wizard_movement_allowance = 0;
  if (current_player < playercount) {
    if (IS_WIZARD_DEAD(players[current_player].modifier_flag)) {
      // player is dead...
      current_player++;
      start_movement_round();
    } else {
      // player is alive... 
      // print "player'S TURN" string...
      current_screen = SCR_MOVEMENT;
      hilite_item = 0;
      set_current_player_index();
      remove_cursor();
#ifdef __WINDOWS__
      remove_win_cursor();
      wait_vsync_int();
#endif
      u8 x,y;
      get_yx(wizard_index,&y,&x);
      move_cursor_to(x-1,y-1);
      set_border_col(current_player);
      
      remove_cursor();
      char str[30];
      clear_message();
      highlight_players_stuff(current_player);
      
      if (IS_CPU(current_player)) {
        PlaySoundFX(SND_CPUSTART);
      }
      strcpy(str, players[current_player].name);
      strcat(str, "'S TURN");
      
//      sprintf(str, "%s'S TURN",players[current_player].name);
      print_text16(str,MESSAGE_X,MESSAGE_Y, 10);
      set_text16_colour(10, RGB16(30,31,0)); // yellow
      for (x = 0; x < 24; x++) {
        wait_vsync_int();
      }
      
      // highlight the current player's creatures
      if (IS_CPU(current_player)) {
        // cpu movement round
        // jump 96f3
        delay(10);
        do_ai_movement();
        current_player++;
        start_movement_round();
      } else {
#ifdef __WINDOWS__
	if (ONLINE_MODE && IS_REMOTE(current_player)) {
	  do_remote_movement();
	  //current_player++;
	  //start_movement_round();
	} else {
#endif
        redraw_cursor();
       // human movement selection...
#ifdef __WINDOWS__
	}
#endif
      }
      
    }
  } else {
    
    // here, check that there are enough wizards left to carry on
    // and that we haven't been playing for players*2+15 rounds yet
    
    if (dead_wizards == (playercount-1)) {
      // uh oh -  no wizards left, do winner screen
      win_contest();
      return;
    }
    if (Options[OPT_ROUND_LIMIT] == DEFAULT_ROUNDS) {
      if (round_count >= playercount*2+15) {
        // we have been playing for ages... no one is gonna win
        drawn_contest();
        return;
      }
    } else if (Options[OPT_ROUND_LIMIT] != 0) {
      if (round_count >= Options[OPT_ROUND_LIMIT]) {
        // we have been playing for more than the optional round limit... draw the game
        drawn_contest();
        return;
      }
    } // else if round limit is 0, then the game can go on for ever!
    
    
    // return to spell selection/cast round
    current_player = 0;
    
#ifdef __WINDOWS__
    if (ONLINE_MODE) {
      if (IS_REMOTE(0)) 
        current_player = get_next_local(0);
      else
        current_player = 0;
#ifdef DEBUG_NET
      printf("current player %d\n", current_player);
#endif

      if (current_player == 9) {
        // there is no local player!
        // this should wait for all spells?
        continue_game();

      } else {
        // we have our local player...
        fade_down();
        show_game_menu();
        fade_up();
      }
    } else {
#endif
    // check if we need to show game menu...
    if (IS_CPU(0)|| IS_WIZARD_DEAD(players[0].modifier_flag)) 
      current_player = get_next_human(0);
    else
      current_player = 0;
#ifdef __WINDOWS__
    }
#endif
    remove_cursor();
    delay(64);
    if (current_player == 9) {
      // there is no human player!
      end_game_option();
      if (current_player == 9) {
        
        drawn_contest();
        return;
      } else {
        continue_game();
      }
      
    } else {
#ifdef __WINDOWS__
	if (ONLINE_MODE) {
          if(is_hosting()) 
  	    reset_spellwaiting();
          update_spellset();
        }
#endif
      fade_down();
      show_game_menu();
      fade_up();
    }  
  }
  
}

int g_highlight_creations = 9;

void movement_l(void) {

#ifdef DEBUG
  if (arena[0][target_index] >= WIZARD_INDEX) {
    players[arena[0][target_index] - WIZARD_INDEX].plyr_type = 
      IS_CPU((arena[0][target_index] - WIZARD_INDEX))?PLYR_HUMAN:PLYR_CPU;
    clear_message();
    if (IS_CPU((arena[0][target_index] - WIZARD_INDEX)))
      print_text16("CPU",MESSAGE_X,MESSAGE_Y, 12);  
    else
      print_text16("HUMAN",MESSAGE_X,MESSAGE_Y, 12);  

  } else if (arena[0][target_index] >= SPELL_KING_COBRA && arena[0][target_index] < SPELL_GOOEY_BLOB) {
    u8 defender = arena[0][target_index];
    if (arena[4][target_index] == 0) {
      // nothing in arena 4...
      if (defender >= SPELL_VAMPIRE || 
        IS_UNDEAD(arena[3][target_index]) ||
        IS_ILLUSION(arena[3][target_index]) ) {
        // shouldn't leave anything underneath...
      } else {
        defender = 0xFF;
      }
      // jump b310
      if (defender == 0xFF) {
        // leaves corpse
        arena[1][target_index] = 1;
        arena[2][target_index] = 4;
        delay(8);
        // do sound fx
        
      } else {
        // creature doesn't leave a corpse
        // check arena 5 val
        if (arena[5][target_index] == 0) {
          arena[0][target_index] = 0;
          arena[1][target_index] = 1;
        } else {
          arena[0][target_index] = arena[5][target_index];
          arena[1][target_index] = 1;
          arena[2][target_index] = 4;
          arena[5][target_index] = 0;
        }
      }
      
    } else {
      // there was something in the arena 4... b2e0
      u8 arena4 = arena[4][target_index];
      arena[4][target_index] = 0;
      if (defender == SPELL_GOOEY_BLOB) {
        arena[3][target_index] = arena[5][target_index];
      }
      else if (!Options[OPT_OLD_BUGS]) {
        // the famous "undead wizard" bug is caused by not updating the arena[3] flag properly
        if (arena4 >= WIZARD_INDEX) {
          arena[3][target_index] = arena4-WIZARD_INDEX;
        }
      }
      arena[0][target_index] = arena4;
      arena[5][target_index] = 0;
    }
    
  }
  wait_for_letgo();
#else
  // get the owner of the creature
  // make sure dead ones don't count...
  if (arena[0][target_index] != 0 && arena[2][target_index] != 4) {
    remove_cursor();
    g_highlight_creations = 9;
    u8 playerid = 9;
    if (arena[0][target_index] >= WIZARD_INDEX) {
      playerid = arena[0][target_index] - WIZARD_INDEX;
    }
    else if (arena[2][target_index] != 4) {
      playerid = GET_OWNER(arena[3][target_index]);
    }
    if (playerid == 9)
      return;
    
    highlight_players_stuff(playerid);
    g_highlight_creations = playerid;
    char str[30];
    strcpy(str, players[playerid].name);
    strcat(str, "'S CREATIONS");
    
    clear_message();
    print_text16(str,MESSAGE_X,MESSAGE_Y, 10);
    set_text16_colour(10, RGB16(30,31,0)); // yellow
    
    wait_for_letgo();
    g_highlight_creations = 9;
    clear_message();
    redraw_cursor();
  }
#endif
  

}

// When select is pressed, move the cursor to the next moveable creature.
void movement_select(void) {
  if (selected_creature == 0) {
    
    u8 i,j;
    u8 new_index = 0xFF;
    u8 lower_bounds, upper_bounds;
    for (j = 0; j < 2 && new_index == 0xFF; j++) {
      if (j == 0) {
        lower_bounds = target_index+1;
        upper_bounds = 0x9F;
      } else {
        lower_bounds = 0;
        upper_bounds = target_index+1;
      }
      // search the rest of the arena for creatures...
      for (i = lower_bounds; i < upper_bounds; i++) {
        // if something here and it is not dead and it is...
        // (a creature, or a wizard, or a wizard inside a magic wood or castle) and
        // it hasn't moved yet AND it is our creature or wizard...
        if ( arena[0][i] && arena[2][i] != 4 &&
             (arena[0][i] < SPELL_GOOEY_BLOB || arena[0][i] >= WIZARD_INDEX
              || arena[0][i] == SPELL_SHADOW_WOOD
              || ( arena[0][i] >= SPELL_MAGIC_WOOD &&  arena[4][i] >= WIZARD_INDEX)
              ) &&
              !HAS_MOVED(arena[3][i])              
        ) {
          // get the owner - low 3 bits of arena 3 for creatures, 
          // (creatureid - WIZARD_INDEX) for wizards
          u8 owner = 0xff;
          if (arena[0][i] >= WIZARD_INDEX) {
            owner = arena[0][i] - WIZARD_INDEX;
          } else {
            // a creature - but is it a mount or a wood/castle?
            if (arena[0][i] < SPELL_MAGIC_WOOD) // a mount...
              owner = arena[3][i] & 0x7;
            else // a castle/wood - could be anyones, so base the owner on the wizard inside
              owner = arena[4][i] - WIZARD_INDEX;
          }
          // is it ours?
          if (owner == current_player) {
            new_index = i;
            break;
          }
          
        }
      }
      
    }
    if (new_index != 0xFF) {
      u8 x,y;
      get_yx(new_index,&y,&x);
      move_cursor_to(x-1,y-1);
    } else {
      clear_message();
      print_text16("PRESS START TO END TURN",MESSAGE_X,MESSAGE_Y, 12);  
      set_text16_colour(12, RGB16(31,30,0)); // yellow
    }
  }
}

void movement_start(void) {  
#ifdef __WINDOWS__
  // local, online players need to tell the world what they are doing
  if (ONLINE_MODE && IS_LOCAL(current_player) )
    send_movement_start();
#endif
  if (selected_creature == 0) {
    current_player++;
    start_movement_round();
  }
}

void movement_a(void) {
  // A pressed in movement round
  
  // check in the actual arena...
  u8 x,y;
  get_yx(target_index, &y, &x);
  if (x >= 0x10) {
    return;
  }
#ifdef __WINDOWS__
  // local, online players need to tell the world what they are doing
  if (ONLINE_MODE && IS_LOCAL(current_player) )
    send_movement_a();
#endif
  if (selected_creature == 0) {
    // select a creature
    select_creature();
    if (selected_creature != 0) {
      
      if (IS_CPU(current_player))
        move_screen_to(target_index);
      
      PlaySoundFX(SND_CHOSEN);
      if (tmp_is_flying)
        draw_cursor(CURSOR_FLY_GFX);
      else if (tmp_engaged_flag == 1) {
        draw_cursor(CURSOR_ENGAGED_GFX);
      } else {
        draw_cursor(CURSOR_GROUND_GFX);
      }
    }
    
  } else if ( (tmp_range_attack == 0) && selected_creature !=0 ) {
    // try moving the selected creature
    disable_interrupts();
    if (tmp_is_flying)
      move_flying_creature();
    else
      move_walking_creature();
    enable_interrupts();
    
    if (tmp_engaged_flag == 1) {
      draw_cursor(CURSOR_ENGAGED_GFX);
    }
    
    
  } else if (tmp_range_attack != 0) {
    disable_interrupts();
    do_range_attack();
    enable_interrupts();
  }
  if (selected_creature == 0) {
    draw_cursor(CURSOR_NORMAL_GFX);
  }
  
}

void movement_b(void) {
#ifdef __WINDOWS__
  // local, online players need to tell the world what they are doing
  if (ONLINE_MODE && IS_LOCAL(current_player) )
    send_movement_b();
#endif
  if (selected_creature != 0) {
    if (tmp_range_attack == 0) {
      end_movement();
    } else {
      set_border_col(current_player);
      tmp_range_attack = 0;
      selected_creature = 0;
    }
    
    if (selected_creature == 0) {
      draw_cursor(CURSOR_NORMAL_GFX);
    }  
    
  }
  
}

  

// taken from aced 
void select_creature(void) {
  // see where the cursor is in the arena..
  // target_index contains the current cursor index.
  start_index = target_index;
  selected_creature = arena[0][start_index];
  tmp_range_attack = 0;
  tmp_engaged_flag = 0;
  tmp_creature_id = 0;
  tmp_range_attack_val = 0;
  tmp_movement_allowance = 0;
  flying_target_found = 0; // CPU player flag
  tmp_is_flying = 0;
  tmp_wizard_movement_allowance = 0;

  // is there a creature here?
  if (selected_creature == 0) 
    return;
  
  // check if we have moved already
  if (HAS_MOVED(arena[3][start_index])) {
    selected_creature = 0;
    return;
  }
  
  
  if (selected_creature >= WIZARD_INDEX) {
    // wizard - check it is ours
    u8 sel_plyr = selected_creature - WIZARD_INDEX;
    if (sel_plyr != current_player) {
      selected_creature = 0;
      return;
    }
    
    tmp_movement_allowance = 0x2; // default
    if ( HAS_MAGICWINGS(players[sel_plyr].modifier_flag) ) {
      tmp_movement_allowance = 0xD;
      tmp_is_flying = 1;
    } else if (HAS_SHADOWFORM(players[sel_plyr].modifier_flag)) {
      tmp_movement_allowance = 0x6;
    }
    
  } else {
    // a creature is selected
    // set the movement value
    tmp_movement_allowance = CHAOS_SPELLS.pSpellDataTable[selected_creature]->movement*2;
    if (selected_creature >= SPELL_PEGASUS && selected_creature < SPELL_SPECTRE) {
      // between pegasus and vampire
      tmp_is_flying = 1;
      tmp_movement_allowance++;
    }
    // check if there is something inside
    u8 inside_creature = arena[4][start_index];
    u8 yes_pressed = 0;
    if (inside_creature >= WIZARD_INDEX) {
      inside_creature -= WIZARD_INDEX;
      // it is a wizard id 
      if (inside_creature != current_player) {
        selected_creature = 0;
        return;
      }
      inside_creature += WIZARD_INDEX;
      // check if the covering creature is a wood or up
      if (selected_creature < SPELL_MAGIC_WOOD) {
        // is not a wood, so carry on
        // display "DISMOUNT WIZARD? (Y OR N)" message
        // loop until Y or N pressed...
        if (IS_CPU(current_player)) {
	  // room for improvement here - e.g. what if stood next to a wood?

	  // traditional:
	  /*
	  if (Options[OPT_OLD_BUGS]) {
	    yes_pressed = 0;
	  } else 
	  {*/
	    // "improved"
	    yes_pressed = cpu_dismount();
	  //}
	}
        else {
          clear_message();
#ifdef __WINDOWS__
	  if (ONLINE_MODE && IS_REMOTE(current_player)) {
	    // online mode - wait for the remote player to tell us what
	    // they want to do... 
	    yes_pressed = remote_dismount();
#ifdef DEBUG_NET
	    printf("Remote dismount %d\n", yes_pressed);
#endif
	  }
	  else {
	    // as per usual...
#endif
          print_text16("DISMOUNT WIZARD? A=YES B=NO",MESSAGE_X,MESSAGE_Y, 12);  
          set_text16_colour(12, RGB16(31,30,0)); // yellow
          wait_for_letgo();
          yes_pressed = 0;
          while (yes_pressed == 0) {
            wait_vsync_int();
            UpdateKeys();
            if (KeyPressedNoBounce(KEY_A,50))
              yes_pressed = 2;
            if (KeyPressedNoBounce(KEY_B,50))
              yes_pressed = 1;
          }
          wait_for_letgo();
          yes_pressed--;
#ifdef __WINDOWS__
	  // send everyone the local player's choice, only if online
	  if (ONLINE_MODE && IS_LOCAL(current_player)) {
#ifdef DEBUG_NET
	    printf("sending dismount online %d\n", yes_pressed);
#endif
	    send_dismount(yes_pressed);
	  }

	  } // end if (ONLINE_MODE)
#endif
        }
        
         
      } else {
        // bit of a hack this...
        yes_pressed = 1;
        // jump add2
      }
      if (yes_pressed) {
        // dismount the wizard
        tmp_movement_allowance = 3;
        tmp_wizard_movement_allowance = 1;
        tmp_is_flying = 0;
        // make sure we move the wizard
        selected_creature = inside_creature;
      }
      
    } // else inside creature < WIZARD_INDEX
    if (!yes_pressed) {
      // adb3 
      if (selected_creature != SPELL_SHADOW_WOOD) {
        // not shaodw wood..
        if (selected_creature >= SPELL_GOOEY_BLOB) {
          // blob etc, so do nothing
          selected_creature = 0;
          return;
        }
      }
      // get the owner - low 3 bits of arena 3.
      u8 owner = arena[3][start_index] & 0x7;
      if (owner != current_player) {
        selected_creature = 0;
        return;
      }
      
      if (arena[2][start_index] == 4 ) {
        // dead 
        selected_creature = 0;
        return;
      }
      
    }
  }
  // add2
  // check the engaged to enemy stuff
  check_engaged(start_index);
  if (tmp_engaged_flag == 0) {
    // ae32
    char str[3];
    clear_message();
//    sprintf(str, "%d", tmp_movement_allowance>>1);
    
    int2a(tmp_movement_allowance>>1, str, 10);
    
    print_text16("MOVEMENT RANGE=",MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(0,30,0)); // green
    print_text16(str,MESSAGE_X+15,MESSAGE_Y, 13);  
    set_text16_colour(13, RGB16(30,30,0)); // yellow
    
    if (tmp_is_flying) {
      print_text16("(FLYING)",MESSAGE_X+17,MESSAGE_Y, 14);  
      set_text16_colour(14, RGB16(0,30,31)); // yellow
    }
    
    wait_for_letgo();
  }
  
  
}

void check_engaged(u8 index) {
  // check engaged to enemy for the creature at start_index
  
  // first part based on code at be21
  u8 x,y;
  get_yx(index, &y, &x);
  tmp_engaged_flag = 0;
  
  u8 surround_creature = 0;
  y--;        // x  y-1
  get_owner_at(x,y,&surround_creature);
  x--;        // x-1  y-1
  get_owner_at(x,y,&surround_creature);
  y++;        // x-1  y
  get_owner_at(x,y,&surround_creature);
  y++;        // x-1  y+1
  get_owner_at(x,y,&surround_creature);
  x++;        // x  y+1
  get_owner_at(x,y,&surround_creature);
  x++;        // x+1  y+1
  get_owner_at(x,y,&surround_creature);
  y--;        // x+1  y
  get_owner_at(x,y,&surround_creature);
  y--;        // x+1  y-1
  get_owner_at(x,y,&surround_creature);
  
  if (surround_creature == 0) {
    return;
  }
  
  // if here, then get the arena 4 value of "index"
  // In ZX Chaos, mounted wizards can never be engaged to enemy
  // change this so they can be, unless the wizard dismounts
  if (Options[OPT_OLD_BUGS] && arena[4][index] != 0) // the old way, "bugged"
    return;
  else if (arena[4][index] != 0 && selected_creature >= WIZARD_INDEX)  // the new way, can be engaged
    return;
  
  // compare maneouvre ratings...
  u8 our_man = CHAOS_SPELLS.pSpellDataTable[arena[0][index]]->manvr;
  u8 r = GetRand(10) + 2; 
  
  //  CALL be0a  (get parameter E=12 of creature D)
  //  LD   E,A    E = manoeuvre of creature in the square where the cursor is
  //  CALL be94        random number 0-9 stored in A
  //  INC  A          
  //  INC  A          
  //  CP   E      compare = rand(10) + 2  - manoeuvre
  if ( (r - our_man) >= 0) {
    // engaged
    set_engaged_to_enemy();

    
  }
  
  
}

void set_engaged_to_enemy(void) {
  tmp_engaged_flag = 1;
  tmp_movement_allowance = 0x1;
  tmp_is_flying = 0; // if engaged, then we can't fly about
  // print message
  clear_message();
  print_text16("ENGAGED TO ENEMY",MESSAGE_X,MESSAGE_Y, 12);  
  set_text16_colour(12, RGB16(30,30,0)); // yellow
}


// based on be52 - get the owner of creature at x,y
void get_owner_at(u8 x, u8 y, u8 *surround_creature) {
  if (y == 0 || y > 0xA)
    return;
  if (x == 0 || x > 0xF) 
    return;
  
  // based on code at bdd1
  u8 index = (x-1) + ( (y-1) << 4);
  
  if (arena[0][index] == 0) 
    return;
  if (arena[2][index] == 4) 
    return;
  
  u8 owner = 0;
  if (arena[0][index] >= SPELL_GOOEY_BLOB) {
    // a blob or more
    if ((arena[0][index] - WIZARD_INDEX) < 0)   //if it is a creature/spell from 22-28
      return;
    owner = arena[0][index] - WIZARD_INDEX + 1;
    if (owner == (current_player + 1))
      return;
  } else {
    // an actual alive creature, get the owner
    owner = GET_OWNER(arena[3][index]) + 1;
    if (owner == (current_player + 1))
      return;
  }
  
  if (owner != 0) {
    *surround_creature = owner;
  }
  
  
}

void move_walking_creature(void) {
  u16 movement_amount;
  get_distance(target_index, start_index, &movement_amount);
  if (tmp_is_flying == 0 && movement_amount > 3) {
    clear_message();
    print_text16("OUT OF RANGE",MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(0,30,30)); // light blue
    return;
  } else if (target_index == start_index) {
    return;
  }
  
  
  move_creature(movement_amount);
}

void move_flying_creature(void) {
  // based on b0a8
  if (tmp_engaged_flag) {
    tmp_movement_allowance = 0x3;
  }
//  if (IS_CPU(current_player)) {
//    s16 in_range = create_range_table(start_index, tmp_movement_allowance);
//    // get best value in LUT
//    
//  } 
  if (start_index == target_index)
    return;
  // check in range...
  u16 dist;
  get_distance(start_index, target_index, &dist);
  
  if (tmp_movement_allowance - dist >= 0) {
    // JP b148
    move_creature(dist);
  } else {
    // print out of range
    clear_message();
    print_text16("OUT OF RANGE",MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(0,30,30)); // light blue
//    wait_for_letgo();
//    wait_for_keypress();
    return;
  }
  
  
}


// after selecting a creature, pressing A again moves it to its target square
void move_creature(u8 distance_moved) {
  // code roughly based on that at aea7 onwards
  
  // this is new...
  
  
  // get the creature in the target square
  if (arena[0][target_index] != 0) {
    // is it dead?
    u8 owner = arena[3][target_index]&0x7;
    if (arena[2][target_index] != 4) {
      if (arena[0][target_index] == SPELL_MAGIC_WOOD) {
        // if magic wood...aec0
        if ((selected_creature >= WIZARD_INDEX && arena[4][target_index] != 0)
        || selected_creature < WIZARD_INDEX)  {
          // we are a wizard and the tree is occupied
          // or we are a creature attacking a tree (can attack our own trees)
          // jump to attack b168 (via aeea)
          make_attack();
          return;
        } 
      } else {
        // not a magic wood
        // get the target square creature
        //aede
        
        if (arena[0][target_index] < SPELL_MAGIC_FIRE || (arena[0][target_index] == SPELL_SHADOW_WOOD))  {
          // a creature or shadow wood  
          if (owner != current_player) {
            // jump b168 attack
            make_attack();
            return;
          } else {
            // aeed
            // creature we are moving to belongs to current player
            // check if we have a wizard selected for movement...
            
            if ((WIZARD_INDEX+current_player) == selected_creature) {
              // we are a wizard... check the target creature
              
              if (arena[0][target_index] < SPELL_HORSE || arena[0][target_index] >= SPELL_BAT) {
                // not a mount, return
                return;
              } 
            } else {
              return;
            }
            
          }
        } else {
          // not an animal-type creature or shadow wood (i.e. could be a wizard or a citadel)
          // jump af0d
          if (arena[0][target_index] < SPELL_MAGIC_CASTLE || arena[0][target_index] >= SPELL_WALL) {
            // not a magic castle/citadel, must be 23 (fire) or wizard
            // jump af2b
            if (arena[0][target_index] < WIZARD_INDEX) //must be fire - can't move here
              return;
            
            if (selected_creature < WIZARD_INDEX) {
              // not a wizard selected, so check we aren't attacking our owner
              if ( (arena[3][start_index] & 0x7) == (arena[0][target_index] - WIZARD_INDEX) )
                return;
              
            }
            // got to here?, we are a wizard attacking another wizard
            // or a creature attacking an enemy wizard
            //attack - jump b168
            make_attack();
            return;
          } else {
            // a magic castle/citadel
            if ((WIZARD_INDEX+current_player) == selected_creature) {
              // wizard selected for movement
              if (owner != current_player) {
                // not our castle so can't go in
                return;
              }
            } else {
              // not a wizard selected, so can't go inside or attack
              return;
            }
            // got to here? we are a wizard and the castle is ours, so move
          }
        }
      }
    }
  } 
  //else nothing here, jump to af50
  // af50...
  do_successful_move(distance_moved);
  
  
}

// based on af50...
void do_successful_move(u8 distance_moved) {
  if (tmp_engaged_flag)
    return;
  
  if (selected_creature == SPELL_SHADOW_WOOD) {
    // shadow wood, don't move!
    // end creature turn? jump b06f
    end_movement();
    return;
  }
  if (IS_CPU(current_player))
    move_screen_to(target_index);
    
  PlaySoundFX(SND_WALK);
  // flag that we need to remake the CPU movement table
  move_table_created = 0;
  
  // tidy up the start square
  
  //u8 start_a0 = arena[0][start_index];
  u8 start_a3 = arena[3][start_index];
  u8 start_a4 = arena[4][start_index];
  
  if (selected_creature >= WIZARD_INDEX && selected_creature != arena[0][start_index]) {
    // the start creature is a wizard, but the visible thign is not
    // i.e. wizard moving out of something
    start_a4 = 0;
  } 
  else {
      // af88
    if (arena[5][start_index] != 0) {
      // af93  
      arena[0][start_index] = arena[5][start_index]; //creature in arena 5??
      arena[2][start_index] = 4; // dead
      arena[5][start_index] = 0; //clear creature in arena 5 (not sure about this)
    } else {
      arena[0][start_index] = 0; // 1 in the game...
    }
    
  }
  // afa3..
  arena[1][start_index] = 1; //update anim
  arena[4][start_index] = 0; // nothing here
  
  // sort out the target square
  
  // continue at afc7....
  if (arena[0][target_index] == 0 || arena[2][target_index] == 4) {
    // afdf
    u8 old_target = arena[0][target_index];
    arena[0][target_index] = selected_creature;
    arena[5][target_index] = old_target;
    arena[1][target_index] = 1;
    arena[2][target_index] = 0;
    arena[3][target_index] = start_a3;
    arena[4][target_index] = start_a4;
  } else {
    // afd7
    // something in target square and thing there is not dead
    // mustbe a wiz moving to a mount or castle
    arena[4][target_index] = selected_creature;
    // end the wizard's move
    tmp_wizard_movement_allowance = 0;
    tmp_movement_allowance = 0;
    // jump b007...
    
  } 
  // b007
  // do a sound effect...
  // check engaged flag if we haven't moved to a magic wood
  start_index = target_index;
  if (attacker != 0) {
    end_movement();
    return;
  }
  
  if (arena[0][start_index] != SPELL_MAGIC_WOOD) {
    // we have moved onto something that isn't a magic wood
    check_engaged(start_index);
    if (tmp_engaged_flag)
      return;
    
    if (tmp_wizard_movement_allowance != 0) {
      // jump b06f...
      end_movement();
      return;
    } 
    
  }
  // b039
  wait_for_letgo();
  s8 ma = tmp_movement_allowance - distance_moved;
  if (tmp_is_flying || ma <= 0)  {
    tmp_movement_allowance = 0;
    // jump b06f
    end_movement();
    return;
  }
  tmp_movement_allowance = ma;
  // show "movement points left" message
  char str[3];
  clear_message();
//  sprintf(str, "%d", (tmp_movement_allowance+1)>>1);
  int2a((tmp_movement_allowance+1)>>1, str, 10);
  
  print_text16("MOVEMENT POINTS LEFT=",MESSAGE_X,MESSAGE_Y, 12);  
  set_text16_colour(12, RGB16(0,30,0)); // green
  print_text16(str,MESSAGE_X+21,MESSAGE_Y, 13);  
  set_text16_colour(13, RGB16(30,30,0)); // yellow
  //////////   end message /////////////
  
}

// end turn for creature at start_index
// based on b06f
void end_movement(void) {
  // whatever happens, the wizard has definitely moved...
  has_wizard_moved = 1;
  // and we definitely need to remake the movement table
  move_table_created = 0;
  attacker = 0;
  
  // set has moved flag...
  arena[3][start_index] |= 0x80;
  clear_message();
  if (arena[0][start_index] == SPELL_SHADOW_WOOD) {
    attacker = 0;
    tmp_range_attack = 0;
    selected_creature = 0;
    return; // if shadow wood
  }
  
  // check range attack...
  if (tmp_range_attack) {
    tmp_range_attack = 0;
    selected_creature = 0;
    return;
  }
  
  if (arena[0][start_index] == 0) {  
    selected_creature = 0;
    return;
  }
  
  u8 has_rc = 0;
  if (arena[0][start_index] >= WIZARD_INDEX) {
    has_rc = (players[arena[0][start_index] -WIZARD_INDEX].ranged_combat != 0);
  }  else {
    has_rc = (CHAOS_SPELLS.pSpellDataTable[arena[0][start_index]]->rangedCombat != 0);
  }
  
  if (has_rc) {
    
    
    if (arena[0][start_index] >= WIZARD_INDEX) {
      tmp_range_attack = players[arena[0][start_index] -WIZARD_INDEX].range;
    }  else {
      tmp_range_attack = CHAOS_SPELLS.pSpellDataTable[arena[0][start_index]]->rangedCombatRange*2+1;
    }
    // code from b8dd - init range attacks
    // draw the border a funny colour...
    
    // print the message RANGED COMBAT,RANGE= range val
    PlaySoundFX(SND_RANGE);
    char str[3];
    clear_message();
//    sprintf(str, "%d", (tmp_range_attack)>>1);
    int2a((tmp_range_attack)>>1, str, 10);
    
    print_text16("RANGED COMBAT,RANGE=", MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(0,30,0)); // green
    print_text16(str,MESSAGE_X+20,MESSAGE_Y, 13);  
    set_text16_colour(13, RGB16(30,30,0)); // yellow
    draw_cursor(CURSOR_FIRE_GFX);
    // set the border colour too
    BGPaletteMem[16*11+2] = RGB16(31,0,31);
    BGPaletteMem[16*11+1] = RGB16(0,0,0);
    
    // seperate out the cpu stuff...
    // originally looped on keyboard here, moving cursor about then waited for S to be pressed
    // i moved that code to do_range_attack(), which is triggered when 
    // you press A again on the joypad
    // also, there was some CPU specific code here, which has gone into the cpu .c
    // but the initial cpu target table creation is easiest to leave in here...
    if (IS_CPU(current_player)) {
      create_all_enemies_table();
      delay(20);
    }
  } else {
    
    tmp_range_attack = 0;
    selected_creature = 0;  
  }
  
}

// start_index attacks thing at target_index 
// code taken from b168 - either moving to a creature or firing on one
void make_attack(void) {
  
  attacker = arena[0][start_index];
  u8 defender = arena[0][target_index];
  s8 attacking_val = 0;
  s8 defending_val = 0;
  u8 attacker_undead = 0;
  u8 defender_undead = 0;
  u8 attacker_modifier = 0;
  u8 defender_modifier = 0;
  
  if ( (attacker >= SPELL_VAMPIRE && attacker <= SPELL_ZOMBIE) || IS_UNDEAD(arena[3][start_index]) ) {
    attacker_undead = attacker;
  }
  
  
  if (attacker >=WIZARD_INDEX) {
    // wizard attacking
    // remove shadow form
    players[attacker-WIZARD_INDEX].modifier_flag &= ~0x8;
    attacker_modifier = players[attacker-WIZARD_INDEX].modifier_flag & 0x7;
    attacker_undead = attacker_modifier;
    if (players[attacker-WIZARD_INDEX].ranged_combat != 0)
      attacker_undead = players[attacker-WIZARD_INDEX].ranged_combat;
    
    // possible bug here - if has magic bow, can melee attack undead?
    
  }
  
  // continues at b1f8...
  if ( (defender >= SPELL_VAMPIRE && defender <= SPELL_ZOMBIE) || IS_UNDEAD(arena[3][target_index]) ) {
    defender_undead = defender;
  }
  
  if (defender >= WIZARD_INDEX) {
    // modify defence if defender has shield or armour
    defender_modifier = players[defender-WIZARD_INDEX].modifier_flag & 0xC0;
    defender_modifier = defender_modifier >> 6; 
    if (defender_modifier) {
      defender_modifier++;
    }
    // if defender has shadow form, gets bonus 3 defence!
    if (HAS_SHADOWFORM(players[defender-WIZARD_INDEX].modifier_flag)) {
      defender_modifier += 3;
    }
  }
  // b23d
  if (defender_undead != 0 && attacker_undead == 0) {
    
    // UNDEAD-CANNOT BE ATTACKED and return 
    clear_message();
    print_text16("UNDEAD-CANNOT BE ATTACKED",MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(0,30,30)); // light blue
    // do a sound fx
    wait_for_letgo();
    wait_for_keypress();
    clear_message();
    return;
  }
  
  if (IS_CPU(current_player))
    move_screen_to(target_index);
  
  // get attacking val...
  if (tmp_range_attack) {
    if (attacker < WIZARD_INDEX)
      attacking_val = CHAOS_SPELLS.pSpellDataTable[arena[0][start_index]]->rangedCombat;
    else
      attacking_val = players[attacker-WIZARD_INDEX].ranged_combat;
  }
  else {
    if (attacker < WIZARD_INDEX)
      attacking_val = CHAOS_SPELLS.pSpellDataTable[arena[0][start_index]]->combat;
    else
      attacking_val = players[attacker-WIZARD_INDEX].combat;
  }
  
  // bug here - if the wizard has magic knife, it improves his ranged combat!
  attacking_val += attacker_modifier + GetRand(10);
  
  // get defending val...
  if (defender < WIZARD_INDEX)
    defending_val = CHAOS_SPELLS.pSpellDataTable[defender]->defence;
  else
    defending_val = players[defender-WIZARD_INDEX].defence;
  
  defending_val += defender_modifier + GetRand(10);
  
  wait_for_letgo();
  
  if (!tmp_range_attack) {
    // do attack anim...
    // CALL b375
    
    do_attack_anim(target_index);
  }
  
  if (defending_val < attacking_val) {
    // attack was a success...
    if (arena[4][target_index] == 0) {
      // nothing in arena 4...
      if (defender >= WIZARD_INDEX) {
        // was a wizard, do wizard death anim...
        kill_wizard();
      } 
      if (defender >= SPELL_VAMPIRE || 
        IS_UNDEAD(arena[3][target_index]) ||
        IS_ILLUSION(arena[3][target_index]) ) {
        // shouldn't leave anything underneath...
      } else {
        defender = 0xFF;
      }
      // jump b310
      remove_creature(defender);
    } else {
      // there was something in the arena 4... b2e0
      u8 arena0 = arena[0][target_index]; 
      u8 arena3 = arena[3][target_index]; 
      u8 arena4 = arena[4][target_index];
      arena[4][target_index] = 0;
      if (defender == SPELL_GOOEY_BLOB) {
        arena[3][target_index] = arena[5][target_index];
      }
      else if (!Options[OPT_OLD_BUGS]) {
        // the famous "undead wizard" bug is caused by not updating the arena[3] flag properly
        if (arena4 >= WIZARD_INDEX) {
          arena[3][target_index] = arena4-WIZARD_INDEX;
        }
      }
      /* Another mini-bug here - if we kill a ridden mount, the wizard appears in
       * the square (that's correct), but the mount leaves no corpse. Needs a fix?
       * arena[5] holds corpse info...
       */ 
      arena[0][target_index] = arena4;
      if ((arena0 > SPELL_MANTICORE) || IS_UNDEAD(arena3) || IS_ILLUSION(arena3))
	arena[5][target_index] = 0;
      else
	arena[5][target_index] = arena0;

      // jump b06f
      end_movement();
    } 
  } else {
    // attack fails...
    // jump b06f
    end_movement();
  }
  
  
  
}


// this code was originally at b94f...
// cpu specific stuff moved to computer.c
void do_range_attack(void) {
  // have a target... attack!
  //char str[30];
  u8 i, x, y;
  if (target_index == start_index) {
    // can't attack self.
    return;
  }
  u16 distance;
  get_distance(start_index, target_index, &distance);
  if (tmp_range_attack < distance) {
    // print out of range and return
    
    clear_message();
    print_text16("OUT OF RANGE",MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(0,30,30)); // light blue
    return;
  }
  // in range, not attacking self...
  if (los_blocked(target_index, 0)) {
    clear_message();
    print_text16("NO LINE OF SIGHT",MESSAGE_X,MESSAGE_Y, 12);  
    set_text16_colour(12, RGB16(31,30,0)); // lblue
    delay(4);
    return;
  }
  remove_cursor();
  delay(4);
  
  // if got to here, do the range attack animation...
  u8 ra_type = 1; // range attack type
  if (selected_creature >= SPELL_DRAGON_GREEN && selected_creature <= SPELL_DRAGON_GOLDEN) {
    ra_type = 4;
  }
  if (selected_creature == SPELL_MANTICORE) {
    ra_type = 2;
  }
  // do the line animation, which is the same code as the los check
  PlaySoundFX(SND_BEAM);
  los_blocked(target_index, ra_type);
  // redraw the creatures righ now
  delay(4);
  
  // do the attack animation 
  void (*animFunc)(u8 x, u8 y, u8 frame);
  if (selected_creature >= SPELL_DRAGON_GREEN && selected_creature <= SPELL_DRAGON_GOLDEN) {
    // dragon breath attack
    animFunc = draw_breath_frame;
    PlaySoundFX(SND_FIRE);
  }
  else {
    // bloop attack thing
    animFunc = draw_splat_frame;
    PlaySoundFX(SND_SPELLSUCCESS);
  }
  
  get_yx(target_index, &y, &x);
  for (i = 0; i < 8; i++) {
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    animFunc(x-1, y-1, i);
  }
  delay(4);
  
  // I was missing this bit before
  if (!(arena[0][target_index] == 0 || arena[2][target_index] == 4)) {
    if (arena[0][target_index] < SPELL_MAGIC_FIRE ||  arena[0][target_index] >= WIZARD_INDEX ||
        arena[0][target_index] == SPELL_MAGIC_WOOD || arena[0][target_index] == SPELL_SHADOW_WOOD) {
        // make the attack on the creature...
        make_attack();
    }
  }
  
  set_border_col(current_player);
  
  selected_creature = 0;
  tmp_range_attack = 0;
  attacker = 0;
  if (!IS_CPU(current_player)
#ifdef __WINDOWS__
      && ! (ONLINE_MODE && IS_REMOTE(current_player))
#endif
      )
    redraw_cursor();
}


// b310
void remove_creature(u8 creature) {
  // remove the creature in target_index and move there
  tmp_engaged_flag = 0;
  tmp_movement_allowance = 0;
  tmp_wizard_movement_allowance = 0;
  
  if (creature == 0xFF) {
    // leaves corpse
    arena[1][target_index] = 1;
    arena[2][target_index] = 4;
    delay(12);
    // do sound fx
    if (tmp_range_attack == 0)
      do_successful_move(3);
    
  } else {
    // creature doesn't leave a corpse
    // check arena 5 val
    if (arena[5][target_index] == 0) {
      arena[0][target_index] = 0;
      arena[1][target_index] = 1;
    } else {
      arena[0][target_index] = arena[5][target_index];
      arena[1][target_index] = 1;
      arena[2][target_index] = 4;
      arena[5][target_index] = 0;
    }
    delay(6);
    
    if (tmp_range_attack == 0)
      do_successful_move(3);

    
  }
  
  
}

void do_attack_anim(u8 index) {
  remove_cursor();
  // do sound fx
  u8 x, y;
  get_yx(index, &y, &x);
  u8 i,j;
#ifdef __GBA__
  PlayLoopedSoundFX(SND_ATTACK,2);
  // the bug here is that in the timer code, I do:
  /*  samples--;
      if (loop) {
      play sample again
      }

      if (samples == 0)
      really switch off all the sound effects

      so looping sounds will have the sound reg pulled
      from under them. Then when the register is turned
      on again, the looping sample is raring to go and
      plays straight away.

      should do a samples++; in the "if (loop) " bit.
      
  */
#endif
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 4; j++) {
      wait_vsync_int();
      wait_vsync_int();
      draw_fight_frame(x-1, y-1, j);
    }
  }
  
  if (!IS_CPU(current_player)
#ifdef __WINDOWS__
      && ! (ONLINE_MODE && IS_REMOTE(current_player))
#endif
      )
    redraw_cursor();
}


void drawn_contest(void) {
  // "THE CONTEST IS DRAWN BETWEEN"
  // then wipe all the flags and stuff and return to splash screen
  // clear arena screen...
  fade_down();
  remove_cursor();
  clear_arena();
  clear_bg();
  clear_palettes();
  
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
  
  set_text16_colour(10, RGB16(30,31,0)); // yellow
  print_text16("THE CONTEST IS DRAWN BETWEEN", 1,1, 10);  
  u8 y = 3, i;
  for (i = 0; i < playercount; i++) {
    if (!IS_WIZARD_DEAD(players[i].modifier_flag)) {
      print_text16(players[i].name, 9,y, 13);  
      y+=2;
    }
  }
  fade_up();
  end_game();
  
}

void win_contest(void) {
  // <player>" IS THE WINNER" 
  // then wipe all the flags and stuff and return to splash screen
  fade_down();
  u8 winner = 0, i;
  for (i = 0; i < playercount; i++) {
    if (!IS_WIZARD_DEAD(players[i].modifier_flag)) {
      winner = i;
      break;
    }
  }
  // clear arena screen...
  remove_cursor();
  clear_arena();
  clear_bg();
  clear_palettes();
  
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
  
  //char str[30];
  set_text16_colour(10, chaos_cols[2]); 
  set_text16_colour(12, chaos_cols[4]); 
  set_text16_colour(13, chaos_cols[8]); 
  
  print_text16("THE WINNER IS:", 8,2, 10);  
  print_text16("++++++++++++++++", 7,6, 12);  
  print_text16("+", 7,8, 12);  print_text16("+", 22,8, 12);  
  print_text16("+", 7,10, 12);  print_text16("+", 22,10, 12);  
  print_text16(players[winner].name, 10,10, 13);  
  print_text16("+", 7,12, 12);  print_text16("+", 22,12, 12);  
  print_text16("++++++++++++++++", 7,14, 12);  
  fade_up();
  end_game();
  
}

void end_game_option(void) {
  fade_down();
  // clear arena screen...
  remove_cursor();
  clear_arena();
  clear_bg();
  clear_palettes();
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
  
  //char str[30];
  set_text16_colour(9, RGB16(31,31,0)); 
  
  print_text16("CONTINUE THE BATTLE?", 5,4, 9);  
  print_text16("YES", 10,8, 7);  
  print_text16("NO",  16,8, 8);  
  
  print_text16("PRESS A", 11,16,  9);  
  
  set_text16_colour(7, RGB16(31,31,0)); 
  set_text16_colour(8, RGB16(12,12,0)); 
  
  u8 decided = 0;
  hilite_item = 0;
  fade_up();
  while (!decided) {
    wait_vsync_int();
    wait_vsync_int();
    UpdateKeys();

    
    if (KeyPressedNoBounce(KEY_LEFT,8)) {
      hilite_item = 0;
    }
    if (KeyPressedNoBounce(KEY_RIGHT,8)) {
      hilite_item = 1;
    }
    if (KeyPressedNoBounce(KEY_A,8)) {
      decided = 1;
    }
    
    if (KeyPressedNoBounce(KEY_B,8)) {
      decided = 1;
      hilite_item = 0;
    }
    
    if (hilite_item  == 0 ) {
      set_text16_colour(7, RGB16(31,31,0)); 
      set_text16_colour(8, RGB16(12,12,0)); 
    } else {
      set_text16_colour(8, RGB16(31,31,0)); 
      set_text16_colour(7, RGB16(12,12,0)); 
    }
  }
  current_player = 0;
  if (hilite_item) {
    current_player = 9;
  }
  
  
}

    

void end_game(void) {
  players[0].plyr_type = PLYR_HUMAN;
  current_player = 0; // otherwise cpu player might just immediately "press" a key!
  wait_for_letgo();
  u8 keypressed = 0;
  u8 i = 0, j = 4, k = 8;
  while (!keypressed) {
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    wait_vsync_int();
    set_text16_colour(10, chaos_cols[i]); 
    set_text16_colour(12, chaos_cols[j]); 
    set_text16_colour(13, chaos_cols[k]); 
    
    BGPaletteMem[16*15+1] = chaos_cols[j];
    BGPaletteMem[16*15+2] = chaos_cols[i];
    
    i++; j++; k++;
    if (i > 8) i=0;
    if (j > 8) j=0;
    if (k > 8) k=0;
    
    wait_vsync_int();
    UpdateKeys();
    if (KeyPressed (KEY_A)){
      keypressed = 1;
    }
  }
  fade_down();
  clear_game_border();
  wait_for_letgo();
  reset_arena_tables();
  reset_players();
  world_chaos = 0;
  current_player = 0;
  dead_wizards = 0;
  playercount = 0;
  load_all_palettes();
  show_splash();
  move_screen_to(0);
  fade_up();
}
