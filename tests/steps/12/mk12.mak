                                     

STEP12_DIR=$(project_root)/12
STEP12_SRC=$(STEP12_DIR)/step12.cpp
STEP12_OBJ=$(STEP12_DIR)/step12.o
STEP12_OBJS=$(STEP12_DIR)/step12.o
STEP12_EXE=$(STEP12_DIR)/step12
STEP12=$(STEP12_EXE)

$(STEP12_OBJ): $(STEP12_SRC)
	$(CXXCOMPILE) -I../../src -I../../src/include -o $(STEP12_OBJ) -c $(STEP12_SRC)

$(STEP12): $(STEP12_OBJS)
	$(CXXCOMPILE) -o $(STEP12) $(STEP12_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

