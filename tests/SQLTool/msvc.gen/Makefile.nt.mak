# General make definitions and rules.
#
# part of grad Cos* build system
#
# (C) Alik Rivkind <alik@Grad.Kiev.UA>, 2000
# $Id: Makefile.nt.mak,v 1.7 2000-05-18 11:36:05 alik Exp $
#
project_root=E:/Grad/COS/CosQuery.1/tests/SQLTool
TARGET_DIR=E:/Grad/COS/CosQuery.1/tests/SQLTool/msvc.gen/

ORBACUS_DIR=d:\OOC
ORBACUS_INCLUDE=-I$(ORBACUS_DIR)\include -I$(ORBACUS_DIR)\include\OB

MSVC_HOME="D:\Microsoft Visual Studio\VC98"
MSVC_INCLUDE=$(MSVC_HOME)\include
LIB=$(LIB);$(MSVC_HOME)\Lib

CXX=cl
CXXFLAGS=/Zm500 /MD /W3 /GR /GX /D "WIN32" /D "ORBACUS" \
        -I$(MSVC_INCLUDE) \
        -I$(project_root) \
        -I$(project_root)/msvc.gen \
        -I$(project_root)/../steps/include \
        -I$(project_root)/../../include \
        -I$(project_root)/../../idl \
        $(ORBACUS_INCLUDE) \

LIB_NAME=../../../msvc.gen/CosQuery.lib
CLIENT_LIB_NAME=../../../msvc.gen/CosQueryClient.lib
ORBACUS_LIB=d:\OOC\lib\ob.lib
OCI_LIB=D:\orant\OCI80\LIB\MSVC\OCI.LIB
JTC_LIB=d:\OOC\lib\jtc.lib
WINSOCK_LIB="D:\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
LIBS=$(ORBACUS_LIB) $(OCI_LIB) $(JTC_LIB) $(WINSOCK_LIB)

SQLTOOL=SQLTool.exe
SQLTOOL_OBJS=\
        SQLTool.obj \
        SQLManager.obj \
        $(project_root)\..\steps\msvc.gen\printLib.obj

PERFS=$(SQLTOOL)

default: all

all:$(SQLTOOL)

clean:
 del *.obj *.exe

SQLTool.exe: $(SQLTOOL_OBJS)
        $(CXX) $(SQLTOOL_OBJS) $(LIBS) $(LIB_NAME)

SQLTool.obj: ../SQLTool.cpp
        $(CXX) $(CXXFLAGS) -c ../SQLTool.cpp

SQLManager.obj: ../SQLManager.cpp
        $(CXX) $(CXXFLAGS) -c ../SQLManager.cpp
