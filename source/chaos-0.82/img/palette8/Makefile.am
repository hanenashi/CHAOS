AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
bolt_anim.pcx\
disappear.pcx\
fight.pcx\
fire_anim.pcx\
magic_wood.pcx
PALETTENAME = palette8.pal
noinst_LIBRARIES = libpalette8.a
libpalette8_a_SOURCES = \
bolt_anim.pcx \
disappear.pcx \
fight.pcx \
fire_anim.pcx \
magic_wood.pcx \
palette8.pal 

libpalette8_a_LIBADD = \
bolt_anim.@RODATASUFF@ \
bolt_anim.map.@RODATASUFF@ \
disappear.@RODATASUFF@ \
disappear.map.@RODATASUFF@ \
fight.@RODATASUFF@ \
fight.map.@RODATASUFF@ \
fire_anim.@RODATASUFF@ \
fire_anim.map.@RODATASUFF@ \
magic_wood.@RODATASUFF@ \
magic_wood.map.@RODATASUFF@ \
palette8.pal.@RODATASUFF@ 

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

