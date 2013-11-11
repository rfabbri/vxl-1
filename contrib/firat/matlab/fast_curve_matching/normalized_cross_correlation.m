function R = normalized_cross_correlation(T, I)
        R1 = normxcorr2(T, I);
        [M, N] = size(T);
        M = round((M-1)/2);
        N = round((N-1)/2);
        R = R1((M+1):(end-M), (N+1):(end-N));
end


