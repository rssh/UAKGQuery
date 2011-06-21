rem @echo off
rem bat file for making java library for CosQuery
rem part of grad db framework
rem (C) Alik Rivkind & Grad 2000 
rem $Id: make.bat,v 1.2 2000-02-15 13:45:15 alik Exp $
set DIRS=org ua "../jsrc"

if x%1==xclean goto clean

:default
for /F %%i in ('dir /b /s /a:d %DIRS%') do for /F %%j in ('dir /B %%i\*.java') do javac %%i\%%j
copy ..\jsrc\ua\kiev\grad\RC\*.class ua\kiev\grad\RC
goto done

:clean
for /F %%i in ('dir /b /s /a:d %DIRS%') do if exist %%i\*.class del %%i\*.class
goto done

:done
