#ifndef __RecordSetFactory_h
#define __RecordSetFactory_h

#ifndef __QueryStepsPostConfig_h
#include <QueryStepsPostConfig.h>
#endif

#ifndef __importUAKGQuery_h
#include <importUAKGQuery.h>
#endif

class RecordSetFactory
{
 
public:

   static void init(CORBA::ORB_ptr orb);

   static UAKGQuery2::RecordSet*  create()
     { return factory_->create(); }

private:

 static UAKGQuery2::RecordSet_init* factory_;

};

#endif
