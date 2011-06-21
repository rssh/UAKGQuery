

STEP7_DIR=$(project_root)/7

STEP7=$(STEP7_DIR)/step7
STEP7_OBJS=$(STEP7_DIR)/step7.o $(STEP3_DIR)/printLib.o

$(STEP7_DIR)/step7.o: $(STEP7_DIR)/step7.cpp
	$(CXXCOMPILE) -c -o $(STEP7_DIR)/step7.o  $(STEP7_DIR)/step7.cpp


$(STEP7_DIR)/step7: $(STEP7_OBJS)
	$(CXXCOMPILE) -o $(STEP7)  $(STEP7_OBJS) -L../../src $(UAKGQUERY_CL_LIB)  $(LIBS)

