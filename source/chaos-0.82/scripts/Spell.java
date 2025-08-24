import java.awt.Color;

public class Spell extends Object {

  // Every spell has the following stats:
  public String   name         = "Void";
  public int      chaosRating  =  0;
  public int      combat       = 0;
  public int      rangedCombat = 0;
  public int      rangeRC      = 0;
  public int      defence      = 0;
  public int      movement     = 0;
  public int      manvr        = 0;
  public int      magicRes     = 0;
  public int      castingChance= 0; 
  public int      castingRange = 0;
  public boolean  creatureType = true;
  public int      palette = 0;
  public String   func = "cast_disbelieve";
  public int      priority = 0x12;  // AI spell priority
  
  public Spell() {
  }
  
  // This is for creature spells:
  public Spell( String   name,
                int      chaosRating,
                int      combat,
                int      rangedCombat,
                int      rangeRC,
                int      defence,
                int      movement,
                int      manvr,
                int      magicRes,
                int      castingChance,
                int      castingRange,
                int      palette,
                String   funcName) {
    // start
    this.name         = name;
    this.chaosRating  = chaosRating;
    this.combat       = combat;
    this.rangedCombat = rangedCombat;
    this.rangeRC      = rangeRC;
    this.defence      = defence;
    this.movement     = movement;
    this.manvr        = manvr;
    this.magicRes     = magicRes;
    this.castingChance= castingChance;
    this.castingRange = castingRange;
    // all creatures have priorty 18
    this.priority     = 18;
    this.creatureType = true;
    this.palette      = palette;
    this.func         = funcName;
    // end of Spell constructor
  }

  // this is for "pure magic" spells like Subversion, wizard spells, Disbelieve, etc
  public Spell(String name, int chaosRating, int castingChance, int castingRange, int priority, String funcName) {
    this(name, chaosRating,0,0,0,0,0,0,0, castingChance, castingRange, 0, funcName);
    // don't have gfx - flag this
    this.creatureType = false;
    this.priority     = priority;
  }
  
  // this is for "creature like" Spells that have gfx. E.g. Blob, Fire, Trees, Castles
  public Spell(String name, int chaosRating, int castingChance, int castingRange, int priority, int pal, String funcName) {
    this(name, chaosRating,0,0,0,0,0,0,0, castingChance, castingRange, pal, funcName);
    this.priority     = priority;
  }
 /* 
  public void setPriority(int i) {
    priority = i;
  }
  
  
  // not every spell has a binary data file. Weed out the ones that don't:
  // replaced by creatureiType/constructor
  public boolean hasBinName() {
      if (
        name.equalsIgnoreCase("")||
        name.equalsIgnoreCase("Magic Bolt") || 
        name.equalsIgnoreCase("Lightning") || 
        name.equalsIgnoreCase("Vengeance") || 
        name.equalsIgnoreCase("Justice") || 
        name.equalsIgnoreCase("Dark Power") || 
        name.equalsIgnoreCase("Decree") || 
        name.equalsIgnoreCase("Raise Dead") || 
        name.equalsIgnoreCase("Subversion") || 
        name.equalsIgnoreCase("Shadow Form") || 
        name.startsWith("Law") || 
        name.startsWith("Chaos") || 
        name.equalsIgnoreCase("Turmoil") || 
        name.equalsIgnoreCase("Teleport") || 
        name.equalsIgnoreCase("Magic Sleep")||
        name.equalsIgnoreCase("Dead Revenge")||
        name.equalsIgnoreCase("Dispell")||
        name.equalsIgnoreCase("Disbelieve") ||
        name.equalsIgnoreCase("Magic Shield") || 
        name.equalsIgnoreCase("Magic Armour") || 
        name.equalsIgnoreCase("Magic Sword") || 
        name.equalsIgnoreCase("Magic Knife") || 
        name.equalsIgnoreCase("Magic Bow") || 
        name.equalsIgnoreCase("Magic Wings")
        ) {
          return false;
        }
        return true;
  }
  */
  
  
  public String getBinName() {
    // return the name to use in the binary include
    
    String lname = name.toLowerCase();
    if (lname.endsWith("dragon")) {
      if (lname.startsWith("red"))
	return "dragon_red";
      if (lname.startsWith("golden"))
	return "dragon_golden";
      if (lname.startsWith("green"))
	return "dragon_green";
    }
    if (lname.startsWith("magic") && castingRange == 0) {
      lname = lname.substring(6);
    }
    // replace spaces with underscores
    lname = lname.replace(' ', '_');
    
    return lname;
    
  }
  
  
  public String getDataStruct(int index) {
    
    // return the data string for the .c file
    // each spell will have this:
    String nl = System.getProperty("line.separator");
    String os = "static const struct SPELL_DATA spell_data_"+Integer.toString(index)+" = {"+nl;
    os+= "    \""+name+"\","+nl;
    os+= "    "+Integer.toString(chaosRating)+", // chaos rating"+nl;
         
    os+= "    "+Integer.toString(combat)+", // combat"+nl;
    os+= "    "+Integer.toString(rangedCombat)+", // rangedCombat"+nl;
    os+= "    "+Integer.toString(rangeRC)+", // rangedCombatRange"+nl;
    os+= "    "+Integer.toString(defence)+", // defence"+nl;
    os+= "    "+Integer.toString(movement)+", // movement"+nl;
    os+= "    "+Integer.toString(manvr)+", // manvr"+nl;
    os+= "    "+Integer.toString(magicRes)+", // magicRes"+nl;
    os+= "    "+Integer.toString(castingChance)+", // castChance"+nl;
    if (castingRange != 0)
      os+= "    "+Integer.toString(castingRange*2+1)+", // castRange"+nl;
    else 
      os+= "    0, // castRange"+nl;
    os+= "    "+Integer.toString(priority)+", // ai cast priority"+nl;
    os+= "    "+Integer.toString(palette)+", // 16 colour palette"+nl;
    os+= "    "+func+", // function pointer"+nl;
    
    String binName = getBinName();
    if (isCreature()) {
      os+= "    binary_"+binName+"_raw_start,"+nl;
      os+= "    binary_"+binName+"_map_start,"+nl;
    } else {
      // no gfx for this spell
      os+= "    0,"+nl;
      os+= "    0,"+nl;
    }
    os+="};";
    
    return os;
  }
  
