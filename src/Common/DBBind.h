#ifndef __Common_DBBind_h
#define __Common_DBBind_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __CosQueryInternalExceptions_h
#include <CosQueryInternalExceptions.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Facade_FieldDescription_h
#include <Facade/FieldDescription.h>
#endif

#ifndef __RecordSet_RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifndef __Common_DBFormatHandler_h
#include <Common/DBFormatHandler.h>
#endif

#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

/*TRASH
#ifdef HAVE_NAMESPACE_STD
#ifndef __STD_USED
using namespace std;
#define __STD_USED
#endif
#endif
*/


#include <vector>


class UAKGQueryBase;

struct InvalidFieldName {};
struct InvalidFieldType {};
struct DBBindException  {
    virtual const char* what() const =0;
};

class DBBindDescription
{
public:

 UAKGQuery2::FieldDescription  getFieldDescription(CORBA::ULong index);

 UAKGQuery2::FieldDescription  getFieldDescription(const char* name);

 void setFieldDescription(CORBA::ULong index, UAKGQuery2::FieldDescription fd);
 void setFieldDescription(const char* name, UAKGQuery2::FieldDescription fd);
 void appendFieldDescription(UAKGQuery2::FieldDescription& fd);

 CORBA::ULong       getFieldIndex(const char* name); 
 bool               equalType(CORBA::ULong index, 
                              UAKGQuery2::FieldType type) const
                     { return (recordSet_->getFieldTypeAt( index )==type); }

 void        checkType(CORBA::ULong index, UAKGQuery2::FieldType type) const
   {  if (!equalType(index,type)) throw InvalidFieldType(); } 

 UAKGQuery2::FieldType   getType(CORBA::ULong index) const
   { return recordSet_->getFieldTypeAt(index); }
 
 /**
  * return internal RecordSet without calling of add_ref 
  **/
 UAKGQuery2::RecordSet*  getRecordSet_()
 {
  return recordSet_;
 }
 
 /**
  * return internal RecordSet and call add_ref 
  **/
 UAKGQuery2::RecordSet*  getRecordSet()
 { 
   recordSet_->_add_ref();
   return recordSet_; 
 }

 CORBA::ULong length() const { return recordSet_->getNColumns(); }

private:
 
 UAKGQuery2::RecordSet*  recordSet_;

public:

 DBBindDescription(UAKGQuery2::RecordSet* rs);
 ~DBBindDescription();

private:

 DBBindDescription(const DBBindDescription&);
 DBBindDescription& operator=(const DBBindDescription&);

};

struct DBFormatHandlerNotSet {};
struct InvalidDate {};

class DBBindSet
{
public:

  struct InvalidFieldSize {};
  struct InvalidRowIndex: DBBindException {
      const char* what() const{
         return "Invalid row index.";
      }
  };
  struct InvalidColumnIndex: DBBindException {
     const char* what() const{
        return "Invalid col index.";
     }
  };

private:

  DBBindSet(const DBBindSet&);
  DBBindSet& operator=(const DBBindSet&);

private:

  DBBindDescription dbBindDescription_; 


  /**
   * how many rows we want fetch during query.
   */
  CORBA::ULong   nRows_;

  /**
   * adresses of data, which we want  to handle
   * (i. e. this adresses are pointed to columns). 
   */
  std::vector<void*>              bindAdresses_;

  /**
   * indicators of NULLS, corresponding to bindAdresses_
   */
  std::vector<CORBA::Short*>      nullIndicators_;

  /**
   * lengths of fields: used when we work with variable-size fields,
   * such as CORBA::Object or Any or varchar2
   */
  std::vector<CORBA::UShort*>              flens_;


  /**
   * true, when memory is allocated.
   */
  bool                    allocated_;
  
  /**
   * pointer to BDFormat
   */
  DBFormatHandler* dbFormatHandler_p_;

  GradSoft::Logger& logger_;

  GradSoft::Logger&         getLogger()     { return logger_; }
  GradSoft::DebugLogStream&    debugs()     { return logger_.debugs(); }
  GradSoft::InfoLogStream&     infos()      { return logger_.infos(); }
  GradSoft::WarningLogStream&  warnings()   { return logger_.warnings(); }
  GradSoft::ErrorLogStream&    errors()     { return logger_.errors(); }
  GradSoft::FatalLogStream&    fatals()     { return logger_.fatals(); }

public:

