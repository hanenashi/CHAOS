#include "platform.h"

#ifdef __WINDOWS__
#include <stdlib.h>
#include "SDL.h"
#include "winkeys.h"
#include "wmhandler.h"
#include "options.h"
// "emulates" the keypad
#define KEY_COUNT       10

int g_winkeys[KEY_COUNT];
int g_last_winkeys[KEY_COUNT];
const SDLKey g_keyCodes[KEY_COUNT] = {
  SDLK_d,      // KEY_A 	  
  SDLK_f,      // KEY_B 	  
  SDLK_SPACE,  // KEY_SELECT
  SDLK_RETURN, // KEY_START 
  SDLK_RIGHT,  // KEY_RIGHT 
  SDLK_LEFT,   // KEY_LEFT  
  SDLK_UP,     // KEY_UP 	  
  SDLK_DOWN,   // KEY_DOWN  
  SDLK_a,      // KEY_R	  
  SDLK_s,      // KEY_L 	  
};

void winKeyEvent(SDL_KeyboardEvent * event)
{
  int i;
  if (event->type == SDL_KEYDOWN) {
    // set in key array  
    for (i = 0; i < KEY_COUNT; i++) {
      if (g_keyCodes[i] == event->keysym.sym) {
        g_winkeys[i] = 1;
      }
    }
  } else if (event->type == SDL_KEYUP) {  
    // unset in key array
    for (i = 0; i < KEY_COUNT; i++) {
      if (g_keyCodes[i] == event->keysym.sym) {
        g_winkeys[i] = 0;
      }
    }
  }
}

void InitKeys(void){
  int i;
  for (i = 0; i < KEY_COUNT; i++) {
    g_winkeys[i] = 0;
    g_last_winkeys[i] = 0;
  }
}

void UpdateKeys(void){
  SDL_Event event;
  
  int i;
  for (i = 0; i < KEY_COUNT; i++) {
    g_last_winkeys[i] = g_winkeys[i];
  }
  /* Check for events */
  while (SDL_PollEvent (&event))
  {
    switch (event.type)
    {
      case SDL_KEYUP:
	// handle window manager keys...
        if ((event.key.keysym.sym == SDLK_F4 
            && SDL_GetModState() == KMOD_LALT)
	    ||event.key.keysym.sym == SDLK_ESCAPE  
	    ) {
          Options[OPT_RESET] = 1;
	}
        if (event.key.keysym.sym == SDLK_F5 ) {
	  toggleFullScreen();
	}
        if (event.key.keysym.sym >= SDLK_1
	    && event.key.keysym.sym <= SDLK_4) {
	  setNewScale(event.key.keysym.sym - SDLK_1 + 1);
	}
	/* FALL THROUGH */
      case SDL_KEYDOWN:
        winKeyEvent(&event.key);
        break;
      case SDL_QUIT:
        Options[OPT_RESET] = 1;
        break;
      case SDL_VIDEORESIZE:
	do_resize(&event.resize);
      default:
        break;
    }
  }
  // a hacky way to exit this, but better than nothing
  if (Options[OPT_RESET])
    exit(0);
}
int KeyPressed(unsigned int key){
  if ((key <= KEY_L))
    return (g_winkeys[key] != 0);
  return 0;
}
u32 repeated[KEY_COUNT];
int KeyPressedNoBounce(unsigned int key, unsigned int r){
  if (KeyPressed(key)) {
    // if the recent poll has the key pressed
    if (!g_last_winkeys[key] || (g_last_winkeys[key] && r < repeated[key])) { 
      // and it wasn't pressed last time..
      repeated[key] = 0;
      return 1;
    }
    else if (g_last_winkeys[key]) {
      repeated[key]++;
    }
  }
  return 0;
}


#endif
