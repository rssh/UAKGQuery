#ifndef ___DBFHEADER_H___
#include "DBFHeader.h"
#define ___DBFHEADER_H___
#endif

#define ORCALE_DATE_FORMAT 0
#define NULL_TOKEN  0

#include <iostream>
#include <strstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

void  DBF_Class::open(char* filename)
{
   char* Ptr_title = NULL;
   DBF_file_name = new char[strlen(filename)+1];
   if ( DBF_file_name == NULL ) {
      cerr << "Error:: Not enough memeory" << endl;
      Exit();
   }
   strcpy(DBF_file_name,filename);
   DBF_file.open( filename, ios::in);
   if ( DBF_file == NULL ) {
      cerr << "Error::Open file " << "errno="<< errno << endl;
      Exit();
   }
   Ptr_title = new char[SIZE_OF_DBF_TITLE+1];
   if ( Ptr_title == NULL ) {
      cerr << "Error:: Not enough memeory" << endl;
      Exit();
   }
   DBF_file.read( Ptr_title, SIZE_OF_DBF_TITLE);
   if ( DBF_file.gcount() != SIZE_OF_DBF_TITLE )  {
      cerr << "Error:: Size of dbf-file" << endl;
      delete [] Ptr_title ;
      Exit();
   }
   Convert_Pointer_2_DBFStruct( Ptr_title );
   delete [] Ptr_title ;
}

void DBF_Class::Convert_Pointer_2_DBFStruct(char* Ptr_to_title)
{
   DBF_title_header.language_identifier = Ptr_to_title[0];
   memcpy(&DBF_title_header.last_data_modification,&Ptr_to_title[1],3);
#if WORDS_BIGENDIAN
   ConverIntel2SunNumber(&Ptr_to_title[4],sizeof(int));
#endif   
   memcpy(&DBF_title_header.number_of_records,&Ptr_to_title[4],sizeof(int));
#if WORDS_BIGENDIAN 
   ConverIntel2SunNumber(&Ptr_to_title[8],2);
#endif  
   memcpy(&DBF_title_header.position_of_first_record,&Ptr_to_title[8],2 );
#if WORDS_BIGENDIAN
   ConverIntel2SunNumber(&Ptr_to_title[10],sizeof(short));
#endif  
   memcpy(&DBF_title_header.record_length,&Ptr_to_title[10],sizeof(short));
   DBF_title_header.index = Ptr_to_title[28];
}

void DBF_Class::Print_DBF_title()
{
   cout << "===== DBF title =====" << endl;
   cout <<"Language identifier:" ;
   cout << (int) DBF_title_header.language_identifier << endl;
   cout << "Date of last modifiaction (yy/mm/dd): " ;
   cout << (int) DBF_title_header.last_data_modification[0] << "/";
   cout << (int) DBF_title_header.last_data_modification[1] << "/";
   cout << (int) DBF_title_header.last_data_modification[2] << endl;
   cout << "Number of records :";
   cout << DBF_title_header.number_of_records << endl;
   cout << "Position of first record :" <<
   DBF_title_header.position_of_first_record << endl;
   cout << "Record length : " << DBF_title_header.record_length << endl;
   cout << "Index : " << (int) DBF_title_header.index << endl;
   cout << "=====================" << endl;
}

void DBF_Class::Read_DBF_fields()
{
   char* Ptr_field_title = NULL;
   if ( DBF_file_name == NULL ) {
     cerr << "Error::DBF file is not assigned" << endl;
     Exit();
   }
   Ptr_field_title = new char[SIZE_OF_FIELD_TITLE+1];
   streampos DBF_position = DBF_file.tellg();
   while ( ! DBF_file.eof() ) {
     if ( DBF_file.peek() == END_OF_FIELDS ) break;
     DBF_file.read( Ptr_field_title, SIZE_OF_FIELD_TITLE);
     Number_Of_Fields++;
     if ( DBF_file.fail() )  {
       cerr << "Error::Input-output operation" << endl;
       delete [] Ptr_field_title;
       Exit();
     }
   }
   Set_Of_Fields = new DBF_field_type[Number_Of_Fields+1];
   if ( Set_Of_Fields == NULL ) {
      cerr << "Error:: Not enough memeory" << endl;
      delete [] Ptr_field_title;
      Exit();
   }
   DBF_file.seekg(DBF_position,ios::beg);
   for(int i=0; i<Number_Of_Fields;i++) {
     DBF_file.read( Ptr_field_title, SIZE_OF_FIELD_TITLE);
     Conver_Pointer_2_TITLEStruct(Ptr_field_title,&Set_Of_Fields[i]);
   }
   delete [] Ptr_field_title;
}

