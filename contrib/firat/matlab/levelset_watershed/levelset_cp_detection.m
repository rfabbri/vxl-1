% This is levelset_cp_detection.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 15, 2013

function levelset_cp_detection(I, sigma)
	addpath ~/lemsvxl/bin/mex
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
	addpath ~/lemsvxl/src/contrib/firat/matlab/topographical
	if sigma > 0
		L = round(7*sigma);
		if mod(L,2) == 0
			L = L + 1;
		end	
		H = fspecial('gaussian', [L L], sigma);
		phi = imfilter(I, H, 'replicate');	
	else
		phi = I;	
	end
	J = phi;
	[H,W] = size(phi);
	
	maxv = max(phi(:));
	minv = min(phi(:));
	
	phi = phi - minv;
	c1 = (maxv - minv)/500;
	currN = 0;
	while any(any(phi > 0))
		imagesc(J);axis image; colormap gray; hold on
		ctrs = plot_contours(phi, 0);
		[L,N] = bwlabel(phi < 0);
		if N != currN
		hold off
		drawnow
		phi = phi - c1;
	end
end

function ctrs = plot_contours(phi, mode)
	if mode == 0
		ctrs = contour(phi, [0, 0], 'r');
	else
		ctrs = compute_zero_level_curve(phi, 1, 1);
		j = 1;
		if ~isempty(ctrs)
			while j <= size(ctrs,2)
				con = ctrs(:, j+1:j+ctrs(2,j))';
				plot(con([1:end, 1],1), con([1:end,1],2), 'r');
				j = j + ctrs(2,j) + 1;
			end
		end
	end
end
