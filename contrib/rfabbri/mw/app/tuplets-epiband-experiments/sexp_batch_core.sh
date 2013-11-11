#!/bin/bash
#
# Stereo experiments - core batch script. Need a bunch of variables as input parameters. called by
# sexp-batch*sh scripts.
#


LEMSVXLBIN=$HOME/lemsvxl-bin

sexp_cmd=sexp-pid$$
cp -i $LEMSVXLBIN/contrib/rfabbri/mw/app/sexp $sexp_cmd


if test ! -d $out_path/old; then
  mkdir $out_path/old
fi

mv $out_path/*.txt $out_path/old

#function killmyjobs() {
##  kill `jobs|grep -o '\[[0-9]*\]'|grep -o '[0-9][0-9]*'|sed 's/\([0-9][0-9]*\)/%\1/g'`
#  killall $sexp_cmd
#  exit
#}

function my_cleanup() {
  killall $sexp_cmd
  rm $sexp_cmd
  exit
}

trap my_cleanup SIGINT
#trap 'kill `jobs -p`; exit' SIGINT
#trap killmyjobs SIGINT

if test -z "$stereo_list"; then
  stereo_list="none"
fi



echo STARTING TESTS
echo ===================================

for exp_type in $experiments; do
  echo $exp_type

  # not used?
  run_avg=true

  # Synthetic data
  for synthetic in $synthetic_list ; do
    echo \ \ $synthetic
    for err_pos in $error_pos_synthetic_list; do
    echo \ \ \ $err_pos
      for angles in $angles_synthetic_list; do
        echo \ \ \ \ $angles
        max_nviews=`echo $angles|tr , \ |wc -w`
        echo \ \ \ \ MaxV: $max_nviews

        for stereo in $stereo_list; do

          if test $stereo = "none"; then # only epipolar geometry; no stereo
            unset stereo;
          fi

          echo  \ \ \ \ \ Stereo: $stereo

          njobs=`jobs|wc -l`
          if test $njobs -gt $max_njobs; then
            echo waiting..
            wait %
          fi
  #        cat /dev/zero > /dev/null&
          . ./sexp_include.sh &
  #        . ./sexp_include.sh
        done

      done
    done
  done

echo --------------------------------------------------------
echo Finished Spawning Synthetic Data
echo ========================================================
echo
echo ========================================================
echo Starting Real Data
echo --------------------------------------------------------

  # not used?
  run_avg=false


  # Real data
  for data_name in $real_data; do
    echo \ \ $data_name
    err_pos_list_name=error_pos_list_$data_name
    err_pos_list=${!err_pos_list_name}
    for err_pos in $err_pos_list; do
      echo \ \ \ $err_pos
      
      for stereo in $stereo_list; do

        if test $stereo = "none"; then # only epipolar geometry; no stereo
          unset stereo;
        fi

        echo  \ \ \ \ Stereo: $stereo

        images_varname=images_$data_name
        images=${!images_varname}

        edgels_varname=edgels_$data_name
        edgels=${!edgels_varname}

        input_path_varname=path_$data_name
        input_path=${!input_path_varname}

        cam_type_varname=cam_type_$data_name
        cam_type=${!cam_type_varname}

        edgel_type_varname=edgel_type_$data_name
        edgel_type=${!edgel_type_varname}

        max_nviews=`echo $images|wc -w`

        echo \ \ \ \ Imgs:$images
        echo \ \ \ \ Edgs:$edgels
        echo \ \ \ \ CamT:$cam_type
        echo \ \ \ \ EdgT:$edgel_type
        echo \ \ \ \ Ipth:$input_path
        echo \ \ \ \ MaxV:$max_nviews

          njobs=`jobs|wc -l`
          if test $njobs -gt $max_njobs; then
            echo waiting..
            wait %
          fi
  #        cat /dev/zero > /dev/null&
          . ./sexp_include.sh &
  #        . ./sexp_include.sh
      done

    done
  done
done

echo "All necessary processes have been spawned. Waiting."
wait
my_cleanup
for i in core.$sexp_cmd*; do
   test -e $i && echo && echo "ERROR: CORE DUMP WAS FOUND: $i"
done


echo Done
echo ===================================


echo FINISHED TESTS
