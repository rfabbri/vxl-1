function [cc_best, best_inliers, cc_best_min] = rf_compute_camera_ransac(image_pts, world_pts)
% INPUTS
%   world_pts: nx4 array of 3D points in Homogeneous coordinates
%   image_pts: nx3 array of 2D points in Homogeneous coordinates
%
% OUTPUTS
%   cc_best: best projection matrix found. May be optimized or computed from
%   inliers 
%
%   cc_best_min: best projection matrix found, computed using minimal number of
%   samples (6)


npts = size(image_pts,1);

% todo make sure size of image pts and world pts are the same


tic;

d_threshold = 3;
e = .50;                % outlier probability
p = .99;
s = 6;
%N = log(1 - p)/log(1 - (1 - e)^s);
N = 3000;
samples_taken = 0;
best_res_err = Inf;
max_inliers = 0;

while samples_taken < N
  samples = randperm(npts);
  samples = samples(1:s);
  samples_taken = samples_taken + 1;

  % Compute the model
  cc_min = rf_compute_camera(image_pts(samples,:), ...
                             world_pts(samples,:));

  % Compute reprojection error for all the data
  dists = rf_reprojection_error(cc_min, ...
                                rf_non_homog_2d(image_pts), rf_non_homog_3d(world_pts));

  % Determine inliers
  inliers = find(dists < d_threshold);
  inlier_count = max(size(inliers));

  if inlier_count > 0
    res_err = mean(dists(inliers));
  else
    res_err = Inf;
  end

  if inlier_count > max_inliers | ...
     (inlier_count == max_inliers & res_err < best_res_err)

     % keep the best found so far
     
      max_inliers = inlier_count;
      best_res_err = res_err;
      best_cc = cc_min;
      best_inliers = inliers;
  end


% if mod(samples_taken,10) == 0
  fprintf( 'iterations:%d/%-4d  inliers:%d/%d  re:%f\n', samples_taken,floor(N),max_inliers,npts, best_res_err);
%  end

end

fprintf('RANSAC took %f seconds.\n', toc);


cc_best_min = cc_min

% Optional: iteratively improve fit from inliers. I've found this to be less
% robust since there may be a false negative (outlier classified as inlier) in
% the inlier set. However, if you are willing to sacrifice robustness, you can
% get more precision by this iterative process.
cc_best = cc_best_min;

