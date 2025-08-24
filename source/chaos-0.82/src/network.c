#include "platform.h"

#ifdef __WINDOWS__
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "remote.h"
#include "creature.h"
#include "arena.h"
// this should be the only place that SDL_net stuff is used
#ifdef NETWORK
#include "SDL_net.h"
#endif
#include "SDL_thread.h"

#include "lobby.h"
#include "winkeys.h"
#include "chaos.h"
#define CLIENT_MSG_SIZE  32
#define MESSAGE_INDEX  0
// seed goes server->client
// data goes client->server and server->(other)client
#define DATA_INDEX     4

#define MAX_CLIENTS 8

#ifdef NETWORK
const char * const defaultServerName = NET_HOSTNAME;
char * serverName;// = (char*) defaultServerName;

// arbitrary port choice..
int g_server_port = NET_PORT;
// the client socket set contains just 1 socket
SDLNet_SocketSet client_socketset = NULL;
// the server socket set contains all sockets connected + 1
// the +1 is the server listening socket
SDLNet_SocketSet server_socketset = NULL;

typedef struct {
  Uint32 active;
  TCPsocket socket;
} client_t;

client_t g_clients[MAX_CLIENTS];

// when the server sends us data, store it here
u8 g_serverMessage[SERVER_MSG_SIZE];
// when sending to the server, use this buffer
u8 g_clientMessage[CLIENT_MSG_SIZE];
// client-server socket
TCPsocket client_socket;
TCPsocket server_socket;


void handle_client_message(int clientid);

/******************************************************************
* This is the CLIENT code...
******************************************************************/

// the server sends a message, deal with it
int client_handle_server(void) {
  // client_socket holds the message
  int len = SDLNet_TCP_Recv(client_socket, (char *)g_serverMessage, SERVER_MSG_SIZE);
  if (len < 4) {
    // something has gone wrong...
    SDLNet_TCP_DelSocket(client_socketset, client_socket);
    SDLNet_TCP_Close(client_socket);
    client_socket = NULL;
    printf("Server connection lost\n");
    return NET_ERROR;
  } else {
    // all is well...
    return SDLNet_Read32(&g_serverMessage[MESSAGE_INDEX]);
  }
  
}
// wait to receive a server message
// returns the NET_ value above
int get_server_message(void){

  // this should wait until the server is ready (CheckSockets)
  // also remember to poll events! otherwise the gui breaks.
  
  // remember, this is "client" code, so the only socket is the
  // one to the server. Retrv data from this socket
  for (;;) 
  {
    // do a quick poll...
    SDLNet_CheckSockets(client_socketset, 0);
    if ( SDLNet_SocketReady(client_socket) ) {
      // the server has spoken to us...
      // return which msg type it sent 
      return client_handle_server();
    }
    // handle events and update screen
    UpdateKeys();
    wait_vsync_int();
    if (game_frames & 2) {
      if (current_screen == SCR_EXAMINE_BOARD || 
	  current_screen == SCR_CASTING || 
	  current_screen == SCR_MOVEMENT) 
      {
        animate_arena();
      }
    }
  }
  return NET_ERROR;
}
/**the server sends some data, retrieve the ints we are expecting*/
void server_data(int *data, int count) {
  // get bytes from g_serverMessage
  int i;
  for (i = 0; i < count; i++) 
    data[i] = SDLNet_Read32(&g_serverMessage[DATA_INDEX+i*4]);
  
}
/**
 *  Send a message to the server.
 *  @param message: the message id
 *  @param data   : some data value to send (spell id, arena index, etc)
 *  @param size   : how many values there are
 */
