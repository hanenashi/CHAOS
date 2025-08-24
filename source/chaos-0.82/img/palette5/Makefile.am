AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
dark_citadel.pcx\
magic_castle.pcx\
shadow_wood.pcx\
wraith.pcx
PALETTENAME = palette5.pal
noinst_LIBRARIES = libpalette5.a
libpalette5_a_SOURCES = \
dark_citadel.pcx \
magic_castle.pcx \
shadow_wood.pcx \
wraith.pcx \
palette5.pal 

libpalette5_a_LIBADD = \
dark_citadel.@RODATASUFF@ \
dark_citadel.map.@RODATASUFF@ \
magic_castle.@RODATASUFF@ \
magic_castle.map.@RODATASUFF@ \
shadow_wood.@RODATASUFF@ \
shadow_wood.map.@RODATASUFF@ \
wraith.@RODATASUFF@ \
wraith.map.@RODATASUFF@ \
palette5.pal.@RODATASUFF@ 

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

