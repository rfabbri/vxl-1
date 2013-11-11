% This is match_model_query_multi_scale.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 25, 2011

% scale, rotation, location invariant fast curve matching

% CM: model curve
% CQ: query curve
% N: number of samples on CM
% K: number of smoothing iterations
% num_scales: at i'th scale, number of samples on CQ is i*N/num_scales.
% maxNmatch: return at most maxNmatch matches

function [match_center, match_length, match_reverse, match_scores] = match_model_query_multi_scale(CM, CQ, N, K, num_scales, maxNmatch)
    if mod(N,2) == 0
        N = N+1;
    end
    CM_desc = curve2desc(CM, K, N, 1);
    CM_rev_desc = curve2desc(CM(end:-1:1,:), K, N, 1);
    match_center = cell(num_scales,1);
    match_length = cell(num_scales,1);
    match_reverse = cell(num_scales,1);
    match_scores = cell(num_scales,1);
    for i = max(round(num_scales/8),1):num_scales
        L = round(i*N/num_scales);
        [match_center{i}, match_length{i}, match_reverse{i}, match_scores{i}] = ...
                match_model_query_single_scale([], CQ, true, N, L, K, CM_desc, CM_rev_desc);
    end
    match_center = cell2mat(match_center);
    match_length = cell2mat(match_length);
    match_reverse = cell2mat(match_reverse);
    match_scores = cell2mat(match_scores);
    [sorted_match_scores, sort_index] = sort(match_scores, 'descend');
    if length(sorted_match_scores) > maxNmatch
        sort_index = sort_index(1:maxNmatch);
    end
    match_scores = match_scores(sort_index);
    match_length = match_length(sort_index);
    match_center = match_center(sort_index);
    match_reverse = match_reverse(sort_index);
end



