AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
bat.pcx\
dire_wolf.pcx\
giant_rat.pcx\
gryphon.pcx\
harpy.pcx\
pegasus.pcx\
troll.pcx\
vampire.pcx
PALETTENAME = palette1.pal
noinst_LIBRARIES = libpalette1.a
libpalette1_a_SOURCES = \
bat.pcx \
dire_wolf.pcx \
giant_rat.pcx \
gryphon.pcx \
harpy.pcx \
pegasus.pcx \
troll.pcx \
vampire.pcx \
palette1.pal 

libpalette1_a_LIBADD = \
bat.@RODATASUFF@ \
bat.map.@RODATASUFF@ \
dire_wolf.@RODATASUFF@ \
dire_wolf.map.@RODATASUFF@ \
giant_rat.@RODATASUFF@ \
giant_rat.map.@RODATASUFF@ \
gryphon.@RODATASUFF@ \
gryphon.map.@RODATASUFF@ \
harpy.@RODATASUFF@ \
harpy.map.@RODATASUFF@ \
pegasus.@RODATASUFF@ \
pegasus.map.@RODATASUFF@ \
troll.@RODATASUFF@ \
troll.map.@RODATASUFF@ \
vampire.@RODATASUFF@ \
vampire.map.@RODATASUFF@ \
palette1.pal.@RODATASUFF@ 

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

