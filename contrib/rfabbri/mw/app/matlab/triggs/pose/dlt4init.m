% DLT-like pose + focal length from n>=4 known 3D points. 

% Output: Estimated 3x4 camera projection P, camera pose Rt=[R,t], and
% focal length f, expressed in the below 3D input and image frames.

% Input: 
% - X(4,n): n>=4 homogeneous 3D points in a well-normalized Euclidean
%   3D frame (i.e. one whose origin is not too far from the centre of
%   the points, and whose scale is such that the point coordinates
%   have size O(1)).

% - x(3,n): n>=4 corresponding homogeneous image points in a
%   nominally-calibrated camera frame (i.e. one in which the unknown
%   true camera calibration matrix K is diag(f,f,1) with f not too far
%   from 1 -- get this by premultiplying points by inverse(K0) where K0
%   is some approximate nominal calibration matrix).

% - lambda = [err_weight, f_weight, f0]: weights for supplementary
%   constraints used to improve conditioning of the solution.
%   err_weight>0 is used to weight point-projection residual error
%   vs. the calibration constraints (vanishing skew, etc). There is no
%   residual unless n>4 points.  f_weight>0 provides a weak prior on
%   estimated focal length f, w.r.t. a nominal value f0 (f0 is in
%   nominal input units, i.e. usually f0=1).
%
%   Heuristically, I find that including a prior for f hurts more often
%   than it helps so I set f_weight=0. However including the residual
%   error term definitely helps on average if you have n>4 points, and a
%   quite strong weight seems to do best here: err_weight ~= 1e2. (This
%   does slow the routine down a bit though).

% The solution method is based on multiresultants and hence uses a
% non-ideal error model, so nonlinear refinement of the output is highly
% recommended. If you have reasonable initial guesses of the pose and
% calibration, just using nonlinear refinement on its own may be
% appropriate. 

% Also, if you have n>=6 well-distributed 3D points, the traditional 6
% point DLT is both simpler and a lot faster.  Actually, the whole
% routine should probably be rewritten with specialized resultants
% (smaller & faster) for the n=5 and n>=6 cases.


