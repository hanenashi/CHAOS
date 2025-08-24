// arena.h 
// the arena is stored in IWRAM and holds references to creatures

#ifndef ARENA_H
#define ARENA_H

#ifdef __GBA__
#include "gba.h"
#endif

#ifdef __GBA__
#define MESSAGE_X   0
#define MESSAGE_Y   18
#elif defined (__WINDOWS__)
#define MESSAGE_X   0
#define MESSAGE_Y   24
#endif

// check bit 7 for movement/spreading
#define HAS_MOVED(n) ((n & 0x80) == 0x80)
#define HAS_SPREAD(n) ((n & 0x80) == 0x80)


extern unsigned char arena[6][160];  // the 6 arena tables... 960 bytes

/*
arena 0 - the creature index
arena 1 - the timer square - when this reaches 0, update the frame in arena 2
arena 2 - the anim frame, 4 is dead.
arena 3 - modifier flag - bits are important
arena 4 - the creature index for creatures under the thing in arena 0
arena 5 - modifier flag for things in arena 4

arena 3 bits
bit 0    | 
bit 1    |    the owner (or player id for wizards)
bit 2    | 
bit 3    is asleep
bit 4    is illusionary (i.e. bit 4 1 = illusion, bit 4 0 = real)
bit 5    has had disbelieve cast on it
bit 6    undead
bit 7    "has been spread" for gooey blob and fire

*/

extern unsigned char round_count; // index into arena of current player
extern unsigned char wizard_index; // index into arena of current player
extern unsigned char start_index;  // index into arena for start square of current spell
extern unsigned char target_index;  // index into arena for target square of current spell
extern signed char world_chaos;  // index into arena for target square of current spell

void move_cursor_left(void);
void move_cursor_right(void);
void move_cursor_up(void);
void move_cursor_down(void);

void countdown_anim(void);
void animate_arena(void);
void clear_message(void);

void get_yx(u16 arena_index, u8 * H, u8 *L);
void get_yx2(u16 arena_index, u8 * H, u8 *L);
void get_yx_upper(unsigned char arena_index, unsigned char *H, unsigned char *L);
// get distance between 2 x,y arena coords
void get_chaosdistance(u8 x1,u8 y1,u8 x2,u8 y2,u16 *diff);
// get distance between 2 arena indices
void get_distance(u16 square1, u16 square2, u16 * distance);
// return a new index based on the old one by applying a surround index value
u8 apply_position_modifier(u8 square, u8 i);

void remove_los_targets(unsigned char index);

void get_cursor_pos(unsigned char * x, unsigned char * y);

void move_cursor_to(u32 x, u32 y);
void move_screen_to(u8 index);

void set_current_player_index(void);
void unset_moved_flags(void);

void destroy_all_creatures(unsigned char playerid);

// random castle crushing...
void destroy_castles(void);
// get a new spell from the wood
void random_new_spell(void);

void reset_arena_tables(void);

void highlight_players_stuff(unsigned char playerid);

void display_arena(void);

// platform specific defines
#ifdef __GBA__
#define SET_CURSOR_POSITION(x,y,bgx,bgy) set_cursor_position(x,y,bgx,bgy)
void set_cursor_position(u32 x, u32 y, s32 bgx, s32 bgy);
#elif defined(__WINDOWS__)
#define SET_CURSOR_POSITION(x,y,bgx,bgy) set_cursor_position(x,y)
void set_cursor_position(u32 x, u32 y);
#endif
void display_cursor_contents(u8 index);
void init_arena_tables(void);

#endif // ARENA_H



