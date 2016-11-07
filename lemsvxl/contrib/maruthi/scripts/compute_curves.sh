#!/bin/bash



input_object=`echo $1 | cut -d "/" -f7`

echo $input_object
filename=$input_object".xml"

temp1=$input_object"one_var.txt";
temp2=$input_object"_temp.xml";

#attach image pryamid to variable
dataset=$1
echo $dataset | sed 's/\//\\\//g' > $temp1
var=`cat $temp1`

sed 's/input_object_dir=\"\"/input_object_dir="'$var$'"/' base/input_defaults_contours.xml > $temp2

files=`ls -1 $dataset/*.png`

for f in $files
do

    temp=`echo $f | cut -d "/" -f8`
    file=`echo $temp | cut -d "." -f1`

    sed  's/input_object_name=\"\"/input_object_name="'$file'"/' $temp2 > $filename 
    ./dborl_compute_contours -x $filename

done

rm -f $temp1
rm -f $temp2
rm -f $filename
rm -f params.xml