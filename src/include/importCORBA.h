#ifndef __importCORBA_H
#define __importCORBA_H

/**
 * import CORBA definitions
 */

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif


#ifdef CORBA_H
#include CORBA_H
#else
#error Please, run autoconf!
#endif


#ifdef RSSH_TAO
#ifdef TAO_HAVE_PORTABLE_SERVER_H
#include <tao/PortableServer/PortableServer.h>
#endif
#ifdef TAO_HAVE_IORTABLE_ADAPTER
#include <tao/IORTable/IORTable.h>
#endif
#endif


#endif
