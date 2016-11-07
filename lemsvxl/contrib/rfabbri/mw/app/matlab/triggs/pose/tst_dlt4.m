% Test routine for `4-point DLT' giving camera pose plus focal length
% from one image of n>=4 known 3D points. See dlt4init.m for details.

function tst_dlt4(n,noise,flat,refine)
   err_weight = 1e2;
   f_weight = 0;

   % Generate a random camera calibration K0 and pose [R0,t0]. We work
   % in nominally calibrated image coordinates, i.e. pixel coordinates
   % premultiplied by the inverse of some nominal calibration K, so that
   % the nominal focal length is scaled to 1.

   f0 = exp(0.5*randn(1));
   % f0 = 1;
   K0 = diag([f0,f0,1]);
   R0 = quat_to_rot([0.2*randn(3,1);1]);
   t0 = [0;0;4] + 0.3*randn(3,1);
   P0 = K0*[R0,t0];

   % Generate a random scene and its noisy projection. Noise is measured
   % in nominal focal lengths not pixels, e.g noise ~= 1e-3 might be
   % typical. The 4-point method fails for planar scenes (flat->0).

   TX = [quat_to_rot([0.1*randn(3,1);1]), 0.1*randn(3,1); zeros(1,3),1];
   X = TX * diag([1,1,flat,1]) * [randn(3,n); ones(1,n)];
   % X = diag([1,1,flat,1]) * [eye(3),ones(3,1),randn(3,n-4); ones(1,n)];
   x = P0*X;
   x = x ./ (ones(3,1)*x(3,:)) + noise*[randn(2,n);zeros(1,n)];

   out = [];
   % for err_weight = [0,1e-2,1e-1,1,1e1,1e2,1e3,1e4]
   
   % Run the method
   [Ps,Rts,fs,cond] = dlt4init(x,X,[err_weight,f_weight,1]);
   
   % Post-process the solutions (there is only ever one in this case)
   dxs = [];
   for i = 1:size(fs,2)
      P = Ps(:,4*i-3:4*i);
      R = Rts(:,4*i-3:4*i-1);
      t = Rts(:,4*i);
      f = fs(i);
      
      if refine
	 
	 % Optionally, refine the solution by nonlinear optimization.
	 % We use a full Newton method here as the error surface is
	 % highly nonlinear. I've also implemented a Gauss-Newton
	 % one, but it was too often slow and unreliable.  Full
	 % Newton is a bit better but not much, and is also quite
	 % intricate to implement. I'm still not completely happy
	 % with it, but don't know how to do better at present.  That
	 % said, and ignoring slowness, most actual failures are due
	 % to the linear solver returning a false focal length near
	 % zero, not to the nonlinear optimizer. You might get better
	 % mileage by using a known-focal-length pose method with a
	 % nominal focal length, then refining the output using the
	 % nonlinear method.
	 
	 % YFREE says which parameters to adjust from [R, t, 1/f,a,s, u0,v0]
	 % DATA transmits the measurements to the fitting routine
	 
	 yfree = [ones(3,1); ones(3,1); 1;1;0; 0;0];
	 q = rot_to_quat(R);
	 y = [q; t; 1/f;0;0; 0;0];
	 data = [yfree; n; reshape([X;x],7*n,1)];
	 
	 f0 = flops;
	 [e,y1,info] = fullnewt([],'calib_pose_hess',y,data);
	 flops-f0

	 R = quat_to_rot(y1(1:4));
	 t = y1(5:7);
	 f = 1/y1(8);
	 P = diag([1,1,1/f])*[R,t];
      end
      
      % Evaluate the relative ground-truth error of the solution
      DP = P/norm(P,1) - P0/norm(P0,1);
      DR = R'*R0 - eye(3);
      DT = (t-t0)/norm(t0);
      DF = (f-f0)/f0;
      dx = [norm(DP),norm(DR),norm(DT),abs(DF)];
      dxs = [dxs; dx];
   end;
   out = [out; dxs];
   % end
   out
%end
