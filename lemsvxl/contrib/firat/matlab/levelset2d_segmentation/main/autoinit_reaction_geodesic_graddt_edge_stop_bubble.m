% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/autoinit_reaction_geodesic_graddt_edge_stop_bubble.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 22, 2011

function phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble(I, beta0, alpha, delta_t, max_num_iter, hx, hy, edge_thresh, edge_sigma, reg_mu, reg_sigma, narrow_band, edg, debug_mode)	
	I = double(I);
	I_orig = I;
	I = 255*bfilter2(I/255);
	I = [I(1,:);I];
	I = [I;I(end,:)];
	I = [I(:,1), I];
	I = [I, I(:,end)];
	[nrows,ncols] = size(I);
	%%speed (stop) function	
	S = 1;
	%%edge detection
	if isnan(edg)
		[edg, edgemap] = third_order_edge_detector_wrapper(I, edge_sigma, edge_thresh, 1);
		edg = lengthen_edges(edg, 1);
		if debug_mode
			figure; imagesc(I); colormap gray; axis off; axis image;
			disp_edg(edg, 'g', 0);
			pause	
		end
	end
	%%geodesic input computation
	[DT,idx] = subpixelDT_fast(edg(:,1:2), nrows, ncols, hx, hy);
	DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/(2*hx);
	DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/(2*hy);
	DTx = make_border_and_nans_zero(DTx);
	DTy = make_border_and_nans_zero(DTy);
	geodesic_input = {DTx, DTy};
	%%region of updates
	RUR = DT > (2*hx);
	GUR = ~RUR;
	%%initialization	
	phi = auto_bubble_initialization(I, edg, 10, hx, hy);
	phi = reinitlevelset_narrow(phi,narrow_band);
	%%evolution
	beta1 = 0;
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);	
	if debug_mode
		figure;
		show_segmentation_matlab(I, phi, gridx, gridy)	
		pause
	end	
	for i = 1:max_num_iter		
		if debug_mode
			show_segmentation_matlab(I, phi, gridx, gridy)	
			title(['iter:' num2str(i)]);
			pause(.1)
		end	
		phi_prev = phi;		
		phi = levelset2dupdate(phi, S, 'graddt', geodesic_input, beta0, beta1, alpha, delta_t, hx, hy,RUR,1,GUR);				
		phi = reinitlevelset_narrow(phi,narrow_band);
		if check_stop_condition(phi_prev, phi)
			break
		end		
	end
	fprintf('Number of iterations (): %d\n', i);
	
	phi(:,1:(1/hx)) = [];
	phi(1:(1/hy),:) = [];
	phi(:,end-(1/hx)+1:end) = [];
	phi(end-(1/hy)+1:end,:) = [];
	phi = remove_small_foreground_from_phi(phi, 0.001, narrow_band);
	phi = -remove_small_foreground_from_phi(-phi, 0.001, narrow_band);		
	if debug_mode
		[gridx, gridy] = meshgrid(1:hx:ncols-2, 1:hy:nrows-2);		
		show_segmentation_matlab(I_orig, phi, gridx, gridy)
	end		
end

