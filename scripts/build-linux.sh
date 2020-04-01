#!/bin/bash

mkdir build
gcc main.c -Wall -static -static-libgcc -O3 -s -o build/minecraft-spoofer-linux
