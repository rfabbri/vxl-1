% This is /lemsvxl/contrib/firat/matlab/elbow/segment_region3.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 29, 2011

function segment_region3(V, init_slice, t)
	[S, mu, sigma, x, y] = thresholding(V(:,:,init_slice), t);
	figure; imagesc(V(:,:,init_slice)); colormap gray; axis image; axis off;
	hold on;
	contour(S, [0,0], 'r', 'LineWidth', 3);
	hold off;
	S_t = S;
	for i = init_slice+1:init_slice+20
		%se = strel('disk', 2);
		%S0 = imdilate(S, se);
		%S = levelset(V(:,:,i), S0);
		S_t = thresholding(V(:,:,i), t, mu, sigma, x, y);
		figure; 
		imagesc(V(:,:,i)); colormap gray; axis image; axis off;
		hold on;
		%contour(S, [0,0], 'r', 'LineWidth', 3);
		contour(S_t, [0,0], 'g--', 'LineWidth', 2);
		pause(.1)
		hold off;
		
	end
end
