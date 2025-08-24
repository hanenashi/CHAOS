#!/bin/sh

# create automake files for the palette directories
# the old makefiles "old.make" were hand coded versions

for i in `ls -1d palette*`
do
  filename=$i/Makefile.am
  rm -f ${filename}

  # first the setup - files etc
  echo "AUTOMAKE_OPTIONS = foreign" >> ${filename}
  echo "SUFFIXES= .pal .pcx .map .raw" >> ${filename}
  echo -n "CURRENTIMGS = " >> ${filename}
  ls $i/*.pcx -1 | awk -F'/' '{ printf "\\\n%s",$2 }'  >> ${filename}
  echo "" >> ${filename}
  echo "PALETTENAME = $i.pal" >> ${filename}
  echo "noinst_LIBRARIES = lib$i.a" >> ${filename}
  echo "lib${i}_a_SOURCES = \\" >> ${filename}
  ls $i/*.pcx -1 | awk -F'/' '{print $2" \\"}' >> ${filename}
  echo "$i.pal " >> ${filename}
  echo "" >> ${filename}

  echo "lib${i}_a_LIBADD = \\" >> ${filename}
  ls $i/*.pcx -1 | awk -F'/' '{split($2,str,"."); print str[1]".@RODATASUFF@ \\"; \
  print str[1]".map.@RODATASUFF@ \\"}' >> ${filename}
  echo "$i.pal.@RODATASUFF@ " >> ${filename}
  echo "" >> ${filename}
  
  gfxflags=`grep GFX2GBA $i/old.make`
  if [ $? == 0 ] ; then
    echo ${gfxflags} >> ${filename}
  else
    echo "GFX2GBAFLAGS = -t8 -m -F -c16 -q" >> ${filename}
  fi

  # end of setup, the rules
  echo "" >> ${filename}
  echo "" >> ${filename}
  cat mkimg.mk.am >> ${filename}

done


