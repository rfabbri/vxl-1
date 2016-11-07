function errors = rf_reprojection_error(P, image_pts, world_pts)

  % Compute the reprojection error
  % TODO: optimize using matrix notation
  n_correspondences = size(image_pts, 1);
  errors = zeros(n_correspondences,1);
  for i=1:n_correspondences
    p_proj = P*[world_pts(i,:) 1]';
    p_proj = p_proj/p_proj(3);
    errors(i) = norm(p_proj(1:2) - image_pts(i,:)');
  end
