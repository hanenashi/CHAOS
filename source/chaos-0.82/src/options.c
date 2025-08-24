// options.c
#include "platform.h"

#ifdef __GBA__
#include "gba.h"
#include "screenmode.h"
#include "sound.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#include "network.h"
#include "cmdline.h"
#endif

#include "sound_data.h"
#include "options.h"
#include "spelldata.h"
#include "chaos.h"
#include "gfx.h"
#include "text16.h"
#include "string.h"
#include "input.h"
#include "splash.h"
#include "creature.h"


#define ON_OFF_OPTION 0
#define NUMBER_OPTION 1
#define NO_OPTION 2


void draw_options (void);
void draw_option (u8 nOpt);

void deselect_option(u8 item);
void select_option(u8 item);


// the options... used to vary in game stuff
typedef struct 
{
    char * cName; // text value of the option on screen
    u8 nType;     // on off, etc
    u8 nMin;      // min value
    u16 nMax;     // max value
} OPTION;


static const OPTION o0  = {"Round Limit",NUMBER_OPTION,0,500};
static const OPTION o1  = {"Old Bugs",ON_OFF_OPTION,0,59};
#ifdef __GBA__
static const OPTION o2  = {"Quit to Pogoshell",NO_OPTION,0,1};
#elif defined( __WINDOWS__)
static const OPTION o2  = {"Quit ",NO_OPTION,0,1};
#endif
static const OPTION o3  = {"Sound Test",NUMBER_OPTION,0,14};
#if defined (__WINDOWS__)
static const OPTION o4  = {"Net Play",NUMBER_OPTION,0,2};
#endif
static const OPTION o5  = {"          BACK",NO_OPTION,0,1};

static const OPTION *OptionTypes[]= {
  &o0,
  &o1,
  &o2,
  &o3,
#if defined (__WINDOWS__)
  &o4,
#endif
  &o5,
};
#if defined (__WINDOWS__)
const char * const netPlayNames[3] = {"OFF    ", "HOST   ", "CONNECT"};
#endif

unsigned int Options[OPTION_COUNT];

void draw_blob(void);


void set_default_options(void) {
  Options[OPT_ROUND_LIMIT] = DEFAULT_ROUNDS;
  Options[OPT_OLD_BUGS] = 1; // on
  Options[OPT_RESET] = 0; // off
}

void show_options(void) {
  
  clear_palettes();
  clear_bg();
  clear_arena();
  anim_col = -31;
  anim_col_grad = -8;
  hilite_item = 0;
  current_screen = SCR_OPTIONS;
  
  draw_blob();
  
  print_text16("GAME OPTIONS", 9, 2, 14);
  draw_options();
  
//  print_text16("Use pad to change options", 2, 15, 14);
//  print_text16("Press B or START to exit", 3, 17, 14);
  
  set_text16_colour(14, RGB16(31,30,0));
  draw_decor_border(15, RGB16(0,0,0),RGB16(31,0,0)); 
  
  select_option(hilite_item);
  
}

u8 blob_frame;
u8 blob_frame_count;
u8 changes_count;
u8 show_creature;
#ifdef __GBA__
#define BLOB_X  12
#define BLOB_Y  7
#else
#define BLOB_X  14
#define BLOB_Y  9
#endif
void draw_blob(void) {
  blob_frame = 1;
  blob_frame_count = 1;
  changes_count = 0;
  show_creature = SPELL_DRAGON_GOLDEN;
  while ((CHAOS_SPELLS.pSpellDataTable[show_creature]->palette > 0
      && CHAOS_SPELLS.pSpellDataTable[show_creature]->palette
                 < (OPTION_COUNT+1))
      || show_creature < SPELL_KING_COBRA)
    show_creature = GetRand(SPELL_WALL+1);
  int palette = CHAOS_SPELLS.pSpellDataTable[show_creature]->palette;
  load_bg_palette(palette,palette);
#ifdef __GBA__
  
  int x, y, i;
  palette <<=12; 
  // mini bug fix to get rid of "echos"...
  bg_level.x_scroll = 0;
  bg_level.y_scroll = 0;
  for (x = 0; x < 32; x++) {
    for (y = 0; y < 32; y++) {
      bg_level.mapData[x+y*32] = 0;
    }
  }
  UpdateBackground(&bg_level);
  
  // need to remove the arena stuff..
  clear_arena();
  x = 1; y = 1; i = 1;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  y++; x--; i+=29;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  
  x = 27; y = 1; i = 25;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  y++; x--; i+=29;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  
  
  x = 1; y = 17; i = 421;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  y++; x--; i+=29;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  
  x = 27; y = 17; i = 445;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
  y++; x--; i+=29;
  bg_level.mapData[x+y*32] = i|palette;
  x++; i++;
  bg_level.mapData[x+y*32] = i|palette;
#else
  set_palette(BLOB_X,0,palette);
  set_palette(BLOB_X,BLOB_Y,palette);
  set_palette(0,BLOB_Y,palette);
  set_palette(0,0,palette);
#endif
  animate_options();
}


