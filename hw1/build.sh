#!/bin/bash

module load gcc/4.7
module unload openmpi
module load openmpi/1.8.4_gcc

rm -rf glog_bld
mkdir glog_bld
cd glog_src
./configure --prefix=`pwd`/../glog_bld && make && make install
cd ..

mv glog_bld/include/* include
mv glog_bld/lib/* lib

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
mpic++ -std=c++11 -I include -L lib -lglog hw1.cpp