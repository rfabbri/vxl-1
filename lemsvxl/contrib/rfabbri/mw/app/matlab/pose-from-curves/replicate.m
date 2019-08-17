function r = replicate(v,N)
% randomly select fr

n = size(v,2);
assert(n < N);
r = zeros(size(v,1),N);
for i=1:size(v,1)
  r(i,:) = replicatev(v(i,:),N);
end
