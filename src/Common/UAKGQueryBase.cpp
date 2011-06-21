#include <Common/UAKGQueryBase.h>
#include <UAKGQuerySingleton.h>
#include <Common/UAKGQueryBase.h>
#include <Common/UAKGQueryUtils.h>
#include <Common/UAKGQueryManagerBase.h>
#include <iostream>

using namespace std;
using namespace GradSoft;
using namespace UAKGQuery2;

UAKGQueryBase::UAKGQueryBase(const char* queryText,
                             UAKGQueryManagerBase* qm_p,
                             const char* flags )
     :qm_p_(qm_p),
      szFlags_(new char[strlen(flags)+1]),
      flags_(&qm_p->flags_),
      resultBindSet_p_(NULL),
      paramsBindSet_p_(NULL),
      prepared_(false),
      executed_(false),
      retrieved_(false),
      closed_(false),
      status_(incomplete),
      selectFlg_(false),
      retrievedNRows_(0),
      fetchedNRows_(0),
      resultRowIndex_(0),
      resultChunkSize_(50),
      error_(false)
{
 qm_p_->incNQueries();
 qm_p_->_add_ref();
 CORBA::String_var sql = qm_p->normalize_sql(queryText);
 queryText_ = new char[strlen(sql)+1];
 strcpy(queryText_,sql);
 strcpy(szFlags_,flags);
 try {
   flags_.parse(szFlags_);
 }catch(const StringFlagSet::ParseError& ex){
   setQueryProcessingError(100,ex.why,false);  
 }
 int x =flags_.get_int("ChunkSize");
 if (x) resultChunkSize_ = x;
#ifdef CORBA_HAVE_OTS
 current_ = qm_p_->transCurrent();
 coordinator_ = CosTransactions::Coordinator::_nil();
 transactionName_ = CORBA::string_dup("");
 memset(&transactionXID_,0,sizeof(XID));
#endif
}

UAKGQueryBase::~UAKGQueryBase()
{
 qm_p_->decNQueries();
 try {
   close();
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   Logger::ErrorLocker guard(errors());
   if (getDebugLevel() > 5) {
     getLogger().errors()<<"["<<__FILE__<<":"<<"]";
   }
   getLogger().errors() << "Error during query destroy:" << ex.why << std::endl;
 }
 qm_p_->_remove_ref();
}
   
void UAKGQueryBase::close()
{
 if (!closed_) {
   delete [] szFlags_; szFlags_ = NULL;
   if (queryText_) {  delete[] queryText_; queryText_=NULL; }
   if (resultBindSet_p_) { delete resultBindSet_p_; resultBindSet_p_=NULL; }
   if (paramsBindSet_p_) { delete paramsBindSet_p_; paramsBindSet_p_=NULL; }
 }
 closed_ = true;
}

const char* UAKGQueryBase::getDBName_p() const
{
  return qm_p_->getDBName_p();
}

QueryManager_ptr UAKGQueryBase::query_mgr()
	 throw (CORBA::SystemException)
{
 return qm_p_->_this();
}

PortableServer::POA_ptr  UAKGQueryBase::getPOA_()
{
 return qm_p_->getQueriesPOA_();
}


PortableServer::POA_ptr  UAKGQueryBase::_default_POA()
{
 return PortableServer::POA::_duplicate(qm_p_->getQueriesPOA_());
}


void UAKGQueryBase::prepare_local( RecordSet* paramsDescription )
{
 qm_p_->touch();
 checkError();
 try {
   selectFlg_=false;
   setParamsBindDescription(paramsDescription);
   paramsBindSet_p_->allocate();
   prepare_internal();
   prepared_ = true;
 }catch(const DBBindException& ex){
   throwQueryProcessingError(400, ex.what(),false);
 }
}
 

void UAKGQueryBase::pre_execute(RecordSet* rs)
{
 if (!prepared_) {
   prepare_local(rs);
 } 
 executed_=false;
 closed_=false;
 retrieved_=false;
 status_ = incomplete;
 retrievedNRows_=0;
 fetchedNRows_=0;
 resultRowIndex_=0;
 error_=false;
 if (logSQL()) {
   Logger::InfoLocker guard(infos());
   infos() << "SQL:" << queryText_ << endl;
 }
}



