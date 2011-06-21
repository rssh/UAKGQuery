#ifndef __Common_DBBind_h
#include <Common/DBBind.h>
#endif

#include <strstream>
using namespace std;

#ifndef assert
#include <assert.h>
#endif

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
#endif

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

using namespace UAKGQuery2;


DBBindDescription::DBBindDescription(RecordSet* rs)
{
   assert(rs!=NULL);
   rs->_add_ref();
   recordSet_ = rs;
}

DBBindDescription::~DBBindDescription()
{
 recordSet_->_remove_ref();
}

UAKGQuery2::FieldDescription
DBBindDescription::getFieldDescription(CORBA::ULong index)
{
    CORBA::UShort precision = 0, scale = 0, size = 0;
    UAKGQuery2::FieldType type;

    assert( (recordSet_ != NULL) && (index < recordSet_->getNColumns()) );

    type = recordSet_->getFieldTypeAt(index);

    if ( recordSet_->haveFieldSizeAt(index) ) {
    size = recordSet_->getFieldSizeAt(index);
    }
    if ( recordSet_->haveNumericFieldPropertiesAt(index) ) {
    precision = recordSet_->getNumericPrecisionAt(index);
        scale = recordSet_-> getNumericScaleAt(index); 
    }
    CORBA::String_var strname = recordSet_->getColumnNameByIndex(index);
    if  ( type == UAKGQuery2::TypeDateTime ) size = sizeof(DateTime);

    return ( FieldDescription( strname.in(), type, size, precision, scale ) );
}

UAKGQuery2::FieldDescription 
DBBindDescription::getFieldDescription(const char* name)
{
    CORBA::UShort precision = 0, scale = 0, size = 0;
    CORBA::ULong  index = recordSet_->getColumnIndexByName(name);
    CORBA::String_var strname = recordSet_->getColumnNameByIndex(index);
    UAKGQuery2::FieldType type;

    assert( (recordSet_ != NULL) && (index < recordSet_->getNColumns()) );

    type = recordSet_->getFieldTypeAt(index);

    if ( recordSet_->haveFieldSizeAt(index) ) {
    size = recordSet_->getFieldSizeAt(index);
    }
    if ( recordSet_->haveNumericFieldPropertiesAt(index) ) {
    precision = recordSet_->getNumericPrecisionAt(index);
        scale = recordSet_-> getNumericScaleAt(index); 
    }
    if  ( type == UAKGQuery2::TypeDateTime ) size = sizeof(DateTime);
    return ( FieldDescription( strname.in(), type, size, precision, scale ) );
}

CORBA::ULong DBBindDescription::getFieldIndex(const char* name)
{
    return recordSet_->getColumnIndexByName( name );
}

void  
DBBindDescription::appendFieldDescription(UAKGQuery2::FieldDescription& fd)
{
    unsigned int nextColumn =  recordSet_->getNColumns();
    recordSet_->addColumn( fd.name, fd.type );
    if ( recordSet_->haveFieldSizeAt( nextColumn ) )
        recordSet_->setFieldSizeAt(nextColumn , fd.size);
    if ( recordSet_->haveNumericFieldPropertiesAt( nextColumn ) ) 
    recordSet_->setNumericFieldPropertiesAt( nextColumn,
                           fd.precision,
                           fd.scale
                         );
}


DBBindSet::DBBindSet(RecordSet* rd, GradSoft::Logger& logger, CORBA::ULong nRows ) :
    dbBindDescription_(rd),
    allocated_(false),
    bindAdresses_(dbBindDescription_.length()),
    nullIndicators_(dbBindDescription_.length()),
    flens_(dbBindDescription_.length()),
    nRows_(nRows),
    dbFormatHandler_p_(NULL),
	logger_(logger)
{
    for(CORBA::ULong i=0; i<getNColumns(); ++i)
    {
        bindAdresses_[i] = NULL;
        nullIndicators_[i] = NULL;
        flens_[i]=NULL;
    }
}


