% [plane, ninliers] = fit_plane_ransac(points)
function [plane, inliers] = fit_plane_ransac(points)

nchoose = 6;
inlier_dist = 0.1;
max_its = 200;
npts = size(points,2);
best_inliers = [];
for i=1:max_its
    r = randperm(npts);
    % fit plane to random selection of points
    plane = fit_plane(points(:,r(1:nchoose)));
    %plane = fit_plane(points(:,randi([1 npts],[1 nchoose])));
    % determine inliners
    dists = plane'*[points; ones(1,size(points,2))];
    inliers = dists.*dists < inlier_dist*inlier_dist;
    if (sum(inliers) > sum(best_inliers))
        best_inliers = inliers;
    end
end

inlier_pts = points(:,best_inliers);
plane = fit_plane(inlier_pts);
inliers = best_inliers;

return
    
    
    
    
    