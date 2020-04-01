#!/bin/sh

mkdir build
clang main.c -Wl,-s -O3 -o build/minecraft-spoofer-osx