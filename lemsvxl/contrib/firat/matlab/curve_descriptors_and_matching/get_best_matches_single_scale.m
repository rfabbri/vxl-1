% This is /lemsvxl/contrib/firat/matlab/curve_descriptors_and_matching/get_best_matches_single_scale.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function [m, sorted_scores] = get_best_matches_single_scale(P, q, K, method, q_indices)
    step_size = 2;
    M = size(P, 1);
    S = zeros(1,(M-1)/step_size+1);
    switch method
        case 'tangent' 
            theta_prime_P = diff_tangent_angles(P, 1);
            theta_prime_q = diff_tangent_angles(q, 0);
            for i = 1:step_size:M   
                S((i-1)/step_size+1) = score_diff_tangent_angles(i, theta_prime_P, theta_prime_q); 
            end 
        case 'chord'
            alpha_prime_P = diff_chord_angles(P, 1);
            beta_prime_q = diff_chord_angles(q, 0, q_indices); 
            for i = 1:step_size:M   
                S((i-1)/step_size+1) = score_diff_chord_angles(i, alpha_prime_P, beta_prime_q, q_indices);
            end
    end
    [sorted_scores, m] = sort(S, 'descend');
    m = (m-1)*step_size+1;
    if K < length(S)
        sorted_scores = sorted_scores(1:K);
        m = m(1:K);
    end
end

