% This is compute_areas_of_effect.m
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 29, 2012

function [Ix,Iy,gradI] = compute_areas_of_effect(I, sigma, mins, maxs, saddles)
	p = maxs([2],:);
	[nrows, ncols] = size(I);
	[Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	gradI = sqrt(Ix.^2 + Iy.^2);
	geodesic_input = {Ix./gradI, Iy./gradI};
	alpha = .05;
	beta0 = 0;
	beta1 = 0;
	narrow_band = 1;
	phi = subpixelDT_fast(p, nrows, ncols, 1, 1) - 1;
	phi_prev = zeros(nrows, ncols);
	i = 0;
	[gridx, gridy] = meshgrid(1:ncols, 1:nrows);
	if sigma > 0
		H = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		I = imfilter(I, H, 'replicate');
	end	
	show_segmentation_matlab(I, phi, gridx, gridy,0)
	for i = 1:25000
	%while ~check_stop_condition(phi_prev,phi, 0.0012)
		phi_prev = phi;		
		phi = levelset2dupdate(phi, 1, 'graddt', geodesic_input, beta0, beta1, alpha, 1, 1, 1, 1, 0, 1);				
		phi = reinitlevelset_narrow(phi,narrow_band);
		%phi = remove_small_foreground_from_phi(phi, 0.01, narrow_band);
		phi = -remove_small_foreground_from_phi(-phi, 0.01, narrow_band);
		%phi = sussman2d(phi, 1, narrow_band);
	%	pause(.1)
		show_segmentation_matlab(I, phi, gridx, gridy,0)
		drawnow
		%i = i + 1
	end
	show_segmentation_matlab(I, phi, gridx, gridy,0)
	%hold on;	
	%plot(mins(:,1), mins(:,2), 'rx','MarkerSize', 6,'LineWidth', 2);
	%plot(maxs(:,1), maxs(:,2), 'gx','MarkerSize', 6,'LineWidth', 2);
	%plot(saddles(:,1), saddles(:,2), 'bo','MarkerSize', 6,'LineWidth', 2);
	%legend({'AOE','min','max','saddle'},'Location','NorthEastOutside')
	%title(sprintf('Gaussian, sigma = %.2f', sigma))
	%axis image; axis off
	%hold off;
	title('Levelset')
end
