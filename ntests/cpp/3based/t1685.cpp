
#include <QueryApplication.h>


class Test1685: public QueryApplication
{

 public:

   Test1685()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1685::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* sql="begin t1685_proc(:ID,:NAME); end;";

 UAKGQuery2::RecordSet_var rs = createRecordSet();
 rs->addColumn("NAME",UAKGQuery2::TypeString);
 rs->addColumn("ID",UAKGQuery2::TypeLong);
 rs->addRow();
 rs->setStringWithSizeAt(0,0,"MY VERY VERY VERY LONG NAME");

 UAKGQuery2::Query_var query = dbc->create_query(sql,"");

 query->execute(rs.in());

 rs=query->get_all_parameters();

 CORBA::Long id = rs->getLongAt(0,rs->getColumnIndexByName("ID"));
 std::cout << "received id=" << id << std::endl;

 query->destroy();

 return true;
}

int main(int argc, char** argv)
{
 Test1685 test1685;
 return test1685.run(argc,argv);
}
