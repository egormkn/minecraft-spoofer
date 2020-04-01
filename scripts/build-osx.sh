#!/bin/sh

mkdir build
clang main.cpp -Wall -std=c++17 -static -static -static-libstdc++ -static-libgcc -fno-exceptions -O3 -Wl,-s -o build/minecraft-spoofer-osx