void send_message(int message, int * data, int size){
  // put data in g_clientMessage and send it...
  SDLNet_Write32(message, &g_clientMessage[MESSAGE_INDEX]);
  int i;
  for (i = 0; i < size; i++) 
    SDLNet_Write32(data[i], &g_clientMessage[DATA_INDEX+i*4]);

  // if we are just sending 1 data value, then send the 
  // server's current random seed as another value
//  if (size == 1)
//    SDLNet_Write32(GetSeed(), &g_clientMessage[DATA_INDEX+4]);
//  if (is_hosting()) {
    // yuck.. can't do it as nicely
    // no need to send it us just to receive it again though...
  //  handle_client_message(-1);//MAX_CLIENTS+1);
  //} else {
    SDLNet_TCP_Send(client_socket, g_clientMessage, CLIENT_MSG_SIZE);
  //}
}

void send_message_to_client(int client, int message,
    int *data, int size)
{
  if (g_clients[client].socket) {
    SDLNet_Write32(message, &g_clientMessage[MESSAGE_INDEX]);
    int i;
    for (i = 0; i < size; i++) 
      SDLNet_Write32(data[i], &g_clientMessage[DATA_INDEX+i*4]);

    // if we are just sending 1 data value, then send the 
    // server's current random seed as another value
   // this is no longer the way - the seed comes from whoever sends the message 
//    if (size == 1)
//      SDLNet_Write32(GetSeed(), &g_clientMessage[DATA_INDEX+4]);
    SDLNet_TCP_Send(g_clients[client].socket, g_clientMessage, CLIENT_MSG_SIZE);
  }
}


// called to send FROM client to server
// see handle_client_message for where it is dealt with on the server
void send_wizard(int img, int colour, const char * name) {
  SDLNet_Write32(NET_SET_WIZ, &g_clientMessage[MESSAGE_INDEX]);
  int index = DATA_INDEX;
  g_clientMessage[index++] = img&0xff;
  g_clientMessage[index++] = colour&0xff;
  while (*name) {
    g_clientMessage[index++] = *name++;
  }
  g_clientMessage[index++] = 0;
  SDLNet_TCP_Send(client_socket, g_clientMessage, CLIENT_MSG_SIZE);
} 

/*server send a message, retrieve the data
  player count is obvious
  id is the id of this client (i.e. which player should be flagged LOCAL)
 */
void server_player_count(unsigned char * playercount, int * id) {
    int tmp;
    tmp= SDLNet_Read32(&g_serverMessage[MESSAGE_INDEX]);
    if (tmp == NET_SET_COUNT) {
      int index = DATA_INDEX;
      tmp= SDLNet_Read32(&g_serverMessage[index]);
      index += 4;
      *playercount = tmp&0xf;
      tmp= SDLNet_Read32(&g_serverMessage[index]);
      *id = tmp;
    }
}

void  server_get_wizard(unsigned short * colour ,  unsigned char * image,  char * name) {
  int tmp;
  tmp= SDLNet_Read32(&g_serverMessage[MESSAGE_INDEX]);
  if (tmp == NET_SET_WIZ) {
    int index = DATA_INDEX;
    tmp= SDLNet_Read32(&g_serverMessage[index]);
    *colour = tmp;
    index += 4;
    tmp= SDLNet_Read32(&g_serverMessage[index]);
    *image = tmp;
    index += 4;
    *name = 0;
    while (g_serverMessage[index]) 
      *name++ = g_serverMessage[index++];
    *name = 0;
  }
}

void client_disconnect(void) {
  // a client leaves...
  SDLNet_TCP_DelSocket(client_socketset, client_socket);
  SDLNet_TCP_Close(client_socket);
  client_socket = NULL;
}

