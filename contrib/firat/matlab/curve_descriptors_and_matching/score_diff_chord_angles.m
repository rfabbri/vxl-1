% This is /lemsvxl/contrib/firat/matlab/curve_descriptors/score_diff_chord_angles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function S = score_diff_chord_angles(m, alpha_prime_P, beta_prime_q, q_indices)
    M = size(alpha_prime_P, 2);
    L = size(beta_prime_q, 2);
    i = q_indices;
    j = 1:(L-2);
    k1_P = kappa(i,m,L,M);
    k2_P = kappa(j,m,L-2,M);    
    k1_q = 1:length(i);
    k2_q = j+1;    
    D = angular_distance(alpha_prime_P(k1_P,k2_P), beta_prime_q(k1_q,k2_q));
    S = 1 - mean(mean(D))/pi;
end

