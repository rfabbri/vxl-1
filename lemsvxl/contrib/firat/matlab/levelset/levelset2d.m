% This is /lemsvxl/contrib/firat/matlab/levelset/levelset2d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 13, 2011

% phi: initial level set function
% S: stop function
% num_iter: number of iterations
% beta0, beta1: coefficients

function phi = levelset2d(phi, S, beta0, beta1, delta_t, num_iter)
	for n = 1:num_iter		
		R = reaction_flux(phi);
		%figure;plot(R(50,:)); title('Horizontal');
		%figure;plot(R(:,50)); title('Vertical');
		%figure;plot(diag(R)); title('Diagonal');
		%max(R(:))
		%min(R(:))
		%pause
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
	
	R(1:2,:) = 0;
	R(end-1:end,:) = 0;
	R(:,1:2) = 0;
	R(:, end-1:end) = 0;
	R(isnan(R)) = 0;	
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


