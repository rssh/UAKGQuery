
QueryApplication.obj: $(NTESTS_LIB_DIR)/QueryApplication.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_LIB_DIR)/QueryApplication.cpp

QueryApplication_OBJS=QueryApplication.obj

QueryApplication_lib: client-lib

client-lib: QueryApplication.lib

QueryApplication.lib: $(QueryApplication_OBJS)
    $(AR) $(AR_FLAGS) $(QueryApplication_OBJS)

