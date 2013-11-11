% This is /lemsvxl/contrib/firat/matlab/eccv2012/ethz-resized/run_ethz_bottles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 24, 2012

function run_ethz_bottles(indices)
    addpath /users/fkalayci/codes/eccv2012
    addpath /users/fkalayci/codes/eccv2012/hypothesis
    addpath /users/fkalayci/codes/eccv2012/verification
    addpath /users/fkalayci/codes/contour_related
    addpath /users/fkalayci/codes/levelset2d_segmentation
    addpath /users/fkalayci/codes/levelset2d_segmentation/tools
    addpath /users/fkalayci/codes/sussman_reinit
    addpath /users/fkalayci/codes/eccv2012/ethz-resized
    
    %data_prefix = '/users/fkalayci/data/fkalayci';
    data_prefix = '/home/firat/Desktop';
    img_folder = [data_prefix '/all_originals_resized'];
    out_folder = [data_prefix '/bottles/exp1'];
    if ~exist(out_folder, 'dir')
        mkdir(out_folder);
    end
    files = dir([img_folder '/bottles*']);
    files = files(indices);
    
    for i = 1:length(files)
        P = get_necessary_variables(data_prefix, files(i).name(1:end-4))
        [hypotheses, match_scores, model_ids, model_contours] = generate_all_detection_hypotheses(P.img_contours_file, P.model_folder, 81, 9, 6, 20, 1000);
        [object_bbs, object_contours, detection_scores] = verify_all_hypotheses(P.edgemap_bw_file, hypotheses, model_ids, model_contours, 101, 0.05, 0.6, 20);
        save([out_folder '/' files(i).name '.mat'], 'object_bbs', 'object_contours', 'detection_scores');
    end
end
