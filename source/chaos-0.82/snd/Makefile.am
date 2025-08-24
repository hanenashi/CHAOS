AUTOMAKE_OPTIONS = foreign


SUFFIXES= .wav

noinst_LIBRARIES = libsnd.a
libsnd_a_SOURCES = \
attack.wav \
beam.wav \
chosen.wav \
cpustart.wav \
electro.wav \
fire.wav \
gooey.wav \
justice.wav \
menu.wav \
range.wav \
scream.wav \
spellstep.wav \
spellsuccess.wav \
urgh.wav \
walk.wav 

libsnd_a_LIBADD = \
attack.@RODATASUFF@ \
beam.@RODATASUFF@ \
chosen.@RODATASUFF@ \
cpustart.@RODATASUFF@ \
electro.@RODATASUFF@ \
fire.@RODATASUFF@ \
gooey.@RODATASUFF@ \
justice.@RODATASUFF@ \
menu.@RODATASUFF@ \
range.@RODATASUFF@ \
scream.@RODATASUFF@ \
spellstep.@RODATASUFF@ \
spellsuccess.@RODATASUFF@ \
urgh.@RODATASUFF@ \
walk.@RODATASUFF@ 


%.all.rodata.o : %.wav 
	@SOX@ $< -r 16000 -s -b -c 1 $*.1.raw
	@PADPROG@ $*.1.raw -p 4 -u -fo $*.raw 
	@OBJCOPY@ @OBJCOPYFLAGS@ $*.raw $@
	@-rm $*.1.raw

