% [Rt,cond] = Rt_from_pts_nl(Rt0,x1,x2)
%
% Nonlinear adjustment of relative orientation R,t between two
% calibrated cameras, to minimize heuristic violation |x2'*E21*x1|^2 of
% epipolar constraint (essential matrix E21=R*mcross(t)). This is not
% a statistically correct error measure so the results aren't optimal,
% but its linearity allows us to `reduce' the data to a 9x9 scatter
% matrix for a fast, stable iteration. It hopefully gets us fairly close
% to the optimal estimate with minimal effort, so that any following
% bundle adjustment will converge in fewer iterations.

function [Rt,cond1] = Rt_from_pts_nl(Rt0,x1,x2)
   n = size(x1,2);
   if (n<5) error('too few points for unique solution: %d/5',n); end;
   R = Rt0(:,1:3);
   t = Rt0(:,4);
   w1 = mcross([1;0;0]);
   w2 = mcross([0;1;0]);
   w3 = mcross([0;0;1]);

   % Compute the usual F-matrix scatter matrix with rows x1p*x2p', and
   % reduce it to 9x9 triangular reduced data matrix A. Linear error
   % computations on the x's reduce to computations with A.
   xx = zeros(n,9);
   for p = 1:n
      xx(p,:) = kron(x1(:,p)',x2(:,p)');
   end;
   [Q,A] = qr(xx,0);
   
   % Nonlinear Gauss-Newton minimization of `linear' error
   % sum(|x2(p)'*E21*x1(p)|^2) = |A.E|^2, where E=vector of 9 components
   % of E21. The local 5 parameter parametrization is E21 =
   % R*(1+mcross(dr))*mcross(t+dt) where dr = 3-vector representing
   % infinitessimal rotation and dt = 3-vector infinitessimal
   % translation/epipole change. Since |t|=1, dt is constrained to be
   % orthogonal to the current t, by projecting it along 2 directions
   % orthogonal to t (found by QR decomposition of t). At each iteration
   % dr is folded into the current rotation R by
   % dr->quaternion->rotation matrix, and t->normalized t+dt. (dr,dt)
   % are found by classical linear 9x5 least squares (MATLAB built-in,
   % but effectively QR decomposition).

   for iter = 1:50
      [tn,T] = qr(t);			% tn(2:3) = normal directions to t
      tx = mcross(t);
      E = reshape(R*tx,9,1);
      E1 = R*w1; E2 = R*w2; E3 = R*w3;
      dEdr = reshape([E1*tx,E2*tx,E3*tx],9,3);
      dEdt = reshape([E1,E2,E3],9,3);
      dEdu = [dEdr,dEdt*tn(:,2),dEdt*tn(:,3)];
      du = -(A*dEdu) \ (A*E);		% solve 9x5 linear least squares for du
      dr = du(1:3);
      dt = du(4)*tn(:,2)+du(5)*tn(:,3);
      if (norm(dt)>0.3)			% limit large steps
%	 warning('limiting large dt: %f',norm(dt));
	 dt = 0.3/norm(dt)*dt; 
      end; 
      if (norm(dr)>0.3) 
%	 warning('limiting large dR: %f',norm(dr));
	 dr = 0.3/norm(dr)*dr; 
      end;
      R = R*quat_to_rot([dr/2;1]);	% quaternion based R update
      t = (t+dt)/norm(t+dt);
      [iter,norm(dr),norm(dt)]
      if (norm(du)<1e-6) break; end;
   end;
   Rt = [R,t];
   if (nargout>1) cond1 = [1/cond(A*dEdu),iter]; end;
%end;
