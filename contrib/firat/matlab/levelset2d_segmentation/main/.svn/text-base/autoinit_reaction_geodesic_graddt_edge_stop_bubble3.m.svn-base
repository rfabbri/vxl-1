% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/autoinit_reaction_geodesic_graddt_edge_stop_bubble3.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

function phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble3(I, slice_index, beta0, alpha, delta_t, max_num_iter, hx, hy, narrow_band, edg, edge_length, debug_mode)	
	I_orig = I;
	I = [I(1,:);I];
	I = [I;I(end,:)];
	I = [I(:,1), I];
	I = [I, I(:,end)];
	edg(:,1:2) = edg(:,1:2) + 1;
	[nrows,ncols] = size(I);
	[edg_BG, edg_FG] = classify_edges(I, edg, 10);
	if edge_length > 0	
		edg_FG_temp = lengthen_edges([edg_FG(:,1:2) atan(-edg_FG(:,4)./edg_FG(:,5)) edg_FG(:,3)], edge_length);
		edg_FG = edg_FG_temp(:,[1:2 4]);
	end
	%%speed (stop) function	
	S = 1;	
	%%geodesic input computation
	[DT,idx] = subpixelDT_fast3(edg_FG(:,1:3), slice_index, nrows, ncols, hx, hy);
	DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/(2*hx);
	DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/(2*hy);
	%[DTx, DTy] = grad_DT(DT, edg_FG(:,1:3), idx, hx, hy);
	DTx = make_border_and_nans_zero(DTx);
	DTy = make_border_and_nans_zero(DTy);
	geodesic_input = {DTx, DTy};
	%%region of updates
	RUR = DT > (1.8*hx);
	GUR = ~RUR;
	%%initialization
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	
	phi = auto_bubble_initialization(I, edg_BG, hx, hy);
	%phi = auto_bubble_initialization(I, [3 3], hx, hy);
	phi = reinitlevelset_narrow(phi,narrow_band);
	%%evolution
	beta1 = 0;
		
	if debug_mode
		figure;
		show_segmentation_matlab(I, phi, gridx, gridy,0)	
		pause
	end	
	for i = 1:max_num_iter		
		if debug_mode
			show_segmentation_matlab(I, phi, gridx, gridy,0)	
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
		show_segmentation_matlab(I_orig, phi, gridx, gridy, 0)
	end		
end


