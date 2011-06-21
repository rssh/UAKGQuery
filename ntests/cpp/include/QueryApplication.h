#ifndef __QueryApplication_h
#define __QueryApplication_h

/**
 * Common functionality for query application. 
 * (particularry - tests).
 * (C) GradSoft Ltd, Kiev, Ukraine
 * 1999-2003
 **/


#ifndef __GradSoft_ProgOptions_h
#include <GradSoft/ProgOptions.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

/**
 * class with common functionality of query tests.
 * usage:
 * <code>
 *   class MyApp: public QueryApplication
 *   {
 *    public:
 *       MyApp(int argc,char* argv)
 *        :QueryApplication(argc,argv) {}
 *
 *       bool doQueryStuff(DBConnectionManager* dbc);
 *
 *   }
 *
 *   bool MyApp::doQueryStuff(DBConnectionManager* dbc)
 *   {
 *    YOU CODE HERE.
 *    IF ALL OK - return true.
 *   }
 *
 *   int main(int argc,char** argv)
 *   {
 *    MyApp app(argc,argv);
 *    int retval=app.run();
 *    return retval;  
 *   }
 * </code>
 */
class QueryApplication
{
public:                  

    QueryApplication();
    ~QueryApplication();

    int run(int argc, char** argv);

    /*
     * must be overloaded to return database driver.
     *  (Oracle8 Oracle9 InterBase ODBC)
     */
    virtual const char*  getDefaultDBDriver() const;

    /**
     * can be overloaded to put additiona options into ProgOptions.
     **/
    virtual void  configure();

    /*
     * can be overloaded to post-parse additional options.
     **/
    virtual void  init() ;


    /**
     * All tests overload this method and return true on success.
     **/
    virtual bool doQueryStuff(UAKGQuery2::QueryManager_ptr dbc) = 0;


    // what we call from subclasses:

    UAKGQuery2::RecordSet*  createRecordSet();
    UAKGQuery2::Numeric*    createNumeric();

    GradSoft::ProgOptions&  getProgOptions();

    void    printRecordSet(std::ostream& out, UAKGQuery2::RecordSet* recordSet);
    void    printNumeric(std::ostream& out, UAKGQuery2::Numeric* numeric);
    void    printDateTime(std::ostream& out, const UAKGQuery2::DateTime& numeric);
    void    printRaw(std::ostream& out, const UAKGQuery2::OctSeq& raw);


private:

    CORBA::ORB_var orb_;
    UAKGQuery2::RecordSet_init* recordSetFactory_;
    UAKGQuery2::Numeric_init*   numericFactory_;

    GradSoft::ProgOptions progOptions_;

};




#endif
