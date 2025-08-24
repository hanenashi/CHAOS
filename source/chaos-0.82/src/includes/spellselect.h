// spellselect.h

#ifndef SPELLSELECT_H
#define SPELLSELECT_H

extern char cast_chance_needed;

void show_spell_screen(void);
void spell_select_up(void);
void spell_select_down(void);
void spell_select_left(void);
void spell_select_right(void);
void spell_select_a(void);
void spell_select_b(void);
void spell_select_r(void);

void anim_spell_select(void);
void remove_null_spells(void);

#endif

