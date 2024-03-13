@echo off

if not exist out\build mkdir out\build

cd out\build
cmake ../../
cd ../../