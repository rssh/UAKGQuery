

STEP5_DIR=$(project_root)/5

STEP5=$(STEP5_DIR)/step5
STEP5_OBJS=$(STEP5_DIR)/step5.o $(STEP3_DIR)/printLib.o

$(STEP5_DIR)/step5.o: $(STEP5_DIR)/step5.cpp
	$(CXXCOMPILE) -c -o $(STEP5_DIR)/step5.o  $(STEP5_DIR)/step5.cpp


$(STEP5_DIR)/step5: $(STEP5_OBJS)
	$(CXXCOMPILE) -o $(STEP5)  $(STEP5_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

