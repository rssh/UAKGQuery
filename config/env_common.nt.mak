## main definishing part to include in several Makefiles
## $Id: env_common.nt.mak,v 1.5 2004-02-10 12:10:48 orchids Exp $
#########################################################
# Not for end user
##

PROJECT_INCLUDES= -I$(PROJECT_ROOT)\include -I$(PROJECT_ROOT)\src -I$(PROJECT_ROOT)\src\include  -I$(PROJECT_ROOT)\idl
PROJECT_LIB_DIRS= -libpath:$(PROJECT_ROOT)\src


TOOLBOX_LIBS= Threading.lib Logger.lib ProgOptions.lib ServiceOptions.lib
TOOLBOX_INCLUDES= -I$(GRAD_TOOLBOX_DIR)\include
!if "$(GRAD_TOOLBOX_DIR)\lib" == "$(GRAD_CORBA_TOOLBOX_LIBDIR)"
TOOLBOX_LIB_DIRS= -libpath:$(GRAD_TOOLBOX_DIR)\lib
!else
TOOLBOX_LIB_DIRS= -libpath:$(GRAD_TOOLBOX_DIR)\lib -libpath:$(GRAD_CORBA_TOOLBOX_LIBDIR)
!endif

                                                  
INCLUDES=-I. $(PROJECT_INCLUDES)  $(TOOLBOX_INCLUDES) $(ORB_INCLUDES) 
IDLFLAGS=-D_MSVC_VER  -D$(ORB) -DHAVE_NAMESPACES

!if "$(IDL_CLN_CPP_SUFFIX)" != "$(IDL_SRV_CPP_SUFFIX)"
IDL_SRV_ALL_OBJS= \
    $(PROJECT_ROOT)\idl\UAKGQuery2$(IDL_SRV_OBJ_SUFFIX) \
    $(PROJECT_ROOT)\idl\UAKGQuery2$(IDL_CLN_OBJ_SUFFIX)
!else                                       
IDL_SRV_ALL_OBJS= \
    $(PROJECT_ROOT)\idl\UAKGQuery2$(IDL_SRV_OBJ_SUFFIX) 
!endif

# libs
AR=lib                                    
AR_FLAGS=/nologo /out:$@
SLIB=UAKGQuery.lib
CLLIB=UAKGQueryClient.lib
EXE=UAKGQueryService.exe
RANLIB=-
# Oracle
!ifdef HAVE_ORACLE
ORACLE_INCLUDE=$(OCI_HOME)\include
OCI_LIB=$(OCI_HOME)\LIB\MSVC\OCI.LIB
!else
ORACLE_INCLUDE=.
!endif
# InterBase
!ifdef HAVE_INTERBASE
IBASE_INCLUDE=$(IBASE_HOME)\SDK\include
IBASE_LIB=$(IBASE_HOME)\SDK\lib_ms\gds32_ms.lib
!else
IBASE_INCLUDE=.
!endif
# MSVC
# project make & include
PREFIX=$(INSTALL_DIR)
MAKE=nmake
INSTALL_DATA=xcopy /s /y 
RM=del
# C
!ifdef ORB_USE_DEBUG_LIB
C_LIB_FLG=/MDd
!else
C_LIB_FLG=/MD
!endif
CC=cl /c 
CFLAGS=$(CPPFLAGS) /Gy $(C_LIB_FLG)
# cpp
CXX=cl /c /TP 
CPPFLAGS=/D "WIN32" /D "CONSOLE" /D $(ORB) 
!ifdef HAVE_INTERBASE
CPPFLAGS=$(CPPFLAGS) /D "HAVE_INTERBASE"
!endif
!ifdef HAVE_ORACLE
CPPFLAGS=$(CPPFLAGS) /D "HAVE_ORACLE"
!endif
COSQUERY_INCLUDES= $(PROJECT_INCLUDES) -I$(ORACLE_INCLUDE) -I$(IBASE_INCLUDE) -I$(GRAD_TOOLBOX_DIR)\include 
CXXFLAGS= $(C_LIB_FLG) /W3 /GR /GX $(CPPFLAGS) $(IDLCXXFLAGS)  $(COSQUERY_INCLUDES)
CXXCOMPILE=$(CXX) $(CXXFLAGS)
#exe
LD=link
LDFLAGS=$(PROJECT_LIB_DIRS) $(SLIB) $(IDL_LIBDIR) $(ORB_LIBS) $(ORB_COSNAMING_LIB) $(OCI_LIB) $(IBASE_LIB) $(TOOLBOX_LIB_DIRS) $(TOOLBOX_LIBS)
LDCLFLAGS=$(PROJECT_ROOT)\src\$(CLLIB) $(IDL_LIBDIR) $(ORB_LIBS)  $(TOOLBOX_LIB_DIRS) $(TOOLBOX_LIBS)
#dirs
CosQuery_idl_DIR=$(PROJECT_ROOT)\idl
SRC_DIR=$(PROJECT_ROOT)\src
CosQuery_src_DIR=$(SRC_DIR)
CosQuery_Common_DIR=$(SRC_DIR)\Common
CosQuery_Debug_DIR=$(SRC_DIR)\Debug
CosQuery_Decimal_DIR=$(SRC_DIR)\Decimal
CosQuery_RecordSet_DIR=$(SRC_DIR)\RecordSet
CosQuery_Numeric_DIR=$(SRC_DIR)\Numeric
CosQuery_Facade_DIR=$(SRC_DIR)\Facade
CosQuery_Pool_DIR=$(SRC_DIR)\Pool
CosQuery_include_DIR=$(SRC_DIR)\include
CosQuery_InterBase_DIR=$(SRC_DIR)\InterBase
CosQuery_Oracle8_DIR=$(SRC_DIR)\Oracle8
CosQuery_RC_DIR=$(SRC_DIR)\RC
CosQuery_UAKGQueryCollection_DIR=$(SRC_DIR)\UAKGQueryCollection
CosQuery_Utils_DIR=$(SRC_DIR)\Utils
CosQuery_Debug_DIR=$(SRC_DIR)\Facade\Debug
CosQuery_idl_DIR=$(PROJECT_ROOT)\idl
TESTS_DIR=$(PROJECT_ROOT)\tests
CosQuery_tests_DBFReader_DIR=$(TESTS_DIR)\DBFReader
CosQuery_tests_performance_DIR=$(TESTS_DIR)\performance
CosQuery_tests_SQLTool_DIR=$(TESTS_DIR)\SQLTool
CosQuery_tests_steps_DIR=$(TESTS_DIR)\steps
CosQuery_tests_UAKGCollection_DIR=$(TESTS_DIR)\UAKGCollection
NTESTS_3BASED_DIR=$(PROJECT_ROOT)\ntests\cpp\3based
NTESTS_LIB_DIR=$(PROJECT_ROOT)\ntests\cpp\lib

#install
INSTALL_EXE= UAKGQueryService.exe
INSTALL_LIB= $(CLLIB) $(SLIB)
INSTALL_CLEXE=
INSTALL_CLLIB= $(CLLIB)
#rules
.SUFFIXES: .obj .c .cpp .cc .exe .idl
.cpp.obj::
    $(CXX) $(CXXFLAGS) $<
.c.obj::
    $(CXX) $(CXXFLAGS) $<
.cc.obj::
    $(CXX) $(CXXFLAGS) $<
.idl.cpp::
    @cd $(CosQuery_idl_DIR)
    $(IDLCXX) $(INCLUDES) $(IDLFLAGS) $<
.idl.cc::
    @cd $(CosQuery_idl_DIR)
    $(IDLCXX) $(INCLUDES) $(IDLFLAGS) $<
