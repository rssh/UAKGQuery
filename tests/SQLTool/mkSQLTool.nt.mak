SQLTool.obj: $(CosQuery_tests_SQLTool_DIR)/SQLTool.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_SQLTool_DIR) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_SQLTool_DIR)/SQLTool.cpp

SQLManager.obj: $(CosQuery_tests_SQLTool_DIR)/SQLManager.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_SQLTool_DIR) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_SQLTool_DIR)/SQLManager.cpp

SQLTOOL_OBJS= SQLTool.obj SQLManager.obj $(CosQuery_tests_steps_DIR)/printLib.obj

SQLTool.exe: $(SQLTOOL_OBJS)
        $(LD) $(SQLTOOL_OBJS) $(LDCLFLAGS)

SQLTool: SQLTool.exe
