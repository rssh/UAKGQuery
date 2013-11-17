#ifndef __RecordSetImpl_h
#define __RecordSetImpl_h

/**
 * RecordSet valuetype implementations.
 * part of Grad-Soft UAKGQuery.
 * (C) Grad-Soft, 2002, Kiev, Ukraine.
 */

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __UAKGQuerySingleton_h
#include <UAKGQuerySingleton.h>
#endif

#ifndef __GradSoft_deque_INCLUDED
#include <deque>
#ifndef __GradSoft_deque_INCLUDED
#define __GradSoft_deque_INCLUDED
#endif
#endif

#ifndef __GradSoft_map_INCLUDED
#include <map>
#ifndef __GradSoft_map_INCLUDED
#define __GradSoft_map_INCLUDED
#endif
#endif

#ifndef __GradSoft_string_INCLUDED
#include <string>
#ifndef __GradSoft_string_INCLUDED
#define __GradSoft_string_INCLUDED
#endif
#endif

#ifndef __Numeric_NumericImpl_h_
#include <Numeric/NumericImpl.h>
#endif

namespace UAKGQuery2 {

/**
 * Field: internal helper class.
 **/
class Field 
{
 UAKGQuery2::FieldType type_;
 union {
        CORBA::Boolean b;
        CORBA::Char c;
        CORBA::Octet o;
        CORBA::Short s;
        CORBA::UShort us;
        CORBA::Long l;
        CORBA::ULong ul;
        CORBA::Float f;
        CORBA::Double d;
        CORBA::Char* str;
        UAKGQuery2::Numeric* num_p;
        UAKGQuery2::DateTime dt;
        UAKGQuery2::OctSeq* raw_p;
        CORBA::WChar* wstr;
        UAKGQuery2::Blob_ptr bl;
        UAKGQuery2::Clob_ptr cl;
        UAKGQuery2::Wclob_ptr wcl;
        CORBA::LongLong ll;
        CORBA::ULongLong ull;
    } value_;

    void setType(UAKGQuery2::FieldType type) { type_ = type; }
    void setField(const Field& fld);
    void destroy();

public:
    Field(): type_(TypeNull) {};
    Field(const Field& fld);
    void operator=(const Field& fld);

    ~Field() { destroy(); }

    UAKGQuery2::FieldType getType() const {
        return type_;
    }

    CORBA::Boolean isNull() const
    { return (type_==TypeNull); }

    CORBA::Boolean getBoolean() const
    { return value_.b; }

    CORBA::Char getChar() const
    { return value_.c; }

    CORBA::Octet getOctet() const
    { return value_.o; }

    CORBA::Short getShort() const
    { return value_.s; }

    CORBA::UShort getUShort() const
    { return value_.us; }

    CORBA::Long getLong() const
    { return value_.l; }

    CORBA::ULong getULong() const
    { return value_.ul; }

    CORBA::LongLong getLongLong() const
    { return value_.ll; }

    CORBA::ULongLong getULongLong() const
    { return value_.ull; }

    CORBA::Float getFloat() const
    { return value_.f; }

    CORBA::Double getDouble() const
    { return value_.d; }

    char* getString()
    { return CORBA::string_dup(value_.str); }

    const char* getString_() const
    { return value_.str; }

    /**
     * return Numeric and increase ref counter.
     **/
    UAKGQuery2::Numeric* getNumeric()
    { 
    value_.num_p->_add_ref();
    return value_.num_p; 
    }

    /**
     * return Numeric without increasing of reference counter.
     **/
    UAKGQuery2::Numeric* getNumeric_() const
    { return value_.num_p; }


    UAKGQuery2::DateTime getDateTime() const
    { return value_.dt; }


    UAKGQuery2::OctSeq* getRaw()
    {
      UAKGQuery2::OctSeq_var retval = new UAKGQuery2::OctSeq;
      retval->length(value_.raw_p->length());
      memcpy(retval->get_buffer(),value_.raw_p->get_buffer(),
                                  value_.raw_p->length());
      return retval._retn();
    }
        
