#!/bin/sh

[ -d build ] || mkdir build
clang++ main.cpp -Wall -std=c++17 -fno-exceptions -O3 -Wl,-s -o build/minecraft-spoofer-osx