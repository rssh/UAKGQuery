@echo off
set OOC_IDL_PATH=-Id:\ooc\include -Id:\ooc\idl
set OB_IDL_PATH=-Id:\ob\ob3.1\ob\idl

echo Translating ...
for /F %%i in ('dir /B ..\idl\*.idl') do jidl --auto-package --no-skeletons %OB_IDL_PATH% %OOC_IDL_PATH% -I../idl ../idl/%%i
