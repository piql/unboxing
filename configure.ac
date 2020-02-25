#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libunboxing], [0.5.1], [boxing@piql.com])
AM_INIT_AUTOMAKE([
	foreign
	1.11
	-Wall
	-Wno-portability
	silent-rules
	tar-pax
	subdir-objects
])
#	no-dist-gzip
#	dist-xz
#	check-news

AC_COPYRIGHT([Copyright (C) 2020 Piql AS])

# Check that we are in the correct dir
AC_CONFIG_SRCDIR([src/unboxer.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB
AC_PROG_LN_S

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])
AC_CHECK_LIB(m, floor)
AC_CHECK_LIB(rt, setitimer)
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(subunit, subunit_test_start)

# Checks for header files.
AC_CHECK_HEADERS([float.h inttypes.h limits.h malloc.h stddef.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor memmove memset pow sqrt strcasecmp strchr strtol strtoull])

# Checks for command line options
AC_ARG_ENABLE([callbacks],
        AS_HELP_STRING([--disable-callbacks], [Disable progress callbacks @<:@default=enabled@:>@]),
        [], enable_callbacks=yes)
AS_IF([test "x$enable_callback" = "xyes"], [
        AC_DEFINE(ENABLE_CALLBACKS, [1], [Progress callbacks.])
])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    tests/Makefile
    tests/unboxingdata/Makefile
    tests/testutils/Makefile
    tests/unboxer/Makefile])
AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE $VERSION
        ================

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}

        callbacks:              ${enable_callbacks}
])
