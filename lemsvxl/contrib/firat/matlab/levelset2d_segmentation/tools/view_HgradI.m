% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/view_HgradI.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011


function view_HgradI(I, sigma, hx, hy)
	close all
	I = double(I)/255;
	[nrows, ncols] = size(I);
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	I_interp = interp2(I, gridx, gridy);

	[Gx, Gy] = gaussian_derivative_filter(sigma, hx, hy);
	Ix = imfilter(I_interp, Gx, 'replicate');
	Iy = imfilter(I_interp, Gy, 'replicate');
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, hx, hy);	
	Ixx = imfilter(I_interp, Gxx, 'replicate');
	Iyy = imfilter(I_interp, Gyy, 'replicate');
	Ixy = imfilter(I_interp, Gxy, 'replicate');	
	
	Fx = Ixx .* Ix + Ixy .* Iy;
	Fy = Ixy .* Ix + Iyy .* Iy;
	
	figure; imagesc(I); colormap gray; axis image
	hold on
	quiver(gridx, gridy, Fx, Fy)
	hold off
	magF = sqrt(Fx.^2 + Fy.^2);
	figure;imagesc(magF);colormap gray; axis image
	figure; hist(magF(:))
end
