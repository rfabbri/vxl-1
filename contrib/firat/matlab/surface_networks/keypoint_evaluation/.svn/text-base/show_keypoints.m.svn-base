% This is show_keypoints.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2012

function show_keypoints(I, points, th)
	points = points(points(:,3) >= th, :);
	figure; imagesc(I); colormap gray; axis image; axis off
	hold on
	plot(points(:,1), points(:,2), 'r.');
	hold off
end