// call this to connect to the host address
int connect_to(const char * hostname) {
  
  IPaddress ipaddress;
  
  //1. Create Socket Set (which just has 1 socket - the one we connect to the server)
  if ( client_socketset == NULL ) {
    client_socketset = SDLNet_AllocSocketSet(1);
    if ( client_socketset == NULL ) {
      printf("SDLNet_AllocSocketSet (client): Couldn't create socket set: %s\n",
	SDLNet_GetError());
      return -1;
    }
  }
  //2. Create an IPaddress for the server port
  if (SDLNet_ResolveHost(&ipaddress, hostname, g_server_port) == -1) {
    printf("SDLNet_ResolveHost (client): %s \"%s\"\n", SDLNet_GetError(),hostname);
    // disable network in gui...
    return -1; 
  }
  //3. Create a socket by opening that IPaddress
  client_socket=SDLNet_TCP_Open(&ipaddress);
  if(!client_socket) {
    printf("SDLNet_TCP_Open (client): %s \"%s\"\n", SDLNet_GetError(),hostname);
    return -1;
  }
  //4. Add the socket to the Socket Set
  if( SDLNet_TCP_AddSocket(client_socketset, client_socket) == -1) {
    // error
    printf("SDLNet_TCP_AddSocket(client): %s\n", SDLNet_GetError());
    return -1;
  }

#ifdef DEBUG_NET
  printf("Client connected to \"%s\" \n",hostname);
#endif
  // at this stage, we have a valid connection to the host.
  // e.g. could call this in the "test" procedure
  
  //4a. When you want to, send some data to the server
  //5. Monitor connections on the client socket set (CheckSockets)
  //6. If the socket connected to the server is ready, read the data it sends (may send us data or whatever)
  return 0;
}

/******************************************************************
* This is the SERVER code...
******************************************************************/
int g_isHosting = 0;
SDL_Thread * g_monitorThread = NULL;
int g_monitor = 0;

// call this when we select "connect to"
// or anytime we want to stop hosting
void stop_host(void) {
  if (g_isHosting) {
    // exit the thread if active...
    g_monitor = 0;
    if (g_monitorThread != NULL) {
      int status;
      SDL_WaitThread(g_monitorThread, &status);
      g_monitorThread = NULL;
    }
    // tidy up the hosting side of things...
    if (server_socket != NULL) {
      SDLNet_TCP_Close(server_socket);
      server_socket = NULL;
    }
    if (server_socketset != NULL) {
      SDLNet_FreeSocketSet(server_socketset);
      server_socketset = NULL;
    }
    g_isHosting = 0;
  }
}


void server_handle_server(void) {
  // the server receives a jolt on its server socket.
  // Who's there?
  TCPsocket newsock;
  newsock = SDLNet_TCP_Accept(server_socket);
#ifdef DEBUG_NET
  printf("A Client is connecting...\n"); 
#endif
  if (newsock == NULL) {
    // error handling...
    printf("SDLNet_TCP_Accept (server): %s\n",SDL_GetError()); 
    g_monitor = 0;
    return;
  }
  // find a client slot
  int available;
  for( available = 0; available < MAX_CLIENTS; ++available) {
    if (g_clients[available].socket==NULL) {
      break;
    }
  }
  // so we have our available slot
  if (available < MAX_CLIENTS) {
    // add it
    g_clients[available].socket = newsock;
    SDLNet_TCP_AddSocket(server_socketset, g_clients[available].socket);
    printf("... added to sock set.\n"); 
  } else {
    // full!
    printf("... couldn't add to socket set (full?)\n"); 
  }

}


void  remember_spell(int clientid) {
  // remember the spell we are sent...
#ifdef DEBUG_NET
  printf("Remeber the spell\n");
#endif
  if (clientid < MAX_CLIENTS) {
    int spell = SDLNet_Read32(&g_clientMessage[DATA_INDEX]);
    int illusion = SDLNet_Read32(&g_clientMessage[DATA_INDEX+4]);
    receive_spellinfo(clientid,  spell, illusion);
  } 
} 

