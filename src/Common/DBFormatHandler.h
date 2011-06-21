#ifndef __Common_DBFormatHandler_h
#define __Common_DBFormatHandler_h

/**
 * DBFormatHandler.h
 * part of implementation of CORBA Query Service.
 * (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 2000
 * $Id: DBFormatHandler.h,v 1.16 2002-08-22 16:58:30 rssh Exp $
 */

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Facade_FieldDescription_h
#include <Facade/FieldDescription.h>
#endif

/**
 * Handler for parsing internal representation of data types
 * (such as decimal or date) in DB.
 * (i. e. we must have one class per DB type)
 */
class DBFormatHandler
{
public:


  /**
   * transform Decimal to raw internal representation, which is 
   * siutable for using in bind variables.
   *
   * memory for return value is allocated in function by std::new.
   */
  virtual void*   decimal2rawmem(UAKGQuery2::Numeric* dec, 
                                 const UAKGQuery2::FieldDescription& fd ) = 0;

  /**
   * transform internal raw memory representation into decimal.
   * memory for return value is allocated by CORBA allocators in raw 
   * representation.
   */
  virtual UAKGQuery2::Numeric* rawmem2decimal(void* , const UAKGQuery2::FieldDescription& fd) = 0;

  /**
   * return length of decimal internal representation in bytes.
   */
  virtual CORBA::ULong    decimal_length()=0;

  //
  // UKAGQuery::DateTime
  //

  /**
   * transform CosQueryCollection::DateTime to raw internal representation, which is 
   * siutable for using in bind variables.
   *
   * memory for return value is allocated in function by std::new.
   */
  virtual void*   datetime2rawmem(const UAKGQuery2::DateTime& datetime, 
                                  const UAKGQuery2::FieldDescription& fd ) = 0;

  /**
   * transform internal raw representation into datetime
   * memory for return value is allocated by CORBA allocators in raw 
   * representation.
   */
  virtual UAKGQuery2::DateTime rawmem2datetime(void*, const UAKGQuery2::FieldDescription& fd) = 0;

  /**
   * return length of datetime internal representation in bytes.
   */
  virtual CORBA::ULong    datetime_length()=0;

  //
  // Object
  //

  /**
   * return length of object internal representation in bytes.
   */
 // virtual CORBA::ULong    object_length()=0;

  //
  // rawid
  //

  /**
   * return length of rawid internal representation in bytes.
   */
  virtual CORBA::ULong    rawid_length()=0;

  //
  // Raw
  //
  /**
   * transform Raw field to raw internal representation, which is 
   * siutable for using in bind variables.
   *
   * memory for return value must be allocated at caller, and have
   * at least "raw_length(fd)" size
   */
  virtual void raw2rawmem_fill(void* buffer, CORBA::ULong bufflen, void* memptr,
                                 const UAKGQuery2::FieldDescription& fd ) = 0;

  /**
   * transform internal raw memory representation into raw field.
   * 
   * memory for return value is allocated by CORBA allocators 
   **/
  virtual UAKGQuery2::OctSeq*  rawmem2raw(void*, const UAKGQuery2::FieldDescription& fd) = 0;

  /**
   * return length of [acket represntation of raw
   **/
  virtual CORBA::ULong    raw_length(const UAKGQuery2::FieldDescription& fd)=0;

  /**
   *  
   **/
  virtual void* allocate(UAKGQuery2::FieldType t, size_t size) {
      void* p = new char*[size];
      memset(p,0,size);
      return p;
  }

  /**
   *
   **/
  virtual void deallocate(UAKGQuery2::FieldType t, void*& p, size_t size) 
  {
      delete [] ((char*)p);
      p = NULL;
  }
  
};

#endif
