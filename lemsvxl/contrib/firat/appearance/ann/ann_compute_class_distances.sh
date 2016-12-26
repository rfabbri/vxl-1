#!/bin/bash


#get query files
query_files=`cat $1`
tfiles=`ls -1 $2/*.pts`
desc_type=$3
error_eps=$4
outdir=$5
for qfiles in $query_files
do
    results_file=$outdir/`basename $qfiles | sed s/.pts// | cut -f3 -d"/"`_results.txt
    echo "Working on $qfiles"  
    for trains in $tfiles
    do
        nlines=`wc -l $trains | cut -f1 -d' '`
        filename=`basename $trains`
        desc_size=`echo $filename | cut -f3 -d"_"`
        cat=`echo $filename | sed s/_${desc_type}_${desc_size}_train.pts//`
        
        ~/lemsvxl/src/contrib/firat/appearance/ann/ann_compute_class_distance -d $desc_size -max $nlines -nn 1 -e $error_eps -df $trains -qf $qfiles -cat $cat >> $results_file
    done
    echo "Writing results to $results_file"
    echo ""
done
