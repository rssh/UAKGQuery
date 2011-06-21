#ifndef __Oracle8_importOCI_h
#define __Oracle8_importOCI_h
/*
#ifndef __ORA_HEADERS
extern "C" 
{
 #include <oratypes.h>
 #include <ocidfn.h>
 #include <ociapr.h>
}
#ifndef __ORA_HEADERS
#define __ORA_HEADERS
#endif
#endif
/*/

extern "C" 
{

#ifdef _MSC_VER

// incompability between OCI and MSVC headers:

#ifndef __STDC__
#define __STDC__ 0
#endif

// incompability between OCI and MSVC STL

#define iterator _oracle_it
#endif

#ifndef __oci_h
#include <oci.h>
#ifndef __oci_h
#define __oci_h
#endif
#endif

#ifdef _MSC_VER
#undef iterator
//#error done

#ifdef __STDC__
#undef __STDC__
#endif

#endif

};
//*/

#endif
