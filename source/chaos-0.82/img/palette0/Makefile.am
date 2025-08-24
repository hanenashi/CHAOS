AUTOMAKE_OPTIONS = foreign
SUFFIXES= .pal .pcx .map .raw
CURRENTIMGS = \
crocodile.pcx\
dragon_green.pcx\
elf.pcx\
gooey_blob.pcx\
hydra.pcx\
king_cobra.pcx\
spell.pcx
PALETTENAME = palette0.pal
noinst_LIBRARIES = libpalette0.a
libpalette0_a_SOURCES = \
crocodile.pcx \
dragon_green.pcx \
elf.pcx \
gooey_blob.pcx \
hydra.pcx \
king_cobra.pcx \
spell.pcx \
palette0.pal 

libpalette0_a_LIBADD = \
crocodile.@RODATASUFF@ \
crocodile.map.@RODATASUFF@ \
dragon_green.@RODATASUFF@ \
dragon_green.map.@RODATASUFF@ \
elf.@RODATASUFF@ \
elf.map.@RODATASUFF@ \
gooey_blob.@RODATASUFF@ \
gooey_blob.map.@RODATASUFF@ \
hydra.@RODATASUFF@ \
hydra.map.@RODATASUFF@ \
king_cobra.@RODATASUFF@ \
king_cobra.map.@RODATASUFF@ \
spell.@RODATASUFF@ \
spell.map.@RODATASUFF@ \
palette0.pal.@RODATASUFF@ 

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

