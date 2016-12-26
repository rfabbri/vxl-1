#!/bin/bash


#synthetic=synthetic_med
#synthetic=synthetic_3

. sexp-real-data-definitions.sh
#data_name="ge1630"
#cam_type="projcamera"

#images_varname=images_$data_name
#images=${!images_varname}
#edgels_varname=edgels_$data_name
#edgels=${!edgels_varname}

#data_name="fragment"
data_name="ge1630"


err_pos=0.5
run_stamp="test";
#run_stamp="test";
out_path="./out/"
# count ntuplets experiment:
exp_type="band_stat"
#exp_type="epi_gain"
run_avg=true # not usd?
# maximum nviews for band_stat:
max_nviews=2
# run in nice mode??
nice="nice -n 2" 

sexp_cmd="./sexp"

#//  char *a = "0,30,60,90,120,150,15,75,35,140,105,135";
#//  char *a = "0,30,60,90,120,150,15,75,35,140,105,135";
#//  char *a = "0,30,60,90,120;
#//  char *a = "0,30,60,90,120";
#//  char *a = "0,30,60";
#//  char *a = "0,30,60,90,120";
#//  char *a = 

#angles="0,30,60,90,120,240,270,300"
#angles="0,30,60"
#angles=0,30,60,90

#angles="0,30,60,90,120,240,270,300"

set -x
set -o pipefail

images_varname=images_$data_name
images=${!images_varname}

edgels_varname=edgels_$data_name
edgels=${!edgels_varname}

input_path_varname=path_$data_name
input_path=${!input_path_varname}

cam_type_varname=cam_type_$data_name
cam_type=${!cam_type_varname}

. ./sexp_include.sh