void  UAKGQueryBase::execute(RecordSet* rs)
               throw(UAKGQuery2::QueryProcessingError,
                     CORBA::SystemException)

{
 execute_local(rs);
}

void  UAKGQueryBase::execute_local(RecordSet* rs)
{ 
 qm_p_->touch();
 try {
  pre_execute(rs);
  if (!prepared_) {
     throwQueryProcessingError(2,"Query not prepared",false);
  }
  CORBA::ULong nRecords = rs->getNRows();
  paramsBindSet_p_->setNRows(nRecords);
  if (nRecords!=0) {
   try {
    CORBA::ULong nColumns = rs->getNColumns();
    if (nColumns==0) {
      paramsBindSet_p_->setNRows(0);
    }else{
     for(CORBA::ULong iRecord = 0; iRecord< nRecords; ++iRecord){
      for(CORBA::ULong iField = 0; iField< nColumns; ++iField){
        Field fld;
        getField(rs,iRecord,iField,fld);
        paramsBindSet_p_->setField(iRecord,iField, fld, *this);
      }
     } 
    }
   }catch(const UAKGQuery2::InvalidFieldType&){
        throwQueryProcessingError(4,"parameter description does not match parameter",false);
   }catch(const InvalidDate&){
        throwQueryProcessingError(403,"invalid date field is passed to query",false);
   }
  }
  execute_internal();
 }catch(const DBBindException& ex){
   throwQueryProcessingError(400,ex.what(),false);
 }
}
 

void  UAKGQueryBase::execute_e()
             throw(UAKGQuery2::QueryProcessingError,
                   CORBA::SystemException)
{
  RecordSet_var rs = new RecordSetImpl;
  execute_local(rs);
}

#if defined(MICO) && MICO_BIN_VERSION < 0x020306
void  UAKGQueryBase::execute_inout(UAKGQuery2::RecordSet* rs)
#else
void  UAKGQueryBase::execute_inout(UAKGQuery2::RecordSet*& rs)
#endif
               throw(UAKGQuery2::QueryProcessingError,
                     CORBA::SystemException)
{
 execute(rs);
 UAKGQuery2::RecordSet_var rs1=get_all_parameters(); 


 rs->assign(rs1);

}

RecordSet* UAKGQueryBase::duplicateResultBindStruct()
{
  RecordSet_var recordSet = new RecordSetImpl;
  RecordSet_var resultRecordSet = getResultBindDescription();
  CORBA::ULong col;

  for( CORBA::ULong i = 0; i < resultRecordSet->getNColumns(); i++) {
      col = recordSet->getNColumns();
      CORBA::String_var name = resultRecordSet->getColumnNameByIndex(i);
      recordSet->addColumn( name.in(),
                            resultRecordSet->getFieldTypeAt(i)
                          );
      if ( resultRecordSet->haveFieldSizeAt(i) )
         recordSet->setFieldSizeAt(i,resultRecordSet->getFieldSizeAt(i));

      if (  resultRecordSet->haveNumericFieldPropertiesAt(i) )
         recordSet->setNumericFieldPropertiesAt( i,
                    resultRecordSet->getNumericPrecisionAt(i),
                    resultRecordSet->getNumericScaleAt(i)
                                               );
  }
  
  return  recordSet._retn();
}


RecordSet* UAKGQueryBase::fetch(CORBA::ULong numberOfRecords, 
                                CORBA::Boolean& b)
                 throw(UAKGQuery2::QueryProcessingError,
                       CORBA::SystemException)
{
  qm_p_->touch();
  RecordSet_var recordSet = duplicateResultBindStruct();

  if (status_ != complete ) {
    CORBA::ULong nRecords = numberOfRecords;
    bool fetchAll = (numberOfRecords==0);
    b=1;
    while(b && (fetchAll || numberOfRecords!=0)) {
     if (fetchAll) numberOfRecords = resultChunkSize_;
     fetch_records_incremental(numberOfRecords, b, recordSet);
    }
    if (!b) status_ = complete;
  }else{
    b=0;
  }
  return  recordSet._retn();
}
  
