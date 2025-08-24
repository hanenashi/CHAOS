// players.h

#ifndef PLAYERS_H
#define PLAYERS_H

extern int hilite_wizard_item;
extern const char * const namesData[];

void show_create_players(void);
void redraw_create_players(void);
void create_players_up(void);
void create_players_down(void);
void create_players_left(void);
void create_players_right(void);
void create_players_l(void);
void create_players_r(void);

void create_players_accept(void);
void create_players_start(void);

void animate_player_screen(void);

unsigned char get_next_human(unsigned char id);

#endif // PLAYERS_H



