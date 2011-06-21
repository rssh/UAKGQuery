/*
 * (C) GradSoft, 2000 - 2001
 * http://www.gradsoft.com.ua
 * (c) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>. 2000
 * (c) Alik Rivkind alik@grad.kiev.ua 2000
 * $Id: SQLTool.cpp,v 1.27 2002-08-23 20:17:13 rssh Exp $
 */
#include <SQLManager.h>
#include <iostream>


using namespace std;
using namespace UAKGQuery2;

CORBA::ORB_ptr myORB;

static char main_query_text[100];
static char main_command_str[100];



void showHelp(void);

int main(int argc, char** argv)
{

 int retval=0;
 CORBA::ORB_var orb;
 int orbInitialized=0;
 try
 {

#ifdef CORBA_HAVE_OTS
#ifdef ORBACUS
  OB::OTSInit(argc,argv);
#endif
#endif

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
  orb = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
  orb = CORBA::ORB_init(argc, argv);
#endif
  orbInitialized=1;
  myORB=orb.in();
  initUAKGQuery2(myORB);

  CORBA::Object_var obj;
  try 
  { obj = orb->resolve_initial_references("UAKGQueryService"); }
  catch(const CORBA::ORB::InvalidName&)
  {
    cerr << argv[0] << ": can't resolve UAKGQueryService" << endl;
    throw FatalException(1);
  }
     
  if(CORBA::is_nil(obj)) 
  {
   cerr << argv[0] << ": UAKGQueryService is a nil object reference" << endl;
   throw FatalException(1);
  }

  DBConnectionManager_var dbManager = DBConnectionManager::_narrow(obj);
  if (CORBA::is_nil(dbManager)) 
  {
   cerr << argv[0] << ": can't narrow dbManager to correct type" << endl;
   throw FatalException(1);
  } 


  SQLManager manager;
  cerr << 2 << endl;
  for( ;; )
  {
   char *query_text = main_query_text;
   char *command_str = main_command_str;

   if (!manager.is_connected()) {
       cout << "enter connect string or 'h' to help:";
   }else{
       cout << "enter query or 'h' to help:";
   }
   cout.flush();
   cin.getline(query_text, 100, '\n');
   for( ; *query_text == ' '; query_text++) ;
   if (query_text[0]=='\0') continue; 
   if (strcmp(query_text, "q")==0) break;
   if (strcmp(query_text, "h")==0){
      showHelp();
      continue;
   }
   int i;
   for(i=0; (query_text[i] != ' ') && (query_text[i] != '\0'); i++) 
     command_str[i] = query_text[i];
   command_str[i] = '\0';
   if( strcmp( command_str, "connect" )==0 ) 
   { 
    manager.connect(dbManager.in(), query_text+i); 
    continue;
   }
   if( strcmp( command_str, "disconnect" )==0 ) 
   {
    manager.disconnect(); 
    continue;
   }
#ifdef CORBA_HAVE_OTS
   if ( strcmp( command_str, "begin" )==0 ) {
    manager.begin(); 
    continue;
   }
   if ( strcmp( command_str, "rollback" )==0 ) {
    manager.rollback(); 
    continue;
   }
   if ( strcmp( command_str, "commit" )==0 ) {
    manager.commit(); 
    continue;
   }
#endif
   manager.execute_query( query_text );
  }

  if( manager.is_connected() ) 
      manager.disconnect(); 
 } catch(const CORBA::SystemException& ex) {
  cerr << ex << endl;
  retval=1;
 } catch(const FatalException& ex) {
  retval=ex.errCode;
 }
 if (orbInitialized)
    orb->destroy();

 return retval;
}

void showHelp(void)
{
 cout << "Usage:" << endl;
 cout << "connect <user>,<pwd>,[<alias>],<driver>[,<flags>] - to [re]connect to DB" << endl;
 cout << "disconnect - to disconnect from the DB" << endl;
 cout << "any ANSI SQL sentences to execute when connected" << endl;
#ifdef CORBA_HAVE_OTS
 cout << "begin - start transaction" << endl;
 cout << "rollback - rollback current transaction" << endl;
 cout << "commit - commit current transaction" << endl;
#endif
 cout << "q - to quit" << endl;
 cout << "h - to see this help" << endl;
}
