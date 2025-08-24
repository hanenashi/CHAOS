#include "platform.h"
#if defined( __WINDOWS__ ) && defined(NETWORK)
#include "network.h"
#include "remote.h"

#include "winkeys.h"

#include "text16.h"
#include "chaos.h"
#include "arena.h"
#include "gfx.h"
#include "casting.h"
#include "creature.h"
#include "wizards.h"
#include "movement.h"
#include "magic.h"
#include "spelldata.h"
#include "spellselect.h"

#define CAST_DAT_LOCATION 0
#define CAST_DAT_SUCCESS  1
#define CAST_DAT_CHAOS    2
#define CAST_DAT_RANDSEED 3
#define CAST_DAT_COUNT    4

// called when a remote player turn to cast 
void do_remote_spell(void) {
  
  if (IS_WIZARD_DEAD(players[current_player].modifier_flag)) {
    // shouldn't happen?
    return;
  }
#ifdef DEBUG_NET
  printf("do_remote_spell\n"); 
#endif
  remove_cursor();
  print_name_spell();
  // the current spell is already decided...
  int vengeance_begin = 0;
  for (;;) {  
    // poll server for square
#ifdef DEBUG_NET
    printf("do_remote_spell\n"); 
#endif
    int message = get_server_message();
    if (message == NET_ERROR) {
      return;
    }
    // now the server has sent a message, it is stored in the g_serverMessage space
    // hopefully it is a NET_SPELL_CAST message
    if (message == NET_SPELL_CAST) {
      /*
	 1. set the user spell to 0 for this spell index (remove spell)
	 2. retrieve from server the following data:
	 position
	 spell success rating
	 world chaos
	 random seed value
	 3. check if it is a justice/vengeance spell, and if so check the success
	 if these fail, do not call spell->pFunc(), just say "failed"
	 4. call pFunc, this performs the local spell anim, updates arena etc	

      */
      // retrieve the data we are expecting:
      int data[CAST_DAT_COUNT];
      server_data(data, CAST_DAT_COUNT);

      temp_illusion_flag = players[current_player].illusion_cast;
      target_index      = data[CAST_DAT_LOCATION];
      temp_success_flag = data[CAST_DAT_SUCCESS];
      world_chaos       = data[CAST_DAT_CHAOS];
      setSeed(data[CAST_DAT_RANDSEED]);
#ifdef DEBUG_NET
      printf("Received illusion %d target %d success %d \n     worldchaos %d seed %d\n",
        temp_illusion_flag, target_index, 
        temp_success_flag, world_chaos, GetSeed()
      );
#endif
      // set spell to 0 (not really needed)
      if (current_spell != SPELL_DISBELIEVE) {
	// set the current spell to 0 if it isn't disblv.
	players[current_player].spells[players[current_player].selected_spell] = 0;
      }
      players[current_player].selected_spell = 0; // set to 0 for "no spell selected"
      // if veng or just, then end turn if it failed
      if (current_spell >= SPELL_VENGEANCE && current_spell <= SPELL_JUSTICE) {
	if (temp_success_flag == 0) {
	  // print spell success/fail message...
	  print_success_status();
	  delay(20);
	  temp_cast_amount = 0;
	  break;
	} else {
          vengeance_begin++;
        }
      }
      // do the cast... everything should sort itself out here (all being well)
      if (vengeance_begin != 1)
        CHAOS_SPELLS.pSpellDataTable[current_spell]->pFunc();
    } else if (message == NET_SPELL_CANCEL) {
      casting_b();
    }

#ifdef DEBUG_NET
    printf("Done the spell, tmp cast %d\n",temp_cast_amount); 
#endif
    if (temp_cast_amount == 0 && (vengeance_begin != 1)) {
      break;
    }
  }
}


// called when the remote players turn to move
void do_remote_movement(void) {
  int done = 0;
  while (!done) {
    int message = get_server_message();
    if (message == NET_ERROR) {
      return;
    }

    if (message == NET_MOVE_A) {
      int data[2];
      server_data(data, 2);
      target_index = data[0];
      setSeed(data[1]);
      movement_a();
    } else
    if (message == NET_MOVE_B) {
      int data[1];
      server_data(data, 1);
      target_index = data[0];
      movement_b();
    } else
    if (message == NET_MOVE_START) {
      movement_start();
      done = 1;
    }
  }
}

