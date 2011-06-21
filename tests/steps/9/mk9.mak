

STEP9_DIR=$(project_root)/9
STEP9_SRC=$(STEP9_DIR)/step9.cpp
STEP9_OBJ=$(STEP9_DIR)/step9.o
STEP9_OBJS=$(STEP9_DIR)/step9.o
STEP9_EXE=$(STEP9_DIR)/step9
STEP9=$(STEP9_EXE)

$(STEP9_OBJ): $(STEP9_SRC)
	$(CXXCOMPILE) -I../../src -I../../src/include -o $(STEP9_OBJ) -c $(STEP9_SRC)

$(STEP9): $(STEP9_OBJS)
	$(CXXCOMPILE) -o $(STEP9) $(STEP9_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

