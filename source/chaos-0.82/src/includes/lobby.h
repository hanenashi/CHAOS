
#ifndef lobby_h_seen
#define lobby_h_seen


void show_create_lobby(void);
void animate_lobby_screen(void);
void redraw_lobby(void);

void lobby_up(void);
void lobby_down(void);
void lobby_left(void);
void lobby_right(void);
void lobby_l(void);
void lobby_r(void);

void lobby_accept(void);
void lobby_start(void);
void set_lobby_player(int clientid, u8 * g_clientMessage);

#endif