DBBindSet::~DBBindSet()
{
    deallocate();
} 


void DBBindSet::allocate()
{
    assert(!allocated_);    
    for(CORBA::ULong i=0; i<getNColumns(); ++i) 
    {
        size_t size = getDefaultLengthOfField(i);
        bindAdresses_[i] = dbFormatHandler_p_->allocate(
                dbBindDescription_.getType(i), size*nRows_
                                                       );
        nullIndicators_[i] = new CORBA::Short[nRows_];
        //memset(nullIndicators_[i],0,1*nRows_); todo - enable and test
        flens_[i] = new CORBA::UShort[nRows_];
        for(CORBA::ULong j=0; j<nRows_; ++j) {
           flens_[i][j]=size; nullIndicators_[i][j]=0;
        }
    }
    allocated_ = true;
}   


void DBBindSet::initialize()
{
    if (allocated_) {
    for(CORBA::ULong i=0; i<getNColumns(); ++i) {
         size_t size = getDefaultLengthOfField(i);
         memset(bindAdresses_[i],0,size*nRows_);
    }   
    }
}

void DBBindSet::deallocate()
{
    if (!allocated_) return;
    for(CORBA::ULong i = 0; i<getNColumns(); ++i) 
    {
        size_t size = getDefaultLengthOfField(i);
        dbFormatHandler_p_->deallocate(
                dbBindDescription_.getType(i), bindAdresses_[i], size*nRows_);
        delete[] nullIndicators_[i];
        nullIndicators_[i] = NULL;
        delete[] flens_[i];
        flens_[i] = NULL;
    }
    allocated_ = false;
};

void  DBBindSet::setNRows(CORBA::ULong nRows)
{
    bool allocFlag = (nRows_ < nRows);
    if (allocFlag) deallocate();
    nRows_ = nRows; 
    if (allocFlag) allocate();
}

void  DBBindSet::setNull(CORBA::ULong row, CORBA::ULong col, bool flg)
{
    if (!allocated_) allocate();
    checkIndexes(row,col);
    nullIndicators_[col][row] = (flg ? -1: 0); 
}


