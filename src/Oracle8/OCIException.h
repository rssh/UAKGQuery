#ifndef __Oracle8_OCIException_h
#define __Oracle8_OCIException_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Oracle8_importOCI_h
#include <Oracle8/importOCI.h>
#endif


/**
 * Exception, which throwed OCI by OCI related functions
 **/
struct OCIException 
{ 
 OCIError* ociError;
 OCIException(OCIError* _ociError):ociError(_ociError) {}
};


#endif