void DBF_Class::Conver_Pointer_2_TITLEStruct(char* Ptr_field,
                                             DBF_field_type* field)
{
#if WORDS_BIGENDIAN
   ConverIntel2SunNumber(&Ptr_field[12],sizeof(int));
#endif   
  memcpy(field,Ptr_field,sizeof(DBF_field_type));
  field->offset = OFFSET;
  OFFSET += (int) field->length;
}

int DBF_Class::GetFirst()
{
   Read_DBF_fields();
   RecordDBF = (char*) new char[DBF_title_header.record_length+2];
   if ( RecordDBF == NULL ) {
      cerr << "Error:: Not enough memeory" << endl;
      Exit();
   }
   if ( DBF_file_name == NULL ) {
     cerr << "Error::DBF file is not assigned" << endl;
     Exit();
   }
   if ( DBF_file.eof() ) {
      cerr << "Error:: dbf file corruted" << endl;
      Exit();
   }
   DBF_file.seekg(DBF_title_header.position_of_first_record,ios::beg);
   if ( DBF_file.peek() == END_OF_DBF_FILE ) return 0;
   DBF_file.read(RecordDBF,DBF_title_header.record_length);
   return (0xFFFF);
}


void DBF_Class::Print_DBF_fields()
{
   for(int i=0;i<Number_Of_Fields; i++) {
      cout << endl;
      cout << "----- DBF FIELD ----- " << endl;
      cout << "Name:" << Set_Of_Fields[i].name << endl;
      cout << "Type:" << Set_Of_Fields[i].type << endl;
      cout << "Offset:" << (int) Set_Of_Fields[i].offset << endl;
      cout << "Length:" << (int) Set_Of_Fields[i].length << endl;
   }
}


int  DBF_Class::GetNext()
{
   if ( DBF_file.eof() ) {
      cerr << "Error:: dbf file corruted" << endl;
      Exit();
   }
   if ( DBF_file.peek() == END_OF_DBF_FILE ) return 0;
   DBF_file.read(RecordDBF,DBF_title_header.record_length);
   return (0xFFFF);
}


void DBF_Class::PrintNumber( char* ptr_str, unsigned char length )
{
  char string[256]="";
  strncpy(string,ptr_str,length);
  string[length]='\0';
  cout << string;
}

void DBF_Class::PrintNumber(char* ptr_str,unsigned char length, char* dst)
{
  char string[256]="";
  if ( ! DeleteAroundSpaces(ptr_str,length) ) {
#if NULL_TOKEN
     strcat(dst,"null");
#endif
     strcat(dst,",");
     return;
  }
  strncpy(string,ptr_str,length);
  string[strlen(ptr_str)]='\0';
  strcat(dst,string); strcat(dst,",");
}

void DBF_Class::PrintCharacter( char* ptr_str, unsigned char length )
{
  char string[256]="";
  strncpy(string,ptr_str,length);
  string[length]='\0';
  cout << string;
}

void DBF_Class::PrintCharacter(char* ptr_str,unsigned char length,char* dst)
{
  char string[256]="";
  if ( ! DeleteAroundSpaces(ptr_str,length) ) {
#if NULL_TOKEN
     strcat(dst,"null"); 
#endif
     strcat(dst,",");
     return;
  }
  strncpy(string,ptr_str,length);
  string[strlen(ptr_str)]='\0';
  strcat(dst,"'"); strcat(dst,string); strcat(dst,"',"); 
}

void DBF_Class::PrintLogical( char ch )
{
  cout << ch ;
}

void DBF_Class::PrintLogical( char ch, char* dst )
{
  char str[3] = " ,";
  str[0]=ch; strcat(dst,str);
}

void DBF_Class::PrintDate(char* date)
{
  cout << date[0] << date[1] << date[2] << date[3] ;
  cout << "-" << date[4] << date[5] ;
  cout << "-" << date[6] << date[7] ;
}

