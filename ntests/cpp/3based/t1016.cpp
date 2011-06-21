
#include <QueryApplication.h>


class Test1016: public QueryApplication
{

 public:

   Test1016()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1016::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* sql="begin raise_application_error(-20001,'qqq'); end;";
 try {
   UAKGQuery2::RecordSet_var rs=dbc->evaluate_e(sql,"");
 }catch(const UAKGQuery2::QueryFlagInvalid& ex){
   std::cerr << "QueryFlagInvalid:" << ex.why << std::endl;
   return false;
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   std::cerr << ex.why << std::endl;
   std::cerr << "dbCode=" << ex.dbCode << std::endl;
   if (ex.dbCode > 0) {
      // i. e. it was hipotese, that user-defined error codes 
      // (less then zero in Oracle) must be less then zero in OCI.
      // this hipotese is false
      return true;
   }
 }
 return false;
}

int main(int argc, char** argv)
{
 Test1016 test1016;
 return test1016.run(argc,argv);
}
