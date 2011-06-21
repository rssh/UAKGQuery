
TEST_DIR=$(project_root)/test_error 

TEST=$(TARGET_DIR)/TEST.exe
TEST_OBJS=$(TARGET_DIR)/TEST.obj $(TARGET_DIR)/printLib.obj

$(TARGET_DIR)/TEST.obj: $(TEST_DIR)/TEST.cpp
        $(CXX) $(CXXFLAGS) -c $(TEST_DIR)/TEST.cpp

$(TARGET_DIR)/TEST.exe: $(SEST_OBJS)
        $(CXX) $(SEST_OBJS) $(LIBS)