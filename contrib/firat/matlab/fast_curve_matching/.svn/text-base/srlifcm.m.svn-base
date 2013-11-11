% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/srlifcm.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 15, 2011

% scale, rotation, location invariant fast curve matching

function [cf_center, cf_len, cf_rev] = srlifcm(C1, C2, N, K, num_scales, maxNmatch)
    if mod(N,2) == 0
        N = N+1;
    end
    S11 = curve2desc(C1, K, N, 1);
    S12 = curve2desc(C1(end:-1:1,:), K, N, 1);
    cf_center = cell(num_scales,1);
    cf_len = cell(num_scales,1);
    cf_rev = cell(num_scales,1);
    scores = cell(num_scales,1);
    for i = max(round(num_scales/8),1):num_scales
        L = round(i*N/num_scales);
        if mod(L,2) == 0
            L = L + 1;
        end
        S2 = curve2desc(C2, K, L, 0);
        [S2_h, S2_w] = size(S2);
        delta_h = (S2_h - 1) / 2;
        delta_w = (S2_w - 1) / 2;
        
        S111 = [S11(:,end-delta_w+1:end) , S11, S11(:, 1:delta_w)];
        S111 = [S111(end-delta_h+1:end,:) ; S111 ; S111(1:delta_h,:)];
        
        S121 = [S12(:,end-delta_w+1:end) , S12, S12(:, 1:delta_w)];
        S121 = [S121(end-delta_h+1:end,:) ; S121 ; S121(1:delta_h,:)];
        
        
        [R1, c1] = diagonal_normalized_cross_correlation(S2, S111);
        [R2, c2] = diagonal_normalized_cross_correlation(S2, S121);
                      
        scores{i} = [R1; R2]; 
        
        center1 = mod(c1-delta_w-1,  N) + 1;        
        
        center2 = N - mod(c2-delta_w-1,  N);
             
        cf_center{i} = [center1; center2];
        cf_rev{i} = [zeros(size(center1)); ones(size(center2))];
        cf_len{i} = ones(length(cf_center{i}),1) * L; 
    end
    cf_center = cell2mat(cf_center);
    cf_len = cell2mat(cf_len);
    cf_rev = cell2mat(cf_rev);
    scores = cell2mat(scores);
    [sorted_scores, sort_index] = sort(scores, 'descend');
    if length(sorted_scores) > maxNmatch
        sort_index = sort_index(1:maxNmatch);
    end
    cf_len = cf_len(sort_index);
    cf_center = cf_center(sort_index);
    cf_rev = cf_rev(sort_index);
end



