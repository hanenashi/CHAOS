#ifndef network_h_seen
#define  network_h_seen


#ifdef NETWORK
#define ONLINE_MODE  ( (Options[OPT_NET] > 0) && (is_connected() || is_hosting()) ) 
#else
#define ONLINE_MODE  ( 0 ) 
#endif
#define NET_HOSTNAME     "localhost"
#define NET_PORT         1999
#define NET_SPELL_CAST    1
#define NET_SPELL_CANCEL  2
#define NET_SPELL_CHOICE  4
#define NET_MOVE_A        8
#define NET_DISMOUNT      9
#define NET_MOVE_B        16
#define NET_MOVE_START    32
#define NET_SET_WIZ       64
#define NET_SET_COUNT     65
#define NET_WAKE_UP       66
#define NET_ERROR         -1

#define CONNECT_TO 2
#define BE_HOST 1

#define SERVER_MSG_SIZE  128
extern char * serverName;
extern const char * const defaultServerName;

extern int g_server_port;
extern u8 g_serverMessage[SERVER_MSG_SIZE];
// wait to receive a server message
// returns the NET_ value above
int get_server_message(void);
/**the server sends a target and random seed... retrieve them*/
//void server_target(int *index, int *seed);
void server_data(int *data, int count);
/**
 *  Send a message to the server.
 *  @param message: the message id
 *  @param data   : some data value to send (spell id, arena index, etc)
 */
void send_message(int message, int * data, int size);
void send_message_to_client(int client, int message,
    int *data, int size);

void send_wizard(int img, int colour, const char * name);

// connect to the server 
int connect_to(const char * hostname);
// be a server
void start_host(void) ;

int is_connected(void);
int is_hosting(void);

int client_online(int i);

void server_player_count(unsigned char * playercount, int * id);
void  server_get_wizard(unsigned short * colour ,  unsigned char * image,  char * name);
void send_player_count(int client, int count);
void  send_player_details(int client, 
	     int colour, 
	     int image, 
	     char* name);
void send_spell_choice(int client, int player, int spell, 
	      int illusion);
void server_spell(int *who, int *spell, int *illusion);

// read an IP file...
char * read_ip_file(const char * file);
#endif
