#!/bin/bash

FILE_PATH=$(realpath $1)
KERNEL_PATH=$TUDAT_BUNDLE/build/tudatpy

(echo "import sys" && echo "sys.path.insert(0, '$KERNEL_PATH')" && echo "" && cat $FILE_PATH) | python
