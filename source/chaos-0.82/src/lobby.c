// lobby.c

// A lobby for creating online players
// a copy of players.c but with gba stuff removed
#include "platform.h"
#if defined(__WINDOWS__)
#include "winkeys.h"
#include "winsound.h"
#include "network.h"
#include "remote.h"
#define PLAYER_WIZ_Y 2

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


/////////////////////////////////////////////////////////

void select_lobby_item(u8 item);
void deselect_lobby_item(u8 item);
void create_default_lobby(void);


void update_lobby(void);
void init_players(void);
unsigned char array_contains(const unsigned char * array, unsigned char val);

/////////////////////////////////////////////////////////
#ifdef NETWORK
void show_create_lobby(void) {
  current_screen = SCR_CREATE_LOBBY;
  clear_bg();
  clear_palettes();
  load_bg_palette(9,9);
  hilite_item = 0;
  anim_col = 30;
  hilite_wizard_item = 0;
  
  if (is_hosting()) {
    print_text16("How many players?", 4,1, 10);
  } else {
    print_text16("Wizard selection", 4,1, 10);
  }
  set_text16_colour(10, RGB16(31,30,30));
  
  // create the default start wizards
  // careful here! for hosts this should create the host player
  // and 1 other
  if (is_hosting())
    playercount = 2;
  else 
    playercount = 1;
  create_default_lobby();
  
  draw_decor_border(15, RGB16(0,31,0),RGB16(0,31,31)); 

  update_lobby();
  
  select_lobby_item(0);
  
}

// player is not connected, draw in grey
const u16 HILITE_NC_COL = RGB16(19,19,19);
const u16 NO_HILITE_NC_COL = RGB16(12,12,12);
const u16 HILITE_C_COL = RGB16(31,30,30);
const u16 NO_HILITE_C_COL = RGB16(0,30,30);

int get_lobby_colour(int plyr) {
  if (players[plyr].plyr_type == PLYR_NC && players[plyr].plyr_type != PLYR_HUMAN) {
    if (hilite_item -1 == plyr)
      return HILITE_NC_COL;
    else
      return NO_HILITE_NC_COL;
  } else {
    if (hilite_item -1 == plyr)
      return HILITE_C_COL;
    else
      return NO_HILITE_C_COL;
  }
}

// used to just redraw, not reinit players
void redraw_lobby(void) {
  current_screen = SCR_CREATE_LOBBY;
  clear_bg();
  clear_palettes();
  load_bg_palette(9,9);
  hilite_item = 0;
  anim_col = 30;
  hilite_wizard_item = 0;
  
  draw_decor_border(15, RGB16(0,31,0),RGB16(0,31,31)); 
  
  print_text16("How many players?", 2,1, 10);
  set_text16_colour(10, RGB16(0,30,30));
  update_lobby();
  
  hilite_item = current_player+1;
  select_lobby_item(hilite_item);
  current_player = 0;
}

  

void lobby_up(void) {
  wait_vsync_int();
  deselect_lobby_item(hilite_item);
  anim_col = 30;
  if (hilite_item > 0)
    hilite_item--;
    
  select_lobby_item(hilite_item);
}


void lobby_down(void) {
  wait_vsync_int();
  deselect_lobby_item(hilite_item);
  anim_col = 30;
  if (hilite_item < playercount)
    hilite_item++;
  select_lobby_item(hilite_item);
}

void select_lobby_item(u8 item) {
  anim_col = -31;
  anim_col_grad = -8;
  
  if (item == 0)
    set_text16_colour(10, RGB16(31,30,30));
  else
    set_text16_colour(item-1, get_lobby_colour(item-1));
}

void deselect_lobby_item(u8 item) {
  if (item == 0)
    set_text16_colour(10, RGB16(0,30,30));
  else
    set_text16_colour(item-1, get_lobby_colour(item-1));
}



void lobby_left(void) {
  if (hilite_item == 0) {
    if (is_hosting()) {
      if (playercount > 2) {
        playercount--;
        PlaySoundFX(SND_MENU);
      }
    }
  } else if (is_hosting()) {
/*    if (IS_CPU(hilite_item-1)) 
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
*/
  }
  update_lobby();
  
}

