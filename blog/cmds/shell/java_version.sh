#!/bin/bash
VER=`java -version 2>&1 | grep "java version" | awk '{print $3}' | tr -d \" | awk '{split($0, array, ".")} END{print array[2]}'`
if [[ $VER -ge 6 ]]; then
    echo " >= 1.6"
else
    echo " <= 1.6"
fi
