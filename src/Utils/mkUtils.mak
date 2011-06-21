
COSQUERY_UTILS_SRCS=\
	$(COSQUERY_UTILS_DIR)/SzArray.cpp \
	$(COSQUERY_UTILS_DIR)/SQLTextUtils.cpp \
	$(COSQUERY_UTILS_DIR)/SQLTerm.cpp \
       

COSQUERY_UTILS_OBJS=\
	$(COSQUERY_UTILS_DIR)/SzArray.o \
	$(COSQUERY_UTILS_DIR)/SQLTextUtils.o \
	$(COSQUERY_UTILS_DIR)/SQLTerm.o

$(COSQUERY_UTILS_DIR)/SzArray.o:$(COSQUERY_UTILS_DIR)/SzArray.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_UTILS_DIR)/SzArray.o -c $(COSQUERY_UTILS_DIR)/SzArray.cpp

$(COSQUERY_UTILS_DIR)/SQLTextUtils.o:$(COSQUERY_UTILS_DIR)/SQLTextUtils.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_UTILS_DIR)/SQLTextUtils.o -c $(COSQUERY_UTILS_DIR)/SQLTextUtils.cpp

$(COSQUERY_UTILS_DIR)/SQLTerm.o:$(COSQUERY_UTILS_DIR)/SQLTerm.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_UTILS_DIR)/SQLTerm.o -c $(COSQUERY_UTILS_DIR)/SQLTerm.cpp

