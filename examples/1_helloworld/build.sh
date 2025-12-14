#!/bin/sh

CXX=clang++
STD="-std=c++20"
CFLAGS=""
DEFINES="-DENTRY_VERBOSE"
INCLUDES=""
LIBS=""
SOURCES="build.cpp"
$CXX $STD $CFLAGS $DEFINES $INCLUDES $SOURCES -o Entry $LIBS
./Entry
rm Entry
