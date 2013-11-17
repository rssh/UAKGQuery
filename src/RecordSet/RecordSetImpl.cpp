#include <include/CosQueryPostConfig.h>
#include CORBA_H
#include <RecordSet/RecordSetImpl.h>
#include <Common/UAKGQueryUtils.h>
#include <UAKGQuerySingleton.h>
#include <UAKGQuerySingleton.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <strstream>

#define HAVE_WCHAR_H
// without configure, becouse we think that alread exists
#include <wchar.h>


using namespace std;
using namespace UAKGQuery2;

namespace {

static void enlarge(CORBA::ULong nOctets, 
         CORBA::ULong pos, 
         CORBA::OctetSeq& octSeq)
{
  CORBA::ULong n = octSeq.length();
  CORBA::ULong req_n = pos+nOctets;
  int changeFlag = 0;
  while (req_n > n) {
     if (n==0) n=req_n;
     else if (n < 4000) n*=2;
     else n+=nOctets;
     changeFlag = 1;
  }
  if (changeFlag) octSeq.length(n);
  // is this possible without saving previous state ?
  // TODO: read standart/
}

static void writeOctet(CORBA::Octet value, 
        CORBA::ULong& pos, 
        CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteOctet=" << pos << endl;
#endif
   enlarge(1,pos,octSeq);
   octSeq[pos++]=value;
}

static void writeShort(CORBA::Short value, 
        CORBA::ULong& pos, 
        CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteShort=" << pos << endl;
#endif
   enlarge(2,pos,octSeq);
   octSeq[pos++] = (value & 0xff);
   octSeq[pos++] = (value >> 8);
}

static void writeLong( CORBA::Long value, 
        CORBA::ULong& pos, 
        CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteLong=" << pos << endl;
#endif
  enlarge(4,pos,octSeq);
  octSeq[pos++] = (value & 0xff);
  octSeq[pos++] = (value >> 8);
  octSeq[pos++] = (value >> 16);
  octSeq[pos++] = (value >> 24);
}

static void writeLongLong(CORBA::LongLong value, 
                          CORBA::ULong& pos, 
                          CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteLongLong=" << pos << endl;
#endif
  enlarge(8,pos,octSeq);
  octSeq[pos++] = (value & 0xff);
  octSeq[pos++] = (value >> 8);
  octSeq[pos++] = (value >> 16);
  octSeq[pos++] = (value >> 24);
  octSeq[pos++] = (value >> 32);
  octSeq[pos++] = (value >> 40);
  octSeq[pos++] = (value >> 48);
  octSeq[pos++] = (value >> 56);
}



static void writeFloat( CORBA::Float value, 
            CORBA::ULong& pos, 
            CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteFloat=" << pos << endl;
#endif
  //CORBA::Long* ua = (CORBA::Long*)(&value);
  CORBA::Long ua;
  memcpy(&ua,&value,sizeof(CORBA::Float));
  writeLong(ua,pos,octSeq);
}



static void writeDouble(CORBA::Double value, 
            CORBA::ULong& pos, 
            CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteDouble=" << pos << endl;
#endif
#if SIZEOF_DOUBLE == 8
  char cv[8];
  double d = value;
  memcpy(cv,&value,SIZEOF_DOUBLE);
#else
#error SIZEOF_DOUBLE is not 8, please call UAKGQuery support team.
#endif
#ifdef SWAP
  enlarge(8,pos,octSeq);
  octSeq[pos++] = cv[7];
  octSeq[pos++] = cv[6];
  octSeq[pos++] = cv[5];
  octSeq[pos++] = cv[4];
  octSeq[pos++] = cv[3];
  octSeq[pos++] = cv[2];
  octSeq[pos++] = cv[1];
  octSeq[pos++] = cv[0];
#else
  enlarge(8,pos,octSeq);
  octSeq[pos++] = cv[0];
  octSeq[pos++] = cv[1];
  octSeq[pos++] = cv[2];
  octSeq[pos++] = cv[3];
  octSeq[pos++] = cv[4];
  octSeq[pos++] = cv[5];
  octSeq[pos++] = cv[6];
  octSeq[pos++] = cv[7];
#endif
}



static void writeString(const char* value, 
            CORBA::ULong& pos, 
            CORBA::OctetSeq& octSeq)
{
  CORBA::ULong n = strlen(value);
#ifdef PRINT_POSITION
  cout << "WriteString=" << pos << endl;
  cout << "StringLength="  << n << endl;
#endif
  enlarge(n+4,pos,octSeq);
  writeLong(n,pos,octSeq);
  strncpy((char*)(octSeq.get_buffer()+pos),value,n);
  pos+=n;
}


static void writeWString(const CORBA::WChar* value,
             CORBA::ULong& pos, 
             CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteWString=" << pos << endl;
#endif
  if (RecordSetFactoryImpl::locale==NULL) {
     RecordSetFactoryImpl::lazyInitLocale();
  }
  CORBA::ULong n = wcslen(value)*(sizeof(CORBA::WChar));
#ifdef PRINT_POSITION
  cout << "n=" << n << endl;
#endif
  enlarge(n+4,pos,octSeq);
  writeLong(n,pos,octSeq);
  memcpy((octSeq.get_buffer()+pos),value,n);
  pos+=n;
}

static void writeDateTime(const DateTime& value, 
              CORBA::ULong& pos,
                          CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteDateTime=" << pos << endl;
#endif
  writeShort(value.year,pos,octSeq);
  writeOctet(value.month,pos,octSeq);
  writeOctet(value.day,pos,octSeq);
  writeOctet(value.hour,pos,octSeq);
  writeOctet(value.minute,pos,octSeq);
  writeOctet(value.second,pos,octSeq);
}

static void writeRawBuffer(CORBA::ULong bufferLength,const CORBA::Octet* buffer,
                           CORBA::ULong& pos,
                           CORBA::OctetSeq& octSeq)
{
  enlarge(bufferLength+4,pos,octSeq);
  writeLong(bufferLength,pos,octSeq);
  memcpy(octSeq.get_buffer()+pos,buffer,bufferLength);
  pos += bufferLength;
}

static void writeRaw(const OctSeq* value, 
             CORBA::ULong& pos, 
             CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteRaw=" << pos << endl;
#endif
  writeRawBuffer(value->length(),value->get_buffer(),pos,octSeq);
}

static void writeNumeric(const Numeric* dec, 
             CORBA::ULong& pos,
             CORBA::OctetSeq& octSeq)
{
  Numeric* num = const_cast<Numeric*>(dec);
#ifdef PRINT_POSITION
  cout << "WriteNumeric=" << pos << endl;
  cout << "WriteNumeric Length=" << dec->value().length() << endl;
  cout << "|" << num << "|" << endl;
#endif
  writeLong(dec->precision(),pos,octSeq);
  writeLong(dec->scale(),pos,octSeq);
  writeRawBuffer(dec->value().length(),
         dec->value().get_buffer(),pos,octSeq);
}

static void writeObject(  CORBA::Object_ptr value,
                          CORBA::ULong& pos,
                          CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "WriteObject=" << pos << endl;
#endif
  CORBA::String_var s = UAKGQuerySingleton::getORB_ptr()->object_to_string(value);
  writeString(s,pos,octSeq);
}

// Read 

static CORBA::Boolean readBoolean(CORBA::ULong& pos,
                                  const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "ReadOctet=" << pos << endl;
#endif
  return octSeq[pos++] ? 1 : 0 ;
}

static CORBA::Char readChar(CORBA::ULong& pos,
                            const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "ReadOctet=" << pos << endl;
#endif
  return (CORBA::Char)octSeq[pos++];
}

static CORBA::Octet readOctet(CORBA::ULong& pos,
                              const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "ReadOctet=" << pos << endl;
#endif
  return octSeq[pos++];
}

static CORBA::Short readShort(CORBA::ULong& pos,
                              const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadShort=" << pos << endl;
#endif
 const CORBA::Octet* cur = octSeq.get_buffer()+pos;
 CORBA::Short retval = *cur++;
 retval += ((*cur)<<8);
 pos+=2;
 return retval;
}

static CORBA::Long readLong(CORBA::ULong& pos,
                            const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadLong=" << pos << endl;
#endif
 const CORBA::Octet* cur = octSeq.get_buffer()+pos;
 CORBA::Long retval = cur[0]+(cur[1]<<8)+(cur[2]<<16)+(cur[3]<<24);
 pos+=4;
 return retval;
}

static CORBA::LongLong readLongLong(CORBA::ULong& pos,
                            const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadLongLong:" << pos << endl;
#endif
 const CORBA::Octet* cur = octSeq.get_buffer()+pos;
 CORBA::LongLong b1 = cur[0]+(cur[1]<<8)+(cur[2]<<16)+(cur[3]<<24);
 CORBA::LongLong b2 = cur[4]+(cur[5]<<8)+(cur[6]<<16)+(cur[7]<<24);
 CORBA::LongLong retval = b1 + (b2 << 32);
 pos+=8;
 return retval;
}


static CORBA::Float readFloat(CORBA::ULong& pos,
                              const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
   cout << "ReadFloat=" << pos << endl;
#endif
   CORBA::Long  lretval = readLong(pos,octSeq);
   CORBA::Float fretval;
   memcpy(&fretval,&lretval,sizeof(CORBA::Float));
   return fretval;
}

static CORBA::Double readDouble(CORBA::ULong& pos,
                                    const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadDouble=" << pos << endl;
#endif
 char cv[8];
 const CORBA::Octet* cur = octSeq.get_buffer()+pos;
#ifdef SWAP
 cv[7]=*cur++;
 cv[6]=*cur++;
 cv[5]=*cur++;
 cv[4]=*cur++;
 cv[3]=*cur++;
 cv[2]=*cur++;
 cv[1]=*cur++;
 cv[0]=*cur++;
#else
 cv[0]=*cur++;
 cv[1]=*cur++;
 cv[2]=*cur++;
 cv[3]=*cur++;
 cv[4]=*cur++;
 cv[5]=*cur++;
 cv[6]=*cur++;
 cv[7]=*cur++;
#endif
 double d ; //* (reinterpret_cast<double*>(cv));
 memcpy(&d,cv,sizeof(double)); 
 CORBA::Double retval = d;
 pos+=8;
 return retval;
}

static char* readString(CORBA::ULong& pos,
                        const CORBA::OctetSeq& octSeq)
{
 CORBA::ULong len = readLong(pos,octSeq);
#ifdef PRINT_POSITION
 cout << "ReadString=" << pos << endl;
 cout << "StringLength=" << len << endl;
#endif
 char* retval = CORBA::string_alloc(len+1);// was new char[]
 strncpy(retval,(char*)(octSeq.get_buffer()+pos),len);
 retval[len]='\0';
 pos+=len;
 return retval;
}

static CORBA::WChar* readWString(CORBA::ULong& pos,
                                 const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadWString=" << pos << endl;
#endif
 CORBA::ULong char_len = readLong(pos,octSeq);
#ifdef PRINT_POSITION
 cout << "char_len=" << char_len << endl;
#endif
 CORBA::ULong wchar_len = char_len/sizeof(CORBA::WChar);
 CORBA::WChar* retval = CORBA::wstring_alloc(wchar_len+1);
 memcpy(retval,(octSeq.get_buffer()+pos),char_len);
 retval[wchar_len] = L'\0';
 pos+=char_len;
 return retval;
}

static OctSeq* readRaw(CORBA::ULong& pos,
                       const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "ReadRaw=" << pos << endl;
#endif
  CORBA::ULong n = readLong(pos,octSeq);
  OctSeq_var retval = new OctSeq();
  retval->length(n);
  memcpy(retval->get_buffer(),octSeq.get_buffer()+pos,n);
  pos+=n;
  return retval._retn();
}

static Numeric* readNumeric(CORBA::ULong& pos,
                            const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
  cout << "ReadNumeric=" << pos << endl;
#endif
  NumericImpl* dec = new NumericImpl();
  dec->precision(readLong(pos,octSeq));
  dec->scale(readLong(pos,octSeq));
  OctSeq_var value=readRaw(pos,octSeq);
#ifdef PRINT_POSITION
  cout << "ReadNumeric Length=" << value->length() << endl;
#endif
#ifndef CORBA_SEQ_HAVE_NO_REPLACE
  CORBA::ULong length=value->length();
  dec->value().replace(length,length,value->get_buffer(1),1);
#else
  dec->value(value);
#endif
  dec->initnDigits();
  Numeric_var num = dec;
  return num._retn();
}

static CORBA::Object_ptr readObject(CORBA::ULong& pos,
                                    const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadObject=" << pos << endl;
#endif
 CORBA::String_var s = readString(pos,octSeq);
 CORBA::Object_var obj = UAKGQuerySingleton::getORB_ptr()->string_to_object(s);
 return obj._retn();
}

static DateTime readDateTime(CORBA::ULong& pos,
                             const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadDateTime=" << pos << endl;
#endif
  DateTime retval;
  retval.year = readShort(pos,octSeq);
  retval.month = readOctet(pos,octSeq);
  retval.day = readOctet(pos,octSeq);
  retval.hour = readOctet(pos,octSeq);
  retval.minute = readOctet(pos,octSeq);
  retval.second = readOctet(pos,octSeq);
  return retval;
}

static Blob* readBlob(CORBA::ULong& pos, const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadObject=" << pos << endl;
#endif
  CORBA::Object_var obj = readObject(pos, octSeq);
  Blob_var bl = Blob::_narrow(obj);
  return bl._retn();
}

static Clob* readClob(CORBA::ULong& pos, const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadObject=" << pos << endl;
#endif
  CORBA::Object_var obj = readObject(pos, octSeq);
  Clob_var cl = Clob::_narrow(obj);
  return cl._retn();
}

static Wclob* readWclob(CORBA::ULong& pos, const CORBA::OctetSeq& octSeq)
{
#ifdef PRINT_POSITION
 cout << "ReadObject=" << pos << endl;
#endif
  CORBA::Object_var obj = readObject(pos, octSeq);
  Wclob_var wcl = Wclob::_narrow(obj);
  return wcl._retn();
}

} // namespace

