@echo off

mkdir build
x86_64-w64-mingw32-gcc main.c -Wall -static -static-libgcc -O3 -s -o build/minecraft-spoofer-windows.exe