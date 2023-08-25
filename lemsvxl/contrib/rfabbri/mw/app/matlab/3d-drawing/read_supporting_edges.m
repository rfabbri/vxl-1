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
%mypath='./amsterdam-house-new-linker-mcs-work/paper-parameters-same-as-capitol/crvs/';
mypath='./amsterdam-house-quarter-size-original-order-small-subset-mcs-work/minlength-10_mininliersview-8_minviews-4_48-conf_edgel-support/anch-10-13_epiangle-10_onlyMark_bothAnchors_epiIndexFix_oneCurve/crvs/';
%mypath='./amsterdam-house-Kovesi-mcs-work/dist10-angle30-otherwise-paper-parameters/crvs/';


[ret, myfiles] = unix(['ls ' mypath '/*-3dcurve-*-support*txt | xargs echo']);

myfiles;
numCurves = 0;

while length(myfiles) ~= 0
  [f,rem]=strtok(myfiles);
  myfiles = rem;
  if length(f) == 0
    break;
  end
  
  numCurves = numCurves+1;
  
end

supportingViews = cell(numCurves,1);
supportingEdgels = cell(numCurves,1);
curveIDX = 1;

[ret, myfiles] = unix(['ls ' mypath '/*-3dcurve-*-support*txt | xargs echo']);

while length(myfiles) ~= 0
  [f,rem]=strtok(myfiles);
  myfiles = rem;
  if length(f) == 0
    break;
  end
  f = strip_trailing_blanks(f);
  fid = fopen(f);
  
  numConfViews = fscanf(fid,'%d',[1 1]);
  curSuppViews = zeros(1,numConfViews);
  curSuppEdgels = cell(1,numConfViews);
  
  for n=1:numConfViews
      
      supportingView = fscanf(fid,'%d',[1 1]);
      curSuppViews(1,n) = supportingView;
      
      numEdgels = fscanf(fid,'%d',[1 1]);
      edgels = fscanf(fid,'%d',[1 numEdgels]);
      curSuppEdgels{1,n} = edgels;
      
  end
  
  supportingViews{curveIDX,1} = curSuppViews;
  supportingEdgels{curveIDX,1} = curSuppEdgels;
  curveIDX = curveIDX + 1;
  
end
