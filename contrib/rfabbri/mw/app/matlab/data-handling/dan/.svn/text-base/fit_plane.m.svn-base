function [plane, eig_ratio] = fit_plane(points,weights)


if (nargin == 1)
    m = mean(points,2);
else
    W = diag(weights);
    m = sum((points*W),2)/sum(weights);
end
%scale = (max(points') - min(points'))';
points_norm = (points - m*ones(1,size(points,2))); %./ (scale*ones(1,size(points,2)));
if (nargin == 1)
    scatter_matrix = points_norm*points_norm';
else
    scatter_matrix = points_norm*W*points_norm';
end
[U,S,V] = svd(scatter_matrix);
plane = [V(:,3); -V(:,3)'*m];

eig_ratio = S(3,3)/S(2,2);

return