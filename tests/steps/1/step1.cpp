#include <QueryStepsPostConfig.h>

#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
  CORBA::ORB_var orb;  
  try {


#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    orb = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
    orb = CORBA::ORB_init(argc, argv);
#endif

    initUAKGQuery2(orb); /* register valuetyre */

    CORBA::Object_var obj;
    try {
       obj = orb->resolve_initial_references("UAKGQueryService");
    }catch(const CORBA::ORB::InvalidName&){
       std::cerr << argv[0] << ": can't resolve UAKGQueryService" << std::endl;
       return 1;
    }
        
    if(CORBA::is_nil(obj)) {
      std::cerr << argv[0] << 
                   ": UAKGQueryService is a nil object reference" << std::endl;
      return 1;
    }

    UAKGQuery2::DBConnectionManager_var dbManager = 
                                UAKGQuery2::DBConnectionManager::_narrow(obj);

    if (CORBA::is_nil(dbManager)) {
      std::cerr << argv[0] << 
                  ": can't narrow dbManager to correct type" << std::endl;
      return 1;
    } 
   
    UAKGQuery2::QueryManager_var queryManager;

    try {
      queryManager = 
           dbManager->create_query_manager("skott","tiger",
                                           "","Oracle8","");
    }catch(const UAKGQuery2::QueryManagerNotFound& ex){
      std::cerr << argv[0] <<": can't find query manager." << std::endl;
      return 1;
    }
    
   // here you can perform 


    //time to disconnect.
    queryManager->destroy();
    std::cout << "disconnected." << std::endl;
  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;

    orb->destroy(); 
    return 1;
  }
  orb->destroy(); 
  return 0;
}

