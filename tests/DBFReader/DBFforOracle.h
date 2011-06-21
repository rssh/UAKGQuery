#ifndef __DBFFORORACLE_H__ 
#ifndef ___DBFHEADER_H__
  #include "DBFHeader.h"  
  #define ___DBFHEADER_H__
#endif

#include <QueryStepsPostConfig.h>

#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

class DBF_OracleClass : public DBF_Class {
public:
  DBF_OracleClass() {};
protected:
  char*  Convert2OracleString( char* str,unsigned char len);
  void   DropTable(UAKGQuery2::RecordSet*& rs);
public:
  UAKGQuery2::RecordSet* CreateTable(char* str);
  UAKGQuery2::RecordSet* CreateTable(int* fields,int len,char* str);
  void  FillTable(UAKGQuery2::RecordSet*& rs); 
  void  FillTable(UAKGQuery2::RecordSet*& rs , int* fields,int len);
  void SetDateFormat(char* template_date);
  ~DBF_OracleClass() {};
};

#define __DBFFORORACLE_H__
#endif
