#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_INIT([porg],[0.10],[http://porg.sourceforge.net])

RELEASEDATE="17 May 2016"
#RELEASEDATE="`date +'%d %B %Y'`"

AC_CONFIG_SRCDIR([porg/main.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])

AM_INIT_AUTOMAKE([-Wall foreign no-installinfo no-texinfo.tex dist-xz no-dist-gzip])
AM_MAINTAINER_MODE

AC_CONFIG_FILES([ \
	Makefile \
	config-bot.h \
	lib/Makefile \
	lib/porg/Makefile \
	lib/porg-log/Makefile \
	porg/Makefile \
	grop/Makefile \
	scripts/Makefile \
	scripts/paco2porg \
	scripts/porgball \
	doc/Makefile \
	doc/porgrc \
	doc/porgrc.5 \
	doc/porg.8 \
	doc/porgball.8 \
])

AC_USE_SYSTEM_EXTENSIONS


#=======================
# Set configure options
#=======================

if echo "$PACKAGE_VERSION" | grep -q svn; then
	AC_DEFINE([GTK_DISABLE_DEPRECATED], [1], [Disable gtkmm-2.4 API])
else
	AC_DEFINE([NDEBUG], [1], [Disable libc assertions])
##	AC_DEFINE([G_DISABLE_ASSERT], [1], [Disable glib assertions])
fi

AC_DEFINE_UNQUOTED([RELEASEDATE], ["$RELEASEDATE"], [Release date])
AC_SUBST([RELEASEDATE])

EXCLUDE="/tmp"
for dir in /dev /proc /selinux /sys /run; do
	test -e "$dir" && EXCLUDE="$EXCLUDE:$dir"
done
AC_DEFINE_UNQUOTED([EXCLUDE], ["$EXCLUDE"], [List of paths to skip])
AC_SUBST([EXCLUDE])

AC_ARG_WITH([porg-logdir],
	[AS_HELP_STRING([--with-porg-logdir=DIR],[porg log directory [default=/var/log/porg]])],
	[LOGDIR=$withval], [LOGDIR=/var/log/porg]
)
AC_DEFINE_UNQUOTED([LOGDIR], ["$LOGDIR"], [Default porg logs directory])
AC_SUBST([LOGDIR])

AC_ARG_ENABLE([grop],
	AS_HELP_STRING([--disable-grop],[do not install grop (the GUI of porg)]),
	[enable_grop=$enableval], [enable_grop=yes])
AM_CONDITIONAL([ENABLE_GROP], [test "$enable_grop" = yes])


#================
# Check programs
#================

AM_PROG_AR
LT_INIT
AC_PROG_SED
AC_PROG_CC
AC_PROG_CXX


#============
# Check libs
#============

AC_SEARCH_LIBS([dlopen], [dl], [], AC_MSG_ERROR([*** dlopen not found ***]))

if test "$enable_grop" = yes; then
	PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.4.0])
fi


#===============
# Check headers
#===============


#=============
# Check types
#=============

AC_TYPE_SIZE_T

AC_CHECK_TYPE([uint], [unsigned int])
AC_CHECK_TYPE([ulong], [unsigned long])
AC_CHECK_TYPE([ino_t], [unsigned long])


#================
# Check compiler
#================

MY_CFLAGS=-W
MY_CXXFLAGS=-W

if test "$GCC" = yes; then
	MY_CFLAGS="$MY_CFLAGS -ansi -Wshadow -Wmissing-declarations -Wall"
fi

if test "$GXX" = yes; then
	MY_CXXFLAGS="$MY_CXXFLAGS -ansi -pedantic -Wall -fno-operator-names -std=c++11 -Wno-deprecated-declarations"
fi

if echo "$PACKAGE_VERSION" | grep -q svn; then
	MY_CFLAGS="$MY_CFLAGS -Werror"
	MY_CXXFLAGS="$MY_CXXFLAGS -Werror"
fi

AC_SUBST([MY_CXXFLAGS])
AC_SUBST([MY_CFLAGS])


#=============
# Check funcs
#=============

AC_CHECK_FUNCS_ONCE([ \
	openat \
	renameat \
	linkat \
	symlinkat \
	open64 \
	creat64 \
	fopen64 \
	freopen64 \
	openat64
])

AC_CHECK_DECLS([__open, __open64], [], [], [[#include <fcntl.h>]])


#========
# Output
#========

AH_TOP([
#ifndef PORG_CONFIG_H
#define PORG_CONFIG_H 1
])

AH_BOTTOM([
#include "config-bot.h"
#endif
])

# for config-bot.h:
AC_SUBST([ac_includes_default])

AC_OUTPUT

