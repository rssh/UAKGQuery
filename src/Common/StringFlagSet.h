#ifndef __Common_StringFlagSet_h
#define __Common_StringFlagSet_h

#ifndef __GradSoft_RWLock_h
#include <GradSoft/RWLock.h>
#endif

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif


#include <functional>
#include <map>
#include <deque>
#include <iostream>
#ifdef HAVE_CSTRING
#include <cstring>
#endif


/*
 * part of UAKGQuery implementation.
 * (C) GradSoft, 2000
 * author: Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>
 * $Id: StringFlagSet.h,v 1.11 2008-05-31 18:17:06 rssh Exp $
 */

/**
 * StringFlagSet is a set of string flags.
 * flag is a pair of string name and value.
 * examples:
 *   ("TransactionReqired", 1)
 *   ("chunkSize", 200)
 *   ("ConnectString","scott/tiger@my.company.com")
 *   ("otid","[SS1234,22]")
 * 
 * value have integer and string representation.
 * string representation of value is a value itself,
 * integer: result of strtol(3), applied to string representation
 *          othervise 0
 *
 * StringFlagSet is an associative hierarchical storage of flags.
 * i. e. this is set of value, which can be getted by name, 
 * and StringFlagSet can have parent, in this case search of
 * property in set means search in this set nad search in it parent. 
 *
 * access to StringFlagSet is thread-safe.
 **/
class StringFlagSet
{
public:

  struct ParseError 
  {
   const char* why;
   //msg should be only constant string literal, so implementation is correct
   ParseError(const char* msg) 
    :why(msg) { }
  };

public:

  /**
   * constructor: accept a parent as parameter.
   **/
  StringFlagSet(const StringFlagSet* parent_p = NULL);

  ~StringFlagSet();

  /**
   * parse string as set of flags, according with next BNF grammar:
   *   
   *   FlagDefs: 
   *            |
   *             FlagDef ',' FlagDefs
   *            ;
   *
   *   FlagDef: FlagName
   *           |
   *            FlagName '=' Value
   *           ;
   *
   *   Value:
   *          IntValue
   *         |
   *          StringValue
   *         ;
   *
   *   FlagName: ([a-z]|[0-9])*
   *   IntValue: [0-9]*
   *   StringValue: ([^,]|[\,])*
   *               ;
   *
   * example:
   *    "ReadOnly,ChunkSize=100"
   **/ 
  void  parse(const char* str); 

  int   get_int(const char* name) const;

  const char* get_sz(const char* name) const;

  void print(std::ostream& out);


//private: compiler bug
public:


  struct Chunk
  {
   char* frs;
   char* snd;
  };

  struct FlagDef
  {
   Chunk name;
   Chunk value;
  };

private:

  void  parseFlagDefs(const char*& str);
  FlagDef  parseFlagDef(const char*& str);
  Chunk parseName(const char*& str);
  Chunk parseValue(const char*& str);

  void addFlagDef(FlagDef& flagDef);

  // note: name ownership is passed to storage
  void addString(const char* name, const char* value);

  // note: name ownership is passed to storage
  void addInt(const char* name, int value);


private:

 class FlagValue
 {
  private:

   bool is_int_;
   int  int_value_;
   char* sz_value_;

  public:

   FlagValue()
    :is_int_(0),int_value_(0),sz_value_(NULL)
   {}

   FlagValue(const char* value);
   FlagValue(int value);
   FlagValue(const FlagValue& x);

  ~FlagValue();

   FlagValue& operator=(const FlagValue& x)
   {
    if (this!=&x) {
      setValue(x);
    }
    return *this;
   }

   const char* getStringValue_p() const
    { return sz_value_; }

   int         getIntValue()  const
    { return int_value_; }
   
  private:
   
    void setValue(const FlagValue& x);

 };


 struct strless: public std::binary_function<const char*, const char*, bool>
 {
  bool operator()(const char* x, const char* y) const
  {
    return (strcmp(x,y) < 0);
  }
 };

 typedef std::map<const char*, FlagValue, strless> StorageType;
 typedef StorageType::iterator StorageIteratorType;
 typedef StorageType::const_iterator StorageConstIteratorType;

 const StringFlagSet* parent_p_;
 StorageType storage_;
 GradSoft::RWLock      rwlock_;

private:


  StorageConstIteratorType find(const char* name, bool& found)  const
  {
   GradSoft::ReadLocker rl(rwlock_);
   StorageConstIteratorType it=storage_.find(name);
   found=(it!=storage_.end());
   if (!found && parent_p_!=NULL) {
     it = parent_p_->find(name,found);
   }
   return it;
  }

  void printChunk(std::ostream& out, const Chunk& chunk);


};


#endif
