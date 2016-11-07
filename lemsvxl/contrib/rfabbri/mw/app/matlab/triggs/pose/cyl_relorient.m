% [R, t, cyls3D] = cyl_relorient(cyls1,cyls2) --- relative camera
% orientation from two calibrated images of (the sides of) nc>=2
% circular cylinders (not all aligned). 

% Input: two matrices cyls1,cyls2, one for each image, each containing
% nc pairs of image lines (represented homogeneously as 3-vectors w.r.t.
% calibrated camera coordinates). Each line pair represents the two
% sides of the image of a cylinder. The line normals must be oriented
% inwards (i.e. towards the cylinder material:
% (line)'*(point_inside_cylinder) > 0). Also, the sides should be
% ordered in the same way in each image, in the sense that the cross
% product (side1)^(side2) (which points along the axis of the cylinder
% image, i.e. towards one of the cylinder's vanishing points) should
% point in the same physical direction along the cylinder in each image.

% Output: The transformation [R,-R*t] between the two camera
% positions, and the 3D structure cyls3D of the cylinders. A point with
% coordinates x w.r.t. the 2nd camera frame has coordinates R*(x-t)
% w.r.t. the 1st. cyls3D is an array of nc columns, one per cylinder,
% containing: (1:3) the 3D direction; (4:6) a 3D point on the axis; (7)
% the radius of the cylinder. All quantities are expressed w.r.t. the
% 2nd camera frame. The overall spatial scale is arbitrary.

% Method: The two edges of an image cylinder converge at a vanishing
% point --- the cylinder's axis direction --- which is invariant
% to camera translations. The rotational part of the camera motion
% can be recovered from nc>=2 distinct vanishing points. Given R, the
% ratio of the angular radii of a cylinder in the two images gives one
% constraint on the camera translation --- sin(angular_radius) =
% (radius)/(distance) --- so the translation (and the radii) can be
% recovered linearly from nc>=2 cylinder images, up to an overall
% spatial scale. 

% We formulate the translation constraint by calculating for
% each image cylinder a camera displacement relative to a frame fixed in
% the cylinder. See the comments in cyl_pose.m for this. This constrains
% the relative motion between the two images to something of the form
% T(R,-R*t) = T(R1,-R1*r*t1) * T(Rx(a),tx(x)) * inv(T(R2,-R2*r*t2)) where
% T() denotes a rigid 3D motion, r the unknown cylinder radius, Rx(a)
% an x-axis rotation of unknown angle a, and tx(x) an x-translation of
% unknown distance x. The unknown angles a are effectively eliminated by
% solving directly for R from the vanishing points. The unknown distances
% t,r(i),x(i) can then be solved homogeneous-linearly from the
% translational part of the above constraint:
% t = r * (t1 - R * t2) + x * R1(:,1).

function [R, t, cond1, cyls3D] = cyl_relorient(cyls1,cyls2,radii)
   nc = size(cyls1,2)/2;
   cyls3D = zeros(7,nc);
   
   % Find camera poses w.r.t. frames fixed in each cylinder, up
   % to scale, x-rotation, and x-translation.
   Rts = [];
   dir1s = [];
   dir2s = [];
   for i = 1:nc
      [R1, t1] = cyl_pose(cyls1(:,2*i-1),cyls1(:,2*i));
      [R2, t2] = cyl_pose(cyls2(:,2*i-1),cyls2(:,2*i));
      Rts = [Rts, [R1,t1; R2,t2]];
      dir1s = [dir1s, R1(:,1)];
      dir2s = [dir2s, R2(:,1)];      
   end;
   % Rts

   % Solve for rotation from cylinder vanishing points.

   [R, cond1] = dirns_to_rot(dir1s,dir2s);

   if nargin<3
      % Solve for translation t, cylinder radii r, and relative
      % translations x along cylinder axes between views, by homogeneous
      % least squares in variables y = [t; r1; x1; ... rn; xn]. 

      nv = 3 + 2*nc;
      A = zeros(3*nc,nv);
      nr = ones(1,nc);
      for i = 1:nc
	 irows = 3*i-2:3*i;
	 icols = 3+2*i-1 : 3+2*i;
	 A(irows, 1:3) = eye(3);
	 R1 = Rts(1:3,4*i-3:4*i-1);
	 R2 = Rts(4:6,4*i-3:4*i-1);
	 t1 = Rts(1:3,4*i);
	 t2 = Rts(4:6,4*i);
	 dt = R' * R1 * t1 - R2 * t2;
	 nr(i) = norm(dt,2);
	 A(irows,icols) = [dt/nr(i), -R' * R1(:,1)];
	 if (nargout>2) cyls3D(4:6,i) = -R2 * t2; end;
      end;
      % A
      [U,S,V] = svd(A);
      S = diag(S)';
      y = V(:,nv);
      
      % Unpack solution and finish solving for 3D structure.
      % Guess right sign for solution by requiring radii to the +ve.
      %   y = y / norm(y(1:3),2);
      rx = reshape(y(4:nv),2,nc);
      radii = rx(1,:) ./ nr;
      if (sum(radii) < 0) y = -y; radii = -radii; end;
      t = -y(1:3);   
      if (nc>2) cond2 = S(nv)/S(nv-1); 
      else cond2 = S(size(S,2))/S(1); 
      end;
   else
      % Solve for translation t and relative translations x1..xn along
      % cylinder axes between views, using least squares and the given
      % cylinder radii.

      nv = 3 + nc;
      A = zeros(3*nc,nv);
      b = zeros(3*nc,1);
      for i = 1:nc
	 irows = 3*i-2:3*i;
	 R1 = Rts(1:3,4*i-3:4*i-1);
	 R2 = Rts(4:6,4*i-3:4*i-1);
	 t1 = Rts(1:3,4*i);
	 t2 = Rts(4:6,4*i);
	 A(irows, 1:3) = eye(3);
	 A(irows,3+i) = -R' * R1(:,1);
	 b(irows) = -(R' * R1 * t1 - R2 * t2)*radii(i);
	 if (nargout>2) cyls3D(4:6,i) = -R2 * t2; end;
      end;
      % A,b
      y = A \ b;
      t = -y(1:3);   

      cond2 = 1/cond(A);
   end

   if (nargout>3)
      cyls3D(1:3,:) = dir2s;
      cyls3D(4:6,:) = cyls3D(4:6,:) * diag(radii);
      cyls3D(7,:) = radii;
   end;
   cond1 = [cond1, cond2];
%end;
