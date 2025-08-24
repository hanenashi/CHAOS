
#ifndef wmhandler_h_seen
#define wmhandler_h_seen

#include "SDL_events.h"

extern int isFullScreen;
void do_resize(SDL_ResizeEvent * event);
void toggleFullScreen(void) ;
void setNewScale(int newScale);
void removeSDLTimer(int , void * );

#endif 
