STEP4_DIR=$(project_root)/4

STEP4=$(STEP4_DIR)/step4
STEP4_OBJS=$(STEP4_DIR)/step4.o $(STEP3_DIR)/printLib.o

$(STEP4_DIR)/step4.o: $(STEP4_DIR)/step4.cpp
	$(CXXCOMPILE) -c -o $(STEP4_DIR)/step4.o  $(STEP4_DIR)/step4.cpp

$(STEP4_DIR)/step4: $(STEP4_OBJS)
	$(CXXCOMPILE) -o $(STEP4)  $(STEP4_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

