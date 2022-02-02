#!/bin/sh

rm -rf build/*
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/

cp -r resources/ build/
cp *.ttf build/resources/
rsync -avrq --exclude='README.md' external_plugins/ build/plugins
cp -r plugin_resources/* build/plugins/
cp -r test_images/* build/
