#!/bin/sh

#ADD ALL LOG MESSAGE TO night.log

HOSTNAME=`hostname`
case $HOSTNAME in
 goblin*) 

. /$HOME/ora-profile
rm config.cache
autoconf -l config

gmake clean
echo "-------------Configure------------" > night.log
./configure --with-ob=/usr/local/OB --with-jtc=/usr/local/OB --without-tao --with-ots --with-ob-xa >> night.log 
echo "-------------Make------------" >> night.log
gmake 2>&1 | cat >>  night.log 
echo "-----------------------------" >> night.log
gmake clean
echo ./configure 
gmake 2>&1 | tee log.make

echo "-------------Configure------------" >> night.log
./configure --with-ob=/usr/local/OB --with-jtc=/usr/local/OB --without-tao --with-ots >> night.log 
echo "-------------Make------------" >> night.log
gmake 2>&1 | cat >> night.log 
echo "-----------------------------" >> night.log
 ;;
 kulshedra*) 

autoconf -l config
gmake clean
./configure > log.configure
gmake client 2>&1 | tee log.make
mail -s "configure log on kulshedra for UAKGQuery" rssh@grad.kiev.ua < log.configure
mail -s "make log on kulshedra for UAKGQuery" rssh@grad.kiev.ua < log.make

 ;;

esac
