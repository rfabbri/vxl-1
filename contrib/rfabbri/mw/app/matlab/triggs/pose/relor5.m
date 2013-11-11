% Relative orientation of two calibrated cameras from N=5 unknown 3D
% points, using sparse resultant. Similar but not identical to the
% method reported in [1].
%
% Let x be the direction vector and dx the distance to a 3D point in
% camera frame 1, and similarly y, dy in image 2. Let the motion between
% the camera frames be R(q),t, where q is a quaternion parametrizing the
% 3x3 inter-frame rotation R and t is the translation between the camera
% centres. Then 
%
%	dy * y = R * (dx * x) + t			(1)
%
% Also, the points will be visible iff dx>0 and dy>0.  The coplanarity
% constraint is
%
%	[y,R*x,t] = 0 
%
% where [,,] is the triple product. By the usual quaternion rotation
% formula, R*x = q*x*C(q) where C(q) is the conjugate quaternion. Also
% the triple product is the scalar part of the triple quaternion
% product, so the coplanarity constraint can be written
%
%	(y * q * x * C(q) * t)_0 = (y * q * x * qt)_0 = 0  
%
% where x,y,t are viewed as quaternions with vanishing scalar part, the
% product is quaternion product, (..)_0 is the scalar part, and qt is
% the quaternion C(q)*t. This constraint can also be written
%
%	q' * B(x,y) * qt = 0 
%
% where we regard q and qt as column 4-vectors and B is the 4x4
% matrix
%
%	B(x,y) = [ x*y' + y*x' - (y'*x)*eye(3), y^x; -(y^x)', -(y'*x) ]
%
% We have one of these constraints for each pair of corresponding
% points (x,y), and also the constraint
%
%	(qt * q)_0 = qt[0]*q[0]-sum(qt[i]*q[i],i=1..3) = 0
%
% which says that R'*t is a pure vector (vanishing scalar part), i.e. qt
% and q are consistent.
%
% For N points this gives us N+1 bilinear polynomials in the 4+4=8
% unknowns qt and q, with known coefficients (depending on the observed
% points x,y). We use a sparse multivariable resultant to reduce the
% solution of this problem to an eigenvector problem as follows (see
% reference [1] for the general method). The system has 20 roots in 10
% twisted pairs (i.e. differing by a relative 180 degree rotation about
% the line between the camera centres), but some of these are often
% complex.
%
% Use my Maple interface to apply Emeris' `far' routine to the above N+1
% polynomials with variable ordering [qt[1],qt[2],q[1],q[2],q[3]] (for
% normalization we take qt[4]=1=q[4], where q[4] is the scalar component
% of q). This gives a 60x60 matrix with 10 rows for each input
% polynomial, the coefficients being linear polynomials in the hidden
% variable qt[3], with coefficients either +-1 or an entry of a B
% matrix. Only 20 of the columns involve qt[3] -- permute these to the
% right hand side of the matrix.
%
% The first 10 rows (qt*q constraint) have 0's and 1's as coefficients,
% so Gaussian elimination based on these leaves a 50x50 matrix, still
% with 20 columns containing qt[3]. This 50x50 matrix is built below
% (M0,M1), by stacking A0 (10x50) and A1 (10x20) matrices, one for each
% point pair, where A1 contains the coefficients of qt[3] and A0 the
% constant coefficients.
%
% Now, continue Gaussian elimination of the first 30 columns of M0,
% applying the decomposition to the remaining 20 columns of M0 and to
% M1. This gives 20x20 matrices R0,R1. In fact, the implementation uses
% QR decomposition rather than LU, but that's only because rectangular
% LU is not available in Octave. 
%
% For solutions of the original problem, we have 
%
%	det(qt[4]*R0 + qt[3]*R1) = 0,
%
% In fact the solutions (q,t) to the original system can be read off
% from the generalized eigenvectors of the matrix pencil (R0,R1), whose
% components are proportional to the 20 cubic monomials in q[i],i=1..4.
% Ideally, we would use a generalized eigenvector routine here, but as
% Octave doesn't have that we find the eigenvectors of R0 \ R1. The
% result is equivalent, but less stable is R0 is ill-conditioned.  Given
% q and hence R, t is easy found from the coplanarity constraints.  The
% depths dx,dy can be recovered linearly from (1) above as usual.
%
% The method fails:
% 1) When the first 30 columns of M0 are not linearly independent or when
%    R0 is not full rank (it is not clear when these happen, but using
%    a generalized eigenvector routine would get around some rank deficiency
%    of R0).
% 2) When qt[3]/qt[4] becomes infinite, and probably also when
%    |(qt[3],qt[4])| << |(qt[1],qt[2])|. NB: qt = (q[4]*t-q^t,q'*t) where
%    ^ is cross product and q is the scaled rotation axis. 
% 3) When two or more solutions happen to have the same value of qt[3]/qt[4].
% 4) If t is very small compared to the distance to the scene.
%
% The failure 4 is intrinsic -- the depth can not be found, i.e. several
% solutions with different depths but the same R coincide.  It may be
% possible to work around 1-3 by applying a random rotation to x before
% calling the routine, then undoing it afterwards. Changes of variable
% ordering and hidden variable would also have a similar effect, but
% require another version of the same routine.
%
% References: 
% [1] Emiris, Ioannis Z., A General Solver Based on Sparse Resultants:
% Numerical Issues and Kinematic Applications, INRIA Research Report
% 3110, January 1997, http://www.inria.fr/RRRT/RR-3110.html

