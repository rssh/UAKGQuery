#
# part of Grad-Soft UAKGQuery
# (C) Grad-Soft Ltd, Kiev, Ukraine.
# http://www.gradsoft.kiev.ua
#

NTESTS_CPP_LIB_SRCS=$(NTESTS_CPP_LIB_DIR)/QueryApplication.cpp

NTESTS_CPP_LIB_OBJS=$(NTESTS_CPP_LIB_DIR)/QueryApplication.o

NTESTS_CPP_LIB=$(NTESTS_CPP_LIB_DIR)/libQueryApplication.so


$(NTESTS_CPP_LIB): $(NTESTS_CPP_LIB_OBJS) 
	$(LD) $(LD_CREATE_FLAG) $(NTESTS_CPP_LIB) $(NTESTS_CPP_LIB_OBJS)


$(NTESTS_CPP_LIB_DIR)/QueryApplication.o: $(NTESTS_CPP_LIB_DIR)/QueryApplication.cpp
	$(NTESTS_CXXLIBCOMPILE) -c -o  $(NTESTS_CPP_LIB_DIR)/QueryApplication.o $(NTESTS_CPP_LIB_DIR)/QueryApplication.cpp


