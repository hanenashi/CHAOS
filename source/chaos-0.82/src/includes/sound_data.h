#ifndef _sound_data_h
#define _sound_data_h

#include "platform.h"
#ifdef __GBA__
#include "sound.h"
#elif defined(__WINDOWS__)
#include "winsound.h"
#define SAMPLE_LENGTH(n) (theSoundMap[(n)]->pSampleEnd - theSoundMap[(n)]->pSample)
#endif

#define SND_ATTACK 0
#define SND_BEAM 1
#define SND_CHOSEN 2
#define SND_CPUSTART 3
#define SND_ELECTRO 4
#define SND_FIRE 5
#define SND_GOOEY 6
#define SND_JUSTICE 7
#define SND_MENU 8
#define SND_RANGE 9
#define SND_SCREAM 10
#define SND_SPELLSTEP 11
#define SND_SPELLSUCCESS 12
#define SND_URGH 13
#define SND_WALK 14

#if defined (__GBA__) || !defined (WIN32)
#define binary_attack_raw_start _binary_attack_raw_start 
#define binary_attack_raw_end _binary_attack_raw_end 
#define binary_beam_raw_start _binary_beam_raw_start 
#define binary_beam_raw_end _binary_beam_raw_end 
#define binary_chosen_raw_start _binary_chosen_raw_start 
#define binary_chosen_raw_end _binary_chosen_raw_end 
#define binary_cpustart_raw_start _binary_cpustart_raw_start 
#define binary_cpustart_raw_end _binary_cpustart_raw_end 
#define binary_electro_raw_start _binary_electro_raw_start 
#define binary_electro_raw_end _binary_electro_raw_end 
#define binary_fire_raw_start _binary_fire_raw_start 
#define binary_fire_raw_end _binary_fire_raw_end 
#define binary_gooey_raw_start _binary_gooey_raw_start 
#define binary_gooey_raw_end _binary_gooey_raw_end 
#define binary_justice_raw_start _binary_justice_raw_start 
#define binary_justice_raw_end _binary_justice_raw_end 
#define binary_menu_raw_start _binary_menu_raw_start 
#define binary_menu_raw_end _binary_menu_raw_end 
#define binary_range_raw_start _binary_range_raw_start 
#define binary_range_raw_end _binary_range_raw_end 
#define binary_scream_raw_start _binary_scream_raw_start 
#define binary_scream_raw_end _binary_scream_raw_end 
#define binary_spellstep_raw_start _binary_spellstep_raw_start 
#define binary_spellstep_raw_end _binary_spellstep_raw_end 
#define binary_spellsuccess_raw_start _binary_spellsuccess_raw_start 
#define binary_spellsuccess_raw_end _binary_spellsuccess_raw_end 
#define binary_urgh_raw_start _binary_urgh_raw_start 
#define binary_urgh_raw_end _binary_urgh_raw_end 
#define binary_walk_raw_start _binary_walk_raw_start 
#define binary_walk_raw_end _binary_walk_raw_end
#endif

extern const soundtype_t binary_attack_raw_start[];
extern const soundtype_t binary_attack_raw_end[];
extern const soundtype_t binary_beam_raw_start[];
extern const soundtype_t binary_beam_raw_end[];
extern const soundtype_t binary_chosen_raw_start[];
extern const soundtype_t binary_chosen_raw_end[];
extern const soundtype_t binary_cpustart_raw_start[];
extern const soundtype_t binary_cpustart_raw_end[];
extern const soundtype_t binary_electro_raw_start[];
extern const soundtype_t binary_electro_raw_end[];
extern const soundtype_t binary_fire_raw_start[];
extern const soundtype_t binary_fire_raw_end[];
extern const soundtype_t binary_gooey_raw_start[];
extern const soundtype_t binary_gooey_raw_end[];
extern const soundtype_t binary_justice_raw_start[];
extern const soundtype_t binary_justice_raw_end[];
extern const soundtype_t binary_menu_raw_start[];
extern const soundtype_t binary_menu_raw_end[];
extern const soundtype_t binary_range_raw_start[];
extern const soundtype_t binary_range_raw_end[];
extern const soundtype_t binary_scream_raw_start[];
extern const soundtype_t binary_scream_raw_end[];
extern const soundtype_t binary_spellstep_raw_start[];
extern const soundtype_t binary_spellstep_raw_end[];
extern const soundtype_t binary_spellsuccess_raw_start[];
extern const soundtype_t binary_spellsuccess_raw_end[];
extern const soundtype_t binary_urgh_raw_start[];
extern const soundtype_t binary_urgh_raw_end[];
extern const soundtype_t binary_walk_raw_start[];
extern const soundtype_t binary_walk_raw_end[];
extern const SoundMap * const theSoundMap[];
 
#endif //sound_data_h
 
