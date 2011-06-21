#define COOKIE_PREFIX "CookieWebSQL_"

#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include CORBA_SKELETON_HEADER(HTTP)
#ifdef COSNAMING_H
#include COSNAMING_H
#else
#error COSNAMING_H undefined, check configuration.
#endif
#include <UAKGQuery2Init.h>

#include <strstream>
#include <string.h>

using namespace std;
using namespace UAKGQuery2;
using namespace HTTP;

static void
parse_string(const char* str,
             CORBA::String_var& login, CORBA::String_var& password,
             CORBA::String_var& driver, CORBA::String_var& alias );

static char*
conver_less_and_greater(char* str);

static int
init_query_manager( CORBA::ORB_ptr orb, 
					QueryManager_ptr& queryManager,
					const char* login, const char* password,
					const char* alias, const char* driver  );

static void
destroy_query_manager(QueryManager_ptr& queryManager);

static void 
display_table(	const char* sqlStatement, HTTPStream_ptr stream,
				const char* login, const char* password,
				const char* alias, const char* driver  );
static void
usage(const char* name);

static CORBA::Long
parse_option(int argc, char* argv[]);

static RecordSet_init*  recordSetFactory = NULL;
static CORBA::ORB_ptr 	orb  = NULL;
static string			encoding;

class WebSQLServlet;
class WebSQLHandler;

class WebSQLHandler: virtual public POA_HTTP::RequestHandler
{
	WebSQLServlet* owner_p_;

public:
	WebSQLHandler(WebSQLServlet* owner_p):owner_p_(owner_p)
	{}

	void 
	handle(const RequestInfo& requestInfo, HTTPStream_ptr stream);

	void
	destroy();
};

class WebSQLServlet : virtual public POA_HTTP::Servlet
{
private:
	PortableServer::POA_var poa_;

public:

	WebSQLServlet(PortableServer::POA_ptr poa)
	:poa_(PortableServer::POA::_duplicate(poa))
	{}

	RequestHandler_ptr 
	create_handler(const char* handlerName,
		const ClientInfo& clientInfo,
		const ServerInfo& serverInfo,
		const char* passwd)
		throw(NoSuchHandlerException,
			RequireAuthInfo,
			AccessDenied,
			CORBA::SystemException);		

	void  
	destroyHandler(WebSQLHandler* handler_impl)
	{
		PortableServer::ObjectId_var oid = poa_->servant_to_id(handler_impl);
		poa_->deactivate_object(oid.in());
		handler_impl->_remove_ref();
	}
};

