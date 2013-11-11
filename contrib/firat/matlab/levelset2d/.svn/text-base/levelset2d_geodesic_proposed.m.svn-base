% This is /lemsvxl/contrib/firat/matlab/levelset2d/levelset2d_geodesic_proposed.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

function [phi, phi_x, phi_y] = levelset2d_geodesic_proposed(phi, Ix, Iy, Ixx, Ixy, Iyy, alpha, delta_t, num_iter)
	for n = 1:num_iter	
		phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/2;
		phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/2;
		A = -sign(Ix .* phi_x + Iy .* phi_y);
		G = A .* (((phi_x.^2) .* Ixx) + (2 .* phi_x .* phi_y .* Ixy) + ((phi_y.^2) .* Iyy)) ./ sqrt(phi_x.^2 + phi_y.^2);
		G = make_border_and_nans_zero(G);
		%G(isnan(G)) = 0;
		%Fx = - phi_x .* G ./ (phi_x.^2 + phi_y.^2);
		%Fy = - phi_y .* G ./ (phi_x.^2 + phi_y.^2);
		phi = phi + delta_t .* alpha .* G;
	end
end

