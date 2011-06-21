dnl  set of additional configure scripts.
dnl   (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
dnl   $Id: RSSH_CHECK_STL.m4,v 1.4 2001-03-10 18:31:46 rssh Exp $
dnl --------------------------------------------------------------------
dnl RSSH_CHECK_STL
dnl  check, if STL is aviable.
dnl  it's perform search of STL, at first in compiler include libs, then 
dnl  in argument of --with-stl if aviable, then in /usr/local/include/STL
dnl
dnl
AC_DEFUN(RSSH_CHECK_STL,[
AC_REQUIRE([RSSH_CHECK_SUNPRO_CC])

AC_ARG_WITH(stl, prefix to STL if STL is not in include dir (default: none) ,
            STL_PREFIX=${with_stl} , STL_PREFIX=/usr/local/include/STL )

AC_CHECKING("for STL")

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

  svCXXCPPFLAGS="$CXXCPPFLAGS"
  svCPPFLAGS="$CPPFLAGS"
  CXXCPPFLAGS="$CXXCPPFLAGS  -I$STL_PREFIX"
  CPPFLAGS="$CPPFLAGS  -I$STL_PREFIX"

  have_stl=no
  AC_CHECK_HEADER(algo.h,have_stl=yes;AC_DEFINE(HAVE_ALGO_H))
  AC_CHECK_HEADER(algorithm,have_stl=yes;AC_DEFINE(HAVE_ALGORITHM))
  AC_CHECK_HEADER(functional,AC_DEFINE(HAVE_FUNCTIONAL))
  AC_CHECK_HEADER(functional.h,AC_DEFINE(HAVE_FUNCTIONAL_H))
  AC_CHECK_HEADER(iterator,AC_DEFINE(HAVE_ITERATOR))
  AC_CHECK_HEADER(iterator.h,AC_DEFINE(HAVE_ITERATOR_H))
  AC_CHECK_HEADER(map,AC_DEFINE(HAVE_MAP))
  AC_CHECK_HEADER(map.h, AC_DEFINE(HAVE_MAP_H))
  AC_CHECK_HEADER(set, AC_DEFINE(HAVE_SET))
  AC_CHECK_HEADER(set.h, AC_DEFINE(HAVE_SET_H))
  AC_CHECK_HEADER(vector,AC_DEFINE(HAVE_VECTOR))
  AC_CHECK_HEADER(vector.h,AC_DEFINE(HAVE_VECTOR_H))
  AC_CHECK_HEADER(deque,AC_DEFINE(HAVE_DEQUE))
  AC_CHECK_HEADER(deque.h,AC_DEFINE(HAVE_DEQUE_H))
  AC_CHECK_HEADER(hash_map,AC_DEFINE(HAVE_HASH_MAP))
  AC_CHECK_HEADER(hash_map.h,AC_DEFINE(HAVE_HASH_MAP_H))
  AC_CHECK_HEADER(hash_set,AC_DEFINE(HAVE_HASH_SET))
  AC_CHECK_HEADER(hash_set.h,AC_DEFINE(HAVE_HASH_SET_H))
  AC_CHECK_HEADER(list,AC_DEFINE(HAVE_LIST))
  AC_CHECK_HEADER(list.h,AC_DEFINE(HAVE_LIST_H))
  AC_CHECK_HEADER(slist,AC_DEFINE(HAVE_SLIST))
  AC_CHECK_HEADER(slist.h,AC_DEFINE(HAVE_SLIST_H))

  if test $have_stl = no
  then
    AC_MSG_ERROR("can not find STL")
  fi

  # local
  if test "$rssh_cv_check_sunpro_cc" = "yes"
  then
    if test -f /usr/local/lib/libstlport_sunpro.a
    then
      LIBS="-lstlport_sunpro $LIBS"
    fi
  fi

AC_LANG_RESTORE

AC_MSG_RESULT("Result for STL: $have_stl")

])dnl
dnl
