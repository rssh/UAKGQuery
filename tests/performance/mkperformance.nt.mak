perf1.obj: $(CosQuery_tests_performance_DIR)/perf1.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_performance_DIR)/perf1.cpp

perf2.obj: $(CosQuery_tests_performance_DIR)/perf2.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_performance_DIR)/perf2.cpp

perf3.obj: $(CosQuery_tests_performance_DIR)/perf3.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_performance_DIR)/perf3.cpp

perf4.obj: $(CosQuery_tests_performance_DIR)/perf4.cpp
        $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_performance_DIR)/perf4.cpp

PERF1_OBJS= perf1.obj
PERF2_OBJS= perf2.obj
PERF3_OBJS= perf3.obj
PERF4_OBJS= perf4.obj

perf1.exe: perf1.obj
        $(LD) $(PERF1_OBJS) $(LDCLFLAGS)

perf2.exe: perf2.obj
        $(LD) $(PERF2_OBJS) $(LDCLFLAGS)

perf3.exe: perf3.obj
        $(LD) $(PERF3_OBJS) $(LDFLAGS)

perf4.exe: perf4.obj
        $(LD) $(PERF4_OBJS) $(LDFLAGS)

performance: perf1 perf2 perf3 perf4

perf1: perf1.exe
perf2: perf2.exe
perf3: perf3.exe
perf4: perf4.exe
