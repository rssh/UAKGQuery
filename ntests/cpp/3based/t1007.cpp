
#include <QueryApplication.h>


class Test1007: public QueryApplication
{

 public:

   Test1007()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1007::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* sql="begin raise_application_error(-20001,'qqq'); end;";
 try {
   UAKGQuery2::RecordSet_var rs=dbc->evaluate_e(sql,"");
 }catch(const UAKGQuery2::QueryFlagInvalid& ex){
   std::cerr << "QueryFlagInvalid:" << ex.why << std::endl;
   return false;
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   std::cerr << "QueryProcessingError:" << ex.why << std::endl;
   std::cerr << "code=" << ex.code << ",dbCode="<< ex.dbCode << std::endl;
   return true;
 }
 return false;
}

int main(int argc, char** argv)
{
 Test1007 test1007;
 return test1007.run(argc,argv);
}