void Field::setField(const Field& fld)
{
 switch (fld.type_) {
    case TypeNull:     setNull(); break;
    case TypeBoolean:  setBoolean(fld.getBoolean()); break;
    case TypeChar:     setChar(fld.getChar()); break;
    case TypeOctet:    setOctet(fld.getOctet()); break;
    case TypeShort:    setShort(fld.getShort()); break;
    case TypeUShort:   setUShort(fld.getUShort()); break;
    case TypeLong:     setLong(fld.getLong()); break;
    case TypeULong:    setULong(fld.getULong()); break;
    case TypeFloat:    setFloat(fld.getFloat()); break;
    case TypeDouble:   setDouble(fld.getDouble()); break;
    case TypeString:   setString(fld.getString_()); break;
    case TypeNumeric:  setNumeric(fld.getNumeric_()); break;
    case TypeDecimal:  setDecimal(fld.getNumeric_()); break;
    case TypeDateTime: setDateTime(fld.getDateTime()); break;
    case TypeRaw:      setRaw(*fld.getRaw_()); break;
    case TypeWString:  setWString(fld.getWString_()); break;
    case TypeBlob:     setBlob(fld.getBlob_()); break;
    case TypeClob:     setClob(fld.getClob_()); break;
    case TypeWclob:    setWclob(fld.getWclob_()); break;
    case TypeLongLong:    setLongLong(fld.getLongLong()); break;
    case TypeULongLong:   setULongLong(fld.getULongLong()); break;
 }
 type_ = fld.type_;
}

