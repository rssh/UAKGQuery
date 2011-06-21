

STEP6_DIR=$(project_root)/6

STEP6=$(STEP6_DIR)/step6
STEP6_OBJS=$(STEP6_DIR)/step6.o $(STEP3_DIR)/printLib.o

$(STEP6_DIR)/step6.o: $(STEP6_DIR)/step6.cpp
	$(CXXCOMPILE) -c -o $(STEP6_DIR)/step6.o  $(STEP6_DIR)/step6.cpp


$(STEP6_DIR)/step6: $(STEP6_OBJS)
	$(CXXCOMPILE) -o $(STEP6)  $(STEP6_OBJS) -L../../src $(UAKGQUERY_CL_LIB)  $(LIBS)