// called when a remote player needs to dismount
int remote_dismount(void) {
  for (;;) {
    int message = get_server_message();
    if (message == NET_ERROR) {
      return 0;
    }
    if (message == NET_DISMOUNT) {
      int data;
      server_data(&data, 1);
#ifdef DEBUG_NET
      printf("received NET_DISMOUNT signal %d\n", data);
#endif
      return data;
    } else {
      return 0;
    }
  }
}
// called this when a local network player dismounts (or not)
void send_dismount(int yes_pressed) {
  send_message(NET_DISMOUNT, &yes_pressed, 1);
}

/* 
 * Called when we finish setting up...  
 * calls each client and tells them who is online
 * and the details... 
 */
void tell_clients_init(void) {
  if (is_hosting()) {
    int c,p;
    // loop over clients
#ifdef DEBUG_NET
    printf("Send all clients the wizards:\n"); 
#endif
    for (c = 1; c < 8; c++) {
#ifdef DEBUG_NET
      printf("client %d ", c); 
#endif
      if (client_online(c)) {
#ifdef DEBUG_NET
	printf("online, sending:\n"); 
#endif
	// first tell them how many to wait for
	send_player_count(c, playercount);
	// then send them the details
	for (p = 0; p < playercount; p++) {
#ifdef DEBUG_NET
	  printf("\t%d c%d i%d #%s#\n",p, players[p].colour, players[p].image,
	      players[p].name); 
#endif
	  send_player_details(c, 
	      players[p].colour, 
	      players[p].image, 
	      players[p].name);
	}
	int data = GetSeed();
#ifdef DEBUG_NET
	printf("send random seed too %d\n", GetSeed()); 
#endif
	send_message_to_client(c,NET_WAKE_UP, &data,1);
#ifdef DEBUG_NET
	printf("done\n"); 
#endif
      } else {
#ifdef DEBUG_NET
	printf("offline\n"); 
#endif
      }
    }
  }
}

int get_server_wizards(void) {
  int message = get_server_message();
  // check we receive the player count
  if (message == NET_ERROR) {
    printf("error waiting on player count\n");
    return message;
  }
  int my_id;
  server_player_count(&playercount,&my_id); 
#ifdef DEBUG_NET
  printf("There are %d players on client %d:\n", playercount, my_id);
#endif
  int p;
  // then for each player, wait to recieve details
  for (p = 0; p < playercount; p++) {
    message = get_server_message();
    // check we receive the player count
    if (message == NET_ERROR) {
      printf("error waiting on wiz data\n");
      return message;
    }
    server_get_wizard(&players[p].colour , &players[p].image,   players[p].name);
    if (p == my_id)
      players[p].plyr_type = PLYR_LOCAL;
    else
      players[p].plyr_type = PLYR_REMOTE;
#ifdef DEBUG_NET
    printf("player %d #%s# col %d img %d type %x\n", p, players[p].name, 
	players[p].colour, players[p].image, players[p].plyr_type);
#endif
  }
  // one last message is sent - the server's random seed
#ifdef DEBUG_NET
  printf("Start %d waiting on random seed\n", my_id);
#endif
  message = get_server_message();
  if (message == NET_ERROR) {
    printf("error waiting on random seed\n");
    return message;
  }
  server_data(&p, 1);
#ifdef DEBUG_NET
  printf("Client %d : Server seed %d\n", my_id, p);
#endif
  setSeed(p);
  
  return NET_SET_WIZ;
}

// clients can send asyncronous messages... handle this for spell selection
void receive_spellinfo(int playerid, int spell, int illusion) {
  if  (playerid < 8) {
#ifdef DEBUG_NET
    printf("receive_spellinfo played %d spell %d illu? %d\n", playerid, spell, illusion);
#endif
    players[playerid].selected_spell = spell;
    players[playerid].illusion_cast = illusion;
    waiting_for_spells++; 
#ifdef DEBUG_NET
    printf("waiting_for_spells = %d (of 8)\n",waiting_for_spells);
#endif
  }
}

// player sends his choice...
void send_spellchoice(int who) {
  int data[2];
  data[0] = players[who].selected_spell;
  data[1] = players[who].illusion_cast;
  send_message(NET_SPELL_CHOICE, data, 2); 
}

// called from client machine to tell server we moved/etc
void send_movement_a() {
  int data[2];
  data[0] = cursor_x+cursor_y*16;
  data[1] = GetSeed();
  send_message(NET_MOVE_A, data, 2);
}
void send_movement_b() {
  int data = cursor_x+cursor_y*16;
  send_message(NET_MOVE_B, &data, 1);
}
void send_movement_start() {
  int data = 0; 
  send_message(NET_MOVE_START, &data, 1); 
}

/* send a message saying we have cast a spell
   data required at the other end:
   location of cursor
   if the spell succeeded
   current world chaos
   new random seed (keep it synch'd)
   */
