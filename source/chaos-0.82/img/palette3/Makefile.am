AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
bear.pcx\
centaur.pcx\
faun.pcx\
horse.pcx\
orc.pcx\
wall.pcx
PALETTENAME = palette3.pal
noinst_LIBRARIES = libpalette3.a
libpalette3_a_SOURCES = \
bear.pcx \
centaur.pcx \
faun.pcx \
horse.pcx \
orc.pcx \
wall.pcx \
palette3.pal 

libpalette3_a_LIBADD = \
bear.@RODATASUFF@ \
bear.map.@RODATASUFF@ \
centaur.@RODATASUFF@ \
centaur.map.@RODATASUFF@ \
faun.@RODATASUFF@ \
faun.map.@RODATASUFF@ \
horse.@RODATASUFF@ \
horse.map.@RODATASUFF@ \
orc.@RODATASUFF@ \
orc.map.@RODATASUFF@ \
wall.@RODATASUFF@ \
wall.map.@RODATASUFF@ \
palette3.pal.@RODATASUFF@ 

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

