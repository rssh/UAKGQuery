

STEP8_DIR=$(project_root)/8

STEP8=$(STEP8_DIR)/step8
STEP8_OBJS=$(STEP8_DIR)/step8.o $(STEP3_DIR)/printLib.o

$(STEP8_DIR)/step8.o: $(STEP8_DIR)/step8.cpp
	$(CXXCOMPILE) -c -o $(STEP8_DIR)/step8.o  $(STEP8_DIR)/step8.cpp


$(STEP8_DIR)/step8: $(STEP8_OBJS)
	$(CXXCOMPILE) -o $(STEP8)  $(STEP8_OBJS) -L../../src $(UAKGQUERY_CL_LIB)  $(LIBS)

