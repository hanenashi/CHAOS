AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
dragon_red.pcx\
ghost.pcx\
magic_fire.pcx\
ogre.pcx\
skeleton.pcx
PALETTENAME = palette4.pal
noinst_LIBRARIES = libpalette4.a
libpalette4_a_SOURCES = \
dragon_red.pcx \
ghost.pcx \
magic_fire.pcx \
ogre.pcx \
skeleton.pcx \
palette4.pal 

libpalette4_a_LIBADD = \
dragon_red.@RODATASUFF@ \
dragon_red.map.@RODATASUFF@ \
ghost.@RODATASUFF@ \
ghost.map.@RODATASUFF@ \
magic_fire.@RODATASUFF@ \
magic_fire.map.@RODATASUFF@ \
ogre.@RODATASUFF@ \
ogre.map.@RODATASUFF@ \
skeleton.@RODATASUFF@ \
skeleton.map.@RODATASUFF@ \
palette4.pal.@RODATASUFF@ 

GFX2GBAFLAGS = -t8 -m -F -c16 -q



%.pal:
	@GFX2GBA@ $(GFX2GBAFLAGS) -G -D -p$@  $(CURRENTIMGS)

%.map: %.pcx
	@GFX2GBA@ $(GFX2GBAFLAGS) -G -P  $(CURRENTIMGS)

%.raw: %.pcx
	@GFX2GBA@ $(GFX2GBAFLAGS) -P -D  $(CURRENTIMGS)

# create gcc object from raw binary file
%.@RODATASUFF@: %.raw
	@OBJCOPY@ @OBJCOPYFLAGS@ $< $@

# create gcc object from palette file
%.pal.@RODATASUFF@: %.pal
	@OBJCOPY@ @OBJCOPYFLAGS@ $< $@

# create gcc object from map file
%.map.@RODATASUFF@: %.map
	@OBJCOPY@ @OBJCOPYFLAGS@ $< $@

