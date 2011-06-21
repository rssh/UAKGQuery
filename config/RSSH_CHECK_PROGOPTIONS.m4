dnl  set of additional configure scripts.
dnl   (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
dnl   $Id: RSSH_CHECK_PROGOPTIONS.m4,v 1.3 2003-04-10 18:41:27 rin Exp $
dnl --------------------------------------------------------------------
dnl
dnl
AC_DEFUN(RSSH_CHECK_PROGOPTIONS,[
AC_REQUIRE([RSSH_CHECK_STL])

AC_ARG_WITH(ProgOptions, ProgOptions: prefix to ProgOptions installation (default: /usr/local) ,
            PROGOPTIONS_PREFIX=${with_ProgOptions} , 
            PROGOPTIONS_PREFIX=/usr/local )

AC_CHECKING("for ProgOptions")

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

  if test -f ${PROGOPTIONS_PREFIX}/include/GradSoft/ProgOptions.h
  then
    for i in $CPPFLAGS
    do
      if test "x$i" = "x-I$PROGOPTIONS_PREFIX/include"
      then
        progoptions_include_found=yes
        break
      fi
    done

    if test "x$progoptions_include_found" != xyes
    then
      CPPFLAGS="$CPPFLAGS  -I$PROGOPTIONS_PREFIX/include"
      CXXCPPFLAGS="$CXXCPPFLAGS  -I$PROGOPTIONS_PREFIX/include"
    fi


    AC_CHECK_HEADER(GradSoft/ProgOptions.h,have_progoptions_header=yes,
                                   AC_MSG_ERROR(can not find ProgOptions) )

    if test $have_progoptions_header = yes
    then
    # check for libraries:
     for i in "$LIBS"
     do
     case $i in 
      -L*)
         if test "x$i" = "x-L${PROGOPTIONS_PREFIX}/lib"
         then
           progoptions_libdir_found=yes
           break
         fi
         ;;
     esac
     done
     if test "x$progoptions_libdir_found" = x
     then
       LIBDIR=-L${PROGOPTIONS_PREFIX}/lib
     else
       LIBDIR=
     fi
     LIBS="$LIBDIR -lProgOptions $LIBS" 
    fi
  else
    have_progoptions_header=no
  fi

  AC_LANG_RESTORE

AC_MSG_RESULT("Result for ProgOptions: $have_progoptions_header")

])dnl
dnl
