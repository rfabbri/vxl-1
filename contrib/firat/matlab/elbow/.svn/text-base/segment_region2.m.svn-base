% This is /lemsvxl/contrib/firat/matlab/elbow/segment_region2.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 28, 2011

function segment_region2(V, init_slice, t)
	S = thresholding(V(:,:,init_slice), t);
	figure; imagesc(V(:,:,init_slice)); colormap gray; axis image; axis off;
	hold on;
	contour(S, [0,0], 'r', 'LineWidth', 3);
	hold off;
	for i = init_slice+1:init_slice+20
		se = strel('disk', 3);
		S0 = imdilate(S, se);
		I = V(:,:,i);
		I(~S) = 0;
		S = levelset(I, S0);
		figure; imagesc(V(:,:,i)); colormap gray; axis image; axis off;
		hold on;
		contour(S, [0,0], 'r', 'LineWidth', 3);
		hold off;
	end
end

