AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
cursor_engaged.pcx\
cursor_fire.pcx\
cursor_fly.pcx\
cursor_ground.pcx\
cursor.pcx\
cursor_spell.pcx
PALETTENAME = paletteb.pal
noinst_LIBRARIES = libpaletteb.a
libpaletteb_a_SOURCES = \
cursor_engaged.pcx \
cursor_fire.pcx \
cursor_fly.pcx \
cursor_ground.pcx \
cursor.pcx \
cursor_spell.pcx \
paletteb.pal 

libpaletteb_a_LIBADD = \
cursor_engaged.@RODATASUFF@ \
cursor_engaged.map.@RODATASUFF@ \
cursor_fire.@RODATASUFF@ \
cursor_fire.map.@RODATASUFF@ \
cursor_fly.@RODATASUFF@ \
cursor_fly.map.@RODATASUFF@ \
cursor_ground.@RODATASUFF@ \
cursor_ground.map.@RODATASUFF@ \
cursor.@RODATASUFF@ \
cursor.map.@RODATASUFF@ \
cursor_spell.@RODATASUFF@ \
cursor_spell.map.@RODATASUFF@ \
paletteb.pal.@RODATASUFF@ 

GFX2GBAFLAGS := -t8 -m -F -q



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

