% This is /lemsvxl/contrib/firat/matlab/curve_descriptors_and_matching/get_best_matches_multi_scale.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function [m, L, rev, sorted_scores] = get_best_matches_multi_scale(P, q, K, L_vals, method, q_indices)
    num_L = length(L_vals);    
    m = cell(1, num_L);
    L = cell(1, num_L);
    rev = cell(1, num_L);
    for L_index = 1:num_L
        l = L_vals(L_index);
        qL = sample_curve(q, l, 0);        
        [m1, s1] = get_best_matches_single_scale(P, qL, K, method, q_indices);
        [m2, s2] = get_best_matches_single_scale(P, qL(end:-1:1,:), K, method, q_indices);
        m{L_index} = [m1, m2];
        sorted_scores{L_index} = [s1,s2];
        rev{L_index} = [zeros(1,length(s1)), ones(1,length(s2))];
        L{L_index} = ones(1, length(m{L_index})) * l;
    end
    L = cell2mat(L);
    m = cell2mat(m);
    rev = cell2mat(rev);
    sorted_scores = cell2mat(sorted_scores);
    [sorted_scores, sort_index] = sort(sorted_scores, 'descend');
    sorted_scores = sorted_scores(1:K);
    sort_index = sort_index(1:K);
    m = m(sort_index);
    L = L(sort_index);
    rev = rev(sort_index);
end
