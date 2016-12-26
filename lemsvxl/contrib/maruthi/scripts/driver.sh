#!/bin/bash

files=`cat /users/mnarayan/data/mnarayan/brown_ethz/brown_ethz_object_list.txt`

for f in $files
do
    echo $f
    ./compute_curves.sh $f

done