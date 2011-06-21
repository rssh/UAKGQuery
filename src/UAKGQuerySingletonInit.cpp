/*
 * part of GradSoft UAKGQuery
 * (C) GradSoft, Kiev, Ukraine, 2001, 2002
 * $Id: UAKGQuerySingletonInit.cpp,v 1.6 2008-05-29 07:22:26 rssh Exp $
 */

#include <importUAKGQuery2.h>
#include <UAKGQuerySingleton.h>
#include <RecordSet/RecordSetImpl.h>

#include <GradSoft/Logger.h>

/*
 * actually this is sinleton for server and for client also.
 * (i. e. this file must be in client library)
 *
 */
UAKGQuerySingleton UAKGQuerySingleton::singleton_;

void  initUAKGQuery2(CORBA::ORB_ptr orb)
{
 UAKGQuerySingleton::init(orb);

 // Register ValueType.
 CORBA::ValueFactoryBase_var factory = new UAKGQuery2::RecordSetFactoryImpl();
 CORBA::ValueFactoryBase_var oldFactory =
        orb -> register_value_factory("IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0", factory);

 CORBA::ValueFactoryBase_var numfactory = new UAKGQuery2::NumericFactoryImpl();
 CORBA::ValueFactoryBase_var oldNumFactory =
        orb -> register_value_factory("IDL:gradsoft.kiev.ua/UAKGQuery2/Numeric:1.0", numfactory);
}

void initUAKGQuery2(CORBA::ORB_ptr orb, GradSoft::Logger& logger)
{
 initUAKGQuery2(orb);
 UAKGQuerySingleton::setLogger(logger);
}
