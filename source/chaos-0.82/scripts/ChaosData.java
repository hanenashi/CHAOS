import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import Spell;

/*

Write the data file containing everything

*/
public class ChaosData /*extends Frame*/ {

  static final int C_DATA = 0;
  static final int HTML_DATA = 1;
/*
PAL0 = crocodile.pcx crocodile_dead.pcx dragon_green.pcx dragon_green_dead.pcx elf.pcx \
	elf_dead.pcx gooey_blob.pcx hydra.pcx hydra_dead.pcx king_cobra.pcx king_cobra_dead.pcx

PAL1 = bat.pcx bat_dead.pcx dire_wolf.pcx dire_wolf_dead.pcx giant_rat.pcx giant_rat_dead.pcx \
	gryphon.pcx gryphon_dead.pcx harpy.pcx harpy_dead.pcx pegasus.pcx pegasus_dead.pcx vampire.pcx wraith.pcx

PAL2 = dragon_golden.pcx dragon_golden_dead.pcx eagle.pcx eagle_dead.pcx  lion.pcx lion_dead.pcx manticore.pcx manticore_dead.pcx 

PAL3 = bear.pcx bear_dead.pcx centaur.pcx centaur_dead.pcx faun.pcx faun_dead.pcx horse.pcx horse_dead.pcx wall.pcx orc.pcx orc_dead.pcx

PAL4 = magic_fire.pcx dragon_red.pcx  dragon_red_dead.pcx ogre.pcx  ogre_dead.pcx ghost.pcx  skeleton.pcx

PAL5 = dark_citadel.pcx magic_castle.pcx wraith.pcx shadow_wood.pcx

PAL6 = giant.pcx giant_dead.pcx  goblin.pcx goblin_dead.pcx 

PAL7 = zombie.pcx gorilla.pcx gorilla_dead.pcx spectre.pcx unicorn.pcx unicorn_dead.pcx

PAL8 = magic_wood.pcx spell_anim.pcx

SPRITES = arrow.pcx cursor.pcx cursor_engaged.pcx cursor_fire.pcx \
	cursor_fly.pcx cursor_ground.pcx fireball.pcx \
	lightning.pcx spell.pcx spellcolours.pcx magic_bolt.pcx pointer.pcx

WIZARDS0 = wiz1.pcx wiz2.pcx wiz3.pcx wiz4.pcx wiz5.pcx wiz6.pcx wiz7.pcx wiz8.pcx 
WIZARDS1 = magic_armour.pcx magic_shield.pcx magic_bow.pcx magic_knife.pcx	magic_sword.pcx magic_wings.pcx 
*/
  public static final Spell spellData[] = {
    new Spell("", 0, 0, 0, 0, "0"),
    //  SPELL ( name  , chaos , chance, range, ai prio, spell function) 
    new Spell("Disbelieve", 0, 10, 20, 0, "cast_disbelieve"),
    
    new Spell("King Cobra", 1, 4, 0, 0,  1, 1, 6, 1, 10, 1,0, "cast_creature"),
    new Spell("Dire Wolf", -1,  3, 0, 0,  2, 3, 7, 2, 9, 1,1, "cast_creature"),
    new Spell("Goblin", -1, 2, 0, 0,  4, 1, 4, 4, 9, 1, 6, "cast_creature"),
    new Spell("Crocodile",0,5, 0, 0,  6, 1, 2, 2, 8, 1,0, "cast_creature"),
    
    new Spell("Troll",-1,4, 0, 0,  8, 1, 1, 4, 6, 1,1, "cast_creature"),
    
    new Spell("Faun", -1,  3, 0, 0,  2, 1, 7, 8, 8, 1,3, "cast_creature"),
    new Spell("Lion",  1,  6, 0, 0,  4, 4, 8, 3, 6, 1,2, "cast_creature"),
    new Spell("Elf",  2, 1, 2, 6,  2, 1, 5, 7, 7, 1,0, "cast_creature"),
    new Spell("Orc", -1,  2, 0, 0,  1, 1, 4, 4, 10, 1,3, "cast_creature"),
    
    new Spell("Bear", 1,    6, 0, 0, 7, 2, 6, 2, 6, 1,3, "cast_creature"),
    new Spell("Gorilla", 0, 6, 0, 0,  5, 1, 4, 2, 7, 1, 7, "cast_creature"),
    new Spell("Ogre", -1,  4, 0, 0,  7, 1, 3, 6, 7, 1,4, "cast_creature"),
    new Spell("Hydra", -1,  7, 0, 0,  8, 1, 4, 6, 5, 1,0, "cast_creature"),
    
    new Spell("Giant Rat", 0,  1, 0, 0,  1, 3, 8, 2, 10, 1, 1, "cast_creature"),
    new Spell("Giant", 1,  9, 0, 0,  7, 2, 6, 5, 4, 1,6, "cast_creature"),
    new Spell("Horse", 1,  1, 0, 0,  3, 4, 8, 1, 9, 1,3, "cast_creature"),
    new Spell("Unicorn", 2, 5, 0, 0,  4, 4, 9, 7, 6, 1,7, "cast_creature"),
    
    new Spell("Centaur",1,  1, 2, 4, 3, 4, 5, 5, 7, 1,3, "cast_creature"),
    new Spell("Pegasus", 2, 2, 0, 0,  4, 5, 6, 7, 7, 1,1, "cast_creature"),
    new Spell("Gryphon", 1, 3, 0, 0,  5, 5, 5, 6, 6, 1, 1, "cast_creature"),
    new Spell("Manticore", -1, 3, 1, 3,  6, 5, 6, 8, 4, 1,2, "cast_creature"),
    
    new Spell("Bat", -1,       1, 0, 0, 1, 5, 9, 4, 8, 1,1, "cast_creature"),
    new Spell("Green Dragon", -1, 5, 4, 6,  8, 3, 4, 4, 2, 1,0, "cast_creature"),
    new Spell("Red Dragon",   -2, 7, 3, 5,  9, 3, 4, 5, 1, 1,4, "cast_creature"),
    new Spell("Golden Dragon", 2, 9, 5, 4,  9, 3, 5, 5, 2, 1,2, "cast_creature"),
    
    new Spell("Harpy", -1,  4, 0, 0,  2, 5, 8, 5, 7, 1,1, "cast_creature"),
    new Spell("Eagle", 1, 3, 0, 0,  3, 6, 8, 2, 7, 1,2, "cast_creature"),
    new Spell("Vampire", -2,  6, 0, 0,  8, 4, 6, 5, 2, 1,1, "cast_creature"),
    new Spell("Ghost", -1,  1, 0, 0,  3, 2, 9, 6, 5, 1,4, "cast_creature"),
    
    new Spell("Spectre", -1,  4, 0, 0,  2, 1, 6, 4, 6, 1,7, "cast_creature"),
    new Spell("Wraith", -1,  5, 0, 0,  5, 2, 4, 5, 6, 1,5, "cast_creature"),
    new Spell("Skeleton", -1,  3, 0, 0,  2, 1, 3, 4, 7, 1,4, "cast_creature"),
    new Spell("Zombie", -1, 1, 0, 0,  1, 1, 2, 3, 9, 1,7, "cast_creature"),
    
    new Spell("Gooey Blob", -1, 9, 6,18,0, "cast_fire_goo"),
    new Spell("Magic Fire", -1, 8, 6,18,4, "cast_fire_goo"),
    new Spell("Magic Wood", 1, 8, 8,17,8, "cast_trees_castles"),
    new Spell("Shadow Wood", -1, 4, 8,18,5, "cast_trees_castles"),
                                   // priority, pal 
    new Spell("Magic Castle",  1, 6,  8,  23,  5, "cast_trees_castles"),
    new Spell("Dark Citadel", -1, 6,  8,  23,  5, "cast_trees_castles"),
    new Spell("Wall",          0, 8,  6,  12,  3, "cast_wall"),
                                   // priority, func
    new Spell("Magic Bolt",  0, 10,  6, 18,  "cast_magic_missile"),
    new Spell("Lightning",   0, 10,  4, 18,  "cast_magic_missile"),
    
    new Spell("Vengeance",  -1,  8, 20, 12,  "cast_justice"),
    new Spell("Decree",      1,  8, 20, 12,  "cast_justice"),
    new Spell("Dark Power", -2,  5, 20, 11,  "cast_justice"),
    new Spell("Justice",     2,  5, 20, 11,  "cast_justice"),
    
    new Spell("Magic Shield", 1, 7,  0, 23,  "cast_magic_shield"),
    new Spell("Magic Armour", 1, 4,  0, 23,  "cast_magic_armour"),
    new Spell("Magic Sword",  1, 4,  0, 16,  "cast_magic_sword"),
    new Spell("Magic Knife",  1, 7,  0, 16,  "cast_magic_knife"),
    
    new Spell("Magic Bow",    1, 5,  0, 17,  "cast_magic_bow"),
    new Spell("Magic Wings",  0, 5,  0, 14,  "cast_magic_wings"),
    
    new Spell("Law 1",        2, 8,  0,  5,  "cast_chaos_law"),
    new Spell("Law 2",        4, 6,  0,  6,  "cast_chaos_law"),
    new Spell("Chaos 1",     -2, 8,  0,  5,  "cast_chaos_law"),
    new Spell("Chaos 2",     -4, 6,  0,  6,  "cast_chaos_law"),
    
    new Spell("Shadow Form",  0,  8, 0, 18,  "cast_shadow_form"),
    new Spell("Subversion",   0, 10, 7, 18,  "cast_subversion"),
    new Spell("Raise Dead",  -1,  5, 4, 18,  "cast_raise_dead"),
    new Spell("Turmoil",     -1,  5, 0, 17,  "cast_turmoil"),
    
//    new Spell("Teleport", 2, 5, 0, "cast_teleport")
//    new Spell("Sleep", 2, 5, 0, "cast_teleport")
//    new Spell("Mutate", 2, 5, 0, "cast_teleport")
    
  };
  
