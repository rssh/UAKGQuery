# Generated! 
#
CosQuery_DBFReader_SRC= \
    $(CosQuery_tests_DBFReader_DIR)/DBFforOracle.cpp \
    $(CosQuery_tests_DBFReader_DIR)/DBFHeader.cpp \
    $(CosQuery_tests_DBFReader_DIR)/DBFReader.cpp \
    $(CosQuery_tests_DBFReader_DIR)/RecordSetFactory.cpp    
#
CosQuery_DBFReader_OBJS= \
    DBFReader.obj \
    DBFforOracle.obj \
    DBFHeader.obj \
    RecordSetFactory.obj
#
DBFforOracle.obj: $(CosQuery_tests_DBFReader_DIR)/DBFforOracle.cpp
    $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_DBFReader_DIR)/DBFforOracle.cpp

DBFHeader.obj: $(CosQuery_tests_DBFReader_DIR)/DBFHeader.cpp
    $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_DBFReader_DIR)/DBFHeader.cpp

DBFReader.obj: $(CosQuery_tests_DBFReader_DIR)/DBFReader.cpp
    $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_DBFReader_DIR)/DBFReader.cpp

RecordSetFactory.obj: $(CosQuery_tests_DBFReader_DIR)/RecordSetFactory.cpp
    $(CXX) $(CXXFLAGS) -I$(CosQuery_tests_steps_DIR)/include -c $(CosQuery_tests_DBFReader_DIR)/RecordSetFactory.cpp

DBFReader.exe: $(CosQuery_DBFReader_OBJS)
        $(LD) $(CosQuery_DBFReader_OBJS) $(LDCLFLAGS) 

DBFReader: DBFReader.exe
