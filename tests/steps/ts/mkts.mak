TS_DIR=$(project_root)/ts

TS_27_06_2008=$(TS_DIR)/ts_27_06_2008
TS_27_06_2008_OBJS=$(TS_DIR)/ts_27_06_2008.o \
  $(STEP3_DIR)/printLib.o

$(TS_DIR)/ts_27_06_2008.o: $(TS_DIR)/ts_27_06_2008.cpp
	$(CXXCOMPILE) -c -o $(TS_DIR)/ts_27_06_2008.o  $(TS_DIR)/ts_27_06_2008.cpp

$(TS_DIR)/ts_27_06_2008: $(TS_27_06_2008_OBJS)
	$(CXXCOMPILE) -o $(TS_27_06_2008)  $(TS_27_06_2008_OBJS) -L../../src $(UAKGQUERY_CL_LIB) $(LIBS)

