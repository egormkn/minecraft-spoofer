@echo off

mkdir build
mingw64-gcc main.c -Wall -static -static-libgcc -O3 -o build/minecraft-spoofer.exe