% This is /lemsvxl/contrib/firat/matlab/sussman_reinit/sussman2d_2nd_order.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 21, 2011

function phi = sussman2d_2nd_order(phi0, h, phi_thresh)
    k = h/10;
    S = sign(phi0);
    first_time = true;
    phi = phi0;
    while first_time || continue_condition(phi_prev, phi, phi_thresh, .001)
        first_time = false;
        phi_prev = phi;
        a = (3*phi - 4*circshift(phi, [0 1]) + circshift(phi, [0 2]))/2/h;
        a(:,1) = a(:,3);
        a(:,2) = a(:,3);
		b = (-circshift(phi, [0 -2]) + 4*circshift(phi, [0 -1]) - 3*phi)/2/h;
		b(:,end) = b(:,end-2);
		b(:,end-1) = b(:,end-2);
		c =  (3*phi - 4*circshift(phi, [1 0]) + circshift(phi, [2 0]))/2/h;
		c(1,:) = c(3,:);
		c(2,:) = c(3,:);
		d =  (-circshift(phi, [-2 0]) + 4*circshift(phi, [-1 0]) - 3*phi)/2/h;
		d(end,:) = d(end-2,:);
		d(end-1,:) = d(end-2,:);
		[aplus,aminus] = get_parts(a);
		[bplus,bminus] = get_parts(b);
		[cplus,cminus] = get_parts(c);
		[dplus,dminus] = get_parts(d);
        G = (sqrt(max(aplus.^2, bminus.^2) + max(cplus.^2, dminus.^2)) - 1) .* (phi0 > 0) + ...
			(sqrt(max(aminus.^2, bplus.^2) + max(cminus.^2, dplus.^2)) - 1) .* (phi0 < 0);
		phi = phi - k .* S .* G;
    end
    phi(phi > phi_thresh) = phi_thresh;
    phi(phi < -phi_thresh) = -phi_thresh;
end

function [p, m] = get_parts(a)
    p = a;
	p(a < 0) = 0;
	m = -a;
	m(a > 0) = 0;
end 

function flag = continue_condition(phi_prev, phi, phi_thresh, eps1)
	mask = abs(phi_prev) < phi_thresh;
	M = sum(sum(mask));
	flag = sum(sum(abs(phi_prev(mask) - phi(mask))))/M >= eps1;
end
