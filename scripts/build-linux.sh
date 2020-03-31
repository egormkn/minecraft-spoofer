#!/bin/bash

mkdir build
gcc main.c -Wall -static -static-libgcc -O3 -o build/minecraft-spoofer
