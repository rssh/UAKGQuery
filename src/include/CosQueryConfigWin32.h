#ifndef __CosQueryConfigWin32_h
#define __CosQueryConfigWin32_h

/**
 * part of Grad-Soft Query Service.
 * (C) Grad-Soft Ltd, Kiev, Ukraine.
 * http://www.gradsoft.kiev.ua
 * $Id: CosQueryConfigWin32.h,v 1.9 2003-03-18 13:05:21 kav Exp $
 **/




// C++ compilers.
                                                
#ifndef STREAMBUF_HAVE_PUBSEEKPOS
#define STREAMBUF_HAVE_PUBSEEKPOS
#endif

// ORB name translation


#if defined(Mico)
# ifndef MICO
#  define MICO
# endif
#endif
#if defined(ORBacus)
# ifndef ORBACUS
#  define ORBACUS
# endif
#endif



// ORB Properties.

#if defined(ORBACUS)


#ifndef CORBA_H
#define CORBA_H <OB/CORBA.h>
#endif

#elif defined(MICO)


#ifndef _WINDOWS
#define _WINDOWS
#endif

#ifndef CORBA_H
#define CORBA_H <CORBA.h>
#endif

#pragma warning( disable : 4267 34 )

#else
#error ORB is not defined

#endif

#ifndef CORBA_HAVE_POA
#define CORBA_HAVE_POA
#endif

#ifndef HAVE_STRSTREAM
#define HAVE_STRSTREAM
#endif

/*                     
#define HAVE_NAMESPACE_ESTD
#define HAVE_NAMESPACES RSTR
#define HAVE_TYPENAME
#define HAVE_MUTABLE
#define HAVE_VECTOR
#define HAVE_DEQUE
#define HAVE_MAP
#define HAVE_SET
#ifndef CORBA_HAVE_POA
#define CORBA_HAVE_POA
#endif
*/

#if defined(ORBACUS)
# define IDL_CLN_H_SUFFIX .h
# define IDL_SRV_H_SUFFIX _skel.h
#elif defined(MICO)
# define IDL_CLN_H_SUFFIX .h
# define IDL_SRV_H_SUFFIX .h
#endif


#endif

