
COSQUERY_COMMON_SRCS=\
  $(COSQUERY_COMMON_DIR)/DBBind.cpp\
  $(COSQUERY_COMMON_DIR)/DBConnectionManager.cpp\
  $(COSQUERY_COMMON_DIR)/DBConnectionServantActivator.cpp\
  $(COSQUERY_COMMON_DIR)/UAKGQueryManagerBase.cpp\
  $(COSQUERY_COMMON_DIR)/UAKGQueryBase.cpp\
  $(COSQUERY_COMMON_DIR)/UAKGQueryUtils.cpp\
  $(COSQUERY_COMMON_DIR)/UAKGLobs.cpp\
  $(COSQUERY_COMMON_DIR)/CosQueryServer.cpp\
  $(COSQUERY_COMMON_DIR)/StringFlagSet.cpp\
  $(COSQUERY_COMMON_DIR)/Nurser.cpp

COSQUERY_COMMON_OBJS=\
  $(COSQUERY_COMMON_DIR)/DBBind.o\
  $(COSQUERY_COMMON_DIR)/DBConnectionManager.o\
  $(COSQUERY_COMMON_DIR)/DBConnectionServantActivator.o\
  $(COSQUERY_COMMON_DIR)/UAKGQueryManagerBase.o\
  $(COSQUERY_COMMON_DIR)/UAKGQueryBase.o\
  $(COSQUERY_COMMON_DIR)/UAKGQueryUtils.o\
  $(COSQUERY_COMMON_DIR)/UAKGLobs.o\
  $(COSQUERY_COMMON_DIR)/CosQueryServer.o\
  $(COSQUERY_COMMON_DIR)/StringFlagSet.o\
  $(COSQUERY_COMMON_DIR)/Nurser.o


$(COSQUERY_COMMON_DIR)/UAKGQueryManagerBase.o:$(COSQUERY_COMMON_DIR)/UAKGQueryManagerBase.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/UAKGQueryManagerBase.o -c $(COSQUERY_COMMON_DIR)/UAKGQueryManagerBase.cpp


$(COSQUERY_COMMON_DIR)/UAKGQueryBase.o:$(COSQUERY_COMMON_DIR)/UAKGQueryBase.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/UAKGQueryBase.o -c $(COSQUERY_COMMON_DIR)/UAKGQueryBase.cpp

$(COSQUERY_COMMON_DIR)/UAKGQueryUtils.o:$(COSQUERY_COMMON_DIR)/UAKGQueryUtils.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/UAKGQueryUtils.o -c $(COSQUERY_COMMON_DIR)/UAKGQueryUtils.cpp

$(COSQUERY_COMMON_DIR)/UAKGLobs.o:$(COSQUERY_COMMON_DIR)/UAKGLobs.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/UAKGLobs.o -c $(COSQUERY_COMMON_DIR)/UAKGLobs.cpp

$(COSQUERY_COMMON_DIR)/DBBind.o:$(COSQUERY_COMMON_DIR)/DBBind.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/DBBind.o -c $(COSQUERY_COMMON_DIR)/DBBind.cpp

$(COSQUERY_COMMON_DIR)/DBConnectionManager.o:$(COSQUERY_COMMON_DIR)/DBConnectionManager.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/DBConnectionManager.o -c $(COSQUERY_COMMON_DIR)/DBConnectionManager.cpp

$(COSQUERY_COMMON_DIR)/CosQueryServer.o:$(COSQUERY_COMMON_DIR)/CosQueryServer.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/CosQueryServer.o -c $(COSQUERY_COMMON_DIR)/CosQueryServer.cpp

$(COSQUERY_COMMON_DIR)/StringFlagSet.o:$(COSQUERY_COMMON_DIR)/StringFlagSet.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/StringFlagSet.o -c $(COSQUERY_COMMON_DIR)/StringFlagSet.cpp

$(COSQUERY_COMMON_DIR)/DBConnectionServantActivator.o:$(COSQUERY_COMMON_DIR)/DBConnectionServantActivator.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/DBConnectionServantActivator.o -c $(COSQUERY_COMMON_DIR)/DBConnectionServantActivator.cpp

$(COSQUERY_COMMON_DIR)/Nurser.o:$(COSQUERY_COMMON_DIR)/Nurser.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_COMMON_DIR)/Nurser.o -c $(COSQUERY_COMMON_DIR)/Nurser.cpp
