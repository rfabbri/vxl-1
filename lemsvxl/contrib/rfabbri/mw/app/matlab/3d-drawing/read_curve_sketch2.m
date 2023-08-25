% Reads the files for two-view recs, *dat, such as in app/dat/reconstr_curves_workdir/
%
% The input files will be named like $prefix-3dcurve-$crv_id-{points|tangents}$ext
%
% See mw_util.h for mywritev(prefix, ext, curves3d)

%mypath='./capitol-mcs-work-default/crvs/';
%mypath='./capitol-mcs-work-second-run/paper-parameters/crvs/';
%mypath='./capitol-mcs-work-second-run/results-paper/capitol_building_optimal_26_instances_mininliers_5/mcs-rec/crvs/';
%mypath='./capitol-mcs-work-second-run/capitol_building_optimal_4_instances_mininliers_250/crvs/';
%mypath='./capitol-mcs-work-inlier-dist-5/crvs/';
%mypath='./capitol-mcs-work-length-10-inlier-dist-2/crvs/';
%mypath='./capitol-mcs-work-epip-overlap-2/crvs/';

%mypath = './capitol-high-mcs-work/capitol_building_high_near_optimal_for_other_30_instance_mininliers_5/crvs/';
%mypath = './capitol-high-mcs-work/capitol_building_high_near_optimal_for_other_30_instance_mininliers_5_minepiangle_10/crvs/';
%mypath = './capitol-high-mcs-work/paper-parameters/crvs/';
%mypath='./amsterdam-house-quarter-size-hand-ordered-mcs-work/minlength-10_mininliersview-8_minviews-8_12-conf-new-stereo/crvs/';
%mypath='./amsterdam-house-quarter-size-small-subset-mcs-work/minlength-10_minviews-2_mininliersview-8_vertical-motion/crvs/';
%mypath='./amsterdam-house-new-linker-curve-completion-tests-mcs-work/paper-parameters-same-as-capitol/curve_overlap3/crvs/';
mypath=[path_to_out, 'crvs/'];
recs = cell(0,0);


[ret, myfiles] = unix(['ls ' mypath '/*-3dcurve-*-points*dat | xargs echo']);

myfiles;

while length(myfiles) ~= 0
  [f,rem]=strtok(myfiles);
  myfiles = rem;
  if length(f) == 0
    break;
  end
  f = strip_trailing_blanks(f);
  
  r = myreadv(f);
  if isempty(r)
    warning(['file is empty: ' f]);
  else
    recs{1,end+1} = r;
  end
end


disp(['ncurves = ' num2str(max(size(recs)))])