void  DBBindSet::setField(CORBA::ULong row,CORBA::ULong col, 
                          const Field& ft, 
                          UAKGQueryBase& query)
{
    checkIndexes(row,col);
    switch( ft.getType() ) {
       case TypeNull:
            setNull(row,col,true);
            break;
       case TypeBoolean:
            setBoolean(row,col,ft.getBoolean());
            break;
       case TypeChar:
            setChar(row,col,ft.getChar());
            break;
       case TypeOctet:
            setOctet(row,col,ft.getOctet());
            break;
       case TypeShort:
            setShort(row,col,ft.getShort());
            break;
       case TypeUShort:
            setUShort(row,col,ft.getUShort());
            break;
       case TypeLong:
            setLong(row,col,ft.getLong());
            break;
       case TypeULong:
            setULong(row,col,ft.getULong());
            break;
       case TypeFloat:
            setFloat(row,col,ft.getFloat());
            break;
       case TypeDouble:
            setDouble(row,col,ft.getDouble());
            break;
       case TypeString:
            setString(row,col,ft.getString_());
            break;
       case TypeSmallInt:
            setSmallInt(row,col,ft.getShort());
            break;
       case TypeInteger:
            setInteger(row,col,ft.getLong());
            break;
       case TypeReal:
            setReal(row,col,ft.getFloat());
            break;
       case TypeDoublePrecision:
            setDoublePrecision(row,col,ft.getDouble());
            break;
       case TypeCharacter:
            setCharacter(row,col, ft.getString_() );
            break;
       case TypeDecimal:
            setDecimal(row,col,ft.getNumeric_());
            break;
       case TypeNumeric:
            setNumeric(row,col,ft.getNumeric_());
            break;
       case TypeDateTime:
            setDateTime(row,col,ft.getDateTime());
            break;
      case TypeRaw:
                {
#ifdef CORBA_SEQ_2_0_MAPPING
                 OctSeq octSeq(ft.getRaw().length(),ft.getRaw().get_buffer(),0);
#else
         OctSeq *ptr = const_cast<OctSeq*>(ft.getRaw_());
                 OctSeq octSeq(ptr->length(),ptr->length(),ptr->get_buffer(),0);
#endif
                 setRaw(row,col,octSeq);
                }
                break;
      case TypeBlob:
               {
               Blob_var bl = Blob::_duplicate(ft.getBlob_());
               PortableServer::POA_var bpoa=query.getUAKGQueryManagerBasePtr()
                                                       ->getBlobWritingPOA();
              

               // TODO: check exceptions fron reference_to_id
               PortableServer::ObjectId_var id = bpoa->reference_to_id(bl);
               void* dst = getRawMemBuffPtr(row,col);
               void* src = *((void**)id->get_buffer());
 
               query.assignBlob((void**)dst,src);
               }
               break;
      case TypeClob:
              {
               Clob_var cl = Clob::_duplicate(ft.getClob_());
               PortableServer::POA_var cpoa=query.getUAKGQueryManagerBasePtr()
                                                       ->getClobWritingPOA();

               // TODO: check exceptions fron reference_to_id
               PortableServer::ObjectId_var id = cpoa->reference_to_id(cl);

               void* lobData;
               memcpy(&lobData,id->get_buffer(),sizeof(void*));
               void* dst = getRawMemBuffPtr(row,col);
               query.assignBlob((void**)dst,lobData);
               }
                break;
      case TypeWString:
                setWString(row,col,ft.getWString_());
                break;
      case TypeWclob:
              {
               Wclob_var wcl = Wclob::_duplicate(ft.getWclob_());
               PortableServer::POA_var wcpoa=query.getUAKGQueryManagerBasePtr()
                                                       ->getWclobWritingPOA();

               // TODO: check exceptions fron reference_to_id
               PortableServer::ObjectId_var id = wcpoa->reference_to_id(wcl);

               void* lobData;
               memcpy(&lobData,id->get_buffer(),sizeof(void*));
               void* dst = getRawMemBuffPtr(row,col);
               query.assignBlob((void**)dst,lobData);
               }
                break;
      default:
                {
                    InternalError ex;
                    ostrstream msgos;
                    msgos << "Unknown value type:" << ft.getType();
                    ex.what = CORBA::string_dup(msgos.str());
                    errors() << ex.what << endl;
                    msgos.rdbuf()->freeze(0);
                    throw ex;
                }
    }
}


namespace {
    int wstrlen(const CORBA::UShort* value) {
        const CORBA::UShort* last = value;
        int i=0;
        while (*last != 0) {
          ++last;
          ++i;
        }
        return i;
    }
}


