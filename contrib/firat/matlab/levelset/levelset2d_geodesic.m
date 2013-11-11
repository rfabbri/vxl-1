% This is /lemsvxl/contrib/firat/matlab/levelset/levelset2d_geodesic.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 18, 2011

function phi = levelset2d_geodesic(phi, S, Sx, Sy, beta1, delta_t, num_iter)
	[D, phi_x, phi_y] = diffusion_flux(phi);
	phi = phi + delta_t .* ((beta1 .* S .* D) + (Sx .* phi_x) + (Sy .* phi_y));
end

function [D, phi_x, phi_y] = diffusion_flux(phi)
	phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/2;
	phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/2;
	phi_xx = (circshift(phi_x, [0 -1]) - circshift(phi_x, [0 1]))/2;
	phi_yy = (circshift(phi_y, [-1 0]) - circshift(phi_y, [1 0]))/2;
	phi_xy = (circshift(phi_x, [-1 0]) - circshift(phi_x, [1 0]))/2;
	
	D = ((phi_xx .* (phi_y.^2)) - (2*phi_y.*phi_x.*phi_xy) + (phi_yy.*(phi_x.^2)))./(((phi_x.^2) + (phi_y.^2)));
	

	D(1:2,:) = 0;
	D(end-1:end,:) = 0;
	D(:,1:2) = 0;
	D(:, end-1:end) = 0;
	D(isnan(D)) = 0;
	
	
	
	%figure(21);imagesc(D); colormap gray;pause(.5);close(21)
end

