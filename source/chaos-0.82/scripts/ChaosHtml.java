import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

public class ChaosHtml {


  final static String htmlTemplate = "template.html";
  final static String htmlOut   = "chaosdata.html";
  final static String BEGIN_COMMENT = "<!-- BEGIN MAIN CONTENT -->";
  final static String END_COMMENT = "<!-- END MAIN CONTENT -->";

  static String htmlOpening;
  static String htmlClosing;
  

  private static int setupTemplate() {
    try {
      File tmplt = new File(htmlTemplate);
      if (tmplt.exists()) {
        BufferedReader reader = new BufferedReader(new FileReader(htmlTemplate));
        String inputLine;
	boolean opening = true;
	boolean closing = false;
	htmlOpening = "";
	htmlClosing = END_COMMENT+"\n";
	while((inputLine = reader.readLine()) != null) {
	  if (opening) {
	    htmlOpening += inputLine+"\n";
	  }
	  if (closing) {
	    htmlClosing += inputLine+"\n";
	  }
	  inputLine = inputLine.trim();
          if ( inputLine.length() != 0) {
	    if (inputLine.equals(BEGIN_COMMENT)) {
	      opening = false;
	    }
	    if (inputLine.equals(END_COMMENT)) {
	      closing = true;
	    }
	  }
	}
      } else {
	return -1;
      }
    }
    catch (Exception e) {
      e.printStackTrace();
      return -1;
    }
    return 0;
  }
  
  public static void writeln(String message) {
    try {
      BufferedWriter writer = new BufferedWriter(new FileWriter(htmlOut, true));
      String nl = System.getProperty("line.separator");
      writer.write(message+nl);
      writer.flush();
      writer.close();
    }
    catch (IOException ioe) {
      // file cannot be opened or line cannot be written
      System.out.println("Error writing to file "+htmlOut+" "+ioe);
    }
  }

  final static int MOUNT_START = 17; // horse
  final static int FLYERS_START =  20;
  final static int UNDEAD_START = 29;
  final static int GOOEY_BLOB = 35;
  
  public static int writeHTML(final Spell spelldata[]) {
    // write the html... a table of spells
    // first read in the "template" which contains the basic html page
    // read to beginComment
    if (setupTemplate()!=0)
      return -1;

    try {
      System.out.println("Creating new file "+htmlOut);
      BufferedWriter writer = new BufferedWriter(new FileWriter(htmlOut, false));
      writeln(htmlOpening);
    }
    catch (IOException e) {
      e.printStackTrace();
      return -1;
    }
    // now loop through the spells and write each one to the table..
    writeln("<div id=\"chaosdata\">");
    
    writeln(Spell.getHtmlKey());
    
    writeln("<table cellspacing=0 cellpadding=0>");
    writeln(Spell.htmlTableHeader());
    for (int i = 1; i < spelldata.length; i++) {
      /*
      if (i == MOUNT_START) {
	//writeln("</div>"); // close walkers div
	writeln("<div id=\"mount\">"); 
      }
      if (i == FLYERS_START) {
	writeln("</div>"); // close mount
	writeln("<div id=\"flying\">"); 
      }
      if (i == UNDEAD_START) {
	writeln("</div>"); // close flyers
	writeln("<div id=\"undead\">"); 
      }
      if (i == GOOEY_BLOB) {
	writeln("</div>"); // close undead
      }
      if ((i%16)==0) {
	writeln(Spell.htmlTableHeader());
      }*/

      writeln( spelldata[i].toHtml(i) );
    }
    writeln("</table>");
    writeln("</div>");
    
    writeln(htmlClosing);
    return 0;
  }
}
