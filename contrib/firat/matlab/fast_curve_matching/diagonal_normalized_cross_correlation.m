% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/diagonal_normalized_cross_correlation.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 15, 2011

function [R, c] = diagonal_normalized_cross_correlation(T, I)
    M = size(T,1);
    N = size(I,1);
    s = N - M + 1;
    T = T(:);
    nT = T / norm(T);
    L = M * (M-2);
    z = ceil(s/2);
    R = zeros(z, 1);
    c = zeros(z, 1);    
    for j = 1:z
        i = 2*j-1;
        hr = i:(M-1+i);
        hc = i:(M-3+i);  
        X = reshape(I(hr, hc), L, 1);
        R(j) = sum(X .* nT)/norm(X);
        c(j) = i + (M-3)/2;
    end    
end
