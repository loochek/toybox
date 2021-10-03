#!/bin/sh

rm -rf build/*
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/
