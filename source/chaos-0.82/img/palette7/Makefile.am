AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
gorilla.pcx\
spectre.pcx\
unicorn.pcx\
zombie.pcx
PALETTENAME = palette7.pal
noinst_LIBRARIES = libpalette7.a
libpalette7_a_SOURCES = \
gorilla.pcx \
spectre.pcx \
unicorn.pcx \
zombie.pcx \
palette7.pal 

libpalette7_a_LIBADD = \
gorilla.@RODATASUFF@ \
gorilla.map.@RODATASUFF@ \
spectre.@RODATASUFF@ \
spectre.map.@RODATASUFF@ \
unicorn.@RODATASUFF@ \
unicorn.map.@RODATASUFF@ \
zombie.@RODATASUFF@ \
zombie.map.@RODATASUFF@ \
palette7.pal.@RODATASUFF@ 

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

