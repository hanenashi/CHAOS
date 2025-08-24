
AC_PREREQ(2.59)
AC_INIT(src/chaos.c)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(chaos, 0.82)


AC_ARG_ENABLE(sdl_net,
                AC_HELP_STRING([--enable-network],[enable network play (default is yes)]),
                  , enable_sdl_net=yes)
AC_ARG_ENABLE(sound,
                AC_HELP_STRING([--enable-sound],[enable ound effects(default is yes)]),
                  , enable_sound=yes)
AC_ARG_ENABLE(debug,
                AC_HELP_STRING([--enable-debug],[enable debug mode (default no)]),
                  , enable_debug=no)


# check for cross compilation
OBJCOPY=objcopy
WINDRES=windres
CC_TARGET=$host_alias

if test "x$host" != "x$build"; then
  echo "cross compiling on $build to $host_alias .."
  OBJCOPY=${CC_TARGET}-objcopy
  WINDRES=${CC_TARGET}-windres
  AC_ARG_WITH(gbalib,
              AC_HELP_STRING([--with-gbalib],[Path to Quirky's gbalib]),
              , with_gbalib=$withval)
fi

case "$host" in
  arm-*elf*)
        enable_sdl=no
        enable_sdl_net=no
        enable_gba_lib=yes
        PREFIX="${gba_prefix}"
        OBJCOPYFLAGS="${OBJCOPYFLAGS} -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata,readonly,contents"
        dnl arm is only suitable on GBA
        MARM_FLAGS="-marm"
        MTHUMB_FLAGS="-mthumb"
        EXE_NAME="chaos.gba"
        ;;
  *)
        enable_sdl=yes
        enable_gba_lib=no
        OBJCOPYFLAGS="-I binary -O ${target} -B i386 --rename-section .data=.rodata,readonly,data,contents"
        MARM_FLAGS="" 
        MTHUMB_FLAGS="" 
        ;;
esac

case "$target" in
  *mingw32*)
    win32_build="yes"
  ;;
esac

AM_CONDITIONAL(WIN32_BUILD, test "x$win32_build" = "xyes" )

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_RANLIB
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_C_VOLATILE
# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit strpbrk])

AC_PATH_PROG(GFX2GBA, gfx2gba)

if test "x$GFX2GBA" = x; then
  AC_MSG_ERROR([*** Couldn't find gfx2gba program. http://www.gbadev.org/tools.php?section=gfx ])
fi
AC_PATH_PROG(PADPROG, pad)
if test "x$PADPROG" = x; then
  AC_MSG_ERROR([*** Couldn't find pad program. See scripts directory for source.])
fi
if test "x$enable_sound" = xyes ; then
  AC_PATH_PROG(SOX, sox)
  if test "x$SOX" = x; then
    AC_MSG_ERROR([*** Couldn't find SoX Sound eXchange. http://sox.sourceforge.net/ or disable sound])
  fi
else
  CFLAGS="$CFLAGS -DNO_SOUND"
fi

if test "x$enable_gba_lib" = xyes ; then
  CFLAGS="$CFLAGS -mthumb-interwork -Iincludes -I${with_gbalib} "
  LIBS="-L${with_gbalib} $LIBS"
  LDFLAGS="$LDFLAGS -mthumb-interwork -specs=gba.specs"
  AC_CHECK_LIB(quirkygba, wait_vsync_int,
                 , AC_MSG_ERROR([*** Cannot compile without Quirky's GBA lib.]))
  
  AC_PATH_PROG(GBAFIX, headertool/ht.pl)
  AC_SUBST(GBAFIX)
fi
AM_CONDITIONAL(HAVE_FIXTOOL, test "x$GBAFIX" != "x" )

if test "x$enable_sdl" = xyes ; then
  SDL_VERSION=1.2.2
  AM_PATH_SDL($SDL_VERSION,[
                            ],[
              AC_MSG_ERROR([*** Couldn't find SDL library (version >= $SDL_VERSION).])
              ])
  CFLAGS="$CFLAGS $SDL_CFLAGS"
  CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
          
  if test "x$enable_sdl_net" = xyes ; then
    AC_CHECK_HEADER(SDL_net.h,have_sdl_net_h=yes,)
    AC_CHECK_LIB(SDL_net, SDLNet_Init, have_sdl_net_lib=yes, , )
    if test x$have_sdl_net_lib = xyes -a x$have_sdl_net_h = xyes ; then
      LIBS="$LIBS -lSDL_net"
      CFLAGS="$CFLAGS -DNETWORK"
      enable_sdl_net=yes
    else 
      enable_sdl_net=no
      AC_MSG_WARN([Cannot find SDL_net, network support disabled])
    fi
  fi
fi

CHAOS_LIBS=" -L../img/palette0 -lpalette0 -L../img/palette1 -lpalette1 -L../img/palette2 -lpalette2 -L../img/palette3 -lpalette3 -L../img/palette4 -lpalette4 -L../img/palette5 -lpalette5 -L../img/palette6 -lpalette6 -L../img/palette7 -lpalette7 -L../img/palette8 -lpalette8 -L../img/palette9 -lpalette9 -L../img/palettea -lpalettea -L../img/paletteb -lpaletteb -L../img/palettec -lpalettec"


if test "x$enable_sound" = xyes ; then
 CHAOS_LIBS="$CHAOS_LIBS -L../snd -lsnd "
 SND_SUBDIR="snd"
fi
if test "x$enable_debug" = xyes ; then
 CFLAGS="$CFLAGS -DDEBUG"
 SND_SUBDIR="snd"
fi


AM_CONDITIONAL(GBA_BUILD, test x$enable_gba_lib = xyes)
if test "x$enable_gba_lib" = xyes; then
  CFLAGS="$CFLAGS -D__GBA__"
  AC_SUBST(EXE_NAME)
fi

RODATASUFF="all.rodata.o"
AC_SUBST(MARM_FLAGS)
AC_SUBST(MTHUMB_FLAGS)
AC_SUBST(CHAOS_LIBS)
AC_SUBST(RODATASUFF)
AC_SUBST(OBJCOPYFLAGS)
AC_SUBST(OBJCOPY)
AC_SUBST(WINDRES)
AC_SUBST(SND_SUBDIR)

AC_OUTPUT(Makefile src/Makefile img/Makefile scripts/Makefile src/includes/Makefile
          img/palette0/Makefile \
          img/palette1/Makefile \
          img/palette2/Makefile \
          img/palette3/Makefile \
          img/palette4/Makefile \
          img/palette5/Makefile \
          img/palette6/Makefile \
          img/palette7/Makefile \
          img/palette8/Makefile \
          img/palette9/Makefile \
          img/palettea/Makefile \
          img/paletteb/Makefile \
          img/palettec/Makefile 
          )
if test "x$enable_sound" = xyes ; then
 AC_OUTPUT(snd/Makefile)
fi

