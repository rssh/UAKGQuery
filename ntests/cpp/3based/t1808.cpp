
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
 try {
   UAKGQuery2::RecordSet_var rs=createRecordSet();
   rs->addColumn("AbC_XyZ_019", UAKGQuery2::TypeString);

   if ( rs->getColumnIndexByName("ABC_XYZ_019") == rs->getColumnIndexByName("abc_xyz_019") ) {
       std::cerr << "No difference" << std::endl;
   } else {
       std::cerr << "Columns differs." << std::endl;
       return false;
   }

 }catch(const UAKGQuery2::InvalidColumnName& ex){
   std::cerr << ex << std::endl;
   return false;

 }

 return true;
}

int main(int argc, char** argv)
{
 TestXXXX test;
 return test.run(argc,argv);
}

