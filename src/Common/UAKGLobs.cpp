
/*
 * part of Grad-Soft UAKGQuery
 * (C) Grad-Soft (tm), Kiev, Ukraine.
 * 1998 - 2003
 * $Id: UAKGLobs.cpp,v 1.2 2003-07-10 15:02:39 rssh Exp $
 */

#ifndef __Common_UAKGLobs_h
#include <Common/UAKGLobs.h>
#endif

#ifndef __UAKGQuerySingleton_h
#include <UAKGQuerySingleton.h>
#endif


PortableServer::ObjectId* UAKGLobBase::currentId()
{
 return UAKGQuerySingleton::getCurrent_ptr()->get_object_id();
}


