#ifndef __Common_UAKGLobs_h
#define __Common_UAKGLobs_h

/*
 * part of Grad-Soft UAKGQuery
 * (C) Grad-Soft (tm), Kiev, Ukraine.
 * 1998 - 2003
 * $Id: UAKGLobs.h,v 1.7 2006-01-23 17:46:09 rssh Exp $
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

class UAKGQueryManagerBase;

/*
* Base class for default servants of LOB objects.
*/
class UAKGLobBase : virtual public PortableServer::RefCountServantBase
{
private:

    PortableServer::POA_ptr poa_p_ ; // we use ptr instead var,
                                    // becouse value inside poa_ var
                                    // became incorrect in subclasses
                                     // in virtual base initialization
                                    // (compiler bag) ?

    UAKGQueryManagerBase*  owner_p_;

public:

    UAKGLobBase():poa_p_(NULL),owner_p_(NULL)
    {
    }
 
    virtual ~UAKGLobBase() 
    {
    }

    /**
     * must be called after object creation in subclasses.
     * (this is workarround accross gcc-3.2 bug)
     **/
    void initUAKGLobBase()
    {
     poa_p_=NULL;
     owner_p_=NULL;
    }

    virtual CORBA::ULong length() = 0;

    virtual void close() = 0;

    PortableServer::ObjectId* currentId(); 

    void                    set_POA(PortableServer::POA_ptr poa)
       { 
         poa_p_ = poa; 
       }

    void printPOA(std::ostream& out)
    {
     out << "UAKGLobBase::printPOA, poa_p_=" << poa_p_;
    }

    PortableServer::POA_ptr get_POA()
       { return PortableServer::POA::_duplicate(poa_p_); }

    PortableServer::POA_ptr _default_POA()
       { return get_POA(); }

    UAKGQueryManagerBase*  getOwnerPtr()
     { return owner_p_; }

    void  setOwnerPtr(UAKGQueryManagerBase* owner_p)
      { owner_p_=owner_p; }
  

private:

    UAKGLobBase(const UAKGLobBase&);
    UAKGLobBase& operator=(const UAKGLobBase&);

};

class UAKGBlobForReading : virtual public POA_UAKGQuery2::Blob,
        virtual public UAKGLobBase
{
public:

    UAKGBlobForReading():UAKGLobBase()
       {}

    virtual ~UAKGBlobForReading() {}

    virtual CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError) = 0;
	
    virtual UAKGQuery2::OctSeq* fetch_chunk(CORBA::ULong chunkSize,
                                           CORBA::Boolean& more)
		             throw(CORBA::SystemException,
                                   UAKGQuery2::WriteOnlyLOB,
                                   UAKGQuery2::QueryProcessingError) = 0;
	
    void put_chunk(const UAKGQuery2::OctSeq& data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError)
     {
      throw UAKGQuery2::ReadOnlyLOB();
     }

     virtual void destroy() throw(CORBA::SystemException)
     {
      close();
     }

};


class UAKGClobForReading : virtual public POA_UAKGQuery2::Clob,
        virtual public UAKGLobBase
{
public:

    UAKGClobForReading():UAKGLobBase()
        {}

    virtual ~UAKGClobForReading()
        {}

    virtual CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)=0;
	
    virtual char* fetch_chunk(CORBA::ULong chunkSize,
                      CORBA::Boolean& more)
                          throw(CORBA::SystemException,
                                UAKGQuery2::WriteOnlyLOB,
                                UAKGQuery2::QueryProcessingError)=0;
	
    void put_chunk(const char* data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError)
    {
     throw UAKGQuery2::ReadOnlyLOB();
    }

    PortableServer::POA_ptr _default_POA()
       { return get_POA(); }

	 virtual void destroy() throw(CORBA::SystemException)
	 {
	   close();
	 }


};

