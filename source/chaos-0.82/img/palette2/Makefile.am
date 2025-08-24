AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
dragon_golden.pcx\
eagle.pcx\
lion.pcx\
manticore.pcx
PALETTENAME = palette2.pal
noinst_LIBRARIES = libpalette2.a
libpalette2_a_SOURCES = \
dragon_golden.pcx \
eagle.pcx \
lion.pcx \
manticore.pcx \
palette2.pal 

libpalette2_a_LIBADD = \
dragon_golden.@RODATASUFF@ \
dragon_golden.map.@RODATASUFF@ \
eagle.@RODATASUFF@ \
eagle.map.@RODATASUFF@ \
lion.@RODATASUFF@ \
lion.map.@RODATASUFF@ \
manticore.@RODATASUFF@ \
manticore.map.@RODATASUFF@ \
palette2.pal.@RODATASUFF@ 

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

