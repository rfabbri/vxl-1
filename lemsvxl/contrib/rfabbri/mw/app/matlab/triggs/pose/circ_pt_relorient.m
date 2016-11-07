% solns = circ_pt_relorient(C2,C1,x2,x1,coplanar,side2,side1) --- Find the
% possible relative orientations of two calibrated cameras given two
% images of a circle and a single 3D point. There can be as many as 8
% solutions.

% Inputs: 3x3 symmetric matrices C2,C1 defining image conics x' * C * x
% = 0, which are the calibrated images of a 3D circle.  3-vectors x2,x1
% which are the calibrated images (directions in the camera frame) of a
% 3D point x. A flag indicating that x is known to be coplanar with the
% circle (which allows a simpler method with at most 4 solutions to be
% used). Optionally, 3-vectors side2,side1 defining which side of the
% camera the conics lie on (see comments in circ_pose.m).

% Outputs: the <=8 possible solutions for relative camera orientation
% and 3D structure, all expressed w.r.t. the frame of the first camera,
% with all distances measured in circle-radii.  Each solution occupies 3
% rows of the output matrix, and gives [ R, t, x, p, H, c1, c2 ].  R,t
% is the rotation and optical centre of camera 2 (whose projection
% matrix is thus R * [eye(3),-t]). x is the 3D position of the point
% x. p' * y = 1 is the equation of the 3D circle's plane. H is the image
% 1 to image 2 homography induced by this plane. c1,c2 are the estimated
% images of the centre of the 3D circle.

% Method: The routine circ_pose() finds possible camera poses w.r.t. the
% frame of the circle, up to a 2-fold ambiguity and a 1 parameter
% rotation. See circ_pose.m for details. We solve for the rotation using
% the images of the point x. 

% If x lies on the plane, each of its images can be mapped directly into
% the circle's frame. The rotation is then chosen to make the two images
% coincide. Their distances from the origin must also coincide, which
% allows a (fairly weak in practice) consistency check.

% If x is not known to lie in the plane of the circle, the general
% 8-solution method is used. The optical rays of x in cameras 1 and 2
% are mapped into the circle's frame. The rotation freedom sweeps ray 1
% through space, forming a 3D surface (a circular ruled quadric). The
% points where ray 2 intersects this surface are the possible rotation
% solutions. We solve algebraically for these, and then check that both
% depths are positive: there are at most 2 solutions for a given pair of
% circle-pose solutions.

