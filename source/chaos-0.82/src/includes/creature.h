// creature.h
#ifndef CREATURE_H
#define CREATURE_H
#ifdef __GBA__
#include "gba.h"
#endif
// bit 6 is undead flag
#define IS_UNDEAD(n) ((n & BIT06) == BIT06)
// bit 4 is illusion
#define IS_ILLUSION(n) ((n & BIT04) == BIT04)
// bit 4 is asleep
#define IS_ASLEEP(n) ((n & BIT03) == BIT03)


void creature_spell_succeeds(unsigned char target);
void draw_all_creatures(void);

void spread_fire_blob(void);
void clear_arena(void);
int GetRand(int i);
void ChurnRand();
void setSeed(unsigned int newSeed);
int GetSeed(void);

#endif // CREATURE_H
