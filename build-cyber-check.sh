#!/bin/bash


export SOC=am62a
mkdir build
cd build
cmake -DCMAKE_CXX_FLAGS=" -fstack-protector -s -Wl,-z,relro,-z,now " -DCMAKE_C_FLAGS=" -fstack-protector -s -Wl,-z,relro,-z,now " ..
make -j1 VERBOSE=1
