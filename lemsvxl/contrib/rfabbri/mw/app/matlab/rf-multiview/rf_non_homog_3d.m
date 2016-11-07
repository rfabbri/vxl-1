function homg_pts3d = rf_non_homog_3d(pts3d);
% converts a nx4 vector of 3D points pts3d(i,:) == [x y z w] to an nx3 vector
% homg_pts3d(i,:) == (x/w, y/w, z/w)

  homg_pts3d = pts3d(:,1:3) ./ [pts3d(:,4) pts3d(:,4) pts3d(:,4)];
