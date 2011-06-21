
#include <QueryApplication.h>


class Test1: public QueryApplication
{

 public:

   Test1()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* sql="select * from tab";
 try {
   UAKGQuery2::RecordSet_var rs=dbc->evaluate_e(sql,"");
 }catch(const UAKGQuery2::QueryFlagInvalid& ex){
   std::cerr << "QueryFlagInvalid:" << ex.why << std::endl;
   return false;
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   std::cerr << "QueryProcessingError:" << ex.why << std::endl;
   std::cerr << "code=" << ex.code << ",dbCode="<< ex.dbCode << std::endl;
   return false;
 }
 return true;
}

int main(int argc, char** argv)
{
 Test1 test1;
 return test1.run(argc,argv);
}
