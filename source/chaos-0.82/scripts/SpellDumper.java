import java.util.*;

public class SpellDumper extends Object {

  final static String SNA_NAME = "CHAOS.SNA";
  final static int SNA_LENGTH = 49179;
  // convert from mem addr to file offset
  final static int SNA_OFFSET = 0x4000-0x1b;

  // this contains the words for each value
  final static int SPL_TABLE = 0xe3e0;

  byte [] g_sna = new byte[SNA_LENGTH];


  // get the byte at the memory address...
  public byte byteAt(int addr) {
    return g_sna[addr - SNA_OFFSET];
  }
  // get the word (16 bits) at the address
  public int wordAt(int addr) {
    return (byteAt(addr)&0xff)|((byteAt(addr+1)&0xff)<<8);
  }
  public String hexString(int i) {
    return hexString(i,2);
  }
  public String hexString(int i, int s) {
    // hexor i to s letters min
    String x = Integer.toHexString(i);
    while (x.length() < s) {
      x = "0"+x;
    }
    return x;
  }

  public SpellDumper() {
    FileHandler fh = new FileHandler(SNA_NAME);
    fh.load(g_sna);
  }
  final static String STATS_NAME[] = {
    "combat\t",
    "ranged combat",
    "ranged range",
    "defence\t",
    "movement",
    "magic res",
    "maneouvre",
    "casting chance",
    "chaos/law",
    "anim speed",
  };
  public void dumpValue(int s) {
    int addr = wordAt(SPL_TABLE + ((s-1)*2));
    System.out.println("Addr = "+hexString(addr,4));
    dumpValueAt(addr, 0x26);
  }
  public void dumpValueAt(int addr, int knownSize) {
    StringBuffer buffer = new StringBuffer();
    for (int i = 0; i < 0xd; i++) {
      byte b = byteAt(addr+i);
      buffer.append((char)(b&0xff));
    }
    System.out.println(buffer.toString());
    int index = 0xd;
    for (int i = 0; i < STATS_NAME.length; i++) {
      System.out.println(
          STATS_NAME[i]+"\t"+
          hexString(byteAt(addr+i+index)&0xff,2));
    }
    index += STATS_NAME.length;
    // after this there are 4 or 5 graphics addresses/info
    // which consist off ADDRESS (2 bytes) and ATTRIBUTE (1)
    for (int i = 0; i < 5 && (index +3*i) < knownSize; i++) {
      System.out.println(
          "GFX Addr\t"+
          hexString(wordAt(addr+(i*3)+index),4));
      System.out.println(
          "GFX Attr\t"+
          hexString(byteAt(addr+(i*3)+2+index)&0xff));
    }
  }

  public void dumpTable(int stat) {
    if (stat > 10)
      dumpTable();
    else {
      // for each thing get the stat...
      int end = wordAt(SPL_TABLE);
      int addresses [] = new int[(end-SPL_TABLE)/2];
      for (int i = 0; i < addresses.length; i++) {
	addresses[i] = wordAt(SPL_TABLE + (i*2));
      }
      String tabName = STATS_NAME[stat].replace(' ', '_');
      System.out.println("const int "+tabName+"_data[] = {");
      for (int j = 0; j < addresses.length; j++) {
	StringBuffer buffer = new StringBuffer();
	for (int i = 0; i < 0xd; i++) {
	  byte b = byteAt(addresses[j]+i);
	  buffer.append((char)(b&0xff));
	}
	System.out.print("/*"+buffer.toString()+"*/");
	int index = 0xd;
	System.out.println(
	    " 0x"+
	    hexString(byteAt(addresses[j]+stat+index)&0xff,2)+",");
      }
      System.out.println("};");

    }
  }
  public void dumpTable() {
    // dump the address table in order...
    int end = wordAt(SPL_TABLE);
    int addresses [] = new int[(end-SPL_TABLE)/2];
    for (int i = 0; i < addresses.length; i++) {
      addresses[i] = wordAt(SPL_TABLE + (i*2));
    }
    Arrays.sort(addresses);

    for (int i = 0; i < addresses.length-1; i++) {
      System.out.print(i+"\t:");
      System.out.print(Integer.toHexString(addresses[i]));
      int size = addresses[i+1]-addresses[i];
      System.out.println(" size "+
          Integer.toHexString(size));

      dumpValueAt(addresses[i], size);
    }
    System.out.print((addresses.length-1)+"\t:");
    System.out.println(Integer.toHexString(addresses[(addresses.length-1)]));
    dumpValueAt(addresses[addresses.length-1], 0x26);
  }

  public static void main(String [] args) {
    SpellDumper sd = new SpellDumper();

    if (args.length > 0) {
      if (args[0].equals("as")){
	// dump anim speed table..
	sd.dumpTable(Integer.decode(args[1]).intValue());
        return;
      }
      
      if (args.length == 1)
        sd.dumpValue(Integer.decode(args[0]).intValue());
      else
        sd.dumpValueAt(Integer.decode(args[1]).intValue(),0x26);  
    }
    else {
      sd.dumpTable();
    }
  }
}

