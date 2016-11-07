% This is compute_hills_dales.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 17, 2012

function [hills, dales, J] = compute_hills_dales(I, sigma)
	[S, mins, maxs, saddles] = get_min_max_saddle(I, sigma);
	[nrows, ncols] = size(I);
	[gridx, gridy] = meshgrid(1:ncols, 1:nrows);
	[Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	if sigma > 0
		H = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		J = imfilter(I, H, 'replicate');
	else
		J = I;
	end
	gradI = sqrt(Ix.^2 + Iy.^2);
	force = {Ix./gradI, Iy./gradI};
	hills = zeros(nrows,ncols,size(maxs,1));
	for i = 1:size(maxs,1)
		hills(:,:,i) = get_hill_dale(I, sigma, maxs(i,:), Ix./gradI, Iy./gradI, gridx, gridy, 1);
	end
	dales = zeros(nrows,ncols,size(mins,1));
	for i = 1:size(mins,1)
		dales(:,:,i) = get_hill_dale(I, sigma, mins(i,:), Ix./gradI, Iy./gradI, gridx, gridy, -1);
	end
end

function mask = get_hill_dale(I, sigma, p, Fx, Fy, gridx, gridy, direction);
	force = {direction*Fx, direction*Fy};
	[nrows, ncols] = size(I);
	alpha = .3;
	beta0 = 0;
	beta1 = 0;
	narrow_band = 1;
	phi = subpixelDT_fast(p, nrows, ncols, 1, 1) - 1;
	phi_prev = zeros(nrows, ncols);
	i = 0;
		
	%show_segmentation_matlab(I, phi, gridx, gridy,0)
	while ~check_stop_condition(phi_prev,phi, 0.01)
		phi_prev = phi;		
		phi = levelset2dupdate(phi, 1, 'graddt', force, beta0, beta1, alpha, 1, 1, 1, 0, 0, 1);				
		phi = reinitlevelset_narrow(phi,narrow_band);
		%phi = remove_small_foreground_from_phi(phi, 0.01, narrow_band);
		phi = -remove_small_foreground_from_phi(-phi, 0.01, narrow_band);
		%phi = sussman2d(phi, 1, narrow_band);
		%pause(.1)
		%show_segmentation_matlab(I, phi, gridx, gridy,0)
		%i = i + 1
	end
	mask = phi <= 0;
	%pause
end
