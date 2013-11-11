#!/bin/bash 

write_dir=`pwd | awk -F'/' '{print $NF}'`"_images"

if [ ! -d $write_dir ]
then
    mkdir $write_dir
else
    rm -f $write_dir/*
fi

#Loop thru all dirs and create an html file
imageDirs=`find . -mindepth 1 -not \( -name "trash" -o -name "$write_dir" \) -type d`
for dirs in $imageDirs
do
    objectName=`echo $dirs | cut -d "/" -f2`
    htmlTitle=$objectName"_"$1
    htmlFile=$htmlTitle".html"

    if [ -r $htmlFile ]
    then
        rm -f $htmlFile
        touch $htmlFile
    fi

    #Append top part of html file to start of table
    echo '<TABLE BORDER="1">' >> $htmlFile
    echo "<caption align=\"top\">$htmlTitle</caption>" >> $htmlFile
    echo "<tr>" >> $htmlFile
    imageFiles=`find $objectName/ -name "*.$2" | sed s/^$objectName/"$write_dir"/g`

    index=1
    rowlength=0
    for imgrefs in $imageFiles
    do
        echo "<td> <img src=\"$imgrefs\" > </td>" >> $htmlFile 
        let "rowlength=index%5"
        let "index+=1"
        if [ $rowlength -eq 0 ]
        then
            echo "</tr>" >> $htmlFile
        fi
     
        realFile=`echo $imgrefs | cut -d "/" -f2`
        filePath=`find . -path $write_dir -prune -o -name $realFile`
        cp $filePath $write_dir

    done
    echo "</table>" >> $htmlFile
 

 
done





