CORBA::ULong UAKGQueryBase::skip(CORBA::ULong numberOfRecords, CORBA::Boolean& b)
                    throw(UAKGQuery2::QueryProcessingError,
                          CORBA::SystemException)
{
  qm_p_->touch();
  CORBA::ULong retval = 0;
  if (status_ != complete ) {
    CORBA::ULong nRecords = numberOfRecords;
    bool fetchAll = (numberOfRecords==0);
    b=1;
    while(b && (fetchAll || numberOfRecords!=0)) {
      if (fetchAll) numberOfRecords = resultChunkSize_;
      skip_incremental(numberOfRecords, b, retval);
    }
    if (!b) status_ = complete;
  }else{
    b=0;
  }
  return retval;
}


void UAKGQueryBase::fetch_records_incremental(CORBA::ULong& numberOfRecords,
                                              CORBA::Boolean& b,
                                              RecordSet* recordSet)
{
 try {
  fetch_retrieved_records(numberOfRecords, recordSet);
  b=retrieve_internal();
  fetch_retrieved_records(numberOfRecords, recordSet);
  b |= (fetchedNRows_ < retrievedNRows_ );
 }catch(DBFormatHandlerNotSet){
    throwQueryProcessingError(100,"DB Format is not set for this driver",false);
 }
}

void UAKGQueryBase::skip_incremental(CORBA::ULong& numberOfRecords,
                                     CORBA::Boolean& b,
                                     CORBA::ULong& actualNumberOfRecords)
{
 skip_retrieved(numberOfRecords, actualNumberOfRecords);
 b=retrieve_internal();
 skip_retrieved(numberOfRecords, actualNumberOfRecords);
 b |= (fetchedNRows_ < retrievedNRows_ );
}

void UAKGQueryBase::fetch_retrieved_records(CORBA::ULong& numberOfRecords,
                                            RecordSet* recordSet)
{
  while(numberOfRecords > 0 &&  fetchRetrievedRowRec(recordSet)) {
     --numberOfRecords;
  }
}

void UAKGQueryBase::skip_retrieved(CORBA::ULong& numberOfRecords,
                                     CORBA::ULong& actualNumberOfRecords)
{
  while(numberOfRecords > 0 && retrievedNRows_ > fetchedNRows_ ) {
         ++actualNumberOfRecords;
         incFetchIndex();
         --numberOfRecords;
  }
}

bool UAKGQueryBase::fetchRetrievedRowRec(RecordSet* recordSet)
{
  if (retrievedNRows_ == fetchedNRows_) return false;

  fetchBindRowRec(resultBindSet_p_,resultRowIndex_,recordSet);

  incFetchIndex();
  return true;
}

void UAKGQueryBase::fetchBindRowRec(DBBindSet* bindSet_p, 
                                    CORBA::ULong bindSetRowIndex,
                                    RecordSet* recordSet)
{                   
  CORBA::ULong nColumns = bindSet_p->getNColumns();
  Field fd;
  CORBA::ULong recordSetIndex=recordSet->getNRows();
  recordSet->addRow();
  for(CORBA::ULong i=0; i<nColumns; ++i) {
       try{
           bindSet_p->getField(bindSetRowIndex,i,fd,*this);
           setField(recordSet, recordSetIndex, i, fd);
       }catch(const UAKGQuery2::InvalidFieldType& ex){
           throwQueryProcessingError(400,"internal: bind and recordSet description does not match in UAKGQueryBase::fetchBindRowRec",false);
       }catch(const UAKGQuery2::InvalidRow& ex){
           throwQueryProcessingError(401,"internal: invalid row index in UAKGQueryBase::fetchBindRowRec",false);
       }catch(const UAKGQuery2::InvalidColumn& ex){
           throwQueryProcessingError(402,"internal: invalid column index in UAKGQueryBase::fetchBindRowRec",false);
       }catch(const InvalidDate& ex){
           throwQueryProcessingError(403,"invalid date passed to Query",false);
       }
  }
}

void UAKGQueryBase::incFetchIndex()
{
  ++fetchedNRows_;
  ++resultRowIndex_;
  if (resultRowIndex_ >= resultBindSet_p_->getNRows()) {
      resultRowIndex_ = 0;
  }
}


