#include "platform.h"
#ifdef __GBA__
#include "screenmode.h"
#endif
#include "chaos.h"
const unsigned short int  chaos_cols[] = {
  RGB16(31,31,31), // white
  RGB16(31,31,0), // yellow
  RGB16(0,31,31), //lightblue
  RGB16(0,31,0), //green
  RGB16(31,0,31), //purple
  RGB16(31,0,0), //red
  RGB16(0,0,31), //blue
  RGB16(20,20,20), //grey
  RGB16(20,20,0), // mustard
};

// on the player select screen, the colours are
/*
red purple green lightblue mustard yellow grey white
*/


const signed char surround_table[8][2] = {
  {-1,-1},
  {-1, 0},
  {-1, 1},
  { 0, 1},
  { 1, 1},
  { 1, 0}, 
  { 1,-1},
  { 0,-1}
};

const unsigned char key_table[8] = {
  'Q','W','E','D','C','X','Z','A'
};



unsigned long int current_screen;
unsigned char hilite_item;
signed char anim_col;
signed char anim_col_grad;
unsigned char current_player;
unsigned char playercount;
unsigned char dead_wizards;

