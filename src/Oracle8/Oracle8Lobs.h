#ifndef __Oracle8Lobs_h
#define __Oracle8Lobs_h

/*
 * part of Grad-Soft UAKGQuery
 * (C) Grad-Soft (tm), Kiev, Ukraine.
 * http://www.gradsoft.kiev.ua
 * 1998 - 2003
 * $Id: Oracle8Lobs.h,v 1.11 2007-02-16 03:51:01 rssh Exp $
 */

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Oracle8_Oracle8Query_h
#include <Oracle8/Oracle8Query.h>
#endif

#ifndef __Common_UAKGLobs_h
#include <Common/UAKGLobs.h>
#endif

#ifndef __GradSoft_set_INCLUDED
#include <set>
#ifndef __GradSoft_set_INCLUDED
#define __GradSoft_set_INCLUDED
#endif
#endif

#ifndef __Oracle8_importOCI_h
#include <Oracle8/importOCI.h>
#endif

/**
 * LOB Base: base for default servants .
 **/
class Oracle8LobBase : virtual public UAKGLobBase
{
public:
	Oracle8LobBase()
           :UAKGLobBase(),
            oraQueryManager_p_(NULL)
        {}

	virtual ~Oracle8LobBase();
        CORBA::ULong length();

	void destroy() throw(CORBA::SystemException);

protected:

	OCIEnv* getOCIEnv_p();
	OCISvcCtx* getOCISvcCtx_p();
	OCIError* getOCIError_p();

    void lazyInitOracleQueryManager();

	void fillOCILobLocator(OCILobLocator*& lobLocator, const PortableServer::ObjectId& id);

	void accept(OCILobLocator* lobLocator);

	char* readChunk(OCILobLocator* lob,
                        CORBA::ULong chunkSize, 
                        CORBA::ULong& actualLength,
                        CORBA::Boolean& more,
                        UAKGQuery2::FieldType t);

	void writeChunk(OCILobLocator* lobLocator,
                                const char* data, unsigned long length,
				UAKGQuery2::FieldType t);

	void close();


private:
	Oracle8QueryManager* oraQueryManager_p_;
        std::set<OCILobLocator*> tmpLobs_;
        GradSoft::Mutex tmpLobsMutex_;

private:
	Oracle8LobBase(const Oracle8LobBase&);
};

class Oracle8BlobForReading : virtual public UAKGBlobForReading,
                              virtual public Oracle8LobBase
{
public:

   Oracle8BlobForReading():UAKGBlobForReading(),Oracle8LobBase()
   {}

	CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)
   	{
        return Oracle8LobBase::length();
        }
	
    UAKGQuery2::OctSeq* fetch_chunk(CORBA::ULong chunkSize,
                                    CORBA::Boolean& more)
			              throw(CORBA::SystemException,
                                UAKGQuery2::WriteOnlyLOB,
                                UAKGQuery2::QueryProcessingError);
	
private:

    
};

class Oracle8ClobForReading : virtual public UAKGClobForReading,
        virtual public Oracle8LobBase
{
public:

   Oracle8ClobForReading():UAKGClobForReading(),Oracle8LobBase()
   {}

   CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)
    {
     return Oracle8LobBase::length();
    }
	
    char* fetch_chunk(CORBA::ULong chunkSize,
                      CORBA::Boolean& more)
                          throw(CORBA::SystemException,
                                UAKGQuery2::WriteOnlyLOB,
                                UAKGQuery2::QueryProcessingError);
	
};

class Oracle8WclobForReading : virtual public UAKGWclobForReading,
        virtual public Oracle8LobBase
{
public:

   Oracle8WclobForReading():UAKGWclobForReading(),Oracle8LobBase()
   {}

	
    CORBA::ULong length() throw (CORBA::SystemException,
                                 UAKGQuery2::QueryProcessingError)
	{
        return Oracle8LobBase::length();
    }
	
    CORBA::WChar* fetch_chunk(CORBA::ULong chunkSize,
                              CORBA::Boolean& more)
			              throw (CORBA::SystemException,
                                 UAKGQuery2::WriteOnlyLOB,
                                 UAKGQuery2::QueryProcessingError);
	
};

class Oracle8BlobForWriting : virtual public UAKGBlobForWriting,
        virtual public Oracle8LobBase
{
public:

   Oracle8BlobForWriting():UAKGBlobForWriting(),Oracle8LobBase()
   {}

   CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)
   {
    return Oracle8LobBase::length();
   }

	void put_chunk(const UAKGQuery2::OctSeq& data)
                          throw (CORBA::SystemException,
                                 UAKGQuery2::ReadOnlyLOB,
                                 UAKGQuery2::QueryProcessingError);
};

class Oracle8ClobForWriting : virtual public UAKGClobForWriting,
        virtual public Oracle8LobBase
{
public:

   Oracle8ClobForWriting():UAKGClobForWriting(),Oracle8LobBase()
   {}

   CORBA::ULong length() throw (CORBA::SystemException,
                                 UAKGQuery2::QueryProcessingError)
   {
    return Oracle8LobBase::length();
   }
	
	
    void put_chunk(const char* data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError);
};

class Oracle8WclobForWriting : virtual public UAKGWclobForWriting,
        virtual public Oracle8LobBase
{
public:
	
   Oracle8WclobForWriting():UAKGWclobForWriting(),Oracle8LobBase()
   {}

    CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)
    {
        return Oracle8LobBase::length();
    }
	
 

    void put_chunk(const CORBA::WChar* data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError);

};


#endif
