#include "sound_data.h"

#ifndef NO_SOUND
const static SoundMap SoundMap_1 = {
  binary_attack_raw_start,
  binary_attack_raw_end
};
const static SoundMap SoundMap_2 = {
  binary_beam_raw_start,
  binary_beam_raw_end
};
const static SoundMap SoundMap_3 = {
  binary_chosen_raw_start,
  binary_chosen_raw_end
};
const static SoundMap SoundMap_4 = {
  binary_cpustart_raw_start,
  binary_cpustart_raw_end
};
const static SoundMap SoundMap_5 = {
  binary_electro_raw_start,
  binary_electro_raw_end
};
const static SoundMap SoundMap_6 = {
  binary_fire_raw_start,
  binary_fire_raw_end
};
const static SoundMap SoundMap_7 = {
  binary_gooey_raw_start,
  binary_gooey_raw_end
};
const static SoundMap SoundMap_8 = {
  binary_justice_raw_start,
  binary_justice_raw_end
};
const static SoundMap SoundMap_9 = {
  binary_menu_raw_start,
  binary_menu_raw_end
};
const static SoundMap SoundMap_10 = {
  binary_range_raw_start,
  binary_range_raw_end
};
const static SoundMap SoundMap_11 = {
  binary_scream_raw_start,
  binary_scream_raw_end
};
const static SoundMap SoundMap_12 = {
  binary_spellstep_raw_start,
  binary_spellstep_raw_end
};
const static SoundMap SoundMap_13 = {
  binary_spellsuccess_raw_start,
  binary_spellsuccess_raw_end
};
const static SoundMap SoundMap_14 = {
  binary_urgh_raw_start,
  binary_urgh_raw_end
};
const static SoundMap SoundMap_15 = {
  binary_walk_raw_start,
  binary_walk_raw_end
};
const SoundMap * const theSoundMap [] = {
  &SoundMap_1,
  &SoundMap_2,
  &SoundMap_3,
  &SoundMap_4,
  &SoundMap_5,
  &SoundMap_6,
  &SoundMap_7,
  &SoundMap_8,
  &SoundMap_9,
  &SoundMap_10,
  &SoundMap_11,
  &SoundMap_12,
  &SoundMap_13,
  &SoundMap_14,
  &SoundMap_15,
};
#else

const SoundMap * const theSoundMap[] = {0};
#endif
