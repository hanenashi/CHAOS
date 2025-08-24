// wizards.h

// the wizard definitions
// the ser colour to change..
#ifndef WIZARDS_H
#define WIZARDS_H

#define WIZARD_COLOUR 0x1

#define GET_OWNER(square) (square & 0x7)
// shadow from is bit 3
#define HAS_SHADOWFORM(flag) ( (flag & 0x8) == 0x8 )
// if bit 4 set, wizard is dead
#define IS_WIZARD_DEAD(flag) ( (flag & 0x10) == 0x10 )
// wings is bit 5
#define HAS_MAGICWINGS(flag) ( (flag & 0x20) == 0x20 )

#define GFX_MAGIC_SHIELD  8
#define GFX_MAGIC_ARMOUR  9
#define GFX_MAGIC_SWORD   10
#define GFX_MAGIC_KNIFE   11
#define GFX_MAGIC_BOW     12
#define GFX_MAGIC_WINGS   13

// human and cpu are obvious
#define PLYR_HUMAN        0
#define PLYR_CPU          1
// remote means "network player, not on this pc"
#define PLYR_REMOTE       2
// remote means "network player, is on this pc"
#define PLYR_LOCAL        4
// player not connected
#define PLYR_NC        8

#define IS_CPU(n) ((players[n].plyr_type&PLYR_CPU)>0)
#define IS_REMOTE(n) ((players[n].plyr_type&PLYR_REMOTE)>0)
#define IS_LOCAL(n) ((players[n].plyr_type&PLYR_LOCAL)>0)

extern unsigned char current_player;

typedef struct 
{
	char name[12];
	unsigned char combat;
	unsigned char ranged_combat;
	unsigned char range;
	unsigned char defence;
	unsigned char movement_allowance;
	unsigned char manoeuvre_rating;
	unsigned char magic_resistance;
	unsigned char spell_count;
	unsigned char spells[42];  // 2 byte pairs, one for the priority, the other for the spell id
	unsigned char ability;
	//animationspeed
	unsigned char image;      // this is different...
	unsigned short colour;
//	unsigned char is_cpu;
        unsigned char plyr_type;  // big change for networking
	unsigned char modifier_flag;
	unsigned char illusion_cast;
	unsigned char selected_spell;
	unsigned char timid;
	
} player_data; // 60 bytes * 8 = 480 bytes of wizard data

extern player_data players[8];

/*
meaning of bits in modifier flag...
bit 0
bit 1    magic knife
bit 2    magic sword
bit 3    shadow form
bit 4    is dead
bit 5    magic wings
bit 6    magic shield
bit 7    magic armour

*/

#if !defined( __MINGW32__)
#define WIZ1_DATA _binary_wiz1_raw_start
#define WIZ1_MAP_DATA _binary_wiz1_map_start
#else
#define WIZ1_DATA binary_wiz1_raw_start
#define WIZ1_MAP_DATA binary_wiz1_map_start
#endif

extern unsigned char WIZ1_DATA[];
extern unsigned char WIZ1_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ2_DATA _binary_wiz2_raw_start
#define WIZ2_MAP_DATA _binary_wiz2_map_start
#else
#define WIZ2_DATA binary_wiz2_raw_start
#define WIZ2_MAP_DATA binary_wiz2_map_start
#endif

extern unsigned char WIZ2_DATA[];
extern unsigned char WIZ2_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ3_DATA _binary_wiz3_raw_start
#define WIZ3_MAP_DATA _binary_wiz3_map_start
#else
#define WIZ3_DATA binary_wiz3_raw_start
#define WIZ3_MAP_DATA binary_wiz3_map_start
#endif

extern unsigned char WIZ3_DATA[];
extern unsigned char WIZ3_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ4_DATA _binary_wiz4_raw_start
#define WIZ4_MAP_DATA _binary_wiz4_map_start
#else
#define WIZ4_DATA binary_wiz4_raw_start
#define WIZ4_MAP_DATA binary_wiz4_map_start
#endif

extern unsigned char WIZ4_DATA[];
extern unsigned char WIZ4_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ5_DATA _binary_wiz5_raw_start
#define WIZ5_MAP_DATA _binary_wiz5_map_start
#else
#define WIZ5_DATA binary_wiz5_raw_start
#define WIZ5_MAP_DATA binary_wiz5_map_start
#endif

