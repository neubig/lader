#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([lader], [0.1.6], [neubig@gmail.com])
AC_CONFIG_SRCDIR(src/lib/lader.cc)
AC_CONFIG_HEADERS([src/include/lader/config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/include/Makefile
    src/lib/Makefile
    src/bin/Makefile
    src/test/Makefile
])

AM_INIT_AUTOMAKE([-Wall])
AM_PROG_AR
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC

AC_LANG_CPLUSPLUS
LDFLAGS="$LDFLAGS -lpthread"

# Checks for libraries.

AC_SYS_LARGEFILE

AX_BOOST_BASE([1.38],,[AC_MSG_ERROR([requireBoost])])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_THREAD
AX_BOOST_REGEX

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Check to make sure that we have unordered_map
AC_LANG([C++])
AC_CHECK_HEADER(unordered_map,AC_DEFINE(HAVE_UNORDERED_MAP,1,
			      [Define to 1 if you have unordered_map]),
        [AC_CHECK_HEADER(tr1/unordered_map,AC_DEFINE(HAVE_TR1_UNORDERED_MAP,1,
			      [Define to 1 if you have tr1/unordered_map]),
	[AC_CHECK_HEADER(ext/hash_map,,OPTIONS=$OPTIONS"-DNOHASH ")])])

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday pow sqrt])

AC_OUTPUT
