NTEST_LIBDIR = -libpath:"$(PROJECT_ROOT)/ntests/cpp/lib"

t1.obj: $(NTESTS_3BASED_DIR)/t1.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1.cpp

t1007.obj: $(NTESTS_3BASED_DIR)/t1007.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1007.cpp

t1013.obj: $(NTESTS_3BASED_DIR)/t1013.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1013.cpp

t1015.obj: $(NTESTS_3BASED_DIR)/t1015.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1015.cpp

t1016.obj: $(NTESTS_3BASED_DIR)/t1016.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1016.cpp

t1037.obj: $(NTESTS_3BASED_DIR)/t1037.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1037.cpp

t1539.obj: $(NTESTS_3BASED_DIR)/t1539.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1539.cpp

t1560.obj: $(NTESTS_3BASED_DIR)/t1560.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1560.cpp

t1667.obj: $(NTESTS_3BASED_DIR)/t1667.cpp
        $(CXX) $(CXXFLAGS) -I$(PROJECT_ROOT)/ntests/cpp/include -c $(NTESTS_3BASED_DIR)/t1667.cpp

t1_OBJS=t1.obj
t1007_OBJS=t1007.obj
t1013_OBJS=t1013.obj
t1015_OBJS=t1015.obj
t1016_OBJS=t1016.obj
t1037_OBJS=t1037.obj
t1539_OBJS=t1539.obj
t1560_OBJS=t1560.obj
t1667_OBJS=t1667.obj

t1.exe: $(t1_OBJS)
        $(LD) $(t1_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1007.exe: $(t1007_OBJS)
        $(LD) $(t1007_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1013.exe: $(t1013_OBJS)
        $(LD) $(t1013_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1015.exe: $(t1015_OBJS)
        $(LD) $(t1015_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1016.exe: $(t1016_OBJS)
        $(LD) $(t1016_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1037.exe: $(t1037_OBJS)
        $(LD) $(t1037_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1539.exe: $(t1539_OBJS)
        $(LD) $(t1539_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1560.exe: $(t1560_OBJS)
        $(LD) $(t1560_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

t1667.exe: $(t1667_OBJS)
        $(LD) $(t1667_OBJS) $(LDCLFLAGS) QueryApplication.lib $(NTEST_LIBDIR)

ntests: t1 t1007 t1013 t1015 t1016 t1037 t1539 t1560 t1667

t1: t1.exe
t1007: t1007.exe
t1013: t1013.exe
t1015: t1015.exe
t1016: t1016.exe
t1037: t1037.exe
t1539: t1539.exe
t1560: t1560.exe
t1667: t1667.exe

