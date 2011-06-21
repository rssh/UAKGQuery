#ifndef ___DBFFORORACLE_H___
#include "DBFforOracle.h"
#define ___DBFFORORACLE_H___
#endif

#include <QueryStepsPostConfig.h>
#include <importUAKGQuery.h>

#include "RecordSetFactory.h"

#define  DATE_FORMAT  "'YYYY/MM/DD'"
using namespace std;
using namespace UAKGQuery2;

char*  DBF_OracleClass::Convert2OracleString(char* str,unsigned char len)
{
  char* str_ptr = NULL;
  unsigned int i,q = 0,quotation = 0;
  for( i = 0 ; i<len ; i++ )
    if ( str[i] == '\'' ) quotation++;
  str_ptr = new char[len + quotation + 1];
  if ( ! quotation ) {
    strncpy(str_ptr,str,(int) len);
  } else
    for( i = 0 ; i<len; i++ ) {
      str_ptr[i+q]=str[i];
      if ( str[i] == '\'' )
            str_ptr[i + (++q) ]='\'';
    }
  str_ptr[ len + quotation ] ='\0';
  return str_ptr;
}

RecordSet* DBF_OracleClass::CreateTable(char* str)
{
  char buf[20]="";
  RecordSet_var recordSet = RecordSetFactory::create();
  CORBA::ULong size = 0;
  for (int i = 0; i<Number_Of_Fields; i++) {
     if ( (Set_Of_Fields[i].type=='D') ||
          (Set_Of_Fields[i].type=='d') ) {
     	strcat(str,"TO_DATE(:t");
     	strcat(str,Set_Of_Fields[i].name);
     	strcat(str,","); strcat(str,DATE_FORMAT); strcat(str,")");
     } else {
     	strcat(str," :t");
     	strcat(str,Set_Of_Fields[i].name);
     }
     strcat(str," ,"); buf[0]='\0';
     strcpy(buf,":t"); strcat(buf,Set_Of_Fields[i].name);

     recordSet->addColumn(buf,TypeString);
     size = Set_Of_Fields[i].length+10;

     if ( (Set_Of_Fields[i].type=='D') ||
          (Set_Of_Fields[i].type=='d') )  size+=2;

     recordSet->setFieldSizeAt(i,size);
  }
  str[strlen(str)-1]='\0';
  return ( recordSet._retn() );
}

RecordSet* DBF_OracleClass::CreateTable(int* fields, int len, char* str)
{
  char buf[20]="";
  RecordSet_var recordSet = RecordSetFactory::create();

  CORBA::ULong size = 0;
  for (int i = 0; i<len; i++) {
     if ( (Set_Of_Fields[fields[i]].type=='D') ||
          (Set_Of_Fields[fields[i]].type=='d') ) {
     	strcat(str,"TO_DATE(:t");
     	strcat(str,Set_Of_Fields[fields[i]].name);
     	strcat(str,","); strcat(str,DATE_FORMAT); strcat(str,")");
     } else {
        strcat(str,":t");
        strcat(str,Set_Of_Fields[fields[i]].name);
     }
     strcat(str,","); buf[0]='\0';
     strcpy(buf,":t"); strcat(buf,Set_Of_Fields[fields[i]].name);

     recordSet->addColumn(buf,TypeString);
     size = Set_Of_Fields[fields[i]].length+10;
     
     if ( (Set_Of_Fields[fields[i]].type=='D') ||
          (Set_Of_Fields[fields[i]].type=='d') )  size+=2;

     recordSet->setFieldSizeAt(i,size);
  }
  str[strlen(str)-1]='\0';
  return ( recordSet._retn() );
}

void DBF_OracleClass::FillTable(RecordSet*& rs)
{
   int i,len;
   char buf_[255];
   DropTable(rs);
   for( i = 0 ; i < Number_Of_Fields; i++ ) {
      rs->addRow();
      buf_[0] = '\0'; len = Set_Of_Fields[i].length;
      strncpy(buf_,&RecordDBF[Set_Of_Fields[i].offset],len);
      buf_[len]='\0'; 
      rs->setStringAt(0,i,buf_);
   }
}

void DBF_OracleClass::FillTable(RecordSet*& rs, int* fields,int Len)
{
   int i,len;
   char buf_[255];
   DropTable(rs);
   for( i = 0 ; i < Len; i++ ) {
      rs->addRow();
      buf_[0]='\0'; len = Set_Of_Fields[fields[i]].length;
      if (len!=0)
      strncpy(buf_,&RecordDBF[Set_Of_Fields[fields[i]].offset],len);
      DeleteAroundSpaces(buf_,len);
      if ( len!=0 )
	rs->setStringAt(0,i,buf_);
      else 
	rs->setNullAt(0,i);
   }
}

void DBF_OracleClass::DropTable(RecordSet*& rs)
{
    CORBA::ULong i,nRows = rs->getNRows();
    
    for(i=0; i<nRows; i++) {
	rs->dropRow(0);
    }
}
