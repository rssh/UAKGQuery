
#include <QueryApplication.h>


class TestGh1: public QueryApplication
{

 public:

   TestGh1()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool TestGh1::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* pre_sql="delete gh1";
 const char* sql="insert into gh1(id,v) values(:ID,:V)";
 const char* post_sql="select * from gh1";

 UAKGQuery2::RecordSet_var rs0=dbc->evaluate_e(pre_sql,"");

 UAKGQuery2::RecordSet_var rs=createRecordSet();

 rs->addColumn("ID",UAKGQuery2::TypeLongLong);
 rs->addColumn("V",UAKGQuery2::TypeString);
 rs->addRow();
 rs->setLongLongAt(0,0,9999999999L);
 rs->setStringWithSizeAt(0,1,"qqq");

 std::cerr << "parameters:" << std::endl;
 printRecordSet(std::cerr,rs);
 std::cerr << std::endl;

 UAKGQuery2::Query_var query=dbc->create_query(sql,"");
 query->prepare_query(rs);
 query->execute(rs);
 query->destroy();

 UAKGQuery2::RecordSet_var rs2=dbc->evaluate_e(post_sql,"");

 if (rs2->isNullAt(0,1)) {
    return false;
 }

 std::cout << "after insert:" << std::endl;
 printRecordSet(std::cout,rs2);

 CORBA::LongLong l = rs2->getLongLongAt(0,0);

 if (l != 9999999999L ) {
    return false;
 }

 return true;
}


int main(int argc, char** argv)
{
 TestGh1 testGh1;
 return testGh1.run(argc,argv);
}
