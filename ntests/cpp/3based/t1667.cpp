
#include <QueryApplication.h>


class Test1667: public QueryApplication
{
 public:

   Test1667()
     :QueryApplication()
   {}


   bool  doQueryStuff(UAKGQuery2::QueryManager_ptr dbc);

};


bool Test1667::doQueryStuff(UAKGQuery2::QueryManager_ptr dbc)
{
   bool return_value = true;

   const char* insert_query = "insert into t1667 values(:A)";

   UAKGQuery2::RecordSet_var records = createRecordSet();;

   records -> addColumn("A", UAKGQuery2::TypeString);

   records -> addRow();
   
//QueryProcessingError ORA-01480
   records -> setFieldSizeAt(0,3);
   records -> setStringAt(0,0,"qwer");

   UAKGQuery2::RecordSet_var dump; 

   try {

     dump = dbc -> evaluate(insert_query, "",records);

   }catch(const UAKGQuery2::QueryProcessingError& ex){
     std::cerr << "QueryProcessingError:" << ex.why << std::endl;
   }

  //QueryProcessingErrorORA-01461: 
   records -> setFieldSizeAt(0,2);
   records -> setStringAt(0,0,"qwe");

   try {
 
     dump = dbc -> evaluate(insert_query, "",records);

   }catch(const UAKGQuery2::QueryProcessingError& ex){
     std::cerr << "QueryProcessingError:" << ex.why << std::endl;
   }
 
  //QueryProcessingError ORA-01401: 
   records -> setFieldSizeAt(0,4);
   records -> setStringAt(0,0,"qwer");

   try {

     dump = dbc -> evaluate(insert_query, "",records);

   }catch(const UAKGQuery2::QueryProcessingError& ex){
     std::cerr << "QueryProcessingError:" << ex.why << std::endl;
     return_value = false;
   }

   records -> setFieldSizeAt(0,30);
   records -> setStringAt(0,0,"asd");
   
   try {

     dump = dbc -> evaluate(insert_query, "",records);

   }catch(const UAKGQuery2::QueryProcessingError& ex){
     std::cerr << "QueryProcessingError:" << ex.why << std::endl;
     return_value = false;
   }

 return return_value;
}

int main(int argc, char** argv)
{
 Test1667 test1667;
 return test1667.run(argc,argv);
}

