#ifndef __RecordSetFactory_h
#include <RecordSetFactory.h>
#endif

using namespace std;
using namespace UAKGQuery2;

RecordSet_init* RecordSetFactory::factory_;

void RecordSetFactory::init(CORBA::ORB_ptr orb)
{
 CORBA::ValueFactory vf=orb->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
 factory_=RecordSet_init::_downcast(vf);
 if (factory_==NULL) {
    cerr <<  "Internal error: can't find valuetype factory for RecordSet" 
         << endl;
 }
}