void
WebSQLHandler::handle(const RequestInfo& requestInfo, 
	HTTPStream_ptr stream)
{
	CORBA::Boolean 		isQuery = false;
	CORBA::String_var 	sqlStatement("");
	CORBA::String_var   login("");
	CORBA::String_var   password("");
	CORBA::String_var   alias("");
	CORBA::String_var   driver("Oracle8");

	login = CORBA::string_dup(""); password = CORBA::string_dup("");
	driver = CORBA::string_dup("Oracle8"); alias = CORBA::string_dup("");

	for (CORBA::ULong i=0; i<requestInfo.cookies.length(); ++i)
	{
		if (!strcmp(requestInfo.cookies[i].name, COOKIE_PREFIX "db_params"))
		{
			CORBA::String_var str = 
				CORBA::string_dup(requestInfo.cookies[i].value);
			parse_string(str, login, password, driver, alias);
		}
	}

    CORBA::ULong nParameters=requestInfo.parameters.length();
	for (CORBA::ULong paramCount=0; paramCount<nParameters; ++paramCount)
	{
		if (strcmp(requestInfo.parameters[paramCount].name,"sql_txt")==0)
		{
			sqlStatement = CORBA::string_dup(
				requestInfo.parameters[paramCount].value
				);
		} else if (strcmp(requestInfo.parameters[paramCount].name,"login")==0)
		{
			login = CORBA::string_dup(
				requestInfo.parameters[paramCount].value
				);
		} else if (strcmp(requestInfo.parameters[paramCount].name,"password")==0)
		{
			password = CORBA::string_dup(
				requestInfo.parameters[paramCount].value
				);
		} else if (strcmp(requestInfo.parameters[paramCount].name,"alias")==0)
		{
			alias = CORBA::string_dup(
				requestInfo.parameters[paramCount].value
				);
		} else if (strcmp(requestInfo.parameters[paramCount].name,"driver")==0)
		{
			driver = CORBA::string_dup(
				requestInfo.parameters[paramCount].value
				);
		} else if (strcmp(requestInfo.parameters[paramCount].name,"qry")==0)
		{
			isQuery = true;
		}
    }


	if ( isQuery ) 
	{
		ostrstream ostr;
		ostr << login << "%20";
		ostr << password << "%20";
		ostr << driver << "%20";
		ostr << alias << '\0';
        ostr.freeze(0);
        ostr.rdbuf()->pubseekpos(0,ios::out);

		stream->set_cookie(COOKIE_PREFIX "db_params", ostr.str(), 3600);
	}
	stream->send_http_header();
	if ( encoding.size() > 0 )
	{
		string content = "text/html; charset=";
		content += encoding;
		stream->set_content_type(content.c_str());
	}
	else
		stream->set_content_type("text/html");

	stream->puts("<HTML>\n"
		"<HEAD><TITLE> Usage Web SQL </TITLE></HEAD>\n"
		"<BODY><CENTER>\n"
		"<FORM ACTION=WebSQL ENCTYPE=multipart/form-data METHOD=POST>\n"
		"<INPUT TYPE=HIDDEN NAME=qry VALUE=ON>\n");
	{
		ostrstream form;
		form << "<PRE>\n";
		form << "Login:    <INPUT TYPE=TEXT NAME=login VALUE="
			 << login << "> ";
		form << "Password: <INPUT TYPE=TEXT NAME=password VALUE="
			 << password << "><BR>\n";
		form << "Driver:   <INPUT TYPE=TEXT NAME=driver VALUE=" 
			 << driver << ">\n";
		form << "Alias:    <INPUT TYPE=TEXT NAME=alias VALUE=" 
			 << alias << "><BR>\n";
		form << "</PRE>\n";
		form << '\0';
		form.freeze(0);
		form.rdbuf()->pubseekpos(0,ios::out);
		stream->puts(form.str());
	}
	stream->puts("<H2>Enter SQL statement:</H2>\n"
		"<TEXTAREA NAME=\"sql_txt\" cols=60 rows=10>\n"
		"</TEXTAREA>\n"
		"<BR>\n"
		"<INPUT TYPE=SUBMIT VALUE=\"Submit\">\n"
		"</FORM><HR>\n");
	if (isQuery && strlen(sqlStatement))
	{
		display_table(sqlStatement, stream, login, password, alias, driver);
	}
	stream->puts("</CENTER></BODY>\n"
		"</HTML>");
}

inline void
WebSQLHandler::destroy()
{
	owner_p_->destroyHandler(this);
}

RequestHandler_ptr 
WebSQLServlet::create_handler(const char* handlerName,
	const ClientInfo& clientInfo,
	const ServerInfo& serverInfo,
	const char* passwd)
	throw(NoSuchHandlerException,
		RequireAuthInfo,
		AccessDenied,
		CORBA::SystemException)
{
	RequestHandler_var retval ;
	if (!strcmp(handlerName,"WebSQL")) {
		WebSQLHandler* handler_impl = new WebSQLHandler(this);
		retval = handler_impl->_this();
	}else{
		throw NoSuchHandlerException();
	}
	return retval._retn();
}

void
parse_string(const char* str, 
			 CORBA::String_var& login, CORBA::String_var& password, 
			 CORBA::String_var& driver, CORBA::String_var& alias )
{
	const int maxarg = 4;
	CORBA::String_var* ptr[maxarg] = {&login, &password, &driver, &alias};
    const char* delimit = " ";
    char  *buf;
    unsigned int indx = 0, start = 0, end;

    login = CORBA::string_dup(""); password = CORBA::string_dup(""); 
	alias = CORBA::string_dup(""); driver = CORBA::string_dup("");
    if ((buf = (char*) malloc(strlen(str)+1)) == NULL) return;
    start = strspn(str, delimit);
    do {
        end = strcspn(&str[start], delimit);
        strncpy(buf, &str[start], end); buf[end] = '\0';
		*(ptr[indx]) = CORBA::string_dup(buf);
        indx++; start += end+1;
    } while (str[start-1] != '\0' && indx < 4 && end > 0);
    free(buf);
}

