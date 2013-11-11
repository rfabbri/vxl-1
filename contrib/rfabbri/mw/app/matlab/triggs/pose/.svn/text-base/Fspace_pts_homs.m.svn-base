% [F12s,cond]=Fspace_pts_homs(x1s,x2s,H12s,k)

% From np pairs of homogeneous image points x1s,x2s and nh image 2 to
% image 1 homographies H12s, estimate a (9-k) dimensional subspace of the
% 9-D space of all 3x3 matrices likely to approximately contain the image
% 2 to image 1 fundamental matrix F12. This is used to obtain small
% subspaces of F-space on which to enforce nonlinear constraints, as in
% the `7-point' F-matrix and `6-point' essential matrix methods, or from
% which to project a valid F-matrix as in the `8-point + SVD' method.

% Input: matrices x1s(3,np),x2s(3,np) of standardized homogeneous image
%   points, and H12s(3,3*nh) of nh standardized image 2->1 homographies.
% Output: estimated fundamental matrix basis F12(3,3*(9-k)) and
%   condition numbers (normalized singular values), which
%   should ideally be close to [ones(k),zeros(9-k)]

% Method: Each point pair gives one linear constraint x1'*F*x2=0 on the 9
% components of F, and each homography gives a symmetric 3x3 matrix of
% them: H12'*F12+F12'*H12=0. Assemble the constraints into an (np+6*nh)x9
% matrix A, SVD to find the minimal residual error (9-k)-D subspace, and
% read off the F matrices.  In applications, *any* (9-k)-D subspace
% containing the true F would usually do, but the minimum residual
% subspace contains the directions of greatest uncertainty, so is the
% subspace most likely to lie close to the true F, and on which nonlinear
% constraints are likely provide the most useful reduction of variability.


function [F12s,cond]=Fspace_pts_homs(x1s,x2s,H12s,k)
   if (k<1 | k>8) error('bad subspace dimension: %d',k); end;
   [d1,np] = size(x1s);
   [d2,n2] = size(x2s);
   [dh,nh] = size(H12s);
   nh = nh/3;
   nc = np + 6*nh;
   if (nc<k) error('too few constraints for solution: %d/%d',nc,k); end;
   A = zeros(max(nc,9),9);

   % points give scalar constraint x1'*F12*x2 = 0
   if (np>0)
      if (~(d1==3) | ~(d2==3)) error('bad x1/x2 point dimension: %d/%d',d1,d2); end
      if (~(n2==np)) error('#points in x1 and x2 differ: %d/%d',np,n2); end;
      for p = 1:np
	 A(p,:) = kron(x2s(:,p)',x1s(:,p)');
      end;
   end;

   % Homographies give 3x3 symmetric constraint H12'*F12 + F12'*H12 = 0
   if (nh>0)
      for p = 1:nh
	 H = H12s(:,3*p-2:3*p);
	 for i = 1:3
	    for j = 1:i
	       c = zeros(1,9);
	       if (i==j)
		  c(3*j-2:3*j) = 2*H(:,i)';
	       else
		  c(3*j-2:3*j) = H(:,i)';
		  c(3*i-2:3*i) = H(:,j)';
	       end;
	       A(np+6*(p-1)+i*(i-1)/2+j,:) = c;
	    end;
	 end;
      end;
   end;
   [U,S,V] = svd(A);
   cond = diag(S)/S(1,1);
   F12s = sqrt(2)*reshape(V(:,k+1:9),3,3*(9-k));
%end;
