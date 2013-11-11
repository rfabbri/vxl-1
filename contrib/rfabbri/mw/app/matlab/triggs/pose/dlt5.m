% DLT-like pose + focal length from 5 known 3D points.

% Using the DLT on 5 or more known 3D points, the possible projection
% matrices are reduced to a 12-2*5=2 parameter family P = u1*P1+u2*P2.
% Adding 2 supplementary calibration constraints aspect_ratio = 1,
% skew=0 and using the resultant allows us to solve linearly for P and
% hence calibration K (focal length f and principal point since s=0,a=1)
% and pose R,t. The constraints are linear in the IAC but quadratic in
% the DIAC W=P*DAC*P' and hence quartic in u1,u2:
% (W(2,2)-W(1,1))*W(3,3)+W(1,3)^2-W(2,3)^2 = 0 =
% W(1,2)*W(3,3)-W(1,3)*W(2,3).  The resultant matrix is 8x8 in
% [u1^7,u1^6*u2,...,u2^7].  For coplanar points it has rank only 6 so
% the method fails.

function [P,K,Rt,cond] = dlt5(x,X)
   % Find 2d subspace of possible projections from 5 points
   [Ps,cond] = proj_subs_from_pts(x,X,2);

   % Set up skew=0, aspect=1 calibration constraints on DIAC.
   % These are quadratic in DIAC=P*DAC*P' and hence quartic in u1,u2
   % where P=u1*P1+u2*P2. M(:,i) gives the coeff of u1^(5-i)*u2^(i-1)
   % for the 2 constraints.
   A1 = Ps(:,1:3);
   A2 = Ps(:,5:7);
   B11 = A1*A1';
   B12 = A1*A2'+A2*A1';
   B22 = A2*A2';
   M = zeros(2,5);
   M(1,1) = (-B11(1,1)+B11(2,2))*B11(3,3)-B11(2,3)^2+B11(1,3)^2;
   M(1,2) = -2*B12(2,3)*B11(2,3)+2*B12(1,3)*B11(1,3)...
       +(-B12(1,1)+B12(2,2))*B11(3,3)+(-B11(1,1)+B11(2,2))*B12(3,3);
   M(1,3) = 2*B22(1,3)*B11(1,3)+B12(1,3)^2-2*B22(2,3)*B11(2,3)...
       -B12(2,3)^2+(B22(2,2)-B22(1,1))*B11(3,3)...
       +(-B12(1,1)+B12(2,2))*B12(3,3)+(-B11(1,1)+B11(2,2))*B22(3,3);
   M(1,4) = 2*B22(1,3)*B12(1,3)+(B22(2,2)-B22(1,1))*B12(3,3)...
       +(-B12(1,1)+B12(2,2))*B22(3,3)-2*B22(2,3)*B12(2,3);
   M(1,5) = (B22(2,2)-B22(1,1))*B22(3,3)-B22(2,3)^2+B22(1,3)^2;
   M(2,1) = B11(1,2)*B11(3,3)-B11(1,3)*B11(2,3);
   M(2,2) = -B12(1,3)*B11(2,3)-B11(1,3)*B12(2,3)...
       +B12(1,2)*B11(3,3)+B11(1,2)*B12(3,3);
   M(2,3) = -B22(1,3)*B11(2,3)-B12(1,3)*B12(2,3)-B11(1,3)*B22(2,3)...
       +B22(1,2)*B11(3,3)+B12(1,2)*B12(3,3)+B11(1,2)*B22(3,3);
   M(2,4) = -B22(1,3)*B12(2,3)-B12(1,3)*B22(2,3)...
       +B22(1,2)*B12(3,3)+B12(1,2)*B22(3,3);
   M(2,5) = B22(1,2)*B22(3,3)-B22(1,3)*B22(2,3);

   % sum(abs(M)),sum(abs(M'))
   % M = diag(sum(M'.^2).^-0.5) * M;
   % sum(abs(M)),sum(abs(M'))

   % Solve 2 quartic system using resultant + SVD
   [u,cond1] = dlt5multires(M);
   cond = [cond,cond1];

   % Find projection matrix
   P = u(1)*Ps(:,1:4) + u(2)*Ps(:,5:8);
   if (det(P(:,1:3))<0) P = -P; end;

   % Recover K,R,t by heuristic projection of P
   % onto K*[R;t] with K = [f,0,u; 0,f,v; 0,0,1])
   if (nargout>1)
      % RQ factorize P(:,1:3)=K*R
      D = fliplr(eye(3));
      [R,K] = qr(D*P(:,1:3)'*D);
      R = D*R'*D;
      K = D*K'*D;
      D1 = diag(sign(diag(K)));
      K = K*D1;
      R = D1*R;
      % Project K onto a=1,s=0 subspace
      Rt = inv(K)*P;
      K = K/K(3,3);
      K(1,1) = sqrt(K(1,1)*K(2,2));
      K(2,2) = K(1,1);
      K(1,2) = 0;
   end;
end;
