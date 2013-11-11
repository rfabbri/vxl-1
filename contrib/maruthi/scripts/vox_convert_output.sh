#!/bin/bash

# This script convers vox downloaded zip files into appropriate file
# folder format
exp_dir="vox_exp_"`echo $1 | grep -o [0-9].* | cut -d "." -f1`

echo $exp_dir
#Lets delete the directory if already there
if [ -d $exp_dir ];
then
    #Directory exists lets get rid of it
    rm -fr $exp_dir

fi

#unzip the results to a directory called vox_results
unzip -o $1 -d $exp_dir

#pushd into directory and lets start script
#we will popd when when we are done
cd $exp_dir

#Lets find out all object ids
objectIds=`ls *.xml | cut -d "_" -f2`

#Create trash directory
#doesnt know where they belong
trash="trash"

#Create a map, key ids, value object_name
for ids in $objectIds
do

    
    #Find all object names
    objectName=`find . -not \( -name "*.xml" -o -name "*patch*" \) -name "*"$ids"_[a-zA-Z]*"  | cut -d "_" -f3- | egrep -o '^[a-zA-Z]*_[a-zA-Z]*|^[a-zA-Z]*|^[a-zA-Z]*_[a-zA-Z0-9]*|^[a-zA-Z]*_[a-zA-Z]*_[a-zA-Z0-9]*|^[a-zA-Z]*[0-9]_[a-zA-Z]*' | uniq`

    #Create directory with object name
    if [ $objectName ];
    then
        if [ ! -d $objectName ];
        then
            mkdir $objectName
            echo ""
            echo "Found object $objectName with id $ids"
            echo "Making directory $objectName"
            echo "Moving all files with that object name to $objectName" 
        fi
    else
        echo ""
        echo "Found unknown object type of $ids"
        echo "Making trash directory and putting there!"

        if [ ! -d $trash ];
        then
            mkdir $trash
        fi
    fi
    
    #Now perform an inner loop with that id
    objectFilesWithName=`find . -not \( -name "*input.xml" \) -name "*"$ids"_[a-zA-Z]*"`
    
    #Move over object files to respective directories
    for object in $objectFilesWithName
    do
        #Cut out object id
        fileObject=`echo $object | cut -d "_" -f3-`
        mv $object $fileObject
        if [ $objectName ];
        then
            mv $fileObject $objectName
        else
            mv $fileObject $trash
        fi
    done

    #Now perform another loop over object files without Names
    objectFilesWithoutName=`find . -name "*_"$ids"_*"`

    for objNoName in $objectFilesWithoutName
    do
      
        #Cut out object id
        noNameObject=`echo $objNoName | cut -d "_" -f3-`
        
        #Create concatenated name
        name=$objectName"_"$noNameObject

        if [ $objectName ];
        then
            mv $objNoName $name
            mv $name $objectName
        else
            mv $objNoName $trash
        fi
    done
    
done

cd ..



















