dnl  set of additional configure scripts.
dnl   (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 2000
dnl   $Id: RSSH_CHECK_JTC.m4,v 1.8 2001-06-19 09:46:45 rssh Exp $
dnl --------------------------------------------------------------------
dnl
dnl
AC_DEFUN(RSSH_CHECK_JTC,[

AC_ARG_WITH(jtc, [jtc: prefix to JTC installation (default: /usr/local)] ,
            JTC_PREFIX=${with_jtc} , JTC_PREFIX=/usr/local )

AC_CHECKING("for JTC")

  svLIBS=$LIBS
  svCXXFLAGS="$CXXFLAGS"
  svCPPFLAGS="$CPPFLAGS"
  svCXXCPPFLAGS="$CXXCPPFLAGS"
  svRSSH_ROLLBACK="$rssh_rollback"
  rssh_rollback="true"

  RSSH_ENABLE_PTHREADS

  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  CXXCPPFLAGS="$CXXCPPFLAGS  -I$JTC_PREFIX/include"
  CPPFLAGS="$CPPFLAGS  -I$JTC_PREFIX/include"
  CXXFLAGS="$CXXFLAGS $CXXFLAGS_THREADS"

  AC_CHECK_HEADER(JTC/JTC.h,have_jtc_header=yes,have_jtc_header=no)

  if test $have_jtc_header = yes
  then
    LIBS="-L${JTC_PREFIX}/lib -lJTC $LIBS" 
    
    AC_CACHE_CHECK("whether we can link with JTC",
                 [rssh_cv_check_jtc],
[
      AC_LANG_SAVE
      AC_LANG_CPLUSPLUS
      AC_TRY_LINK(
#include <JTC/JTC.h>
,
int x=0;
JTCInitialize bootJTC(x,NULL),
                 [rssh_cv_check_jtc=yes],
                 [rssh_cv_check_jtc=no]
     )
     AC_LANG_RESTORE
])
  else
    rssh_cv_check_jtc=no
  fi

  AC_LANG_RESTORE

AC_MSG_RESULT("Result for JTC: $rssh_cv_check_jtc")

if test $rssh_cv_check_jtc = yes
then
  AC_DEFINE(HAVE_JTC,1)
else
  LIBS=$svLIBS
  CXXFLAGS="$svCXXFLAGS"
  CXXCPPFLAGS="$svCXXCPPFLAGS"
  CPPFLAGS="$svCPPFLAGS"
  eval "$rssh_rollback" 
  rssh_rollback="$svRSSH_ROLLBACK"
fi

])dnl
dnl
