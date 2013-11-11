% Generate random calibrated images of plane with a camera rotation of
% around ROT degrees and translation of around TRANS starting DIST from
% plane, and test pose from circle routine on them for a circle of unit
% radius. Heuristic noise of NOISE radians is added to the image conic.
% (Depending on the geometry, anything much more than about 1e-3 radians
% begins to cause occasional failures).

function tst_circ_pose(ntrials,rot,trans,dist,noise)
   rot = pi*rot/180;
   C0 = diag([1,1,-1]);
   
   for i = 1:ntrials
      % Generate random camera motion, corresponding 3D circle plane to
      % image plane homography, and noisy image of 3D circle. The motion
      % is w.r.t a 3D camera frame looking straight into the plane from
      % the circle centre. t is the optical centre and R * [eye(3),-t]
      % the camera projection matrix. The camera must move back to see
      % the plane and look towards plane, so t(3)<0 and R(3,3)>0 for
      % realistic camera positions. H is the homography between an image
      % 1 unit above plane (z=-1) and the new camera position R,t.

      R = quat_to_rot([rot * randn(3,1); 1]);
      t = trans * randn(3,1) + [0; 0; -dist];
      if (t(3)>0) t = -t; end;
      side = R(:,3);
      
      H = R * [ 1,0,-t(1); 0,1,-t(2); 0,0,-t(3) ];
      IH = inv(H);

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
      
      % Find two possible pose solutions from image circle
      
      [U1,U2] = circ_pose(C,side);
      R1 = U1(:,1:3); 
      t1 = U1(:,4); 
      H1 = U1(:,5:7); 
      c1 = U1(:,8);
      R2 = U2(:,1:3);      
      t2 = U2(:,4);
      H2 = U2(:,5:7);
      c2 = U2(:,8);

      % Error measures: offset of calculated from true image 
      % circle centre, re-expressed in nominal frame, 
      % camera rotation error error (ignoring Rz ambiguity),
      % relative translation error (given correct spatial scale)
      % difference of nominalized C from nominal circle
      
      DR1 = R' * R1;
      DR2 = R' * R2;
      dc1 = IH * c1;
      dc2 = IH * c2;
      C1 = H1' * C * H1; 
      C2 = H2' * C * H2;
      C1 = C1 / abs(det(C1))^(1/3);
      C2 = C2 / abs(det(C2))^(1/3);
      errs = [norm(dc1(1:2)) / abs(dc1(3)), ...
	     norm(DR1(3,1:2)) + norm(DR1(1:2,3)),...
	     norm(DR1 * t1 - t)/dist,...       
	     norm(C1 - diag([1,1,-1])); ...
	     norm(dc2(1:2)) / abs(dc2(3)),...
	     norm(DR2(3,1:2)) + norm(DR2(1:2,3)),...
	     norm(DR2 * t2 - t)/dist,...
	     norm(C2 - diag([1,1,-1]))]
      % if (sqrt(min(sumsq(errs'))) > (1e-8 + 100 * noise)) errs end;
%       if ((errs(1,3) > .5) & (errs(2,3) > .5))
% 	 R
% 	 [R1, DR1, H1]
% 	 [R2, DR2, H2]
% 	 [t, DR1 * t1, DR2 * t2]
% 	 c = -R*t
% 	 % [sign(dc1(3)),sign(dc2(3))]
%       end;
   end;
%end;
