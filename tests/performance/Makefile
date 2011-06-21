# Generated automatically from Makefile.in by configure.
# General make definitions and rules.
#
# part of grad Cos* build system
#
# (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998
# $Id: Makefile,v 1.4 2000-10-02 14:05:32 jos Exp $
#
project_root=/home/jos/work/COS/CosQuery.1/tests/performance


CXX=c++
CPPFLAGS=-I$(project_root)/../steps/include -I$(project_root)/../../include -I$(project_root)/../../generated -I/home/jos/work/COS/CosQuery.1/tests/performance/generated   -I/usr/local/include -I/usr/local/include/OB
CXXFLAGS=$(CPPFLAGS)  -DIDL_CLN_H=.h -DIDL_CLN_CPP=.cpp -DIDL_SRV_H=_skel.h -DCORBA_SKELETON_SUFFIX=_skel -DCORBA_H=\<OB/CORBA.h\> -DCOSNAMING_H=\<OB/CosNaming.h\> -DCORBA_MODULE_C_MAPPING=1 -DORBACUS=1  -g -O2
INCLUDE=@INCLUDE@
CXXCOMPILE=c++ $(CXXFLAGS)

LD=ld
LDFLAG=
LIBS=-L/usr/local/lib -lOB -lnsl 

AR=ar
ARFLAGS=cru

CXXLIBPREPARE=$(CXX) $(CXXFLAGS)


default: build

PERF1=perf1
PERF1_OBJS=perf1.o ../steps/3/printLib.o
PERF2=perf2
PERF2_OBJS=perf2.o ../steps/3/printLib.o
PERF3=perf3
PERF3_OBJS=perf3.o ../steps/3/printLib.o
PERF4=perf4
PERF4_OBJS=perf4.o ../steps/3/printLib.o

PERFS=$(PERF1) $(PERF2) $(PERF3) $(PERF4)

build: $(PERFS)

clean: 
	-rm -f $(PERFS)
	-rm -f *.o
	-rm -rf Templates.DB


perf1: perf1.o
	$(LD) $(LDFLAGS) -o perf1 $(PERF1_OBJS) -L../../src -lCosQueryClient.1 $(LIBS)

perf1.o: perf1.cpp
	$(CXXCOMPILE) -c -o perf1.o perf1.cpp

perf2: perf2.o
	$(LD) $(LDFLAGS) -o perf2 $(PERF2_OBJS) -L../../src -lCosQueryClient.1 $(LIBS)

perf2.o: perf2.cpp
	$(CXXCOMPILE) -c -o perf2.o perf2.cpp

perf3: perf3.o
	$(LD) $(LDFLAGS) -o perf3 $(PERF3_OBJS) -L../../src -lCosQuery.1 -lclntsh -loi $(LIBS)

perf3.o: perf3.cpp
	$(CXXCOMPILE) -c -o perf3.o perf3.cpp

perf4.o: perf4.cpp
	$(CXXCOMPILE) -c -o perf4.o perf4.cpp

perf4: perf4.o
	$(LD) $(LDFLAGS) -o perf4 $(PERF4_OBJS) -L../../src -lCosQuery.1 -lclntsh -loi $(LIBS) 

