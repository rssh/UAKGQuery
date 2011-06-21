dnl  set of additional configure scripts.
dnl   (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
dnl   $Id: RSSH_CHECK_SERVICEOPTIONS.m4,v 1.3 2008-06-06 12:43:57 paull Exp $
dnl --------------------------------------------------------------------
dnl
dnl
AC_DEFUN(RSSH_CHECK_SERVICEOPTIONS,[
AC_REQUIRE([RSSH_CHECK_PROGOPTIONS])

AC_ARG_WITH(ServiceOptions, ServiceOptions: prefix to ServiceOptions installation (default: /usr/local) ,
            SERVICEOPTIONS_PREFIX=${with_ServiceOptions} , 
            SERVICEOPTIONS_PREFIX=/usr/local )

AC_CHECKING("for ServiceOptions")

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

  if test -f ${SERVICEOPTIONS_PREFIX}/include/GradSoft/ServiceOptions.h
  then
    for i in $CPPFLAGS
    do
      if test "x$i" = "x-I$SERVICEOPTIONS_PREFIX/include"
      then
        serviceoptions_include_found=yes
        break
      fi
    done

    if test "x$serviceoptions_include_found" != xyes
    then
      CPPFLAGS="$CPPFLAGS  -I$SERVICEOPTIONS_PREFIX/include"
      CXXCPPFLAGS="$CXXCPPFLAGS  -I$SERVICEOPTIONS_PREFIX/include"
    fi


    have_serviceoptions_header=yes

    if test $have_serviceoptions_header = yes
    then
    # check for libraries:
     for i in "$LIBS"
     do
     case $i in 
      -L*)
         if test "x$i" = "x-L${SERVICEOPTIONS_PREFIX}/lib"
         then
           serviceoptions_libdir_found=yes
           break
         fi
         ;;
     esac
     done
     if test "x$serviceoptions_libdir_found" = x
     then
       LIBDIR=-L${SERVICEOPTIONS_PREFIX}/lib
     else
       LIBDIR=
     fi
     LIBS="$LIBDIR -lServiceOptions $LIBS" 
    fi
  else
    have_serviceoptions_header=no
    AC_MSG_ERROR(ServiceOptions not found)
  fi

  AC_LANG_RESTORE

AC_MSG_RESULT("Result for ServiceOptions: $have_serviceoptions_header")

])dnl
dnl
