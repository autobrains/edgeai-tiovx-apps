#!/bin/bash

cd edgeai-tiovx-apps
export SOC=am62a
mkdir build
cd build
cmake ..
make -j2
cp ./lib/Release/libaewb_logger.so.0.1.0 /usr/lib/
