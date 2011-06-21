#ifndef __importCORBA_H
#define __importCORBA_H

/**
 * import CORBA definitions
 */

#ifndef __QueryStepsPostConfig_h
#include <QueryStepsPostConfig.h>
#endif

#ifdef CORBA_H
#include CORBA_H
#else
#error Please, run autoconf!
#endif

#endif
