dnl RSSH_CHECK_INTERBASE.m4
dnl autoconf macroses for determination of parameters of InterBase
dnl (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 2001
dnl $Id: RSSH_CHECK_INTERBASE.m4,v 1.5 2003-04-30 09:09:03 qwerty Exp $
dnl
dnl Process this file with autoconf to produce a configure script.
dnl
dnl
AC_DEFUN(RSSH_CHECK_INTERBASE,[

AC_ARG_WITH(interbase, interbase: INTERBASE_HOME (default: $INTERBASE_HOME), \
 INTERBASE_HOME=${with_interbase} , )

dnl
dnl Some customization, local to grad.kiev.ua:
dnl

AC_CHECKING(Interbase)

if test "x$INTERBASE_HOME" = "x"
then
 if test -f /opt/interbase/include/ibase.h
 then
   INTERBASE_HOME=/opt/interbase
 else
   if test -f /usr/interbase/include/ibase.h
   then
     INTERBASE_HOME=/usr/interbase
   fi
 fi
fi

echo INTERBASE_HOME=$INTERBASE_HOME

dnl
dnl finally, add ibase includes to includes:
dnl

if test "x$INTERBASE_HOME" = "x"
then
  interbase=no
  AC_MSG_WARN(INTERBASE_HOME not set.)
else

  interbase=$INTERBASE_HOME
  AC_SUBST(INTERBASE_HOME,$INTERBASE_HOME)

  CPPFLAGS="$CPPFLAGS -I$INTERBASE_HOME/include"

dnl
dnl Check client libs:
dnl

  INTERBASE_LIBDIR=$INTERBASE_HOME/lib
  svLIBS=LIBS
  LIBS="-L$INTERBASE_LIBDIR  $LIBS"

  if test -f $INTERBASE_HOME/lib/libgds.so
  then
    LIBS="$LIBS -lgds"
  else
    dnl  lins can be installed in USR
    if test -f /usr/lib/libgds.so
    then
      LIBS="$LIBS -lgds"
    else
      interbase=no 
    fi
  fi
  if test -f $INTERBASE_HOME/lib/libgdsmt.so
  then
    LIBS="$LIBS -lgdsmt"
  else
    if test -f /usr/lib/libgdsmt.so
    then
      LIBS="$LIBS -lgdsmt"
    fi
  fi
  AC_CHECK_LIB(crypt,crypt,LIBS="$LIBS -lcrypt",,)
fi

if test "x$interbase" = "xno"
then
  INTERBASE_DEF=no
else
  INTERBASE_DEF=yes
fi

AC_SUBST(INTERBASE_DEF)

AC_MSG_RESULT(for Interbase: $interbase)

])dnl

