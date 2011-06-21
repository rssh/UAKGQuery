#ifndef __Numeric_NumericImpl_h_ 
#define __Numeric_NumericImpl_h_ 1

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __Numeric_NumericParser_h
#include <Numeric/NumericParser.h>
#endif
 
#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

namespace UAKGQuery2 {

class NumericImpl : public virtual  OBV_UAKGQuery2::Numeric,
            public virtual  CORBA::DefaultValueRefCountBase
{
    int nDigits_;

   /**
    * resize internal decimal, to allow at least nBytes be present
    **/
    void resize(int nBytes);
 
    /**
     * return true, if last halph of byte is unused.
     **/
    int lastHalphByteIsUnused()
    {
       return ((value()[nDigits_*2+2-1]&0x0F)==0x0F);
    }

    void initialAppendULong(CORBA::ULong l);

    NumericImpl(const NumericImpl& num);
    void operator=(const NumericImpl&);

public:

    NumericImpl() {init();}

    ~NumericImpl();

    virtual CORBA::ValueBase* _copy_value();

    virtual char* getAsString();

    virtual void setAsString(const char* str);

    virtual CORBA::Long getAsLong();

    virtual void setAsLong(CORBA::Long l);

    virtual CORBA::Double  getAsDouble();

    virtual void setAsDouble(double);

    int  positive() const
    {
       return is0() ? (1) : (((char)(value()[1]))!='-');
    }

    int  negative() const
    {
       return is0() ? (0) : (((char)(value()[1]))=='-');
    }

    int    stringExpLen()   const ;
    char*  createStringExp()   const ;
    void   fillStringExp(char*) const ;
    char*  createStringFixed() const ;
    void   fillStringFixed(char*) const ;

    void init();
   
    void initnDigits();

    int readDigit(int nDigit) const;

    void _writeDigit(int digit, int nDigit);

    void normalizeLeft();

    void normalizeExponente();

    void normalizeRight();

    void  normalize();

    void parse(const char* sz);

    void appendHandre(int handre);

    void appendDigit(int digit);

    void _exponente(int exp)
    {
       value()[0]=static_cast<CORBA::Octet>(exp);
    }

    void _sign(char c)
    {
       value()[1]=c;
    }

    void _positive(int flg)
    {
       value()[1]=(flg ? '+' : '-');
    }

    int nDigits() const
    {
       return nDigits_;
    }

    int is0(void) const;

    int  exponente() const
    {
       return is0() ? 0 : (int)((signed char)(value()[0]));
    }

};

class NumericFactoryImpl : virtual public Numeric_init
{
    virtual CORBA::ValueBase* create_for_unmarshal();
public:
    NumericFactoryImpl();
    virtual ~NumericFactoryImpl();
    virtual Numeric*  create();
};

}

#endif
