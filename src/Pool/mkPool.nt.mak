CosQuery_Pool_OBJS= \
 $(CosQuery_Pool_DIR)\PoolConnectionManager.obj \
 $(CosQuery_Pool_DIR)\PoolQueryManager.obj 

$(CosQuery_Pool_OBJS): $*.cpp $*.h

test:
  echo $(CosQuery_Pool_OBJS)