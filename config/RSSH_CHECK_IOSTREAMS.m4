dnl  set of additional configure scripts.
dnl   (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
dnl   $Id: RSSH_CHECK_IOSTREAMS.m4,v 1.4 2001-03-10 18:31:45 rssh Exp $
dnl --------------------------------------------------------------------
dnl RSSH_CHECK_IOSTREAMS
dnl  check, C++ systems iostream interfaces.
dnl define 
dnl@cpp   HAVE_IOSTREAM    iostream is aviable
dnl@cpp   HAVE_IOSTREAM_H  iostream.h is aviable
dnl@cpp   HAVE_FSTREAM     fstream is aviable
dnl@cpp   HAVE_FSTREAM_H   fstream.h is aviable
dnl@cpp   HAVE_STRSTREAM   strstream header file 
dnl@cpp   HAVE_STRSTREAM_H strstream.h header file 
dnl@cpp   HAVE_SSTREAM     stream header file 
dnl@cpp   IFSTREAM_HAVE_ISOPEN  when ifstream.is_open() exisits
dnl@cpp   IOS_HAVE_NOCREATE when ios::nocreate exisits
dnl@cpp   STREAMBUF_HAVE_PUBSEEKPOS when streambuf::pubseekpos is defined
dnl
AC_DEFUN(RSSH_CHECK_IOSTREAMS,[
AC_REQUIRE([AC_CXX_NAMESPACES])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER(iostream,AC_DEFINE(HAVE_IOSTREAM))
AC_CHECK_HEADER(iostream.h,AC_DEFINE(HAVE_IOSTREAM_H))
AC_CHECK_HEADER(fstream,AC_DEFINE(HAVE_FSTREAM))
AC_CHECK_HEADER(fstream.h,AC_DEFINE(HAVE_FSTREAM_H))
AC_CHECK_HEADER(strstream,AC_DEFINE(HAVE_STRSTREAM))
AC_CHECK_HEADER(strstream.h,AC_DEFINE(HAVE_STRSTREAM_H))
AC_CHECK_HEADER(sstream,AC_DEFINE(HAVE_SSTREAM))
AC_CACHE_CHECK([whether ifstream.is_open() exists],
                [rssh_cv_ifstreamhaveisopen],
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(
[
#ifdef HAVE_FSTREAM
#include <fstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif
#else
#include <fstream.h>
#endif
],
[
ifstream ifs;
if (ifs.is_open()) return 1;
],
               rssh_cv_ifstreamhaveisopen=yes,
               rssh_cv_ifstreamhaveisopen=no)
AC_LANG_RESTORE])
if test "$rssh_cv_ifstreamhaveisopen" = yes
then
  AC_DEFINE(IFSTREAM_HAVE_ISOPEN)
fi
AC_CACHE_CHECK([whether ios::nocreate exists],
               [rssh_cv_ioshavenocreate],
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(
[
#ifdef HAVE_FSTREAM
#include <fstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif
#else
#include <fstream.h>
#endif
],
[
ifstream ifs;
ifs.open("qqq",ios::nocreate);
],
               rssh_cv_ioshavenocreate=yes,
               rssh_cv_ioshavenocreate=no)
AC_LANG_RESTORE])
if test "$rssh_cv_ioshavenocreate" = yes
then
  AC_DEFINE(IOS_HAVE_NOCREATE)
fi
AC_CACHE_CHECK([whether streambuf::pubseekpos exists],
               [rssh_cv_streambufhavepubseekpos],
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(
[
#ifdef HAVE_STRSTREAM
#include <strstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif
#else
#include <strstream.h>
#endif
],
[
 ostrstream strb;
 strb.rdbuf()->pubseekpos(0,ios::out);
],
               rssh_cv_streambufhavepubseekpos=yes,
               rssh_cv_streambufhavepubseekpos=no)
AC_LANG_RESTORE])
if test "$rssh_cv_streambufhavepubseekpos" = yes
then
  AC_DEFINE(STREAMBUF_HAVE_PUBSEEKPOS)
fi
AC_LANG_RESTORE
])dnl
dnl
