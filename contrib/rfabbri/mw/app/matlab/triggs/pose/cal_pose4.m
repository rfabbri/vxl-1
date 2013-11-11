% Pose of a calibrated camera from n>=3 known 3D points, with unique
% solution for n>=4. Returns the solution rotations and translations R,t
% (with calibrated camera projection P=[R,t]), and the point depths u.

function [Rts,us] = cal_pose4(x,X,method,u)
   if nargin<3, method = 1; end;
   n = size(x,2);

   % Normalize image points to direction vectors and calculate 3D point
   % distances D(i,j) = |X(i)-X(j)|^2 and direction cosines C(i,j) =
   % 2*cos(theta(x(i),x(j))).

   C = 2*ones(n);
   D = zeros(n);
   xdirn = x(1:3,:) ./ (ones(3,1)*sqrt(sum(x(1:3,:).^2)));
   % cmin = 2; imin=1; jmin=1;
   for i = 2:n
      for j = 1:i-1
	 C(i,j) = 2*(xdirn(:,i)'*xdirn(:,j));   % 2*cos(theta_ij)
	 C(j,i) = C(i,j);
	 D(i,j) = sum( (X(:,i)-X(:,j)).^2 );	% |X_i-X_j|^2
	 D(j,i) = D(i,j);
	 % if C(j,i)<cmin, cmin=C(j,i); imin=i; jmin=j; end;
	 % u(i)^2+u(j)^2-C(i,j)*u(i)*u(j)-D(i,j)
      end;
   end;

   % Heuristic for improving numerical conditioning (sometimes...).
   % Choose the best-separated pair of points (minimal C(i,j)), and then
   % the third point best separated from these, and use these as a
   % basis.

   perm = zeros(0);
   if method>=100
      [imin,jmin] = find(min(min(C))==C);
      imin = imin(1); jmin = jmin(1);
      A = max(C([imin,jmin],:));
      kmin = find(min(A)==A);
      perm = [imin,jmin,kmin];
      if n>3
	 A = 1:n;
	 perm = [perm,A(find(A~=imin & A~=jmin & A~=kmin))];
      end;
      C = C(perm,perm);
      D = D(perm,perm);
      xdirn = xdirn(:,perm);
      X = X(:,perm);
      method = method-100;
   end;

   % y1 = u(1)^-2
   % A = pose3elim(C(1,2),D(1,2), C(1,3),D(1,3), C(2,3),D(2,3))
   % A * [y1^4;y1^3;y1^2;y1;1]
   % pose3elim(C(1,2),D(1,2), C(1,3),D(1,3), C(2,3),D(2,3), y1)
   
   % Solve for point-camera distances u(i).
   if method == 3 | method == 5, 
      n = 3; 
      X = X(:,1:3);
      xdirn = xdirn(:,1:3);
   end;
   if method == 1,		u = pose4elim(C,D);
   elseif method == 2,		u = pose4elim2(C,D);
   elseif n==3 
      if method == 3,		u = pose4elim(C(1:3,1:3),D(1:3,1:3));
      elseif method == 5,	u = pose3dixon(C(1,2),D(1,2), C(1,3),D(1,3), C(2,3),D(2,3));
      else,			error('no method!');
      end;
   elseif n==4
      if method == 9,		u = pose4_9(C,D);
      elseif method == 12,	u = pose4_12(C,D);
      elseif method == 24,	u = pose4_24(C,D);
      elseif method == 25,	u = pose4_24x(C,D,[1,0]);
      else,			error('no method!');
      end;
   else
				error('no method!');
   end;
   % if ~isempty(perm), u(:,perm) = u; end;

   % Eliminate any impossible solutions with negative depths, etc. For
   % n=3 points there will usually be multiple solutions.

   us = zeros(0,n);
   for i = 1:size(u,1)
      if min(u(i,:))>0
	 us = [us; u(i,:)];
      end;
   end;
   nsol = size(us,1);
   
   % Apply the depths to the direction vectors, and solve for camera
   % pose by finding the Euclidean transform that aligns the resulting
   % point cloud with the 3D points X.

   Rts = zeros(3*nsol,4);
   for i = 1:nsol
      Y = xdirn * diag(us(i,:));
      [R,t] = eucl_align_points(Y,X(1:3,:),1);
      Rts(3*i-2:3*i,:) = [R,t];
   end;
%end;