% [Ses,cond]=dFspace_from_pts(x,v,k)
% See dF_from_pts_lin.m
% Estimate a `k-point', (9-k)-D subspace of the 9-D space of all 
% infinitessimal epipolar geometry (S,e) pairs, likely to approximately
% contain the true pair (S,e) linking n homogeneous image points x and
% their homogeneous image velocities v. This is used to
% obtain small subspaces of (S,e)-space on which to enforce nonlinear
% constraints.
%
% Method: each point pair gives a linear constraint on the 9 components
% of (S,e), assemble the constraints into an nx9 matrix A, SVD to find the
% minimal residual error (9-k)-D subspace, and read off the (S,e) pairs.
%
% Input: 3xn matrices x,v of standardized homogeneous image points
% Output: estimated fundamental matrix basis F12(3,3*(9-k)) and
% condition numbers (normalized singular values), which
% should ideally be close to [ones(k),zeros(9-k)]

function [Ses,cond]=dFspace_from_pts(x,v,k)
   if (k<1 | k>8) error('bad subspace dimension: %d',k); end;
   [d,n] = size(x); 
   [d2,n2] = size(v);
   if (~(d==3) | ~(d2==3)) error('bad x/v point dimension: %d/%d',d,d2); end
   if (~(n2==n)) error('#points in x and v differ: %d/%d',n,n2); end;
   if (n<k) error('too few points for solution: %d/%d',n,k); end;
   A = zeros(max(n,9),9);
   for p = 1:n
      xp = x(:,p);
      vp = v(:,p);
      A(p,:) = [ xp(1)^2, ...
	     2*xp(2)*xp(1), xp(2)^2, ...
	     2*xp(3)*xp(1), 2*xp(3)*xp(2), xp(3)^2, ...
	     xp(2)*vp(3) - xp(3)*vp(2), ...
	     xp(3)*vp(1) - xp(1)*vp(3), ...
	     xp(1)*vp(2) - xp(2)*vp(1) ];
   end;
   [U,S,V] = svd(A);
   cond = diag(S)/S(1,1);
   Ses = zeros(3,4*(9-k));
   for j = 1:9-k
      Vj = V(:,k+j);
      Ses(:,4*j-3:4*j) = [
	 Vj(1),Vj(2),Vj(4),Vj(7); ...
	 Vj(2),Vj(3),Vj(5),Vj(8); ...
	 Vj(4),Vj(5),Vj(6),Vj(9) ] / norm(Vj(7:9));
%       e = 0;
%       for p = 1:n
% 	 xp = x(:,p);
% 	 vp = v(:,p);
% 	 e = e + (xp'*(Ses(:,4*j-3:4*j-1)*xp - cross(Ses(:,4*j),vp)))^2;
%       end;
%       j,sqrt(e)
   end;
end;