Field::Field(const Field& fld)
{
     type_ = TypeNull;
     setField(fld);
}

void Field::operator=(const Field& fld)
{
     if (this == &fld) return;
     if (! isNull()) destroy();
     setField(fld);
}

void Field::write(CORBA::OctetSeq& seq, CORBA::ULong& pos)
{
     switch (type_) {
    case TypeNull:
        break;
    case TypeBoolean:
        writeOctet(static_cast<CORBA::Octet>(value_.b), pos, seq);
        break;
    case TypeChar:
        writeOctet(static_cast<CORBA::Octet>(value_.c), pos, seq);
        break;
    case TypeOctet:
        writeOctet(value_.o, pos, seq);
        break;
    case TypeShort:
        writeShort(value_.s, pos, seq);
        break;
    case TypeUShort:
        writeShort(value_.us, pos, seq);
        break;
    case TypeLong:
        writeLong(value_.l, pos, seq);
        break;
    case TypeULong:
        writeLong(value_.ul, pos, seq);
        break;
    case TypeFloat:
        writeFloat(value_.f, pos, seq);
        break;
    case TypeDouble:
        writeDouble(value_.d, pos, seq);
        break;
    case TypeString:
        writeString(value_.str, pos, seq);
        break;
    case TypeNumeric:
    case TypeDecimal:
        writeNumeric(value_.num_p, pos, seq);
        break;
    case TypeDateTime:
        writeDateTime(value_.dt, pos, seq);
        break;
    case TypeRaw:
        writeRaw(value_.raw_p, pos, seq);
        break;
    case TypeWString:
        writeWString(value_.wstr, pos, seq);
        break;
    case TypeBlob: 
        writeObject(value_.bl, pos, seq);
        break;
    case TypeClob:
        writeObject(value_.cl, pos, seq);
        break;
    case TypeWclob:
        writeObject(value_.wcl, pos, seq);
        break;
    case TypeLongLong:
        writeLongLong(value_.ll, pos, seq);
        break;
    case TypeULongLong:
        writeLongLong(value_.ull, pos, seq);
        break;
     }
}

void Field::read(CORBA::Octet fieldType,
              CORBA::OctetSeq& seq,
              CORBA::ULong& pos)
{
     if (! isNull()) destroy();
     type_ = static_cast<FieldType>(fieldType);
     switch (type_) {
        case TypeBoolean:
                value_.b = readBoolean(pos, seq);
                break;
        case TypeChar:
                value_.c = readChar(pos, seq);
                break;
        case TypeOctet:
                value_.o = readOctet(pos, seq);
                break;
        case TypeShort:
                value_.s = readShort(pos, seq);
                break;
        case TypeUShort:
                value_.us = readShort(pos, seq);
                break;
        case TypeLong:
                value_.l = readLong(pos, seq);
                break;
        case TypeULong:
                value_.ul = readLong(pos, seq);
                break;
        case TypeFloat:
                value_.f = readFloat(pos, seq);
                break;
        case TypeDouble:
                value_.d = readDouble(pos, seq);
                break;
        case TypeString:
                value_.str = readString(pos, seq);
                break;
        case TypeNumeric:
        case TypeDecimal:
                value_.num_p = readNumeric(pos, seq);
                break;
        case TypeDateTime:
                value_.dt = readDateTime(pos, seq);
                break;
        case TypeRaw:
                value_.raw_p = readRaw(pos, seq);
                break;
        case TypeWString:
                value_.wstr = readWString(pos, seq);
                break;
        case TypeBlob:
                value_.bl = readBlob(pos, seq);
                break;
        case TypeClob:
                value_.cl = readClob(pos, seq);
                break;
        case TypeWclob:
                value_.wcl = readWclob(pos, seq);
                break;
        case TypeLongLong:
                value_.ll = readLongLong(pos, seq);
                break;
        case TypeULongLong:
                value_.ull = readLongLong(pos, seq);
                break;
     }
}


