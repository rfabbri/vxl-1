% This is /lemsvxl/contrib/firat/matlab/levelset/levelset2dupdate.m.
%
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 07, 2011
%
% levelset update equation using diffusion + reaction + geodesic terms
%
% INPUT PARAMETERS:
%
% phi (in): current levelset function
% S: stop function
% geodesic_type: 'traditional', 'graddt', 'NHN'
% geodesic_inputs: if 'traditional' -> {Sx, Sy} (partial derivatives of
%                                                       the stop function)
%				   if 'NHN' -> {Ix, Iy, Ixx, Ixy, Iyy} 
%                                                (image gradient + hessian)
% beta0: coefficient of the reaction term
% beta1: coefficient of the diffusion term
% alpha: coefficient of the geodesic term
% delta_t: scale of the update
% hx: grid spacing for x-axis
% hy: grid spacing for y-axis
% R_UPDATE_REGION: binary mask showing the pixels to be updated under the
%                  effect of the reaction term
% D_UPDATE_REGION: binary mask showing the pixels to be updated under the
%                  effect of the diffuion term 
% G_UPDATE_REGION: binary mask showing the pixels to be updated under the
%                  effect of the geodesic term
%
% OUTPUT PARAMETER:
%
% phi (out): updated levelset function

function phi = levelset2dupdate(phi, S, geodesic_type, geodesic_inputs, ...
    beta0, beta1, alpha, delta_t, hx, hy, R_UPDATE_REGION, ...
    D_UPDATE_REGION, G_UPDATE_REGION)
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
		
    if beta0 ~= 0	
        R = reaction_flux(phi, beta0, hx, hy);			
    else
        R = 0;
    end
    if beta1 ~= 0
        [D, phi_x, phi_y] = diffusion_flux(phi, hx, hy);
    else
        D = 0;
        if alpha ~= 0
            phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))...
                /(2*hx);
            phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))...
                /(2*hy);
            phi_x = make_border_and_nans_zero(phi_x);
            phi_y = make_border_and_nans_zero(phi_y);
        end	
    end
    if alpha ~= 0
        G = geodesic_term(phi_x, phi_y, geodesic_inputs);
    else
        G = 0;
    end		
    phi = phi + delta_t .*  (S .* (...
          (R_UPDATE_REGION .* beta0 .* R)...
        - (D_UPDATE_REGION .* beta1 .* D)...
                                  ) ...
        + (G_UPDATE_REGION .* alpha .* G)...
        );
	
end

function R = reaction_flux(phi, beta0, hx, hy)	
	phi_x_minus = (phi - circshift(phi, [0 1]))/hx;
	phi_x_plus =  (circshift(phi, [0 -1]) - phi)/hx;	
	phi_y_minus = (phi - circshift(phi, [1 0]))/hy;
	phi_y_plus = (circshift(phi, [-1 0]) - phi)/hy;
	
	if beta0 > 0	
		min_phi_x_minus = phi_x_minus;
		min_phi_x_minus(phi_x_minus > 0) = 0;
		max_phi_x_plus = phi_x_plus;
		max_phi_x_plus(phi_x_plus < 0) = 0;	
		xflux = sqrt(min_phi_x_minus.^2 + max_phi_x_plus.^2);
		xdegenerate = find(min_phi_x_minus ~=  0 & max_phi_x_plus ~= 0);
		xflux(xdegenerate) = (abs(phi_x_minus(xdegenerate)) ...
            + abs(phi_x_plus(xdegenerate)))/2;
		
		min_phi_y_minus = phi_y_minus;
		min_phi_y_minus(phi_y_minus > 0) = 0;
		max_phi_y_plus = phi_y_plus;
		max_phi_y_plus(phi_y_plus < 0) = 0;	
		yflux = sqrt(min_phi_y_minus.^2 + max_phi_y_plus.^2);
		ydegenerate = find(min_phi_y_minus ~=  0 & max_phi_y_plus ~= 0);
		yflux(ydegenerate) = (abs(phi_y_minus(ydegenerate)) ...
            + abs(phi_y_plus(ydegenerate)))/2;				
	else
		max_phi_x_minus = phi_x_minus;
		max_phi_x_minus(phi_x_minus < 0) = 0;
		min_phi_x_plus = phi_x_plus;
		min_phi_x_plus(phi_x_plus > 0) = 0;			
		xflux = sqrt(max_phi_x_minus.^2 + min_phi_x_plus.^2);
		xdegenerate = find(max_phi_x_minus ==  0 & min_phi_x_plus == 0);
		xflux(xdegenerate) = (abs(phi_x_minus(xdegenerate)) ...
            + abs(phi_x_plus(xdegenerate)))/2;
		
		max_phi_y_minus = phi_y_minus;
		max_phi_y_minus(phi_y_minus < 0) = 0;
		min_phi_y_plus = phi_y_plus;
		min_phi_y_plus(phi_y_plus > 0) = 0;			
		yflux = sqrt(max_phi_y_minus.^2 + min_phi_y_plus.^2);
		ydegenerate = find(max_phi_y_minus ==  0 & min_phi_y_plus == 0);
		yflux(ydegenerate) = (abs(phi_y_minus(ydegenerate)) ...
            + abs(phi_y_plus(ydegenerate)))/2;
	end
	
	R = sqrt(xflux.^2 + yflux.^2);
	%imagesc(R);colormap gray; axis image; pause
	R = make_border_and_nans_zero(R);	
end

function [D, phi_x, phi_y] = diffusion_flux(phi, hx, hy)
	phi_x = (circshift(phi, [0 -1]) - circshift(phi, [0 1]))/(2*hx);
	phi_y = (circshift(phi, [-1 0]) - circshift(phi, [1 0]))/(2*hy);
	phi_xx = (circshift(phi, [0 -1]) - (2*phi) + circshift(phi, [0 1]))...
        /(hx*hx);
	phi_yy = (circshift(phi, [-1 0]) - (2*phi) + circshift(phi, [1 0]))...
        /(hy*hy);
	phi_xy = (circshift(phi, [-1 -1]) - circshift(phi, [1 -1]) ...
        - circshift(phi, [-1 1]) + circshift(phi, [1 1]))/(4*hx*hy);	
	D = ((phi_xx .* (phi_y.^2)) - (2*phi_y.*phi_x.*phi_xy) + ...
        (phi_yy.*(phi_x.^2)))./(((phi_x.^2) + (phi_y.^2)));
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
	NHN = (((phi_x.^2) .* Ixx) + (2 .* phi_x .* phi_y .* Ixy) ...
        + ((phi_y.^2) .* Iyy))./(mag_gradphi.^2);	
	Fx = sgn .* NHN .* phi_x ./ mag_gradphi;
	Fy = sgn .* NHN .* phi_y ./ mag_gradphi;
	%[gridx, gridy] = meshgrid(1:.5:100, 1:.5:100);
	%hold on; quiver(gridx, gridy,Fx, Fy);hold off;		
	G = -(Fx.*phi_x + Fy.*phi_y);			
	G = make_border_and_nans_zero(G);	
end





