#!/bin/bash

# directories to be formatted (recursive search)
DIRS="GNode/include GNode/src examples tests"
FORMAT_CMD="clang-format -style=file:scripts/clang_style -i {}"

for D in ${DIRS}; do
    find ${D}/. -type f \( -iname \*.hpp -o -iname \*.cpp \) -exec ${FORMAT_CMD} \;
done
