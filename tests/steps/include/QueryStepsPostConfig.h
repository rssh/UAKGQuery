#ifndef __QueryStepsPostConfig_h
#define __QueryStepsPostConfig_h

#ifndef _MSC_VER
# include <QueryStepsConfig.h>
#else

#if defined(Mico)
# ifndef MICO
#  define MICO
# endif
# undef Mico
#endif
#if defined(ORBacus)
# ifndef ORBACUS
#  define ORBACUS
# endif
#endif


#if defined(ORBACUS)

#ifndef CORBA_H
#define CORBA_H <OB/CORBA.h>
#endif

#ifndef IDL_CLN_H_SUFFIX
#define IDL_CLN_H_SUFFIX .h
#endif

#ifndef IDL_SRV_H_SUFFIX
#define IDL_SRV_H_SUFFIX _skel.h
#endif


#elif defined(MICO)

#ifndef CORBA_H
#define CORBA_H <CORBA.h>
#endif

#ifndef IDL_CLN_H_SUFFIX
#define IDL_CLN_H_SUFFIX .h
#endif

#ifndef IDL_SRV_H_SUFFIX
#define IDL_SRV_H_SUFFIX .h
#endif


#else

#error unknown ORB

#endif

#ifndef HAVE_TYPENAME
#undef HAVE_TYPENAME
#endif

#ifndef HAVE_NAMESPACE_STD
#define HAVE_NAMESPACE_STD
#endif

#endif

#ifndef __CAT1_PP
#define __CAT1_PP(x) x
#endif

#ifndef __CAT2_PP
#define __CAT2_PP(x,y) x##y
#endif
 
#ifndef __CAT2_FF
#define __CAT2_FF(x,y) < x##y >
#endif

#ifndef __CAT2_F
#define __CAT2_F(x,y) __CAT2_FF(x,y)
#endif

#ifndef __CAT3_DOT_FF
#define __CAT3_DOT_FF(x,y,z) < __CAT2_PP(x,y).__CAT1_PP(z) >
#endif

#ifndef __CAT3_DOT_F
#define __CAT3_DOT_F(x,y,z)  __CAT3_DOT_FF(x,y,z)
#endif

#define CORBA_STUB_HEADER(x)  __CAT3_DOT_F(x,IDL_CLN_H_BEFOREDOT_SUFFIX,IDL_CLN_H_EXT)
#define CORBA_SKELETON_HEADER(x)  __CAT3_DOT_F(x,IDL_SRV_H_BEFOREDOT_SUFFIX,IDL_SRV_H_EXT)


#endif
