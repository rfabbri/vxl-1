% This is /lemsvxl/contrib/firat/matlab/levelset/levelset2d_drg.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 20, 2011

% levelset implementation using diffusion + reaction + geodesic terms

% function phi = levelset2d_drg(phi, S, geodesic_type, geodesic_inputs, beta0, beta1, alpha, delta_t, num_iter)
%
%
% phi: current levelset function
% S: stop function
% geodesic_type: 'snake' or 'proposed'
% geodesic_inputs: if 'snake' -> {Sx, Sy}
%				   if 'proposed' -> {Ix, Iy, Ixx, Ixy, Iyy}
% beta0: coefficient of the reaction term
% beta1: coefficient of the diffusion term
% alpha: coefficient of the geodesic term

function phi = levelset2d_drg(phi, S, geodesic_type, geodesic_inputs, beta0, beta1, alpha, delta_t, num_iter, R_UPDATE_REGION, D_UPDATE_REGION, G_UPDATE_REGION)
	if ~exist('R_UPDATE_REGION', 'var')
		R_UPDATE_REGION = 1;
	end
	if ~exist('D_UPDATE_REGION', 'var')
		D_UPDATE_REGION = 1;
	end
	if ~exist('G_UPDATE_REGION', 'var')
		G_UPDATE_REGION = 1;
	end
	if strcmp(geodesic_type, 'traditional')
		geodesic_term = @geodesic_traditional_term;
	elseif strcmp(geodesic_type, 'graddt')
		geodesic_term = @geodesic_graddt_term;
	elseif strcmp(geodesic_type, 'NHN')
		geodesic_term = @geodesic_NHN_term;
	else
		disp('Unknown geodesic term type! Assume alpha = 0!');
		alpha = 0;
	end
	for n = 1:num_iter	
		if beta0 ~= 0	
			R = reaction_flux(phi, beta0);			
		else
			R = 0;
		end
		if beta1 ~= 0
			[D, phi_x, phi_y] = diffusion_flux(phi);
		else
			D = 0;
			if alpha ~= 0
				phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/2;
				phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/2;
				phi_x = make_border_and_nans_zero(phi_x);
				phi_y = make_border_and_nans_zero(phi_y);
			end	
		end
		if alpha ~= 0
			G = geodesic_term(phi_x, phi_y, geodesic_inputs);
		else
			G = 0;
		end		
		phi = phi + delta_t .*  (S .* ((R_UPDATE_REGION .* beta0 .* R) - (D_UPDATE_REGION .* beta1 .* D)) + G_UPDATE_REGION .* alpha .* G);
	end
end

function R = reaction_flux(phi, beta0)	
	phi_x_minus = (phi - circshift(phi, [0 1]));
	phi_x_plus =  (circshift(phi, [0 -1]) - phi);	
	phi_y_minus = (phi - circshift(phi, [1 0]));
	phi_y_plus = (circshift(phi, [-1 0]) - phi);
	
	if beta0 > 0	
		min_phi_x_minus = phi_x_minus;
		min_phi_x_minus(phi_x_minus > 0) = 0;
		max_phi_x_plus = phi_x_plus;
		max_phi_x_plus(phi_x_plus < 0) = 0;	
		xflux = sqrt(min_phi_x_minus.^2 + max_phi_x_plus.^2);
		xdegenerate = find(min_phi_x_minus ~=  0 & max_phi_x_plus ~= 0);
		xflux(xdegenerate) = (abs(phi_x_minus(xdegenerate)) + abs(phi_x_plus(xdegenerate)))/2;
		
		min_phi_y_minus = phi_y_minus;
		min_phi_y_minus(phi_y_minus > 0) = 0;
		max_phi_y_plus = phi_y_plus;
		max_phi_y_plus(phi_y_plus < 0) = 0;	
		yflux = sqrt(min_phi_y_minus.^2 + max_phi_y_plus.^2);
		ydegenerate = find(min_phi_y_minus ~=  0 & max_phi_y_plus ~= 0);
		yflux(ydegenerate) = (abs(phi_y_minus(ydegenerate)) + abs(phi_y_plus(ydegenerate)))/2;				
	else
		max_phi_x_minus = phi_x_minus;
		max_phi_x_minus(phi_x_minus < 0) = 0;
		min_phi_x_plus = phi_x_plus;
		min_phi_x_plus(phi_x_plus > 0) = 0;			
		xflux = sqrt(max_phi_x_minus.^2 + min_phi_x_plus.^2);
		xdegenerate = find(max_phi_x_minus ==  0 & min_phi_x_plus == 0);
		xflux(xdegenerate) = (abs(phi_x_minus(xdegenerate)) + abs(phi_x_plus(xdegenerate)))/2;
		
		max_phi_y_minus = phi_y_minus;
		max_phi_y_minus(phi_y_minus < 0) = 0;
		min_phi_y_plus = phi_y_plus;
		min_phi_y_plus(phi_y_plus > 0) = 0;			
		yflux = sqrt(max_phi_y_minus.^2 + min_phi_y_plus.^2);
		ydegenerate = find(max_phi_y_minus ==  0 & min_phi_y_plus == 0);
		yflux(ydegenerate) = (abs(phi_y_minus(ydegenerate)) + abs(phi_y_plus(ydegenerate)))/2;
	end
	
	R = sqrt(xflux.^2 + yflux.^2);
	R = make_border_and_nans_zero(R);	
