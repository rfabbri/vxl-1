% This is levelset3dupdate.m.
%
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 18, 2011
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

function phi = levelset3dupdate(phi, S, geodesic_type, geodesic_inputs, ...
    beta0, beta1, alpha, delta_t, hx, hy, hz, R_UPDATE_REGION, ...
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
	if strcmp(geodesic_type, 'gradS')
		geodesic_term = @geodesic_gradS_term;
	elseif strcmp(geodesic_type, 'HgradI')
		geodesic_term = @geodesic_HgradI_term;
	elseif strcmp(geodesic_type, 'graddt')
		geodesic_term = @geodesic_graddt_term;
	elseif strcmp(geodesic_type, 'NHN')
		geodesic_term = @geodesic_NHN_term;
	else
		disp('Unknown geodesic term type! Assume alpha = 0!');
		alpha = 0;
	end
		
    if beta0 ~= 0	
        R = reaction_flux(phi, beta0, hx, hy, hz);			
    else
        R = 0;
    end
    if beta1 ~= 0
        [D, phi_x, phi_y, phi_z] = diffusion_flux(phi, hx, hy, hz);
    else
        D = 0;
        if alpha ~= 0
            phi_x = (circshift(phi, [0 -1 0]) - circshift(phi, [0 1 0]))...
                /(2*hx);
            phi_y = (circshift(phi, [-1 0 0]) - circshift(phi, [1 0 0]))...
                /(2*hy);
            phi_z = (circshift(phi, [0 0 -1]) - circshift(phi, [0 0 1]))...
                /(2*hz);
            phi_x = make_border_and_nans_zero(phi_x);
            phi_y = make_border_and_nans_zero(phi_y);
            phi_z = make_border_and_nans_zero(phi_z);
        end	
    end
    if alpha ~= 0
        G = geodesic_term(phi_x, phi_y, phi_z, geodesic_inputs);
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

function xflux = compute_flux_pos(phi_x_minus, phi_x_plus)
    min_phi_x_minus = phi_x_minus;
    min_phi_x_minus(phi_x_minus > 0) = 0;
    max_phi_x_plus = phi_x_plus;
    max_phi_x_plus(phi_x_plus < 0) = 0;	
    xflux = sqrt(min_phi_x_minus.^2 + max_phi_x_plus.^2);
    xdegenerate = find(min_phi_x_minus ~=  0 & max_phi_x_plus ~= 0);
    xflux(xdegenerate) = (abs(phi_x_minus(xdegenerate)) ...
        + abs(phi_x_plus(xdegenerate)))/2;
end

function xflux = compute_flux_neg(phi_x_minus, phi_x_plus)
    max_phi_x_minus = phi_x_minus;
    max_phi_x_minus(phi_x_minus < 0) = 0;
    min_phi_x_plus = phi_x_plus;
    min_phi_x_plus(phi_x_plus > 0) = 0;			
    xflux = sqrt(max_phi_x_minus.^2 + min_phi_x_plus.^2);
    xdegenerate = find(max_phi_x_minus ==  0 & min_phi_x_plus == 0);
    xflux(xdegenerate) = (abs(phi_x_minus(xdegenerate)) ...
        + abs(phi_x_plus(xdegenerate)))/2;
end

function R = reaction_flux(phi, beta0, hx, hy, hz)	
	phi_x_minus = (phi - circshift(phi, [0 1 0]))/hx;
	phi_x_plus =  (circshift(phi, [0 -1 0]) - phi)/hx;	
	phi_y_minus = (phi - circshift(phi, [1 0 0]))/hy;
	phi_y_plus = (circshift(phi, [-1 0 0]) - phi)/hy;
    phi_z_minus = (phi - circshift(phi, [0 0 1]))/hz;
	phi_z_plus = (circshift(phi, [0 0 -1]) - phi)/hz;
	
	if beta0 > 0	
		xflux = compute_flux_pos(phi_x_minus, phi_x_plus);		
		yflux = compute_flux_pos(phi_y_minus, phi_y_plus);        
        zflux = compute_flux_pos(phi_z_minus, phi_z_plus);
	else
		xflux = compute_flux_neg(phi_x_minus, phi_x_plus);		
		yflux = compute_flux_neg(phi_y_minus, phi_y_plus);        
        zflux = compute_flux_neg(phi_z_minus, phi_z_plus);
	end
	
	R = sqrt(xflux.^2 + yflux.^2 + zflux.^2);	
	R = make_border_and_nans_zero(R);	
end

function [D, phi_x, phi_y, phi_z] = diffusion_flux(phi, hx, hy, hz)
	phi_x = (circshift(phi, [0 -1 0]) - circshift(phi, [0 1 0]))/(2*hx);
	phi_y = (circshift(phi, [-1 0 0]) - circshift(phi, [1 0 0]))/(2*hy);
    phi_z = (circshift(phi, [0 0 -1]) - circshift(phi, [0 0 1]))/(2*hz);
    
    phi_x2 = phi_x .^ 2;
	phi_y2 = phi_y .^ 2;
	phi_z2 = phi_z .^ 2;
    
	phi_xx = (circshift(phi, [0 -1 0]) - (2*phi) + circshift(phi, [0 1 0]))...
        /(hx*hx);
	phi_yy = (circshift(phi, [-1 0 0]) - (2*phi) + circshift(phi, [1 0 0]))...
        /(hy*hy);
    phi_zz = (circshift(phi, [0 0 -1]) - (2*phi) + circshift(phi, [0 0 1]))...
        /(hz*hz);
	phi_xy = (circshift(phi, [-1 -1 0]) - circshift(phi, [1 -1 0]) ...
        - circshift(phi, [-1 1 0]) + circshift(phi, [1 1 0]))/(4*hx*hy);    
    phi_xz = (circshift(phi, [0 -1 -1]) - circshift(phi, [0 1 -1]) ...
        - circshift(phi, [0 -1 1]) + circshift(phi, [0 1 1]))/(4*hx*hz);
    phi_yz = (circshift(phi, [-1 0 -1]) - circshift(phi, [1 0 -1]) ...
        - circshift(phi, [-1  0 1]) + circshift(phi, [1 0 1]))/(4*hy*hz);   
    
	H = (phi_xx .* (phi_y2 + phi_z2) + phi_yy .* (phi_x2 + phi_z2) + phi_zz .* (phi_x2 + phi_y2) - 2.*(phi_x.*phi_y.*phi_xy + phi_y.*phi_z.*phi_yz + phi_x .* phi_z .* phi_xz)) ./ ...
		(2.*((phi_x2 + phi_y2 + phi_z2) .^ 1.5));
	
	G = (phi_x2 .* (phi_yy .* phi_zz - phi_yz.^2) + phi_y2 .* (phi_xx .* phi_zz - phi_xz.^2) + phi_z2 .* (phi_xx.*phi_yy - phi_xy.^2) ...
		+ 2.*(phi_x .* phi_y .* (phi_xz.*phi_yz - phi_xy.*phi_zz) + phi_y .* phi_z .* (phi_xy.*phi_xz - phi_yz.*phi_xx) + phi_x .* phi_z .* (phi_xy.*phi_yz - phi_xz.*phi_yy))) ./ ...
		((phi_x2 + phi_y2 + phi_z2) .^ 2);
		
	D = sign(H).*sqrt(G + abs(G)).* sqrt(phi_x2 + phi_y2 + phi_z2);	
	D = make_border_and_nans_zero(D);	
	phi_x = make_border_and_nans_zero(phi_x);
	phi_y = make_border_and_nans_zero(phi_y);
	phi_z = make_border_and_nans_zero(phi_z);	
end

function G = geodesic_gradS_term(phi_x, phi_y, phi_z, geodesic_inputs)
	Sx = geodesic_inputs{1};
	Sy = geodesic_inputs{2};
    Sz = geodesic_inputs{3};
	G = (Sx .* phi_x) + (Sy .* phi_y) + (Sz .* phi_z);
end

function G = geodesic_HgradI_term(phi_x, phi_y, phi_z, geodesic_inputs)
	Fx = geodesic_inputs{1};
	Fy = geodesic_inputs{2};
    Fz = geodesic_inputs{3};
	G = -(Fx.*phi_x + Fy.*phi_y + Fz.*phi_z);			
	G = make_border_and_nans_zero(G);	
end

function G = geodesic_graddt_term(phi_x, phi_y, phi_z, geodesic_inputs)
	DTx = geodesic_inputs{1};
	DTy = geodesic_inputs{2};
    DTz = geodesic_inputs{3};
	G = (DTx .* phi_x) + (DTy .* phi_y) + (DTz .* phi_z);
end

function G = geodesic_NHN_term(phi_x, phi_y, phi_z, geodesic_inputs)
	Ix = geodesic_inputs{1};
	Iy = geodesic_inputs{2};
    Iz = geodesic_inputs{3};
	Ixx = geodesic_inputs{4};
    Iyy = geodesic_inputs{5};
    Izz = geodesic_inputs{6};
	Ixy = geodesic_inputs{7};
    Ixz = geodesic_inputs{8};
    Iyz = geodesic_inputs{9};    
	sgn = sign(Ix .* phi_x + Iy .* phi_y + Iz .* phi_z);
	mag_gradphi = sqrt(phi_x.^2 + phi_y.^2 + phi_z.^2);
	NHN = (phi_x.*(Ixx.*phi_x + Ixy.*phi_y + Ixz.*phi_z) + ...
           phi_y.*(Ixy.*phi_x + Iyy.*phi_y + Iyz.*phi_z) + ...
           phi_z.*(Ixz.*phi_x + Iyz.*phi_y + Izz.*phi_z))./...
           (mag_gradphi.^2);	
	Fx = sgn .* NHN .* phi_x ./ mag_gradphi;
	Fy = sgn .* NHN .* phi_y ./ mag_gradphi;
    Fz = sgn .* NHN .* phi_z ./ mag_gradphi;			
	G = -(Fx.*phi_x + Fy.*phi_y + Fz.*phi_z);			
	G = make_border_and_nans_zero(G);	
end





