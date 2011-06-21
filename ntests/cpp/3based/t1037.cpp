
#include <QueryApplication.h>


class Test1037: public QueryApplication
{

 public:

   Test1037()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1037::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{

 const char* plsql="begin :ID:=1; end;";

 UAKGQuery2::RecordSet_var rs=createRecordSet();

 rs->addColumn("ID",UAKGQuery2::TypeBoolean);
 rs->addRow();
 //rs->setBooleanAt(0,0,0);
 rs->setNullAt(0,0);

 UAKGQuery2::Query_var query =dbc->create_query(plsql,"");
 query->execute(rs);
 UAKGQuery2::RecordSet_var rs2=query->get_all_parameters();

 std::cout << "t1037:" ;
 printRecordSet(std::cout,rs2);
 std::cout << std::endl;
 

 if (!rs2->getBooleanAt(0,0)) {
   return false;
 }

 return true;
}

int main(int argc, char** argv)
{
 Test1037 test1037;
 return test1037.run(argc,argv);
}

