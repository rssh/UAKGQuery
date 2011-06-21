#ifndef __Oracle8_Oracle8DecimalFormatHandler_h
#define __Oracle8_Oracle8DecimalFormatHandler_h

/**
 * DecimalFormatHandler.h
 * part of implementation of CORBA Query Service.
 * (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 2000
 * $Id: Oracle8DecimalFormatHandler_sv.h,v 1.3 2000-03-15 08:56:42 rssh Exp $
 */

#ifndef __Common_DecimalFormatHandler_h
#include <Common/DecimalFormatHandler.h>
#endif

#ifndef __Oracle8_importOCI_h
#include <Oracle8/importOCI.h>
#endif

#ifndef __importCosQuery_h
#include <importCosQuery.h>
#endif

/**
 * Handler for parsing internal representation of Decimal in Oracle 8
 */
class Oracle8DecimalFormatHandler: public DecimalFormatHandler
{
public:

  /**
   * transform Decimal to raw internal representation, which is 
   * siutable for using in bind variables.
   *
   * memory for return value is allocated in function by std::new.
   */
  void*    transform2raw(const Decimal& dec, const FieldDescription& fd);

  /**
   * transform internal raw representation into decimal.
   * memory for return value is allocated by CORBA Allocators. (new must work)
   */
  Decimal* transform2decimal(void* x,const  FieldDescription& fd);

  /**
   * return length of decimal internal representation in bytes.
   */
  ULong    length() { return 21; }

};

extern Oracle8DecimalFormatHandler oracle8DecimalFormatHandler;

#endif