void lobby_right(void) {
  if (hilite_item == 0) {
    if (is_hosting()) {
      if (playercount < 8) {
        playercount++;
        PlaySoundFX(SND_MENU);
      }
    }
  } else if (is_hosting()) {
    /*
    int level = players[hilite_item-1].plyr_type>>4;
    if (level < 8) {
      level++;
      players[hilite_item-1].plyr_type = PLYR_CPU | (level<<4);
    }
    PlaySoundFX(SND_MENU);
    */
  }
  update_lobby();
}

void lobby_l(void) {
  // change colour
  if (hilite_item ==0)
    return;
  if (players[hilite_item-1].colour < 8) {
    players[hilite_item-1].colour++;
  }
  else {
    players[hilite_item-1].colour = 0;
  }
  update_lobby();
}

void lobby_r(void) {
  // change image
  if (hilite_item ==0)
    return;
  if (players[hilite_item-1].image < 7) {
    players[hilite_item-1].image++;
  }
  else {
    players[hilite_item-1].image = 0;
  }
  update_lobby();
}


void lobby_accept(void) {
  
  if (hilite_item == 0) {
    // A prssed on the "How many players?" bit
    // reset the players!  
  } else 
  if (hilite_item == 1) {
    // go to the edit name screen
    current_player = hilite_item -1;
    fade_down();
    show_editname();
    fade_up();
  }
  
    
}

void leave_lobby(void) {
  init_players();
  current_player = 0;
  round_count = 0;
  // check if we need to show game menu...
  // a bit different for online play
  // first, assume only 1 LOCAL per client
  
  // Show the spell casting only for this local
  // then, for everyone else, wait to get there spells
  // problem - async spell arrival
  // poll constantly for messages?
    
  //if (IS_CPU(0)) 
  //  current_player = get_next_human(0);
  //else
  //  current_player = 0;
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
  
}

int total_ready(void) {
  // get how many players are ready
  int r = 0;
  int i;
  for (i = 0; i < playercount; i++) {
    if (players[i].plyr_type == PLYR_HUMAN)
      r++;
  }
  return r;
}


void tidy_players(void) {
  // remove disconnected players... 
  int i;
  for (i = 0; i < playercount; i++) {
    if (players[i].plyr_type != PLYR_HUMAN) {
      players[i].modifier_flag |= 0x10;
    }
    trim_whitespace(players[i].name);
  }
    
  
}

