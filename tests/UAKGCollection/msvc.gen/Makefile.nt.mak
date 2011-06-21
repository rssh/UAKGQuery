# General make definitions and rules.
#
# part of grad Cos* build system
#
# (C) Alik Rivkind <alik@Grad.Kiev.UA>, 2000
# $Id: Makefile.nt.mak,v 1.3 2000-05-26 12:24:35 alik Exp $
#
project_root=E:/Grad/COS/CosQuery.1/tests/UAKGCollection
TARGET_DIR=E:/Grad/COS/CosQuery.1/tests/UAKGCollection/msvc.gen

ORBACUS_INCLUDE=-Id:/OOC/include -Id:/OOC/include/ob
MSVC_HOME="d:\Microsoft Visual Studio\VC98"

CXX_INCLUDE=$(MSVC_HOME)\include
LIB=$(LIB);$(MSVC_HOME)\Lib

CXX=cl
CXXFLAGS=/Zm500 /MD /W3 /GR /GX /D "WIN32" /D "INSIDE_UAKGQUERY" \
        -I$(CXX_INCLUDE) \
        -I$(project_root)/msvc.gen \
        -I$(project_root)/include \
        -I$(project_root)/../../src \
        -I$(project_root)/../../src/include \
        -I$(project_root)/../steps/include \
        -I$(project_root)/../../include \
        $(ORBACUS_INCLUDE) \
        -I$(project_root)/../../idl

LIB_NAME=$(project_root)/../../msvc.gen/CosQueryClient.lib
ORBACUS_LIB=d:\OOC\lib\ob.lib
OCI_LIB=D:\orant\OCI80\LIB\MSVC\OCI.LIB
JTC_LIB=d:\OOC\lib\jtc.lib
WINSOCK_LIB="D:\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
LIBS=$(LIB_NAME) $(ORBACUS_LIB) $(OCI_LIB) $(JTC_LIB) $(WINSOCK_LIB)

include ../test1/mk1.nt.mak

STEPS=$(TEST1)

default:$(STEPS)

all:$(STEPS)

clean:
 del *.obj *.exe