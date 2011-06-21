#include <Oracle8/Oracle8Lobs.h>
#include <Oracle8/Oracle8QueryManager.h>

#include <climits>

OCIEnv* Oracle8LobBase::getOCIEnv_p()
{ lazyInitOracleQueryManager(); return oraQueryManager_p_->getOCIEnv_p(); }

OCISvcCtx* Oracle8LobBase::getOCISvcCtx_p()
{ lazyInitOracleQueryManager(); return oraQueryManager_p_->getOCISvcCtx_p(); }

OCIError* Oracle8LobBase::getOCIError_p()
{ lazyInitOracleQueryManager(); return oraQueryManager_p_->getOCIError_p(); }


void Oracle8LobBase::lazyInitOracleQueryManager()
{
 if (oraQueryManager_p_==NULL) {
   oraQueryManager_p_=dynamic_cast<Oracle8QueryManager*>(getOwnerPtr());
   if (oraQueryManager_p_==NULL) {
	 throw UAKGQuery2::QueryProcessingError("internal error in Oracle8LobBase::lazyInitOracle8Manager",4000,0);
   }
 }
}


char* Oracle8LobBase::readChunk(OCILobLocator* lob,
                                CORBA::ULong chunkSize, 
				CORBA::ULong& actualLength,
                                CORBA::Boolean& more,
				UAKGQuery2::FieldType t)
{
    // Read segment from blob

  if (chunkSize==0) {
      ub4 lobLength;
      sword rc=OCILobGetLength(getOCISvcCtx_p(),getOCIError_p(),lob,&lobLength);
      oraQueryManager_p_->checkError(rc);
      chunkSize=lobLength;
  }


    char* blobSegment;
    switch (t) {
        case UAKGQuery2::TypeBlob:
            blobSegment = reinterpret_cast<char*>(new CORBA::Octet[chunkSize]);
            break;
        case UAKGQuery2::TypeClob:
            blobSegment = CORBA::string_alloc(chunkSize+1);
            blobSegment[chunkSize]='\0';
            break;
	case UAKGQuery2::TypeWclob:
            blobSegment = reinterpret_cast<char*>(CORBA::wstring_alloc(chunkSize+1));
            blobSegment[chunkSize*sizeof(CORBA::WChar)]='\0';
            break;
        default:
        	throw UAKGQuery2::QueryProcessingError("Internal error 1 in Oracle8LobBase::readChunk",4001,0);
    }

    ub4 ociActualLength = 0;
    sb2 ret_code = OCILobRead(getOCISvcCtx_p(), getOCIError_p(), lob, 
		                        &ociActualLength, (ub4)1,
                                (dvoid *) blobSegment,
                                (ub4) chunkSize, (dvoid *)0,
                                (sb4 (*) (dvoid *, CONST dvoid *, ub4, ub1)) 0,
                                (ub2) 0, (ub1) SQLCS_IMPLICIT);
    if (ret_code == OCI_NEED_DATA) {
        more = true;
    } else {
        more = false;
        oraQueryManager_p_->checkError(ret_code);
        blobSegment[ociActualLength]='\0';
      }
    actualLength = ociActualLength;
    
    return blobSegment;
}

void Oracle8LobBase::writeChunk(OCILobLocator* lobLocator,
                                const char* data, unsigned long length,
				UAKGQuery2::FieldType t)
{
    oraQueryManager_p_->checkError(
        OCILobOpen(getOCISvcCtx_p(), getOCIError_p(), lobLocator, OCI_LOB_READWRITE)
    );
 
    ub4 amtp = length;
    oraQueryManager_p_->checkError(
            OCILobWriteAppend(getOCISvcCtx_p(), getOCIError_p(), lobLocator, 
			                  &amtp, 
							  (dvoid*) data,
                              length,
							  OCI_ONE_PIECE, 
							  NULL, /* context for callback */
							  NULL, /* callback */
							  0, /* csID */
							  SQLCS_IMPLICIT)
        );
    oraQueryManager_p_->checkError(
        OCILobClose(getOCISvcCtx_p(), getOCIError_p(), lobLocator)
    );
}

void Oracle8LobBase::close() 
{
 OCILobLocator* lobLocator;
 PortableServer::ObjectId_var cid=currentId();
 fillOCILobLocator(lobLocator,cid.in());
 boolean temporary=FALSE;
 OCILobIsTemporary(getOCIEnv_p(),getOCIError_p(),lobLocator,&temporary);
 if (temporary==TRUE) {
    oraQueryManager_p_->checkError(
        OCILobFreeTemporary(getOCISvcCtx_p(), getOCIError_p(), lobLocator)
    );
    {
     GradSoft::MutexLocker ml(tmpLobsMutex_);
	 tmpLobs_.erase(lobLocator);
    }
    // non-temporary BLOBS will be closed in query.
    try {
      oraQueryManager_p_->checkError(
        OCIDescriptorFree(lobLocator, OCI_DTYPE_LOB)
      );
    }catch(const UAKGQuery2::QueryProcessingError& ex){
      if (oraQueryManager_p_->getDebugLevel()>5) {
        oraQueryManager_p_->getLogger().warnings() << 
             "[" << __FILE__ << ":" << __LINE__ << "]";
      }
      oraQueryManager_p_->getLogger().warnings() << 
              "error during Oracle LOB destroy:" << ex.why << std::endl;
     
    }
 }
}

void Oracle8LobBase::accept(OCILobLocator* lobLocator)
{
 boolean temporary;
 OCILobIsTemporary(getOCIEnv_p(),getOCIError_p(),lobLocator,&temporary);
 if (temporary) {
	 GradSoft::MutexLocker ml(tmpLobsMutex_);
	 std::set<OCILobLocator*>::const_iterator it=tmpLobs_.find(lobLocator);
	 if (it!=tmpLobs_.end()) tmpLobs_.insert(lobLocator);
 }
}