void UAKGQueryBase::setField( RecordSet* rs, CORBA::ULong row, CORBA::ULong col, const Field& ft )
{
    switch ( ft.getType() ) {
      case TypeNull:
            rs->setNullAt(row,col);
            break;
      case TypeBoolean:
            rs->setBooleanAt(row,col,ft.getBoolean());
            break;
      case TypeChar:
      case TypeCharacter:
            rs->setCharAt(row,col,ft.getChar());
            break;
      case TypeOctet:
            rs->setOctetAt(row,col,ft.getOctet());
            break;
      case TypeShort:
      case TypeSmallInt:
            rs->setShortAt(row,col,ft.getShort());
            break;
      case TypeUShort:
            rs->setUShortAt(row,col,ft.getUShort());
            break;
      case TypeLong:
      case TypeInteger:
            rs->setLongAt(row,col,ft.getLong());
            break;
      case TypeULong:
            rs->setULongAt(row,col,ft.getULong());
            break;
      case TypeFloat:
      case TypeReal:
            rs->setFloatAt(row,col,ft.getFloat());
            break;
      case TypeDouble:
      case TypeDoublePrecision:
            rs->setDoubleAt(row,col,ft.getDouble());
            break;
      case TypeString:
            rs->setStringWithSizeAt(row,col,ft.getString_());
            break;
      case TypeNumeric:
      case TypeDecimal:
            rs->setNumericAt(row,col,ft.getNumeric_());
            break;
      case TypeDateTime:
            rs->setDateTimeAt(row,col,ft.getDateTime());
            break;
      case TypeRaw:
        {
             const OctSeq* ptr = ft.getRaw_();
                 rs->setRawAt(row,col,*ptr);
        }
            break;
      case TypeBlob:
            rs->setBlobAt(row,col,ft.getBlob_());
            break;
      case TypeClob:
            rs->setClobAt(row,col,ft.getClob_());
            break;
      case TypeWString:
            rs->setWStringWithSizeAt(row,col,ft.getWString_());
            break;
      case TypeWclob:
            rs->setWclobAt(row,col,ft.getWclob_());
            break;
     }
}




void UAKGQueryBase::getField(RecordSet* rs, 
                             CORBA::ULong row, 
                             CORBA::ULong col, 
                             Field& fld)
{
    fld.setNull();
    if ( rs->isNullAt(row,col) ) return;
    switch ( rs->getFieldTypeAt(col) ) {
      case TypeBoolean:
            fld.setBoolean(rs->getBooleanAt(row,col));
            break;
      case TypeChar:
      case TypeCharacter:
            fld.setChar(rs->getCharAt(row,col));
            break;
      case TypeOctet:
            fld.setOctet(rs->getOctetAt(row,col));
            break;
      case TypeShort:
      case TypeSmallInt:
        fld.setShort(rs->getShortAt(row,col));
            break;
      case TypeUShort:
        fld.setUShort(rs->getUShortAt(row,col));
            break;
      case TypeLong:
      case TypeInteger:
            fld.setLong(rs->getLongAt(row,col));
            break;
      case TypeULong:
            fld.setULong(rs->getULongAt(row,col));
            break;
      case TypeFloat:
      case TypeReal:
            fld.setFloat(rs->getFloatAt(row,col));
            break;
      case TypeDouble:
      case TypeDoublePrecision:
            fld.setDouble(rs->getDoubleAt(row,col));
            break;
      case TypeString:
            {
             CORBA::String_var s = rs->getStringAt(row,col);
             fld.setString(s);
            }
            break;
      case TypeNumeric:
      case TypeDecimal:           
            {
             Numeric_var num = rs->getNumericAt(row,col);
             fld.setNumeric(num);
            }
            break;
      case TypeDateTime:
            fld.setDateTime(rs->getDateTimeAt(row,col));
            break;
      case TypeRaw:
            {
             OctSeq_var oct = rs->getRawAt(row,col);
             fld.setRaw(oct);
            }
            break;
      case TypeBlob:
        {
        Blob_var bl = rs->getBlobAt(row,col);
        fld.setBlob(bl);
        }
        break;
      case TypeClob:
            {
                Clob_var cl = rs->getClobAt(row,col);
                fld.setClob(cl);
            }
            break;
      case TypeWString:
        {
                CORBA::WString_var wstr = rs->getWStringAt(row,col);
        fld.setWString(wstr.in());
        }
            break;
      case TypeWclob:
            {
                Wclob_var wcl = rs->getWclobAt(row,col);
                fld.setWclob(wcl);
            }
            break;
     }
}

