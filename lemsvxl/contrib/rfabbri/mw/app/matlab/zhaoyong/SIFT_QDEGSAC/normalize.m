function [newpts, T] = normalize( pts )
% returns a transform for column vectors to normalize the points 

centroid = sum(pts(:,1:2)) / size(pts,1);
diff = pts(:,1:2) - repmat(centroid,size(pts,1),1);
avgdist = sum(sqrt(diff(:,1).^2 + diff( :,2).^2)) / size(pts,1);
scale = sqrt(2) / avgdist;
T = diag([scale scale 1]) * [eye(3,2) [-centroid 1]'];
newpts = pts;
newpts(:,3) = 1;
newpts = newpts * T';
newpts = newpts ./ repmat( newpts(:,3), 1, 3);
newpts(:,3) = [];
