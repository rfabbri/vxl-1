function r = replicate(v,N)
% randomly select fr

n = length(v);
r = [v v(randi([1 n],1,N-n))];
