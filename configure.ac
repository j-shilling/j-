AC_PREREQ([2.69])
AC_INIT([kava], [0.1], [shilling.jake@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_SRCDIR([src/kavac/main.c])
AC_CONFIG_HEADERS([src/include/config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER([pthread.h],
		[AC_DEFINE([HAVE_PTHREAD_H], [1], [Define 1 if you have the <pthread.h> header file.])],
		[AC_DEFINE([HAVE_PTHREAD_H], [0], [Define 1 if you have the <pthread.h> header file.])])
AC_CHECK_HEADER([unistd.h],
		[AC_DEFINE([HAVE_UNISTD_H], [1], [Define 1 if you have the <unistd.h> header file.])],
		[AC_DEFINE([HAVE_UNISTD_H], [0], [Define 1 if you have the <unistd.h> header file.])])
AC_CHECK_HEADER([sys/stat.h],
		[AC_DEFINE([HAVE_SYS_STAT_H], [1], [Define 1 if you have the <sys/stat.h> header file.])],
		[AC_DEFINE([HAVE_SYS_STAT_H], [0], [Define 1 if you have the <sys/stat.h> header file.])])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 src/include/Makefile
                 src/kavac/Makefile
		 src/kavac/lexical/Makefile
                 src/kavavm/Makefile
                 src/libkava/Makefile])
AC_OUTPUT
