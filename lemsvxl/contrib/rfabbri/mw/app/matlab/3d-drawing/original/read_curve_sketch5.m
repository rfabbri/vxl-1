clear all;
close all;

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
%mypath='./amsterdam-house-quarter-size-mcs-work/minlength-10_mininliersview-8_minviews-8_12-conf-new-stereo/crvs/';
%mypath='./amsterdam-house-quarter-size-original-order-small-subset-mcs-work/minlength-10_mininliersview-8_minviews-3_6-conf-new-stereo/anch-10-15/crvs/';
%mypath='./amsterdam-house-quarter-size-small-subset-mcs-work/minlength-10_minviews-2_mininliersview-8_vertical-motion/crvs/';
%mypath='./amsterdam-house-new-linker-curve-completion-tests-mcs-work/paper-parameters-same-as-capitol/curve_overlap3/crvs/';

views = 0:24;
recs = cell(0,0);

for v=1:size(views,2)
    v
    mypath=['./paper-runs/pavilion-midday-half/',num2str(views(1,v)),'/6'];
    first_recs = cell(0,0);
    
    [ret, myfiles] = unix(['ls ' mypath '/crvs/*-3dcurve-*-points*dat | xargs echo']);

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
        first_recs{1,end+1} = r;
      end
    end


    disp(['first_ncurves = ' num2str(max(size(first_recs)))]);

%     ncurves = size(first_recs,2);
%     broken_recs = cell(1,5*ncurves);
%     brokenIND = 1;
% 
%     for c=1:ncurves
% 
%         numsamples = size(first_recs{c},1);
%         startSample = 1;
% 
%         for s=2:numsamples
%             distVec = [first_recs{c}(s,1)-first_recs{c}(s-1,1) first_recs{c}(s,2)-first_recs{c}(s-1,2) first_recs{c}(s,3)-first_recs{c}(s-1,3)];
%             dist = norm(distVec);
%             if(dist > 0.05)
%                 if(s>startSample+1)
%                     broken_recs{1,brokenIND} = first_recs{c}(startSample:(s-1),:);
%                     brokenIND = brokenIND+1;
%                 end
%                 startSample = s;
%             end
%         end
%         broken_recs{1,brokenIND} = first_recs{c}(startSample:numsamples,:);
%         brokenIND = brokenIND+1;
% 
%     end
%     
%     temp_recs = cell(1,brokenIND-1);
%     numtotal=0;
% 
%     for bc=1:(brokenIND-1)
% 
%         if(size(broken_recs{1,bc},1)>=10)
%             temp_recs{1,bc} = broken_recs{1,bc};
%             numtotal=numtotal+1;
%         end
% 
%     end
% 
%     for pc=1:(brokenIND-1)
% 
%         if(~isempty(temp_recs{1,pc}))
%             recs{1,end+1} = temp_recs{1,pc};
%         end
% 
%     end

    recs = [recs first_recs];

    disp(['final_ncurves = ' num2str(max(size(recs)))]);

    % %Read the certainty flags
    % fid = fopen([mypath, '/flags.txt'],'r');
    % numCurves = fscanf(fid,'%d',[1 1]);
    % flags = cell(numCurves,1);
    % for i=1:numCurves
    %     numSamples = fscanf(fid,'%d',[1 1]);
    %     flags{i,1} = fscanf(fid,'%d',[1 numSamples]);
    % end

end


