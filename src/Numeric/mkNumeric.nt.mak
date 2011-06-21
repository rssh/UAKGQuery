# $Id: mkNumeric.nt.mak,v 1.2 2002-08-29 11:51:30 srkr Exp $

# Generated! 
#


CosQuery_Numeric_SRC= \
    $(CosQuery_Numeric_DIR)/NumericImpl.cpp \
    $(CosQuery_Numeric_DIR)/NumericParser.cpp
#
CosQuery_Numeric_OBJS= \
    $(CosQuery_Numeric_DIR)/NumericImpl.obj \
    $(CosQuery_Numeric_DIR)/NumericParser.obj

CosQuery_Numeric_CLSRC=$(CosQuery_Numeric_SRC)
CosQuery_Numeric_CLOBJS=$(CosQuery_Numeric_OBJS)
#
$(CosQuery_Numeric_DIR)/NumericImpl.obj: NumericImpl.cpp

$(CosQuery_Numeric_DIR)/NumericParser.obj: NumericParser.cpp