
noinst_PROGRAMS = unboxer
noinst_SCRIPTS = run_test.sh

unboxer_CFLAGS = \
     $(AM_CFLAGS) \
	-I${top_srcdir}/inc \
	-I${top_srcdir}/thirdparty/glib \
    -I${top_srcdir}/tests/testutils/inc
unboxer_LDADD = ../testutils/libtestutils.a ${top_builddir}/src/libunboxing.a -lm 
unboxer_SOURCES = \
    main.c

check_SCRIPTS = run_test.sh
check_PROGRAMS = unboxer 
TESTS = run_test.sh
EXTRA_DIST = run_test.sh
