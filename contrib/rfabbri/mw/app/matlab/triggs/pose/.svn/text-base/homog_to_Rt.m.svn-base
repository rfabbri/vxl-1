% M = homog_to_Rt(H,side) --  Estimate relative orientation of
% two calibrated cameras from a plane homography H. 
%
%   Any two perspective images of a set of coplanar 3D points are
% projectively equivalent, i.e. there is a 2D homography (projective 
% transformation) H mapping the points of one image to the other. 
% H is represented by a 3x3 matrix, and can be recovered linearly 
% from >=4 image-of-coplanar-point correspondences.
%   If the cameras are internally calibrated, we can recover their
% poses relative to each other and the plane from H, up to at most a
% two-fold ambiguity. Some false solutions can be eliminated by sign
% (visibility) tests, but usually either points off the plane or 
% constraints on the 3D affine or metric structure of the plane are
% needed to reliably disambiguate the solutions.
%
% Input: 3x3 homography H in calibrated coordinates, with arbitrary
%	scale but sign chosen so that positive depth is preserved, 
%       i.e. x2' * H * x1 > 0 for all corresponding image points 
%       x1,x2 on plane). Optional argument SIDE is any finite visible
%       point on the plane in image 1. It is used to choose the right
%       sign for the plane normal n and translation t. The default
%       [0;0;1] is *only* valid if the forward optical axis intersects the
%       plane finitely (which can easily be false). If side is wrong
%       the signs of n,t will be wrong, but could be corrected elsewhere if
%       necessary. 
%       
% Output:  two 4x4 matrices Mi = [ R  | t   ]  where
%                                [ n' | 1/d ]
%	R = relative rotation
%	t = translation/optical centre unit vector
%	- camera matrices are P1 = (I|0), P2 = R.(I|-t)
%	n.x=d is plane with  depth d>0 and normal n (pointing *away* 
%	from cameras, i.e. n'*x>0 for visible points x on plane)
%
% Method: similar to [Wunderlich 1982], but for stability we work with
% the SVD of H rather than the eigendecomposition of H'*H. 1/d >>0 is a
% good measure of the numerical conditioning of the solution.

% Notes:
%  - if P1=(I|0), P2=R(I|-t), and plane is n.x-d=0, the homography is
%    H ~ R*H1 where H1 = (I-t*n'/d)
%  - the SVD of H=U*S*V' is same as that of H1=U1*S*V', except that
%    U contains an extra factor R. 
%  - H1 leaves cross product t^n untouched, corresponding to the middle
%    SV (s2=1) of H1 and hence the middle column v2 of V (and also of V1
%    and U1). So we normalize by S -> S/s2.
%  - the other two columns {v1,v3} span {t,n}. Any direction orthogonal 
%    to n has norm unchanged by H1, in particular (t^n) and 
%    n^(t^n) = a.v1+b.v3 say, whence (s1.a)^2+(s3.b)^2=(a^2+b^2) 
%  - solve to get (a,b) ~ (+-sqrt(1-s3^2),+-sqrt(s1^2-1)), then
%    n ~ b*v1-a*v3 (as n is orthogonal to v2 and a*v1+b*v2).
%  - by the same argument on the left, R*t ~ -(b*u1+a*u3).
%  - these relations hold up to b->-b and (n,t)->(-n,-t)
%  - t is an eigenvector of H1 with eigenvalue (1-t.n/d), so
%    H*t = (1-t.n/d) R*t which gives t ~ H^-1*(R*t) ~ b/s1*v1+a/s3*v3
%    and 1/d = s1-s3 (after an ugly simplification).
%  - columns 1 and 3 of U1 are linear combinations of v1 and v3 as they 
%    are orthogonal to v2=col(U1,2). Requiring that U1*S*V=H1 preserve
%    t gives the coefficients c ~ sqrt(b^2/s1+a^2/s3), d ~ a.b.(1/s1-1/s3).
%  - canceling U1 from U gives R.
%  - the formulation should still be stable even if t is parallel to n
%    (in which case s1=s2>s3, t^n=0, (a,b)~(1,0), t ~ n ~ v3, but v1,v2
%    get mixed up), or orthogonal to n (in which case t.n=0, s1>s2=s3,
%    (a,b)~(0,1), n ~ v1, R*t ~ u1, but v2,v3 and u2,u3 get mixed up),
%    provided 0 << 1/d=s1-s3 << infinity, i.e the plane is not too far
%    away, and the cameras stay on one side of it. For 1/d -> 0, S ~= I
%    and a,b,n and t become unreliable but R should still be OK. Also,
%    if the points are not well spread in the images, H -- and hence
%    n,t,R -- will be inaccurate.
%  - for *any* putative homography, no matter how wrong, there is a
%    solution (or actually 2). If the scene is really planar both are
%    usually internally self-consistent.

function M = homog_to_Rt(H,side)

   [U,S,V] = svd(H);
   s1 = S(1,1)/S(2,2);
   s3 = S(3,3)/S(2,2);
   a1 = sqrt(1-s3^2); 
   b1 = sqrt(s1^2-1);
   [a,b] = unitize(a1,b1);
   [c,d] = unitize( 1+s1*s3, a1*b1 );
   [e,f] = unitize( -b/s1, -a/s3 );
   v1 = V(:,1); v3 = V(:,3);
   
   n1 = b*v1-a*v3;
   n2 = b*v1+a*v3;
   invd = s1-s3;
   R1 = U*[c,0,d; 0,1,0; -d,0,c]*V';
   R2 = U*[c,0,-d; 0,1,0; d,0,c]*V';
   t1 = e*v1+f*v3;
   t2 = e*v1-f*v3;
   %% translations in t'=-Rt form (c.f. P2 = R.(I|-t) = (R|t')) :
   % rt1 = -(b*U(:,1)+a*U(:,3));
   % rt2 = -(b*U(:,1)-a*U(:,3));	
   %% direct form of Essential matrices :
   % E1 = (a*U(:,1)-b*U(:,3))*V(:,2)'+U(:,2)*(f*v1-e*v3)'
   %    = R1*mcross(t1)
   % E2 = -(a*U(:,1)+b*U(:,3))*V(:,2)'-U(:,2)*(f*v1+e*v3)'
   %    = R2*mcross(t2)

   if (nargin<2) side = [0; 0; 1]; end;
   if (side' * n1 < 0)  t1 = -t1; n1 = -n1; end;
   if (side' * n2 < 0)  t2 = -t2; n2 = -n2; end;
   M1 = [ R1,t1; n1',invd ];
   M2 = [ R2,t2; n2',invd ];
   M = [M1, M2];
%end;
