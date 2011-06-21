#ifndef __InterBase_InterBaseDBFormatHandler_h
#define __InterBase_InterBaseDBFormatHandler_h

/**
 * DecimalFormatHandler.h
 * part of implementation of CORBA Query Service.
 * (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 2000
 * $Id: InterBaseDBFormatHandler.h,v 1.8 2002-08-27 11:13:56 rssh Exp $
 */

#ifndef __Common_DBFormatHandler_h
#include <Common/DBFormatHandler.h>
#endif

#ifndef __InterBase_importIBASE_h
#include <InterBase/importIBASE.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Numeric_NumericImpl_h_
#include <Numeric/NumericImpl.h>
#endif

/**
 * Handler for parsing internal representation of Decimal in Oracle 8
 */
class InterBaseDBFormatHandler: public DBFormatHandler
{
public:

  /**
   * transform Decimal to raw internal representation, which is 
   * siutable for using in bind variables.
   *
   * memory for return value is allocated in function by std::new.
   */
  void*    decimal2rawmem(UAKGQuery2::Numeric* dec, const UAKGQuery2::FieldDescription& fd);

  /**
   * transform internal raw representation into decimal.
   * memory for return value is allocated by CORBA Allocators. 
   *  (i. e. by Decimal::operator new) 
   */
  UAKGQuery2::Numeric* rawmem2decimal(void* x,const  UAKGQuery2::FieldDescription& fd);


  /**
   * return length of decimal internal representation in bytes.
   */
  CORBA::ULong    decimal_length() { return 8; }

  /**
   * transform Datetime to raw internal representation, which is 
   * siutable for using in bind variables.
   *
   * memory for return value is allocated in function by std::new.
   */
  void*    datetime2rawmem(const UAKGQuery2::DateTime& dt, const UAKGQuery2::FieldDescription& fd);

  /**
   * transform internal raw representation into datetime
   */
  UAKGQuery2::DateTime rawmem2datetime(void* x,const  UAKGQuery2::FieldDescription& fd);

  /**
   * return length of datetime internal representation in bytes.
   */
  CORBA::ULong    datetime_length() { return 7; }

  /**
   * return length of object (usially - IOR) internal representation in bytes.
   */
  CORBA::ULong    object_length() 
                { return 1000; }

  CORBA::ULong    rawid_length() 
                 { return sizeof(ISC_QUAD); }

  //
  /**
   * transform Raw field to raw internal representation, which is
   * siutable for using in bind variables.
   *
   */
  void raw2rawmem_fill(void* buffer, CORBA::ULong bufflen, void* memptr,
                                 const UAKGQuery2::FieldDescription& fd );

  /**
   * transform internal raw memory representation into raw field.
   *
   * memory for return value is allocated by CORBA allocators
   **/
  UAKGQuery2::OctSeq*  rawmem2raw(void*, const UAKGQuery2::FieldDescription& fd);

  /**
   * return length of packet represntation of raw
   **/
  CORBA::ULong    raw_length(const UAKGQuery2::FieldDescription& fd);


};

extern InterBaseDBFormatHandler interBaseDBFormatHandler;

#endif
