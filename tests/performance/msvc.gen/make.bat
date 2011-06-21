@echo off
if 1%1==1 goto default
goto %1

:default
nmake.exe /F Makefile.nt.mak all
goto done

:all
nmake.exe /F Makefile.nt.mak %1
goto done

:clean
del *.obj
del *.exe
goto done

:done