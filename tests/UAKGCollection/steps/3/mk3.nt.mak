STEP3_DIR=$(project_root)/3

STEP3=$(TARGET_DIR)/step3.exe
STEP3_OBJS=$(TARGET_DIR)/step3.obj $(TARGET_DIR)/../../../steps/msvc.gen/printLib.obj

$(TARGET_DIR)/step3.obj: $(STEP3_DIR)/step3.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP3_DIR)/step3.cpp

$(TARGET_DIR)/step3.exe: $(STEP3_OBJS)
        $(CXX) $(STEP3_OBJS) $(LIBS)