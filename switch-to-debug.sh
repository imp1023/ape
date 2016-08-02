#!/bin/bash

rm -f 0RELEASE
touch 0DEBUG
cmake -DCMAKE_BUILD_TYPE=debug -DEXECUTABLE_OUTPUT_PATH=../bin_debug .

