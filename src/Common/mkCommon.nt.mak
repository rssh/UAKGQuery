# Generated! 
#
CosQuery_Common_SRC= \
	$(CosQuery_Common_DIR)/CosQueryServer.cpp \
	$(CosQuery_Common_DIR)/DBBind.cpp \
	$(CosQuery_Common_DIR)/DBConnectionManager.cpp \
	$(CosQuery_Common_DIR)/UAKGQueryBase.cpp \
	$(CosQuery_Common_DIR)/UAKGQueryManagerBase.cpp \
	$(CosQuery_Common_DIR)/UAKGQueryUtils.cpp \
	$(CosQuery_Common_DIR)/StringFlagSet.cpp \
        $(CosQuery_Common_DIR)/UAKGLobs.cpp \
        $(CosQuery_Common_DIR)/DBConnectionServantActivator.cpp \
        $(CosQuery_Common_DIR)/Nurser.cpp 
#
CosQuery_Common_OBJS= \
	$(CosQuery_Common_DIR)/CosQueryServer.obj \
	$(CosQuery_Common_DIR)/DBBind.obj \
	$(CosQuery_Common_DIR)/DBConnectionManager.obj \
	$(CosQuery_Common_DIR)/UAKGQueryBase.obj \
	$(CosQuery_Common_DIR)/UAKGQueryManagerBase.obj \
	$(CosQuery_Common_DIR)/UAKGQueryUtils.obj \
	$(CosQuery_Common_DIR)/StringFlagSet.obj \
        $(CosQuery_Common_DIR)/UAKGLobs.obj \
        $(CosQuery_Common_DIR)/DBConnectionServantActivator.obj \
        $(CosQuery_Common_DIR)/Nurser.obj
#
CosQuery_Common_CLSRCS=
CosQuery_Common_CLOBJS=$(CosQuery_Common_DIR)/UAKGQueryUtils.obj


$(CosQuery_Common_DIR)/DBBind.obj: DBBind.cpp
$(CosQuery_Common_DIR)/DBConnectionManager.obj: DBConnectionManager.cpp
$(CosQuery_Common_DIR)/UAKGQueryBase.obj: UAKGQueryBase.cpp
$(CosQuery_Common_DIR)/UAKGQueryManagerBase.obj: UAKGQueryManagerBase.cpp
$(CosQuery_Common_DIR)/UAKGQueryUtils.obj: UAKGQueryUtils.cpp
$(CosQuery_Common_DIR)/CosQueryServer.obj: CosQueryServer.cpp
$(CosQuery_Common_DIR)/StringFlagSet.obj: StringFlagSet.cpp
$(CosQuery_Common_DIR)/UAKGLobs.obj: UAKGLobs.cpp 
$(CosQuery_Common_DIR)/DBConnectionServantActivator.obj: DBConnectionServantActivator.cpp
$(CosQuery_Common_DIR)/Nurser.obj: Nurser.cpp 
