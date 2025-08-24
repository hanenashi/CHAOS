#include "platform.h"
#ifdef __GBA__
#include "sound.h"
#include "screenmode.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#include "lobby.h"
#include "network.h"
#include "options.h"
#endif
#include "sound_data.h"
#include "editname.h"
#include "chaos.h"
#include "gfx.h"
#include "wizards.h"
#include "text16.h"
#include "string.h"
#include "players.h"

char name_string[12];
void select_letter(u8 letter);
// convert a hilite index to the lette and x, y position
void index_to_position(u8 letter, u8 * x, u8 *y, char * c);
void update_name(void);

void show_editname(void) {
  
  // the edit name screen... 
  current_screen = SCR_EDIT_NAME;
  clear_bg();
  clear_palettes();
  strcpy(name_string, players[current_player].name);
  hilite_item = 71;
  
  // draw the alphabet...
  update_name();
  
  print_text16("A B C D E F G H I J K L M", 2,6, 10);
  print_text16("N O P Q R S T U V W X Y Z", 2,8, 10);
  print_text16("                         ", 2,10, 10);
  print_text16("a b c d e f g h i j k l m", 2,12, 10);
  print_text16("n o p q r s t u v w x y z", 2,14, 10);
  print_text16("OK", 15,16, 13);
  
  set_text16_colour(10, RGB16(0,30,30)); // l blue
  set_text16_colour(11, RGB16(31,30,30)); // white
  set_text16_colour(12, RGB16(31,30,0)); // yellow
  set_text16_colour(13, RGB16(31,30,30)); // white
  
  select_letter(hilite_item);
  
  draw_decor_border(15, RGB16(0,0,31),RGB16(0,31,31)); 
}

void index_to_position(u8 letter, u8 * x, u8 *y, char * c) {
  // 0 - 26 = A - Z
  // 27 - 52 = a - z
  // position = A = (2,6) B = (4,6)... M = 26,6
  // i.e. A-M = (letter - startx * 2), 6
  if (letter <= 12) {
    *x = (letter)<<1;
    *y = 6;
    *c = 'A'+letter;
  }
  if (letter  >= 13 && letter <= 25) {
    *x = (letter - 13)<<1;
    *y = 8;
    *c = 'N'+letter-13;
  }
  if (letter  >= 26 && letter <= 38) {
    *x = (letter - 26)<<1;
    *y = 10;
    *c = '_';
  }
  
  if (letter  >= 39 && letter <= 51) {
    *x = (letter - 39)<<1;
    *y = 12;
    *c = 'a'+letter-39;
  }
  if (letter  >= 52 && letter <= 64) {
    *x = (letter - 52)<<1;
    *y = 14;
    *c = 'n'+letter-52;
  }
  *x += 2;
}

  

void select_letter(u8 letter) {
  if (letter > 64) {
    // OK hilited
    set_text16_colour(13, RGB16(0,30,0)); // white
  } else {
    u8 x = 2;
    u8 y = 6;
    char c;
    index_to_position(letter, &x, &y, &c);
    char str[3];
    str[0] = c;
    str[1] = 0;
//    sprintf(str, "%c", c);
    print_text16(str, x,y, 11);
  }
  
}

void deselect_letter(u8 letter) {
  
  if (letter > 64) {
    // OK hilited
    set_text16_colour(13, RGB16(31,30,30)); // white
  }
  else {
    u8 x = 2;
    u8 y = 6;
    char c;
    index_to_position(letter, &x, &y, &c);
    char str[3];
    if (c == '_')
      c = ' ';
//    sprintf(str, "%c", c);
    str[0] = c;
    str[1] = 0;
    print_text16(str, x,y, 10);
  }
  
}

void edit_name_up(void){
  // move up a row...
  deselect_letter(hilite_item);
  if (hilite_item > 12) {
    hilite_item -= 13;
  }
  else {
    hilite_item = 71;
  }
  select_letter(hilite_item);
  
}

  
void edit_name_down(void){
  // move down a row...
  deselect_letter(hilite_item);
  if (hilite_item < 52) {
    hilite_item += 13;
  }
  else {
    //hilite_item -= 13*4;
    // select OK
    hilite_item = 71;
  }
  select_letter(hilite_item);
  
}

void edit_name_left(void){
  
  deselect_letter(hilite_item);
  
  // get the multiple of 13..
  u8 this_row_index = hilite_item; 
  while (this_row_index - 13 >= 0) {
    this_row_index -= 13;
  }
  
  if (this_row_index > 0) {
    // not on the zeroth column
    hilite_item --;
  }
  else {
    // on the zeroth col, so move to the end of this row
    hilite_item += 12;
  }
  select_letter(hilite_item);
}

void edit_name_right(void){
  deselect_letter(hilite_item);
  
  // get the multiple of 12..
  u8 this_row_index = hilite_item; 
  while (this_row_index - 13 >= 0) {
    this_row_index -= 13;
  }
  
  if (this_row_index < 12) {
    hilite_item++;
  }
  else {
    hilite_item -= 12;
  }
  select_letter(hilite_item);
}

void edit_name_a(void){
  // add the currently selected character to the name
  if (hilite_item > 64) {
    // OK hilited...
    edit_name_start();
    
  }
  else {
    u8 x;
    u8 y;
    char c;
    index_to_position(hilite_item, &x, &y, &c);
    if (c == '_')
      c = ' ';
    u8 len = strlen(name_string);
    if (len < 11) {
      name_string[len] = c;
      name_string[len+1] = 0;
//      sprintf(name_string, "%s%c", name_string, c);
      PlaySoundFX(SND_MENU);
    }
    update_name();
  }
  
}


void edit_name_start(void) {
  trim_whitespace(name_string);
  u8 len = strlen(name_string);
  if (len > 0) {
    // name is valid
    PlaySoundFX(SND_SPELLSUCCESS);
    strcpy(players[current_player].name, name_string);
    fade_down();
#ifdef __WINDOWS__
    if (ONLINE_MODE)
      redraw_lobby();
    else
#endif
    redraw_create_players();
    fade_up();
  }
}


void edit_name_b(void){
  // delete a letter...
  
  u8 len = strlen(name_string);
  if (len > 0) {
    name_string[len-1] = 0;
    PlaySoundFX(SND_CHOSEN);
  }
  update_name();
}

void update_name(void) {
  wait_vsync_int();
  print_text16("             ", 4,2, 12);
  print_text16(name_string, 4,2, 12);
  u8 len = strlen(name_string);
  if (len < 11) {
    print_text16("_", 4+len,2, 12);
  }
  
}
