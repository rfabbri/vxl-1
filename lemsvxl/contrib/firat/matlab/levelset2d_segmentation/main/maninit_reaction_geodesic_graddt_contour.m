% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/maninit_reaction_geodesic_graddt_contour.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 14, 2011

function phi = maninit_reaction_geodesic_graddt_contour(I, beta0, alpha, delta_t, num_iter, reinit_freq, hx, hy, edge_thresh, edge_sigma)
	[nrows,ncols] = size(I);
	narrow_band = 2;
	%%required paths
	addpath ..
	addpath ../initialization
	addpath ../tools
	addpath ../../third_order_edge_detector	
	%%speed (stop) function	
	S = 1;	
	%%edge detection
	[edg, edgemap] = third_order_edge_detector_wrapper(I, edge_sigma, edge_thresh, 1);
	%edg = lengthen_edges(edg, 1);
	figure; imagesc(I); colormap gray; axis off; axis image;
	disp_edg(edg, 'g', 0);
	pause
	close all
	num_edges = size(edg,1);
	%%geodesic input computation
	[DT,idx] = subpixelDT_fast(edg(:,1:2), nrows, ncols, hx, hy);
	%DT = abs(DT - 0.2);
	DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/(2*hx);
	DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/(2*hy);
	DTx = make_border_and_nans_zero(DTx);
	DTy = make_border_and_nans_zero(DTy);
	geodesic_input = {DTx, DTy};
	%%region of updates
	RUR = DT > (2*hx);
	GUR = ~RUR;
	%%initialization
	phi = manual_contour_initialization(I, hx, hy);
	phi = reinitlevelset_narrow(phi,narrow_band);
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
		phi_prev = phi;		
		phi = levelset2dupdate(phi, S, 'graddt', geodesic_input, beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);				
		phi = reinitlevelset_narrow(phi,narrow_band);
		if check_stop_condition(phi_prev, phi)
			break
		end
		pause(.1)
	end
	phi = remove_small_foreground_from_phi(phi, 0.005, narrow_band);
	imagesc(I); colormap gray; axis image; axis off;		
	hold on;
	C = contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;
end
