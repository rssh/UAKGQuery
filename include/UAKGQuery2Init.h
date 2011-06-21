#ifndef __UAKGQuery2Init_h
#define __UAKGQuery2Init_h

#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

/*
 * part of UAKGQuery2Service
 * (C) GradSoft , Kiev, Ukraine 1998-2002
 * http://www.gradsoft.com.ua/
 * $Id: UAKGQuery2Init.h,v 1.2 2008-05-29 07:22:26 rssh Exp $
 */

void initUAKGQuery2(CORBA::ORB_ptr orb);

void initUAKGQuery2(CORBA::ORB_ptr orb, GradSoft::Logger& logger);

#endif
