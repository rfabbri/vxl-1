function homg_pts2d = rf_non_homog_2d(pts2d);
% converts a nx3 vector of 2D points pts2d(i,:) == [x y w] to an nx2 vector
% homg_pts2d(i,:) == (x/w, y/w)

  homg_pts2d = pts2d(:,1:2) ./ [pts2d(:,3) pts2d(:,3)];
