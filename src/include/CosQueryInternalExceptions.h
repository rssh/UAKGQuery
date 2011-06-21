#ifndef __CosQueryInternalExceptions_h
#define __CosQueryInternalExceptions_h

#ifndef __CosQueryPostConfig_h
#include <CosQueryPostConfig.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

struct NotImplemented
{
 CORBA::String_var what;
 NotImplemented(const char* s)
 {
   what=CORBA::string_dup(s);
 }
};
struct InternalError
{
 CORBA::String_var what;
 InternalError() {}
 InternalError(const char* s)
 {
   what=CORBA::string_dup(s);
 }
};

#endif
