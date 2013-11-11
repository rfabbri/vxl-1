% [u,v] = split_bipole(A) -- Given a rank 2 symmetric matrix A, split
% it into a symmetric sum of two vectors A = u * v' + v * u'. Usually, A
% represents a degenerate conic or quadric which we want to split into a
% point or hyperplane pair.

function [u,v] = split_bipole(A)
   [E,L] = eigs(A,1);
   if (L(1)<0) L = -L; end;
   a = sqrt( [L(1),-L(2)] );
%   if (any(imag(a)))
%      warning("Complex solution -- camera inside quadric??");
%   end;
   u = a(1) * E(:,1) + a(2) * E(:,2);
   v = L(1)/a(1) * E(:,1) + L(2)/a(2) * E(:,2);
   u = u/norm(u,2);
   v = v/norm(v,2);
%end;