void  DBBindSet::getField(CORBA::ULong row,CORBA::ULong col, Field& ft
        , UAKGQueryBase& query)
{
    checkIndexes(row,col);
    if (nullIndicators_[col][row]==-1)
    {
        ft.setNull();
        return;
    }
    switch(dbBindDescription_.getType(col))
    {
        case TypeBoolean:
            ft.setBoolean(getBoolean(row,col));
            break;
        case TypeChar:
            ft.setChar(getChar(row,col));
            break;
        case TypeOctet:
            ft.setOctet(getOctet(row,col));
            break;
        case TypeShort:
            ft.setShort(getShort(row,col));
            break;
        case TypeSmallInt:
            ft.setShort(getSmallInt(row,col));
            break;
        case TypeUShort:
            ft.setUShort(getUShort(row,col));
            break;
        case TypeLong:
            ft.setLong(getLong(row,col));
            break;
        case TypeInteger:
            ft.setLong(getInteger(row,col));
            break;
        case TypeULong:
            ft.setULong(getULong(row,col));
            break;
        case TypeFloat:
            ft.setFloat(getFloat(row,col));
            break;
        case TypeReal:
            ft.setFloat(getReal(row,col));
            break;
        case TypeDouble:
            ft.setDouble(getDouble(row,col));
            break;
        case TypeDoublePrecision:
            ft.setDouble(getDoublePrecision(row,col));
            break;
        case TypeString:
            {
            //TODO: check, that commented is valid, 
            //TODO: ft.setString(getConstCharacterPtr(row,col));
                CORBA::UShort flen = flens_[col][row];
                char* s = CORBA::string_alloc(flen+1);
                strncpy(s,getStringPtr(row,col),flen);
                s[flen]='\0';
                ft.setString(s);
		CORBA::string_free(s);
            }
            break;
        case TypeCharacter:
            ft.setString(getConstCharacterPtr(row,col));
            break;
        case TypeDecimal:
            {
                Numeric_var dec = getNumeric(row,col);
                ft.setDecimal(dec);
            }
            break;
        case TypeNumeric:
            {
                Numeric_var num = getNumeric(row,col);
                ft.setNumeric(num);
            }
            break;
        case TypeDateTime:
            {
                DateTime dt = getDateTime(row,col);
                ft.setDateTime(dt);
            }
            break;
        case TypeRaw:
            {
        OctSeq_var octSeq = getRaw(row,col);
                ft.setRaw( octSeq.in() );
        }
            break;
        case TypeBlob:
            {
              const void** lobData = (const void**)getBlobIdPtr(row,col);
              CORBA::ULong lobLen = dbFormatHandler_p_->rawid_length();

              PortableServer::ObjectId id(lobLen,lobLen,
                                         (CORBA::Octet*)(lobData),0);
              //about previous string:
              //   while lobData, not *lobData ? 
              //     void **lobData is pointed to *lobLocator (value, 
              //     which we want put into object id).
              //   so we need move to id context of pointer to *lobLocator,
              //   i. e. **lobLocator. (it's what we have inside lobData).
              //   may be next block of code 
              //     - restore void* from PortableServer::ObjectId id
              //-------
              //     void* backResolvedLobPtr;
              //     memcpy(&backResolvedLobPtr,id.get_buffer(),sizeof(void*));
              //     std::cerr << "backResolved lobPtr=" << backResolvedLobPtr 
              //-------
              //     will show this in more intuitive way for programmer.

              PortableServer::POA_var poa = query.getUAKGQueryManagerBasePtr()->
                                             getBlobReadingPOA();
              CORBA::Object_var o = poa->create_reference_with_id(
                                                id,
                              "IDL:gradsoft.kiev.ua/UAKGQuery2/Blob:1.0"
                                                                   );
              UAKGQuery2::Blob_var bl = UAKGQuery2::Blob::_narrow(o);
              ft.setBlob(bl);
            }
            break;
        case TypeClob:
            {
              const void** lobData = (const void**)getClobIdPtr(row,col);
              CORBA::ULong lobLen = dbFormatHandler_p_->rawid_length();
              PortableServer::ObjectId id(lobLen,lobLen,(CORBA::Octet*)(lobData),0);
              PortableServer::POA_var poa = query.getUAKGQueryManagerBasePtr()->
                                             getClobReadingPOA();
              CORBA::Object_var o = poa->create_reference_with_id(
                                                id,
                              "IDL:gradsoft.kiev.ua/UAKGQuery2/Clob:1.0"
                                                                   );
              UAKGQuery2::Clob_var cl = UAKGQuery2::Clob::_narrow(o);
              ft.setClob(cl);
            }
            break;
        case TypeWString:
            {
                CORBA::UShort* wstrPtr = getWStringPtr(row,col);
                unsigned long len = wstrlen(wstrPtr);
                CORBA::WChar* wstr = CORBA::wstring_alloc(len + 1);
                if (sizeof(CORBA::WChar)==2) {
                  memcpy(wstr, wstrPtr, (len+1)*sizeof(CORBA::WChar));
                }else{
#ifdef HAVE_ICONV
                  iconv_t cd = iconv_open("WCHAR_T","UTF16");
                  char* inbuff = (char*)wstrPtr; 
                  size_t inbytes=(len+1)*sizeof(CORBA::UShort);
                  char* outbuff = (char*)wstr; 
                  size_t outbytes=(len+1)*sizeof(CORBA::WChar);
                  size_t res=iconv(cd,&inbuff,&inbytes,&outbuff,&outbytes);
                  if (res==(size_t)-1) {
                    errors() << "wstring conversion error:" << strerror(errno) << endl;
                  }
                  iconv_close(cd);
#else
#error "iconv is required"
#endif
                }
                wstr[len]=L'\0';
                ft.setWString(wstr);
            }
            break;
        case TypeWclob:
            {
              const void** lobData = (const void**)getWclobIdPtr(row,col);
              CORBA::ULong lobLen = dbFormatHandler_p_->rawid_length();
              PortableServer::ObjectId id(lobLen,lobLen,(CORBA::Octet*)*lobData,0);
              PortableServer::POA_var poa = query.getUAKGQueryManagerBasePtr()->
                                             getWclobReadingPOA();
              CORBA::Object_var o = poa->create_reference_with_id(
                                                id,
                              "IDL:gradsoft.kiev.ua/UAKGQuery2/Wclob:1.0"
                                                                   );
              UAKGQuery2::Wclob_var wcl = UAKGQuery2::Wclob::_narrow(o);
              ft.setWclob(wcl);
            }
            break;
        default:
            throw InternalError("unknown field type");
    }
}



