#-I$(CosQuery_tests_steps_DIR)/include

uakgtest1.obj: $(CosQuery_tests_UAKGCollection_DIR)/test1/uakgtest1.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/test1/uakgtest1.cpp

uakgstep1.obj: $(CosQuery_tests_UAKGCollection_DIR)/steps/1/uakgstep1.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/steps/1/uakgstep1.cpp

uakgstep2.obj: $(CosQuery_tests_UAKGCollection_DIR)/steps/2/uakgstep2.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/steps/2/uakgstep2.cpp

uakgstep3.obj: $(CosQuery_tests_UAKGCollection_DIR)/steps/3/uakgstep3.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/steps/3/uakgstep3.cpp

uakgstep4.obj: $(CosQuery_tests_UAKGCollection_DIR)/steps/4/uakgstep4.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/steps/4/uakgstep4.cpp

#uakgstep5.obj: $(CosQuery_tests_UAKGCollection_DIR)/steps/5/uakgstep5.cpp
#        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/steps/5/uakgstep5.cpp

#uakgstep6.obj: $(CosQuery_tests_UAKGCollection_DIR)/steps/6/uakgstep6.cpp
#        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_UAKGCollection_DIR)/steps/6/uakgstep6.cpp

TEST1_OBJS=uakgtest1.obj $(CosQuery_tests_steps_DIR)/printLib.obj
STEP1_OBJS=uakgstep1.obj $(CosQuery_tests_steps_DIR)/printLib.obj
STEP2_OBJS=uakgstep2.obj $(CosQuery_tests_steps_DIR)/printLib.obj
STEP3_OBJS=uakgstep3.obj $(CosQuery_tests_steps_DIR)/printLib.obj
STEP4_OBJS=uakgstep4.obj $(CosQuery_tests_steps_DIR)/printLib.obj
#STEP5_OBJS=uakgstep5.obj $(CosQuery_tests_steps_DIR)/printLib.obj
#STEP6_OBJS=uakgstep6.obj $(CosQuery_tests_steps_DIR)/printLib.obj

uakgtest1.exe: $(TEST1_OBJS)
        $(LD) $(TEST1_OBJS) $(LDFLAGS)

uakgstep1.exe: $(STEP1_OBJS)
        $(LD) $(STEP1_OBJS) $(LDFLAGS)

uakgstep2.exe: $(STEP2_OBJS)
        $(LD) $(STEP2_OBJS) $(LDFLAGS)

uakgstep3.exe: $(STEP3_OBJS)
        $(LD) $(STEP3_OBJS) $(LDFLAGS)

uakgstep4.exe: $(STEP4_OBJS)
        $(LD) $(STEP4_OBJS) $(LDFLAGS)

#uakgstep5.exe: $(STEP5_OBJS)
#        $(LD) $(STEP5_OBJS) $(LDFLAGS)

#uakgstep6.exe: $(STEP6_OBJS)
#        $(LD) $(STEP6_OBJS) $(LDFLAGS)

UAKGCollection: uakgtest1 uakgstep1 uakgstep2 uakgstep3 uakgstep4 
# uakgstep5 uakgstep6

uakgtest1: uakgtest1.exe
uakgstep1: uakgstep1.exe
uakgstep2: uakgstep2.exe
uakgstep3: uakgstep3.exe
uakgstep4: uakgstep4.exe
#uakgstep5: uakgstep5.exe
#uakgstep6: uakgstep6.exe
