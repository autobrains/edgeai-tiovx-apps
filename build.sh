#!/bin/bash


export SOC=am62a
mkdir build
cd build
cmake ..
make -j2 install