void animate_options(void) {
  blob_frame_count--;
  if (blob_frame_count == 0) {
        
    draw_gfx(CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFX,
             CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFXMap,
             BLOB_X, 0,blob_frame );
    draw_gfx(CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFX,
             CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFXMap,
             0, 0,blob_frame );
    draw_gfx(CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFX,
             CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFXMap,
             BLOB_X, BLOB_Y,blob_frame );
    draw_gfx(CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFX,
             CHAOS_SPELLS.pSpellDataTable[show_creature]->pGFXMap,
             0, BLOB_Y,blob_frame );
//    draw_creature(BLOB_X, 0, show_creature,blob_frame);
//    draw_creature(0, 0, show_creature,blob_frame);
//    draw_creature(BLOB_X, BLOB_Y, show_creature,blob_frame);
//    draw_creature(0, BLOB_Y, show_creature,blob_frame);
    
    if (blob_frame == 0 || blob_frame > 3) {
      blob_frame = 3;
    }
    else {
      blob_frame--;
    }
    blob_frame_count = 29;
  }
}

// change this so it uses a scroller...
void draw_options(void) {
  u8 j;
  for (j = 0; j < OPTION_COUNT; j++) { 
    draw_option(j);
    deselect_option(j);
  }
}


void draw_option (u8 nOpt) {
  // used to draw the value of a particular option after changing it...
  u8 x = 3;
  u8 y = 5;
  if (nOpt < OPTION_COUNT) {
    print_text16(OptionTypes[nOpt]->cName, x, y+nOpt*3, nOpt+1);
    if (OptionTypes[nOpt]->nType == ON_OFF_OPTION) {
      if (Options[nOpt])
        print_text16("ON ", x+12, y+nOpt*3, nOpt+1);
      else
        print_text16("OFF", x+12, y+nOpt*3, nOpt+1);
    } else if (OptionTypes[nOpt]->nType == NUMBER_OPTION) {
      // number...
      char str[9];
      if (nOpt == OPT_ROUND_LIMIT && Options[nOpt] < 2) {
        if (Options[nOpt] == 0) {
          strcpy(str, "OFF    ");
        } else if (Options[nOpt] == 1) {
          strcpy(str, "DEFAULT");
        }
#ifdef __WINDOWS__
      } else if (nOpt == OPT_NET) {
	strcpy(str, netPlayNames[Options[nOpt]] );
#endif
      } else {
        int2a(Options[nOpt], str, 10);
        u8 len = strlen(str);
        while (len < 7) {
          str[len++] = ' ';
        }
        str[len] = 0;
      }
      print_text16(str, x+12, y+nOpt*3,nOpt+1);
    }
  } 
    
}

void deselect_option(u8 item) {
  set_text16_colour(item+1, RGB16(0,14,0));
}

void select_option(u8 item) {
  set_text16_colour(item+1, RGB16(0,31,0));
}

void options_a(void) {
  if (hilite_item == 2)  {
    Options[OPT_RESET] = 1;
  }
  
  if (hilite_item == OPT_BACK)  {
    options_back();
  }
  
  if (hilite_item == OPT_SOUND)  {
    PlaySoundFX(Options[OPT_SOUND]);
  }

#if defined( __WINDOWS__ ) && defined(NETWORK)
  if (hilite_item == OPT_NET)  {
    if (Options[OPT_NET] == BE_HOST) {
      // start the host running...
      set_text16_colour(OPT_NET+1, RGB16(31,31,0));
      start_host();
      serverName = (char*)defaultServerName;
    } 
    if (Options[OPT_NET]) {
      // if we are "connect to" then read the server address file
      // that contains the char* value of address to connect to
      // for now, hack in localhost...
      if (serverName == NULL) {
	set_servername();
      }
      if (connect_to(serverName) == -1) {
	// draw the name in red...
	set_text16_colour(OPT_NET+1, RGB16(31,0,0));
      } else {
       set_text16_colour(OPT_NET+1, RGB16(31,31,31));
      }
    }
    
  }
#endif 
}


void options_up(void) {
  
  if (hilite_item > 0)  {
    PlaySoundFX(SND_MENU);
    deselect_option(hilite_item);
    hilite_item--;
    select_option(hilite_item);
  }
  
}

void options_down(void) {
  if (hilite_item < OPTION_COUNT-1)  {
    PlaySoundFX(SND_MENU);
    deselect_option(hilite_item);
    hilite_item++;
    select_option(hilite_item);
  }
}

void options_left(void) {
  if (OptionTypes[hilite_item]->nType == ON_OFF_OPTION) {
    if (!Options[hilite_item]) {
      Options[hilite_item] = 1;
      draw_option(hilite_item);
    }
  } else if (OptionTypes[hilite_item]->nType == NUMBER_OPTION) {
    // number type, decrease
    if (Options[hilite_item] > OptionTypes[hilite_item]->nMin) {
      Options[hilite_item]--;
      draw_option(hilite_item);
    }
    
  }
    
}

void options_right(void) {
  if (OptionTypes[hilite_item]->nType == ON_OFF_OPTION) {
    if (Options[hilite_item]) {
      Options[hilite_item] = 0;
      draw_option(hilite_item);
    }
    
  } else if (OptionTypes[hilite_item]->nType == NUMBER_OPTION) {
    // number type, increase
    if (Options[hilite_item] < OptionTypes[hilite_item]->nMax) {
      Options[hilite_item]++;
      draw_option(hilite_item);
    }
    
  }
  
}


void options_back(void) {
  PlaySoundFX(SND_CHOSEN);
  wait_for_letgo();
  fade_down();
  show_splash();
  fade_up();  
}


