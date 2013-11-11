% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/match_model_model_single_scale.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 25, 2011

function [M_match_center, Q_match_center, match_length, match_reverse, match_scores] = match_model_model_single_scale(CM, CQ, N, K, min_L, L_increment)
    if mod(N,2) == 0
        N = N+1;
    end
    CM_desc = curve2desc(CM, K, N, 1);
    CM_rev_desc = curve2desc(CM(end:-1:1,:), K, N, 1);  
    CQ_desc_N = curve2desc(CQ, K, N, 1);  
    L_vals = min_L:L_increment:N-2;
    even_L_vals = ~mod(L_vals,2);
    L_vals(even_L_vals) = L_vals(even_L_vals) + 1;
    L_vals = unique(L_vals);
    if(L_vals(end) ~= N-2)
        L_vals(end+1) = N-2;
    end
    num_L = length(L_vals);
    M_match_center = cell(num_L,1);
    Q_match_center = cell(num_L,1);
    match_length = cell(num_L,1);
    match_reverse = cell(num_L,1);
    match_scores = cell(num_L,1);
    for L_index = 1:num_L    
        L = L_vals(L_index);
        for k = 1:N-L-1 
            CQ_desc = CQ_desc_N(k:k+L+1, k+1:k+L);       
            [M_match_center{k}, match_length{k}, match_reverse{k}, match_scores{k}] = ...
                 match_model_query_single_scale([], [], false, N, L+2, K, CM_desc, CM_rev_desc, CQ_desc);
            Q_match_center{k} = (2*k+L+1)/2 * ones(length(M_match_center{k}), 1);
        end
    end
    M_match_center = cell2mat(M_match_center);
    Q_match_center = cell2mat(Q_match_center);
    match_length = cell2mat(match_length);
    match_reverse = cell2mat(match_reverse);
    match_scores = cell2mat(match_scores);
    [match_scores, sort_index] = sort(match_scores, 'descend');
    match_length = match_length(sort_index);
    M_match_center = M_match_center(sort_index);
    Q_match_center = Q_match_center(sort_index);
    match_reverse = match_reverse(sort_index);     
end