void lobby_start(void) {
  PlaySoundFX(SND_CHOSEN);
  if (is_connected() && !is_hosting()) {
    // send a server message!
#ifdef DEBUG_NET
    printf("I'm client, send host my wizard: %d %d %s\n", players[0].image, players[0].colour,
        players[0].name); 
#endif
    send_wizard(players[0].image, players[0].colour, players[0].name);
    // right, now wait for the wizards...
    print_text16("WAITING...", 1,20,10);
    int message = get_server_wizards();
    // check we receive the player count
    if (message == NET_ERROR) {
      return;
    } else {
      // wow, that is it, start spell round...
      leave_lobby();
    }
  }
  update_spellset();
  if (is_hosting() ) {
    // right.. if we are hosting, need to initialise the players...
    // however, to avoid passing all the data across the nw,
    // tell each client to init their own players, which will
    // be a copy of the players here, OK?

    // need a routine to tidy up lost connections/"underbooking"
    if (total_ready() < 2) {
      return;
    }
    tidy_players();
    
#ifdef DEBUG_NET
    printf("Server, tell clients about wizards\n");
#endif
    tell_clients_init();

    int i;
    for (i = 1; i < playercount; i++) {
      players[i].plyr_type = PLYR_REMOTE; 
    } 
    players[0].plyr_type = PLYR_LOCAL; 

    reset_spellwaiting(); 
#ifdef DEBUG_NET
    printf("Host told clients to init. Here are host wizards:\n");
    for (i = 0; i < playercount; i++) {
      if ( players[i].plyr_type != PLYR_NC) { 
        printf("player %d #%s# col %d img %d type %x\n", i, players[i].name, 
            players[i].colour, players[i].image, players[i].plyr_type); 
      }
    }
#endif

    leave_lobby();

  }  
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
int requires_update = 0;
int anim_lobby_count = 0;
void animate_lobby_screen(void){
  if (hilite_item == 0) {
    anim_selection(10, 0, 31, 31);
  }
  else {
    u16 col = get_lobby_colour(hilite_item-1);
    anim_selection(hilite_item-1, GetRed(col), GetBlue(col), GetGreen(col));
  }
  if (requires_update || anim_lobby_count == 10) {
    update_lobby();
    requires_update = 0;
    anim_lobby_count = 0;
  }
  anim_lobby_count++; 
}


// this should change to create players with no random names.
// each client tells us what to show
// create 1 randomly (us) and 7 others "empty" and disabled
// if someone drops out, make that person "dead"
// when someone connects, they should have the same playerid
// as the connection id
const char * const ipConnect = "------";
const char * const clearIpConnect = "       ";
void create_default_lobby(void) {
  int i, tmp; 
  for (i = 0; i < 8; i++) {
    if (i == 0) {
      tmp = GetRand(22);
      strcpy(players[i].name , namesData[tmp]);
      trim_whitespace(players[i].name);

      tmp = GetRand(8);
      players[i].image = tmp;
      
      tmp = GetRand(9);
      players[i].colour = tmp; //chaos_cols[colours[i]-1];
      
      players[i].modifier_flag = 0;
      players[i].plyr_type = PLYR_HUMAN; 
    } else {
      strcpy(players[i].name , ipConnect);
      players[i].plyr_type = PLYR_NC;
    } 
  }
}


void update_lobby(void) {
  // use the global variables to update the amount of players shown...
  u8 i;
  char str[30];
  wait_vsync_int();
  clear_arena(); 
  // clear the arena and redraw it...
  for (i = 0; i < 8; i++) {
    // draw a wizard at this screen position
    if (i < playercount) {
      if (players[i].plyr_type != PLYR_NC) {
        set_player_col(i, chaos_cols[players[i].colour]);
        draw_wizard8(0,(i<<1)+PLAYER_WIZ_Y,players[i].image,0,i);
      }
      print_text16(players[i].name, 6,3+i*2, i);
      //trim_whitespace(players[i].name);
      set_text16_colour(i, get_lobby_colour (i));
      if (IS_CPU(i)) {
        strcpy(str, "Computer ");
        char istr[3];
        int level = players[i].plyr_type>>4;
        int2a(level, istr, 10);
        strcat(str, istr);
        print_text16(str, 16,3+i*2, i);
      }
      else {
        if (players[i].plyr_type == PLYR_NC) { 
	  if (client_online(i)) {
            print_text16("Connected ", 16,3+i*2, i);
	  } else {
            print_text16("Offline   ", 16,3+i*2, i);
	  }
	}
        else
          print_text16("READY     ", 16,3+i*2, i);
      }
      
      
    } else {
      set_text16_colour(i, RGB16(0,0,0));
#if defined(__WINDOWS__)
      clear_square8(0,(i<<1)+PLAYER_WIZ_Y);
#endif
    }
    
  }
  set_text16_colour(10, RGB16(0,30,30));
  if (is_hosting()) { 
    int2a(playercount, str, 10);
    print_text16(str, 22,1, 10);
  }
}

void  set_lobby_player(int clientid, u8 * g_clientMessage) {
  // server... the message arrives saying someone connected with a player id
  // NB glient message already points to DATA_INDEX
  int index = 0;
  int plyr = clientid;
  players[plyr].image = g_clientMessage[index++]; 
  players[plyr].colour = g_clientMessage[index++]; 

//  strcpy( players[plyr].name,  clearIpConnect);
  strcpy( players[plyr].name, (char*)&g_clientMessage[index] );
  int len = strlen(players[plyr].name);
  if (len < 6) {
    int i;
    for ( i = len; i < 7; i++)
        players[plyr].name[i] = ' ';
    players[plyr].name[i] = 0;
  }
  players[plyr].plyr_type = PLYR_HUMAN;
  // don't call gfx routines in seperate threads!
  // works on windows, but X complains about async updates
#ifdef DEBUG_NET
  printf("%s receives new wiz in lobby\n", is_hosting()?"Host":"Client");
#endif
  requires_update = 1;
}
// NETWORK
#endif

#endif // __WINDOWS__

