% This is /lemsvxl/contrib/firat/matlab/curve_descriptors/score_diff_tangent_angles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function S = score_diff_tangent_angles(m, theta_prime_P, theta_prime_q)
    M = length(theta_prime_P);
    L = length(theta_prime_q);
    i = 1:(L-4);
    k_P = kappa(i,m,L-4,M);
    k_q = i+2;
    D = angular_distance(theta_prime_P(k_P), theta_prime_q(k_q));
    S = 1 - mean(D);
end

