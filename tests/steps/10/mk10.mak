#
# part of GradSoft UAKGQuery.
#

STEP10_DIR=$(project_root)/10
STEP10_SRC=$(STEP10_DIR)/step10.cpp
STEP10_OBJ=$(STEP10_DIR)/step10.o
STEP10_OBJS=$(STEP10_DIR)/step10.o
STEP10_EXE=$(STEP10_DIR)/step10
STEP10=$(STEP10_EXE)

default: $(STEP10)

$(STEP10_OBJ): $(STEP10_SRC)
	$(CXXCOMPILE) -I../../src -I../../src/include -o $(STEP10_OBJ) -c $(STEP10_SRC)

$(STEP10): $(STEP10_OBJS)
	$(CXXCOMPILE) -o $(STEP10) $(STEP10_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

