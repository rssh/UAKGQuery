#ifndef __Common_UAKGQueryUtils_h
#define __Common_UAKGQueryUtils_h

#ifndef __CosQueryPostConfig_h
#include <CosQueryPostConfig.h>
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

#ifdef CORBA_HAVE_OTS
#ifdef ORBACUS
#include <OB/xa.h>
#else
#include <xa.h>
#endif
#endif

#include <iostream>


struct ParameterNotFound {};

#if 0
class QueryDestroyGuard
{
  UAKGQuery2::Query_ptr q_;
public:  
  QueryDestroyGuard(UAKGQuery2::Query_ptr q)
    :q_(q) {}
  ~QueryDestroyGuard()
  { q_->destroy(); }
private:
  QueryDestroyGuard();
  QueryDestroyGuard(const QueryDestroyGuard&);
  QueryDestroyGuard operator=(const QueryDestroyGuard&);
};

#endif

class UAKGQueryUtils
{
public:

  
  /**
   * transform char** to StringSeq
   **/
  static UAKGQuery2::StringSeq* szArray2StringSeq(const char * const* szArray);

#ifdef CORBA_HAVE_OTS
  /**
   * transform OTS otid to XA xid
   *  (result is passed into xid, memory for xid must be allocated by caller)
   **/
  static void  otid_to_xid(const CosTransactions::otid_t& otid, XID& xid);

  /**
   * print XA xid (used in debug purposes)
   **/
  static void  print_xid(std::ostream& out, const XID& xid);

#endif

   /**
    * returns true if <code> dt </code> represent correct date.
    **/
   static bool checkDate ( const UAKGQuery2::DateTime& dt );


   /**
    * print field description.
    **/
   static void printFieldDescription(std::ostream& out,
                          const UAKGQuery2::FieldDescription& fd);

   /**
    * print field description.
    **/
   static void printFieldType(std::ostream& out, UAKGQuery2::FieldType ft);


   /**
    * print record description.
    **/
   static void printRecordSetDescription(std::ostream& out, 
                                         UAKGQuery2::RecordSet* rs);


   /**
    * print record set.
    **/
   static void printRecordSet(std::ostream& out,
                              UAKGQuery2::RecordSet* rs);


   /**
   * print field
   **/
/*
   static void printField(std::ostream& out,
                          const UAKGQuery2::Field fd);
*/

   /**
   * print Decimal
   **/
   static void printNumeric(std::ostream& out,
                                     const UAKGQuery2::Numeric* dec);

   /**
   * print DateTime
   **/
   static void printDateTime(std::ostream& out,
                                     const UAKGQuery2::DateTime& dec);

   /**
   *  print Raw
   **/
   static void printRaw(std::ostream& out, const UAKGQuery2::OctSeq* octSeq);

   /**
    * transform long to string
    **/
   static std::string longToString(long x);

   /**
    * code s with method 'methodCode' using key 'key'
    *  in such way, that
    *     1. return string does not contains character expCh.
    *     2. uncode(code(s,ch,key,m),ch,key,m)=s for any s.
    * code 
    *   note - this code does not guarantue real security. 
    *   (which depends from method).
    **/
   static std::string code(const std::string& s, char expCh, 
		            const std::string& key, int methodCode=0);
		           
   /**
    * uncode s, previously coded by code with same arguments
    **/
   static std::string uncode(const std::string& s, char expCh, 
		            const std::string& key, int methodCode=0);
   

   
   static const char*  getStringEncoding();


   static const char*  getWStringEncoding();

};

inline
std::ostream& operator<<(std::ostream& out, UAKGQuery2::RecordSet* rs)
{
  UAKGQueryUtils::printRecordSet(out,rs);
  return out;
}

inline
std::ostream& operator<<(std::ostream& out, UAKGQuery2::Numeric* num)
{
  UAKGQueryUtils::printNumeric(out,num);
  return out;
}


#endif
