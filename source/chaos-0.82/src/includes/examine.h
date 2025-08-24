// examine.h

#ifndef EXAMINE_H
#define EXAMINE_H

void examine_board(void);
void examine_back(void);

// examine creature at this square, 
// when done retrun to board
void examine_square(unsigned char index);
// examine spell at this square
// when done, does nothing
void examine_spell(unsigned char index);

#endif

