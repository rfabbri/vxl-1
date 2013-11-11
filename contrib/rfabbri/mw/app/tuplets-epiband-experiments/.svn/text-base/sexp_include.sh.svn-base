#this file is to be included by other scripts. Do not run it by itself
# One script that calls this one is sexp.sh. Another is sexp-batch.sh

# INPUT:
# angles
# cam_type
# edgel_type
# data_name
# edgels
# err_pos
# exp_type
# fname_id
# images
# input_path
# max_nviews
# out_path
# synthetic
# sexp_cmd
# stereo

mytime=`which time`

if test -z "$mytime"; then
  echo 'Time not found.'
  exit 1
fi

pid_stamp=pid$$
date_stamp="`date +%b%d`_$pid_stamp"
fname_id="-err_pos$err_pos-$run_stamp-$date_stamp"

if test -n "$synthetic"; then
  synth_opt="-$synthetic"
  fname_id="-$synthetic-angles_$angles$fname_id"
  angles_opt="-angles $angles"
fi
if test -n "$data_name"; then
  edgels_opt="-edgels '$edgels'"
  images_opt="-images '$images'"
  fname_id="-$data_name$fname_id"
  cam_type_opt="-cam_type $cam_type"

  if test -n "$edgel_type"; then
    edg_type_opt="-edgel_type $edgel_type"
  fi
  input_path_opt="-prefix $input_path"
fi

if test -n "$stereo"; then
  fname_id="$stereo$fname_id"
fi

mytime="$mytime -v -o $out_path/sexp-time$fname_id.txt"
out_run_path="$out_path/sexp-output$fname_id.txt"

mysexp="$sexp_cmd $input_path_opt $synth_opt -err_pos $err_pos $stereo -simulation_type $exp_type -fname_id $fname_id"
mysexp+=" -out_path $out_path $edgels_opt $edg_type_opt $images_opt $cam_type_opt -max_nviews $max_nviews"
mysexp+=" $angles_opt"

eval $nice $mytime $mysexp |tee -a $out_run_path

#test $? -gt 0 && exit 1
