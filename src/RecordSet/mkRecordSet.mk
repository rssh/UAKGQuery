
COSQUERY_RECORDSET_OBJS=\
 $(COSQUERY_RECORDSET_DIR)/RecordSetImpl.o

$(COSQUERY_RECORDSET_OBJS): $(COSQUERY_RECORDSET_DIR)/%.o: $(COSQUERY_RECORDSET_DIR)/%.cpp
	$(CXXLIBCOMPILE) -c $< -o $@