    void setRawTo(UAKGQuery2::OctSeq& raw)
    {
      raw.replace(value_.raw_p->length(),value_.raw_p->length(),
                   value_.raw_p->get_buffer(),false);
    }

    UAKGQuery2::OctSeq& getRawRef()
    {
      return *value_.raw_p;
    }

    /**
     * return pointer to reference.
     **/
    const UAKGQuery2::OctSeq* getRaw_() const
    { return value_.raw_p; };

    CORBA::WChar* getWString()
    { return CORBA::wstring_dup(value_.wstr); }

    CORBA::WChar* getWString() const
    { return CORBA::wstring_dup(value_.wstr); }

    const CORBA::WChar* getWString_() const
    { return value_.wstr; }

    UAKGQuery2::Blob_ptr getBlob()
    { return UAKGQuery2::Blob::_duplicate(value_.bl); }

    const UAKGQuery2::Blob_ptr getBlob_() const
    { return value_.bl; }

    UAKGQuery2::Clob_ptr getClob()
    { return UAKGQuery2::Clob::_duplicate(value_.cl); }

    const UAKGQuery2::Clob_ptr getClob_() const
    { return value_.cl; }

    UAKGQuery2::Wclob_ptr getWclob()
    { return UAKGQuery2::Wclob::_duplicate(value_.wcl); }

    const UAKGQuery2::Wclob_ptr getWclob_() const
    { return value_.wcl; }

    void setNull()
    { destroy();  setType(TypeNull); }

    void setBoolean(const CORBA::Boolean v)
    { destroy(); setType(TypeBoolean); value_.b = v; }

    void setChar(const CORBA::Char v)
    { destroy(); setType(TypeChar); value_.c = v; }

    void setOctet(const CORBA::Octet v)
    { destroy(); setType(TypeOctet); value_.o = v; }

    void setShort(const CORBA::Short v)
    { destroy(); setType(TypeShort); value_.s = v; }

    void setUShort(const CORBA::UShort v)
    { destroy(); setType(TypeUShort); value_.us = v; }

    void setLong(const CORBA::Long v)
    { destroy();  setType(TypeLong); value_.l = v; }

    void setULong(const CORBA::ULong v)
    { destroy();  setType(TypeULong); value_.ul = v; }

    void setFloat(const CORBA::Float v)
    { destroy(); setType(TypeFloat); value_.f = v;}

    void setDouble(const CORBA::Double v)
    { destroy(); setType(TypeDouble); value_.d = v; }

    void setLongLong(const CORBA::LongLong v)
    { destroy(); setType(TypeLongLong); value_.ll = v; }
    
    void setULongLong(const CORBA::ULongLong v)
    { destroy(); setType(TypeULongLong); value_.ull = v; }


    //
    // Memory managment. Release in deconstructor.
    // 
    void setString(const char* v)
    { 
      destroy();
      setType(TypeString);
      value_.str = CORBA::string_dup(v);
    }
     
    //
    // memory managment:  ref-counting: v is passed with increasing of counter.
    //
    void setNumeric(UAKGQuery2::Numeric* v)
    {
     destroy();
     setType(TypeNumeric);
     v->_add_ref();
     value_.num_p = v;
    }

    //
    // memory managment: 
    //    'v' is incremented by 1 and decremented in Field destructor
    //
    void setDecimal(UAKGQuery2::Numeric* v)
    {
      destroy();
      setType(TypeNumeric); 
      v->_add_ref();
      value_.num_p = v;
    }

    void setDateTime(const UAKGQuery2::DateTime& v)
    {
      destroy();
      setType(TypeDateTime);  
      value_.dt = v;    
    }

    //
    // memory managment: v passed by value.
    //
    void setRaw(const UAKGQuery2::OctSeq& v)
    {
      destroy();
      setType(TypeRaw);
      value_.raw_p = new UAKGQuery2::OctSeq;
      value_.raw_p->length(v.length());
      memcpy(value_.raw_p->get_buffer(),v.get_buffer(),v.length());
    }

