#!/bin/bash



input_object=`echo $1 | cut -d "/" -f7`

echo $input_object
filename=$input_object".xml"

temp1=$input_object"one_var.txt";
temp2=$input_object"_temp.xml";

echo $1 | sed 's/\//\\\//g' > $temp1
var=`cat $temp1`

sed  's/input_object_dir=\"\"/input_object_dir="'$var$'"/' base/baseline_inputs.xml > $temp2
sed  's/input_object_name=\"\"/input_object_name="'$input_object'"/' $temp2 > $filename 

/home/mn/lemsvxl/bin/brcv/rec/dborl/algo/vox_compute_contours/dborl_compute_contours -x $filename

rm -f $temp1
rm -f $temp2
rm -f $filename