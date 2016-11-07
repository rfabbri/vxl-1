% This is /lemsvxl/contrib/firat/matlab/sussman_reinit/sussman1d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 21, 2011

function phi = sussman1d(phi0, h, phi_thresh)
    k = h/10;
    S = sign(phi0);
    first_time = true;
    phi = phi0;
    while first_time || continue_condition(phi_prev, phi, phi_thresh, .00000001)
        first_time = false;
        phi_prev = phi;
        a = (phi - circshift(phi, [0 1]))/h;
        a(1) = a(2);
		b = (circshift(phi, [0 -1]) - phi)/h;
		b(end) = b(end-1);
		aplus = a;
		aplus(a < 0) = 0;
		aminus = -a;
		aminus(a > 0) = 0;
		bplus = b;
		bplus(b < 0) = 0;
		bminus = -b;
		bminus(b > 0) = 0;
        G = (sqrt(max(aplus.^2, bminus.^2)) - 1) .* (phi0 > 0) + ...
			(sqrt(max(aminus.^2, bplus.^2)) - 1) .* (phi0 < 0);
		phi = phi - k .* S .* G;
    end
end

function flag = continue_condition(phi_prev, phi, phi_thresh, eps1)
	mask = abs(phi_prev) < phi_thresh;
	M = sum(sum(mask));
	flag = sum(abs(phi_prev(mask) - phi(mask)))/M >= eps1;
end