end

function [D, phi_x, phi_y] = diffusion_flux(phi)
	phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/2;
	phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/2;
	phi_xx = circshift(phi, [0 -1]) - (2*phi) + circshift(phi, [0 1]);
	phi_yy = circshift(phi, [-1 0]) - (2*phi) + circshift(phi, [1 0]);
	phi_xy = (circshift(phi, [-1 -1]) - circshift(phi, [1 -1]) - circshift(phi, [-1 1]) + circshift(phi, [1 1]))/4;	
	D = ((phi_xx .* (phi_y.^2)) - (2*phi_y.*phi_x.*phi_xy) + (phi_yy.*(phi_x.^2)))./(((phi_x.^2) + (phi_y.^2)));
	D = make_border_and_nans_zero(D);
	phi_x = make_border_and_nans_zero(phi_x);
	phi_y = make_border_and_nans_zero(phi_y);	
end

function G = geodesic_traditional_term(phi_x, phi_y, geodesic_inputs)
	Sx = geodesic_inputs{1};
	Sy = geodesic_inputs{2};
	G = (Sx .* phi_x) + (Sy .* phi_y);
end

function G = geodesic_graddt_term(phi_x, phi_y, geodesic_inputs)
	DTx = geodesic_inputs{1};
	DTy = geodesic_inputs{2};
	%hold on;quiver(-DTx, -DTy);pause
	G = (DTx .* phi_x) + (DTy .* phi_y);
end

function G = geodesic_NHN_term(phi_x, phi_y, geodesic_inputs)
	Ix = geodesic_inputs{1};
	Iy = geodesic_inputs{2};
	Ixx = geodesic_inputs{3};
	Ixy = geodesic_inputs{4};
	Iyy = geodesic_inputs{5};
	
	sgn = sign(Ix .* phi_x + Iy .* phi_y);
	mag_gradphi = sqrt(phi_x.^2 + phi_y.^2);
	NHN = (((phi_x.^2) .* Ixx) + (2 .* phi_x .* phi_y .* Ixy) + ((phi_y.^2) .* Iyy))./(mag_gradphi.^2);	
	
	Fx = sgn .* NHN .* phi_x ./ mag_gradphi;
	Fy = sgn .* NHN .* phi_y ./ mag_gradphi;
		
	G = -(Fx.*phi_x + Fy.*phi_y);	
	%G = -sign(Ix .* phi_x + Iy .* phi_y) .* (((phi_x.^2) .* Ixx) + (2 .* phi_x .* phi_y .* Ixy) + ((phi_y.^2) .* Iyy))...
	% ./ sqrt(phi_x.^2 + phi_y.^2);		
	G = make_border_and_nans_zero(G);	
end




