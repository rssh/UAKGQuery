#!/bin/sh
#
# Usage:
# 
# sh run.sh -ORBInitRef UAKGQueryService=corbaloc::192.168.56.101:3903/UAKGQueryService --db-user test --db-password test --db-link xe
#
# where test db is created and init by scripts in ntests/sql
#
cd cpp
sh ./run.sh $* 
cd ..