void Field::destroy()
{
    switch(type_) {
    case TypeString:
         if(value_.str != NULL){
             CORBA::string_free(value_.str);
             value_.str = NULL;
         }
         break;
    case TypeNumeric: 
    case TypeDecimal:
         if (value_.num_p!=NULL)
            value_.num_p ->_remove_ref();
         break;
    case TypeRaw:
         if (value_.raw_p!=NULL)
             value_.raw_p->release();
         break;
    case TypeWString:
         if(value_.wstr != NULL){
             CORBA::wstring_free(value_.wstr);
             value_.wstr = NULL;
         }
         break;
    case TypeBlob:
         CORBA::release(value_.bl);
         break;
    case TypeClob:
         CORBA::release(value_.cl);
         break;
    case TypeWclob:
         CORBA::release(value_.wcl);
         break;
    }
    type_ = TypeNull;
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet:1.0
//
RecordSetImpl::RecordSetImpl()
{
}

RecordSetImpl::~RecordSetImpl()
{
}

CORBA::ValueBase*
RecordSetImpl::_copy_value()
{
    RecordSetImpl* fr = new RecordSetImpl();
    fr->tbl_ = tbl_;
    fr->nc_ = nc_;
    fr->cn_ = cn_;
    fr->ct_ = ct_;
    fr->sz_ = sz_;
    fr->pr_ = pr_;
    fr->sc_ = sc_;
    return fr;
}

void
RecordSetImpl::marshal(CORBA::DataOutputStream* os)
{
    CORBA::OctetSeq seq;
    CORBA::ULong pos = 0;
    CORBA::ULong length;
    for (ColumnNames::const_iterator i = cn_.begin(); i != cn_.end(); ++i) {
        length = (*i).length();
        if (length > 0) {
            seq.length(pos + length);
            CORBA::Octet* cur = seq.get_buffer(0);
            memcpy(cur + pos, (*i).c_str(), length);
            pos += length;
        }
        seq.length(++pos);
        seq[pos - 1] = 0;
    }
    CORBA::ULong u = 0;
    for(u = 0; u < getNColumns(); ++u)
    {
        writeOctet(ct_[u],pos,seq); 
    }
    for(u = 0; u < getNColumns(); ++u)
    {
        writeLong(sz_[u],pos,seq); 
    }
    for(u = 0; u < getNColumns(); ++u)
    {   
        writeLong(pr_[u],pos,seq);
    }
    for(u = 0; u < getNColumns(); ++u)
    {   
        writeShort(sc_[u],pos,seq);
    }
    CORBA::ULong nullBitArrayLength = getNRows()*getNColumns();
    CORBA::ULong nullBitPackedLength = nullBitArrayLength/8 + 1;
    //pos = seq.length();
    seq.length(pos + nullBitPackedLength);
    CORBA::ULong posNullBitArray = pos;
    seq[posNullBitArray] = 0;
    pos += nullBitPackedLength;
    CORBA::ULong l = 0;
    CORBA::ULong shift;
    for (CORBA::ULong j = 0; j < getNRows() ; ++j) {
        for (CORBA::ULong k = 0 ; k < getNColumns(); ++k, ++l) {
            shift = l%8;
            if (l != 0 && shift == 0) {
                seq[++posNullBitArray] = 0;
            }
            if (! tbl_[j][k].isNull()) {
                tbl_[j][k].write(seq, pos);
            } else {
                CORBA::Octet mask = 1;
                mask = mask << shift;
                seq[posNullBitArray] |= mask;
            }
        }
    }
    seq.length(pos+1);
    os->write_ulong(seq.length());
    os->write_ulong(getNRows());
    os->write_ulong(getNColumns());
    os->write_octet_array(seq, 0, seq.length());
}

#if defined(MICO) && MICO_BIN_VERSION < 0x02030b
CORBA::ValueBase* 
#else
void
#endif
             RecordSetImpl::unmarshal(CORBA::DataInputStream* is)
{
    CORBA::ULong length = is->read_ulong();
    CORBA::ULong nRows = is->read_ulong();
    CORBA::ULong nColumns = is->read_ulong();
    CORBA::OctetSeq seq;
    seq.length(length);
    is->read_octet_array(seq, 0, length);

    CORBA::ULong pos = 0;
    CORBA::Octet* cur = seq.get_buffer(0);
    for (CORBA::ULong i = 0; i < nColumns; ++i) {
        length = strlen((char*)cur + pos);
        nc_[(char*)cur + pos] = i;
        cn_.push_back((char*) cur + pos );
        pos += length + 1;
    }

    CORBA::ULong t = 0;

    for (t = 0; t < nColumns; ++t) {
        //ct_.push_back((FieldType) seq[pos++]);
        ct_.push_back((FieldType)  readOctet(pos,seq));
    }
    for (t = 0; t < nColumns; ++t) {
        sz_.push_back( readLong(pos,seq) );
    }
    for (t = 0; t < nColumns; ++t) {
        pr_.push_back( readLong(pos,seq) );
    }

    for (t = 0; t < nColumns; ++t) {
        sc_.push_back( readShort(pos,seq) );
    }

    CORBA::ULong posNullBitArray = pos;
    CORBA::ULong nullBitArrayLength = nRows * nColumns;
    CORBA::ULong nullBitPackedLength = nullBitArrayLength/8 + 1;
    pos += nullBitPackedLength;
    CORBA::ULong l = 0;
    CORBA::ULong shift;
    CORBA::Octet mask;
    for (CORBA::ULong j = 0; j < nRows; ++j) {
    addRow();
        for (CORBA::ULong k = 0; k < nColumns; ++k, ++l) {
            shift = l%8;
            if (l != 0 && shift == 0) {
                ++posNullBitArray;
            }
            mask = 1;
            mask = mask << shift;
            if (seq[posNullBitArray] & mask) {
                tbl_[j][k].setNull();
            } else {
                //tbl_[j][k].read(seq[pos+k], seq, pos);
                tbl_[j][k].read(ct_[k], seq, pos);
            }
        }
    }
#if defined(MICO) && MICO_BIN_VERSION < 0x02030b
	return this;
#endif
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getNRows:1.0
//
CORBA::ULong
RecordSetImpl::getNRows() const
{
    return (CORBA::ULong)tbl_.size();
}

CORBA::ULong
RecordSetImpl::getNRows()
{
    return (CORBA::ULong)tbl_.size();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getNColumns:1.0
//
CORBA::ULong
RecordSetImpl::getNColumns()
{
    return (CORBA::ULong)cn_.size();
}

CORBA::ULong
RecordSetImpl::getNColumns() const
{
    return (CORBA::ULong)cn_.size();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/addRow:1.0
//
void
RecordSetImpl::addRow()
{
    Column elem(getNColumns());
    tbl_.push_back(elem);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/addColumn:1.0
//
void
RecordSetImpl::addColumn(const char* name, FieldType ft)
{
    char *lwrname = new char[strlen(name)+1];
    for (int j=0;j<=strlen(name);j++)
       lwrname[j]=tolower(name[j]);
    Field fl;
    NameColumns::const_iterator it = nc_.find(lwrname);
    if (it != nc_.end()) throw NameInUse();
    CORBA::ULong length = getNColumns();
    for (CORBA::ULong i = 0; i < getNRows(); ++i) {
        tbl_[i].push_back(fl);
    }
    nc_[lwrname] = length;
    //cn_[length] = string(name);
    cn_.push_back(lwrname);
    //ct_[length] = ft;
    ct_.push_back(ft);
    sz_.push_back(0); pr_.push_back(0); sc_.push_back(0);
    delete [] lwrname;
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setFieldSize:1.0
//
void
RecordSetImpl::setFieldSizeAt(CORBA::ULong col, CORBA::ULong sz)
{
    if ( ! haveFieldSizeAt(col) )
        throw InvalidFieldType();
    sz_[col] = sz;
}

CORBA::Boolean 
RecordSetImpl::haveFieldSizeAt(CORBA::ULong col)
{
    checkColumn(col);
    switch (ct_[col]) {
      case TypeString:
      case TypeWString:
      case TypeNumeric:
      case TypeDecimal:
      case TypeRaw:
         return true;
    }
    return false;
}

CORBA::Boolean 
RecordSetImpl::haveFieldSizeAt(CORBA::ULong col) const
{
    checkColumn(col);
    switch (ct_[col]) {
      case TypeString:
      case TypeWString:
      case TypeNumeric:
      case TypeDecimal:
      case TypeRaw:
         return true;
    }
    return false;
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getFieldSizeAt:1.0
//

CORBA::ULong
RecordSetImpl::getFieldSizeAt(CORBA::ULong col)
{
    if ( ! haveFieldSizeAt(col) )
        throw InvalidFieldType();
    
    return sz_[col];
}
 
CORBA::ULong
RecordSetImpl::getFieldSizeAt(CORBA::ULong col) const
{
    if ( ! haveFieldSizeAt(col) )
        throw InvalidFieldType();
    
    return sz_[col];
}


UAKGQuery2::FieldType
RecordSetImpl::getFieldTypeAt(CORBA::ULong col)
{
    checkColumn(col);
    return ct_[col];
}

UAKGQuery2::FieldType
RecordSetImpl::getFieldTypeAt(CORBA::ULong col) const
{
    checkColumn(col);
    return ct_[col];
}

CORBA::Boolean 
RecordSetImpl::haveNumericFieldPropertiesAt(CORBA::ULong col) const
{
    checkColumn(col);
    return ((ct_[col] == TypeNumeric) || (ct_[col] == TypeDecimal));
}

CORBA::Boolean 
RecordSetImpl::haveNumericFieldPropertiesAt(CORBA::ULong col)
{
    checkColumn(col);
    return ((ct_[col] == TypeNumeric) || (ct_[col] == TypeDecimal));
}

void
RecordSetImpl::setNumericFieldPropertiesAt(CORBA::ULong col,
                                         CORBA::UShort precision,
                                         CORBA::UShort scale)
{
    checkColumn(col);
    //isNotType(col,TypeNumeric)
    pr_[col] = precision;
    sc_[col] = scale;
}

CORBA::UShort RecordSetImpl::getNumericPrecisionAt(CORBA::ULong col)
{
    checkColumn(col);
    if (ct_[col] != TypeNumeric) throw InvalidFieldType();
    return pr_[col];
}

CORBA::UShort RecordSetImpl::getNumericPrecisionAt(CORBA::ULong col) const
{
    checkColumn(col);
    if (ct_[col] != TypeNumeric) throw InvalidFieldType();
    return pr_[col];
}

CORBA::UShort RecordSetImpl::getNumericScaleAt(CORBA::ULong col)
{
    checkColumn(col);
    if (ct_[col] != TypeNumeric) throw InvalidFieldType();
    return sc_[col];
}

CORBA::UShort RecordSetImpl::getNumericScaleAt(CORBA::ULong col) const
{
    checkColumn(col);
    if (ct_[col] != TypeNumeric) throw InvalidFieldType();
    return sc_[col];
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/dropRow:1.0
//
void
RecordSetImpl::dropRow(CORBA::ULong row)
{
    checkRow(row);
    tbl_.erase( tbl_.begin() + row );
}

void
RecordSetImpl::dropAllRows()
{
    tbl_.erase( tbl_.begin(), tbl_.end()  );
}


//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/dropColumn:1.0
//
void
RecordSetImpl::dropColumn(CORBA::ULong col)
{
    checkColumn(col);
    for (CORBA::ULong i = 0; i < getNRows(); ++i) {
        tbl_[i].erase( tbl_[i].begin() + col );
    }
    std::string name = cn_[col];
    nc_.erase(name);
    cn_.erase(cn_.begin() + col);
    ct_.erase(ct_.begin() + col);
    sz_.erase(sz_.begin() + col);
    pr_.erase(pr_.begin() + col);
    sc_.erase(sc_.begin() + col);
}

void
RecordSetImpl::dropColumns(CORBA::ULong frs, CORBA::ULong snd)
{
    checkColumn(frs);
    checkColumn(snd-1);
    for (CORBA::ULong i = 0; i < getNRows(); ++i) {
        tbl_[i].erase( tbl_[i].begin() + frs, tbl_[i].begin()+snd );
    }
    for(CORBA::ULong col=frs; col<snd; ++col) {
      std::string name = cn_[col];
      nc_.erase(name);
    }

    cn_.erase(cn_.begin() + frs, cn_.begin()+snd );
    ct_.erase(ct_.begin() + frs, ct_.begin()+snd );
    sz_.erase(sz_.begin() + frs, sz_.begin()+snd );
    pr_.erase(pr_.begin() + frs, pr_.begin()+snd );
    sc_.erase(sc_.begin() + frs, sc_.begin()+snd );
}



//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/isNullAt:1.0
//
CORBA::Boolean
RecordSetImpl::isNullAt(CORBA::ULong row,
                        CORBA::ULong col)
{
    checkCell(row,col);
    return tbl_[row][col].isNull();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setNullAt:1.0
//
void
RecordSetImpl::setNullAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    tbl_[row][col].setNull();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getBooleanAt:1.0
//
CORBA::Boolean
RecordSetImpl::getBooleanAt(CORBA::ULong row,
                            CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeBoolean) ) throw InvalidFieldType();
    return tbl_[row][col].getBoolean();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setBooleanAt:1.0
//
void
RecordSetImpl::setBooleanAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::Boolean v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeBoolean) ) throw InvalidFieldType();
    tbl_[row][col].setBoolean(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getCharAt:1.0
//
CORBA::Char
RecordSetImpl::getCharAt(CORBA::ULong row,
                                     CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeChar) && 
         isNotType(col,TypeCharacter)  ) throw InvalidFieldType();
    return tbl_[row][col].getChar();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setCharAt:1.0
//
void
RecordSetImpl::setCharAt(CORBA::ULong row,
                                     CORBA::ULong col,
                                     CORBA::Char v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeChar) && 
         isNotType(col,TypeCharacter)  ) throw InvalidFieldType();
    tbl_[row][col].setChar(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getOctetAt:1.0
//
CORBA::Octet
RecordSetImpl::getOctetAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeOctet) ) throw InvalidFieldType();
    return tbl_[row][col].getOctet();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setOctetAt:1.0
//
void
RecordSetImpl::setOctetAt(CORBA::ULong row, CORBA::ULong col,
                          CORBA::Octet v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeOctet) ) throw InvalidFieldType();
    tbl_[row][col].setOctet(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getShortAt:1.0
//
CORBA::Short
RecordSetImpl::getShortAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeShort)  &&
         isNotType(col,TypeUShort) &&
         isNotType(col,TypeSmallInt) )  throw InvalidFieldType();
    return tbl_[row][col].getShort();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setShortAt:1.0
//
void
RecordSetImpl::setShortAt(CORBA::ULong row, CORBA::ULong col,
                          CORBA::Short v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeShort)  &&
         isNotType(col,TypeUShort) &&
         isNotType(col,TypeSmallInt) )  throw InvalidFieldType();
    tbl_[row][col].setShort(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getUShortAt:1.0
//
CORBA::UShort
RecordSetImpl::getUShortAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeShort)  &&
         isNotType(col,TypeUShort) &&
         isNotType(col,TypeSmallInt) )  throw InvalidFieldType();
    return tbl_[row][col].getUShort();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setUShortAt:1.0
