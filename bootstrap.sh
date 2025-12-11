#!/bin/sh

CXX=clang++
STD="-std=c++20"
CFLAGS="-Wall -Wextra -Werror -O0 -g"
DEFINES="-DENTRY_VERBOSE"
INCLUDES="
  -Iinclude
  -Ivendor
  -Ivendor/json/single_include
  -I/usr/include/lua5.4
"
LIBS="-llua5.4"

SOURCES=$(find src -type f -name "*.cpp")

echo "Compiling..."
$CXX $STD $CFLAGS $DEFINES $INCLUDES $SOURCES -o entry $LIBS

echo "Build complete: ./entry"

