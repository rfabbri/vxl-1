% This is levelset_evolution.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 20, 2012

function phi = levelset_evolution(phi, alpha, narrow_band, I, max_iter)
    [nrows,ncols] = size(I);
	%%speed (stop) function	
	S = 1;
	%%geodesic input computation
	DT = bwdist(I);
	DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/2;
	DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/2;
	DTx = make_border_and_nans_zero(DTx);
	DTy = make_border_and_nans_zero(DTy);
	geodesic_input = {DTx, DTy};
	%%region of updates	
	GUR = 1;%DT <= 3;	
	for i = 1:max_iter	
		phi_prev = phi;		
		phi = levelset2dupdate(phi, 1, 'graddt', geodesic_input, 0, 0, alpha, 1, 1, 1, 0, 0, GUR);				
		phi = reinitlevelset_narrow(phi,narrow_band);
		phi = sussman2d(phi, 1, narrow_band);
		if check_stop_condition(phi_prev, phi)
			break
		end		
	end	
end
