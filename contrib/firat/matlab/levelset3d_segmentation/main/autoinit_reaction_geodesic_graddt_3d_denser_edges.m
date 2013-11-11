% This is /lemsvxl/contrib/firat/matlab/levelset3d_segmentation/main/autoinit_reaction_geodesic_graddt_3d_denser_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Dec 1, 2011

function phi = autoinit_reaction_geodesic_graddt_3d_denser_edges(I, beta0, alpha, delta_t, max_num_iter, hx, hy, hz, narrow_band, edg, edge_length, debug_mode)	
	I_orig = I;
	I = padarray(I, [1 1 1], 'replicate');
	edg(:,1:3) = edg(:,1:3) + 1;
	[nrows,ncols, nbands] = size(I);
	%[edg_BG, edg_FG] = classify_edges_3d(I, edg, 10);
	%plot3(edg_FG(:,1),edg_FG(:,2),edg_FG(:,3),'r.')
	%pause
	edg_FG = edg;
	edg_BG = [1,1,1];
	%%speed (stop) function	
	S = 1;	
	%%geodesic input computation
	if edge_length > 0
	    edg_FG = make_edgemap_denser(edg_FG, edge_length);
	end
	[DT,idx] = subpixelDT_fast_3d(edg_FG(:,1:3), nrows, ncols, nbands, hx, hy, hz);
	DTx = (circshift(DT, [0 -1 0]) - circshift(DT, [0 1 0]))/(2*hx);
	DTy = (circshift(DT, [-1 0 0]) - circshift(DT, [1 0 0]))/(2*hy);
	DTz = (circshift(DT, [0 0 -1]) - circshift(DT, [0 0 1]))/(2*hz);
	%[DTx, DTy] = grad_DT(DT, edg_FG(:,1:3), idx, hx, hy);
	DTx = make_border_and_nans_zero(DTx);
	DTy = make_border_and_nans_zero(DTy);
	DTz = make_border_and_nans_zero(DTz);
	geodesic_input = {DTx, DTy, DTz};
	%%region of updates
	RUR = DT > (1.8*hx);
	GUR = ~RUR;
	%%initialization
	[gridx, gridy, gridz] = meshgrid(1:hx:ncols, 1:hy:nrows, 1:hz:nbands);
	
	phi = auto_bubble_initialization_3d(I, edg_BG, hx, hy, hz);
	%phi = auto_bubble_initialization_3d(I, [3 3 3], hx, hy, hz);
	phi = reinitlevelset_narrow(phi,1.5*narrow_band);	
	phi = sussman3d(phi, hx, narrow_band);
	
	
	%%evolution
	beta1 = 0;
	%visualize_foreground(phi, 0);	
	for i = 1:max_num_iter		
		i
		phi_prev = phi;		
		phi = levelset3dupdate(phi, S, 'graddt', geodesic_input, beta0, beta1, alpha, delta_t, hx, hy, hz, RUR,1,GUR);				
		phi = reinitlevelset_narrow(phi,1.5*narrow_band);
		phi = sussman3d(phi, hx, narrow_band);
		%pause(.1)
		%clf
		%visualize_foreground(phi, 0);
		if check_stop_condition_3d(phi_prev, phi)
			break
		end		
	end
	fprintf('Number of iterations (): %d\n', i);
	
	phi(:,1:(1/hx),:) = [];
	phi(1:(1/hy),:,:) = [];
	phi(:,:,1:(1/hz)) = [];
	phi(:,end-(1/hx)+1:end,:) = [];
	phi(end-(1/hy)+1:end,:,:) = [];
	phi(:,:,end-(1/hz)+1:end) = [];
	%phi = remove_small_foreground_from_phi(phi, 0.001, narrow_band);
	%phi = -remove_small_foreground_from_phi(-phi, 0.001, narrow_band);		
end
