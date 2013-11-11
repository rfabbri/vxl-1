% [F12,e12,e21,cond] = Fmat_pts_homs_lin(x1,x2,H12)
% Linearly estimate 3x3 fundamental matrix F12 from n homogeneous image
% 1 & 2 points x1(3,n), x2(3,n) and nh image 2->1 plane
% homographies H12(3,3*nh)
%
% Input: homogeneous image point matrices X(3,n),Y(3,n)
% Output: estimated fundamental matrix F12 and condition numbers
%	cond = [smallest/largest `non-zero' singular value,
%	        `zero'/smallest `non-zero' singular value]
%	(ideally these should be close to [1,0] )
%
% Method: the `8 point' method, followed by SVD to enforce the det(F)=0
% constraint.

function [F12,e12,e21,cond] = Fmat_pts_homs_lin(x1,x2,H12)
   [F12,cond] = Fspace_pts_homs(x1,x2,H12,8);
   [U,S,V] = svd(F12);
   S = S/S(1,1);
   cond = [S(2,2); S(3,3); cond];
   S(3,3) = 0;
   F12 = U*S*V';
   e12 = V(:,3);
   e21 = U(:,3);
%end;
