# $Id: mkRecordSet.nt.mak,v 1.1 2002-08-08 08:34:46 srkr Exp $

# Generated! 
#


CosQuery_RecordSet_SRC= \
	$(CosQuery_RecordSet_DIR)/RecordSetImpl.cpp
#
CosQuery_RecordSet_OBJS= \
	$(CosQuery_RecordSet_DIR)/RecordSetImpl.obj

CosQuery_RecordSet_CLSRC=$(CosQuery_RecordSet_SRC)
CosQuery_Recordset_CLOBJS=$(CosQuery_RecordSet_OBJS)
#
$(CosQuery_RecordSet_DIR)/RecordSetImpl.obj: RecordSetImpl.cpp
