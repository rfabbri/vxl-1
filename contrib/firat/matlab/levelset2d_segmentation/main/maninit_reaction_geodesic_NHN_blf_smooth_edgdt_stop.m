% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/maninit_reaction_geodesic_NHN_blf_smooth_edgdt_stop.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 21, 2011

function phi = maninit_reaction_geodesic_NHN_blf_smooth_edgdt_stop(I, beta0, alpha, delta_t, num_iter, reinit_freq, hx, hy, edge_thresh, edge_sigma)
	[nrows,ncols] = size(I);
	narrow_band = .5;
	I = double(I);
	maxI = max(I(:));
	B = bfilter2(I/maxI, 10, [3 .1])*maxI;
	figure; imagesc(B); colormap gray; axis image; pause
	%%required paths
	addpath ../initialization
	addpath ../tools
	addpath ../../third_order_edge_detector	
	%%speed (stop) function	
	S = 1;	
	%%edge detection
	[edg, edgemap] = third_order_edge_detector_wrapper(B, edge_sigma, edge_thresh, 1);
	edg = lengthen_edges(edg, 1);
	figure; imagesc(I); colormap gray; axis off; axis image;
	disp_edg(edg, 'g', 0);
	pause
	close all
	num_edges = size(edg,1);
	%%geodesic input computation
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	I_interp = interp2(B, gridx, gridy);
	sigma = 1;
	[Gx, Gy] = gaussian_derivative_filter(sigma, hx, hy);
	Ix = imfilter(I_interp, Gx, 'replicate');
	Iy = imfilter(I_interp, Gy, 'replicate');
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, hx, hy);	
	Ixx = imfilter(I_interp, Gxx, 'replicate');
	Iyy = imfilter(I_interp, Gyy, 'replicate');
	Ixy = imfilter(I_interp, Gxy, 'replicate');	
	geodesic_input = {Ix, Iy, Ixx, Ixy, Iyy};
	%%region of updates
	DT = subpixelDT_fast(edg(:,1:2), nrows, ncols, hx, hy);
	DT = imfilter(DT, fspecial('gaussian',15, 2), 'replicate');
	RUR = DT > (2*hx);
	GUR = ~RUR;
	%%initialization
	phi = manual_contour_initialization(I, hx, hy);
	phi = reinitlevelset_narrow(phi,narrow_band);
	%%evolution
	beta1 = 0;
	
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
		pause(.1)
	end
end
