STEP3_DIR=$(project_root)/3

STEP3=$(STEP3_DIR)/step3
STEP3_OBJS=$(STEP3_DIR)/step3.o $(STEP3_DIR)/printLib.o

$(STEP3_DIR)/step3.o: $(STEP3_DIR)/step3.cpp
	$(CXXCOMPILE) -c -o $(STEP3_DIR)/step3.o  $(STEP3_DIR)/step3.cpp

$(STEP3_DIR)/printLib.o: $(STEP3_DIR)/printLib.cpp
	$(CXXCOMPILE) -c -o $(STEP3_DIR)/printLib.o  $(STEP3_DIR)/printLib.cpp

$(STEP3_DIR)/step3: $(STEP3_OBJS)
	$(CXXCOMPILE) -o $(STEP3)  $(STEP3_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

