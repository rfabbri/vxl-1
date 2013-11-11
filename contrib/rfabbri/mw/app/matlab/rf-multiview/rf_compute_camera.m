function camera = rf_compute_camera(image_pts, world_pts)
% Given homogeneous 2D points and 3D points, return camera.
%
% INPUTS
%   world_pts: nx4 array of 3D points in Homogeneous coordinates
%   image_pts: nx3 array of 2D points in Homogeneous coordinates
%
% OUTPUTS
%   camera matrix P computed by direct linear transformation 
%
% AUTHOR
%   Ricardo Fabbri 
%
% Adapted from vxl.sf.net

num_correspondences = size(world_pts, 1);

if (num_correspondences < 6)
  error('There must be at least 6 correspondences');
end


if (num_correspondences ~= size(image_pts,1))
  error('The number of world points should be the same as the number of image points');
end

% Form the solution matrix
S = zeros(2*num_correspondences, 12);

for i=1:num_correspondences
   i1 = 2*i-1;
   i2 = 2*i;

   S(i1,1)    = -image_pts(i,3)*world_pts(i,1);
   S(i1,2)    = -image_pts(i,3)*world_pts(i,2);
   S(i1,3)    = -image_pts(i,3)*world_pts(i,3);
   S(i1,4)    = -image_pts(i,3)*world_pts(i,4);
   S(i1,9)    =  image_pts(i,1)*world_pts(i,1);
   S(i1,10)   =  image_pts(i,1)*world_pts(i,2);
   S(i1,11)   =  image_pts(i,1)*world_pts(i,3);
   S(i1,12)   =  image_pts(i,1)*world_pts(i,4);
   S(i2,5)    = -image_pts(i,3)*world_pts(i,1);
   S(i2,6)    = -image_pts(i,3)*world_pts(i,2);
   S(i2,7)    = -image_pts(i,3)*world_pts(i,3);
   S(i2,8)    = -image_pts(i,3)*world_pts(i,4);
   S(i2,9)    =  image_pts(i,2)*world_pts(i,1);
   S(i2,10)   =  image_pts(i,2)*world_pts(i,2);
   S(i2,11)   =  image_pts(i,2)*world_pts(i,3);
   S(i2,12)   =  image_pts(i,2)*world_pts(i,4);
end

[U,W,V] = svd(S);
c = V(:,end);

camera = zeros(3,4);

camera(1,1)=c(1); camera(1,2)=c(2); camera(1,3)=c(3); camera(1,4)=c(4);
camera(2,1)=c(5); camera(2,2)=c(6); camera(2,3)=c(7); camera(2,4)=c(8);
camera(3,1)=c(9); camera(3,2)=c(10); camera(3,3)=c(11); camera(3,4)=c(12);