//
void
RecordSetImpl::setUShortAt(CORBA::ULong row, CORBA::ULong col,
                           CORBA::UShort v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeShort)  &&
         isNotType(col,TypeUShort) &&
         isNotType(col,TypeSmallInt) )  throw InvalidFieldType();
    tbl_[row][col].setShort(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getLongAt:1.0
//
CORBA::Long
RecordSetImpl::getLongAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if (   isType(col,TypeLongLong) 
        || isType(col,TypeULongLong)) {
      CORBA::LongLong ll = tbl_[row][col].getLongLong();   
      if ((ll & 0xFFFFFFFF) == ll) {
        return (CORBA::Long)(ll & 0xFFFFFFFF);
      } else {
        //  value is too big.
        //  TODO: think about better diagnostics.
        throw InvalidFieldType();
      }
    } else if ( isNotType(col,TypeLong)  &&
                isNotType(col,TypeULong) &&
                isNotType(col,TypeInteger) )  {
       throw InvalidFieldType();
    } 
    return tbl_[row][col].getLong();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setLongAt:1.0
//
void
RecordSetImpl::setLongAt(CORBA::ULong row, CORBA::ULong col,
                         CORBA::Long v)
{
    checkCell(row,col); 
    if (  isType(col,TypeLongLong) 
        || isType(col,TypeULongLong)) {
          tbl_[row][col].setLongLong(v);
    } else if ( isNotType(col,TypeLong)  &&
         isNotType(col,TypeULong) &&
         isNotType(col,TypeInteger) )  {
      throw InvalidFieldType();
    } else {
      tbl_[row][col].setLong(v);
    }
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getULongAt:1.0
//
CORBA::ULong
RecordSetImpl::getULongAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isType(col,TypeLongLong) ||
         isType(col,TypeULongLong) ) {
        if ( tbl_[row][col].getULongLong() < 0xFFFFFFFFUL ) {
            return tbl_[row][col].getULongLong() & 0xFFFFFFFF;
        } else {
           throw InvalidFieldType();
        }
    } else if ( isType(col,TypeLong) ||
         isType(col,TypeULong) ||
         isType(col,TypeInteger) ) {
        return tbl_[row][col].getULong();
    } else {
         throw InvalidFieldType();
    }
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setULongAt:1.0
//
void
RecordSetImpl::setULongAt(CORBA::ULong row, CORBA::ULong col,
                          CORBA::ULong v)
{
    checkCell(row,col); 
    if ( isType(col,TypeLong)  ||
         isType(col,TypeULong) ||
         isType(col,TypeInteger) )  {
       tbl_[row][col].setULong(v);
    } else if (isType(col,TypeLongLong) ||
               isType(col,TypeULongLong) ) {
       tbl_[row][col].setULongLong(v);
    } else {
      throw InvalidFieldType();
    }
}





//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getLongLongAt:1.0
//
CORBA::LongLong
RecordSetImpl::getLongLongAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isType(col,TypeLongLong) ||
         isType(col,TypeULongLong) ) {
       return tbl_[row][col].getLongLong();
    } else if (isType(col,TypeLong) ||
               isType(col,TypeULong) ||
               isType(col,TypeInteger) ) {
       return tbl_[row][col].getLong();
    } else {
       throw InvalidFieldType();
    }
}


