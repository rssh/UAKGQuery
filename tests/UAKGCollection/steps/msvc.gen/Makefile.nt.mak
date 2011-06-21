# General make definitions and rules.
#
# part of grad Cos* build system
#
# (C) Alik Rivkind <alik@Grad.Kiev.UA>, 2000
# $Id: Makefile.nt.mak,v 1.5 2000-04-19 13:29:32 alik Exp $
#
project_root=E:/Grad/COS/CosQuery.1/tests/UAKGCollection/steps
TARGET_DIR=E:/Grad/COS/CosQuery.1/tests/UAKGCollection/steps/msvc.gen

ORBACUS_INCLUDE=d:\OOC\include
MSVC_HOME="d:\Microsoft Visual Studio\VC98"

CXX_INCLUDE=$(MSVC_HOME)\include
LIB=$(LIB);$(MSVC_HOME)\Lib

CXX=cl
CXXFLAGS=/Zm500 /MD /W3 /GR /GX /D "WIN32" \
        -I$(ORBACUS_INCLUDE) \
        -I$(CXX_INCLUDE) \
        -I$(project_root)/msvc.gen \
        -I$(project_root)/include \
        -I$(project_root)/../../../src \
        -I$(project_root)/../../../src/include \
        -I$(project_root)/../../steps/include \
        -I$(project_root)/../../../include \
        -I$(project_root)/../../../idl

LIB_NAME=../../../../msvc.gen/CosQueryClient.lib
ORBACUS_LIB=d:\OOC\lib\ob.lib
OCI_LIB=D:\orant\OCI80\LIB\MSVC\OCI.LIB
JTC_LIB=d:\OOC\lib\jtc.lib
WINSOCK_LIB="D:\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
LIBS=$(LIB_NAME) $(ORBACUS_LIB) $(OCI_LIB) $(JTC_LIB) $(WINSOCK_LIB)

include ../1/mk1.nt.mak
include ../2/mk2.nt.mak
include ../3/mk3.nt.mak
include ../4/mk4.nt.mak
include ../5/mk5.nt.mak
include ../6/mk6.nt.mak

STEPS=$(STEP1) $(STEP2) $(STEP3) $(STEP4) $(STEP5) $(STEP6)

default:$(STEPS)

all:$(STEPS)

clean:
 del *.obj *.exe