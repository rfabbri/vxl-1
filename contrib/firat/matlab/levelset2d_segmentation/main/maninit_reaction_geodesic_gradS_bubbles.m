% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/maninit_reaction_geodesic_gradS_bubbles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function maninit_reaction_geodesic_gradS_bubbles(I, beta0, alpha, delta_t, num_iter, reinit_freq, hx, hy)
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
	mag_gradI = sqrt(Ix.^2 + Iy.^2);
	%%speed (stop) function	
	S_t = 1;
	S = 1 ./ (1 + (mag_gradI/S_t).^2);
	%%geodesic input computation
	Sx = (circshift(S, [0 -1]) - circshift(S, [0 1]))/(2*hx);
	Sy = (circshift(S, [-1 0]) - circshift(S, [1 0]))/(2*hy);
	Sx = make_border_and_nans_zero(Sx);
	Sy = make_border_and_nans_zero(Sy);	
	geodesic_input = {Sx, Sy};
	%%region of updates
	mag_gradS = sqrt(Sx.^2 + Sy.^2);
	imagesc(mag_gradS);colormap gray; axis image;pause
	imagesc(I); colormap gray; axis image; hold on; quiver(gridx, gridy, -Sx, -Sy);hold off; pause 
	RUR = 1;
	GUR = 1;
	%%initialization
	phi = manual_bubble_initialization(I, .3, hx, hy);
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
		phi = levelset2dupdate(phi, S, 'gradS', geodesic_input, beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);		
		if(~mod(i,reinit_freq))
			phi = reinitlevelset(C, phi, nrows, ncols, hx, hy);
		end
		pause(.1)
	end
end