void  DBBindSet::setString(CORBA::ULong row, CORBA::ULong col, const char* value)
{
 checkIndexes(row,col);
 dbBindDescription_.checkType(col,TypeString);
 setString_(row,col,value);
}

void  DBBindSet::setWString(CORBA::ULong row, CORBA::ULong col, const CORBA::WChar* value)
{
 checkIndexes(row,col);
 dbBindDescription_.checkType(col,TypeWString);
 size_t flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col)+2;
 CORBA::UShort* mem_ptr = getWStringPtr(row,col);
 const CORBA::WChar* value_ptr = value;
 memset(mem_ptr,0,flen);
#ifdef HAVE_ICONV
 iconv_t cd = iconv_open("UTF16","WCHAR_T");
 char* inptr = (char*)value;
 size_t inbytes = flen*2;
 char* outptr = (char*)mem_ptr;
 size_t outbytes = flen;
 size_t res=iconv(cd,&inptr,&inbytes,&outptr,&outbytes);
 if (res==(size_t)-1) {
    errors() << "wstring conversion error:" << strerror(errno) << endl;
 }
 iconv_close(cd);
#else
 int i=0;
 for(; i<flen; ++i) {
    *mem_ptr++=(CORBA::UShort)*(value_ptr++);
    if (*value_ptr==0) break;
 }
 if(i==flen) {
   // string to large, DB will raise error.
   // we do nothing
   //  TODO: is it safe to produce something like InvalidDataFormat ?
   errors() << "Warning: DBBind: string too large" << endl;
 }
#endif
 flens_[col][row] = flen+2;//strlen for wstring should be
 setNull(row,col,false);
}


void  DBBindSet::setString_(CORBA::ULong row, CORBA::ULong col, const char* value)
{
 checkIndexes(row,col);
 size_t flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col)+1;
 char* mem_ptr = (char*)getMemPtr(row,col,sizeof(char)*flen);
 memset(mem_ptr,0,flen);
 strncpy(mem_ptr,value,flen);
 if(mem_ptr[flen-1]!=0) {
   // string to large, DB will raise error.
   // we do nothing
   //  TODO: is it safe to produce something like IvalidDataFormat ?
   errors() << "Warning: DBBind: string too large" << endl;
 }
 flens_[col][row] = strlen(value)+1;
 setNull(row,col,false);
}

