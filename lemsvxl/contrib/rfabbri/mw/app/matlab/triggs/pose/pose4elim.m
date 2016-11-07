% Pose of a calibrated camera from known 3D points. Subroutine for
% finding point depths in the case of n>=4 points, using an elimination
% based method. Returns a vector of point-camera distances u.i, i=1:n,
% or for 3 points a matrix of the <=4 possible distance vectors.

% The 3D points X(i) are represented by the squared 3D distances D(i,j)
% = |X(i)-X(j)|^2, and their images are represented by the cosines of
% the angles between their image rays C(i,j) = 2*cos(theta_ij). These
% are used in the cosine law formula u.i^2 + u.j^2 - C(i,j) = D(i,j),
% where u.i are the point-camera distances.  The method is basically
% that Quan & Lan ICCV'98, PAMI'99, which is similar to the 4 point
% method of Killian (1955).  Triples of equations involving u1 are
% reduced by elimination (in pose3elim()) to single quartics in
% y1=u1^-2. Each quartic gives a linear constraint on the monomial
% vector [y1^4,y1^3,y1^2,y1,1]. These constraints are combined into a
% matrix and their null space is found by SVD. For n>=5 points there are
% 4 or more constraints and the solution is unique. For n=4 points there
% are only 3 constraints and the solution must be found by enforcing the
% structure of the monomial vector. For n=3 points there is only 1
% constraint and we have a quartic to solve (4 solutions), which we do
% by eigendecomposition of the companion matrix.

function u = pose4elim(C,D)
   n = size(C,1);
   u = zeros(0,n);
   if n == 3
      % 3 point method. Find quartic in y1=u1^-2 and solve for y1
      % by companion matrix method 
      A = pose3elim(C(1,2),D(1,2), C(1,3),D(1,3), C(2,3),D(2,3));
      M = diag(ones(1,3),-1);
      M(1,:) = -A(2:5)/A(1);
      ys = eig(M);

      % Eliminate any imaginary or negative solutions, and back solve
      % for the depths for each solution y1.
      ys = real(ys(find(~imag(ys))));
      if isempty(ys), return; end;
      ys = ys(find(ys>0));
      nsol = size(ys,1);
      u = zeros(nsol,3);
      for i = 1:nsol
	 u(i,:) = pose3elim(C(1,2),D(1,2), C(1,3),D(1,3), ...
	     C(2,3),D(2,3), ys(i));
      end;
      return;
   end;

   % n>=4 point methods. Build all the quartics in y1=u1^-2 that
   % we can, and find their null space.
   A = zeros((n-1)*(n-2)/2,5);
   k = 1;
   for i = 2:n-1
      for j = i+1:n
	 A(k,:) = pose3elim(C(1,i),D(1,i), C(1,j),D(1,j), C(i,j),D(i,j));
	k = k+1;
      end;
   end;
   [U,S,V] = svd(A);
   % S = diag(S)'

   % Find the vector v=[y1^4,..,y1,1] up to scale.  For n>=5 points this
   % is just the unique null vector. For n=4 we need to find the
   % solution in the 2D null space formed by v4,v5. We do this by
   % requiring that the components of the solution v = a*v4+b*v5 (for
   % some scalars a,b) satisfy the monomial identities y1^i * y1^(i-j) =
   % y1^k * y1^(i-k) for all combinations of i,j,k that we can calculate
   % from [y1^4..y1,1]. This gives a 7x3 system in (a^2,a*b,b^2) which
   % we solve linearly by SVD.

   if n >= 5
      v = flipud(V(:,5));
   else
      v4 = flipud(V(:,4));
      v5 = flipud(V(:,5));
      
      combi = [4,2,3,3; 4,1,3,2; 4,0,3,1; 4,0,2,2; ...
	       3,1,2,2; 3,0,2,1; 2,0,1,1] +1;
      ncombi = size(combi,1);
      B = zeros(ncombi,3);
      for r = 1:ncombi
	 i = combi(r,1); 
	 j = combi(r,2);
	 k = combi(r,3);
	 l = combi(r,4);
	 B(r,:) = [ v4(i)*v4(j)-v4(k)*v4(l), ...
		  ( v4(i)*v5(j)-v4(k)*v5(l) ...
		   +v5(i)*v4(j)-v5(k)*v4(l) ), ...
		    v5(i)*v5(j)-v5(k)*v5(l) ];
      end;
      [U,S,V] = svd(B);
      % S = diag(S)'
      vv = V(:,3);
      if abs(vv(1)) >= abs(vv(3))
	 v = vv(1)*v4 + vv(2)*v5;
      else
	 v = vv(2)*v4 + vv(3)*v5;
      end;
   end;

   % Read off y1 from v
   if abs(v(1)) >= abs(v(5))
      y1 = v(2)/v(1);
   else
      y1 = v(5)/v(4);
   end;

   % Back solve for the u.i from y1=u1^-2. We solve first for u1,2,3
   % using the 3 point solver, then use y1 and y2=u2/u1 to solve for
   % u.i, i=3:n (using again the elimination formula from the 1,2,i 3
   % point method). For good numerical conditioning, it is best to order
   % the points so that 1 and 2 are well separated in the image.

   u = pose3elim(C(1,2),D(1,2), C(1,3),D(1,3), ...
			    C(2,3),D(2,3), y1);
   if isempty(u), return; end;
   u = [u,zeros(1,n-3)];
   y2 = u(2)/u(1);
   for i = 4:n
      u(i) = ((D(1,i)-D(2,i))*y1+y2^2-1)/(C(2,i)*y2-C(1,i))*u(1);
   end;
%end;