function [qs,Rts,ds] = relor5(ys,xs)
   N = 5;

   % Build up the 50x50 and 50x20 resultant matrices M0,M1, adding 10
   % rows A0,A1 for each point. Each row represents a different multiple
   % of the point pairs' coplanarity constraint polynomial q'*B(x,y)*qt.

   M0 = zeros(10*N,50);
   M1 = zeros(10*N,20);
   for p=1:N
      y = ys(:,p);
      x = xs(:,p);
      % B = [ x*y' + y*x' - (y'*x)*eye(3), y^x; -(y^x)', -(y'*x) ]
      
      B = [y(1)*x(1)-y(2)*x(2)-y(3)*x(3), y(1)*x(2)+y(2)*x(1), ...
	     y(1)*x(3)+y(3)*x(1), y(2)*x(3)-y(3)*x(2); ...
	   y(1)*x(2)+y(2)*x(1), -y(1)*x(1)+y(2)*x(2)-y(3)*x(3), ...
	     y(2)*x(3)+y(3)*x(2), -y(1)*x(3)+y(3)*x(1); ...
	   y(1)*x(3)+y(3)*x(1), y(2)*x(3)+y(3)*x(2), ...
	     -y(1)*x(1)-y(2)*x(2)+y(3)*x(3), y(1)*x(2)-y(2)*x(1); ...
	   -y(2)*x(3)+y(3)*x(2), y(1)*x(3)-y(3)*x(1), ...
	     -y(1)*x(2)+y(2)*x(1), -y(1)*x(1)-y(2)*x(2)-y(3)*x(3)];

      A0 = zeros(10,50);
      A1 = zeros(10,20);

      A0(1,24) = B(2,1);
      A0(1,25) = B(3,1);
      A0(1,26) = B(1,2);
      A0(1,27) = B(2,2)-B(1,1);
      A0(1,28) = B(3,2);
      A0(1,29) = B(4,1);
      A0(1,30) = B(4,2);
      A0(1,47) = B(1,4);
      A0(1,48) = B(2,4);
      A0(1,49) = B(3,4);
      A0(1,50) = B(4,4)+B(1,1);
      A0(2,16) = B(2,1);
      A0(2,17) = B(3,1);
      A0(2,20) = B(1,2);
      A0(2,22) = B(2,2)-B(1,1);
      A0(2,23) = B(3,2);
      A0(2,25) = B(4,1);
      A0(2,28) = B(4,2);
      A0(2,43) = B(1,4);
      A0(2,45) = B(2,4);
      A0(2,46) = B(3,4);
      A0(2,49) = B(4,4)+B(1,1);
      A0(3,3) = B(2,1);
      A0(3,4) = B(3,1);
      A0(3,10) = B(1,2);
      A0(3,13) = B(2,2)-B(1,1);
      A0(3,14) = B(3,2);
      A0(3,17) = B(4,1);
      A0(3,23) = B(4,2);
      A0(3,36) = B(1,4);
      A0(3,39) = B(2,4);
      A0(3,40) = B(3,4);
      A0(3,46) = B(4,4)+B(1,1);
      A0(4,15) = B(2,1);
      A0(4,16) = B(3,1);
      A0(4,19) = B(1,2);
      A0(4,21) = B(2,2)-B(1,1);
      A0(4,22) = B(3,2);
      A0(4,24) = B(4,1);
      A0(4,27) = B(4,2);
      A0(4,42) = B(1,4);
      A0(4,44) = B(2,4);
      A0(4,45) = B(3,4);
      A0(4,48) = B(4,4)+B(1,1);
      A0(5,2) = B(2,1);
      A0(5,3) = B(3,1);
      A0(5,9) = B(1,2);
      A0(5,12) = B(2,2)-B(1,1);
      A0(5,13) = B(3,2);
      A0(5,16) = B(4,1);
      A0(5,22) = B(4,2);
      A0(5,35) = B(1,4);
      A0(5,38) = B(2,4);
      A0(5,39) = B(3,4);
      A0(5,45) = B(4,4)+B(1,1);
      A0(6,1) = B(2,1);
      A0(6,2) = B(3,1);
      A0(6,8) = B(1,2);
      A0(6,11) = B(2,2)-B(1,1);
      A0(6,12) = B(3,2);
      A0(6,15) = B(4,1);
      A0(6,21) = B(4,2);
      A0(6,34) = B(1,4);
      A0(6,37) = B(2,4);
      A0(6,38) = B(3,4);
      A0(6,44) = B(4,4)+B(1,1);
      A0(7,18) = B(1,2);
      A0(7,19) = B(2,2)-B(1,1);
      A0(7,20) = B(3,2);
      A0(7,21) = -B(2,1);
      A0(7,22) = -B(3,1);
      A0(7,26) = B(4,2);
      A0(7,27) = -B(4,1);
      A0(7,41) = B(1,4);
      A0(7,42) = B(2,4);
      A0(7,43) = B(3,4);
      A0(7,47) = B(4,4)+B(1,1);
      A0(7,48) = B(2,1);
      A0(7,49) = B(3,1);
      A0(7,50) = B(4,1);
      A0(8,7) = B(1,2);
      A0(8,9) = B(2,2)-B(1,1);
      A0(8,10) = B(3,2);
      A0(8,12) = -B(2,1);
      A0(8,13) = -B(3,1);
      A0(8,20) = B(4,2);
      A0(8,22) = -B(4,1);
      A0(8,33) = B(1,4);
      A0(8,35) = B(2,4);
      A0(8,36) = B(3,4);
      A0(8,43) = B(4,4)+B(1,1);
      A0(8,45) = B(2,1);
      A0(8,46) = B(3,1);
      A0(8,49) = B(4,1);
      A0(9,6) = B(1,2);
      A0(9,8) = B(2,2)-B(1,1);
      A0(9,9) = B(3,2);
      A0(9,11) = -B(2,1);
      A0(9,12) = -B(3,1);
      A0(9,19) = B(4,2);
      A0(9,21) = -B(4,1);
      A0(9,32) = B(1,4);
      A0(9,34) = B(2,4);
      A0(9,35) = B(3,4);
      A0(9,42) = B(4,4)+B(1,1);
      A0(9,44) = B(2,1);
      A0(9,45) = B(3,1);
      A0(9,48) = B(4,1);
      A0(10,5) = B(1,2);
      A0(10,6) = B(2,2)-B(1,1);
      A0(10,7) = B(3,2);
      A0(10,8) = -B(2,1);
      A0(10,9) = -B(3,1);
      A0(10,18) = B(4,2);
      A0(10,19) = -B(4,1);
      A0(10,31) = B(1,4);
      A0(10,32) = B(2,4);
      A0(10,33) = B(3,4);
      A0(10,41) = B(4,4)+B(1,1);
      A0(10,42) = B(2,1);
      A0(10,43) = B(3,1);
      A0(10,47) = B(4,1);

      A1(1,17) = B(1,3);
      A1(1,18) = B(2,3);
      A1(1,19) = B(3,3)-B(1,1);
      A1(1,20) = B(4,3);
      A1(2,13) = B(1,3);
      A1(2,15) = B(2,3);
      A1(2,16) = B(3,3)-B(1,1);
      A1(2,19) = B(4,3);
      A1(3,6) = B(1,3);
      A1(3,9) = B(2,3);
      A1(3,10) = B(3,3)-B(1,1);
      A1(3,16) = B(4,3);
      A1(4,12) = B(1,3);
      A1(4,14) = B(2,3);
      A1(4,15) = B(3,3)-B(1,1);
      A1(4,18) = B(4,3);
      A1(5,5) = B(1,3);
      A1(5,8) = B(2,3);
      A1(5,9) = B(3,3)-B(1,1);
      A1(5,15) = B(4,3);
      A1(6,4) = B(1,3);
      A1(6,7) = B(2,3);
      A1(6,8) = B(3,3)-B(1,1);
      A1(6,14) = B(4,3);
      A1(7,11) = B(1,3);
      A1(7,12) = B(2,3);
      A1(7,13) = B(3,3)-B(1,1);
      A1(7,15) = -B(2,1);
      A1(7,16) = -B(3,1);
      A1(7,17) = B(4,3);
      A1(7,19) = -B(4,1);
      A1(8,3) = B(1,3);
      A1(8,5) = B(2,3);
      A1(8,6) = B(3,3)-B(1,1);
      A1(8,9) = -B(2,1);
      A1(8,10) = -B(3,1);
      A1(8,13) = B(4,3);
      A1(8,16) = -B(4,1);
      A1(9,2) = B(1,3);
      A1(9,4) = B(2,3);
      A1(9,5) = B(3,3)-B(1,1);
      A1(9,8) = -B(2,1);
      A1(9,9) = -B(3,1);
      A1(9,12) = B(4,3);
      A1(9,15) = -B(4,1);
      A1(10,1) = B(1,3);
      A1(10,2) = B(2,3);
      A1(10,3) = B(3,3)-B(1,1);
      A1(10,5) = -B(2,1);
      A1(10,6) = -B(3,1);
      A1(10,11) = B(4,3);
      A1(10,13) = -B(4,1);

      M0(10*p-9:10*p,:) = A0;
      M1(10*p-9:10*p,:) = A1;
   end;   

   % Reduce the 50x50 system M0 + lambda*[zeros(50,30),M1] to a 20x20
   % system R0 + lambda*R1 by QR decomposition, where
   % lambda=qt(3)/qt(4). (We could also use LU decomposition here).

   [Q,R] = qr(M0(:,1:30));
   Q1 = Q(:,31:50)';
   R0 = Q1*M0(:,31:50);
   R1 = Q1*M1;
   
   % Solve the 20x20 eigensystem R0 + lambda*R1. (We could also use
   % generalized eigensolver here for better stability, or check whether
   % R1 was better conditioned than R0 and if so use R1\R0 and 1/lambda.
   
   [cond(R0),cond(R1),cond(M0(:,1:30))]
   [rank(R0),rank(R1),rank(M0),rank(M0(:,1:30))]
   E1 = R0 \ R1;
   [U,E] = eig(E1);
   E = diag(E)';

   % Pull out roots (q,R(q),t) from eigenvectors, ignoring complex
   % solutions.

   qs = zeros(4,0);
   Rts = zeros(3,0);
   ds = zeros(N,0);
   for s = 1:20
      if imag(E(s)) == 0 
	 % The eigenvector for solution (q,t) is proportional to:
	 %
	 % [q(1)^3,      q(1)^2*q(2), q(1)^2*q(3), q(1)*q(2)^2, q(1)*q(2)*q(3),
	 %  q(1)*q(3)^2, q(2)^3,      q(2)^2*q(3), q(2)*q(3)^2, q(3)^3, 
	 %  q(1)^2,      q(1)*q(2),   q(1)*q(3),   q(2)^2,      q(2)*q(3), 
	 %  q(3)^2,      q(1),        q(2),        q(3),        1]
	 %
	 % Read off the solution from it, using the largest (best
	 % conditioned) components for stability.

	 u = U(:,s);
	 V = [[u(1:3);u(11)], ...	% q(1)^2*q
	      [u(4);u(7:8);u(14)], ...	% q(2)^2*q
	      [u(6);u(9:10);u(16)], ...	% q(3)^2*q
	      u(17:20)];		% q(4)^2*q (q(4)=1)
         z = sum(V.^2);
	 j = find(z = max(z));
         q = V(:,j(1));
	 q = q/norm(q,2);
	 R = quat_to_rot(q);
	 Rx = R*xs;
	 A = zeros(N,3);
	 for p = 1:N
	    A(p,:) = [ys(2,p)*Rx(3,p)-ys(3,p)*Rx(2,p), ...
		      ys(3,p)*Rx(1,p)-ys(1,p)*Rx(3,p), ...
		      ys(1,p)*Rx(2,p)-ys(2,p)*Rx(1,p)];
	 end;

	 [U1,S1,V1] = svd(A);
	 S1 = diag(S1)';
	 t = V1(:,3);
	 d1 = zeros(2,N);
	 for p = 1:N
	    [U2,S2,V2] = svd([ys(:,p),-Rx(:,p),t]);
	    d1(:,p) = V2(1:2,3)/V2(3,3);
	 end;
	 if sum(sum(d1<0)) > N
	    t = -t;
	    d1 = -d1;
	 end;
	 if sum(sum(d1<0)) == 0
	    qs = [qs, q];
	    Rts = [Rts, R, t];
	    ds = [ds, d1'];
	 end;
      end;
   end;
%end;
