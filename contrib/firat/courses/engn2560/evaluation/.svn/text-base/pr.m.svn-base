function [p, r] = pr(A, G)
        sizeG = sum(G(:));
        sizeA = sum(A(:));
        sizeAG = sum(sum(A & G));
        p = sizeAG / sizeA;
        r = sizeAG / sizeG; 
end