  // the priority was seperate
//  int prio_table[] = 
// {0, 0,
// 18,18,18,18,18,/*troll*/18, 18,18,18,18,18,
// 18,18,18,18,18,18,18,18,18,18,
// 18,18,18,18,18,18,18,18,18,18,
// 18,18,18,18,/*magic fire*/
 
// 17,18,23,23,12,/*wall*/
// 18,18,12,12,11,11,/*dark powers*/
// 23,23,16,16,17,14,5,6,5,6,18, /*shadow form*/
// 18,18,17, 17};
  
  public ChaosData(int output) {
    //System.out.println("prio table length = "+prio_table.length);
    System.out.println("spellData table length = "+spellData.length);
 /* 
    for (int i = 0; i < spellData.length; i++) {
      spellData[i].setPriority(prio_table[i]);
    }
 */ 
    if (output == C_DATA) {
      // write the data file.
      if (writeHeader()!=0) {
	System.exit(0);
      }

      if (writeData()!=0) {
	System.exit(0);
      }
    } else if (output == HTML_DATA) {
      if (ChaosHtml.writeHTML(spellData) != 0) {
	System.exit(0);
      }
    }
    System.out.println("finished OK!");
    System.exit(0);
  }
  
  public ChaosData() {
    this(C_DATA);
  }
  
