// options.h

#ifndef OPTIONS_H
#define OPTIONS_H

#define OPT_ROUND_LIMIT 0
#define OPT_OLD_BUGS 1
#define OPT_RESET 2
#define OPT_SOUND 3

#ifdef __GBA__

#define OPT_BACK 4
#define OPTION_COUNT 5

#elif defined (__WINDOWS__)

#define OPT_NET 4
#define OPT_BACK 5
#define OPTION_COUNT 6

#endif

#define DEFAULT_ROUNDS 1

extern unsigned int Options[OPTION_COUNT];

void set_default_options(void);
void show_options(void);
void animate_options(void);

void options_up(void);
void options_down(void);
void options_a(void);
void options_left(void);
void options_right(void);
void options_back(void);

#endif // OPTIONS_H



