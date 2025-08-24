#ifndef remote_h_seen
#define remote_h_seen

// need more "types" of player

// currently there is only human and CPU
// will need:
// human
// cpu
// remote
// local

/*
human: human controlled   
cpu  : cpu controlled
local: like human, but whenever we do certain actions, need to send a message to the server
remote: a player on another machine - human only?
*/

extern int waiting_for_spells;


void tell_clients_init(void);
int get_server_wizards(void);
void do_remote_spell(void);
void do_remote_movement(void);
int remote_dismount(void);
void send_dismount(int yes_pressed);
void send_movement_a(void);
void send_movement_b(void);
void send_movement_start(void);
void send_casting_a(void);
void send_casting_b(void);
void send_spellchoice(int who);
void show_spellwaiting_screen(void);
void receive_spellinfo(int playerid, int spell, int illusion);
int get_next_local(int id) ;
void reset_spellwaiting(void);
void update_spellset(void);
void  client_disconnected(int clientid);
#endif


