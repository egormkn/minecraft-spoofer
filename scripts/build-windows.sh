#!/bin/bash

[ -d foo ] || mkdir build
x86_64-w64-mingw32-g++ main.cpp -Wall -std=c++17 -static -static-libstdc++ -static-libgcc -fno-exceptions -O3 -s -o build/minecraft-spoofer.exe