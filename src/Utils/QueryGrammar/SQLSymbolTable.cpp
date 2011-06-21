#ifndef __Utils_QueryGrammar_SQLSymbolTable_h
#include <Utils/QueryGrammar/SQLSymbolTable.h>
#endif

/**
 * part of GradSoft UAKGQuery
 * http://www.gradsoft.com.ua
 * (C) GradSoft, 2001
 * $Id: SQLSymbolTable.cpp,v 1.7 2001-04-04 13:10:29 rssh Exp $
 **/

#include <Utils/QueryGrammar/y.tab.h>
#include <iostream>

SQLSymbolTable::StorageType&
SQLSymbolTable::getStorage()
{
    static SQLSymbolTable::StorageType storage_;
    return storage_;
}
bool SQLSymbolTable::initialized_ = false;

SQLSymbolTable::SQLSymbolTable()
{
  if (!initialized_) init();
}

int SQLSymbolTable::checkSymbol(const char* symbol)
{
  StorageIteratorType it = getStorage().find(symbol);
  return (it==getStorage().end()) ? -1 : (*it).second;
}


void  SQLSymbolTable::init()
{
 if (!initialized_) {
  getStorage()["OR"]=OR;
  getStorage()["AND"]=AND;
  getStorage()["NOT"]=NOT;
  getStorage()["ALL"]=ALL;
  getStorage()["ANY"]=ANY;
  getStorage()["BETWEEN"]=BETWEEN;
  getStorage()["BY"]=BY;
  getStorage()["DISTINCT"]=DISTINCT;
  getStorage()["ESCAPE"]=ESCAPE;
  getStorage()["EXISTS"]=EXISTS;
  getStorage()["FROM"]=FROM;
  getStorage()["GROUP"]=GROUP;
  getStorage()["HAVING"]=HAVING;
  getStorage()["ORDER"]=ORDER;
  getStorage()["SELECT"]=SELECT;
  getStorage()["SOME"]=SOME;
  getStorage()["WHERE"]=WHERE;
  getStorage()["IN"]=IN;
  getStorage()["INDICATOR"]=INDICATOR;
  getStorage()["IS"]=IS;
  getStorage()["USER"]=USER;
  getStorage()["ASC"]=ASC;
  getStorage()["DESC"]=DESC;
  getStorage()["WITH"]=WITH;
  getStorage()["KEY"]=KEY;
  getStorage()["NULL"]=DBNULL;
  getStorage()["LIKE"]=LIKE;
  initialized_ = true;
 }
}


