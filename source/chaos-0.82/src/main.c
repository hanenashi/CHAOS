//standard headers and library calls
#include "platform.h"
#ifdef __GBA__
#include"gba.h"
#include"screenmode.h"
#include"interupts.h"
#include"sprite.h"
#include"dma.h"
#include"sleep.h"
#include"backgrounds.h"
#include"timers.h"
#include "reset.h"
#include "sound.h"
#include "sound_data.h"
// graphics...
#include"text16.h"
#endif

#ifdef __WINDOWS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#ifdef NETWORK
#include "SDL_net.h"
#endif
#ifdef WIN32
#include <windows.h>
#endif
#include "winsound.h"
#include "winkeys.h"
#include "lobby.h"
#include "network.h"
#include "wmhandler.h"
#include "cmdline.h"
#endif

// specific includes
#include"spelldata.h"
#include "gfx.h"
#include "creature.h"
#include "chaos_sprites.h"
#include "arena.h"
#include "players.h"
#include "chaos.h"
#include "input.h"
#include "options.h"
#include "wizards.h"
#include "splash.h"
#include "spellselect.h"

void Setup(void);
void InterruptProcess(void);
void LoadBG(void);
//void init_interrupts(void);

#ifdef __WINDOWS__
int  initialiseSDL(void);
SDL_TimerID g_vblankTimerId;
SDL_TimerID g_timer2TimerId;
int int_main = 0;
int g_icon = SPELL_GOOEY_BLOB;
#endif
u8 game_frames;

#ifdef __WINDOWS__
int
main (int argc, char * argv[])
#elif defined( __GBA__)
int main(void)
//int AgbMain(void)
#endif
{
#ifdef __GBA__
  clear_vram();
  screen_white(16);
  Setup();
  LoadBG();
  InitKeys();
  init_interrupts(INT_VBLANK, InterruptProcess);
#endif
  set_default_options();
#ifdef __WINDOWS__
  if (parse_opts(argc, argv)) {
    // some weird option that we don't support.
    return 0;
  }
#endif
#ifdef __WINDOWS__
  int exitCode = initialiseSDL();
  if (exitCode) {
    return exitCode;
  }
  InitKeys();
  LoadBG();
#ifdef NETWORK
  if (Options[OPT_NET] == BE_HOST) {
    start_host();
    connect_to(NET_HOSTNAME);
  } 
  if (Options[OPT_NET] == CONNECT_TO) {
    connect_to(serverName);
  } 
#endif
   
#endif
  load_sprite_palette();
  reset_arena_tables();
  reset_players();
  show_splash();
#ifdef __GBA__
  screen_white_in();
#else
  fade_up();
#endif
  while(Options[OPT_RESET] == 0) {  
    wait_vsync_int();
    handle_keys();
    if ( game_frames & 2 ){
      if (current_screen == SCR_SPLASH) {
        ChurnRand(); // make sure we are more random
        animate_splash_screen();
      }
      else if (current_screen == SCR_OPTIONS)
        animate_options();
      else if (current_screen == SCR_CREATE_PLAYERS)
        animate_player_screen();
#if defined( __WINDOWS__ ) && defined(NETWORK)
      else if (current_screen == SCR_CREATE_LOBBY)
        animate_lobby_screen();        
#endif
      else if (current_screen == SCR_SELECT_SPELL)
        anim_spell_select();
        
      if (current_screen == SCR_EXAMINE_BOARD || 
	  current_screen == SCR_CASTING || 
	  current_screen == SCR_MOVEMENT) 
      {
        animate_arena();
      }
    } 
  }
#if defined(__GBA__)  
  REG_IME = 0x00;
  reset_gba();
#endif
  return 0;
}


#if defined(__GBA__)  
#if 0
void init_interrupts(void) {
  
  REG_IME = 0x00;                       // Disable interrupts 
  REG_INTERUPT = (u32)InterruptProcess;       // Set interrupt proc address (IRQHandler) 
  //REG_IE = INT_TIMMER2 | INT_VBLANK;
  REG_IE = INT_VBLANK;
  REG_IF = 0x00;
  REG_DISPSTAT = BIT03;                 // Enable Display V-Blank IRQ also.
//  REG_TM2CNT = TIME_FREQUENCY_64 |TIME_ENABLE| TIME_IRQ_ENABLE;
  REG_IME = 0x01;
  game_frames = 0;
}
#endif

void Setup(void) {
  SetMode(MODE_1 |OBJ_ENABLE | OBJ_MAP_1D); //set mode 0
  
  // initialise all the sprites to 0
  u16 loop;
  for(loop = 0; loop < 4; loop++)  {
    sprites[loop].attribute0 = 160;  //y to > 159
    sprites[loop].attribute1 = 240;  //x to > 239
  }
  unsigned short int* temp;
  temp = (unsigned short int*)sprites;
  for(loop = 0; loop < 128*4; loop++) {
    OAM[loop] = temp[1];
  }
  

  // create a bg
  bg_level.number = 0;				//background number 0-2
  bg_level.charBaseBlock = 0;
  bg_level.screenBaseBlock = 28; // 31 is the largest base block for the map data
  bg_level.colorMode = BG_COLOR_16;
  bg_level.size = TEXTBG_SIZE_512x256; 
  bg_level.mosiac = 0;
  bg_level.x_scroll = 0;
  bg_level.y_scroll = 0;
  EnableBackground(&bg_level);
  REG_BG0CNT &= 0xFFFD;
  REG_BG0CNT |= 2;
  UpdateBackground(&bg_level);
  
  
  bg_text.number = 1;				//background number 0-2
  bg_text.charBaseBlock = 0;
  bg_text.screenBaseBlock = 30; // 31 is the largest base block for the map data
  bg_text.colorMode = BG_COLOR_16;
  bg_text.size = TEXTBG_SIZE_256x256; 
  bg_text.mosiac = 0;
  bg_text.x_scroll = 0;
  bg_text.y_scroll = 0;
  EnableBackground(&bg_text);
  REG_BG1CNT &= 0xFFFD;
  REG_BG1CNT |= 1;
  UpdateBackground(&bg_text);
  
}

