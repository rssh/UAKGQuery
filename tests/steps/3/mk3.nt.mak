STEP3_DIR=$(project_root)/3

STEP3=$(TARGET_DIR)/STEP3.exe
STEP3_OBJS=$(TARGET_DIR)/STEP3.obj $(TARGET_DIR)/printLib.obj

$(TARGET_DIR)/STEP3.obj: $(STEP3_DIR)/STEP3.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP3_DIR)/STEP3.cpp

$(TARGET_DIR)/printLib.obj: $(STEP3_DIR)/printLib.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP3_DIR)/printLib.cpp

$(TARGET_DIR)/STEP3.exe: $(STEP3_OBJS)
        $(CXX) $(STEP3_OBJS) $(LIBS)