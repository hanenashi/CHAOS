
public class RandomStat extends Object {

  byte [] g_sna;


  public RandomStat(String fn) {
    FileHandler fh = new FileHandler(fn);
    g_sna  = fh.load(g_sna);
  }
  
  public void doStats(int to) {
    if (to == 0)
      to = g_sna.length;
    int count [] = new int[256];
    
    for (int i = 0; i < to; i++) {
      count[g_sna[i]&0xff]++;
    }
    double total = 0.;
    double thisVal;
    for (int i = 0; i < 256; i++) {
      thisVal = 100.0*count[i]/(1.0*to);
      total += thisVal;
      System.out.println(i+": "+count[i]+" "+thisVal);
    }
    System.out.println("*** "+total);
    
  }
  
  
  public static void main(String [] args) {
    RandomStat rs = new RandomStat(args[0]);
    rs.doStats(0x7ff);
    
  }
}

