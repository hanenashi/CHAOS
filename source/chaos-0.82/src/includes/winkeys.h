#ifndef _winkeys_h
#define _winkeys_h

#ifdef __WINDOWS__

#include "SDL.h"

#define KEY_A 		0
#define KEY_B 	        1 	
#define KEY_SELECT	2
#define KEY_START 	3
#define KEY_RIGHT 	4
#define KEY_LEFT 	5
#define KEY_UP 		6
#define KEY_DOWN 	7
#define KEY_R		8
#define KEY_L 		9

void winKeyEvent(SDL_KeyboardEvent * event);
void InitKeys(void);
void UpdateKeys(void);
int KeyPressed(unsigned int key);
int KeyPressedNoBounce(unsigned int key, unsigned int r);

#endif //__WINDOWS__

#endif // _winkeys_h
