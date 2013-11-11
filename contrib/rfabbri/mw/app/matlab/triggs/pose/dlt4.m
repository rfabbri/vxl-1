% DLT-like pose + focal length from 4 known 3D points.

% Using the DLT on 4 or more known 3D points, the possible projection
% matrices are reduced to a 12-2*4=4 parameter family P = u1*P1+..+u4*P4
% where (u1..u4) are unknown parameters.  We use 4 supplementary
% calibration constraints aspect_ratio = 1, skew=0,
% principal_point=(0,0) (i.e.  w(1,1)-w(2,2) = w(1,2) = w(1,3) = w(2,3)
% = 0 where w is the DIAC) to eliminate (u1..u4) and solve for P, camera
% pose, calibration, etc.  The redundancy is 1 (since u1..u4 are only
% defined up to scale), so a linear solution is possible.  The
% constraints are all quadratic in (u1..u4). Each row of the 4x10 input
% matrix M represents one of these homogeneous quadratics (i.e. it is a
% flattened 4x4 symmetric matrix, with entries = coeffs of
% [u1^2,u1*u2,u2^2,...,u3*u4,u4^2]). The output multiresultant matrix
% has rank 79 in the noiseless case. The last 4 components of its null
% vector are (...u1..u4), which in turn gives P, pose, etc. The matrix
% was calculated with Maple using my extended multiresultant routine.

function [P,Rt,f,cond] = dlt4(x,X,method,refine)
   [Ps,cond] = proj_subs_from_pts(x,X,4);
%   if (method>10) 
%      method = method-10;
%      refine=1; 
%   else
%      refine = 0;
%   end;
   % cond
   % sum(abs(reshape(Ps,12,4))),sum(abs(reshape(Ps,12,4)'))
   bal = 1;
   if (bal==1)  Q = flipud(eye(4));
   elseif (bal==2) [Q,R] = qr(randn(4));
   elseif (bal==3)  [QQ,R,Q] = qr(randn(4)); 
   else Q = eye(4);
   end;
   Ps = reshape(reshape(Ps,12,4)*Q,3,16);

   M = zeros(4,10);
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
	 M(1,k) = W(1,1) - W(2,2);
	 M(2,k) = W(1,2);
	 M(3,k) = W(1,3);
	 M(4,k) = W(2,3);
	 k = k+1;
      end;
   end;
   % sum(abs(M)),sum(abs(M'))
   % M = diag(sum(M'.^2).^-0.5) * M;
   % MM = [M(:,[1,2,4,7]); M(:,[2,3,5,8]); M(:,[4:6,9]); M(:,7:10)]
   % [U,S,V]=svd(MM); diag(S)'
   %[Q,R] = qr(M)

   % Solve 4 quadric system using multiresultant
   if (nargin<3) method = 2; end;
   if (method == 3)	[u,cond1] = dlt4multires0(M);
   elseif (method==1)	[u,cond1] = dlt4multires(M);
   elseif (method==49)	[u,cond1] = dlt4multires49(M);
   elseif (method==84)	[u,cond1] = dlt4multires84(M);
   else			[u,cond1] = dlt4multires2(M);
   end;
   cond = [cond,cond1];
   v = [];
   if (nargin>=4 & refine)
      for i = 1:size(u,2)
	 [uu,e] = dlt4refine(M,u(:,i));
	 % [sqrt(e),norm(uu-u(:,i))]
	 if (size(v,2)>0)
	    t = uu * ones(1,size(v,2));
	    if (min(sum(abs([v-t,v+t]))) < 1e-7) 
	       % fprintf(1,'ignoring duplicate solution\n');
	       continue; 
	    end;
	 end;
	 v = [v,uu];
	 % u(:,i) = uu;
	 % u = [u,uu];      
      end;
      u = v;
   end;
   
   % Find projection matrix
   P = []; Rt = []; f = [];
   nskip = 0;
   for j = 1:size(u,2)
      P1 = zeros(3,4);
      for i = 1:4
	 P1 = P1 + u(i,j)*Ps(:,4*i-3:4*i);
      end;
      if (det(P1(:,1:3))<0) P1 = -P1; end;
      if (sum(sign(sum(x .* (P1*X)))) < 0)
	 nskip = nskip+1;
	 continue;
      end;
      if (nargout>1)
	 % Recover f and R,t by heuristic projection of P
	 % onto K*[R;t] with K = diag([f,f,1])
	 W = P1(:,1:3) * P1(:,1:3)';
	 f1 = sqrt((W(1,1)+W(2,2))/(2*W(3,3)));
	 Rt1 = diag([1,1,f1]) * P1;
	 [U,S,V] = svd(Rt1(:,1:3));
	 Rt1 = U * [V', diag(diag(S).^(-1)) * U'*Rt1(:,4)];
	 P1 = P1 * norm(Rt1,1)/norm(P1,1);
	 % if (det(Rt1(:,1:3))<0) Rt1 = -Rt1; P1 = -P1; end;
	 Rt = [Rt,Rt1];
	 f = [f,f1];
      end;
      P = [P,P1];
   end;
   if (nskip>0) 
      % fprintf(1,'skipped %d solution(s) with points behind camera\n',nskip);
   end;

%   % Find projection matrix
%   P = zeros(3,4);
%   for i = 1:4
%      P = P + u(i)*Ps(:,4*i-3:4*i);
%   end;
%   if (det(P(:,1:3))<0) P = -P; end;
%   if (nargout>1)
%      % Recover f and R,t by heuristic projection of P
%      % onto K*[R;t] with K = diag([f,f,1])
%      W = P(:,1:3) * P(:,1:3)';
%      f = sqrt((W(1,1)+W(2,2))/(2*W(3,3)));
%      Rt = diag([1,1,f]) * P;
%      [U,S,V] = svd(Rt(:,1:3));
%      Rt = U * [V', diag(diag(S).^(-1)) * U'*Rt(:,4)];
%      P = P * norm(Rt,1)/norm(P,1);
%      % if (det(Rt(:,1:3))<0) Rt = -Rt; P = -P; end;
%   end;
end;
