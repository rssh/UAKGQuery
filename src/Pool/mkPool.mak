

COSQUERY_POOL_SRCS=\
 $(COSQUERY_POOL_DIR)/PoolConnectionManager.cpp \
 $(COSQUERY_POOL_DIR)/PoolQueryManager.cpp

COSQUERY_POOL_OBJS=\
 $(COSQUERY_POOL_DIR)/PoolConnectionManager.o \
 $(COSQUERY_POOL_DIR)/PoolQueryManager.o


$(COSQUERY_POOL_DIR)/PoolQueryManager.o: $(COSQUERY_POOL_DIR)/PoolQueryManager.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_POOL_DIR)/PoolQueryManager.o -c $(COSQUERY_POOL_DIR)/PoolQueryManager.cpp

$(COSQUERY_POOL_DIR)/PoolConnectionManager.o: $(COSQUERY_POOL_DIR)/PoolConnectionManager.cpp
	$(CXXLIBCOMPILE) -o $(COSQUERY_POOL_DIR)/PoolConnectionManager.o -c $(COSQUERY_POOL_DIR)/PoolConnectionManager.cpp
