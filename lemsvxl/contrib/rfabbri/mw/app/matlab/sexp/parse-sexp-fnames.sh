#!/bin/bash

#set -x

allfiles=`ls *avg*`

#all_echo allfiles|cut -d '-' -f 2|uniq


all_possible_datasets="ge1630 fragment synthetic_med synthetic_3 synthetic_2"

for data_set_name in $all_possible_datasets; do
  strresult=`echo $allfiles | grep $data_set_name`
  if test -n "$strresult"; then
    for f in $strresult; do
      echo DataNm: $data_set_name
      echo FileNm: $f
      err_pos=`echo $f| grep -o 'err_pos[^-]*-'|sed 's/err_pos//g'|tr - \ `
      echo ErrPos: $err_pos

      synthresult=`echo $data_set_name|grep synth`
      if test -n "$synthresult"; then
        # get angles
        angles=`echo $f | grep -o 'angles_[^-]*-'|sed 's/angles_//g'|tr - \ ` 
        echo Angles: $angles
      fi

    done
  fi
done

# - from list of known datasets, try to find them among filenames. If there is
# any, include them.
#   -for each dataset
#       - for each filename of this dataset
#            - get err_pos value
#            - get angles value
#            - get pid_stamp
