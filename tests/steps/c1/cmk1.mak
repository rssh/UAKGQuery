

CSTEP1_DIR=$(project_root)/c1/
CSTEP1_SRC=$(CSTEP1_DIR)c1.cpp
CSTEP1_OBJ=$(CSTEP1_DIR)/c1.o
CSTEP1_OBJS=$(CSTEP1_DIR)/c1.o $(project_root)/3/printLib.o
CSTEP1_EXE=$(CSTEP1_DIR)/cc1
STEPCC1=$(CSTEP1_EXE)

$(CSTEP1_OBJ): $(CSTEP1_SRC)
	$(CXXCOMPILE) -o $(CSTEP1_OBJ) -c $(CSTEP1_SRC)

$(STEPCC1): $(CSTEP1_OBJS)
	$(CXXCOMPILE) -o $(STEPCC1) $(CSTEP1_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

