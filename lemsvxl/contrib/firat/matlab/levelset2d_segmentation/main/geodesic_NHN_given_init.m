% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/geodesic_NHN_given_init.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 18, 2011

function phi = geodesic_NHN_given_init(I, alpha, delta_t, num_iter, reinit_freq, hx, hy, phi0)
	narrow_band = .2;
	I = double(I);
	[nrows,ncols] = size(I);
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	I_interp = interp2(I, gridx, gridy);
	%%required paths
	addpath ../initialization
	addpath ../tools
	%%image derivatives
	sigma = 1;
	[Gx, Gy] = gaussian_derivative_filter(sigma, hx, hy);
	Ix = conv2(I_interp, Gx, 'same');
	Iy = conv2(I_interp, Gy, 'same');
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, hx, hy);	
	Ixx = conv2(I_interp, Gxx, 'same');
	Iyy = conv2(I_interp, Gyy, 'same');
	Ixy = conv2(I_interp, Gxy, 'same');	
	%%speed (stop) function	
	S = 1;
	%%geodesic input computation
	geodesic_input = {Ix, Iy, Ixx, Ixy, Iyy};
	%%region of updates	
	RUR = 0;	
	GUR = 1;
	%%initialization
	phi = phi0;
	%%evolution
	beta0 = 0;
	beta1 = 0;
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	close all
	figure;
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	pause
	hold off;
	for i = 1:num_iter		
		imagesc(I); colormap gray; axis image; axis off;		
		hold on;
		C = contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);	
		hold off;	
		title(['iter:' num2str(i)]);		
		
		phi = levelset2dupdate(phi, S, 'NHN', geodesic_input, beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);	
		phi = reinitlevelset_narrow(phi,narrow_band);	
		%if(~mod(i,reinit_freq))
		%	phi = reinitlevelset(C, phi, nrows, ncols, hx, hy);
		%end
		
		pause(.1)
	end
end