void DBF_Class::PrintDate(char* date, char* dst)
{
  char  buf[60]; 
  if ( ! DeleteAroundSpaces(date,8) ) {
#if NULL_TOKEN
     strcat(dst,"null"); 
#endif
     strcat(dst,",");
     return;
  }
#if ORACLE_DATE_FORMAT
  ostrstream(buf,sizeof(buf)) << "TO_DATE('" << date[0] << date[1] << date[2] << date[3] << "-" << date[4] << date[5] << "-" << date[6] << date[7] << "','YYYY-MM-DD')," << '\0' ;
#else 
  ostrstream(buf,sizeof(buf)) << date[0] << date[1] << date[2] << date[3] << "-" << date[4] << date[5] << "-" << date[6] << date[7] << "," << '\0' ;
#endif
  strcat(dst,buf);
}

void DBF_Class::PrintRecord()
{
   int i;
   char  string[1000] = "";
   cout << "Deleted:" << (char) RecordDBF[0] << endl;
   for( i = 0 ; i < Number_Of_Fields; i++) {
      cout << Set_Of_Fields[i].name << ":";
      switch (Set_Of_Fields[i].type) {
        case 'N' : 
           PrintNumber( (char*) &RecordDBF[Set_Of_Fields[i].offset],
                          Set_Of_Fields[i].length);
        case 'C': 
           PrintCharacter( (char*) &RecordDBF[Set_Of_Fields[i].offset],
                          Set_Of_Fields[i].length);
           break;
        case 'D' : 
           PrintDate((char*)&RecordDBF[Set_Of_Fields[i].offset]);
           break;
        case 'L' : 
           PrintLogical((char) RecordDBF[Set_Of_Fields[i].offset]);
           break;
        case 'M' : cout << "Not process";
        default : cerr << "Unknown type" << endl;
      }
      cout << string << endl;
   }
}

void DBF_Class::PrintRecord(char* string,int* records, int len)
{
   int i;
   for( i = 0 ; i < len; i++) {
      //cout << Set_Of_Fields[records[i]].name << ":";
      switch (Set_Of_Fields[records[i]].type) {
        case 'N' : 
           PrintNumber( (char*) &RecordDBF[Set_Of_Fields[records[i]].offset],
                     Set_Of_Fields[records[i]].length,string);
           break;
        case 'C':
           PrintCharacter((char*) &RecordDBF[Set_Of_Fields[records[i]].offset],Set_Of_Fields[records[i]].length,string);
           break;
        case 'D' : 
           PrintDate((char*)&RecordDBF[Set_Of_Fields[records[i]].offset],
                      string);
           break;
        case 'L' : 
           PrintLogical((char) RecordDBF[Set_Of_Fields[records[i]].offset],
                      string);
           break;
        case 'M' : cout << "Not process";
        default : cerr << "Unknown type" << endl;
      }
   }
   if ( string[0] ) string[strlen(string) - 1] = '\0';
}


int DBF_Class::DeleteAroundSpaces( char* str, unsigned char len)
{
  unsigned char i;
  unsigned char* ustr = (unsigned char*) str;
  if ( len==0 ) return 0;
  for(i = len-1; i>0 && ustr[i]<=' '; i--);
  if ( i==0 &&  ustr[i]<=' ' ) { str[0]='\0'; return 0; }
  else str[ len = i+1 ] = '\0';
  for(i = 0 ; i<len && ustr[i]<=' '; i++);
  strcpy(str,&str[i]);
  return 0xFFFF;
}


void DBF_Class::Exit()
{
   DBF_file.close();
   if ( Set_Of_Fields!=NULL ) delete [] Set_Of_Fields;
   if ( RecordDBF != NULL ) delete [] RecordDBF;
   if ( DBF_file_name != NULL ) delete [] DBF_file_name;
   exit(1);
}

#if WORDS_BIGENDIAN

void DBF_Class::ConverIntel2SunNumber(char* mas,unsigned char len)
{
   int i,t=0;
   for(i=0;i<len/2;i++) {
     t = mas[i];
     mas[i]=mas[len-1-i];
     mas[len-1-i]=t;
   }
}

#endif