  public int writeHeader() {
    // write the spelldata.h file
    try {
      String nl = System.getProperty("line.separator");
      FileWriter fw = new FileWriter("spelldata.h");
      fw.write("// Chaos spell data structures. Generated by ChaosData.java"+nl+nl);
      
      for (int i = 0; i < spellData.length; i++) {
        String binName = spellData[i].getBinName();
        if (!binName.equals("")) {
//          if (i < 33) {
            fw.write("#define SPELL_"+binName.toUpperCase()+" "+Integer.toString(i)+nl);
//          }
        }
      }
      fw.write(nl+nl);
      // write out all the data includes
      String binName;
      for (int i = 0; i < spellData.length; i++) {
        //if (spellData[i].hasBinName()) {
        if (spellData[i].isCreature()) {
          binName = spellData[i].getBinName();
          fw.write("#if !defined (__MINGW32__)"+nl);
          fw.write("#define     binary_"+binName+"_raw_start  _binary_"+binName+"_raw_start"+nl);
          fw.write("#define     binary_"+binName+"_map_start  _binary_"+binName+"_map_start"+nl);
          fw.write("#endif"+nl);
          fw.write("extern unsigned char binary_"+binName+"_raw_start[];"+nl);
          fw.write("extern unsigned char binary_"+binName+"_map_start[];"+nl);
          
        } 
      }
      
      
      fw.write("struct SPELL_DATA"+nl);
      fw.write("{"+nl);
      fw.write("   const char* spellName;"+nl);
      fw.write("   const signed char chaosRating;"+nl);
      fw.write("   const unsigned char combat;"+nl);
      fw.write("   const unsigned char rangedCombat;"+nl);
      fw.write("   const unsigned char rangedCombatRange;"+nl);
      fw.write("   const unsigned char defence;"+nl);
      fw.write("   const unsigned char movement;"+nl);
      fw.write("   const unsigned char manvr;"+nl);
      fw.write("   const unsigned char magicRes;"+nl);
      fw.write("   const unsigned char castChance;"+nl);
      fw.write("   const unsigned char castRange;"+nl);
      fw.write("   const unsigned char castPriority;"+nl);
      fw.write("   const unsigned char palette;"+nl);
      fw.write("   void (*pFunc)(void); // pointer to the spell casting function"+nl);
      fw.write("   const unsigned char *pGFX;  // pointer to the graphics array (extern u8 _binary_GFX_raw_start[];)"+nl);
      fw.write("   const unsigned char *pGFXMap; "+
	  "// pointer to the map file that makes up the graphics (extern u8 _binary_GFX_map_start[];)"+nl);
      fw.write("};"+nl);
      
//      fw.write("struct SPELL_TABLE"+nl);
//      fw.write("{"+nl);
//      fw.write("  const struct SPELL_DATA* pSpellData;"+nl);
//      fw.write("};"+nl);
      
      fw.write("struct SPELL_DATA_ALL "+nl);
      fw.write("{"+nl);
      fw.write("  const unsigned char nSpellCount; // number of spells"+nl);
      //fw.write("  const struct SPELL_TABLE *pSpellDataTable;"+nl);
      fw.write("  const struct SPELL_DATA *const *pSpellDataTable;"+nl);
      fw.write("};"+nl);
      
      fw.write("extern const struct SPELL_DATA_ALL CHAOS_SPELLS;"+nl);
      fw.flush();
      fw.close();
      return 0;
    }
    catch (IOException ioe) {
      System.out.println("Error writing header file!");
      System.out.println(ioe.toString());
      return 1;
    }
    
  }
  