#endif

void remove_sprite(int);
void LoadBG(void) {
  load_all_palettes();
#if defined(__GBA__)  
  setup_text16(&bg_text, 605);
  int i;
  for (i = 0; i < 128; ++i)
  {
    remove_sprite(i);
  }
#endif
}


//void VblankIntr(void) {
//  game_frames++;
//    
//  // let the bios know that vblank occurred
//  (*(volatile u32*)0x03fffff8) = INT_VBLANK;  
//}
//
//void Timer2Intr(void){}
//void JoypadIntr(void){}

// GBA specific software interrupt (SWI) call to the BIOS..
// pauses the CPU while waiting for the vblank (saves batteries)
//void wait_vsync_int(void) {
//  asm volatile("swi 0x5"); 
//
//}

// delay a given number of vblanks
// 60 waits 1 second.
void delay (u8 time) {
  game_frames = 0; 
#if 1
#ifdef __WINDOWS__
  time++;
  time /= 2;
#endif 
#endif 
  while(game_frames < time) {
    wait_vsync_int();
    if ( game_frames & 2 ){
      if (current_screen == SCR_CREATE_PLAYERS)
        animate_player_screen();
      else if (current_screen == SCR_SELECT_SPELL)
        anim_spell_select();
        
      if (current_screen == SCR_EXAMINE_BOARD 
	  || current_screen == SCR_CASTING 
	  || current_screen == SCR_MOVEMENT)
      {
        animate_arena();
      }
    }
  }
  
}

void disable_interrupts(void) {
#if defined(__GBA__)  
  REG_IME = 0x00; // Disable interrupts 
#elif defined(__WINDOWS__)
  int_main = 0; 
#endif
}

void enable_interrupts(void) {
#if defined(__GBA__)  
  REG_IME = 0x01;
#elif defined(__WINDOWS__)
  int_main = 1; 
#endif
}
#ifdef __WINDOWS__
int initialiseSDL() {
  char msg[256];
  /* Initialize SDL */
  if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_TIMER
#if !defined(NO_SOUND)	
	|SDL_INIT_AUDIO
#endif
//	|SDL_INIT_NOPARACHUTE
	) < 0)
  {
    sprintf (msg, "Couldn't initialize SDL: %s\n", SDL_GetError ());
#if defined(WIN32)
    MessageBox (0, msg, "Error", MB_ICONHAND);
#else
    printf(msg);
#endif
    return 1;
  }

#if defined(NETWORK)
  if (SDLNet_Init () < 0) {
    sprintf (msg, "Couldn't initialize SDLNet: %s\n", SDL_GetError ());
#if defined(WIN32)
    MessageBox (0, msg, "Error", MB_ICONHAND);
#else
    printf(msg);
#endif
    return 1;
  }
#endif
  
  atexit (SDL_Quit);
#ifdef NETWORK
  atexit (SDLNet_Quit);
#endif
  int flags = SDL_SWSURFACE ;
  if (isFullScreen) {
    flags |= SDL_FULLSCREEN;
  } else {
    flags |= SDL_RESIZABLE;
  }
  screen = SDL_SetVideoMode (gfx_scale*WIN_SCRN_X*8,gfx_scale*WIN_SCRN_Y*8, 16, 
      flags);
  SDL_WM_SetIcon(get_icon(g_icon), NULL);
  if (screen == NULL)
  {
    sprintf (msg, "Couldn't set 16 bit video mode: %s\n",
        SDL_GetError ());
#if defined(WIN32)
    MessageBox (0, msg, "Error", MB_ICONHAND);
#else
    printf(msg);
#endif
    return 2;
  }
  SDL_WM_SetCaption ("Chaos", NULL);

#if !defined(NO_SOUND)	
  if (init_sdl_sound() == 0) {
    sprintf (msg, "Couldn't start sound: %s\n",
        SDL_GetError ());
#if defined(WIN32)
    MessageBox (0, msg, "Error", MB_ICONHAND);
#else
    printf(msg);
#endif
  }
#endif
    
  enable_interrupts();
  // runs at 30 frames... hmm..
  g_vblankTimerId =  SDL_AddTimer((33/10)*10, callback_vblank, NULL);
  // "timmer2" is called every 0.25 seconds (??)
  //g_timer2TimerId =  SDL_AddTimer(250, callback_timer2, NULL);
  // timer 2 updates the arena counter - originally called @ 50Hz
  g_timer2TimerId =  SDL_AddTimer(20, callback_timer2, NULL);


#if !defined(WIN32)  
  // where the heck is on_exit on Win32?
  on_exit(removeSDLTimer, g_vblankTimerId);
  on_exit(removeSDLTimer, g_timer2TimerId);
#endif
  return 0;
}

// horribly hacky..
void wait_vsync_int(void){
  // wait for the game_frame to change...
  int tmpGF = game_frames;
  while (tmpGF == game_frames) {
    SDL_Delay(5);
  }
  SDL_Flip(screen);
}
#endif

