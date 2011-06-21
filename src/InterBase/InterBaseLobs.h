#ifndef __InterBaseLobs_h
#define __InterBaseLobs_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#include <InterBase/InterBaseQuery.h>
#include <map>

class InterBaseLobBase {
private:
    PortableServer::Current_var current_;
protected:
    void fill_properties(ISC_QUAD& id, InterBaseQuery*& q);
public:
    InterBaseLobBase(PortableServer::Current_ptr current)
            : current_(current) {}
    CORBA::ULong length();
    PortableServer::ObjectId* id();
};

class InterBaseBlobForReading : virtual public POA_UAKGQuery2::Blob,
        virtual public PortableServer::RefCountServantBase,
        virtual public InterBaseLobBase
{
public:
    InterBaseBlobForReading(PortableServer::Current_ptr current)
            : InterBaseLobBase(current) {}
    CORBA::ULong length() {
        return InterBaseLobBase::length();
    }
    UAKGQuery2::OctSeq* fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more);
    void put_chunk(const UAKGQuery2::OctSeq& data) {
        throw UAKGQuery2::ReadOnlyLOB();
    }
};

class InterBaseClobForReading : virtual public POA_UAKGQuery2::Clob,
        virtual public PortableServer::RefCountServantBase,
        virtual public InterBaseLobBase
{
public:
    InterBaseClobForReading(PortableServer::Current_ptr current)
            : InterBaseLobBase(current) {}
    CORBA::ULong length() {
        return InterBaseLobBase::length();
    }
    char* fetch_chunk(CORBA::ULong chunkSize
                , CORBA::Boolean& more);
    void put_chunk(const char* data) {
        throw UAKGQuery2::ReadOnlyLOB();
    }
};

class InterBaseWclobForReading : virtual public POA_UAKGQuery2::Wclob,
        virtual public PortableServer::RefCountServantBase,
        virtual public InterBaseLobBase
{
public:
    InterBaseWclobForReading(PortableServer::Current_ptr current)
            : InterBaseLobBase(current) {}
    CORBA::ULong length() {
        return InterBaseLobBase::length();
    }
    CORBA::WChar* fetch_chunk(CORBA::ULong chunkSize
                , CORBA::Boolean& more);
    void put_chunk(const CORBA::WChar* data) {
        throw UAKGQuery2::ReadOnlyLOB();
    }
};

class InterBaseBlob : virtual public POA_UAKGQuery2::Blob,
        virtual public PortableServer::RefCountServantBase,
        virtual public InterBaseLobBase
{
public:
    InterBaseBlob(PortableServer::Current_ptr current)
            : InterBaseLobBase(current) {}
    CORBA::ULong length() {
        return InterBaseLobBase::length();
    }
    UAKGQuery2::OctSeq* fetch_chunk(CORBA::ULong chunkSize
                , CORBA::Boolean& more) {
        throw UAKGQuery2::WriteOnlyLOB();
    }
    void put_chunk(const UAKGQuery2::OctSeq& data);
};

class InterBaseClob : virtual public POA_UAKGQuery2::Clob,
        virtual public PortableServer::RefCountServantBase,
        virtual public InterBaseLobBase
{
public:
    InterBaseClob(PortableServer::Current_ptr current)
            : InterBaseLobBase(current) {}
    CORBA::ULong length() {
        return InterBaseLobBase::length();
    }
    char* fetch_chunk(CORBA::ULong chunkSize
                , CORBA::Boolean& more) {
        throw UAKGQuery2::WriteOnlyLOB();
    }
    void put_chunk(const char* data);
};

class InterBaseWclob : virtual public POA_UAKGQuery2::Wclob,
        virtual public PortableServer::RefCountServantBase,
        virtual public InterBaseLobBase
{
public:
    InterBaseWclob(PortableServer::Current_ptr current)
            : InterBaseLobBase(current) {}
    CORBA::ULong length() {
        return InterBaseLobBase::length();
    }
    CORBA::WChar* fetch_chunk(CORBA::ULong chunkSize
                , CORBA::Boolean& more) {
        throw UAKGQuery2::WriteOnlyLOB();
    }
    void put_chunk(const CORBA::WChar* data);
};


#endif
