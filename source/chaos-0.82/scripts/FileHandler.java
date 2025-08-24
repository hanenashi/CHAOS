
import java.io.*;

public class FileHandler extends Object {
  
  String file;
  
  public FileHandler(String theFile) {
    file = theFile;
  }
  
  public void setFile(String theFile) {
    file = theFile;
  }
  
  
  public byte [] load(byte [] theBytes) {
    try {
      
      FileInputStream is = new FileInputStream(file);
//      if (is.available() > theBytes.length) {
//        System.out.println("Bytes not enough! Need "+is.available()+" you gave me "+
//        theBytes.length);
//        return;
//      }
      
      if (theBytes == null || (is.available() > theBytes.length))
        theBytes = new byte[is.available()];
      
      is.read(theBytes);
      is.close();
    }
    catch (Exception e) {
      System.out.println("Error loading file \""+file+"\":"+e.getMessage());
    }
    return theBytes;
  }
  
  public void saveAsBytes(int [] toBytes, String filename) {
    // save the ints as bytes, truncates 
    try {
      
      FileOutputStream os = new FileOutputStream(filename);
      byte bytes[] = new byte[toBytes.length];
      for (int i = 0; i < toBytes.length; i++) {
        bytes[i] = (byte)(toBytes[i]&0xff);  
      }
      os.write(bytes);
      os.close();
      
    }
    catch (Exception e) {
      System.out.println("Error loading file");
    }
    
  }
  
  public void save(byte [] theBytes, int offset, int size, String filename) {
    // save the ints as bytes, truncates 
    try {
      
      FileOutputStream os = new FileOutputStream(filename);
      os.write(theBytes, offset, size);
      os.close();
      
    }
    catch (Exception e) {
      System.out.println("Error loading file");
    }
    
  }
  
  public void dumpFile(int width) {
    byte [] bytes = null;
    bytes = load(bytes);
    if (bytes != null)  {
      boolean needComma = false;
      for (int i = 0; i < bytes.length; i++) {
        if (needComma)
          System.out.print(", ");
        if ( (i%width) == 0) {
          System.out.println();
        }
        String s = Integer.toHexString(bytes[i]&0xff);
        if (s.length() < 2)
          s = "0"+s;
        System.out.print(s);
        needComma = true;
      }
      
    }
    
  }
  
  
  public static void main(String [] args) {
    
    FileHandler fh = new FileHandler(args[0]);
    if (args.length > 1) {
      byte [] ImNull = null;
      byte [] low = fh.load(ImNull);
      fh.setFile(args[1]);
      byte [] hi = fh.load(ImNull);
      boolean needComma = false;
      int width = 8;
      for (int i = 0; i < hi.length && i < low.length; i++) {
        if (needComma)
          System.out.print(" ");
        if ( (i%width) == 0) {
          System.out.println();
        }
        int address = ((hi[i]&0xff)<<8)| (low[i]&0xff);
        address++;
        
        String s = Integer.toHexString(address);
        if (s.length() < 2)
          s = "0"+s;
        System.out.print("$"+s);
        needComma = true;
      }
    }
    else  {
      fh.dumpFile(8);
    }
  }
  
}
  
