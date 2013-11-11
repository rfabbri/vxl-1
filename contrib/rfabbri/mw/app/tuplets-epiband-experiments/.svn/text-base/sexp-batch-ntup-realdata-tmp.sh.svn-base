#!/bin/bash
#
# Stereo experiments - toplevel batch script
#
#--------------------------------------------
# Set the parameters for batch processing:
#--------------------------------------------

set -x
set -o pipefail

max_njobs=0
#experiments="epi_gain band_stat"
experiments="band_stat"
#experiments="epi_gain"
#run_stamp="test";
run_stamp="valid";
#out_path="./out/real-run-synthetic-xxx2/"
out_path="./out/real-run-ge1630-xxx/ntups/"
#out_path="./out/real-run-fragment/gain/"
#out_path="./out/tuplet-test-realdata-tmp"

# ---- Data-specific parameters ----
# Synthetic:
#synthetic_list="synthetic_med synthetic_3"
#XXX synthetic_list="synthetic_med"

#error_pos_synthetic_list="0.01 0.1 0.5"
#angles_synthetic_list="0,30,60,90,120,240,270,300 0,5,10,15,25,30,35"
#XXX angles_synthetic_list="0,30,60,90,120,240,270,300"
#XXX error_pos_synthetic_list="1"
#angles_synthetic_list="0,30,60,90 0,5,10,15"
#error_pos_synthetic_list="0.01"
#angles_synthetic_list="0,30,60"

# Real data:
#real_data="fragment ge1630"
real_data="ge1630"

. sexp-real-data-definitions.sh

#error_pos_list_ge1630="0.5 1 2"
error_pos_list_ge1630="0.5"
#error_pos_list_fragment="0.5"
#error_pos_list_fragment_sm="0.5"


# run in nice mode??
#nice="nice -n 2" 

. sexp_batch_core.sh
