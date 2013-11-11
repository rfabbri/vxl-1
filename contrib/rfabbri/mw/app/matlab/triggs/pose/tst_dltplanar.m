% Test routine for dltplanar() - camera pose + calibration from one or
% more images of known 3D planes. See the comments in dltplanar.m for
% the options.

function tst_dltplanar(n,nplanes,Kfree,noise,flat)
   
   % nplanes = 3;	  % how many planes to fit
   % Kfree = [1,1,1,1,1]; % which parameters to estimate from [f,s,a,u0,v0]
   % Kfree = [1,0,0,0,0];
   % noise = 1e-4;	  % image noise (in units of nominal focal lengths)
   % flat = 1e-2;	  % scene non-flatness, for testing response to non-planarity
   % n = 8;	          % number of points per plane n>=4

   % Nominal calibration for pre-normalization.  Here we generate
   % already-normalized image points f=1, a=1, s=u0=v0=0
   Kref = [1,0,1,0,0];

   % Generate n random 3D points on each plane (or near it, if flat!=0).
   % planeno contains the indices of the first and last input point on
   % each plane.

   nt = n*nplanes;
   X0 = diag([1,1,flat,1]) * [randn(3,nt); ones(1,nt)];
   X = X0([1,2,4],:);
   x0 = zeros(3,nt);
   planeno = [[1:n:nt];[n:n:nt]]';

   % Generate a random calibration and random poses for the planes, and
   % project their 3D points. R00 biases the generated planes away from
   % fronto-parallelism on average, as focal length is difficult to
   % recover near there.

   f0 = exp(0.3*randn(1));
   K0 = diag([f0,f0,1]);
   Rt0 = zeros(3,4*nplanes);
   H0 = zeros(3,3*nplanes);
   R00 = quat_to_rot([0.2*ones(1,3),1]);
   t0 = [0;0;4];
   for p=1:nplanes
      R = quat_to_rot([0.2*randn(3,1); 1]) * R00;
      t = t0 + randn(3,1);
      Rt0(:,4*p-3:4*p) = [R,t];
      H0(:,3*p-2:3*p) = K0 * [R(:,1:2),t];
      x0(:,(p-1)*n+1:p*n) = K0 * [R,t] * X0(:,(p-1)*n+1:p*n);
   end;
   x = x0 ./ (ones(3,1)*x0(3,:)) + noise*[randn(2,nt);zeros(1,nt)];

   % Run the pose + calibration method

   [K1,H1,Rt1,cond] = dltplanar(x,X,planeno,[Kref;Kfree]);

   % Accumulate random error statistics 
   err = zeros(3,nplanes);
   for p = 1:nplanes
      h0 = H0(:,3*p-2:3*p);
      h1 = H1(:,3*p-2:3*p);
      rt0 = Rt0(:,4*p-3:4*p);
      rt1 = Rt1(:,4*p-3:4*p);
      err(:,p) = [norm(h1/det(h1)^(1/3) - h0/det(h0)^(1/3));...
	     norm(rt1(:,1:3)-rt0(:,1:3));...
	     norm(rt1(:,4)-rt0(:,4))/norm(rt0(:,4))];
   end;
   if (nplanes>1)
      norm(K1-K0),err %,cond
   else
      [norm(K1-K0),err'] %,cond
   end;
end;
