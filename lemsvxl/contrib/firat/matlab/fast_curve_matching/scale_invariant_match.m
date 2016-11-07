function [cf_center, cf_len] = scale_invariant_match(C1, C2, N, K)
    addpath /home/firat/lemsvxl/src/contrib/firat/courses/engn2560/top-down/segmentation
    S1 = curve2signal(C1, K, N);
    num_scales = 15;
    cf_center = cell(num_scales,1);
    cf_len = cell(num_scales,1);
    for i = num_scales/5:num_scales-1
        L = i*N/num_scales;
        S2 = curve2signal(C2, K, L);
        R = normalized_cross_correlation(S2, S1);
        [r,c] =  nonmaxsuppts(R, 11, 0.25);
        cf_center{i} = unique(c);
        cf_len{i} = ones(length(cf_center{i}),1) * L; 
    end
    cf_center = cell2mat(cf_center);
    cf_len = cell2mat(cf_len);
end



