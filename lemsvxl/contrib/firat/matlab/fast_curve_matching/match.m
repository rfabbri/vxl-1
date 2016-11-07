% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/match.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 1, 2011

function cf_center = match(C1, C2, N1, N2, K1, K2, max_match)
    addpath /home/firat/lemsvxl/src/contrib/firat/courses/engn2560/top-down/segmentation
    S1 = curve2signal(C1, K1, N1);
    S2 = curve2signal(C2, K2, N2);
    R = normalized_cross_correlation(S2, S1);
    [r,c] =  nonmaxsuppts(R, 13, 0.2);
    cf_center = unique(c);
    %m = max(R(:));
    %[r,c] = find(R == m)
    %imagesc(R); colormap gray;
end
