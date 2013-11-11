#!/bin/bash

DATASETPATH=$1
OUTPUTPATH=$2
IMGEXT=$3

LISTIMAGES=$(ls $DATASETPATH/*.$IMGEXT)

for im in $LISTIMAGES
do
	im=$(basename $im)
	echo "Processing" $im
	OBJECTNAME=${im%\.*}
	INFILE=$DATASETPATH/$im
	OUTFOLDER=$OUTPUTPATH/$OBJECTNAME
	OUTFILE=$OUTFOLDER/$OBJECTNAME
	mkdir -p $OUTFOLDER
	echo Input: $INFILE
	echo Output: $OUTFILE
	echo Params: $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} Input Output
	./engn2560_egb $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} $INFILE $OUTFILE
done

# Example Usage:
# ~/lemsvxl/src/contrib/firat/courses/engn2560/bottom-up/egb/egb4dataset.sh /home/firat/Desktop/weizmann_horse_db/gray /home/firat/Desktop/weizmann_horse_db/egb_sigma1_k100_min0_1800_200 jpg 1 1 1 100 100 1 0 1800 200