void  DBBindSet::setCharacter(CORBA::ULong row, CORBA::ULong col, const char* value)
{
 checkIndexes(row,col);
 dbBindDescription_.checkType(col,TypeChar);
 setString_(row,col,value);
}

void  DBBindSet::setDecimal(CORBA::ULong row, CORBA::ULong col, Numeric* decimal)
{
 setNumeric(row,col,decimal);
}
  
Numeric*  DBBindSet::getDecimal(CORBA::ULong row, CORBA::ULong col)
{
   return getNumeric(row,col);
}

void  DBBindSet::setNumeric(CORBA::ULong row, CORBA::ULong col,Numeric* decimal)
{
 checkIndexes(row,col);
 checkDBFormatHandler();
 void* dv = dbFormatHandler_p_->decimal2rawmem(decimal,
                                   dbBindDescription_.getFieldDescription(col)
                                                  );
 //th();
 flens_[col][row]=dbFormatHandler_p_->decimal_length();
 setRawMem(row,col,dv,dbFormatHandler_p_->decimal_length());
 delete[] (unsigned char*)dv;
 setNull(row,col,false);
}
  
Numeric*  DBBindSet::getNumeric(CORBA::ULong row, CORBA::ULong col)
{
 checkDBFormatHandler();
 void* dv = getRawMemBuffPtr(row,col);
 Numeric* retval = dbFormatHandler_p_->rawmem2decimal(dv, dbBindDescription_.getFieldDescription(col));
 return retval;
}

void  DBBindSet::setDateTime(CORBA::ULong row, CORBA::ULong col, const DateTime& datetime)
{
 checkDBFormatHandler();
 if (!UAKGQueryUtils::checkDate(datetime)) {
   throw InvalidDate(); 
 }
 void* dv = dbFormatHandler_p_->datetime2rawmem(datetime,
                                   dbBindDescription_.getFieldDescription(col)
                                                  );
 setRawMem(row,col,dv,dbFormatHandler_p_->datetime_length());
 delete[] (unsigned char*)dv;
 flens_[col][row]=dbFormatHandler_p_->datetime_length();
 setNull(row,col,false);
}


DateTime   DBBindSet::getDateTime(CORBA::ULong row, CORBA::ULong col)
{
 checkDBFormatHandler();
 void* dv = getRawMemBuffPtr(row,col);
 DateTime retval = dbFormatHandler_p_->rawmem2datetime(dv,
                                   dbBindDescription_.getFieldDescription(col)
                                                      );
 return retval;
}

void DBBindSet::setRawMem(CORBA::ULong row, CORBA::ULong col, 
                          const void* buff, CORBA::ULong len)
{
 checkDBFormatHandler();
 flens_[col][row]=len;
 size_t flen=getDefaultLengthOfField(col);
 if (flen < len) {
   //TODO: declare InvalidFieldSize
   //throw InvalidFieldSize();
   throw QueryProcessingError("Invalid field size in setRawMem",500,0);
 }
 memcpy(getMemPtr(row,col,flen),buff,len);
}

void      DBBindSet::setRaw(CORBA::ULong row, CORBA::ULong col, void* buff, CORBA::ULong len)
{
 checkDBFormatHandler();
 FieldDescription fd = dbBindDescription_.getFieldDescription(col);
 size_t flen = fd.size;
 size_t real_flen = dbFormatHandler_p_->raw_length(fd);
 if (flen < real_flen) {
   //TODO: throw InvalidFieldSize;
   //now: trancate
 }
 dbFormatHandler_p_->raw2rawmem_fill(buff, len, getMemPtr(row,col,flen), fd);
}

void      DBBindSet::setRawMem(CORBA::ULong row, CORBA::ULong col, const OctSeq& value)
{
 setRawMem(row,col,(CORBA::Octet*)value.get_buffer(),value.length());
}