Oracle8LobBase::~Oracle8LobBase()
{
  std::set<OCILobLocator*>::const_iterator it;
  for(it=tmpLobs_.begin(); it!=tmpLobs_.end(); ++it) {
      OCILobFreeTemporary(getOCISvcCtx_p(), getOCIError_p(), *it);
      OCIDescriptorFree(*it, OCI_DTYPE_LOB);
  }
  tmpLobs_.erase(tmpLobs_.begin(),tmpLobs_.end());
}

void Oracle8LobBase::fillOCILobLocator(OCILobLocator*& lobLocator, 
									   const PortableServer::ObjectId& id)
{
 memcpy(&lobLocator,id.get_buffer(),sizeof(OCILobLocator*));
}



CORBA::ULong Oracle8LobBase::length()
{
 OCILobLocator* lobLocator;
 PortableServer::ObjectId_var cid=currentId();
 fillOCILobLocator(lobLocator,cid.in());

 ub4 lobLength;
 sword rc=OCILobGetLength(getOCISvcCtx_p(),getOCIError_p(),lobLocator,&lobLength);
 oraQueryManager_p_->checkError(rc);
 return lobLength;
}

UAKGQuery2::OctSeq* Oracle8BlobForReading::fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more)
        throw(CORBA::SystemException,
              UAKGQuery2::WriteOnlyLOB,
              UAKGQuery2::QueryProcessingError)
{

  OCILobLocator* lobLocator;
  PortableServer::ObjectId_var cid=currentId();
  fillOCILobLocator(lobLocator,cid.in());
  accept(lobLocator);


  CORBA::ULong actual_length;
  char* data = readChunk(lobLocator, chunkSize, actual_length, more, 
		         UAKGQuery2::TypeBlob);


  if (data == NULL) return new UAKGQuery2::OctSeq();

  UAKGQuery2::OctSeq* result = new UAKGQuery2::OctSeq(
      actual_length, actual_length, reinterpret_cast<unsigned char*>(data), 1);

  return result;
}

char* Oracle8ClobForReading::fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more)
		throw(CORBA::SystemException,
			UAKGQuery2::WriteOnlyLOB,
			UAKGQuery2::QueryProcessingError)
{
//std::cerr << "Oracle8ClobForReading::fetch_chunk begin" << std::endl;
  OCILobLocator* lobLocator;
  PortableServer::ObjectId_var cid=currentId();
  fillOCILobLocator(lobLocator,cid.in());
  accept(lobLocator);
  CORBA::ULong actual_length;

//std::cerr << "Oracle8ClobForReading::fetch_chunk 1" << std::endl;

  char* data = readChunk(lobLocator, chunkSize, actual_length, more, 
		         UAKGQuery2::TypeClob);

//std::cerr << "Oracle8ClobForReading::fetch_chunk 2" << std::endl;
   
  if (data == NULL) data = CORBA::string_dup("");

//std::cerr << "Oracle8ClobForReading::fetch_chunk end" << std::endl;
  return data;
}

CORBA::WChar*
Oracle8WclobForReading::fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more)
		throw (CORBA::SystemException,
				UAKGQuery2::WriteOnlyLOB,
				UAKGQuery2::QueryProcessingError)
{
  OCILobLocator* lobLocator;
  PortableServer::ObjectId_var cid=currentId();
  fillOCILobLocator(lobLocator,cid.in());
  accept(lobLocator);


  CORBA::ULong actual_length;
  char* data = readChunk(lobLocator, chunkSize, actual_length, more, 
		         UAKGQuery2::TypeWclob);
  if (data == NULL) return CORBA::wstring_dup((const CORBA::WChar*)L"");
  return (CORBA::WChar*)data;
}	


//
// For writing (query parameters)
//

void
Oracle8BlobForWriting::put_chunk(const UAKGQuery2::OctSeq& data)
		throw (CORBA::SystemException,
				UAKGQuery2::ReadOnlyLOB,
				UAKGQuery2::QueryProcessingError)
{
  OCILobLocator* lobLocator;
  PortableServer::ObjectId_var cid=currentId();
  fillOCILobLocator(lobLocator,cid.in());
  accept(lobLocator);
  writeChunk(lobLocator, reinterpret_cast<const char*>(data.get_buffer()), data.length(),
	         UAKGQuery2::TypeBlob);
}

void Oracle8ClobForWriting::put_chunk(const char* data)	throw(CORBA::SystemException,
				                                      UAKGQuery2::ReadOnlyLOB,
				                                      UAKGQuery2::QueryProcessingError)
{
  OCILobLocator* lobLocator;
  PortableServer::ObjectId_var cid=currentId();
  fillOCILobLocator(lobLocator,cid.in());
  accept(lobLocator);
  writeChunk(lobLocator, data, strlen(data),UAKGQuery2::TypeClob);
}

namespace {
    unsigned long get_wchar_byte_length(const CORBA::WChar* data) {
        const CORBA::WChar* p = data;
        while (*p != 0 && ((p - data) < USHRT_MAX)) ++p;
        return p - data;
    }
}

void
Oracle8WclobForWriting::put_chunk(const CORBA::WChar* data)
		throw(CORBA::SystemException,
				UAKGQuery2::ReadOnlyLOB,
				UAKGQuery2::QueryProcessingError)
{
  OCILobLocator* lobLocator;
  PortableServer::ObjectId_var cid=currentId();
  fillOCILobLocator(lobLocator,cid.in());
  accept(lobLocator);
  writeChunk(lobLocator, (char*)data, get_wchar_byte_length(data),UAKGQuery2::TypeWclob);
}
