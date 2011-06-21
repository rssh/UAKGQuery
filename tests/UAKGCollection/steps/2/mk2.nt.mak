STEP2_DIR=$(project_root)/2

STEP2=$(TARGET_DIR)/step2.exe
STEP2_OBJS=$(TARGET_DIR)/step2.obj $(TARGET_DIR)/../../../steps/msvc.gen/printLib.obj

$(TARGET_DIR)/step2.obj: $(STEP2_DIR)/step2.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP2_DIR)/step2.cpp

$(TARGET_DIR)/step2.exe: $(STEP2_OBJS)
        $(CXX) $(STEP2_OBJS) $(LIBS)