//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setLongLongAt:1.0
//
void
RecordSetImpl::setLongLongAt(CORBA::ULong row, CORBA::ULong col,
                         CORBA::LongLong v)
{
    checkCell(row,col); 
    if (   isType(col,TypeLongLong) 
        || isType(col,TypeULongLong)) {
      tbl_[row][col].setLongLong(v);
    } else {
      throw InvalidFieldType();
    }
}


//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getLongLongAt:1.0
//
CORBA::ULongLong
RecordSetImpl::getULongLongAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isType(col,TypeLongLong) ||
         isType(col,TypeULongLong) ) {
       return tbl_[row][col].getULongLong();
    } else if (isType(col,TypeLong) ||
               isType(col,TypeULong) ||
               isType(col,TypeInteger) ) {
       return tbl_[row][col].getULong();
    } else {
       throw InvalidFieldType();
    }
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setULongLongAt:1.0
//
void
RecordSetImpl::setULongLongAt(CORBA::ULong row, CORBA::ULong col,
                         CORBA::ULongLong v)
{
    checkCell(row,col); 
    if (   isType(col,TypeLongLong) 
        || isType(col,TypeULongLong)) {
      tbl_[row][col].setULongLong(v);
    } else {
      throw InvalidFieldType();
    }
}


//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getFloatAt:1.0
//
CORBA::Float
RecordSetImpl::getFloatAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeFloat) && 
         isNotType(col,TypeReal)  ) throw InvalidFieldType();
    return tbl_[row][col].getFloat();
}


//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setFloatAt:1.0
//
void
RecordSetImpl::setFloatAt(CORBA::ULong row, CORBA::ULong col,
                          CORBA::Float v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeFloat) && 
         isNotType(col,TypeReal)  ) throw InvalidFieldType();
    tbl_[row][col].setFloat(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getDoubleAt:1.0
//
CORBA::Double
RecordSetImpl::getDoubleAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeDouble) && 
         isNotType(col,TypeDoublePrecision) ) throw InvalidFieldType();
    return tbl_[row][col].getDouble();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setDoubleAt:1.0
//
void
RecordSetImpl::setDoubleAt(CORBA::ULong row, CORBA::ULong col,
                           CORBA::Double v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeDouble) &&
         isNotType(col,TypeDoublePrecision) ) throw InvalidFieldType();
    tbl_[row][col].setDouble(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getStringAt:1.0
//
char*
RecordSetImpl::getStringAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col); 
    checkNull(row,col);
    if ( isNotType(col,TypeString) ) throw InvalidFieldType();
    return tbl_[row][col].getString();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setStringAt:1.0
//
void
RecordSetImpl::setStringAt(CORBA::ULong row, CORBA::ULong col,
                           const char* v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeString) ) {
      throw InvalidFieldType();
    }
    if (getFieldSizeAt(col)==0) throw FieldSizeIsNotSet();
    tbl_[row][col].setString(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setStringWithSizeAt:1.0
//
void
RecordSetImpl::setStringWithSizeAt(CORBA::ULong row, CORBA::ULong col,
                                   const char* v)
{
    checkCell(row,col); 
    if ( isNotType(col,TypeString) ) throw InvalidFieldType();
    int vlen=strlen(v);
    if (getFieldSizeAt(col)<vlen) {
        setFieldSizeAt(col,vlen);
    }
    tbl_[row][col].setString(v);
}


//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getNumericAt:1.0
//
Numeric*
RecordSetImpl::getNumericAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeNumeric) &&
         isNotType(col,TypeDecimal) ) throw InvalidFieldType();
    return tbl_[row][col].getNumeric();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setNumericAt:1.0
