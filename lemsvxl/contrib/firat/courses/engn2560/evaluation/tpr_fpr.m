function [tpr, fpr] = tpr_fpr(A, G)
        Nplus = sum(G(:));
        Nminus = prod(size(G)) - Nplus;
        TP = sum(sum(A & G));
        FP = sum(sum(A & (~G)));
        tpr = TP/Nplus;
        fpr = FP/Nminus; 
end
