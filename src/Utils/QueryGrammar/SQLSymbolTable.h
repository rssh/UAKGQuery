#ifndef __Utils_QueryGrammar_SQLSymbolTable_h
#define __Utils_QueryGrammar_SQLSymbolTable_h

/**
 * part of GradSoft Query Service
 * Symbol table for SQL parser.
 * (C) GradSoft 2000, 2001
 * $Id: SQLSymbolTable.h,v 1.8 2001-12-27 09:54:14 yad Exp $
 **/

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif


#include <map>

/**
 * Symbol table for SQL identifieries in SQL query.
 * (for SQL92 select queries, extenden by "WITH KEY" clause 
 **/
class SQLSymbolTable
{
public:

  /**
   * constructor (note - really, SQLSymbolTable is a singleton object
   **/
  SQLSymbolTable();

  /**
   * check: is symbol <code> symbol </code> is a SQL indentifier.
   * if yes - return it's code from y.tab.h,  return otherwise -1
   **/
  static int checkSymbol(const char* symbol);

private:

  struct strcasecmpLess: public std::binary_function<const char*, const char*, bool>
  {
    bool operator()(const char* x,const char* y) const
#ifdef _MSC_VER
    { return stricmp(x,y) < 0; }
#else
    { return strcasecmp(x,y) < 0; }
#endif
  };

public:

  typedef std::map<const char*,int,strcasecmpLess> StorageType;
  typedef StorageType::iterator StorageIteratorType;

private:

  static StorageType& getStorage();
  static bool        initialized_;

  void  init();

private:

  SQLSymbolTable(const SQLSymbolTable&);
  SQLSymbolTable& operator=(const SQLSymbolTable&);

};

#endif
