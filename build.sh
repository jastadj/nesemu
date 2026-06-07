#!/bin/bash

rm -rf build
mkdir build
pushd build
cmake .. -DGLFW_DIR=$GLFW_DIR || { echo "failed to generate build files." ; exit 1 ; }
cmake --build . --config RelWithDebInfo || { echo "failed to build." ; exit 2 ; }
popd