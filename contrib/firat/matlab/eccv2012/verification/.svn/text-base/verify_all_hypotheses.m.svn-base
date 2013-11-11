% This is verify_all_hypotheses.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 20, 2012

%max_iter = 20 (originally)
function [object_bbs, object_contours, detection_scores] = verify_all_hypotheses(edgemap_bw_file, hypotheses, model_ids, model_contours, num_samples, rel_size_thresh, nms_thresh, max_iter)
    N = length(model_ids);
    object_bbs = zeros(N,4);
    object_contours = cell(N,1);
    detection_scores = zeros(N,1);
    edg = imread(edgemap_bw_file);
    for i = 1:N
        [hypo_x, hypo_y] = get_hypothesis_contour(model_contours{model_ids(i)}, hypotheses(i,:));
        [object_bbs(i,:), object_contours{i}, detection_scores(i)] = verify_hypothesis_by_levelset(edg, hypo_x, hypo_y, model_contours, num_samples, rel_size_thresh, max_iter);
    end
    [detection_scores, sort_index] = sort(detection_scores, 'descend');
    object_bbs = object_bbs(sort_index,:);
    object_contours = object_contours(sort_index);
    no_det_index = find(detection_scores <= 0);
    if ~isempty(no_det_index)
        object_bbs(no_det_index(1):end,:) = [];
        object_contours(no_det_index(1):end) = [];
        detection_scores(no_det_index(1):end) = []; 
    end
    
    dets2keep = nms_detections(object_bbs, nms_thresh);
    object_bbs = object_bbs(dets2keep,:);
    object_contours = object_contours(dets2keep);
    detection_scores = detection_scores(dets2keep);   
end
