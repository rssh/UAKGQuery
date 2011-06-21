#ifndef __CosQueryPostConfig_h
#define __CosQueryPostConfig_h


/*
 * CosQueryPostConfig.h
 * part of implementation of CORBA query service.
 * (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998 - 2003
 * $Id: CosQueryPostConfig.h,v 1.22 2008-02-23 23:26:38 rssh Exp $
 */


#ifdef HAVE_CONFIG_H
# include <include/CosQueryConfig.h>
#else
# ifndef _MSC_VER
#  error : on UNIX, please, run autoconf; on WIN32 call us
# else
#  include <include/CosQueryConfigWin32.h>  
# endif
#endif


#ifdef HAVE_TYPENAME
#define TYPENAME typename
#else
#define TYPENAME
#endif
 
#ifndef __CAT1_PP
#define __CAT1_PP(x)  x 
#endif

#ifndef __CAT2_PP
#define __CAT2_PP(x,y)  x##y 
#endif

#ifndef __CAT2_FF
#define __CAT2_FF(x,y) < x##y >
#endif

#ifndef __CAT3_DOT_FF
#define __CAT3_DOT_FF(x,y,z) < __CAT2_PP(x,y).__CAT1_PP(z) >
#endif

#ifndef __CAT2_F
#define __CAT2_F(x,y) __CAT2_FF(x,y)
#endif

#ifndef __CAT3_DOT_F
#define __CAT3_DOT_F(x,y,z) __CAT3_DOT_FF(x,y,z)
#endif

#define CORBA_STUB_HEADER(x)  __CAT3_DOT_F(x,IDL_CLN_H_BEFOREDOT_SUFFIX,IDL_CLN_H_EXT)
#define CORBA_SKELETON_HEADER(x)  __CAT3_DOT_F(x,IDL_SRV_H_BEFOREDOT_SUFFIX,IDL_SRV_H_EXT)


#ifndef WORDS_BIGENDIAN
#define RC_SWAP
#endif


#ifdef ORBACUS
#ifndef CORBA_HAVE_POA
#define HAVE_OB_UTIL_H
#define HAVE_OBPRINTEXCEPTION
#endif
#ifdef CORBA_HAVE_OTS
#define COSTRANSACTIONS_H <OB/CosTransactions.h>
#endif
#endif

#ifdef __linux__
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

//#define __DEBUG__ 1

#ifdef __DEBUG__
#define PRINT_POSITION
#endif

#endif
