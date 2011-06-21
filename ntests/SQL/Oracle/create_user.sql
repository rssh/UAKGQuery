REM script for creation of new ORACLE user 
REM part of Grad-Soft UAKGQueryService
REM (C) Ruslan Shevchenko  < Ruslan@Shevchenko.Kiev.UA>, 1998
REM (C) Grad-Soft Ltd, < http://www.gradsoft.kiev.ua> 2000-2003
REM
REM
REM default username, which used in test-scripts: t/t 
REM
REM

set define "&"

ACCEPT M_USER CHAR PROMPT 'Input username:' 
drop user &M_USER cascade;
create user &M_USER identified by &M_USER;
grant connect, resource to &M_USER;
grant create any sequence to &M_USER;
grant alter any sequence to &M_USER;
grant drop any sequence to &M_USER;

GRANT CREATE ANY DIRECTORY TO &M_USER;
GRANT DROP ANY DIRECTORY TO &M_USER;
commit;