void      DBBindSet::setRaw(CORBA::ULong row, CORBA::ULong col, const OctSeq& value)
{
 setRaw(row,col,(CORBA::Octet*)value.get_buffer(),value.length());
}

void*     DBBindSet::getRawMemBuffPtr(CORBA::ULong row, CORBA::ULong col)
{
 size_t flen = 0;
 if(dbBindDescription_.getRecordSet_()->haveFieldSizeAt(col)){
    flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col);
 }else{
    flen = getDefaultLengthOfField(col);
 }
 assert(flen!=0 || flen==0 && row==0);
 return getMemPtr(row,col,flen);
}

OctSeq*   DBBindSet::getRawMem(CORBA::ULong row, CORBA::ULong col)
{
 size_t flen = dbBindDescription_.getRecordSet_()->getFieldSizeAt(col);
 OctSeq* retval = new OctSeq;
 retval->length(flens_[col][row]);
 memcpy(retval->get_buffer(),getMemPtr(row,col,flen),flens_[col][row]);
 return retval;
}

OctSeq*   DBBindSet::getRaw(CORBA::ULong row, CORBA::ULong col)
{
 size_t flen = getDefaultLengthOfField(col);
 return dbFormatHandler_p_->rawmem2raw(getMemPtr(row,col,flen),
                                       getFieldDescription(col)
                                      );
}

void   DBBindSet::setBlobId(CORBA::ULong row, CORBA::ULong col, const void* value)
{
    setRawMem(row,col,(CORBA::Octet*)value,dbFormatHandler_p_->rawid_length());
}


CORBA::UShort  DBBindSet::getDefaultLengthOfField(CORBA::ULong col)
{
 switch(dbBindDescription_.getRecordSet_()->getFieldTypeAt(col)){
      case TypeBoolean:
          return sizeof(CORBA::Boolean);
      case TypeChar:
          return sizeof(CORBA::Char);
      case TypeOctet:
          return sizeof(CORBA::Octet);
      case TypeShort:
      case TypeUShort:
      case TypeSmallInt:
          return sizeof(CORBA::UShort);
      case TypeLong:
      case TypeULong:
      case TypeInteger:
          return sizeof(CORBA::ULong);
      case TypeFloat:
      case TypeReal:
          return sizeof(CORBA::Float);
      case TypeDouble:
      case TypeDoublePrecision:
          return sizeof(CORBA::Double);
      case TypeNumeric:
      case TypeDecimal:
          return dbFormatHandler_p_->decimal_length();
      case TypeDateTime:
          return dbFormatHandler_p_->datetime_length();
/*
      case TypeObject:
          return dbFormatHandler_p_->object_length();
*/
      case TypeRaw:
          return dbFormatHandler_p_->raw_length(
                          dbBindDescription_.getFieldDescription(col));
      case TypeBlob:
      case TypeClob:
      case TypeWclob:
          return dbFormatHandler_p_->rawid_length();
      case TypeString:
        return (CORBA::UShort)dbBindDescription_.getRecordSet_()->getFieldSizeAt(col)+1;
      case TypeWString:
        return (CORBA::UShort)dbBindDescription_.getRecordSet_()->getFieldSizeAt(col)+2;
      default:
        return (CORBA::UShort)dbBindDescription_.getRecordSet_()->getFieldSizeAt(col);
 }
}


void  DBBindSet::dumpColumn(std::ostream& out, int col)
{
  if (!allocated_) {
    out << "{not allocated}" << std::endl;
  }else{
    char* carrd = (char*)bindAdresses_[col];
    size_t size = getDefaultLengthOfField(col);
    for(int i=0;i<nRows_; ++i) {
      out << "[";
      for(int j=0; j<size; ++j) {
        if (isprint(*carrd)) {
            out << (*carrd);
        }else{
            out << ' ';
        }
        ++carrd;
      }
      out << "]" << std::endl;
    }
  }
  
}


