% This is /lemsvxl/contrib/firat/matlab/levelset/levelset2d_drg.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 20, 2011

% levelset implementation using diffusion + reaction + geodesic


function phi = levelset2d_drg(phi, S, Sx, Sy, beta0, beta1, alpha, delta_t, num_iter)
	for n = 1:num_iter	
		if beta0 ~= 0	
			R = reaction_flux(phi);
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
			G = (Sx .* phi_x) + (Sy .* phi_y);
		else
			G = 0;
		end		
		phi = phi + delta_t .* (S .* ((beta0 * R) - (beta1 * D)) + alpha .* G);
	end
end

function R = reaction_flux(phi)
	phi_x_minus = phi - circshift(phi, [0 1]);
	phi_x_plus = circshift(phi, [0 -1]) - phi;	
	phi_y_minus = phi - circshift(phi, [1 0]);
	phi_y_plus = circshift(phi, [-1 0]) - phi;
	
	phi_x_minus(phi_x_minus > 0) = 0;
	phi_x_plus(phi_x_plus < 0) = 0;
	phi_y_minus(phi_y_minus > 0) = 0;
	phi_y_plus(phi_y_plus < 0) = 0;
	
	R = sqrt(phi_x_minus.^2 + phi_x_plus.^2 + phi_y_minus.^2 + phi_y_plus.^2);	
	R = make_border_and_nans_zero(R);	
end

function [D, phi_x, phi_y] = diffusion_flux(phi)
	phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/2;
	phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/2;
	phi_xx = (circshift(phi_x, [0 -1]) - circshift(phi_x, [0 1]))/2;
	%phi_xx = conv2(phi, [1 -2 1], 'same')/2;
	phi_yy = (circshift(phi_y, [-1 0]) - circshift(phi_y, [1 0]))/2;
	%phi_yy = conv2(phi, [1; -2; 1], 'same')/2;
	phi_xy = (circshift(phi_x, [-1 0]) - circshift(phi_x, [1 0]))/2;	
	D = ((phi_xx .* (phi_y.^2)) - (2*phi_y.*phi_x.*phi_xy) + (phi_yy.*(phi_x.^2)))./(((phi_x.^2) + (phi_y.^2)));
	D = make_border_and_nans_zero(D);
	phi_x = make_border_and_nans_zero(phi_x);
	phi_y = make_border_and_nans_zero(phi_y);	
end




