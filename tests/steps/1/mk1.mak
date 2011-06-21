

STEP1_DIR=$(project_root)/1

STEP1=$(STEP1_DIR)/step1
STEP1_OBJS=$(STEP1_DIR)/step1.o

$(STEP1_OBJS): $(STEP1_DIR)/step1.cpp
	$(CXXCOMPILE) -c -o $(STEP1_DIR)/step1.o  $(STEP1_DIR)/step1.cpp

$(STEP1): $(STEP1_OBJS)
	$(CXXCOMPILE)  -o $(STEP1)  $(STEP1_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

