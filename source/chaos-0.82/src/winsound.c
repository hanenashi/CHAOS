#include "platform.h"
#ifdef __WINDOWS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"
//#include <windows.h>
#include "sound_data.h"



#define NUM_SOUNDS 2
struct sample {
    const unsigned char *data;
    Uint32 dpos;
    Uint32 dlen;
} sounds[NUM_SOUNDS];

void mixaudio(void *unused, Uint8 *stream, int len)
{
#ifndef NO_SOUND
    int i;
    Sint32 amount;

    for ( i=0; i<NUM_SOUNDS; ++i ) {
        amount = (sounds[i].dlen-sounds[i].dpos);
        if ( amount > len ) {
            amount = len;
        }
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);
        sounds[i].dpos += amount;
    }
#endif
}

void init_wav(int nSample)
{
#ifndef NO_SOUND
  int index;

  /* Look for an empty (or finished) sound slot */
  for ( index=0; index<NUM_SOUNDS; ++index ) {
    if ( sounds[index].dpos == sounds[index].dlen ) {
      break;
    }
  }
  if ( index == NUM_SOUNDS )
    return;

  SDL_LockAudio();
  sounds[index].data = theSoundMap[nSample]->pSample;
  sounds[index].dlen = SAMPLE_LENGTH(nSample);
  sounds[index].dpos = 0;
  SDL_UnlockAudio();
#endif
}
int g_soundOK;

void mixaudio(void *unused, Uint8 *stream, int len);
int init_sdl_sound(void) {
#ifndef NO_SOUND
  SDL_AudioSpec fmt;

  /* Set 16-bit stereo audio at 16Khz */
  fmt.freq = 16000;
  fmt.format = AUDIO_S8;
  fmt.channels = 1;
  fmt.samples = 512;        /* A good value for games */
  fmt.callback = mixaudio;
  fmt.userdata = NULL;

  /* Open the audio device and start playing sound! */
  if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
    g_soundOK = 0;
    return g_soundOK;
  }
  g_soundOK = 1;
  return g_soundOK;
#endif
}

void PlaySoundFX(int soundid) {
#ifndef NO_SOUND
  if (g_soundOK) {
    SDL_PauseAudio(1);
    init_wav(soundid);

    SDL_PauseAudio(0);
  }
#endif
}
#endif

