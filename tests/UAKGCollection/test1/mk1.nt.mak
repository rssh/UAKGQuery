TEST1_DIR=$(project_root)/test1

TEST1=$(TARGET_DIR)/test1.exe
TEST1_OBJS=$(TARGET_DIR)/test1.obj $(TARGET_DIR)/../../steps/msvc.gen/printLib.obj

$(TARGET_DIR)/test1.obj: $(TEST1_DIR)/test1.cpp
        $(CXX) $(CXXFLAGS) -c $(TEST1_DIR)/test1.cpp

$(TARGET_DIR)/test1.exe: $(TEST1_OBJS)
        $(CXX) $(TEST1_OBJS) $(LIBS)