//
void
RecordSetImpl::setNumericAt(CORBA::ULong row, CORBA::ULong col,
                            Numeric* v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeNumeric) &&
         isNotType(col,TypeDecimal) ) throw InvalidFieldType();
    tbl_[row][col].setNumeric(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getDateTimeAt:1.0
//
DateTime
RecordSetImpl::getDateTimeAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeDateTime) ) throw InvalidFieldType();
    return tbl_[row][col].getDateTime();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setDateTimeAt:1.0
//
void
RecordSetImpl::setDateTimeAt(CORBA::ULong row, CORBA::ULong col,
                             const DateTime& v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeDateTime) ) throw InvalidFieldType();
    tbl_[row][col].setDateTime(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getRawAt:1.0
//
OctSeq*
RecordSetImpl::getRawAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeRaw) ) throw InvalidFieldType();
    return tbl_[row][col].getRaw();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setRawAt:1.0
//
void
RecordSetImpl::setRawAt(CORBA::ULong row, CORBA::ULong col,
                        const OctSeq& v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeRaw) ) throw InvalidFieldType();
    tbl_[row][col].setRaw(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setRawAt:1.0
//
void
RecordSetImpl::passRawAt(CORBA::ULong row, CORBA::ULong col,
                        OctSeq& v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeRaw) ) throw InvalidFieldType();
    tbl_[row][col].passRaw(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getWStringAt:1.0
//
CORBA::WChar*
RecordSetImpl::getWStringAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeWString) ) throw InvalidFieldType();
    return tbl_[row][col].getWString();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setWStringAt:1.0
//
void
RecordSetImpl::setWStringAt(CORBA::ULong row, CORBA::ULong col,
                            const CORBA::WChar* v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeWString) ) throw InvalidFieldType();
    if ( getFieldSizeAt(col)==0) throw FieldSizeIsNotSet();
    tbl_[row][col].setWString(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setWStringAt:1.0
//
void
RecordSetImpl::setWStringWithSizeAt(CORBA::ULong row, 
                                    CORBA::ULong col,
                                    const CORBA::WChar* v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeWString) ) throw InvalidFieldType();
    unsigned long vlen=wcslen(v);
    if ( getFieldSizeAt(col)<vlen) setFieldSizeAt(col,vlen+sizeof(CORBA::WChar));
    tbl_[row][col].setWString(v);
}



//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getBlobAt:1.0
//
Blob_ptr
RecordSetImpl::getBlobAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeBlob) ) throw InvalidFieldType();
    return tbl_[row][col].getBlob();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setBlobAt:1.0
//
void
RecordSetImpl::setBlobAt(CORBA::ULong row, CORBA::ULong col,
                         Blob_ptr v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeBlob) ) throw InvalidFieldType();
    tbl_[row][col].setBlob(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getClobAt:1.0
//
Clob_ptr
RecordSetImpl::getClobAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeClob) ) throw InvalidFieldType();
    return tbl_[row][col].getClob();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setClobAt:1.0
