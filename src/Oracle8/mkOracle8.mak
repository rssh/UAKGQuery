
COSQUERY_ORACLE8_SRCS=\
 $(COSQUERY_ORACLE8_DIR)/OCIHandlers.cpp\
 $(COSQUERY_ORACLE8_DIR)/Oracle8Query.cpp\
 $(COSQUERY_ORACLE8_DIR)/Oracle8QueryResource.cpp\
 $(COSQUERY_ORACLE8_DIR)/Oracle8QueryManager.cpp\
 $(COSQUERY_ORACLE8_DIR)/Oracle8ConnectionManager.cpp\
 $(COSQUERY_ORACLE8_DIR)/Oracle8DBFormatHandler.cpp\
 $(COSQUERY_ORACLE8_DIR)/Oracle8Lobs.cpp\
 $(COSQUERY_ORACLE8_DIR)/OCITransSet.cpp


COSQUERY_ORACLE8_OBJS=\
 $(COSQUERY_ORACLE8_DIR)/OCIHandlers.o\
 $(COSQUERY_ORACLE8_DIR)/Oracle8Query.o\
 $(COSQUERY_ORACLE8_DIR)/Oracle8QueryManager.o\
 $(COSQUERY_ORACLE8_DIR)/Oracle8QueryResource.o\
 $(COSQUERY_ORACLE8_DIR)/Oracle8ConnectionManager.o\
 $(COSQUERY_ORACLE8_DIR)/Oracle8DBFormatHandler.o\
 $(COSQUERY_ORACLE8_DIR)/Oracle8Lobs.o\
 $(COSQUERY_ORACLE8_DIR)/OCITransSet.o



$(COSQUERY_ORACLE8_DIR)/OCIHandlers.o: $(COSQUERY_ORACLE8_DIR)/OCIHandlers.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/OCIHandlers.o -c $(COSQUERY_ORACLE8_DIR)/OCIHandlers.cpp


$(COSQUERY_ORACLE8_DIR)/Oracle8Query.o: $(COSQUERY_ORACLE8_DIR)/Oracle8Query.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/Oracle8Query.o -c $(COSQUERY_ORACLE8_DIR)/Oracle8Query.cpp

$(COSQUERY_ORACLE8_DIR)/Oracle8QueryManager.o: $(COSQUERY_ORACLE8_DIR)/Oracle8QueryManager.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/Oracle8QueryManager.o -c $(COSQUERY_ORACLE8_DIR)/Oracle8QueryManager.cpp

$(COSQUERY_ORACLE8_DIR)/Oracle8QueryResource.o: $(COSQUERY_ORACLE8_DIR)/Oracle8QueryResource.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/Oracle8QueryResource.o -c $(COSQUERY_ORACLE8_DIR)/Oracle8QueryResource.cpp

$(COSQUERY_ORACLE8_DIR)/Oracle8ConnectionManager.o: $(COSQUERY_ORACLE8_DIR)/Oracle8ConnectionManager.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/Oracle8ConnectionManager.o -c $(COSQUERY_ORACLE8_DIR)/Oracle8ConnectionManager.cpp

$(COSQUERY_ORACLE8_DIR)/Oracle8DBFormatHandler.o: $(COSQUERY_ORACLE8_DIR)/Oracle8DBFormatHandler.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/Oracle8DBFormatHandler.o -c $(COSQUERY_ORACLE8_DIR)/Oracle8DBFormatHandler.cpp


$(COSQUERY_ORACLE8_DIR)/OCITransSet.o: $(COSQUERY_ORACLE8_DIR)/OCITransSet.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/OCITransSet.o -c $(COSQUERY_ORACLE8_DIR)/OCITransSet.cpp

$(COSQUERY_ORACLE8_DIR)/Oracle8Lobs.o: $(COSQUERY_ORACLE8_DIR)/Oracle8Lobs.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_ORACLE8_DIR)/Oracle8Lobs.o -c $(COSQUERY_ORACLE8_DIR)/Oracle8Lobs.cpp
