% [R,t] = cyl_pose(L) -- given a calibrated image of a 3D circular cylinder,
% find the recoverable part of the camera pose relative to the cylinder.

% Input: two image lines e1,e2 representing the edges of the cylinder in
% calibrated image coordinates. The lines are e'*x = 0. Their normals
% should point inwards, i.e. e1'*x > 0 and e2'*x > 0 for images of
% points x inside the cylinder. If there are several images of the
% cylinder, `corresponding' edges should be given the same label e1
% vs. e2, e.g. the clockwise image rotation of normal e1 should always
% point in the same physical `direction of flow' along the cylinder.

% Output: a rotation and translation R,t taking the camera from a
% nominal 3D pose w.r.t. the cylinder to the image pose. The image
% projection matrix is R*(I|-t), so t is the optical centre in nominal
% coordinates. The cylinder's axis is defined to be the +x-axis in the
% nominal coordinates, and its radius is defined to be 1.  The nominal
% frame is only defined up to an arbitrary x translation and x-axis
% rotation, i.e. the recovered R,t are ambiguous up to R -> R * Rx(a),
% t -> t+[x;0;0], for arbitrary a,x. R is chosen so that t = [0; 0; -z].

% Method: e1,e2 directly give the image of the cylinder axis in line
% (axis) and parametric (centre+lambda*dirn) form, and hence R.
% Un-rotating the camera gives the normalized cylinder e1'*R, e2'*R =
% [0; +-cos(a); sin(a)] of visual radius sin(a)=r/z, and hence the
% optical centre position -z/r = 1/sin(a). For very narrow cylinders
% e1~=-e2 and centre, dirn and z/r>>1 are sensitive to noise in e1,e2.

function [R,t] = cyl_pose(e1,e2)
   e1 = e1/norm(e1,2);
   e2 = e2/norm(e2,2);  
   axis = e1-e2;
   centre = e1+e2;
   dirn = vcross(axis,centre);
   R = [ dirn/norm(dirn,2), axis/norm(axis,2), centre/norm(centre,2) ];
   t = [ 0; 0; -2/norm(centre,2) ];
%end;