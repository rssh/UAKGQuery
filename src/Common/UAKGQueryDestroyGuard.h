#ifndef __Common_UAKGQueryDestroyGuard_h
#define __Common_UAKGQueryDestroyGuard_h

/**
 * part of UAKGQueryService.
 * (C) Grad-Soft Ltd, 1998-2003
 * $Id: UAKGQueryDestroyGuard.h,v 1.1 2003-12-05 20:59:03 rssh Exp $
 **/

#ifndef __CosQueryPostConfig_h
#include <CosQueryPostConfig.h>
#endif

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
#endif

class UAKGQueryDestroyGuard
{
 UAKGQueryBase* qi_;
public:  
  UAKGQueryDestroyGuard(UAKGQueryBase* qi)
	  :qi_(qi)
  {}
  ~UAKGQueryDestroyGuard()
  { qi_->_remove_ref(); }
};

#endif
