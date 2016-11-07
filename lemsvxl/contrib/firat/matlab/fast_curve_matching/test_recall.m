% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test_recall.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 18, 2011

addpath ../part_whole
clear
close all
obj_contour = load_contours('/home/firat/Desktop/db/100bottles/bottle1.txt');
files = dir('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/bottles_*');
groundtruth_bb_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_groundtruths_patched2';
max_num_samples = 80;
num_rows = 5;
num_cols = 6;
num_detection = num_rows * num_cols;
num_tp = 0;
tot_gt = 0;
problematic = {};
for iii = 1:length(files)
    image_name = get_objectname_from_filename(files(iii).name);    
    query_contours = load_contours(['/home/firat/Desktop/db/bottle_contours/' image_name '.txt']);
    det_bbs = [];
    for it = 1:length(query_contours)
        query_curve = query_contours{it}; %6    
        test_curve = obj_contour{1};
        F = query_curve;        
        tic;
        [cf_center, cf_len, cf_rev] = srlifcm(test_curve, F, max_num_samples, 3, 25, num_detection);
        toc    
        for i = 1:length(cf_center)            
            j = cf_center(i)/max_num_samples*size(test_curve,1);
            l = cf_len(i)/max_num_samples*size(test_curve,1);
            MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;
            
            S2 = sample_curve(F, 3, length(MF));
            S3 = test_curve(MF,:);   
            
            if cf_rev(i)
                T = cp2tform(S3(end:-1:1,:),S2,'nonreflective similarity');
            else
                T = cp2tform(S3,S2,'nonreflective similarity');
            end
            [tCurvex, tCurvey] = tformfwd(T, test_curve(:,1), test_curve(:,2));
            
            det_bb = [min(tCurvex) min(tCurvey) max(tCurvex) max(tCurvey)];
            det_bbs = [det_bbs; det_bb];            
        end         
    end
    gt_bbs = load([groundtruth_bb_folder '/' image_name '.groundtruth']);
    num_gt = size(gt_bbs,1);
    num_det = size(det_bbs,1);
    already_matched = zeros(num_gt,1);
    tot_gt = tot_gt + num_gt;
    for i = 1:num_det
        if all(already_matched)
            break
        end        
        det_bb = det_bbs(i,:);
        for j = 1:num_gt            
            if already_matched(j) == 0
                gt_bb = gt_bbs(j,:);
                pd = pascal_dist(gt_bb, det_bb);
                if  pd >= 0.5
                    num_tp = num_tp + 1; 
                    already_matched(j) = 1;
                    break                  
                end  
            end          
        end         
    end
    if ~all(already_matched)
       problematic = [problematic, image_name]; 
    end
end

fprintf('Num gt: %d Num correct det: %d\n', tot_gt, num_tp);