  DBBindSet(UAKGQuery2::RecordSet* rd, 
			GradSoft::Logger& logger, CORBA::ULong nRows = 1);

 ~DBBindSet();

  void  setColumnAddress(CORBA::ULong index, char* addr, bool isNil) ;

  void  setNRows(CORBA::ULong nRows);

  CORBA::ULong  getNRows() { return nRows_; }
  CORBA::ULong  getNColumns() { return dbBindDescription_.length(); }

  UAKGQuery2::RecordSet* getDescription()
    { return dbBindDescription_.getRecordSet(); }
  
  UAKGQuery2::RecordSet* getDescription_()
    { return dbBindDescription_.getRecordSet_(); }

  UAKGQuery2::FieldDescription  getFieldDescription(CORBA::ULong col)
  { return dbBindDescription_.getFieldDescription(col); }

  CORBA::ULong       getFieldIndex(const char* name)
   { return dbBindDescription_.getFieldIndex(name); }

  void  setField(CORBA::ULong row,CORBA::ULong col, const UAKGQuery2::Field& ft,
             UAKGQueryBase& q);

  void  getField(CORBA::ULong row,CORBA::ULong col, UAKGQuery2::Field& ft, UAKGQueryBase& q);

  void  setNull(CORBA::ULong row,CORBA::ULong col, bool flg);

protected:

