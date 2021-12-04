#!/bin/sh

rm -rf build/*
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/

cp *.ttf build/
cp -r resources/ build/
cp -r external_plugins/* build/
cp -r test_images/* build/
