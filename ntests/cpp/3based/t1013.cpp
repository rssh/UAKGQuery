
#include <QueryApplication.h>


class Test1013: public QueryApplication
{

 public:

   Test1013()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1013::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* sql="begin select t1013_seq.nextval into :ID from dual; end;";
 UAKGQuery2::RecordSet_var rs=createRecordSet();
 rs->addColumn(":ID",UAKGQuery2::TypeLong);
 rs->addRow();
 rs->setLongAt(0,0,321);
 UAKGQuery2::Query_var query=dbc->create_query(sql,"");
 std::cout << "prepare" << std::endl;
 query->prepare_query(rs); 
 std::cout << "execute" << std::endl;
 query->execute(rs);

 CORBA::ULong nRetrieved=10;
 CORBA::Boolean more=1;

 while(more) {
   rs = query->fetch(nRetrieved,more);
   printRecordSet(std::cout,rs);
 }

 rs=query->get_all_parameters();
 printRecordSet(std::cout,rs);

 query->destroy();

 return true;
}

int main(int argc, char** argv)
{
 Test1013 test1013;
 return test1013.run(argc,argv);
}

