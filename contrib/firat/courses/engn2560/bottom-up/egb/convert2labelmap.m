function convert2labelmap(imfile, outfile)
        I = double(imread(imfile));
        K = I(:,:,1) + 300*I(:,:,2) + 300*300*I(:,:,3);
        P = unique(K);
        for a = 1:length(P)
                K(K == P(a)) = a;
        end
        dlmwrite(outfile, K, ' ');
        fprintf('Validation: num of components = %d\n', max(K(:)));        
end

