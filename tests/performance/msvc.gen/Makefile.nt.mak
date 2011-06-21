# General make definitions and rules.
#
# part of grad Cos* build system
#
# (C) Alik Rivkind <alik@Grad.Kiev.UA>, 2000
# $Id: Makefile.nt.mak,v 1.3 2000-02-22 09:04:04 alik Exp $
#
project_root=E:/Grad/COS/CosQuery.1/tests/performance
TARGET_DIR=E:/Grad/COS/CosQuery.1/tests/performance/msvc.gen/

ORBACUS_INCLUDE=d:\OOC\include

MSVC_HOME="D:\Microsoft Visual Studio\VC98"
MSVC_INCLUDE=$(MSVC_HOME)\include
LIB=$(LIB);$(MSVC_HOME)\Lib

CXX=cl
CXXFLAGS=/Zm500 /MDd /W3 /GR /GX /D "WIN32" \
        -I$(ORBACUS_INCLUDE) \
        -I$(MSVC_INCLUDE) \
        -I$(project_root)/msvc.gen \
        -I$(project_root)/../steps/include \
        -I$(project_root)/../../include \
        -I$(project_root)/../../idl

LIB_NAME=../../../lib/CosQuery.lib
CLIENT_LIB_NAME=../../../lib/CosQueryClient.lib
ORBACUS_LIB=d:\OOC\lib\ob.lib
OCI_LIB=D:\orant\OCI80\LIB\MSVC\OCI.LIB
JTC_LIB=d:\OOC\lib\jtc.lib
WINSOCK_LIB="D:\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
LIBS=$(ORBACUS_LIB) $(OCI_LIB) $(JTC_LIB) $(WINSOCK_LIB)

PERF1=perf1.exe
PERF1_OBJS=perf1.obj $(project_root)\..\steps\msvc.gen\printLib.obj
PERF2=perf2.exe
PERF2_OBJS=perf2.obj $(project_root)\..\steps\msvc.gen\printLib.obj
PERF3=perf3.exe
PERF3_OBJS=perf3.obj $(project_root)\..\steps\msvc.gen\printLib.obj
PERF4=perf4.exe
PERF4_OBJS=perf4.obj $(project_root)\..\steps\msvc.gen\printLib.obj

PERFS=$(PERF1) $(PERF2) $(PERF3) $(PERF4)

default:$(PERFS)

all:$(PERFS)

perf1.exe: perf1.obj
        $(CXX) $(PERF1_OBJS) $(LIBS) $(CLIENT_LIB_NAME)

perf1.obj: ../perf1.cpp
        $(CXX) $(CXXFLAGS) -c ../perf1.cpp

perf2.exe: perf2.obj
        $(CXX) $(PERF2_OBJS) $(LIBS) $(CLIENT_LIB_NAME)

perf2.obj: ../perf2.cpp
        $(CXX) $(CXXFLAGS) -c ../perf2.cpp

perf3.exe: perf3.obj
        $(CXX) $(PERF3_OBJS) $(LIBS) $(LIB_NAME)

perf3.obj: ../perf3.cpp
        $(CXX) $(CXXFLAGS) -c ../perf3.cpp

perf4.exe: perf4.obj
        $(CXX) $(PERF4_OBJS) $(LIBS) $(LIB_NAME)

perf4.obj: ../perf4.cpp
        $(CXX) $(CXXFLAGS) -c ../perf4.cpp
