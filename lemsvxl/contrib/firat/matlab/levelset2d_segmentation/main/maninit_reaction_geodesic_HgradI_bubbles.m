% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/maninit_reaction_geodesic_HgradI_bubbles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function maninit_reaction_geodesic_HgradI_bubbles(I, beta0, alpha, delta_t, num_iter, reinit_freq, hx, hy)
	I = double(I)/255;
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
	S_t = 1;
	mag_gradI = sqrt(Ix.^2 + Iy.^2);
	S = 1 ./ (1 + (mag_gradI/S_t).^2);
	%%geodesic input computation
	Fx = Ixx .* Ix + Ixy .* Iy;
	Fy = Ixy .* Ix + Iyy .* Iy;
	geodesic_input = {Fx, Fy};
	%%region of updates
	mag_F = sqrt(Fx.^2 + Fy.^2);
	RUR = 1;
	%imagesc(RUR);axis image; pause
	GUR = 1;
	%%initialization
	phi = manual_bubble_initialization(I, .3, hx, hy);
	%phi = manual_contour_initialization(I, hx, hy);
	%%evolution
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
		phi = levelset2dupdate(phi, S, 'HgradI', geodesic_input, beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);		
		if(~mod(i,reinit_freq))
			phi = reinitlevelset(C, phi, nrows, ncols, hx, hy);
		end
		pause(.1)
	end
end

