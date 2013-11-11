function A = ivec(v,r); 
% Creates a matrix A with r rows from vector v 
% A is filled columnwise. Is the inverse of VEC

A = reshape(v,r,length(v)/r);
