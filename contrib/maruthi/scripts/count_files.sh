#!/bin/bash

files=`cat /users/mnarayan/data/mnarayan/voc_objectlist.txt`

#files=`cat ethz_folders.txt`

for f in $files
do
    count=`ls -1 $f | wc -l`
    if [ $count -ne "33" ]; then
        echo $f
    fi
    
done