function solns = circ_pt_relorient(C2,C1,x2,x1,coplanar,side2,side1)
   solns = [];
   if (nargin<7) 
      side2 = [0; 0; 1];
      if (nargin<6) side1 = side2; end;
   end;
   
   % Solve for camera pose of each image relative to circle frame (2
   % solutions each).
   
   [U11, U12] = circ_pose(C1,side1);
   [U21, U22] = circ_pose(C2,side2);
   U1 = [U11, U12];
   U2 = [U21, U22];

   % For each pair of pose solutions, solve for rotation Rx and 3D
   % structure, and eliminate any impossible solutions.

   for i1 = 1:2
      R1 = U1(:,i1*8-7:i1*8-5);
      t1 = U1(:,i1*8-4);
      d1 = R1' * x1;
      H1 = U1(:,i1*8-3:i1*8-1);
      cent1 = U1(:,i1*8);

      for i2 = 1:2
	 R2 = U2(:,i2*8-7:i2*8-5);
	 t2 = U2(:,i2*8-4);
	 d2 = R2' * x2;   
	 H2 = U2(:,i2*8-3:i2*8-1);
	 cent2 = U2(:,i2*8);

	 if (coplanar)

	    % The 3D point x is known to be on the plane of the 3D circle.
	    % The two images x1,x2 correspond to directions [xy,d(3)] in
	    % the frame used for the homographies (i.e. frontoparallel,
	    % 1 circle radius from the circle's centre), and hence to 
	    % circle-plane points xy/d(3) at distance |xy|/d(3) from the
	    % origin. We choose the rotation Rx to align the two plane points along a
	    % ray from the origin. If the pose solutions are valid, the
	    % two points should have about the same distance from the
	    % origin, but since this could be large (and hence very
	    % uncertain) for points near the horizon, we avoid
	    % ill-conditioning by comparing their angular
	    % elevation atan2(|xy|,d(3)) instead. Unfortunately, this
	    % test is quite weak in practice and could perhaps be
	    % omitted: the threshold has to be set wide to catch true
	    % solutions, while false ones often have very similar
	    % distances to the true one... 
		    
	    xy1 = d1(3) * t1(1:2) - t1(3) * d1(1:2);
	    xy2 = d2(3) * t2(1:2) - t2(3) * d2(1:2);
	    n1 = norm(xy1,2);
	    n2 = norm(xy2,2);

	    % r1 = n1 / d1(3), r2 = n2 / d2(3)
	    
	    max_dr_elevation = 0.1; % a very loose angular threshold (in radians)

	    if ( abs( n1*d2(3) - n2*d1(3) ) ...
	         < max_dr_elevation * norm([n1, d1(3)]) * norm([n2, d2(3)]))

	      dir1 = xy1 / n1;
	      dir2 = xy2 / n2;
	      c = dir1(1) * dir2(1) + dir1(2) * dir2(2);
	      s = dir1(1) * dir2(2) - dir1(2) * dir2(1); 
	      Rx = [ c,-s,0; s,c,0; 0,0,1 ];

	      R = R2 * Rx * R1';
	      t = R1 * (t1 - Rx' * t2);
	      x = R1 * ([( xy1/d1(3) + [c,s; -s,c] * xy2/d2(3) )/2; 0] - t1);
	      p = -R1(:,3) / t1(3);
	      solns = [ solns; R, t, x, p, H2 * Rx * inv(H1), cent1, cent2 ];
	   
	   end;
	 
	 else
	    % General case, 3D point x is not known to be on plane of
	    % circle.  Find rotations Rx in nominal frame for which
	    % optical ray of x1 intersects rotated optical ray of
	    % x2. The difference of origins (t2 - Rx * t1) and the two
	    % directions d2, Rx * d1 must be coplanar, hence their 3x3
	    % determinant vanishes. Given cos^2+sin^2=1, the determinant
	    % is linear in cos(rx), sin(rx). Start by working out their 
	    % coefficients...

	    u = (t1(3)-t2(3)) * (d1(1) * d2(2) - d1(2) * d2(1)) ...
		- d1(3) * (t1(1) * d2(2) - t1(2) * d2(1)) ...
		- d2(3) * (t2(1) * d1(2) - t2(2) * d1(1));

	    v = (t2(3) - t1(3)) * (d1(1) * d2(1) + d1(2) * d2(2)) ...
		+ d1(3) * (t1(1) * d2(1) + t1(2) * d2(2)) ...
		- d2(3) * (t2(1) * d1(1) + t2(2) * d1(2));

	    w = -(d1(3) * (t2(1) * d2(2) - t2(2) * d2(1)) ...
		 + d2(3) * (t1(1) * d1(2) - t1(2) * d1(1)));
	     
	     % Now solve u * cos(rx) + v * sin(rx) = w for
	     % cos(rx),sin(rx) (2 solutions). Reject poses for which
	     % there is no real solution, but (optionally) try to
	     % heuristically patch up ones which nearly have a solution,
	     % since noise can sometimes perturb valid poses with nearby
	     % false solutions into the complex.

	    u2v2 = u^2 + v^2;
	    disc = u2v2 - w^2;
	    if (disc > -0.1 * (u2v2 + w^2)) 
	       if (disc<0)
		  warning('perturbing a nearly real Rx solution to be real ...');
		  w = sign(w) * sqrt(u2v2);
		  disc = 0;
	       end;
	       disc = sqrt(disc);
	       s1 = (v * w - u * disc) / u2v2;
	       s2 = (v * w + u * disc) / u2v2;
	       c1 = (u * w + v * disc) / u2v2;
	       c2 = (u * w - v * disc) / u2v2;
	       
	       % [c1 * u + s1 * v - w, c2 * u + s2 * v - w]
	    
	       % Apply each rotation Rx, back substitute to find 3D point
	       % x = t1 + lambda(1) * d1 = Rx' * (t2 + lambda(2) * d2)
	       % by solving [d1, -Rx' * d2] * lambda = (Rx' * t2 - t1) in
	       % least squares (the residual should be exactly 0), and 
	       % prune solutions x that are behind either camera.

	       ss = [s1,s2];
	       cc = [c1,c2];
	       for j = 1:2
		  % solnum = 1 + size(solns,1)/3
		  s = ss(j);
		  c = cc(j);
		  Rx = [ c,-s,0; s,c,0; 0,0,1 ];
		  rt2 = Rx' * t2;
		  rd2 = Rx' * d2;
		  % dd = (rt2 - t1)' * cross(d1, rd2)

		  lambda = [d1, -rd2] \ (rt2 - t1);
		  x = t1 + lambda(1) * d1;
		  % x - Rx' * (t2 + lambda(2) * d2)
		  if (all(lambda >= 0)) 
		     R = R2 * Rx * R1';
		     t = R1 * (t1 - rt2);
		     x = R1 * (x - t1);
		     p = -R1(:,3) / t1(3);	       
		     solns = [ solns; R, t, x, p, H2 * Rx * inv(H1), ...
			    cent1, cent2 ];
		  end;
	       end;
	    end;
	 end;
      end;
   end;
%end;