    //
    // memory managment: v passed as pointers to memory region.
    //
    void passRaw(UAKGQuery2::OctSeq& v)
    {
      destroy();
      setType(TypeRaw);
      value_.raw_p = new UAKGQuery2::OctSeq;
      value_.raw_p->replace(v.length(), v.length(), v.get_buffer(), false);
    }

    //
    // memory managment: string is copied.
    //
    void setWString(const CORBA::WChar* v)
    {
    destroy();
    setType(TypeWString);
    value_.wstr = CORBA::wstring_dup(v);
    }


    //
    // memory managment: Release in deconstructor.
    //
    void setBlob(const UAKGQuery2::Blob* v)
    {
     destroy();
     setType(TypeBlob);
     value_.bl = UAKGQuery2::
            Blob::_duplicate(const_cast<UAKGQuery2::Blob*>(v));
    }

    //
    // Memory managment: 'v' is duplicated.
    //
    void setClob(const UAKGQuery2::Clob* v)
    {
    destroy();
    setType(TypeClob);
        value_.cl = UAKGQuery2:: 
            Clob::_duplicate(const_cast<UAKGQuery2::Clob*>(v));
    }

    //
    // Memory managment. Release in deconstructor.
    //
    void setWclob(const UAKGQuery2::Wclob* v)
    {
    destroy();
    setType(TypeWclob);
        value_.wcl = UAKGQuery2::
             Wclob::_duplicate(const_cast<UAKGQuery2::Wclob*>(v));
    }

    void write(CORBA::OctetSeq& seq,CORBA::ULong& pos);

