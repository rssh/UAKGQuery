# RSSH_CHECK_GNU_LD([ACTION-IF-YES], [ACTION-IF-NOT])
# ------------------------------------------------------
# check : are we using GNU ld.
#  Corresponding cache value: rssh_cv_check_gnu_ld is set to yes or no
#
#@author  Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 2000
#@version $Id: RSSH_CHECK_GNU_LD.m4,v 1.1 2001-03-12 14:35:16 jos Exp $
#
#  RSSH_CHECK_GNU_LD([ACTION-IF-YES],[ACTION-IF-NOT])
#
AC_DEFUN([RSSH_CHECK_GNU_LD],
[AC_CACHE_CHECK([whether using GNU ld],
                [rssh_cv_check_gnu_ld],

[
rssh_cv_check_gnu_ld=no
ld -v | grep GNU > /dev/null && rssh_cv_check_gnu_ld=yes
])
if test ${rssh_cv_check_gnu_ld} = yes
then
  $2
  :
else 
  $3
  :
fi
])# RSSH_CHECK_GNU_LD
