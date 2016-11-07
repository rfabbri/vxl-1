% This is cond47.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 12, 2012

function cond47(I, sigma)
	addpath ../levelset2d_segmentation/tools
	[Gx, Gy] = gaussian_derivative_filter(sigma,1,1);
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	Ixx = imfilter(I, Gxx, 'replicate', 'conv');
	Ixy = imfilter(I, Gxy, 'replicate', 'conv');
	Iyy = imfilter(I, Gyy, 'replicate', 'conv');
	c47 = 2*Ix.*Ixy + Iy.*(Iyy-Ixx) + sign(Ixx + Iyy).*Iy.*sqrt((Ixx-Iyy).^2 + 4*Ixy.^2);
	[S, mins, maxs, saddles] = get_min_max_saddle(I, sigma);	
	close all;
	BW1 = double(c47 < -20);
	BW2 = double(c47 > 20);
	
	L1 = bwlabel(BW1,4);
	L2 = bwlabel(BW2,4);
	L2(L2 ~= 0) = L2(L2 ~= 0) + max(L1(:));
	L = L1+L2;
	figure; imagesc(L); axis image; axis off; colormap gray 
	hold on;	
	plot(mins(:,1), mins(:,2), 'rx','MarkerSize', 6,'LineWidth', 2);
	plot(maxs(:,1), maxs(:,2), 'gx','MarkerSize', 6,'LineWidth', 2);
	plot(saddles(:,1), saddles(:,2), 'bo','MarkerSize', 6,'LineWidth', 2);
	legend({'min','max','saddle'},'Location','NorthEastOutside')
	title(sprintf('Gaussian, sigma = %.2f', sigma))
	axis image; axis off
	hold off;
end
