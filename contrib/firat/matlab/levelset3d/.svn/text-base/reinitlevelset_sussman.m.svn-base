% This is /lemsvxl/contrib/firat/matlab/levelset2d/reinitlevelset_sussman.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

% 3d version

function phi = reinitlevelset_sussman(phi0, h, phi_thresh)
	phi = phi0;
	epsilon = h;
	delta_t = h/10;
	
	S = phi0 ./ sqrt(phi0.^2 + epsilon^2);
	num_iter = 0;
	while num_iter == 0 || continue_condition(phi_old, phi, phi_thresh, delta_t, h)
		phi_old = phi;
		num_iter = num_iter + 1;
		a = (phi - circshift(phi, [0 1 0]));
		b =  (circshift(phi, [0 -1 0]) - phi);	
		c =  (phi - circshift(phi, [1 0 0]));
		d =  (circshift(phi, [-1 0 0]) - phi);
		e = (phi - circshift(phi, [0 0 1]));
		f =  (circshift(phi, [0  0 -1]) - phi);		
		 
		aplus = a;
		aplus(a < 0) = 0;
		aminus = -a;
		aminus(a > 0) = 0;

		bplus = b;
		bplus(b < 0) = 0;
		bminus = -b;
		bminus(b > 0) = 0;
	
		cplus = c;
		cplus(c < 0) = 0;
		cminus = -c;
		cminus(c > 0) = 0;
	
		dplus = d;
		dplus(d < 0) = 0;
		dminus = -d;
		dminus(d > 0) = 0;
		
		eplus = e;
		eplus(e < 0) = 0;
		eminus = -e;
		eminus(e > 0) = 0;
	
		fplus = f;
		fplus(f < 0) = 0;
		fminus = -f;
		fminus(f > 0) = 0;	
	
		G = (sqrt(max(aplus.^2, bminus.^2) + max(cplus.^2, dminus.^2) + max(eplus.^2, fminus.^2)) - 1) .* (phi0 > 0) + ...
			(sqrt(max(aminus.^2, bplus.^2) + max(cminus.^2, dplus.^2) + max(eminus.^2, fplus.^2)) - 1) .* (phi0 < 0);
	
		phi = phi - delta_t .* S .* G;
	end	
end

function flag = continue_condition(phi_old, phi, phi_thresh, delta_t, h)
	mask = abs(phi_old) < phi_thresh;
	M = sum(sum(sum(mask)));
	flag = sum(abs(phi_old(mask) - phi(mask)))/M >= delta_t * h^2;
end
	
		
