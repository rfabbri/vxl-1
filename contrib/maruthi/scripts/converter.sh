#!/bin/bash

images=`ls -1`

for files in $images
do
    #strip off and replace
    resultFile=`echo $files | sed 's/.ps/.png/'`

    convert $files $resultFile    
    echo "finished with $files"
    
done
