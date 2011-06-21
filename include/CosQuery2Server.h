#ifndef __CosQuery2Server_h
#define __CosQuery2Server_h

// hhh,  why we need this function ?

#include <GradSoft/ServiceOptions.h>
#include <GradSoft/Logger.h>



UAKGQuery2::DBConnectionManager_ptr createDefaultDBConnectionManager(
                                    CORBA::ORB_ptr orb,
                                    PortableServer::POA_ptr poa,
                                    GradSoft::ServiceOptions& options,
                                    GradSoft::Logger& logger);

#endif
