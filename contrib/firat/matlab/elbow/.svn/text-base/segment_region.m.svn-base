% This is /lemsvxl/contrib/firat/matlab/elbow/segment_region.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 24, 2011

function segment_region(V, init_slice, t)
	S = thresholding_all_bones(V(:,:,init_slice), t, 0);
	figure; imagesc(V(:,:,init_slice)); colormap gray; axis image; axis off;
	hold on;
	contour(S, [0,0], 'r', 'LineWidth', 3);
	hold off;
	for i = init_slice+1:init_slice+20
		se = strel('disk', 5);
		S0 = imdilate(S, se);
		S = levelset(V(:,:,i), S0);
		figure; imagesc(V(:,:,i)); colormap gray; axis image; axis off;
		hold on;
		contour(S, [0,0], 'r', 'LineWidth', 3);
		hold off;
	end
end



