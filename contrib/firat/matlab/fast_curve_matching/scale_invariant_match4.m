function [cf_center, cf_len, cf_rev] = scale_invariant_match4(C1, C2, N, K, maxN)
    S11 = curve2signal(C1, K, N);
    S12 = curve2signal(C1(end:-1:1,:), K, N);
    num_scales = 20;
    cf_center = cell(num_scales,1);
    cf_len = cell(num_scales,1);
    cf_rev = cell(num_scales,1);
    scores = cell(num_scales,1);
    for i = round(num_scales/8):num_scales-1
        L = i*N/num_scales;
        S2 = curve2signal(C2, K, L);
        R1 = normalized_cross_correlation(S2, S11);
        R2 = normalized_cross_correlation(S2, S12);
        [r1,c1] =  nonmaxsuppts(R1, 1, 0.1);
        [r2,c2] =  nonmaxsuppts(R2, 1, 0.1);        
        scores{i} = [diag(R1(r1,c1)); diag(R2(r2,c2))];      
        cf_center{i} = [c1; N-c2];
        cf_rev{i} = [zeros(size(c1)); ones(size(c2))];
        cf_len{i} = ones(length(cf_center{i}),1) * L; 
    end
    cf_center = cell2mat(cf_center);
    cf_len = cell2mat(cf_len);
    cf_rev = cell2mat(cf_rev);
    scores = cell2mat(scores);
    [sorted_scores, sort_index] = sort(scores, 'descend');
    if length(sorted_scores) > maxN
        sort_index = sort_index(1:maxN);
    end
    cf_len = cf_len(sort_index);
    cf_center = cf_center(sort_index);
    cf_rev = cf_rev(sort_index);
end