extern unsigned char WIZ5_DATA[];
extern unsigned char WIZ5_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ6_DATA _binary_wiz6_raw_start
#define WIZ6_MAP_DATA _binary_wiz6_map_start
#else
#define WIZ6_DATA binary_wiz6_raw_start
#define WIZ6_MAP_DATA binary_wiz6_map_start
#endif

extern unsigned char WIZ6_DATA[];
extern unsigned char WIZ6_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ7_DATA _binary_wiz7_raw_start
#define WIZ7_MAP_DATA _binary_wiz7_map_start
#else
#define WIZ7_DATA binary_wiz7_raw_start
#define WIZ7_MAP_DATA binary_wiz7_map_start
#endif

extern unsigned char WIZ7_DATA[];
extern unsigned char WIZ7_MAP_DATA[];

#if !defined( __MINGW32__)
#define WIZ8_DATA _binary_wiz8_raw_start
#define WIZ8_MAP_DATA _binary_wiz8_map_start
#else
#define WIZ8_DATA binary_wiz8_raw_start
#define WIZ8_MAP_DATA binary_wiz8_map_start
#endif

extern unsigned char WIZ8_DATA[];
extern unsigned char WIZ8_MAP_DATA[];


#if !defined( __MINGW32__)
#define SHIELD_DATA _binary_shield_raw_start
#define SHIELD_MAP_DATA _binary_shield_map_start
#else
#define SHIELD_DATA binary_shield_raw_start
#define SHIELD_MAP_DATA binary_shield_map_start
#endif

extern unsigned char SHIELD_DATA[];
extern unsigned char SHIELD_MAP_DATA[];

#if !defined( __MINGW32__)
#define ARMOUR_DATA _binary_armour_raw_start
#define ARMOUR_MAP_DATA _binary_armour_map_start
#else
#define ARMOUR_DATA binary_armour_raw_start
#define ARMOUR_MAP_DATA binary_armour_map_start
#endif

extern unsigned char ARMOUR_DATA[];
extern unsigned char ARMOUR_MAP_DATA[];

#if !defined( __MINGW32__)
#define SWORD_DATA _binary_sword_raw_start
#define SWORD_MAP_DATA _binary_sword_map_start
#else
#define SWORD_DATA binary_sword_raw_start
#define SWORD_MAP_DATA binary_sword_map_start
#endif

extern unsigned char SWORD_DATA[];
extern unsigned char SWORD_MAP_DATA[];

#if !defined( __MINGW32__)
#define KNIFE_DATA _binary_knife_raw_start
#define KNIFE_MAP_DATA _binary_knife_map_start
#else
#define KNIFE_DATA binary_knife_raw_start
#define KNIFE_MAP_DATA binary_knife_map_start
#endif

extern unsigned char KNIFE_DATA[];
extern unsigned char KNIFE_MAP_DATA[];

#if !defined( __MINGW32__)
#define BOW_DATA _binary_bow_raw_start
#define BOW_MAP_DATA _binary_bow_map_start
#else
#define BOW_DATA binary_bow_raw_start
#define BOW_MAP_DATA binary_bow_map_start
#endif

extern unsigned char BOW_DATA[];
extern unsigned char BOW_MAP_DATA[];

#if !defined( __MINGW32__)
#define WINGS_DATA _binary_wings_raw_start
#define WINGS_MAP_DATA _binary_wings_map_start
#else
#define WINGS_DATA binary_wings_raw_start
#define WINGS_MAP_DATA binary_wings_map_start
#endif

extern unsigned char WINGS_DATA[];
extern unsigned char WINGS_MAP_DATA[];


struct WIZARD_DATA
{
  const unsigned char * pGFX;
  const unsigned char * pMap;
};

struct WIZARD_DATATABLE 
{
  const struct WIZARD_DATA *pWizardData;
};

extern const struct WIZARD_DATATABLE WizardGFX[];

void update_creaturecount(void);

// kills the wizard at the target_index
// updates flags, etc
void kill_wizard(void);

void reset_players(void);

#endif // WIZARDS_H

