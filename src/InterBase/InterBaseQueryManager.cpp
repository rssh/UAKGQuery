#include <InterBase/InterBaseQueryManager.h>
#include <InterBase/InterBaseQuery.h>
#include <InterBase/InterBaseLobs.h>
#include <UAKGQuerySingleton.h>
#include <strstream>
using namespace UAKGQuery2;
using namespace std;

void InterBaseQueryManager::init( 
                               DBConnectionManagers_impl* dbConnectionManagers_p,
                               const char* username, const char* password,
                               const char* dbLink,
                               const char* other)
{
 UAKGQueryManagerBase::init(dbConnectionManagers_p, username,
                                       password,dbLink,other);
 hndls_p_ = new SessionLevelIBHandlersPool(this);
}
 
void InterBaseQueryManager::close()
{
 delete hndls_p_;
 hndls_p_ = NULL;
}

Query_ptr  InterBaseQueryManager::create_query(
                           const char* query_text, const char* flags)
{
  InterBaseQuery* query_impl = new InterBaseQuery(query_text,this,flags);
  Query_var query = query_impl->_this();
  query_impl->_remove_ref();
  ++nQueries_;
  return query._retn(); 
}


CORBA::Long InterBaseQueryManager::get_next_sequence_value(const char* sequence)
{
   ostrstream sql;
   sql << "select gen_id(" << sequence<< ", 1) from dual" << '\0';   
   RecordSet_var rs = evaluate_e(sql.str(), "");
   sql.rdbuf()->freeze(0);
   CORBA::String_var tmp = rs->getAsStringAt(0,0,"0");
   long retval = atoi(tmp);
   return retval;
}


/**
*   return sql adapted for custom driver.
**/
char*  InterBaseQueryManager::normalize_sql(const char* sql)
{
   ostrstream ostr;
   int i=0;
   bool add = true;
   for (i=0; i<strlen(sql); i++)
   {
        if (add)
        {
            if (sql[i] == ':')
            {
                ostr << '?';
                add = false;
            } else {
                ostr << sql[i];
            }
        }else{
            switch(sql[i])
            {
                case ' ' :
                case ',' :
                case ')' :
                case '(' :
                case '.' :
                case ';' :
                case '%' :
                case '=' :
                case '-' :
                case '+' :
                    {
                        add = true;
                        ostr << sql[i];
                    }
            }
        }
   }
   ostr << '\0';
   CORBA::String_var retval = CORBA::string_dup(ostr.str());
   ostr.rdbuf()->freeze(0);
   return retval._retn();
}

PortableServer::ServantBase* InterBaseQueryManager::getBlobReadingServant()
{
    return new InterBaseBlobForReading(PortableServer::Current::_duplicate(
             UAKGQuerySingleton::getCurrent_ptr()));
}

PortableServer::ServantBase* InterBaseQueryManager::getClobReadingServant()
{
    return new InterBaseClobForReading(PortableServer::Current::_duplicate(
             UAKGQuerySingleton::getCurrent_ptr()));
}

PortableServer::ServantBase* InterBaseQueryManager::getWclobReadingServant()
{
    return new InterBaseWclobForReading(PortableServer::Current::_duplicate(
             UAKGQuerySingleton::getCurrent_ptr()));
}

PortableServer::ServantBase* InterBaseQueryManager::getBlobWritingServant()
{
    return new InterBaseBlob(PortableServer::Current::_duplicate(
             UAKGQuerySingleton::getCurrent_ptr()));
}

PortableServer::ServantBase* InterBaseQueryManager::getClobWritingServant()
{
    return new InterBaseClob(PortableServer::Current::_duplicate(
             UAKGQuerySingleton::getCurrent_ptr()));
}

PortableServer::ServantBase* InterBaseQueryManager::getWclobWritingServant()
{
    return new InterBaseWclob(PortableServer::Current::_duplicate(
             UAKGQuerySingleton::getCurrent_ptr()));
}


