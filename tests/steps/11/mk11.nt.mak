STEP11_DIR=$(project_root)/11

STEP11=$(TARGET_DIR)/STEP11.exe
STEP11_OBJS=$(TARGET_DIR)/STEP11.obj $(TARGET_DIR)/printLib.obj

$(TARGET_DIR)/STEP11.obj: $(STEP11_DIR)/STEP11.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP11_DIR)/STEP11.cpp

$(TARGET_DIR)/STEP11.exe: $(STEP11_OBJS)
        $(CXX) $(STEP11_OBJS) $(LIBS)