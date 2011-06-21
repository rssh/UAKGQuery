#!/bin/sh
# part of Grad-Soft UAKGQuery
#
#
#TESTS="t1 t1007 t1013 t1015 t1016 t1037 t1539 t1560 t1667 t1685 t1808"
TESTS="t1037"
#
#
LD_LIBRARY_PATH=`pwd`/../lib:`pwd`/../../src:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
#
#
#TODO:
# b) run all tests, or some selected
# (add option handling)
for i in $TESTS 
do
 if ./$i $*
 then
   echo $i Ok
 else
   echo $i Failure
 fi
done
