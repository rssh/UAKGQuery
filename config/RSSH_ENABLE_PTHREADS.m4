dnl@synopsis RSSH_ENABLE_PTHREADS
dnl
dnl modify CFLAGS, CXXFLAGS and LIBS for compiling pthread-based programs.
dnl
dnl@author  (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 2000
dnl@id  $Id: RSSH_ENABLE_PTHREADS.m4,v 1.4 2001-03-10 18:31:47 rssh Exp $
dnl
dnl
AC_DEFUN([RSSH_ENABLE_PTHREADS],[
AC_REQUIRE([RSSH_CHECK_PTHREADS])

if test -z "$rssh_enable_pthreads_done"
then
 CFLAGS="$CFLAGS $CFLAGS_PTHREADS"
 CXXFLAGS="$CXXFLAGS $CXXFLAGS_PTHREADS"
 LIBS="$LIBS $LIBS_PTHREADS"
fi
rssh_enable_pthreads_done=yes
])dnl
dnl
