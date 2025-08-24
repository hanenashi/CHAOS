/* Parse command line options */
#include "platform.h"

#ifdef __WINDOWS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"

#include "cmdline.h"

#include "network.h"
#include "options.h"
#include "wmhandler.h"
#include "gfx.h"
#include "spelldata.h"
#include "chaos.h"

struct option cmdline_opts[] = {
  { "connect", optional_argument, 0, 'c' },
  { "host", no_argument, 0, 's' },
  { "help", no_argument, 0, '?' },
  { "off", no_argument, (int*) &Options[OPT_OLD_BUGS], 0 },
  { "on", no_argument, (int*) &Options[OPT_OLD_BUGS], 1 },
  { "old-bugs", required_argument, 0, 'o' },
  { "rounds", required_argument, 0, 'r' },
  { "forever", no_argument, (int*) &Options[OPT_ROUND_LIMIT], 0 },
  { "scale", no_argument, 0 , 'x' },
  { "scale-1", no_argument, &gfx_scale , 1 },
  { "scale-2", no_argument, &gfx_scale , 2 },
  { "scale-3", no_argument, &gfx_scale , 3 },
  { "icon", required_argument, 0, 'i' },
  { "full-screen", no_argument, 0, 'f' },
  { "port", required_argument, 0, 'p'},
  { NULL, no_argument, NULL, 0 }
};
void print_usage(void) {
  printf("Usage: chaos [OPTIONS]...\n\n");
  
  printf("Chaos - The Battle of Wizards by Julian Gollop\n");
  printf("Turn-based magical combat for up to 8 players\n\n");

  printf("Examples: \n");
  printf(" chaos --connect          # Connect to game, address from chaos.ip\n");
  printf(" chaos -c178.98.11.5      # Connect to host on the address 178.98.11.5\n");
  printf(" chaos -sx2 --full-screen # full screen mode, 2x scale, host a game\n\n");

  printf("If a long option shows an argument as mandatory, then it is mandatory\n");
  printf("for the equivalent short option also.  Similarly for optional arguments.\n\n");
  
  printf("Display options: \n");
  printf("-f, --full-screen       Run in full screen mode\n");
  printf("-x, --scale=SCALE       Set scale 1x - 3x\n");
  printf("    --scale-1\n");
  printf("    --scale-2\n");
  printf("    --scale-3\n");
  printf("-i, --icon=SPELL        Set application icon to the spell number.\n");
  printf("                        Accepts values from %d to %d inclusive.\n\n", 
      SPELL_KING_COBRA,SPELL_WALL);
  printf("Gameplay options: \n");
  printf("-o, --old-bugs=on       Set old bugs 'on' or 'off'\n");
  printf("    --on\n");
  printf("    --off\n");
  printf("-r, --rounds=ROUNDS     Set number of rounds played:  \n");
  printf("    --forever            0    infinite rounds\n");
  printf("                         1    players*2+15 rounds (default)\n");
  printf("                         any  maximum rounds (e.g. 25)\n\n");
  printf("Network options: \n");
  printf("-c, --connect[=IP]      Connect to an online host.\n");
  printf("                        By default, connects to address in chaos.ip\n");
  printf("                        If that file can't be found, 'localhost' is used.\n");
  printf("                        Optional IP address argument is given priority.\n");
  printf("-p, --port=PORT         Use non-zero PORT as the \n");
  printf("                        connection port (default %d)\n",NET_PORT);
  printf("-s, --host              Start hosting an online game\n");
}

void set_servername(void) {
#ifdef NETWORK
  char * tmp  = read_ip_file("chaos.ip");
  if (tmp != NULL) {
    serverName = tmp;
  } else {
    serverName = (char*)malloc(strlen(defaultServerName));
    strcpy(serverName,defaultServerName);
  }
#endif
} 

int parse_opts(int argc, char * argv[]) {

  int op = -1;
  int usage = 0;
  while((op = getopt_long(argc,
                          argv,
                          "c::fi:o:p:r:sx:",
                          cmdline_opts,
                          NULL)) != -1)
  {
    switch(op) {
      case 'c':

#ifndef NETWORK
        usage = 1;
        break; 
#else
	if (Options[OPT_NET] != 0) {
	  usage = 1;
	  break; 
	}
	if(optarg == NULL) {
	  set_servername();
	} else { //if(optarg != NULL) 
	  free(serverName);
	  serverName = (char*)malloc(strlen(optarg));
	  strcpy(serverName,optarg);
	}
	printf("Will connect to: %s \n",serverName);
	Options[OPT_NET] = CONNECT_TO;
#endif
 //	connect_to(serverName);
	break;
      case 'f':
	isFullScreen = 1;
	break;
      case 'i':
        g_icon = atoi(optarg);
	if (g_icon < SPELL_KING_COBRA || g_icon > SPELL_WALL)
	  g_icon = SPELL_GOOEY_BLOB;
	break;
      case 'o':
	if (strcmp(optarg, "on") == 0) {
	  Options[OPT_OLD_BUGS] = 1;
	} else {
	  Options[OPT_OLD_BUGS] = 0;
	}
	break;
      case 'p':
#ifdef NETWORK
        g_server_port = atoi(optarg);
        g_server_port = g_server_port?g_server_port:NET_PORT; 
#endif
        break;
      case 'r':
	Options[OPT_ROUND_LIMIT] = atoi(optarg);
	break;
      case 's':
	if (Options[OPT_NET] != 0) {
	  usage = 1;
	  break; 
	}
	Options[OPT_NET] = BE_HOST;
//	start_host();
//	connect_to(NET_HOSTNAME);
	break;
      case 'x':
	gfx_scale = atoi(optarg);
	switch (gfx_scale) {
	  case 1: case 2: case 3: 
	    break;
	  default:
	    printf("Unknown graphics scale: %s\n", optarg);
	    gfx_scale = 1;
	    break;
	}
	break;
      case '?':
	  usage = 1;
	  break; 
      default:
	if (Options[OPT_NET] != 0) {
	  usage = 1;
	  break; 
	}
	break;
    }
  }
  /*
  if (gfx_scale > 1 && gfx_scale < 4) {
    setNewScale(gfx_scale);
  }
  */
  if (usage) {
    print_usage();
    return 1;
  }
  return 0;
}


#endif


