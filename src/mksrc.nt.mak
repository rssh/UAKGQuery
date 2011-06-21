#
CosQuery_src_SRC= $(CosQuery_src_DIR)/UAKGQueryService.cpp \
		  $(CosQuery_src_DIR)/UAKGQuerySingletonInit.cpp
#
CosQuery_src_OBJS= $(CosQuery_src_DIR)/UAKGQueryService.obj \
		   $(CosQuery_src_DIR)/UAKGQuerySingletonInit.obj
#
$(CosQuery_src_DIR)/UAKGQueryService.obj: UAKGQueryService.cpp UAKGQuerySingletonInit.cpp
