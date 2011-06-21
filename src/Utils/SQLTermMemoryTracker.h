#ifndef __Utils_SQLTermMemoryTracker_h
#define __Utils_SQLTermMemoryTracker_h
/*
 * part of UAKGQueryService
 * (C) GradSoft, 2001
 * $Id: SQLTermMemoryTracker.h,v 1.5 2001-12-27 09:54:12 yad Exp $
 */

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#include <set>


class SQLTerm;

struct SQLTermPtrLess: public std::binary_function<SQLTerm*, SQLTerm*, bool>
{
  bool operator()(SQLTerm* x, SQLTerm* y) const
  {
    return ((unsigned long)x - (unsigned long)y) < 0;
  }
};

class SQLTermMemoryTracker
{
 std::set<SQLTerm*, SQLTermPtrLess> ptrs_;
public:

  void mark_new(SQLTerm* x)
  {
     ptrs_.insert(x);
  }

  void mark_deleted(SQLTerm* x)
  {
     ptrs_.erase(x);
  }

  void clean();

  ~SQLTermMemoryTracker()
  {
    clean();
  }   

};

#endif
