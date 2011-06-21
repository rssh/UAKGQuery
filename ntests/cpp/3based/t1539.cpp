
#include <QueryApplication.h>


class Test1539: public QueryApplication
{

 public:

   Test1539()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1539::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{

 const char* plsql="begin :ID:=1; end;";

 UAKGQuery2::RecordSet_var rs=createRecordSet();

 rs->addColumn("ID",UAKGQuery2::TypeBoolean);
 rs->addRow();
 rs->setBooleanAt(0,0,0);

 UAKGQuery2::Query_var query =dbc->create_query(plsql,"");
 query->execute_inout(rs.inout());

 std::cout << "t1539:" ;
 printRecordSet(std::cout,rs);
 std::cout << std::endl;

 query->destroy();

 if (!rs->getBooleanAt(0,0)) {
   return false;
 }

 return true;
}

int main(int argc, char** argv)
{
 Test1539 test1539;
 return test1539.run(argc,argv);
}

