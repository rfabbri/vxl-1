% Test routine for relative orientation from a circle and a point.
% Generate random calibrated images of plane with a camera rotation of
% around ROT degrees and translation of around TRANS starting DIST from
% plane, and run orientation on them for a circle of unit radius and a
% random 3D point (which will be coplanar with the circle if COPLANAR is
% nonzero). Heuristic noise of NOISE radians is added to the
% images. 

% Noise sensitivity: The method is fairly sensitive but may be usable
% with large-ish circles.  The relative error is *at best* of order
% (noise)*(camera distance)/(radius of circle) where noise is the
% feature extraction error in radians (c.f. point based orientation has
% similar error with radius replaced by point separation). It can be
% worse, depending on the geometry. Anything much more than about 1e-3
% radians noise begins to cause occasional failures even for fairly
% strong geometries.

function tst_circ_pt_relor(ntrials,rot,trans,dist,noise,coplanar)
   rot = pi*rot/180;
   
   for i = 1:ntrials
      % Generate two random camera motions, corresponding 3D circle plane to
      % image plane homographies, and noisy images of 3D circle. The motion
      % is w.r.t a 3D camera frame looking straight into the plane from
      % the circle centre. t is the optical centre and R * [eye(3),-t]
      % the camera projection matrix. The camera must move back to see
      % the plane and look towards plane, so t(3)<0 and R(3,3)>0 for
      % realistic camera positions. H is the homography between an image
      % 1 unit above plane (z=-1) and the new camera position R,t.

      C0 = diag([1,1,-1]);
      x0 = trans * randn(3,1);
      if (coplanar) x0(3) = 0; end;
      
      pose = [];
      imdata = [];
      for im = 1:2
	 R = quat_to_rot([rot * randn(3,1); 1]);
	 t = trans * randn(3,1) + [0; 0; -dist];
	 if (t(3) > 0) t = -t; end;
	 side = R(:,3);
	 H = R * [ 1,0,-t(1); 0,1,-t(2); 0,0,-t(3) ];
	 IH = inv(H);
	 x = R * (x0 - t);

	 % Noise model for conic: a change dC in C produces to first
	 % order a change dx given by
	 % 0 = (x+dx)'*(C+dC)*(x+dx) = (x'*dC*x + 2*x'*C*dx) 
	 % Hence for image noise |dx|, |dC| ~= 2*|C*x|*|dx|/|x|^2.
	 % For an image conic of size r radians, |C*x| ~ r, hence 
	 % |dC| ~= 2 * r * noise.

	 C = IH' * C0 * IH;
	 C = C / norm(C);
	 dC = noise * randn(3) * 1/norm(t);
	 C = C + (dC + dC');

	 pose = [ pose; R,t,H ];
	 imdata = [ imdata; C,x,side ];
      end;

      % Find possible pose solutions from image circle and point

      I1 = imdata(1:3,:);
      I2 = imdata(4:6,:);
      solns = circ_pt_relorient(I2(:,1:3),I1(:,1:3),I2(:,4),I1(:,4),...
	  coplanar,I2(:,5),I1(:,5));
      nsolns = size(solns,1)/3

      % Ground truth: R21,t21 = rotation,centre of camera 2 in frame of
      % camera 1, H21 = image 1 to 2 homography based on 3D circle's plane.
      
      R1 = pose(1:3,1:3); t1 = pose(1:3,4); H1 = pose(1:3,5:7);
      R2 = pose(4:6,1:3); t2 = pose(4:6,4); H2 = pose(4:6,5:7);
      R21 = R2 * R1';
      t21 = R1 * (t1 - t2);
      H21 = H2 * inv(H1);

      % Unpack solutions and evaluate residual error for each one
      % w.r.t. ground truth.  The values are relative to camera frame 1:
      % R,t = pose of camera 2, x = 3D position of point x, H = image 1
      % to image 2 homography for points on 3D plane of circle, c1,c2 =
      % estimated images of circle centres.
      errs = [];
      for s = 1:nsolns
	 sol = solns(3*s-2:3*s,:);
	 R = sol(:,1:3);
	 t = sol(:,4);
	 x = sol(:,5);
	 p = sol(:,6);	 
	 H = sol(:,7:9);
	 c1 = sol(:,10);
	 c2 = sol(:,11);

	 err = [ norm(R21' * R - eye(3)), ...
		norm(t - t21), norm(x - R1 * (x0-t1)), ...
		norm(p + R1(:,3) / t1(3)), ...
		norm(H * inv(H21) - eye(3)), ...
		norm( c1 - H1(:,3)/norm(H1(:,3)) ), ...
		norm( c2 - H2(:,3)/norm(H2(:,3)) ) ];
	 errs = [errs; err];
	 if (0)
	    [ R21' * R, t - t21, ...
		   (R1' * x + t1) - x0, ...
		   p + R1(:,3) / t1(3), ...
		   H  * inv(H21), ...
		   c1 - H1(:,3)/norm(H1(:,3)), ...
		   c2 - H2(:,3)/norm(H2(:,3)) ]
	 end;
      end;
      errs
   end;
%end;
