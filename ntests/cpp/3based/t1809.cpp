
#include <QueryApplication.h>


class TestXXXX: public QueryApplication
{

 public:

   TestXXXX()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool TestXXXX::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 // Checks for not closing of cursors resulting in
 // ORA-01000: Maximum open cursors limit exceded.
 const char* sql = "begin raise_application_error(-20001,'more cursors?');end;";
 const long twiceMaxOpenCursors = 1000;
 try {
   UAKGQuery2::Query_var qry;
   for (long i=0; i<twiceMaxOpenCursors; ++i) {
      try {
          std::cerr << ".";
          qry =  dbc -> create_query(sql, "SQL92");
          qry -> execute_e();
  
      } catch (const UAKGQuery2::QueryProcessingError& ex) {
          if (ex.code != 20001) {
            std::cerr << std::endl;
            return false;
          }
      }
      qry -> destroy();
   }

 } catch (...) {
   std::cerr << std::endl;
   std::cerr << "Error: unexpected exception." << std::endl;
   return false;
 }

 std::cerr << std::endl;
 std::cerr << "Passed." << std::endl;
 return true;
}



int main(int argc, char** argv)
{
 TestXXXX test;
 return test.run(argc,argv);
}

