
noinst_PROGRAMS = testunboxing

testunboxing_CFLAGS = \
   $(AM_CFLAGS) \
   $(CHECKLIB_CFLAGS) \
   -I${top_srcdir}/inc \
   -I${top_srcdir}/thirdparty/glib \
   -I${top_srcdir}/tests/testutils/inc

testunboxing_LDADD = ${top_builddir}/src/libunboxing.a

if UNIT_TESTS_ENABLED
testunboxing_LDADD += -lcheck -lm -lpthread
endif

testunboxing_SOURCES = \
    matrixtests.c      \
    metadatatests.c    \
    crctests.c         \
    stringtests.c      \
    testsmain.c        \
    image8tests.c      \
    mathtests.c        \
    configtests.h      \
    configtests.c            
#    frametrackerutiltests.c    
#    boxertests.c           


check_PROGRAMS = testunboxing
TESTS = testunboxing

# Clean files generated by the testunboxing application:
DISTCLEANFILES = unboxingdata.xml
