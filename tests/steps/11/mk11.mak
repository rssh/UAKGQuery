

STEP11_DIR=$(project_root)/11

STEP11=$(STEP11_DIR)/step11
STEP11_OBJS=$(STEP11_DIR)/step11.o $(STEP3_DIR)/printLib.o

$(STEP11_DIR)/step11.o: $(STEP11_DIR)/step11.cpp
	$(CXXCOMPILE) -c -o $(STEP11_DIR)/step11.o  $(STEP11_DIR)/step11.cpp


$(STEP11_DIR)/step11: $(STEP11_OBJS)
	$(CXXCOMPILE) -o $(STEP11)  $(STEP11_OBJS) -L../../src $(UAKGQUERY_CL_LIB)  $(LIBS)

