set PROJECT_ROOT=c:\home\rssh\work\COS\CosQuery.1
set MICO_ROOT=c:\home\rssh\packages\mico
%MICO_ROOT%\win32-bin\idl %PROJECT_ROOT%\idl\UAKGQuery2.idl
move UAKGQuery2.h %PROJECT_ROOT%\idl\UAKGQuery2.h  
move UAKGQuery2.cc %PROJECT_ROOT%\idl\UAKGQuery2.cc


         