#!/bin/bash

mkdir build
gcc main.cpp -Wall -std=c++17 -static -static-libstdc++ -static-libgcc -fno-exceptions -O3 -s -o build/minecraft-spoofer-linux
