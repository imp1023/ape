#!/bin/bash
rm -f 0DEBUG
touch 0RELEASE
cmake -DCMAKE_BUILD_TYPE=release -DEXECUTABLE_OUTPUT_PATH=../bin_release .