void handle_client_message(int clientid) {
  // they sent some message. relay it to the other clients...
  int message_type = SDLNet_Read32(g_clientMessage);
  int i;

  switch (message_type) {
    case NET_SPELL_CAST:
    case NET_SPELL_CANCEL:
    case NET_DISMOUNT:
    case NET_MOVE_A:
    case NET_MOVE_B:
    case NET_MOVE_START:
      // all these require redirection
#ifdef DEBUG_NET
      printf("Client Message: redirect to all clients %d\n", message_type); 
#endif
      for (i = 0; i < MAX_CLIENTS; i++) {
	if (g_clients[i].socket != NULL && i != clientid) {
	  // relay the message on to the client socket...
	  SDLNet_TCP_Send(g_clients[i].socket, g_clientMessage, CLIENT_MSG_SIZE);
	}
      }
      break;
    case NET_SET_WIZ:
      // if this happens, check we are on the lobby screen and call 
      // set player
#ifdef DEBUG_NET
      printf("Client Message: set player\n"); 
#endif
      set_lobby_player(clientid,&g_clientMessage[DATA_INDEX]);
      break;
    case NET_SPELL_CHOICE:
      // the spell choice... the server must remember all choices
      // until everyone has replied, then let everyone know
      // who has chosen what (keeps stuff synch'd that way)
#ifdef DEBUG_NET
      printf("Client Message: spell choice\n"); 
#endif
      remember_spell(clientid); 
    default:
#ifdef DEBUG_NET
      printf("Client Message: unhandled (default) #%d\n",message_type); 
#endif
      break;
  } 
}

void server_handle_client(int clientid) {
  // clientid has done something.
  // e.g. pressed a, b start..
#ifdef DEBUG_NET
  printf("server_handle_client \n"); 
#endif
  if ( SDLNet_TCP_Recv(g_clients[clientid].socket, g_clientMessage,
       	CLIENT_MSG_SIZE) <= 0 )
  {
    // the received data is invalid
    SDLNet_TCP_DelSocket(server_socketset, g_clients[clientid].socket);
    SDLNet_TCP_Close(g_clients[clientid].socket);
    g_clients[clientid].socket = NULL;
    client_disconnected(clientid);
    printf("Received data invalid - client disconnected?\n"); 
  } else {
    handle_client_message(clientid);
  }
}
// this should run in a seperate thread...
int monitor_clients(void* v) {
  // monitor the clients
  // whenever something happens, act on it
  // 5. Monitor connections on the socket set (CheckSockets)
  // 6. If the Server Socket is ready, accept the connection and add the connected socket to the socket set
  // 7. If a client socket is ready, retrieve the data or disconnect the socket, as necessary
  // 8. "broadcast" the data to all connected clients?
  int i;
  while (g_monitor) {
    SDLNet_CheckSockets(server_socketset, ~0);
    if ( SDLNet_SocketReady(server_socket) ) {
      // a client has connected. That is all server sockets look for
      server_handle_server();
    }
    for (i = 0; i < MAX_CLIENTS; i++) {
      if (g_clients[i].socket) {
        // this client is active...
        if (SDLNet_SocketReady(g_clients[i].socket)) {
          // a client has spoken to us... see what they want
#ifdef DEBUG_NET
          printf("Client %d is calling...\n",i); 
#endif
          server_handle_client(i);
        }
      }
    }
  }
  return 0;
}

// call this when we are to host 
void start_host(void) {
  // are we already hosting?
  stop_host();
  int i;
  // initialise the client slots
  for (i = 0; i < MAX_CLIENTS; i++) {
    g_clients[i].socket = NULL;
    g_clients[i].active = 0;
  }
  
  IPaddress ipaddress;
  // 1. Create Socket Set size = 1+max_clients
  if ( server_socketset == NULL ) {
    server_socketset = SDLNet_AllocSocketSet(MAX_CLIENTS + 1);
    if ( server_socketset == NULL ) {
      printf("SDLNet_AllocSocketSet (server): Couldn't create server socket set. %s\n",
	  SDLNet_GetError());
      return;
    }
  }
  // 2. Create an IPaddress for a port
  if (SDLNet_ResolveHost(&ipaddress, NULL, g_server_port) == -1) {
    printf("SDLNet_ResolveHost (server): %s\n", SDLNet_GetError());
    // disable network in gui...
    return; 
  }
  // 3. Create a socket by opening that IPaddress
  server_socket=SDLNet_TCP_Open(&ipaddress);
  if(!server_socket) {
    printf("SDLNet_TCP_Open (server): %s\n", SDLNet_GetError());
    return;
  }
  // 4. Add the socket to the Socket Set
  if( SDLNet_TCP_AddSocket(server_socketset, server_socket) == -1) {
    // error
    printf("SDLNet_TCP_AddSocket(server): %s\n", SDLNet_GetError());
    return;
  }
  g_isHosting = 1;
  g_monitor = 1;
  // start the monitor thread...
  g_monitorThread = SDL_CreateThread(monitor_clients, (void*)NULL);
#ifdef DEBUG_NET
  printf("\nStarted Server\n"); 
#endif
}

