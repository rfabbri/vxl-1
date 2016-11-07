#!/bin/sh

# As of 

set -x
set -o pipefail

symmetric_3=true
compute_epipolars=true # compute epipolar constraint
run_stereo=true
compare=true
write_to_final_file=false
pid_stamp=pid$$
#pid_stamp=pid24950
date_stamp=sep25_$pid_stamp

#synthetic=false
#synthetic=synth_1
synthetic=synth_2

perturb=true
widebaseline=true

#constraint_opt=run_synth_geometry
#constraint_opt=trinocular
#constraint_opt=trinocular_tangent_diff
constraint_opt=trinocular_curvature_diff
trinocular_radius=10

if test -z "$trinocular_radius"; then
  if test $constraint_opt = run_synth_geometry; then
    out_sufix=Gama3dot_sqrt_speed_abs
#    out_sufix=K_abs_speed_abs
#    out_sufix=t_diff_abs
#    out_sufix=k_diff_abs
#    out_sufix=disparity
  else
    out_sufix=${constraint_opt}
  fi
else
  out_sufix=${constraint_opt}_noepiptan_nrad$trinocular_radius
#  out_sufix=${constraint_opt}_nrad${trinocular_radius}_180deg
fi

if test $perturb != false; then
  perturb_stamp=perturb_dot5deg
  outprefix=-$perturb_stamp
fi

if test $symmetric_3 != false; then
  outprefix=$outprefix-symmetric
  corresp_ext=corresp3
else
  corresp_ext=corresp
fi

outprefix=$outprefix-noepitan
#outprefix=$outprefix
# TODO write cmdlines to .out files


#: the following is a default used only if we did not run the stereo matcher before
#corresp_in=
#          synth-ctspheres-0-10-5dtheta-stereo_result_synth_relative_Gama3dot-may22.$corresp_ext

# doesn't hurt to set some parameters even though we might not use them
if test -z "$trinocular_radius"; then
  myscmd="scmd"
else
  myscmd="scmd -trinocular_nrad $trinocular_radius"
fi

if test $synthetic = synth_1; then

  angle1=0
  angle2=10 
  angle3=60 

  myprefix=/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/ 
  outname=output-synth1-ctspheres-pid$$
  dtheta=5
  outprefix=synth-ctspheres-${dtheta}dtheta$outprefix
  synth_opt=-synthetic_1 #:< option passed to scmd

elif test $synthetic = synth_2; then
  
  angle1=0
  if test $widebaseline = false; then
    angle2=5
    angle3=60 
  else
    angle2=60
    angle3=5
  fi

  if test $perturb != false; then
    myprefix=/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/perturb/
  else
    myprefix=/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/
  fi

  outname=output-synth2-olympus-pid$$
  outprefix=synth-olympus-$angle1-${angle2}$outprefix
  synth_opt=-synthetic_2 #:< option passed to scmd
fi

if test $synthetic != false; then
  myscmd="$myscmd -angle1 $angle1 -angle2 $angle2 -angle3 $angle3"
  if test $perturb != false; then
    myscmd="$myscmd -perturb_camera"
  fi
fi

if test $symmetric_3 != false; then
  myscmd="$myscmd -symmetric_3"
fi

epipolar_fname=$outprefix-epipolar.$corresp_ext
outname=$outprefix-stereo_result_$out_sufix-$date_stamp
if test $write_to_final_file = false; then
  corresp_in=out/$outname.$corresp_ext
else
  corresp_in=$outname.$corresp_ext
fi


#scmd -prefix \
#/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/david/david-02-26-2006-crop2/small-h100/\
#-image1 000-maximage.png\
#-image2 003-maximage.png\
#-image3 006-maximage.png\
#-corresp david-0-3-edges_t10.$corresp_ext

#scmd -compare_corresp -corresp_in david-0-3-edges_t10-stereo_sel_result_may04_1.$corresp_ext

#scmd -sel_geometry -read_epip


##############################
# Compute epipolar geometry + write it
##############################
#
#scmd -compute_epipolars -write_epip -corresp_epip synth-ctspheres-0-10.$corresp_ext

if test $compute_epipolars = true; then
  echo
  echo "..::: RUNNING EPIPOLAR CONSTRAINT ($constraint_opt) :::.."
  echo
  if test $synthetic != false; then
    $myscmd $synth_opt -compute_epipolars -write_epip -corresp_epip $epipolar_fname -prefix $myprefix 
    if test $? = 1; then
      exit 1
    fi
  fi
  if $write_to_final_file ; then
    mv $myprefix/out/$epipolar_fname  $myprefix
  fi
fi


##############################
# Run for synthetic data w/ precomputed epipolars
##############################

if test $run_stereo != false; then
  echo
  echo "..::: RUNNING STEREO CONSTRAINT ($constraint_opt) :::.."
  echo
  if test $synthetic != false; then
    out_run_path=$myprefix/out/$outname.out
    echo COMMAND: $myscmd $synth_opt -$constraint_opt -corresp_out out/$outname.$corresp_ext -read_epip -corresp_epip $epipolar_fname -prefix $myprefix >> $out_run_path

    time $myscmd $synth_opt -$constraint_opt -corresp_out out/$outname.$corresp_ext -read_epip -corresp_epip $epipolar_fname -prefix $myprefix | tee -a $out_run_path

    if test $? = 1; then
      exit 1
    fi

    if $write_to_final_file ; then
      mv $myprefix/out/$outname.{$corresp_ext,out}   $myprefix
    fi
  fi
fi



##############################
# Compare
##############################

if test $compare != false; then
  echo
  echo "..::: COMPARING CORRESP :::.."
  echo
  if test $synthetic != false; then

    out_run_path=$myprefix/out/$outname-compare.out
    echo $myscmd $synth_opt -compare_corresp -prefix $myprefix -corresp_in $corresp_in >> $out_run_path

         $myscmd $synth_opt -compare_corresp -prefix $myprefix -corresp_in $corresp_in | tee -a $out_run_path

    if test $? = 1; then
      exit 1
    fi

    if $write_to_final_file ; then
      cat $myprefix/out/$outname-compare.out  >> $myprefix/$outname.out
      rm $myprefix/out/$outname-compare.out 
    fi
  fi
fi

if test $perturb != false; then
  echo
  echo "..::: WRITING PERTURBED CAMERAS :::.." 
  echo
  
  $myscmd $synth_opt -perturb_camera -write_perturb_camera -prefix $myprefix 

  if test $? = 1; then
    exit 1
  fi
   
fi


# CUSTOM CORRESPONDENCE FOR COMPARISON
#$myscmd $synth_opt -compare_corresp -prefix $myprefix -corresp_in perturb/synth-olympus-0-5-perturb_1deg-epipolar.$corresp_ext

# Generate edgels for synthetic:
#scmd -synthetic_1 -synth_write_edgels \
#  -prefix /home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/ \
#  -edgels1 000-ct_table.edgels \



#if test $synthetic != false; then
#  $myscmd  $synth_opt -synth_write_edgels \
#    -prefix /home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/ \
#    -edgels1 000deg-noepitan-digicam_table.edgels \
#    -edgels2 005deg-noepitan-digicam_table.edgels \
#    -edgels3 060deg-noepitan-digicam_table.edgels 
#fi
