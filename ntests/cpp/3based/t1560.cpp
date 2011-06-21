
#include <QueryApplication.h>


class Test1560: public QueryApplication
{
 public:

   Test1560()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1560::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{

 const char* insertSql ="insert into BLOB_TEST(data) values(:x)";
 const char* selectSql ="select data from BLOB_TEST";
 const char* deleteSql ="delete BLOB_TEST";


 UAKGQuery2::OctSeq_var octSeq = new UAKGQuery2::OctSeq;
 const CORBA::ULong myBlobLen=1000;
 octSeq->length(myBlobLen);
 for(CORBA::ULong i=0; i<myBlobLen; ++i) {
   octSeq[i]=(i%255);
 }

 UAKGQuery2::RecordSet_var rs=createRecordSet();
 rs->addColumn("x",UAKGQuery2::TypeBlob);
 rs->addRow();
 std::cout << "t1560: create blob" << std::endl;

 std::cout << "t1560: insert blob" << std::endl;
 UAKGQuery2::Query_var query =dbc->create_query(insertSql,"");
 UAKGQuery2::Blob_var bl=query->create_blob();
 bl->put_chunk(octSeq.in());
 rs->setBlobAt(0,0,bl);
 query->execute(rs);

 std::cout << "destroy  insert query" << std::endl;
 bl->destroy();
 query->destroy();

 std::cout << "t1560: select query" << std::endl;
 query =dbc->create_query(selectSql,"");
 query->execute_e();
 CORBA::Boolean more;
 rs=query->fetch(1,more);

 std::cout << "t1560: received" << std::endl;
 printRecordSet(std::cout,rs);

 std::cout << "t1560: destroy  select query" << std::endl;
 query->destroy(); 

 dbc->evaluate_e(deleteSql,"");

 return true;
}

int main(int argc, char** argv)
{
 Test1560 test1560;
 return test1560.run(argc,argv);
}

