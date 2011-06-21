#ifndef __QueryStepsPostConfig_h
#define __QueryStepsPostConfig_h

#ifndef _MSC_VER
# include <QueryStepsConfig.h>
#else

#ifndef CORBA_H
#if defined(ORBACUS)
#define CORBA_H <OB/CORBA.h>
#define IDL_CLN_H_SUFFIX .h
#define IDL_SRV_H_SUFFIX _skel.h
#elif defined(MICO)
#define CORBA_H <CORBA.h>
#define IDL_CLN_H_SUFFIX .h
#define IDL_SRV_H_SUFFIX .h
#else
#error "Unknown ORB"
#endif
#endif

#endif

 
#ifndef __CAT2_FF
#define __CAT2_FF(x,y) < x##y >
#endif

#ifndef __CAT2_F
#define __CAT2_F(x,y) __CAT2_FF(x,y)
#endif

#define CORBA_STUB_HEADER(x)  __CAT2_F(x,IDL_CLN_H_SUFFIX)
#define CORBA_SKELETON_HEADER(x)  __CAT2_F(x,IDL_SRV_H_SUFFIX)


#endif
