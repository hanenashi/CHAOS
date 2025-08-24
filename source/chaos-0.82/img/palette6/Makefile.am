AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
giant.pcx\
goblin.pcx
PALETTENAME = palette6.pal
noinst_LIBRARIES = libpalette6.a
libpalette6_a_SOURCES = \
giant.pcx \
goblin.pcx \
palette6.pal 

libpalette6_a_LIBADD = \
giant.@RODATASUFF@ \
giant.map.@RODATASUFF@ \
goblin.@RODATASUFF@ \
goblin.map.@RODATASUFF@ \
palette6.pal.@RODATASUFF@ 

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

