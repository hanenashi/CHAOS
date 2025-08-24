// movement.h

#ifndef MOVEMENT_H
#define MOVEMENT_H

extern int g_highlight_creations;
extern unsigned char selected_creature;
extern unsigned char tmp_is_flying;
extern unsigned char tmp_engaged_flag;
extern unsigned char tmp_movement_allowance;
extern unsigned char tmp_range_attack;

void start_movement_round(void);
//void movement_up(void);
//void movement_down(void);
//void movement_left(void);
//void movement_right(void);
void movement_l(void);
void movement_a(void);
void movement_b(void);
void movement_start(void);
void movement_select(void);

void drawn_contest(void);
void win_contest(void);

#endif

