
# empty all: we don't need in QueryGrammar
#   

COSQUERY_UTILS_QUERYGRAMMAR_DIR=$(CosQuery_Utils_DIR)/QueryGrammar

COSQUERY_UTILS_QUERYGRAMMAR_SRCS=

CosQuery_Utils_QueryGrammar_OBJS=

$(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLSymbolTable.obj: $(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLSymbolTable.cpp
$(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLScanner.obj: $(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLScanner.cpp
$(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLParser.obj: $(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLParser.cpp
$(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/y.tab.obj: $(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/y.tab.c
$(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLTermUtils.obj: $(COSQUERY_UTILS_QUERYGRAMMAR_DIR)/SQLTermUtils.cpp