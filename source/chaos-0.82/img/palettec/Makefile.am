AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
chaosfont.pcx
PALETTENAME = palettec.pal
noinst_LIBRARIES = libpalettec.a
libpalettec_a_SOURCES = \
chaosfont.pcx \
palettec.pal 

libpalettec_a_LIBADD = \
chaosfont.@RODATASUFF@ \
chaosfont.map.@RODATASUFF@ \
palettec.pal.@RODATASUFF@ 

GFX2GBAFLAGS := -t8 -M -F -c16 -q



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

