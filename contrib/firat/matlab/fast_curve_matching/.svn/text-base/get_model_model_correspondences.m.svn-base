% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/get_model_model_correspondences.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 28, 2011

function [M_match_center, Q_match_center, match_length, match_reverse, match_scores] = get_model_model_correspondences(model1, model2, N, K, L_min, L_increment, score_threshold)
    %L_min = round(N/10);
    %L_increment = round(N/20);
    [M_match_center, Q_match_center, match_length, match_reverse, match_scores] = match_model_model_single_scale(model1, model2, N, K, L_min, L_increment);
    fprintf('Orig Number of matches = %d\n', length(match_scores));
    elim_mask = match_scores < score_threshold;
    M_match_center(elim_mask) = [];
    Q_match_center(elim_mask) = [];
    match_length(elim_mask) = [];
    match_reverse(elim_mask) = [];
    match_scores(elim_mask) = [];
    fprintf('Number of matches after thresholding = %d\n', length(match_scores));
    elim_mask2 = false(1,length(match_scores));
    for i = 1:length(match_scores)
        for j = 1:length(match_scores)
            if i ~= j && ~elim_mask2(j)
                l1 = (match_length(i)-1) / 2;
                l2 = (match_length(j)-1) / 2;
                if l1 >= l2
                    a1 = M_match_center(i) - l1;
                    a2 = M_match_center(i) + l1;
                    
                    b1 = Q_match_center(i) - l1;
                    b2 = Q_match_center(i) + l1;
                    
                    p1 = M_match_center(j) - l2;
                    p2 = M_match_center(j) + l2;
                    
                    q1 = Q_match_center(j) - l2;
                    q2 = Q_match_center(j) + l2;
                    
                    if ((a1 == 1 && a2 == N) || (p1 >= a1 && p2 <= a2)) && ((b1 == 1 && b2 == N) || (q1 >= b1 && q2 <= b2)) && (p1-q1) == (a1-b1)
                        %fprintf('[%d,%d] -> [%d,%d] contains [%d,%d] -> [%d,%d]\n',a,a+2*l1,b,b+2*l1,p,p+2*l2,q,q+2*l2);
                        elim_mask2(j) = true;
                    end
                end
            end
        end
    end 
    M_match_center(elim_mask2) = [];
    Q_match_center(elim_mask2) = [];
    match_length(elim_mask2) = [];
    match_reverse(elim_mask2) = [];
    match_scores(elim_mask2) = [];  
    fprintf('Number of matches after eliminating overlaps = %d\n', length(match_scores));  
end