function [P,Rt,f,cond] = dlt4init(x,X,lambda)
   
   % Find linear family P(u) = u1*P1 +..+ u4*P4 of 3x4 projections
   % consistent with the given n input points. With n==4 points, there
   % is no redundancy and all members of the family are equally
   % plausible. With n>4 points there is redundancy and some members
   % will have large error (encoded by singular-value-of-residual matrix
   % svs), but we still pull out a 4-D space of possible solutions
   % P1..P4.

   n = size(X,2);
   [Ps,svs] = proj_subs_from_pts(x,X,4);
   % Q = flipud(eye(4));
   % Ps = reshape(reshape(Ps,12,4)*Q,3,16);

   % Decide which polynomial constraints to include.
   npolys = 4;
   f_weight = lambda(2)^2;
   if f_weight>0		% prior on focal length f
      npolys = npolys+1;
      f02 = lambda(3)^2;
   end;
   nerr = npolys;
   err_weight = lambda(1)^2;
   if err_weight>0		% include penalty for image residuals (n>4)
      if n==5, npolys=npolys+2; 
      elseif n>=6, npolys=npolys+4; 
      else err_weight = 0;
      end;
   end;
   
   % Build npolys*10 matrix of constraint polynomials for multiresultant
   % solver. Each row contains the coefficients of a quadratic
   % polynomial in [u1^2,u1*u2,u2^2,u1*u3,...,u3*u4,u4^2] that encodes
   % one calibration or residual constraint on P(u) =
   % u1*P1+..+u4*P4. The calibration constraints depend only on the
   % leading 3x3 submatrix A(u) of P(u): from P = K*[R,t] we have
   % A(u)*A(u)' = K*R*R'*K' = K*K' = diag(f^2,f^2,1). Optionally, a weak
   % prior (f_weight<<1) on the focal length can be included to help
   % stabilize the solution.
   
   M = zeros(npolys,10);
   k = 1;
   for i = 1:4
      Ai = Ps(:,4*i-3:4*i-1);
      for j = 1:i
	 if (i==j) 
	    W = Ai * Ai';
	 else
	    Aj = Ps(:,4*j-3:4*j-1);
	    W = Ai * Aj' + Aj * Ai';
	 end;
	 M(1,k) = W(1,1) - W(2,2);	% aspect_ratio = 1
	 M(2,k) = W(1,2);		% skew = 0
	 M(3,k) = W(1,3);		% u0 = 0
	 M(4,k) = W(2,3);		% v0 = 0
	 if f_weight>0			% f ~= f0
	    M(5,k) = f_weight * ((W(1,1)+W(2,2))/2-f02*W(3,3));
	 end;
	 k = k+1;
      end;
   end;
   
   % For n>4 points, optionally include a penalty against solutions
   % P(u) with large reprojection error. Since SVD gave us orthogonal
   % vectors encoding the P.i, the error term is diagonal in the u.i,
   % i.e. the polynomials have only (u.i)^2 terms.
   
   if err_weight>0
      M(nerr+1,1) = err_weight * svs(2)^2;	% u1^2
      M(nerr+2,3) = err_weight * svs(3)^2;	% u2^2
      if n>=6
	 M(nerr+3,6) = err_weight * svs(4)^2;	% u3^2
	 M(nerr+4,10) = err_weight * svs(5)^2;	% u4^2
      end;
   end;

   % Solve the npoly>=4 quadric constraint system using a multiresultant.
   % This may fail for some degenerate geometries.

   [u,cond] = dlt4mres(M);
   cond = [svs,cond];
   
   % Reconstruct the projection matrix P(u) = u1*P1+..+u4*P4 from the
   % multiresultant solution u, and decompose it into motion Rt and
   % calibration f. Finesse the signs to ensure a forward facing
   % right-handed camera. This is ready to handle multiple solutions u
   % some of which may be crazy and have points behind the camera, etc,
   % but the multiresultant solver used above only ever gives one
   % solution.

   P = []; Rt = []; f = [];
   nskip = 0;
   for j = 1:size(u,2)
      P1 = reshape(reshape(Ps,12,4)*u(:,j),3,4);
      % Make sure rotation is proper.
      if (det(P1(:,1:3))<0) P1 = -P1; end;
      if (sum((P1(3,:)*X) >= 0) < n/2)

	 % The multiresultant solution has most points behind the
	 % camera. This happens mainly for planar and near-planar
	 % scenes. There is an exact ambiguity for planar scenes:
	 % reflecting the camera centre through the plane and rotating
	 % 180 degrees about the plane normal leaves the image unchanged
	 % except for behind-camera-ness. A heuristic fix is to estimate
	 % the plane normal from the scatter of the input points,
	 % transform the projection matrix to a frame centred on the
	 % points with x and y as major scatter directions and hence z
	 % as approximate plane normal, flip, and then transform back to
	 % the original coordinates.

	 % NB: This heuristic sometimes helps but is by no means
	 % foolproof. In particular, a common failure mode of the
	 % multiresultant solver (especially, but not exclusively, for
	 % near-planar scenes) is to return a false solution with
	 % extremely small focal length and the camera very close to the
	 % (near-)plane. This often activates flipping, but to no
	 % purpose as the input solution is just wrong. It's not the
	 % flip's fault.

	 XX = X(1:3,:)./(ones(3,1)*X(4,:));
	 Xc = sum(XX')'/n;	% centroid of points
	 DX = XX-Xc*ones(1,n);
	 [U,S,V] = svd(DX');
	 RX = V'/det(V);	% principal axes of points
	 R = RX'*diag([-1,-1,1])*RX;
	 P1 = P1 * [R, -Xc-R*Xc; zeros(1,3),-1];

	 fprintf(1,'flipped a solution with points behind camera\n'); 
      end
      if (nargout>1)

	 % Recover f and R,t by heuristic projection of P onto K*[R;t]
	 % with K = diag([f,f,1]). Another way would be to decompose
	 % P=K*[R,t] as usual, then adjust K to diag([f,f,1]). Neither
	 % is optimal, but nor is the multiresultant solver: subsequent
	 % nonlinear refinement is recommended.

	 W = P1(:,1:3) * P1(:,1:3)';
	 f1 = sqrt((W(1,1)+W(2,2))/(2*W(3,3)));
	 Rt1 = diag([1,1,f1]) * P1;
	 [U,S,V] = svd(Rt1(:,1:3));
	 Rt1 = U * [V', diag(diag(S).^(-1)) * U'*Rt1(:,4)];
	 P1 = P1 * norm(Rt1,1)/norm(P1,1);
	 Rt = [Rt,Rt1];
	 f = [f,f1];
      end
      P = [P,P1];
   end

%end
