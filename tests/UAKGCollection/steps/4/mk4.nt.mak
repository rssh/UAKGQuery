STEP4_DIR=$(project_root)/4

STEP4=$(TARGET_DIR)/step4.exe
STEP4_OBJS=$(TARGET_DIR)/step4.obj $(TARGET_DIR)/../../../steps/msvc.gen/printLib.obj

$(TARGET_DIR)/step4.obj: $(STEP4_DIR)/step4.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP4_DIR)/step4.cpp

$(TARGET_DIR)/step4.exe: $(STEP4_OBJS)
        $(CXX) $(STEP4_OBJS) $(LIBS)