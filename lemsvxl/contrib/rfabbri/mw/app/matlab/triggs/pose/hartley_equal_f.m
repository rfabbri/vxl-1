% [f,cond] = hartley_equal_f(F,e,pp0,pp1)
% Given a fundamental matrix and one epipole derived from cameras with
% zero skew, equal aspect ratio, known principal points pp0,pp1 and
% equal but unknown focal length, find the focal length f using an
% adaption of Hartley's Kruppa equation based method for unequal f's.
% Usually, the routine returns a single focal length, but in rare cases
% (notably for certain geometries near the corresponding-principal-point
% singularity of the unequal f method) it may return two possible
% solutions in f. However one is usually unrealistically small and can
% be discarded.

function [f,cond] = hartley_equal_f(F10,e10,pp0,pp1)
   if (nargin<4) 
      if (nargin<3) pp0=[0;0;1]; end; 
      pp1=pp0;
   end;
   me = mcross(e10);
   Fp = F10' * pp1;
   mep = me * pp0;
   FIF = F10' * diag([1,1,0]) * F10;
   FpF = Fp * Fp';
   eIe = me * diag([1,1,0]) * me';
   epe = mep * mep';
   A = zeros(6,4);
   for i=1:3
      for j = 1:i
	 A(i*(i-1)/2+j,:) = [FIF(i,j), FpF(i,j), eIe(i,j), epe(i,j)];
      end;
   end;
   % reduce the number of equations
   [U,S,V] = svd(A);
   B = S*V';
   % Find 2x2 minors of (image 0 column)*u +(image 1 column)
   % in B2*[u^2; u; 1], eliminate the u^2 term and solve for u
   B2 = zeros(6,3);
   for i=2:4
      for j=1:i-1
	 B2((i-1)*(i-2)/2+j,:) = ... 
	     [  B(i,1)*B(j,2)-B(i,2)*B(j,1), ...
		B(i,1)*B(j,4)-B(i,4)*B(j,1) + (B(i,3)*B(j,2)-B(i,2)*B(j,3)), ...
		B(i,3)*B(j,4)-B(i,4)*B(j,3) ];
      end;
   end;
   [Q,R] = qr(B2,0);

   % If we are too close to corresponding principal point ideal
   % or there is too much residual noise, it is unstable to eliminate the
   % quadratic term, so try both solutions.

   if (norm(R(2,:),1) > max(1e-4*norm(R(1,:),1), 2e2*norm(R(3,:),1)))
      u = [-R(2,3)/R(2,2)];
   else
      u = solve_quadratic(R(1,:));
      if (imag(u(1))) u = []; end;
   end;
   f=[]; cond=[];
   for uu=u
      % Backsub u into f equations and solve, reject any outrageous f's
      C = [B(:,1)*uu + B(:,3), B(:,2)*uu + B(:,4)];
      [U1,S1,V1] = svd(C,0);
      ff = V1(1,2)/V1(2,2);
      if (ff>1e-12 & ff<1e12)
	 f = [f, sqrt(ff)];
	 cond = [cond, [S(4,4)/S(3,3),S(3,3)/S(1,1),R(3,3)/R(2,2),S1(2,2)/S1(1,1),uu]'];
      end;
   end;
%end;
