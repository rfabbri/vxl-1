% This is /lemsvxl/contrib/firat/matlab/levelset3d/levelset3d_drg.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 22, 2011

% levelset implementation using diffusion + reaction + geodesic


function phi = levelset3d_drg(phi, S, Sx, Sy, Sz, beta0, beta1, alpha, delta_t, num_iter)
	for n = 1:num_iter	
		if beta0 ~= 0	
			R = reaction_flux(phi, beta0);
		else
			R = 0;
		end
		if beta1 ~= 0
			[D, phi_x, phi_y, phi_z] = diffusion_flux(phi);
		else
			D = 0;
			if alpha ~= 0
				phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/2;
				phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/2;
				phi_z = (circshift(phi, [0 0 -1]) - circshift(phi, [0 0 1]))/2;
				phi_x = make_border_and_nans_zero(phi_x);
				phi_y = make_border_and_nans_zero(phi_y);
				phi_z = make_border_and_nans_zero(phi_z);
			end	
		end
		if alpha ~= 0
			G = (Sx .* phi_x) + (Sy .* phi_y) + (Sz .* phi_z);
		else
			G = 0;
		end		
		phi = phi + delta_t .* (S .* ((beta0 * R) - (beta1 * D)) + alpha .* G);
	end
end

function R = reaction_flux(phi, beta0)
	sgn = sign(beta0);
	phi_x_minus = sgn * (phi - circshift(phi, [0 1 0]));
	phi_x_plus = sgn * (circshift(phi, [0 -1 0]) - phi);	
	phi_y_minus = sgn * (phi - circshift(phi, [1 0 0]));
	phi_y_plus = sgn * (circshift(phi, [-1 0 0]) - phi);
	phi_z_minus = sgn * (phi - circshift(phi, [0 0 1]));
	phi_z_plus = sgn * (circshift(phi, [0 0 -1]) - phi);
	
	phi_x_minus(phi_x_minus > 0) = 0;
	phi_x_plus(phi_x_plus < 0) = 0;
	phi_y_minus(phi_y_minus > 0) = 0;
	phi_y_plus(phi_y_plus < 0) = 0;
	phi_z_minus(phi_z_minus > 0) = 0;
	phi_z_plus(phi_z_plus < 0) = 0;
	
	R = sqrt(phi_x_minus.^2 + phi_x_plus.^2 + phi_y_minus.^2 + phi_y_plus.^2 + phi_z_minus.^2 + phi_z_plus.^2);	
	%R = make_border_and_nans_zero(R);	
end

function [D, phi_x, phi_y, phi_z] = diffusion_flux(phi)
	phi_x = (circshift(phi, [0 -1 0]) - circshift(phi, [0 1 0]))/2;
	phi_y = (circshift(phi, [-1 0 0]) - circshift(phi, [1 0 0]))/2;
	phi_z = (circshift(phi, [0 0 -1]) - circshift(phi, [0 0 1]))/2;
	phi_xx = (circshift(phi_x, [0 -1 0]) - circshift(phi_x, [0 1 0]))/2;
	phi_yy = (circshift(phi_y, [-1 0 0]) - circshift(phi_y, [1 0 0]))/2;
	phi_zz = (circshift(phi_z, [0 0 -1]) - circshift(phi_z, [0 0 1]))/2;
	phi_xy = (circshift(phi_x, [-1 0 0]) - circshift(phi_x, [1 0 0]))/2;	
	phi_xz = (circshift(phi_x, [0 0 -1]) - circshift(phi_x, [0 0 1]))/2;
	phi_yz = (circshift(phi_y, [0 0 -1]) - circshift(phi_y, [0 0 1]))/2;
	
	phi_x2 = phi_x .^ 2;
	phi_y2 = phi_y .^ 2;
	phi_z2 = phi_z .^ 2;
	
	
	H = (phi_xx .* (phi_y2 + phi_z2) + phi_yy .* (phi_x2 + phi_z2) + phi_zz .* (phi_x2 + phi_y2) - 2.*(phi_x.*phi_y.*phi_xy + phi_y.*phi_z.*phi_yz + phi_x .* phi_z .* phi_xz)) ./ ...
		(2.*((phi_x2 + phi_y2 + phi_z2) .^ 1.5));
	
	G = (phi_x2 .* (phi_yy .* phi_zz - phi_yz.^2) + phi_y2 .* (phi_xx .* phi_zz - phi_xz.^2) + phi_z2 .* (phi_xx.*phi_yy - phi_xy.^2) ...
		+ 2.*(phi_x .* phi_y .* (phi_xz.*phi_yz - phi_xy.*phi_zz) + phi_y .* phi_z .* (phi_xy.*phi_xz - phi_yz.*phi_xx) + phi_x .* phi_z .* (phi_xy.*phi_yz - phi_xz.*phi_yy))) ./ ...
		((phi_x2 + phi_y2 + phi_z2) .^ 2);
		
	D = sign(H).*sqrt(G + abs(G)).* sqrt(phi_x2 + phi_y2 + phi_z2);	
	
	D = make_border_and_nans_zero(D);
	
	%phi_x = make_border_and_nans_zero(phi_x);
	%phi_y = make_border_and_nans_zero(phi_y);
	%phi_z = make_border_and_nans_zero(phi_z);	
end




