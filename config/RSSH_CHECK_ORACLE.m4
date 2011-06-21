dnl RSSH_CHECK_ORACLE.m4
dnl auticonf macroses for determination of parameters of ORACLE installation
dnl (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
dnl $Id: RSSH_CHECK_ORACLE.m4,v 1.12 2008-02-14 16:24:10 rssh Exp $
dnl
dnl Process this file with autoconf to produce a configure script.
dnl
dnl
AC_DEFUN(RSSH_CHECK_ORACLE,[

AC_ARG_WITH(oracle, oracle: ORACLE_HOME value (default: $ORACLE_HOME), \
 ORACLE_HOME=${with_oracle} , )

dnl
dnl Some customization, local to grad.kiev.ua:
dnl

AC_CHECKING(Oracle)

if test "x$ORACLE_HOME" = "x"
then
 if test -f /u01/app/oracle/product/8.0.4/rdbms/demo/oratypes.h
 then
   ORACLE_HOME=/u01/app/oracle/product/8.0.4
 else
   if test -f /usr/oracle7/rdbms/demo/oratypes.h
   then
     ORACLE_HOME=/usr/oracle7
   fi
 fi
fi

dnl echo ORACLE_HOME=$ORACLE_HOME

dnl
dnl finally, add OCI includes to includes:
dnl

if test "x$ORACLE_HOME" = "x"
then
  oracle=no
  AC_MSG_WARN(ORACLE_HOME not set.)
else

  oracle=$ORACLE_HOME
  AC_SUBST(ORACLE_HOME,$ORACLE_HOME)

  if test -d "$ORACLE_HOME/sdk"
  then
    rssh_oracle_instance_client=yes
  fi

  if test "$rssh_oracle_instance_client" = "yes"
  then
    CPPFLAGS="$CPPFLAGS -I$ORACLE_HOME/sdk/include"
  else
    CPPFLAGS="$CPPFLAGS -I$ORACLE_HOME/rdbms/demo"
    CPPFLAGS="$CPPFLAGS -I$ORACLE_HOME/rdbms/public"
    CPPFLAGS="$CPPFLAGS -I$ORACLE_HOME/plsql/public"
    CPPFLAGS="$CPPFLAGS -I$ORACLE_HOME/network/public"
  fi

dnl
dnl Check client libs:
dnl

  if test "$rssh_oracle_instance_client" = "yes"
  then
    if test -d $ORACLE_HOME/lib
    then
      ORACLE_LIBDIR=$ORACLE_HOME/lib
    else
      ORACLE_LIBDIR=$ORACLE_HOME
    fi
  else
    ORACLE_LIBDIR=$ORACLE_HOME/lib
  fi 
  LIBS="-L$ORACLE_LIBDIR  $LIBS"
  if test -d $ORACLE_HOME/server/lib
  then
    LIBS="-L$ORACLE_HOME/server/lib $LIBS"
  fi

  if test -d /opt/SUNWspro/lib
  then
    LIBS="-L/opt/SUNWspro/lib $LIBS"
  fi

  if test -d /opt/SUNWspro/SC4.2/lib
  then
    LIBS="-L/opt/SUNWspro/SC4.2/lib $LIBS"
  fi

  if test -f $ORACLE_LIBDIR/libclntsh.so
  then
    LIBS="$LIBS -lclntsh"
    if test -f $ORACLE_LIBDIR/liboi.so
    then
      LIBS="$LIBS -loi"
    fi
    if test -f $ORACLE_LIBDIR/libnnz10.so
    then
      LIBS="$LIBS -lnnz10"
    fi
  else
    found=no
    # search as libcntlsh.so.xx.xx
    for i in $ORACLE_LIBDIR/libclntsh.so.*
    do
      found=yes
      libfile=$i
    done
    if test found = no
    then
      AC_MSG_WARN("Can not find ORACLE client library");
      oracle=no 
    else
      AC_MSG_WARN("detected library $libfile but with not .so name");
      AC_MSG_WARN("please, do symbolic link from one to $ORACLE_LIBDIR/libclntsh.so  ");
      AC_MSG_WARN("i.e. ln -s $libfile $ORACLE_LIBDIR/libclntsh.so  ");
      AC_MSG_WARN("otherwise library will not be linked  ");
    fi
  fi
fi

if test "x$oracle" = "xno"
then
  ORACLE_DEF=no
else 
  ORACLE_DEF=yes
fi

AC_SUBST(ORACLE_DEF)

AC_MSG_RESULT(for Oracle: $oracle)

])dnl

