#!/bin/bash

scales[0]=06
scales[1]=05
scales[2]=04
scales[3]=03


files=`cat $2`
file_to_use="temp"

index=0
for f in $files
do
    if [ $index -eq $1 ]; 
    then
        file_to_use=$f
        break
    fi
    let index=$index+1
done

if [ "$file_to_use" == "temp" ];
then
    echo "File is out of bounds"
    exit 0
fi

for item in ${scales[*]}
do
    echo "Working on $file_to_use at scale $item"
    /users/mnarayan/vox_code/compute_frags.sh $file_to_use $item
done

#echo $file_to_use
# for f in $files
# do
#     echo $f
#     ./compute_frags.sh $f
# done