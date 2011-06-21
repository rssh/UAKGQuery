#include <InterBase/InterBaseLobs.h>

#include <climits>
using namespace UAKGQuery2;


void InterBaseLobBase::fill_properties(ISC_QUAD& id, InterBaseQuery*& q)
{
    PortableServer::ObjectId_var oid = current_->get_object_id();
    char* id_p = reinterpret_cast<char*>(&id);
    unsigned long i;
    for (i = 0; i < sizeof(ISC_QUAD); ++i) {
        id_p[i] = oid[i];
    }
    char* q_p = reinterpret_cast<char*>(&q);
    for (unsigned long j = 0; j < sizeof(InterBaseQuery*); ++j) {
        q_p[j] = oid[i + j];
    };
}

PortableServer::ObjectId* InterBaseLobBase::id()
{
    return current_->get_object_id();
}

//
// For reading (result set)
//

CORBA::ULong InterBaseLobBase::length()
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    return q->get_lob_length(id);
}

UAKGQuery2::OctSeq*
InterBaseBlobForReading::fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more)
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    unsigned long actual_length;
    char* data = q->read_lob_chunk(id, chunkSize, actual_length, more
            , TypeBlob);
    if (data == NULL) return new UAKGQuery2::OctSeq();
    UAKGQuery2::OctSeq_var res = new UAKGQuery2::OctSeq(
            actual_length, actual_length, reinterpret_cast<unsigned char*>(data)
            , 1);
    return res._retn();
}

char*
InterBaseClobForReading::fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more)
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    unsigned long actual_length;
    char* data = q->read_lob_chunk(id, chunkSize, actual_length, more
            , TypeClob);
    if (data == NULL) return CORBA::string_dup("");
    return data;
}

CORBA::WChar*
InterBaseWclobForReading::fetch_chunk(CORBA::ULong chunkSize, CORBA::Boolean& more)
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    unsigned long actual_length;
    CORBA::WChar* data = (CORBA::WChar*)q->read_lob_chunk(id, chunkSize*sizeof(CORBA::WChar)
            , actual_length, more, TypeWclob);
    if (data == NULL) return CORBA::wstring_dup((const CORBA::WChar*)L"");
    return data;
}


//
// For writing (query parameters)
//

void
InterBaseBlob::put_chunk(const UAKGQuery2::OctSeq& data)
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    q->write_lob_chunk(id, reinterpret_cast<const char*>(data.get_buffer())
            , data.length());
}

void
InterBaseClob::put_chunk(const char* data)
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    q->write_lob_chunk(id, data, strlen(data));
}

namespace {
    unsigned long get_wchar_byte_length(const CORBA::WChar* data) {
        const CORBA::WChar* p = data; 
        for ( ; *p != 0 && ((p - data) < USHRT_MAX); ++p);
        return p - data;
    }
}

void
InterBaseWclob::put_chunk(const CORBA::WChar* data)
{
    ISC_QUAD id;
    InterBaseQuery* q;
    fill_properties(id, q);
    q->write_lob_chunk(id, (char*)data, get_wchar_byte_length(data));
}
