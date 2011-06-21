#ifndef __Utils_SQLTerm_h
#define __Utils_SQLTerm_h

/*
 * part of UAKG Query Service
 * (C) GradSoft 2001
 * http://www.gradsoft.com.ua
 * author: Ruslan Shevchenko
 * $Id: SQLTerm.h,v 1.16 2003-04-10 18:41:28 rin Exp $
 */

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#include <iostream>

#ifndef __Utils_SQLTermMemoryTracker_h
#include <Utils/SQLTermMemoryTracker.h>
#endif


/**
 * class for representation of SQL TERM.
 **/
class SQLTerm
{
public:

  enum Type {
    Error,       // we have no members and szvalue 

    ConstString, // szvalue is point to const string, which is not deleted
                 // members_p is NULL

    StringTerm,  // szvalue is point to string. 

    SelectExpr,  // represent SELECT query
                  // degree_ is 4
                  // Flawor is FlaworSelectOptALL oe FlaworSelectOptDistinct
                  //  members_p_[0] is StringTerm <selection> [from grammar]
                  //  members_p_[1] is TableExpr 
                  //  members_p_[2] is opt_order_by_clause as StringTerm
                  //    NULL, if empty, 
                  //    otherwise: string which behing order-by
                  //  members_p_[3] is opt_key_clause as StringTerm
                  //                 contine <selection> from grammar
                  //    NULL, if empty, 
                  //    otherwise: string which behing order-by
                  // szvalue is NULL

    SelectOptAll, // represent option of Select. all fields are null

    SelectOptDistinct, // represent option of SELECT clause.
                       // all fields are NULL 

    TableExpr        // represent table_expr expression
                     // degree_ is 4 
                     // members_p_[0] is StringTerm <table_ref_command_list>
                     // members_p_[1] is StringTerm <search_condition>
                     //  from WHERE clause or NULL, if where is absent
                     // members_p_[2] is StringTerm <column_ref_commalist> 
                     //  from GROUP BY clause, or NULL if GROUP by clause
                     //  is empty
                     // members_p_[3] is StringTerm <search_condition> 
                     //  from HAVING clause, or NULL
                     // szvalue is NULL
  };

  enum Flawor
  {
    UndefinedFlawor,
    FlaworSelectOptAll,
    FlaworSelectOptDistinct
  };

private:

  // type
  Type type_;

  // various flawors.
  Flawor flawor_;

  // number of members. degre_==0 => members_p_=NULL
  int degree_;

  SQLTerm** members_p_;

  //
  char* szValue_; 

  SQLTermMemoryTracker& memoryTracker_;

private:

  // omitting gcc bug
  friend class SQLTermMemoryTracker;

  SQLTerm(SQLTermMemoryTracker& memoryTracker, 
          Type type, 
          Flawor flawor, 
          int degree, 
          SQLTerm** members, 
          char* szValue);

public:

   static SQLTerm* createSQLTerm(SQLTermMemoryTracker& memoryTracker,
                          Type type,
                          Flawor flawor, 
                          int degree, 
                          SQLTerm** members, 
                          char* szValue);

  ~SQLTerm();

public:

  struct TypeMismatch {};

  void print(std::ostream& os) const;

  bool isSelect() const
   { return type_ == SelectExpr; }

  bool isTableExpr() const
   { return type_ == TableExpr; }

  bool isSelectOptAll() const
   { return type_ == SelectOptAll; }

  bool isSelectOptDistinct() const
   { return type_ == SelectOptDistinct; }

  /**
   *return SelectionList in SELECT query as string 
   *memory for returning value is allocated in 
   *        function by CORBA allocator
   *@precondition SQLTerm must be select term
   *@precondition SQLTerm must be build correctly
   *@returns SelectionsInString
   **/
  char* getSelectionListString() const;

  /**
   *return FromPart (i. e. table_ref_commalist ) as string 
   * for Select or TableExpr term.
   *memory for returning value is allocated in 
   *        function by CORBA allocator
   *@precondition SQLTerm must be select term 
   *@precondition SQLTerm must be build correctly
   *@returns list of table refs as string.
   **/
  char* getFromPartString() const;

  char* getWherePartString() const;

  char* getOrderByPartString() const;

  char* getWithKeyPartString() const;
  
  //
  // constructors:
  //

  static SQLTerm* createStringTerm(SQLTermMemoryTracker& memoryTracker,
                                   const char* sz);

  static SQLTerm* createConstStringTerm(SQLTermMemoryTracker& memoryTracker,
                                        const char* sz);

private:

  char* getMemberSzValue(int i) const;

private:

  void destroy();
  
  SQLTerm();
  SQLTerm(const SQLTerm&);
  SQLTerm& operator=(const SQLTerm&);


};

inline std::ostream& operator<<(std::ostream& os,const SQLTerm& term)
{
 term.print(os);
 return os;
}

#endif
