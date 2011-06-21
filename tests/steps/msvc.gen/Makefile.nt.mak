# General make definitions and rules.
#
# part of grad Cos* build system
#
# (C) Alik Rivkind <alik@Grad.Kiev.UA>, 2000
# $Id: Makefile.nt.mak,v 1.11 2000-08-04 15:03:59 srkr Exp $
#
project_root=D:/SK/work/COS/CosQuery.1/tests/steps
TARGET_DIR=D:\SK\work\COS\CosQuery.1\tests\steps\msvc.gen\

ORBACUS_HOME=C:\OOC
ORBACUS_INCLUDE=-I$(ORBACUS_HOME)\include -I$(ORBACUS_HOME)\include\OB
#MSVC_HOME="d:\Microsoft Visual Studio\VC98"
MSVC_HOME="D:\Program Files\Microsoft Visual Studio\VC98\" 

CXX_INCLUDE=$(MSVC_HOME)\include
LIB=$(LIB);$(MSVC_HOME)\Lib

CXX=cl
CXXFLAGS=/Zm500 /MD /W3 /GR /GX /D "WIN32" \
        -I$(CXX_INCLUDE) \
        -I$(project_root)/msvc.gen \
        -I$(project_root)/include \
        -I$(project_root)/../../include \
        -I$(project_root)/../../idl \
        $(ORBACUS_INCLUDE)

LIB_NAME=../../../msvc.gen/CosQueryClient.lib
ORBACUS_LIB=C:\OOC\lib\ob.lib
JTC_LIB=C:\OOC\lib\jtc.lib
WINSOCK_LIB="D:\Program Files\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
LIBS=$(LIB_NAME) $(ORBACUS_LIB) $(JTC_LIB) $(WINSOCK_LIB)

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