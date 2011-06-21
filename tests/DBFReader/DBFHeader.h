#ifndef ___DEFINE_DBF_HEADER___ 
#define ___DEFINE_DBF_HEADER___

#include <fstream>

/**/

/* 
------------------  DBF Title discription ------------------------
00 - byte
                       FoxBASE+/dBASE III +, without memo - 0È03
                       FoxBASE+/dBASE III +, with memo - 0È83
                       FoxPro/dBASE IV, without memo - 0È03
                       FoxPro with memo - 0ÈF5
                       dBASE IV with memo - 0x8B
01-03 - bytes
                       Date of last modification
                       in format yy/mm/dd
04-07 - bytes
                       Number of records in DBF file.
08-09 - bytes
                       Position of first record in DBF file.
10-11 - bytes
                       Length of one record.
12-27 - bytes
                       Reserved.
28    - byte
                       Is there complex index structure ? (CDX)
                       1 - Yes; 0 - No
29-31 - bytes 
                       Reserved.
------------------------------------------------------------------
*/

#define SIZE_OF_DBF_TITLE 32 
#define END_OF_FIELDS  0x0D
#define SIZE_OF_FIELD_TITLE 32
#define END_OF_DBF_FILE 0x1A
#define MAX_LEN_NAME_FIELD 11

struct DBF_title_header_type {
   char  language_identifier ;
   char  last_data_modification[4]; 
   int   number_of_records;
   short position_of_first_record;
   short record_length;   
   char  index;            
};

/*
------------------  DBF Field discription ------------------------
0-10 - bytes            Field name
11   - byte             Field type 
                          C - Character
                          N - Numeber (0 - 9)
                          L - Logical (Y,y,T,t,N,n,F,f,?)
                          D - Date (yyyy/mm/dd)
                          M - Memo
12-15 - bytes           Offset from begin
16    - byte            Field length
17    - byte            Field counter
18-19 - bytes           Reserved
20    - byte            ID work domain
21-22 - bytes           Reserved
23    - byte            Flag SET FIELDS
24-31 - bytes           Reseved
------------------------------------------------------------------
*/

struct DBF_field_type {
   char   name[11];
   char   type;
   int    offset; 
   unsigned char   length;
};


class DBF_Class {
protected:
  std::ifstream         DBF_file;
  DBF_title_header_type DBF_title_header;
  DBF_field_type*       Set_Of_Fields;
  int                   Number_Of_Fields;
  char*                 RecordDBF;
  char*                 DBF_file_name;
private:
  unsigned int          OFFSET;

protected:

  void Convert_Pointer_2_DBFStruct(char* Ptr_to_title);
  void Read_DBF_fields();
  void Conver_Pointer_2_TITLEStruct(char* Ptr_field,DBF_field_type* field);
  void PrintNumber( char* ptr_str, unsigned char length );
  void PrintNumber( char* ptr_str, unsigned char length, char* dst);
  void PrintCharacter( char* ptr_str, unsigned char length );
  void PrintCharacter( char* ptr_str, unsigned char length, char* dst);
  void PrintLogical( char ch );
  void PrintLogical( char ch, char* dst);
  void PrintDate(char* date);
  void PrintDate(char* date, char* dst );
  int  DeleteAroundSpaces(char* str, unsigned char len);
#if WORDS_BIGENDIAN
  void ConverIntel2SunNumber(char* str, unsigned char len);
#endif
  void Exit();
public:

  DBF_Class() : Set_Of_Fields(NULL), Number_Of_Fields(0),
                RecordDBF(NULL), OFFSET(1), DBF_file_name(NULL)
                {};
  void open(char* filename);
  int  GetFirst();
  int  GetNext();
  void Print_DBF_title();
  void Print_DBF_fields();
  void PrintRecord();
  void PrintRecord(char* string,int* records,int num);
  ~DBF_Class() { Exit(); }
};

#endif