RecordSet* UAKGQueryBase::get_all_parameters()
               throw(UAKGQuery2::QueryProcessingError,
                     CORBA::SystemException)
{                           
 checkExecuted();
 RecordSet_var retval = new RecordSetImpl;

 RecordSet_var paramsDescription = getParamsBindDescription();


 for(CORBA::ULong i=0; i<paramsDescription->getNColumns(); ++i){
    CORBA::String_var name = paramsDescription->getColumnNameByIndex(i);
    retval->addColumn( name.in(),
                          paramsDescription->getFieldTypeAt(i)
                        );
    if ( paramsDescription->haveFieldSizeAt(i) ){
       retval->setFieldSizeAt(i,paramsDescription->getFieldSizeAt(i));
    }

    if (  paramsDescription->haveNumericFieldPropertiesAt(i) ) {
       retval->setNumericFieldPropertiesAt(
                   i,
                   paramsDescription->getNumericPrecisionAt(i),
                   paramsDescription->getNumericScaleAt(i)
                                             );
    }
 }  

 for(CORBA::ULong row=0; row<paramsBindSet_p_->getNRows(); ++row){
   fetchBindRowRec(paramsBindSet_p_,row,retval);
 }  

 return retval._retn();
}

RecordSet* UAKGQueryBase::get_parameters(const StringSeq& fieldNames)
               throw(UAKGQuery2::QueryProcessingError,
                     UAKGQuery2::InvalidParameterName,
                     CORBA::SystemException)
{
 checkExecuted();
 RecordSet_var retval = new RecordSetImpl();
 Field fd;
 FieldDescription fdesc;
 CORBA::ULong col;
 try {
   // create RecordSet corresponding to fieldNames.
   for(CORBA::ULong i=0; i<fieldNames.length(); ++i) {
      CORBA::ULong j = paramsBindSet_p_->getFieldIndex(fieldNames[i]);
      fdesc = paramsBindSet_p_->getFieldDescription(j);
      retval->addColumn(fdesc.name,fdesc.type);
      if (fdesc.haveFieldSize()) {
         retval->setFieldSizeAt(i,fdesc.size);
      }
      if ((fdesc.type == TypeNumeric) || (fdesc.type == TypeDecimal)) {
         retval->setNumericFieldPropertiesAt(i,fdesc.precision,fdesc.scale);
      }
   }
   for(CORBA::ULong row=0; row<paramsBindSet_p_->getNRows(); ++row){
    retval->addRow();
    for(CORBA::ULong i=0; i<fieldNames.length(); ++i) {
      CORBA::ULong j = paramsBindSet_p_->getFieldIndex(fieldNames[i]);
      paramsBindSet_p_->getField(row,j,fd,*this);
      setField(retval,row,i,fd);
    }
   }
 }catch(InvalidFieldName){
     throw InvalidParameterName();  
 }catch(InvalidDate){
     throwQueryProcessingError(403,"invalid date field is passed to query",false);
 }
 return retval._retn();
}

RecordSet* UAKGQueryBase::get_parameter(const char* fieldName)
        throw(UAKGQuery2::QueryProcessingError,
              UAKGQuery2::InvalidParameterName,
              CORBA::SystemException)
{
 CORBA::ULong nameIndex;
 try {
   nameIndex=paramsBindSet_p_->getFieldIndex(fieldName);
 }catch(InvalidFieldName){
   throw InvalidParameterName();  
 }

 RecordSet_var retval = new RecordSetImpl();
 RecordSet_var paramsDescription = getParamsBindDescription();

 retval->addColumn(fieldName,paramsDescription->getFieldTypeAt(nameIndex));
 if (paramsDescription->haveFieldSizeAt(nameIndex)) {
   retval->setFieldSizeAt(0,paramsDescription->getFieldSizeAt(nameIndex));
 }
 if (paramsDescription->haveNumericFieldPropertiesAt(nameIndex)) {
   retval->setNumericFieldPropertiesAt(0,
                   paramsDescription->getNumericPrecisionAt(nameIndex),
                   paramsDescription->getNumericScaleAt(nameIndex)
                                       );
 }
 Field fd;
 for(CORBA::ULong row=0; row<paramsBindSet_p_->getNRows(); ++row) {
  retval->addRow();
  paramsBindSet_p_->getField(row,nameIndex,fd,*this);
  setField(retval,row,0,fd);
 }
 return retval._retn();
}