char*
conver_less_and_greater(char* str)
{
	CORBA::ULong i, j, count = 0, len;
	
	len = strlen(str);
	for(i = 0; i < len; i++)
		if ( str[i] == '<' || str[i] == '>' )
			count++;
	
	if ( count == 0 ) 
		return str;
	
	CORBA::String_var tmpstr = CORBA::string_alloc(len + 3 * count + 1);
	for(i = j = 0; i <= len; i++, j++)
	{
		if ( str[i] == '<' )
		{
			tmpstr[j++] = '&'; tmpstr[j++] = 'l'; 
			tmpstr[j++] = 't'; tmpstr[j] = ';';
		} else if ( str[i] == '>' )
		{
			tmpstr[j++] = '&'; tmpstr[j++] = 'g'; 
			tmpstr[j++] = 't'; tmpstr[j] = ';';
		} else
			tmpstr[j] = str[i];
	}
	CORBA::string_free(str);

	return tmpstr._retn();
}

void 
display_table(	const char* sqlStatement, HTTPStream_ptr stream,
				const char* login, const char* password,
				const char* alias, const char* driver  )
{
	QueryManager_ptr queryManager;
	RecordSet_var recordSet;


	if ( init_query_manager(orb, queryManager, login, password, alias, driver) )
	{
		return;
	}
	try 
	{
		recordSet = queryManager->evaluate_e(sqlStatement, "");
	}
	catch (const QueryFlagInvalid& ex)
	{
		stream->puts("<H3>Query flag invalid</H3>");
		destroy_query_manager(queryManager);
		return;
	}
	catch (const QueryProcessingError& ex)
	{
		stream->puts("<H3>Query processing error:</H3>");
		stream->puts(ex.why);
		destroy_query_manager(queryManager);
		return;
	}
	stream->puts("<TABLE BORDER WIDTH=100%>\n");
	stream->puts("<THEAD>\n   <TR>");
	try
	{
		for(CORBA::ULong col = 0; col < recordSet->getNColumns(); col++)
		{
			CORBA::String_var name = recordSet->getColumnNameByIndex(col);
			stream->puts("<TH>");
			stream->puts(name);
			stream->puts("</TH>");
		}
		stream->puts("   </TR>\n</THEAD>\n");
		stream->puts("<TBODY>\n");
		for(CORBA::ULong row = 0; row < recordSet->getNRows(); row++) 
		{
			stream->puts("   <TR>");
			for(CORBA::ULong col = 0;col < recordSet->getNColumns(); col++)
			{
				char* str = 
					recordSet->getAsStringAt(row, col, "NULL");
				CORBA::String_var asString = conver_less_and_greater(str);
				stream->puts("<TD>");
				stream->puts(asString);
				stream->puts("</TD>");
			}
			stream->puts("</TR>\n");
		}
		stream->puts("</TBODY>\n</TABLE>\n");
	}
	catch (const InvalidColumn& ex) 
	{
		std::cerr << "Invalid column" << endl;
	}
	catch (const InvalidRow& ex)
	{
		std::cerr << "Invalid row" << endl;
	}
	destroy_query_manager(queryManager);
}