void send_casting_a() {
  
  int data[4];

  data[CAST_DAT_LOCATION] = cursor_x+cursor_y*16;
  data[CAST_DAT_SUCCESS]  = temp_success_flag; 
  data[CAST_DAT_CHAOS]    = world_chaos;
  data[CAST_DAT_RANDSEED] = GetSeed();
#ifdef DEBUG_NET
  printf("Send illusion %d target %d success %d \n     worldchaos %d seed %d\n",
      players[current_player].illusion_cast, data[CAST_DAT_LOCATION], 
      data[CAST_DAT_SUCCESS], data[CAST_DAT_CHAOS], data[CAST_DAT_RANDSEED]
      );
#endif
  
  send_message(NET_SPELL_CAST, data, CAST_DAT_COUNT);

}
void send_casting_b() {
  int data = 0; 
  send_message(NET_SPELL_CANCEL, &data, 1); 
}

// flag for locking the wait thread..
int waiting_for_spells;
// show a generic "Waiting for other players" screen...
void show_spellwaiting_screen(void) {
  // spell waiting screen
  print_text16("WAITING..", 2,19,2);

  if (is_hosting()) {
    // if we are the host, then wait for every1 to respond
    // then pass on all players choices to all clients
    
    // waiting_for_spells is incremented everytime someone tells us
    // about their spell selection - doesn't matter when or where
    // when this counter == 8, we have all the spells from everyone
    // discounts dead and disconnected wizards
    while (waiting_for_spells < 8) {
      wait_vsync_int();
      UpdateKeys();
      // animate screen... 
    }
    // now we are done waiting, send spells
    int c,p;
    // loop over clients
#ifdef DEBUG_NET
    printf("Send all clients the spells selected:\n");
#endif
    // no need to send to ourselves (c==0) 
    for (c = 1; c < 8; c++) {
#ifdef DEBUG_NET
      printf("client %d ", c); 
#endif
      if (client_online(c)) {
#ifdef DEBUG_NET
	printf("online, sending:\n"); 
#endif
	// first tell them how many to wait for
	// then send them the details
	for (p = 0; p < playercount; p++) {
#ifdef DEBUG_NET
	  printf("\t%d #%s# %d %d \n",p,players[p].name,
	      players[p].selected_spell, 
	      players[p].illusion_cast); 
#endif
	  send_spell_choice(c,p, players[p].selected_spell, 
	      players[p].illusion_cast);
	}
#ifdef DEBUG_NET
	printf("done\n");
#endif

	send_message_to_client(c,NET_WAKE_UP, &c, 1);
      } else {
#ifdef DEBUG_NET
	printf("offline\n"); 
#endif
      }
    }
  } else
  if (is_connected()) {
    // wait for the server to tell us that every1 has finished
    // then wait for the list of all the chosen spells
    
    for (;;) {
      // get a message:
      int message = get_server_message();
      // now act on it
      if (message == NET_ERROR) {
	//
        break;	
      }
      // else get the spell sent
      if (message == NET_SPELL_CHOICE) {
	// get the spell data...
	int who, spell, illusion;
	server_spell(&who, &spell, &illusion);
	players[who].selected_spell = spell;
	players[who].illusion_cast = illusion;
#ifdef DEBUG_NET
	printf("server: player %d casts %d (%s)\n", who, spell, illusion?"illusion":"real");
#endif

      } else {
	break;
      }
    } 
  } 
}

void update_spellset(void) {
  int i, tmp;
  for (i = 0; i < playercount; i++) {
    tmp = current_player;
    current_player = i;
    remove_null_spells();
    current_player = tmp;
  }
}

int get_next_local(int id) {
  int i;
  for (i = id+1; i < playercount; i++) {
    if (!IS_REMOTE(i) && !IS_WIZARD_DEAD(players[i].modifier_flag))
      return i;
  }
  return 9;
}

void reset_spellwaiting(void) {
  int i;
  waiting_for_spells = 0;
  for (i = 0; i < 8; i++) {
    if (IS_WIZARD_DEAD(players[i].modifier_flag) || 
	players[i].plyr_type == PLYR_NC ) 
    {
      waiting_for_spells++;
    }
  }
}

// callback for when a client disconnects...
void  client_disconnected(int clientid) {
  // this is not ideal... but better than nothing?
  if (is_hosting()) {
    players[clientid].plyr_type = PLYR_NC;
    waiting_for_spells++;
  }
}

// at the end of each round, should have all clients call the server
// and check who is still online - anyone not online should be killed

#endif

