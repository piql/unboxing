
noinst_PROGRAMS = static_unboxer

static_unboxer_CFLAGS = \
     $(AM_CFLAGS) \
     -I${top_srcdir}/inc \
     -I${top_srcdir}/inc/boxing \
     -I${top_srcdir}/thirdparty/glib \
     -I${top_srcdir}/tests/testutils/inc
static_unboxer_LDADD = ../testutils/libtestutils.a ${top_builddir}/src/libunboxing.a -lm 
static_unboxer_SOURCES = \
     main.c

check_PROGRAMS = static_unboxer 