class UAKGWclobForReading : virtual public POA_UAKGQuery2::Wclob,
        virtual public UAKGLobBase
{
public:

    UAKGWclobForReading():UAKGLobBase() {}
    virtual ~UAKGWclobForReading() {}
	
    virtual CORBA::ULong length() throw (CORBA::SystemException,
                                 UAKGQuery2::QueryProcessingError) = 0;
	
    virtual CORBA::WChar* fetch_chunk(CORBA::ULong chunkSize,
                              CORBA::Boolean& more)
			              throw (CORBA::SystemException,
                                 UAKGQuery2::WriteOnlyLOB,
                                 UAKGQuery2::QueryProcessingError) = 0;
	
    void put_chunk(const CORBA::WChar* data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError)
    {
     throw UAKGQuery2::ReadOnlyLOB();
    }

    PortableServer::POA_ptr _default_POA()
       { return get_POA(); }

	virtual void destroy() throw (CORBA::SystemException)
	{
	   close();
	}


};

class UAKGBlobForWriting : virtual public POA_UAKGQuery2::Blob,
        virtual public UAKGLobBase
{
public:

    UAKGBlobForWriting():UAKGLobBase() {}
    virtual ~UAKGBlobForWriting() {}

    virtual CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)=0;

    virtual UAKGQuery2::OctSeq* fetch_chunk(CORBA::ULong chunkSize,
                                    CORBA::Boolean& more)
			              throw (CORBA::SystemException,
                                 UAKGQuery2::WriteOnlyLOB,
                                 UAKGQuery2::QueryProcessingError)
    {
     throw UAKGQuery2::WriteOnlyLOB();
    }

    virtual void put_chunk(const UAKGQuery2::OctSeq& data)
                          throw (CORBA::SystemException,
                                 UAKGQuery2::ReadOnlyLOB,
                                 UAKGQuery2::QueryProcessingError) = 0;

    PortableServer::POA_ptr _default_POA()
       { return get_POA(); }

	virtual void destroy()throw (CORBA::SystemException)
	{
	   close();
	}


};

class UAKGClobForWriting : virtual public POA_UAKGQuery2::Clob,
        virtual public UAKGLobBase
{
public:

    UAKGClobForWriting():UAKGLobBase() {}
    virtual ~UAKGClobForWriting() {}

    virtual CORBA::ULong length() throw (CORBA::SystemException,
                                 UAKGQuery2::QueryProcessingError)=0;
	
    char* fetch_chunk(CORBA::ULong chunkSize,
                      CORBA::Boolean& more)
                          throw (CORBA::SystemException,
                                 UAKGQuery2::WriteOnlyLOB,
                                 UAKGQuery2::QueryProcessingError)
   	{
        throw UAKGQuery2::WriteOnlyLOB();
        }
	
    virtual void put_chunk(const char* data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError) = 0;

    PortableServer::POA_ptr _default_POA()
       { return get_POA(); }

	virtual void destroy() throw (CORBA::SystemException)
	{
	   close();
	}


};

class UAKGWclobForWriting : virtual public POA_UAKGQuery2::Wclob,
                  virtual public UAKGLobBase
{
public:

    UAKGWclobForWriting():UAKGLobBase()
    {}

    virtual ~UAKGWclobForWriting() {}
	
    virtual CORBA::ULong length() throw(CORBA::SystemException,
                                UAKGQuery2::QueryProcessingError)=0;
	
    CORBA::WChar* fetch_chunk(CORBA::ULong chunkSize,
                              CORBA::Boolean& more)
                          throw(CORBA::SystemException,
                                UAKGQuery2::WriteOnlyLOB,
                                UAKGQuery2::QueryProcessingError)
   	{
        throw UAKGQuery2::WriteOnlyLOB();
        }

    virtual void put_chunk(const CORBA::WChar* data)
                          throw(CORBA::SystemException,
                                UAKGQuery2::ReadOnlyLOB,
                                UAKGQuery2::QueryProcessingError) = 0;

    PortableServer::POA_ptr _default_POA()
       { return get_POA(); }

	virtual void destroy() throw (CORBA::SystemException)
	{
	   close();
	}


};


#endif