  void  setBoolean(CORBA::ULong row, CORBA::ULong col, const CORBA::Boolean value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeBoolean);
    *((CORBA::Boolean*)(getMemPtr(row,col,sizeof(CORBA::Boolean))))=value;
    flens_[col][row]=sizeof(CORBA::Boolean);
  }

  CORBA::Boolean  getBoolean(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeBoolean);
    return *((CORBA::Boolean*)(getMemPtr(row,col,sizeof(CORBA::Boolean))));
  }


  void  setChar(CORBA::ULong row, CORBA::ULong col, const CORBA::Char value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeChar);
    *((CORBA::Char*)(getMemPtr(row,col,sizeof(CORBA::Char))))=value;
    flens_[col][row]=sizeof(CORBA::Char);
  }


  CORBA::Char  getChar(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeChar);
    return *((CORBA::Char*)(getMemPtr(row,col,sizeof(CORBA::Char))));
  }


  void  setOctet(CORBA::ULong row, CORBA::ULong col, const CORBA::Octet value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeOctet);
    *((CORBA::Octet*)(getMemPtr(row,col,sizeof(CORBA::Octet))))=value;
    flens_[col][row]=sizeof(CORBA::Octet);
  }

  CORBA::Octet  getOctet(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeOctet);
    return *((CORBA::Octet*)(getMemPtr(row,col,sizeof(CORBA::Octet))));
  }


  void  setShort(CORBA::ULong row, CORBA::ULong col, CORBA::Short value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeShort);
    *((CORBA::Short*)(getMemPtr(row,col,sizeof(CORBA::Short))))=value;
    flens_[col][row]=sizeof(CORBA::Short);
  }

  void  setShort_(CORBA::ULong row, CORBA::ULong col, CORBA::Short value)
  {
    *((CORBA::Short*)(getMemPtr(row,col,sizeof(CORBA::Short))))=value;
    flens_[col][row]=sizeof(CORBA::Short);
  }

  CORBA::Short  getShort(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeShort);
    return *((CORBA::Short*)(getMemPtr(row,col,sizeof(CORBA::Short))));
  }

  CORBA::Short  getShort_(CORBA::ULong row, CORBA::ULong col)
  {
    return *((CORBA::Short*)(getMemPtr(row,col,sizeof(CORBA::Short))));
  }

  void  setUShort(CORBA::ULong row, CORBA::ULong col, CORBA::UShort value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeUShort);
    *((CORBA::UShort*)(getMemPtr(row,col,sizeof(CORBA::UShort))))=value;
    flens_[col][row]=sizeof(CORBA::UShort);
  }

  CORBA::UShort  getUShort(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeUShort);
    return *((CORBA::UShort*)(getMemPtr(row,col,sizeof(CORBA::UShort))));
  }


  void  setLong(CORBA::ULong row, CORBA::ULong col, CORBA::Long value)
  {
   dbBindDescription_.checkType(col,UAKGQuery2::TypeLong);
    *((CORBA::Long*)(getMemPtr(row,col,sizeof(CORBA::Long))))=value;
    flens_[col][row]=sizeof(CORBA::Long);
  }

  void  setLong_(CORBA::ULong row, CORBA::ULong col, CORBA::Long value)
  {
    *((CORBA::Long*)(getMemPtr(row,col,sizeof(CORBA::Long))))=value;
    flens_[col][row]=sizeof(CORBA::Long);
  }

  CORBA::Long  getLong(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeLong);
    return *((CORBA::Long*)(getMemPtr(row,col,sizeof(CORBA::Long))));
  }

  CORBA::Long  getLong_(CORBA::ULong row, CORBA::ULong col)
  {
    return *((CORBA::Long*)(getMemPtr(row,col,sizeof(CORBA::Long))));
  }

  void  setULong(CORBA::ULong row, CORBA::ULong col, CORBA::ULong value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeULong);
    *((CORBA::ULong*)(getMemPtr(row,col,sizeof(CORBA::ULong))))=value;
    flens_[col][row]=sizeof(CORBA::ULong);
  }

  CORBA::ULong  getULong(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeULong);
    return *((CORBA::ULong*)(getMemPtr(row,col,sizeof(CORBA::ULong))));
  }


  void  setFloat(CORBA::ULong row, CORBA::ULong col, CORBA::Float value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeFloat);
    setFloat_(row,col,value);
    flens_[col][row]=sizeof(CORBA::Float);
  }

  void  setFloat_(CORBA::ULong row, CORBA::ULong col, CORBA::Float value)
  {
    *((CORBA::Float*)(getMemPtr(row,col,sizeof(CORBA::Float))))=value;
  }
  
  CORBA::Float  getFloat(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeFloat);
    return *((CORBA::Float*)(getMemPtr(row,col,sizeof(CORBA::Float))));
  }

  CORBA::Float  getFloat_(CORBA::ULong row, CORBA::ULong col)
  {
    return *((CORBA::Float*)(getMemPtr(row,col,sizeof(CORBA::Float))));
  }
  
  CORBA::Double  getDouble(CORBA::ULong row, CORBA::ULong col)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeDouble);
    return *((CORBA::Double*)(getMemPtr(row,col,sizeof(CORBA::Double))));
  }

  CORBA::Double  getDouble_(CORBA::ULong row, CORBA::ULong col)
  {
    return *((CORBA::Double*)(getMemPtr(row,col,sizeof(CORBA::Double))));
  }
  
  void  setDouble(CORBA::ULong row, CORBA::ULong col, CORBA::Double value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeDouble);
    *((CORBA::Double*)(getMemPtr(row,col,sizeof(CORBA::Double))))=value;
    flens_[col][row]=sizeof(CORBA::Double);
  }
  
  void  setDouble_(CORBA::ULong row, CORBA::ULong col, CORBA::Double value)
  {
    *((CORBA::Double*)(getMemPtr(row,col,sizeof(CORBA::Double))))=value;
    flens_[col][row]=sizeof(CORBA::Double);
  }
  
  void  setString(CORBA::ULong row, CORBA::ULong col, const char* value);

  void  setString_(CORBA::ULong row, CORBA::ULong col, const char* value);

  char* getStringPtr(CORBA::ULong row, CORBA::ULong col)
  { 
    size_t flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col);
    return (char*)getMemPtr(row,col,sizeof(char)*flen);
  }

  const char* getConstStringPtr(CORBA::ULong row, CORBA::ULong col)
  { 
    size_t flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col);
    return (const char*)getMemPtr(row,col,sizeof(char)*flen);
  }


  void  setClobId(CORBA::ULong row, CORBA::ULong col, 
                  void* lobData)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeClob);
    setRawMem(row,col,lobData,dbFormatHandler_p_->rawid_length());
  }

  const unsigned char* getClobIdPtr(CORBA::ULong row, CORBA::ULong col)
  { 
    CORBA::ULong flen=dbFormatHandler_p_->rawid_length();
    return (const unsigned char*)getMemPtr(row,col,sizeof(char)*flen);
  }

  UAKGQuery2::OctSeq* getClobId(CORBA::ULong row, CORBA::ULong col)
  { 
   CORBA::ULong flen=dbFormatHandler_p_->rawid_length();
   UAKGQuery2::OctSeq_var retval = new UAKGQuery2::OctSeq();
   retval->length(flen);
   memcpy(retval->get_buffer(),getMemPtr(row,col,flen),flen);
   return retval._retn();
  }

  void  setWString(CORBA::ULong row, CORBA::ULong col, const CORBA::WChar* value);

  // get utf16 wstring-ptr
  CORBA::UShort* getWStringPtr(CORBA::ULong row, CORBA::ULong col)
  {
    size_t flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col)+2;
    return (CORBA::UShort*)getMemPtr(row,col,sizeof(char)*flen);
  }

  void  setWclobId(CORBA::ULong row, CORBA::ULong col, const void* value)
  {
    dbBindDescription_.checkType(col,UAKGQuery2::TypeWclob);
    setRawMem(row,col,value,dbFormatHandler_p_->rawid_length());
  }

  const unsigned char* getWclobIdPtr(CORBA::ULong row, CORBA::ULong col)
  {
    return (const unsigned char*)getRawMem(row,col);
  }

  UAKGQuery2::OctSeq* getWclobId(CORBA::ULong row, CORBA::ULong col)
  { 
   CORBA::ULong flen=dbFormatHandler_p_->rawid_length();
   UAKGQuery2::OctSeq_var retval = new UAKGQuery2::OctSeq();
   retval->length(flen);
   memcpy(retval->get_buffer(),getMemPtr(row,col,flen),flen);
   return retval._retn();
  }

