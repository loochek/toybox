#!/bin/sh

rm -rf build/*
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/

cp -r resources/ build/
cp *.ttf build/resources/