    void read(CORBA::Octet fieldType,
              CORBA::OctetSeq& seq,
              CORBA::ULong& pos);
};

class RecordSetImpl : virtual public OBV_UAKGQuery2::RecordSet,
                      virtual public CORBA::DefaultValueRefCountBase
{
    RecordSetImpl(const RecordSetImpl&);
    void operator=(const RecordSetImpl&);

    typedef std::deque<UAKGQuery2::Field>     Column;
    typedef std::deque<UAKGQuery2::FieldType> ColumnTypes;
    typedef std::deque<Column>                Table;

    typedef std::deque<CORBA::ULong>   SequenceOfULongs;
    typedef std::deque<CORBA::UShort>  SequenceOfUShorts;

    typedef std::map<std::string, CORBA::ULong> NameColumns;
    typedef std::deque<std::string> ColumnNames;

    ColumnNames cn_;
    ColumnTypes ct_;
    NameColumns nc_;
    Table tbl_;
    SequenceOfULongs  sz_; // field's sizes
    SequenceOfULongs  pr_; // numeric precisions
    SequenceOfUShorts sc_; // numeric scales

    void checkRow(CORBA::ULong row) const
    {
        if (row >= getNRows()) {
            throw InvalidRow();
        }
    }

    void checkColumn(CORBA::ULong col) const
    {
        if (col >= getNColumns()) {
      throw InvalidColumn();
        }
    }

    void checkCell(CORBA::ULong row, CORBA::ULong col) const
    {
    checkRow(row); checkColumn(col);
    }

    void checkNull(CORBA::ULong row, CORBA::ULong col) const
    {
        if (tbl_[row][col].isNull()) {
         throw FieldIsNull();
    }
    }

    inline CORBA::Boolean isNotType(CORBA::ULong col, 
                 UAKGQuery2::FieldType ftype) const
    {
        return (ct_[col] != ftype);
    }

    inline CORBA::Boolean isType(CORBA::ULong col, 
                 UAKGQuery2::FieldType ftype) const
    {
        return (ct_[col] == ftype);
    }


    void setField( CORBA::ULong row, CORBA::ULong col, const Field& ft );
    void getField(UAKGQuery2::RecordSet* rs,CORBA::ULong row, CORBA::ULong col, Field& fld);

public:

    RecordSetImpl();
    ~RecordSetImpl();

    virtual CORBA::ValueBase* _copy_value();

    virtual void marshal(CORBA::DataOutputStream*);

#if defined(MICO) && MICO_BIN_VERSION < 0x02030b
	virtual CORBA::ValueBase* unmarshal(CORBA::DataInputStream* is );
#else
    virtual void unmarshal(CORBA::DataInputStream*);
#endif


    //
    // IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getNRows:1.0
    //
    virtual CORBA::ULong getNRows() const;

    virtual CORBA::ULong getNRows();

    //
    // IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/getNColumns:1.0
    //
    virtual CORBA::ULong getNColumns();

    virtual CORBA::ULong getNColumns() const;

    //
    // IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/addRow:1.0
    //
    virtual void addRow();

    //
    // IDL:gradsoft.kiev.ua/UAKGQuery/RecordSet/addColumn:1.0
    //
    virtual void addColumn(const char* name,
                           UAKGQuery2::FieldType ft);


    virtual void setFieldSizeAt(CORBA::ULong col,
                              CORBA::ULong len);

    virtual CORBA::ULong getFieldSizeAt(CORBA::ULong col);

    virtual CORBA::ULong getFieldSizeAt(CORBA::ULong col) const;

    virtual FieldType getFieldTypeAt(CORBA::ULong column);

    virtual FieldType getFieldTypeAt(CORBA::ULong column) const;

    virtual CORBA::Boolean haveFieldSizeAt(CORBA::ULong column);

    virtual CORBA::Boolean haveFieldSizeAt(CORBA::ULong column) const;

    virtual void setNumericFieldPropertiesAt(CORBA::ULong col,
                                             CORBA::UShort precision,
                                             CORBA::UShort scale);

    virtual CORBA::Boolean haveNumericFieldPropertiesAt(CORBA::ULong column) const;

    virtual CORBA::Boolean haveNumericFieldPropertiesAt(CORBA::ULong column);

    virtual CORBA::UShort getNumericPrecisionAt(CORBA::ULong col);

    virtual CORBA::UShort getNumericPrecisionAt(CORBA::ULong col) const;

    virtual CORBA::UShort getNumericScaleAt(CORBA::ULong column);

    virtual CORBA::UShort getNumericScaleAt(CORBA::ULong column) const;


    virtual void dropRow(CORBA::ULong row);

	virtual void dropAllRows();

    virtual void dropColumn(CORBA::ULong col);

	virtual void dropColumns(CORBA::ULong frs, CORBA::ULong snd);

    virtual CORBA::Boolean isNullAt(CORBA::ULong row,
                                    CORBA::ULong col);

    virtual void setNullAt(CORBA::ULong row,
                           CORBA::ULong col);

    virtual CORBA::Boolean getBooleanAt(CORBA::ULong row,
                                        CORBA::ULong col);

    virtual void setBooleanAt(CORBA::ULong row,
                              CORBA::ULong col,
                              CORBA::Boolean v);

    virtual CORBA::Char getCharAt(CORBA::ULong row,
                                  CORBA::ULong col);

    virtual void setCharAt(CORBA::ULong row,
                           CORBA::ULong col,
                           CORBA::Char v);

    virtual CORBA::Octet getOctetAt(CORBA::ULong row,
                                    CORBA::ULong col);

    virtual void setOctetAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::Octet v);

    virtual CORBA::Short getShortAt(CORBA::ULong row,
                                    CORBA::ULong col);

