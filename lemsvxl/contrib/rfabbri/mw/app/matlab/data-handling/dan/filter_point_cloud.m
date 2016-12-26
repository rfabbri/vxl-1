function pts_in = filter_point_cloud(pts)

% filter outliers
centroid = mean(pts,2);
sigma_x = std(pts(1,:));
sigma_y = std(pts(2,:));
sigma_z = std(pts(3,:));
npts = size(pts,2);
dists = abs(pts - centroid*ones(1,npts));
good_x = dists(1,:) < 4.5*sigma_x;
good_y = dists(2,:) < 4.5*sigma_y;
good_z = dists(3,:) < 4.5*sigma_z;
good = good_x & good_y & good_z;
pts_in = pts(:,good);


return