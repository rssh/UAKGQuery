STEP1_DIR=$(project_root)/1

STEP1=$(TARGET_DIR)/step1.exe
STEP1_OBJS=$(TARGET_DIR)/step1.obj

$(TARGET_DIR)/step1.obj: $(STEP1_DIR)/step1.cpp
        $(CXX) $(CXXFLAGS) -c $(STEP1_DIR)/step1.cpp

$(TARGET_DIR)/step1.exe: $(STEP1_OBJS)
        $(CXX) $(STEP1_OBJS) $(LIBS)