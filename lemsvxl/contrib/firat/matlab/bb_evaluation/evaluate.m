% This is /lemsvxl/contrib/firat/matlab/bb_evaluation/evaluate.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 25, 2012

function [tp, fp, fppi, dr] = evaluate(object_bbs, image_ids, gt_bbs)
    num_det = size(object_bbs, 1);
    num_imgs = length(gt_bbs);
    num_gt = size(cell2mat(gt_bbs),1);
    tp = zeros(num_det, 1);
        
    for i = 1:num_det
        iid = image_ids(i);
        bb = object_bbs(i,:);
        gt = gt_bbs{iid};   
        det_index = -1;
        for j = 1:size(gt,1)
            if pascal_sim(bb, gt(j,:)) >= 0.5
                det_index = j;
                tp(i) = 1;    
                break            
            end        
        end
        if det_index > 0
            gt_bbs{iid}(det_index, :) = [];
        end       
    end
    num_gt
    num_imgs = 255
    fp = 1 - tp;
    fppi = cumsum(fp)/num_imgs;
    dr = cumsum(tp)/num_gt;
end
