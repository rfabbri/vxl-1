%returns only the part of the vector which has a surely valid 
% smoothing  value. It actually cuts 2sigma from two ends
% TODO: work for non-equal sampling
function vt = mytrim(v,sigma)

nel = max(size(v));
vt = v((2*sigma):(nel-2*sigma));
