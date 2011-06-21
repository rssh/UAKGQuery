step1.obj: $(CosQuery_tests_steps_DIR)/1/step1.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/1/step1.cpp

step2.obj: $(CosQuery_tests_steps_DIR)/2/step2.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/2/step2.cpp

step3.obj: $(CosQuery_tests_steps_DIR)/3/step3.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/3/step3.cpp

printLib.obj: $(CosQuery_tests_steps_DIR)/3/printLib.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/3/printLib.cpp

step4.obj: $(CosQuery_tests_steps_DIR)/4/step4.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/4/step4.cpp

step5.obj: $(CosQuery_tests_steps_DIR)/5/step5.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/5/step5.cpp

step6.obj: $(CosQuery_tests_steps_DIR)/6/step6.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/6/step6.cpp

step7.obj: $(CosQuery_tests_steps_DIR)/7/step7.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/7/step7.cpp

step8.obj: $(CosQuery_tests_steps_DIR)/8/step8.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/8/step8.cpp

step9.obj: $(CosQuery_tests_steps_DIR)/9/step9.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/9/step9.cpp

step10.obj: $(CosQuery_tests_steps_DIR)/10/step10.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/10/step10.cpp

step13.obj: $(CosQuery_tests_steps_DIR)/13/step13.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/13/step13.cpp

test.obj:  $(CosQuery_tests_steps_DIR)/test_error/test.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/test_error/test.cpp

c4.obj: $(CosQuery_tests_steps_DIR)/c4/c4.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/c4/c4.cpp

c5.obj: $(CosQuery_tests_steps_DIR)/c5/c5.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/c5/c5.cpp

c6.obj: $(CosQuery_tests_steps_DIR)/c6/c6.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_steps_DIR)/c6/c6.cpp

STEP1_OBJS=step1.obj
STEP2_OBJS=step2.obj printLib.obj
STEP3_OBJS=step3.obj printLib.obj
STEP4_OBJS=step4.obj printLib.obj
STEP5_OBJS=step5.obj printLib.obj
STEP6_OBJS=step6.obj printLib.obj
STEP7_OBJS=step7.obj
STEP8_OBJS=step8.obj
STEP9_OBJS=step9.obj
STEP10_OBJS=step10.obj
#STEP13_OBJS=step13.obj
STEP13_OBJS=

TEST_OBJS = test.obj
C4_OBJS=c4.obj printlib.obj
C5_OBJS=c5.obj printlib.obj
C6_OBJS=c6.obj printlib.obj

step1.exe: $(STEP1_OBJS)
        $(LD) $(STEP1_OBJS) $(LDCLFLAGS)

step2.exe: $(STEP2_OBJS)
        $(LD) $(STEP2_OBJS) $(LDCLFLAGS)

step3.exe: $(STEP3_OBJS)
        $(LD) $(STEP3_OBJS) $(LDCLFLAGS)

step4.exe: $(STEP4_OBJS)
        $(LD) $(STEP4_OBJS) $(LDCLFLAGS)

step5.exe: $(STEP5_OBJS)
        $(LD) $(STEP5_OBJS) $(LDCLFLAGS)

step6.exe: $(STEP6_OBJS)
        $(LD) $(STEP6_OBJS) $(LDCLFLAGS)

step7.exe: $(STEP7_OBJS)
        $(LD) $(STEP7_OBJS) $(LDCLFLAGS)

step8.exe: $(STEP8_OBJS)
        $(LD) $(STEP8_OBJS) $(LDCLFLAGS)

step9.exe: $(STEP9_OBJS)
        $(LD) $(STEP9_OBJS) $(LDCLFLAGS)

step10.exe: $(STEP10_OBJS)
        $(LD) $(STEP10_OBJS) $(LDCLFLAGS)

step13.exe: $(STEP13_OBJS)
        $(LD) $(STEP13_OBJS) $(LDCLFLAGS)

test.exe: $(TEST_OBJS)
        $(LD) $(TEST_OBJS) $(LDCLFLAGS)


c4.exe: $(C4_OBJS)
        $(LD) $(C4_OBJS) $(LDCLFLAGS)

c5.exe: $(C5_OBJS)
        $(LD) $(C5_OBJS) $(LDCLFLAGS)

c6.exe: $(C6_OBJS)
        $(LD) $(C6_OBJS) $(LDCLFLAGS)

steps: step1 step2 step3 step4 step5 step6 step7 step8 step9 step10 


step1: step1.exe
step2: step2.exe
step3: step3.exe
step4: step4.exe
step5: step5.exe
step6: step6.exe
step7: step7.exe
step8: step8.exe
step9: step9.exe
step10: step10.exe
step13: step13.exe
test:test.exe

c5: c5.exe
c4: c4.exe
c6: c6.exe
