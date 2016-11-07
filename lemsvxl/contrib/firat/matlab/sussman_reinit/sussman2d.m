% This is /lemsvxl/contrib/firat/matlab/sussman_reinit/sussman2d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 21, 2011

function phi = sussman2d(phi0, h, phi_thresh)
    k = h/10;
    S = sign(phi0);
    first_time = true;
    phi = phi0;
    while first_time || continue_condition(phi_prev, phi, phi_thresh, .01)
        first_time = false;
        phi_prev = phi;
        a = (phi - circshift(phi, [0 1]))/h;
        a(:,1) = a(:,2);
		b = (circshift(phi, [0 -1]) - phi)/h;
		b(:,end) = b(:,end-1);
		c =  (phi - circshift(phi, [1 0]))/h;
		c(1,:) = c(2,:);
		d =  (circshift(phi, [-1 0]) - phi)/h;
		d(end,:) = d(end-1,:);
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
