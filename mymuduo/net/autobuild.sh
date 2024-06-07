#!/bin/bash

set -e

#如果没有build目录,就先创建build目录

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

make

#把头文件拷贝到/user/include/mymuduo so库拷贝到/usr/lib
if [ ! -d /usr/include/mymuduo ]; then
    mkdir /usr/include/mymuduo
fi

for header in `ls ./Tcpserver/*.h`
do 
    cp $header /usr/include/mymuduo 
done


cp `pwd`/build/libmymuduo.so /usr/lib

ldconfig