    virtual void setShortAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::Short v);

    virtual CORBA::UShort getUShortAt(CORBA::ULong row,
                                      CORBA::ULong col);

    virtual void setUShortAt(CORBA::ULong row,
                             CORBA::ULong col,
                             CORBA::UShort v);

    virtual CORBA::Long getLongAt(CORBA::ULong row,
                                  CORBA::ULong col);

    virtual void setLongAt(CORBA::ULong row,
                           CORBA::ULong col,
                           CORBA::Long v);

    virtual CORBA::ULong getULongAt(CORBA::ULong row,
                                    CORBA::ULong col);

    virtual void setULongAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::ULong v);

    virtual CORBA::LongLong getLongLongAt(CORBA::ULong row,
                                          CORBA::ULong col);

    virtual void setLongLongAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::LongLong v);

    virtual CORBA::ULongLong getULongLongAt(CORBA::ULong row,
                                            CORBA::ULong col);

    virtual void setULongLongAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::ULongLong v);


    virtual CORBA::Float getFloatAt(CORBA::ULong row,
                                    CORBA::ULong col);

    virtual void setFloatAt(CORBA::ULong row,
                            CORBA::ULong col,
                            CORBA::Float v);

    virtual CORBA::Double getDoubleAt(CORBA::ULong row,
                                      CORBA::ULong col);

    virtual void setDoubleAt(CORBA::ULong row,
                             CORBA::ULong col,
                             CORBA::Double v);

    virtual char* getStringAt(CORBA::ULong row,
                              CORBA::ULong col);

    virtual void setStringAt(CORBA::ULong row,
                             CORBA::ULong col,
                             const char* v);

    virtual void setStringWithSizeAt(CORBA::ULong row,
                                     CORBA::ULong col,
                                     const char* v);

    virtual UAKGQuery2::Numeric* getNumericAt(CORBA::ULong row,
                                             CORBA::ULong col);

    virtual void setNumericAt(CORBA::ULong row,
                              CORBA::ULong col,
                              UAKGQuery2::Numeric* v);

    virtual UAKGQuery2::DateTime getDateTimeAt(CORBA::ULong row,
                                              CORBA::ULong col);

    virtual void setDateTimeAt(CORBA::ULong row,
                               CORBA::ULong col,
                               const UAKGQuery2::DateTime& v);

    virtual UAKGQuery2::OctSeq* getRawAt(CORBA::ULong row,
                                        CORBA::ULong col);

    virtual void setRawAt(CORBA::ULong row,
                          CORBA::ULong col,
                          const UAKGQuery2::OctSeq& v);

    virtual void passRawAt(CORBA::ULong row,
                          CORBA::ULong col,
                          UAKGQuery2::OctSeq& v);


    virtual CORBA::WChar* getWStringAt(CORBA::ULong row,
                                       CORBA::ULong col);



    virtual void setWStringAt(CORBA::ULong row,
                              CORBA::ULong col,
                              const CORBA::WChar* v);

    virtual void setWStringWithSizeAt(CORBA::ULong row,
                                      CORBA::ULong col,
                                      const CORBA::WChar* v);


    virtual UAKGQuery2::Blob_ptr getBlobAt(CORBA::ULong row,
                                          CORBA::ULong col);

    virtual void setBlobAt(CORBA::ULong row,
                           CORBA::ULong col,
                           UAKGQuery2::Blob_ptr v);

    virtual UAKGQuery2::Clob_ptr getClobAt(CORBA::ULong row,
                                          CORBA::ULong col);

    virtual void setClobAt(CORBA::ULong row,
                           CORBA::ULong col,
                           UAKGQuery2::Clob_ptr v);

    virtual UAKGQuery2::Wclob_ptr getWclobAt(CORBA::ULong row,
                                            CORBA::ULong col);

    virtual void setWclobAt(CORBA::ULong row,
                            CORBA::ULong col,
                            UAKGQuery2::Wclob_ptr v);

    virtual char* getAsStringAt(CORBA::ULong row,
                                CORBA::ULong col,
                                const char* nullValue);

    virtual char* getColumnNameByIndex(CORBA::ULong col);

    virtual CORBA::ULong getColumnIndexByName(const char* name);

    virtual void mergeColumns(UAKGQuery2::RecordSet* rs);

    virtual void appendRows(UAKGQuery2::RecordSet* rs);

    virtual void assign(UAKGQuery2::RecordSet* rs);

};

class RecordSetFactoryImpl : virtual public UAKGQuery2::RecordSet_init
{
    virtual CORBA::ValueBase* create_for_unmarshal();

public:
    RecordSetFactoryImpl();
    virtual ~RecordSetFactoryImpl();

    virtual UAKGQuery2::RecordSet* create();

public:

  // for initializing of locale before wstrcmb call.
  static char* locale;
  static void   lazyInitLocale();

  

};

} // UAKGQuery2 namespace


#endif
