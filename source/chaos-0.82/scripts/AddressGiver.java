import java.net.*;
import java.util.*;

public class AddressGiver implements Runnable {
  /* listens on a port for connections
   * stores alive connections, checks for
   * disconnections.
   */
  
  Requests requests;
  Registers registers;
  
  public AddressGiver() {
    requests = new Requests(1100);
    registers = new Registers(1000);
    requests.start();
    registers.start();
    // also, a main thread to check alive sockets
    new Thread(this).start();
  }
  
  public void run() {
    // check alive sockets
    for  (;;) {
      // check alive threads...
      // do nowt for now
    }
  }
  
  private class Requests extends Thread {
    // clients request connected server addresses on requestSock
    ServerSocket requestSock;
    Requests(int port) {
      try {
        requestSock = new ServerSocket(port);
      }
      catch (Exception e) {}
    }
    
    public void run() {
      for (;;) {
        // listen for requests (this call is blocking)
        try {
          Socket s = requestSock.accept();
          System.out.println("request got a signal");
          InetAddress ip = registers.getInetAddress();
          String str = "Nothing";
          if (ip != null) {
            // send the ip thingy as a name
            str = ip.getHostName();
          } 
          s.getOutputStream().write(str.getBytes());
          s.close();
        }
        catch (Exception e) {}
        // check the first "server" is active by attempting to connect to it
      }
    }
  }
  
  private class Registers extends Thread {
    // servers register on registerSock
    ServerSocket registerSock;
    InetAddress theAddr;
    //Vector addresses;
    Registers(int port) {
      //addresses = new Vector();
      try {
        registerSock = new ServerSocket(port);
      }
      catch (Exception e) {}
      
    }
    
    public void run() { 
      for (;;) {
        // listen for registering (this call is blocking)
        try {
          Socket s = registerSock.accept();
          theAddr = s.getInetAddress();
          System.out.println("register got a signal");
        }
        catch (Exception e) {}
        // add address/port to addresses
        // oh, also, request the server port that is active
      }
    }
    
    public InetAddress getInetAddress() {
      return theAddr;
    }
    
  }
  
  public static  void main(String [] args) {
    new AddressGiver();
  }
}
