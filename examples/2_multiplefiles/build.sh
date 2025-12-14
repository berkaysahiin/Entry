#!/bin/sh

CXX=clang++
STD="-std=c++20"
DEFINES="-DENTRY_VERBOSE"
SOURCES="build.cpp"

$CXX $STD $DEFINES $SOURCES -o Build && ./Build && rm Build 
