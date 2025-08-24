

AUTOMAKE_OPTIONS = foreign

SUBDIRS = includes

SUFFIXES = .gba
bin_PROGRAMS = chaos
if GBA_BUILD
bin_PROGRAMS+= chaos.gba
endif

chaos_SOURCES = arena.c \
casting.c \
chaos.c \
chaos_sprites.c \
cmdline.c \
computer.c \
creature.c \
editname.c \
examine.c \
gamemenu.c \
gfx.c \
input.c \
lobby.c \
magic.c \
main.c \
movement.c \
network.c \
options.c \
players.c \
remote.c \
sound_data.c \
spelldata.c \
spellselect.c \
splash.c \
text16.c \
version.c \
winkeys.c \
winsound.c \
wizards.c \
wmhandler.c 

chaos_LDADD= @CHAOS_LIBS@ @SDL_LIBS@ -L. -linterrupt
if WIN32_BUILD
chaos_LDADD+=chaos.res
chaos_DEPENDENCIES=chaos.res
chaos_SOURCES+=chaos.ico
SUFFIXES+= .ico .rc .res
%.res : %.rc
	@WINDRES@ -i $< -I rc -o $@ -O coff
%.rc : %.ico
	@echo "A ICON MOVEABLE PURE LOADONCALL DISCARDABLE \"$<\"" > $@
	@echo -n "" >> $@
endif
chaos_CFLAGS=@MTHUMB_FLAGS@

if GBA_BUILD
noinst_PROGRAMS=chaos.gba
chaos_DEPENDENCIES=libinterrupt.a 
if HAVE_FIXTOOL

chaos.gba : chaos
	@OBJCOPY@ -v -O binary $< $*.tofix.gba
	@GBAFIX@ -cl -n "@PACKAGE@" -o $@ $*.tofix.gba
	@-rm $*.tofix.gba

else

chaos.gba : chaos
	@OBJCOPY@ -v -O binary $< $@ 

endif
endif

noinst_LIBRARIES=libinterrupt.a
libinterrupt_a_SOURCES= interrupt.arm.c 
libinterrupt_a_CFLAGS=@MARM_FLAGS@

