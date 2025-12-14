#!/bin/sh

CXX=clang++
STD="-std=c++20"
CFLAGS="-Wall -Wextra -Werror -O0 -g"
DEFINES="-DENTRY_VERBOSE"
INCLUDES=""

SOURCES="build.cpp"

echo "Compiling..."
$CXX $STD $CFLAGS $DEFINES $INCLUDES $SOURCES -o Entry $LIBS

echo "Build complete: ./entry"