int
init_query_manager( CORBA::ORB_ptr orb, 
					QueryManager_ptr& queryManager,
					const char* login, const char* password,
					const char* alias, const char* driver  )
{
	CORBA::Object_var obj;
	try 
	{
		obj = orb->resolve_initial_references("UAKGQueryService2");
	}
	catch(const CORBA::ORB::InvalidName&)
	{
		cerr << ": can't resolve UAKGQueryService" << endl;
		return 1;
	}

	if(CORBA::is_nil(obj)) 
	{
		cerr << ": UAKGQueryService is a nil object reference" << endl;
		return 1;
    }

	CORBA::ValueFactory vf=orb->lookup_value_factory(
						"IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
	recordSetFactory=RecordSet_init::_downcast(vf);
	if (recordSetFactory==NULL) 
	{
		cerr <<  "Internal error: can't find valuetype factory for RecordSet"
             << endl;
		return 1;
	}

	DBConnectionManager_var dbManager = DBConnectionManager::_narrow(obj);
	if (CORBA::is_nil(dbManager)) {
		cerr << ": can't narrow dbManager to correct type" << endl;
		return 1;
	}

    try 
	{
		queryManager =
			dbManager->create_query_manager(login, password,
				alias, driver, "");
	}catch(QueryManagerNotFound){
		cerr << ": can't find query manager." << endl;
		return 1;
    }

	return 0;
}

void
destroy_query_manager(QueryManager_ptr& queryManager)
{
	queryManager->destroy();
	queryManager = NULL;
}

void
usage(const char* name)
{
	cerr << "Usage: " << name ;
	cerr << " [--encoding <character coding>]";
	cerr << endl;
}

CORBA::Long
parse_option(int argc, char* argv[])
{
	CORBA::ULong i;
	for(i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i],"--encoding"))
		{
			++i;
			if (argv[i]) 
				encoding = argv[i]; 
			else {
				usage(argv[0]);
				return 1;
			}
		}
	}

	return 0;
}

int
main(int argc, char* argv[])
{
	if ( parse_option(argc, argv) )
		return 1;

	try 
	{
#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
		orb = CORBA::ORB_init(argc, argv, "omniORB3");
#else
		orb = CORBA::ORB_init(argc, argv);
#endif
		CORBA::Object_var poaObj =
			orb -> resolve_initial_references("RootPOA");

		initUAKGQuery2(orb); /* register valuetyre */

		PortableServer::POA_var rootPOA = 
			PortableServer::POA::_narrow(poaObj.in());

		PortableServer::POAManager_var manager = rootPOA->the_POAManager();
		
		WebSQLServlet WebSQLImpl(rootPOA.in());

		Servlet_var WebSQL = WebSQLImpl._this();

		CORBA::Object_var namingObj;

		try 
		{
			namingObj = orb->resolve_initial_references("NameService");
		}
		catch(const CORBA::ORB::InvalidName&)
		{
			std::cerr << argv[0] << ": Can't resolve initial reference "  <<
									"'NameService'";
			std::cerr << std::endl;
			return 1;
		}

		CosNaming::NamingContext_var nc = 
			CosNaming::NamingContext::_narrow(namingObj.in());

		CosNaming::Name httpServContextName;
		httpServContextName.length(1);
		httpServContextName[0].id = CORBA::string_dup("HTTPServ");
		httpServContextName[0].kind = CORBA::string_dup("");
	
		CosNaming::NamingContext_var httpServContext;

		try 
		{
			httpServContext = nc->bind_new_context(httpServContextName);
		}
		catch(const CosNaming::NamingContext::AlreadyBound& ex)
		{
			cerr << "Warning : " << argv[0] << 
				": ServiceContext already bound" << endl;
			CORBA::Object_var httpServCtxObj = nc->resolve(httpServContextName);
			httpServContext = 
				CosNaming::NamingContext::_narrow(httpServCtxObj.in());
		}

		CosNaming::Name WebSQLServletName;
		WebSQLServletName.length(1);
		WebSQLServletName[0].id = CORBA::string_dup("WebSQL");
		WebSQLServletName[0].kind = CORBA::string_dup("");
	
		try 
		{
			httpServContext -> bind(WebSQLServletName, WebSQL.in());
		}
		catch(const CosNaming::NamingContext::AlreadyBound&)
		{
			cerr << argv[0] << ": Name already bound, rebinding" << endl;
			httpServContext -> rebind(WebSQLServletName, WebSQL.in());
		}

		manager->activate();
		orb->run();

		nc->unbind(WebSQLServletName);
		nc->unbind(httpServContextName);
	} 
	catch(const CORBA::SystemException& ex)
	{
#ifdef CORBA_SYSTEM_EXCEPTION_IS_STREAMBLE
		cerr << ex << endl;
#else
		cerr << "System Exception" << endl;
#endif
		orb->destroy();
		return 1;
	}
	orb->destroy();

	return 0;
}