/*
  void  setObject(CORBA::ULong row, CORBA::ULong col, CORBA::Object_ptr value,
          CORBA::ORB_ptr orb)
  {

//// Change Here
//    dbBindDescription_.checkType(col,UAKGQuery2::TypeObject);
//    CORBA::String_var s = orb->object_to_string(value);
//    setString_(row,col,s.in());
  }

  CORBA::Object*  getObject(CORBA::ULong row, CORBA::ULong col, CORBA::ORB_ptr orb)
  {
//// Change Here
//    dbBindDescription_.checkType(col,UAKGQuery2::TypeObject);
//    const char* s = getStringPtr(row,col);
//    CORBA::Object_var o = orb->string_to_object(s);
//    return o._retn();
     return NULL;
  }

*/

  void  setSmallInt(CORBA::ULong row, CORBA::ULong col, CORBA::Short value)
  {
    setShort_(row,col,value);
  }

  CORBA::Short getSmallInt(CORBA::ULong row, CORBA::ULong col)
  {
    return getShort_(row,col);
  }

  void  setInteger(CORBA::ULong row, CORBA::ULong col, CORBA::Long value)
                               { setLong_(row,col,value); }

  CORBA::Long getInteger(CORBA::ULong row, CORBA::ULong col)
  {
    return getLong_(row,col);
  }

  void  setReal(CORBA::ULong row, CORBA::ULong col, CORBA::Float value) { setFloat_(row,col,value); }
  CORBA::Float getReal(CORBA::ULong row, CORBA::ULong col) { return getFloat_(row,col); }

  void  setDoublePrecision(CORBA::ULong row, CORBA::ULong col, CORBA::Double value) 
         { setDouble_(row,col,value); }

  CORBA::Double getDoublePrecision(CORBA::ULong row, CORBA::ULong col) 
         { return getDouble_(row,col); }


  void  setCharacter(CORBA::ULong row, CORBA::ULong col, const char* value);

  char* getCharacterPtr(CORBA::ULong row, CORBA::ULong col)
  {
    return getStringPtr(row,col);
  }

  const char* getConstCharacterPtr(CORBA::ULong row, CORBA::ULong col)
  {
    return getConstStringPtr(row,col);
  }

  void checkDBFormatHandler()
  {
    if (dbFormatHandler_p_ == NULL) {
     throw DBFormatHandlerNotSet();
    }
  }

  void  checkIndexes(CORBA::ULong row, CORBA::ULong col)
  {
    if (row >= getNRows() )
    {
       throw UAKGQuery2::InvalidRow();
    }
    if (col >= getNColumns() )
    {
       throw UAKGQuery2::InvalidColumn();
    }
  }

  void  setDecimal(CORBA::ULong row, CORBA::ULong col,UAKGQuery2::Numeric* decimal);
  
  UAKGQuery2::Numeric*  getDecimal(CORBA::ULong row, CORBA::ULong col);

  void  setNumeric(CORBA::ULong row, CORBA::ULong col,UAKGQuery2::Numeric* decimal);
  
  UAKGQuery2::Numeric*  getNumeric(CORBA::ULong row, CORBA::ULong col);

  void  setDateTime(CORBA::ULong row, CORBA::ULong col, const UAKGQuery2::DateTime& datetime);

  UAKGQuery2::DateTime   getDateTime(CORBA::ULong row, CORBA::ULong col);

  /**
   * set raw data (in sence raw memory)
   **/
  void      setRawMem(CORBA::ULong row, CORBA::ULong col, 
                      const void* buff, CORBA::ULong len);

  /**
   * set raw field (in sence field of type TypeRaw)
   **/
  void      setRaw(CORBA::ULong row, CORBA::ULong col, void* buff, CORBA::ULong len);

  /**
   * set raw data
   **/
  void      setRawMem(CORBA::ULong row, CORBA::ULong col, const UAKGQuery2::OctSeq& value);

  /**
   * set Raw field
   **/
  void      setRaw(CORBA::ULong row, CORBA::ULong col, const UAKGQuery2::OctSeq& value);

