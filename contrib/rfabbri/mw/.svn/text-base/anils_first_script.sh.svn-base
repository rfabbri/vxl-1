#!/bin/bash


files_to_diff=`find . -name "*.txt"`

for f in $files_to_diff
do
    
    b=`diff $f /home/anilusumezbas/lemsvxl-ric/src/contrib/rfabbri/mw/$f`
    if [ -n "$b" ];
    then
        echo "$f has changed"
    fi

done