#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>

#include <iostream>

void printRecordSet(std::ostream& out, const UAKGQuery2::RecordSet* recordSet);
void printNumeric(std::ostream& out, const UAKGQuery2::Numeric* num);
void printDateTime(std::ostream& out, const UAKGQuery2::DateTime& dec);
void printRaw(std::ostream& out,const UAKGQuery2::OctSeq* octSeq);
