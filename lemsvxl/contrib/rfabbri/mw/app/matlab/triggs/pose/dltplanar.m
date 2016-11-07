% DLT-like camera pose + calibration from images of known planes

% Given 4 or more known points on each of N unknown 3D planes (i.e. the
% point positions on each plane are known, but the relative positions of
% the planes and the camera are unknown), and their images in an
% uncalibrated or partially calibrated projective camera, find the
% unknown camera parameters and the camera pose relative to each plane.
%
% Inputs: 
% x = 3xNpoints homogeneous image coordinates
% X = 3xNpoints homogeneous 3D coordinates (x,y,w)' in the plane frames
%   (any frame where the plane is z=0)
% planeno = Nplanes x 2 matrix giving range of columns of x,X
%   belonging to each plane
% Kpars = 2x5 matrix specifying constraints on the 5 calibration
%  parameters [f,s,a,u0,v0]. The second row entries are 0 for known
%  parameters, 1 for unknown ones to be estimated. The first row
%  gives the known values for known parameters, and approximate/
%  nominal values for unknown ones (used for preconditioning).
%
% Outputs:
% K = estimated 3x3 camera calibration
% Hs = estimated 3D plane to image homographies
% Rt = estimated rotation and translation to plane (H = K*[R(:,1:2),t]).

function [K,Hs,Rt,conds] = dltplanar(x,X,planeno,Kpars)
   nplanes = size(planeno,1);
   if (nargin<4)
      % Defaults: for 1 plane estimate only f (although we could
      % actually do 2 parameters), and for 2 planes only f and principal
      % point.
      Kbase = [1,0,1,0,0];
      if (nplanes==1) Kfree = [1,0,0,0,0]; 
      elseif (nplanes==2) Kfree = [1,0,0,1,1]; 
      else Kfree = ones(1,5);
      end;
   else
      Kbase = Kpars(1,:);
      Kfree = Kpars(2,:);
   end;
   if (sum(~~Kfree)>2*nplanes) 
      error('Not enough planes to fit # parameters');
   end;
   % Precondition with inverse nominal calibration
   K0 = [Kbase(1), Kbase(1)*Kbase(2), Kbase(4); ...
	  0,Kbase(1)*Kbase(3),Kbase(5); ...
	  0,0,1];
   x = K0 \ x;
   % Set up autocalibration constraints. Each plane gives 2
   % constraints which essentially say that orthogonal direction
   % vectors in the 3D plane project to orthogonal vectors in the
   % calibrated image. Pulling back the IAC = inv(K')*inv(K) using
   % the plane-to-image homography H and restricting to the planes
   % line at infinity gives (H'*IAC*H)(1:2,1:2) ~ eye(2), i.e. linear
   % constraints on IAC(K) given H. We do this for each plane and
   % solve for IAC, the decompose it to get K.

   Hs = zeros(3,3*nplanes);
   A = zeros(2*nplanes,6);
   conds = [];
   for p=1:nplanes
      n0 = planeno(p,1);
      n1 = planeno(p,2);
      if (n1-n0+1 < 4)
	 error('Need >=4 points to fit homography'); 
      end;
      [H,cond1] = proj_from_pts_lin(x(:,n0:n1),X(:,n0:n1));
      % [HH,cond1] = proj_subs_from_pts(x(:,n0:n1),X(:,n0:n1),4);
      % H = HH(:,4*4-3:4*4)
      conds = [conds; cond1];
      % H'*H
      Hs(:,3*p-2:3*p) = H;
      for i=1:3
	 for j=1:i-1
	    A(2*p-1:2*p,i*(i-1)/2+j) = ...
		[2*(H(i,1)*H(j,1)-H(i,2)*H(j,2)); ...
	         H(i,1)*H(j,2)+H(i,2)*H(j,1)];
	 end;
	 A(2*p-1:2*p,i*(i-1)/2+i) = [H(i,1)^2-H(i,2)^2; H(i,1)*H(i,2)];
      end;
   end;
   % Squeeze out any known parameters. Actually we fudge things slightly
   % here when skew s!=0 and u0,v0 or a are known. s!=0 couples things,
   % so instead of setting (u0,v0,a)=(0,0,1) in the preconditioned
   % coordinates as we should, we actually set
   % (u0*a-v0*s,v0*(1+s^2)-u*s*a,a/sqrt(1+s^2)) to (0,0,1). But the
   % difference is small if s<<1, a~=1. There is no error if u0,v0,a are
   % treated as unknowns.
   B = A;
   if (~Kfree(5)) B(:,5) = []; end;	% know v0=0
   if (~Kfree(4)) B(:,4) = []; end;	% know u0=0
   if (~Kfree(3)) B(:,1) = B(:,1)+B(:,3); B(:,3) = []; end;	% know a=1
   if (~Kfree(2)) B(:,2) = []; end;	% know s=0
   if (~Kfree(1)) n=size(B,2); B(:,n)=B(:,n)+B(:,1); B(:,1)=[]; end; % know f=1
   
   % Solve for the free calibration parameters, if any.
   n = size(B,2);
   if (n>1)
      [U,S,V] = svd(B);
      S = diag(S)';
      conds = [S(n-1)/S(1), S(n)/S(n-1); conds];
      v = V(:,n);
   else
      conds = [1,0; conds];
      v = [1];
   end;
   % Unpack and factorize calibration update
   if (~Kfree(1)) v = [v(size(v,1)); v]; end;
   if (~Kfree(2)) v = [v(1); 0; v(2:size(v,1))]; end;
   if (~Kfree(3)) v = [v(1:2); v(1); v(3:size(v,1))]; end;
   if (~Kfree(4)) v = [v(1:3); 0; v(4:size(v,1))]; end;
   if (~Kfree(5)) v = [v(1:4); 0; v(5:size(v,1))]; end;

   C = [v(1),v(2),v(4); v(2),v(3),v(5); v(4),v(5),v(6)];
   if (0)
      if (trace(C)<0) C = -C; end;
      Kinv = chol(C);
   else
      % hack to ensure C is +ve definite before Choleski
      % As C is symmetric, V1*U1' gives signs converting sing. values
      % to eigenvalues
      [U1,S1,V1] = svd(C);
      S2 = diag(S1) .* sign(diag(U1'*V1));
      if (sum(S2)<0) S2 = -S2; C=-C; end;
      CC = U1 * diag(S2 - 1.01*min([S2;0])*ones(3,1)) * U1';
      % if (min(S2)<0) [min(S2),norm(C-CC)],[S2,C,CC],[diag(S1),U1,V1] end;
      Kinv = chol(CC);
   end;
   K = K0 * inv(Kinv);
   K = K/K(3,3);

   % Solve for plane poses from rectified homographies. 
   % We should have H ~= [R(:,1:2),t] so we straighten the first
   % two columns and extend them to a rotation matrix using SVD
   Rt = zeros(3,4*nplanes);
   cond1 = [0];
   for p = 1:nplanes
      H = Kinv * Hs(:,3*p-2:3*p);
      [U,S,V] = svd([H(:,1:2),zeros(3,1)]);
      S = diag(S)';
      cond1 = [cond1; abs((S(1)-S(2))/S(1))];
      R = U*V';
      if (det(R)<0) R = U*diag([1,1,-1])*V'; end;
      Rt(:,4*p-3:4*p) = [R, H(:,3)/sqrt(S(1)*S(2))];
   end;
   conds = [conds, cond1];
   Hs = K0 * Hs;
   
end;