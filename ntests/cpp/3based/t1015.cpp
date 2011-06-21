
#include <QueryApplication.h>


class Test1015: public QueryApplication
{

 public:

   Test1015()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1015::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
 const char* pre_sql="delete t1015";
 const char* sql="insert into t1015(id,numeric) values(:A_ID,:A_NUMERIC)";
 const char* post_sql="select * from t1015";

 std::cerr << 1 << std::endl;

 UAKGQuery2::RecordSet_var rs=createRecordSet();
 UAKGQuery2::Numeric_var numeric=createNumeric();
 numeric->scale(19);
 numeric->precision(9);
 numeric->setAsDouble((double)7.77777777777777);
 std::cerr << 2 << std::endl;

 UAKGQuery2::RecordSet_var rs1=dbc->evaluate_e(pre_sql,"");

 std::cerr << 2 << std::endl;

 rs->addColumn("A_ID",UAKGQuery2::TypeLong);
 rs->addColumn("A_NUMERIC",UAKGQuery2::TypeNumeric);
 rs->addRow();
 rs->setLongAt(0,0,7);
 rs->setNumericAt(0,1,numeric);
 std::cerr << 3 << std::endl;

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

 return true;
}

int main(int argc, char** argv)
{
 Test1015 test1015;
 return test1015.run(argc,argv);
}

