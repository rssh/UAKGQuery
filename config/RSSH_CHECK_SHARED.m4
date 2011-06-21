dnl  set of additional configure scripts.
dnl   (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
dnl   $Id: RSSH_CHECK_SHARED.m4,v 1.1 2001-06-19 09:46:45 rssh Exp $
dnl --------------------------------------------------------------------
dnl
dnl
AC_DEFUN(RSSH_CHECK_SHARED,[
AC_REQUIRE([RSSH_PIC_FLAG])

AC_MSG_CHECKING("Are we use shared libraries:")

AC_ARG_WITH(shared, shared: use shared libraries (default: yes) ,
            USE_SHARED=${with_shared} , 
            USE_SHARED=yes )

AC_ARG_WITH(static, static: use static libraries (default: no) ,
            USE_STATIC=${with_static} , 
            USE_STATIC=no )

if test "x$USE_SHARED" = x
then
 USE_SHARED=yes
fi

if test "x$USE_STATIC" = x
then
 USE_STATIC=yes
fi

if test "$USE_SHARED" != "no" -a "$USE_SHARED" != "yes"
then
 AC_MSG_ERROR("shared must be set to yes or no")
fi  

if test "$USE_STATIC" != "no" -a "$USE_STATIC" != "yes"
then
 AC_MSG_ERROR("static must be set to yes or no")
fi  

if test "$USE_SHARED" = "no" 
then 
 USE_STATIC=yes
fi

if test "$USE_STATIC" = "yes" -a "$USE_SHARED" = "yes"
then
 AC_MSG_ERROR(
 "you can not compile static and shared version in one configuration \
  : you must run configure twice for doing this")
fi

if test "$USE_STATIC" = "no" -a "$USE_SHARED" = "no"
then
 AC_MSG_ERROR(["static and shared versions are disabed, so nothing to do"])
fi
      
if test "$USE_SHARED" = "yes"
then
 SHARED=yes
 STATIC=no
else
 SHARED=no
 STATIC=yes
fi

AC_SUBST(SHARED)
AC_SUBST(STATIC)

AC_MSG_RESULT($SHARED)

])dnl
dnl