  private final static String TD =   "<td>";
  private final static String TDE = "</td>"+"\n";

  private String rating2string() {
    if (chaosRating != 0) {
      String s = "LAW";
      if (chaosRating < 0) {
	s = "CHAOS";
      }
      if ( Math.abs(chaosRating) > 1) {
	s += " " + Integer.toString(Math.abs(chaosRating));
      }
      return s;
    }
    else {
      return "NEUTRAL";
    }
  }
  
  public static String htmlTableHeader() {
    String nl = System.getProperty("line.separator");

    String os = "<tr>"+nl;

    os+=TD;
    os+=TDE;

    os+=TD; os+="<h3>";
    os+="Spell";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Chaos Rating";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Cast Chance";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Combat";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Ranged Combat";
    os+="</h3>"; os+=TDE;
    
    os+=TD; os+="<h3>";
    os+="Range";
    os+="</h3>"; os+=TDE;
    
    os+=TD; os+="<h3>";
    os+="Defence";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Movement Allowance";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Manoeuvre Rating";
    os+="</h3>"; os+=TDE;

    os+=TD; os+="<h3>";
    os+="Magic Resistance";
    os+="</h3>"; os+=TDE;

    os += "</tr>";
    return os;

  }

  public static String getHtmlKey() {
//    String os = "<table>";
//    os += "<tr>";
    String os = "";
    String opening = "<b";
    String closing = "</b><br>"+"\n";

    os+=opening+">Key:"+closing;
    
    os+=opening+" id=\"creature_mount\">"; 
    os+="Mount";
    os+=closing;
    
    os+=opening+" id=\"creature_mount_flying\">"; 
    os+="Flying Mount";
    os+=closing;

    os+=opening+" id=\"creature_flying\">"; 
    os+="Flying";
    os+=closing;

    os+=opening+" id=\"creature_flying_undead\">"; 
    os+="Flying Undead";
    os+=closing;

    os+=opening+" id=\"creature_undead\">"; 
    os+="Undead";
    os+=closing;
    /*os += "</tr>";
    os += "</table>";*/

    return os;

  }

  static final int SPELL_HORSE = 17;
  static final int SPELL_PEGASUS = 20;
  static final int SPELL_MANTICORE = 22;
  static final int SPELL_VAMPIRE = 29;
  static final int SPELL_GHOST = 30;
  static final int SPELL_ZOMBIE = 34;
  protected String getType(int id) {
    if (isCreature()) {
      String type="creature";
      if (id >= SPELL_HORSE && id <= SPELL_MANTICORE) {
	type+="_mount";
      }
      if (id >= SPELL_PEGASUS && id <= SPELL_GHOST) {
	type += "_flying";
      }
      if ( id >= SPELL_VAMPIRE && id <= SPELL_ZOMBIE) {
	type += "_undead";
      }
      return type;	
    }
    else {
      return "non_creature";
    }
    
  }
  
  public String toHtml(int index) {
    // create a table entry for this spell...
    String nl = System.getProperty("line.separator");

    String type = getType(index);
    String os = "<tr id=\"tr"+(index&1)+"\">"+nl;

    os+=TD;
    if (isCreature())
      os+="<img src=imagespng/"+getBinName()+".pcx.png width=32 height=32>";
    else if (castingRange==0 && name.toLowerCase().startsWith("magic")) {
      os+="<img src=imagespng/"+getBinName()+".pcx.png width=32 height=32>";
    } else {
      os += "<!-- no image -->";
    }
    os+=TDE;

    os+="<td id=\""+type+"\">"+nl;
    os+="<h4>";
    os+=name;
    os+="</h4>";
    os+=TDE;

    os+=TD;
    os+=rating2string();
    os+=TDE;

    os+=TD;
    os+="<div id=\"castchance"+castingChance+"\">";
    os+= Integer.toString(10*castingChance)+" %";
    os+="</div>";
    os+=TDE;

    if (isCreature()) {
      os+=TD;
      os+=Integer.toString(combat);
      os+=TDE;

      os+=TD;
      if (rangedCombat != 0) {
	os+=Integer.toString(rangedCombat);
      }
      os+=TDE;

      os+=TD;
      if (rangedCombat != 0) {
	os+=Integer.toString(rangeRC);
      }
      os+=TDE;

      os+=TD;
      os+=Integer.toString(defence);
      os+=TDE;

      os+=TD;
      os+=Integer.toString(movement);
      os+=TDE;

      os+=TD;
      os+=Integer.toString(manvr);
      os+=TDE;

      os+=TD;
      os+=Integer.toString(magicRes);
      os+=TDE;
    } else {
      os+="<td colspan=2>";
      os+=TDE;
      
      os+=TD;
      if (castingRange != 0) {
	os+=Integer.toString(castingRange);
      }
      os+=TDE;
      os+="<td colspan=4>";
      os+=TDE;
    }

    os += "</tr>";
    return os;

  } 
  
  public boolean isCreature() {
    return creatureType;
  }
}           