  public int writeData() {
    // write the spelldata.c file
    try {
      String nl = System.getProperty("line.separator");
      FileWriter fw = new FileWriter("spelldata.c");
      
      fw.write("// Chaos spell data structures. Generated by ChaosData.java"+nl+nl);
      fw.write("#include \"spelldata.h\""+nl);
      fw.write("#include \"magic.h\""+nl);
      fw.write("#include \"platform.h\""+nl+nl);
      for (int i = 0; i < spellData.length; i++) {
        fw.write(spellData[i].getDataStruct(i));  
        fw.write(nl+nl);
      }
      fw.write(nl);
      //fw.write("static const struct SPELL_TABLE spell_table[] = {"+nl);
      fw.write("static const struct SPELL_DATA* const spell_table[] = {"+nl);
      for (int i = 0; i < spellData.length; i++) {
        //fw.write("    {&spell_data_"+Integer.toString(i)+"},"+nl);  
        fw.write("    &spell_data_"+Integer.toString(i)+","+nl);  
      }
      fw.write("};"+nl);
      
      fw.write("const struct SPELL_DATA_ALL CHAOS_SPELLS = {"+nl);
      fw.write("    "+Integer.toString(spellData.length)+", // number of spells"+nl);
      fw.write("    spell_table, // pointer to spells"+nl);
      fw.write("};"+nl);
      
      fw.flush();
      fw.close();
      return 0;
    } catch (IOException ioe) {
      System.out.println("Error writing C file!");
      System.out.println(ioe.toString());
      return 1;
    }
  }

  
  
  public static void main(String [] args) {
    if (args.length == 0) {
      new ChaosData(/*args[0]*/);
    } else {
      new ChaosData(HTML_DATA);
    }
  }  
}

