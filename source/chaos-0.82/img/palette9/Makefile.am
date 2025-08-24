AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
armour.pcx\
bow.pcx\
knife.pcx\
shield.pcx\
sword.pcx\
wings.pcx\
wiz1.pcx\
wiz2.pcx\
wiz3.pcx\
wiz4.pcx\
wiz5.pcx\
wiz6.pcx\
wiz7.pcx\
wiz8.pcx
PALETTENAME = palette9.pal
noinst_LIBRARIES = libpalette9.a
libpalette9_a_SOURCES = \
armour.pcx \
bow.pcx \
knife.pcx \
shield.pcx \
sword.pcx \
wings.pcx \
wiz1.pcx \
wiz2.pcx \
wiz3.pcx \
wiz4.pcx \
wiz5.pcx \
wiz6.pcx \
wiz7.pcx \
wiz8.pcx \
palette9.pal 

libpalette9_a_LIBADD = \
armour.@RODATASUFF@ \
armour.map.@RODATASUFF@ \
bow.@RODATASUFF@ \
bow.map.@RODATASUFF@ \
knife.@RODATASUFF@ \
knife.map.@RODATASUFF@ \
shield.@RODATASUFF@ \
shield.map.@RODATASUFF@ \
sword.@RODATASUFF@ \
sword.map.@RODATASUFF@ \
wings.@RODATASUFF@ \
wings.map.@RODATASUFF@ \
wiz1.@RODATASUFF@ \
wiz1.map.@RODATASUFF@ \
wiz2.@RODATASUFF@ \
wiz2.map.@RODATASUFF@ \
wiz3.@RODATASUFF@ \
wiz3.map.@RODATASUFF@ \
wiz4.@RODATASUFF@ \
wiz4.map.@RODATASUFF@ \
wiz5.@RODATASUFF@ \
wiz5.map.@RODATASUFF@ \
wiz6.@RODATASUFF@ \
wiz6.map.@RODATASUFF@ \
wiz7.@RODATASUFF@ \
wiz7.map.@RODATASUFF@ \
wiz8.@RODATASUFF@ \
wiz8.map.@RODATASUFF@ \
palette9.pal.@RODATASUFF@ 

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

