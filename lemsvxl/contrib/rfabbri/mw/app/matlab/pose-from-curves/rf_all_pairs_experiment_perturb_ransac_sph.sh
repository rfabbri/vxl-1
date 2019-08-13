# usage: cmd initial_view end_view
#
# called by -batch.sh

workdir="$HOME/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work"
echo output to  $workdir-views-$1-$2-matlab_output
sed "s/@INI@/$1/g
s/@END@/$2/g" rf_all_pairs_experiment_perturb_ransac_sph_batch.m | matlab > $workdir-views-$1-$2-matlab_output

