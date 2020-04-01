#!/bin/bash

mkdir build
x86_64-w64-mingw32-gcc main.cpp -Wall -std=c++17 -static -static-libstdc++ -static-libgcc -fno-exceptions -O3 -s -o build/minecraft-spoofer.exe