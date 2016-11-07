% This is /lemsvxl/contrib/firat/matlab/levelset/levelset2.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 19, 2011

function phi = levelset2(phi, S, beta0, beta1, delta_t, num_iter)
	for n = 1:num_iter		
		R = reaction_flux(phi);
		D = diffusion_flux(phi);		
		phi = phi + delta_t .* S .* ((beta0 * R) - (beta1 * D));
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
end

function D = diffusion_flux(phi)
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
end

function D = diffusion_flux2(phi)
	phi_x = conv2(phi,[1 0 -1]/2,'same');
	phi_y = conv2(phi,[1; 0; -1]/2,'same');
	phi_xx = conv2(phi,[1 -2 1]/2,'same');
	phi_yy = conv2(phi,[1; -2; 1]/2,'same');
	phi_xy = conv2(phi,[-1 0 1; 0 0 0; 1 0 -1]/4,'same');
	
	D = ((phi_xx .* (phi_y.^2)) - (2*phi_y.*phi_x.*phi_xy) + (phi_yy.*(phi_x.^2)))./(((phi_x.^2) + (phi_y.^2)));
	
	D(1:2,:) = 0;
	D(end-1:end,:) = 0;
	D(:,1:2) = 0;
	D(:, end-1:end) = 0;
	D(isnan(D)) = 0;	
	
end


