
#include "platform.h"

#include "chaos.h"
#include "arena.h"
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


void InterruptProcess(void) {
 
  REG_IME = 0x00; // Disable interrupts 
  int Int_Flag = REG_IF; // Read the interrupt flags
  
  if (Int_Flag & INT_TIMER0) {
    Timer0UpdateSound();
  } 
  if (Int_Flag & INT_VBLANK) {
    game_frames++;
    
//  }
//  if (Int_Flag & INT_TIMMER2) {
    if (current_screen == SCR_EXAMINE_BOARD 
        || current_screen == SCR_CASTING || current_screen == SCR_MOVEMENT) {
      countdown_anim();
    }
    // let the bios know that vblank occurred
    (*(volatile u32*)0x03fffff8) = INT_VBLANK;  
  }  
  if (isSleepMode && (Int_Flag & INT_KEYBOARD) == INT_KEYBOARD) {
    Sleep_Mode(0);
  }
    
  REG_IME = 0x1;					// Enable interrupts
  REG_IF = Int_Flag;		// Write back the interrupt flags  
}
#elif defined(__WINDOWS__)
#include "SDL.h"
// note that these 2 do nothing "dangerous" like access screen ram 
// so it's OK if they live in a seperate thread
Uint32 callback_vblank(Uint32 interval, void* param) {
//  if (int_main) {
    game_frames++;
  //}
  return interval;
}
Uint32 callback_timer2(Uint32 interval, void* param) {
  if (int_main) {
    if (current_screen == SCR_EXAMINE_BOARD ||
	current_screen == SCR_CASTING || current_screen == SCR_MOVEMENT) 
    {
      countdown_anim();
    }
  }
  return interval;
}
#endif