void UAKGQueryBase::destroy() throw (CORBA::SystemException)
{
  PortableServer::ObjectId_var oid = getPOA_()->servant_to_id(this);
  getPOA_()->deactivate_object(oid.in());
}


/**
 * save error information, and mark error in this query, but
 * do not throw.
 *  Usually, when we don't want to throw error from constructor
 * errorCode - error code of your error.
 * msg - message.
 * dbFlg - true, if in underlaying DB we have additional error information,
 *         false  if this is error in CosQuery itself.
 **/
void UAKGQueryBase::setQueryProcessingError(CORBA::ULong errorCode,
                                            const char* msg,
                                            bool dbFlg)
{
 queryError_.code = errorCode;
 if (dbFlg) fillDBError();
 else {
   queryError_.why=CORBA::string_dup(msg);
   queryError_.dbCode=0;
 }
 error_ = true;
 {
  Logger::ErrorLocker guard(errors());
  if (getDebugLevel()>5) {
     errors()<<"["<<__FILE__<<":"<<__LINE__<<"]";
  }
  errors() << "QueryProcessingError: " <<
               errorCode << "," << queryError_.dbCode << "\n";
  errors() << "why: " <<  queryError_.why << "\n";
  //errors() << "msg: " <<  msg << "\n";
  if (queryText_!=NULL)
     errors() << "sql: " <<  queryText_  << endl;
 }
}


Blob* UAKGQueryBase::create_blob()
          throw (CORBA::SystemException,
                 UAKGQuery2::QueryProcessingError)
{
    checkError();
    qm_p_->touch();
    PortableServer::ObjectId_var oid=createBlobIdForWriting();
    CORBA::Object_var obj;
    try {
        obj = qm_p_->getBlobWritingPOA_()->create_reference_with_id(oid
                           , "IDL:gradsoft.kiev.ua/UAKGQuery2/Blob:1.0");
    } catch (const PortableServer::POA::WrongPolicy&) {
      {
       Logger::ErrorLocker l(errors());
       if (getDebugLevel()>5) {
          errors()<<"["<<__FILE__<<":"<<__LINE__<<"]";
       }
       errors() << "Error on create_blob: POA::WrongPolicy" << endl;
      }
      throwQueryProcessingError(100,
                               "Error on create_blob: POA::WrongPolicy",false);
    } catch (const CORBA::SystemException& ex) {
        {
          Logger::ErrorLocker l(errors());
          if (getDebugLevel()>5) {
              errors()<<"["<<__FILE__<<":"<<__LINE__<<"]";
          }
          errors() << "Error on create_blob: " << ex << endl;
        }
        throw;
    }
    Blob_var bl = Blob::_narrow(obj); // chek this ML for other ORB.
    return bl._retn();
}

Clob* UAKGQueryBase::create_clob()
          throw (CORBA::SystemException,
                 UAKGQuery2::QueryProcessingError)
{
    checkError();
    qm_p_->touch();
   // TODO: check for nil and system exception during 
    PortableServer::ObjectId_var oid=createClobIdForWriting();
    CORBA::Object_var obj = getUAKGQueryManagerBasePtr()->getClobWritingPOA_()->create_reference_with_id(oid
            , "IDL:gradsoft.kiev.ua/UAKGQuery2/Clob:1.0");
    Clob_var cl = Clob::_narrow(obj);
    return cl._retn();
}

Wclob* UAKGQueryBase::create_wclob()
          throw (CORBA::SystemException,
                 UAKGQuery2::QueryProcessingError)
{
  checkError();
  qm_p_->touch();
 // TODO: check for nil and system exception during 
  PortableServer::ObjectId_var oid=createWclobIdForWriting();
  CORBA::Object_var obj = getUAKGQueryManagerBasePtr()->getWclobWritingPOA_()->create_reference_with_id(oid
            , "IDL:gradsoft.kiev.ua/UAKGQuery2/Wclob:1.0");
  Wclob_var wcl = Wclob::_narrow(obj);
  return wcl._retn();
}