//
void
RecordSetImpl::setClobAt(CORBA::ULong row,
                                     CORBA::ULong col,
                                     Clob_ptr v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeClob) ) throw InvalidFieldType();
    tbl_[row][col].setClob(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getWclobAt:1.0
//
Wclob_ptr
RecordSetImpl::getWclobAt(CORBA::ULong row, CORBA::ULong col)
{
    checkCell(row,col);
    checkNull(row,col);
    if ( isNotType(col,TypeWclob) ) throw InvalidFieldType();
    return tbl_[row][col].getWclob();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/setWclobAt:1.0
//
void
RecordSetImpl::setWclobAt(CORBA::ULong row, CORBA::ULong col,
                          Wclob_ptr v)
{
    checkCell(row,col);
    if ( isNotType(col,TypeWclob) ) throw InvalidFieldType();
    tbl_[row][col].setWclob(v);
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getAsStringAt:1.0
//
char*
RecordSetImpl::getAsStringAt(CORBA::ULong row, CORBA::ULong col,
                             const char* nullValue)
{
    CORBA::String_var retStr = CORBA::string_dup(nullValue);
    checkCell(row,col);
    Field& f = tbl_[row][col];
    if (f.isNull()) return retStr._retn();
    ostrstream ostr;
    switch (f.getType())
    {
      case TypeBoolean:
    ostr <<  ( f.getBoolean() ? 'T' : 'F' );
    break;
      case TypeChar:
    ostr << f.getChar();
    break;
      case TypeOctet:
    ostr << f.getOctet();
    break;
      case TypeShort:
    ostr << f.getShort();
    break;
      case TypeUShort:
    ostr << f.getUShort();
    break;
      case TypeLong:
    ostr << f.getLong();
    break;
      case TypeULong:
    ostr << f.getULong();
    break;
      case TypeLongLong:
    ostr << f.getLongLong();
    break;
      case TypeULongLong:
    ostr << f.getULongLong();
    break;
      case TypeFloat:
    ostr << f.getFloat();
    break;
      case TypeDouble:
    ostr << f.getDouble();
    break;
      case TypeString:
    ostr << f.getString_();
    break;
      case TypeNumeric:
    {
         Numeric_var num = f.getNumeric();
         ostr << num->getAsDouble(); 
    }
    break;
      case TypeDateTime:
         {
           char str[32];
           DateTime dt = f.getDateTime();
           sprintf(str,"%.2d:%.2d:%.4d %.2d:%.2d", (int)dt.day, (int)dt.month, (int)dt.year, (int)dt.hour, (int)dt.minute);
           ostr << str;
        }
    break;
    case TypeRaw:
           ostr << "TypeRaw";
           break;
    case TypeBlob:
           ostr << "Blob";
           break;
    case TypeClob:
           ostr << "Clob";
           break;
    case TypeWclob:
           ostr << "Wclob";
           break;
    case TypeWString:
          {
#ifdef HAVE_WCHAR_H
            const CORBA::WChar* curr = f.getWString_();
            unsigned long len=wcslen(curr);
           
            if (NULL == setlocale(LC_ALL,"")) {
                ostr << "{ wchar string } ";
            }else{
                mbstate_t ss;
                memset(&ss,0,sizeof(mbstate_t));
                size_t outbufflen=(len+1)*sizeof(CORBA::WChar);
                char* outbuff=new char[outbufflen];
                const wchar_t* tmp = (wchar_t*)curr;
                const wchar_t ** tmpp = &tmp;
                size_t nconverted = wcsrtombs(outbuff,tmpp,outbufflen,&ss);
                if (nconverted==((size_t)-1)) {
                   ostr << "{conversion-error}";
                } else {
                   ostr << outbuff;
                }
                delete[] outbuff;
            }
        
#else
#error    "WCHAR_T required".
#endif
          }
          break;
       default:
          ostr << "(strange type:" << f.getType() << ")" ;
          break;
    }
    ostr << '\0';
    retStr = CORBA::string_dup(ostr.str());
    ostr.rdbuf()->freeze(0);
    return retStr._retn();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getColumnNameByIndex:1.0
//
char*
RecordSetImpl::getColumnNameByIndex(CORBA::ULong col)
{
    if (col >= getNColumns()) throw InvalidColumn();
    CORBA::String_var name = CORBA::string_dup(cn_[col].c_str());
    return name._retn();
}

//
// IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getColumnIndexByName:1.0
//
CORBA::ULong
RecordSetImpl::getColumnIndexByName(const char* name)
{
    char *lwrname = new char [strlen(name)+1];
    for (int i=0;i<=strlen(name);i++)
       lwrname[i]=tolower(name[i]);
    NameColumns::const_iterator it = nc_.find(lwrname);
    if (it == nc_.end()) throw InvalidColumnName();
    delete [] lwrname;
    return (*it).second;
}

void
RecordSetImpl::mergeColumns(UAKGQuery2::RecordSet* rs)
{
    Field fl;
    CORBA::ULong i;
    CORBA::ULong oldNColumns = getNColumns();
    char* tmp;
    for (i = 0; i < rs->getNColumns(); i++){
        tmp = rs->getColumnNameByIndex(i);
        addColumn(tmp,rs->getFieldTypeAt(i));
        if(rs->haveFieldSizeAt(i)){
            setFieldSizeAt(i+oldNColumns,rs->getFieldSizeAt(i));
        }
        CORBA::string_free(tmp);
    }
    
    if (rs->getNRows() > getNRows()) { // align sizes.
        for(CORBA::ULong i=getNRows(); i<rs->getNRows(); ++i) {
           addRow();
	}
    }

    for (i=0; i < rs->getNRows(); i++){
        for(CORBA::ULong j=oldNColumns; j < getNColumns(); j++){
            Field f;
            getField(rs,i,j-oldNColumns, f);
            setField(i,j,f);    
        }
    }
}


void 
RecordSetImpl::appendRows(UAKGQuery2::RecordSet* rs)
{
    CORBA::ULong i, j, oldRows;
    for(i=0; i<rs->getNColumns(); i++){
        if(getFieldTypeAt(i)!=rs->getFieldTypeAt(i))
            throw UAKGQuery2::InvalidFieldType();
    }
    
    oldRows = getNRows();

    for(i=0; i<rs->getNRows(); i++) addRow();

    for(i=oldRows; i<oldRows+rs->getNRows(); i++){
        for(j=0; j<getNColumns(); j++){
            Field f;
            getField(rs,i-oldRows,j,f);
            setField(i,j,f);
        }
    }
}

void RecordSetImpl::assign(UAKGQuery2::RecordSet* rs)
{
 dropAllRows();
 dropColumns(0,getNColumns());
 mergeColumns(rs);
 appendRows(rs);
}

void RecordSetImpl::getField(UAKGQuery2::RecordSet* rs,
                              CORBA::ULong row, 
                              CORBA::ULong col,
                              Field& fld)
{
    fld.setNull();
    if (rs->isNullAt(row,col) ) return;
    switch (rs-> getFieldTypeAt(col) ) {
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
      case TypeLongLong:
            fld.setLongLong(rs->getLongLongAt(row,col));
            break;
      case TypeULongLong:
            fld.setULongLong(rs->getULongLongAt(row,col));
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
                char*  str = rs->getStringAt(row,col);
        fld.setString(str);
        CORBA::string_free(str);
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
      default:
            {
             std::ostringstream ostr;
             ostr << "unknown field type ( " << rs-> getFieldTypeAt(col) << ") in " << __FILE__ << ":" << __LINE__ ;
             std::cerr << ostr.str().c_str() << std::endl;
             throw std::runtime_error(ostr.str());
            }

     }
}

void RecordSetImpl::setField( CORBA::ULong row, CORBA::ULong col, const Field& ft )
{
    switch ( ft.getType() ) {
      case TypeNull:
            setNullAt(row,col);
            break;
      case TypeBoolean:
            setBooleanAt(row,col,ft.getBoolean());
            break;
      case TypeChar:
      case TypeCharacter:
            setCharAt(row,col,ft.getChar());
            break;
      case TypeOctet:
            setOctetAt(row,col,ft.getOctet());
            break;
      case TypeShort:
      case TypeSmallInt:
            setShortAt(row,col,ft.getShort());
            break;
      case TypeUShort:
            setUShortAt(row,col,ft.getUShort());
            break;
      case TypeLong:
      case TypeInteger:
            setLongAt(row,col,ft.getLong());
            break;
      case TypeULong:
            setULongAt(row,col,ft.getULong());
            break;
      case TypeLongLong:
            setLongLongAt(row,col,ft.getLongLong());
            break;
      case TypeULongLong:
            setULongLongAt(row,col,ft.getULongLong());
            break;
      case TypeFloat:
      case TypeReal:
            setFloatAt(row,col,ft.getFloat());
            break;
      case TypeDouble:
      case TypeDoublePrecision:
            setDoubleAt(row,col,ft.getDouble());
            break;
      case TypeString:
            setStringAt(row,col,ft.getString_());
            break;
      case TypeNumeric:
      case TypeDecimal:
            setNumericAt(row,col,ft.getNumeric_());
            break;
      case TypeDateTime:
            setDateTimeAt(row,col,ft.getDateTime());
            break;
      case TypeRaw:
        {
             const OctSeq* ptr = ft.getRaw_();
                 setRawAt(row,col,*ptr);
        }
            break;
      case TypeBlob:
            setBlobAt(row,col,ft.getBlob_());
            break;
      case TypeClob:
            setClobAt(row,col,ft.getClob_());
            break;
      case TypeWString:
            setWStringAt(row,col,ft.getWString_());
            break;
      case TypeWclob:
            setWclobAt(row,col,ft.getWclob_());
            break;
      default:
            {
             std::ostringstream ostr;
             ostr << "unknown field type ( " << ft.getType() << ") in " << __FILE__ << ":" << __LINE__ ;
             std::cerr << ostr.str().c_str() << std::endl;
             throw std::runtime_error(ostr.str());
            }
     }
}



RecordSetFactoryImpl::RecordSetFactoryImpl()
{
  lazyInitLocale();
}

RecordSetFactoryImpl::~RecordSetFactoryImpl()
{
}

CORBA::ValueBase*
RecordSetFactoryImpl::create_for_unmarshal()
{
    return new RecordSetImpl;
}

RecordSet*
RecordSetFactoryImpl::create()
{
    return new RecordSetImpl();
}

char* RecordSetFactoryImpl::locale = NULL;

void RecordSetFactoryImpl::lazyInitLocale()
{
 locale=setlocale(LC_ALL,"");
 if (locale==NULL) {
   std::cerr << "warning, can't init locale" << std::endl;
 }
}

