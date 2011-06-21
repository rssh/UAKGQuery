

STEPCC2_DIR=$(project_root)/cc2

STEPCC2=$(STEPCC2_DIR)/c2
STEPCC2_OBJS=$(STEPCC2_DIR)/c2.o $(STEP3_DIR)/printLib.o

$(STEPCC2_DIR)/c2.o: $(STEPCC2_DIR)/c2.cpp
	$(CXXCOMPILE) -c -o $(STEPCC2_DIR)/c2.o  $(STEPCC2_DIR)/c2.cpp


$(STEPCC2_DIR)/c2: $(STEPCC2_OBJS)
	$(CXXCOMPILE) -o $(STEPCC2)  $(STEPCC2_OBJS) -L../../src $(UAKGQUERY_CL_LIB)  $(LIBS)