public:
  /**
   * get Raw data
   **/
  void*     getRawMemBuffPtr(CORBA::ULong row, CORBA::ULong col);

protected:

  /**
   * get Raw field in newly allocated (by CORBA alloc) buffer.
   */
  UAKGQuery2::OctSeq*   getRawMem(CORBA::ULong row, CORBA::ULong col);

  /**
   * get Raw field in newly allocated (by CORBA alloc) buffer.
   */
  UAKGQuery2::OctSeq*   getRaw(CORBA::ULong row, CORBA::ULong col);


  void      setBlobId(CORBA::ULong row, CORBA::ULong col, 
                      const void* );

  const unsigned char* getBlobIdPtr(CORBA::ULong row, CORBA::ULong col)
  {
    return (const unsigned char*)getRawMemBuffPtr(row,col);
  }

  UAKGQuery2::OctSeq*   getBlobId(CORBA::ULong row, CORBA::ULong col)
  {
   CORBA::ULong flen=dbFormatHandler_p_->rawid_length();
   UAKGQuery2::OctSeq_var retval = new UAKGQuery2::OctSeq();
   retval->length(flen);
   memcpy(retval->get_buffer(),getMemPtr(row,col,flen),flen);
   return retval._retn();
  }


protected:


public:

  void* getMemPtr(CORBA::ULong row, CORBA::ULong col, size_t s)
  {
   if (!allocated_) allocate();
   return ((char*)bindAdresses_[col])+row*s;
  }

  void* getColAddr(CORBA::ULong col)
  {
   if (!allocated_) allocate();
   return bindAdresses_[col];
  }

  void* getColNullIndicators(CORBA::ULong col)
  {
   if (!allocated_) allocate();
   return nullIndicators_[col];
  }

  CORBA::UShort* getColFlens(CORBA::ULong col)
  {
   if (!allocated_) allocate();
   return flens_[col];
  }

  CORBA::UShort getFlen(CORBA::ULong row, CORBA::ULong col)
  {
   if (!allocated_) allocate();
   return flens_[col][row];
  }


  UAKGQuery2::FieldType  getFieldType(CORBA::ULong index)  
  { 
        return dbBindDescription_.getType(index); 
  }

  void  setDBFormatHandler_p(DBFormatHandler* dbFormatHandler_p)
  {
    dbFormatHandler_p_ = dbFormatHandler_p;
  }

  CORBA::Short* getNullIndicatorPtr(CORBA::ULong row, CORBA::ULong col)
  {
     if (!allocated_) allocate();
     return  &nullIndicators_[col][row];
  }

  void allocate();
  void initialize();
  void deallocate();

  CORBA::UShort  getDefaultLengthOfField(CORBA::ULong col);

  void remove_this_function();

// debug

  void dumpColumn(std::ostream& out, int col);

};

#endif
