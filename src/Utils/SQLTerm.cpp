#ifndef __Utils_SQLTerm_h
#include <Utils/SQLTerm.h>
#endif

/*
 * part of UAKG Query Service
 * (C) GradSoft 2001
 * http://www.gradsoft.com.ua
 * author: Ruslan Shevchenko
 * $Id: SQLTerm.cpp,v 1.9 2001-12-27 09:54:11 yad Exp $
 */

#ifndef assert
#include <assert.h>
#endif
#include <iostream>
using namespace std;

SQLTerm::SQLTerm(SQLTermMemoryTracker& memoryTracker, 
          Type type, 
          Flawor flawor, 
          int degree, 
          SQLTerm** members, 
          char* szValue)
     :memoryTracker_(memoryTracker),
      type_(type),flawor_(flawor),
      degree_(degree),members_p_(members),
      szValue_(szValue)
{}


SQLTerm* SQLTerm::createSQLTerm(SQLTermMemoryTracker& memoryTracker,
                          Type type,
                          Flawor flawor, 
                          int degree, 
                          SQLTerm** members, 
                          char* szValue)
{
 SQLTerm* retval = new SQLTerm(memoryTracker,type,flawor,degree,
                               members,szValue);
 memoryTracker.mark_new(retval);
 return retval;
}

SQLTerm::~SQLTerm()
{ 
   destroy(); 
}

void SQLTerm::destroy(void)
{
 memoryTracker_.mark_deleted(this);
 if (degree_ !=0) {
   assert(members_p_!=NULL);
   for(int i=0; i<degree_; ++i) {
     SQLTerm* member = members_p_[i];
     if (member!=NULL) delete member;  
   }
   delete[] members_p_;
   members_p_ = NULL;
   degree_ = 0;
 }
 if (szValue_ != NULL)  {
   if (type_ != ConstString) {
     delete[] szValue_; 
   }
   szValue_ = NULL;
 }
 type_ = Error;
}

void SQLTerm::print(ostream& os) const
{
  switch(type_) {
    case Error: os << " Error ";
                break;
    case ConstString:
    case StringTerm:
                os << szValue_;
                break;
    case SelectExpr:
                os << "SELECT ";
                if (flawor_ == FlaworSelectOptDistinct) {
                   os << "DISTINCT ";
                }
                if (members_p_[0]!=NULL) {
                   members_p_[0]->print(os);
                }
                if (members_p_[1]!=NULL) {
                   os << " FROM "; 
                   members_p_[1]->print(os);
                   os << " ";
                }
                if (members_p_[2]!=NULL) {
                   os << " ORDER BY ";
                   members_p_[2]->print(os);
                }
                break;
    case SelectOptAll:
                os << "ALL";
                break;
    case SelectOptDistinct:
                os << "DISTINCT";
                break;
    case TableExpr:
                if (members_p_[0]!=NULL) {
                   members_p_[0]->print(os);
                }
                if (members_p_[1]!=NULL) {
                   os << " WHERE ";
                   members_p_[1]->print(os);
                }
                if (members_p_[2]!=NULL) {
                   os << " GROUP BY ";
                   members_p_[2]->print(os);
                }
                if (members_p_[3]!=NULL) {
                   os << " HAVING ";
                   members_p_[3]->print(os);
                }
                break;
    default: 
             os << "UNKNOWN TYPE";
             break; 
  }
}

SQLTerm*  SQLTerm::createStringTerm(SQLTermMemoryTracker& memoryTracker,
                                    const char* sz)
{
  char* newSz = new char[strlen(sz)+1];
  strcpy(newSz,sz);
  return createSQLTerm(memoryTracker,StringTerm,UndefinedFlawor,0,NULL,newSz);
}

SQLTerm*  SQLTerm::createConstStringTerm(SQLTermMemoryTracker& memoryTracker,
                                         const char* sz)
{
  SQLTerm* newTerm = createSQLTerm(memoryTracker,
                                   ConstString,UndefinedFlawor,0,NULL,
                                   const_cast<char*>(sz));
  return newTerm;
}



/**
 *return SelectionList in SELECT query as string 
 *memory for returning value is allocated in 
 *        function by CORBA allocator
 *@precondition SQLTerm must be select term
 *@precondition SQLTerm must be build correctly
 *@returns SelectionsInString
 **/
char* SQLTerm::getSelectionListString() const
{
 if (!isSelect()) throw TypeMismatch();
 return getMemberSzValue(0);
}

/**
 *return FromPart (i. e. table_ref_commalist ) as string 
 * for Select or TableExpr term.
 *memory for returning value is allocated in 
 *        function by CORBA allocator
 *@precondition SQLTerm must be select term 
 *@precondition SQLTerm must be build correctly
 *@returns list of table refs as string.
 **/
char* SQLTerm::getFromPartString() const
{
 if (!isSelect()) throw TypeMismatch();
 return CORBA::string_dup((members_p_[1]==NULL) ? "" : 
                    members_p_[1]->members_p_[0]->szValue_  );
}

char* SQLTerm::getWherePartString() const
{
 if (isSelect()) {
    return members_p_[1]->getWherePartString();
 }else if(isTableExpr()){
    return getMemberSzValue(1);
 }else{
    throw TypeMismatch(); 
    return NULL; // make compiler happy
 }
}

char* SQLTerm::getOrderByPartString() const 
{
  if (!isSelect()) throw TypeMismatch();
  return getMemberSzValue(2);
}

char* SQLTerm::getWithKeyPartString() const 
{
 if (!isSelect()) throw TypeMismatch();
 return getMemberSzValue(3); 
}
  
char* SQLTerm::getMemberSzValue(int i) const
{
  return CORBA::string_dup((members_p_[i]==NULL) ? "" : 
                    members_p_[i]->szValue_  );
}



void SQLTermMemoryTracker::clean()
{
 while(ptrs_.size()!=0) {
     SQLTerm* x = *(ptrs_.begin());
     if (x!=NULL)
         delete x;
 }
}