void send_player_count(int client, int count) {
  // server sends a message to the client
  if (g_clients[client].socket != NULL) {
    SDLNet_Write32(NET_SET_COUNT, &g_serverMessage[MESSAGE_INDEX]);
    int index = DATA_INDEX;
    SDLNet_Write32(count, &g_serverMessage[index]);
    index += 4;
    SDLNet_Write32((client), &g_serverMessage[index]);
    SDLNet_TCP_Send(g_clients[client].socket, g_serverMessage, SERVER_MSG_SIZE);
  }
}
void  send_player_details(int client, 
	     int colour, 
	     int image, 
	     char* name)
{
  if (g_clients[client].socket != NULL) {
    SDLNet_Write32(NET_SET_WIZ, &g_serverMessage[MESSAGE_INDEX]);
    int index = DATA_INDEX;
    SDLNet_Write32(colour, &g_serverMessage[index]);
    index += 4;
    SDLNet_Write32(image, &g_serverMessage[index]);
    index += 4;
    g_serverMessage[index] = 0; 
    while (*name) 
      g_serverMessage[index++] = *name++;
    g_serverMessage[index++] = 0;
    SDLNet_TCP_Send(g_clients[client].socket, g_serverMessage, SERVER_MSG_SIZE);
  }
}
void send_spell_choice(int client, int player, int spell, 
	      int illusion)
{
  // the server sends a message about spell selection to a client
  if (g_clients[client].socket != NULL) {
    SDLNet_Write32(NET_SPELL_CHOICE, &g_serverMessage[MESSAGE_INDEX]);
    int index = DATA_INDEX;
    SDLNet_Write32(player, &g_serverMessage[index]);
    index += 4;
    SDLNet_Write32(spell, &g_serverMessage[index]);
    index += 4;
    SDLNet_Write32(illusion, &g_serverMessage[index]);
    index += 4;
    SDLNet_TCP_Send(g_clients[client].socket, g_serverMessage, SERVER_MSG_SIZE);
  }
  
}

void server_spell(int *who, int *spell, int *illusion)
{
  *who   = SDLNet_Read32(&g_serverMessage[DATA_INDEX]);
  *spell = SDLNet_Read32(&g_serverMessage[DATA_INDEX+4]);
  *illusion = SDLNet_Read32(&g_serverMessage[DATA_INDEX+8]);
}
// this will return either 1 for hosting, or 0 for not listening
// this is independent of our choice, because the listening may
// fail for some reason (port blocked, not enough privelege, etc)
int is_hosting(void) {
  return g_monitor && (server_socket!= NULL);
}

// this will return either 1 when connected, or 0 for not connected
// this obviously depends on many factors, regardless of choice
// the likelihood of a connection succeeding is, woo, 2%? :)
int is_connected(void) {
  return (client_socket!= NULL);
}

int client_online(int i) {
  return g_clients[i].socket != NULL;
}

char * read_ip_file(const char * filename) {
  SDL_RWops* file =  SDL_RWFromFile(filename, "r");
  if (file != NULL) {
    int size = SDL_RWseek(file, 0, SEEK_END);
    SDL_RWseek(file, 0, SEEK_SET);
    char * ip = (char*)malloc(size+1); 
    SDL_RWread(file, ip, size, 1);
    SDL_RWclose(file);
    ip[size] = 0;
    char *nl = strpbrk(ip, "\n");
    if (nl)
      *nl = 0;
    return ip;
  }
  printf("SDL_RWops (IP address file) %s\n", SDL_GetError());
  return NULL;
}

#endif
#endif


