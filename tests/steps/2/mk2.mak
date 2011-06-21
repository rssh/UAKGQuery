STEP2_DIR=$(project_root)/2

STEP2=$(STEP2_DIR)/step2
STEP2_OBJS=$(STEP2_DIR)/step2.o 

$(STEP2_DIR)/step2.o: $(STEP2_DIR)/step2.cpp
	$(CXXCOMPILE) -c -o $(STEP2_DIR)/step2.o  $(STEP2_DIR)/step2.cpp

$(STEP2_DIR)/step2: $(STEP2_DIR)/step2.o
	$(CXXCOMPILE) -o $(STEP2)  $(STEP2_OBJS) -L../../src $(UAKGQUERY_CL_LIB)  $(LIBS)

