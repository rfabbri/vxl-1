% This is reinitlevelset.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 8, 2011

function I = reinitlevelset(C, phi, maxy, maxx, hx, hy)
	L = size(C,2);
	delete_mask = zeros(1, L);	
	i = 1;
	while i < L
		delete_mask(i) = 1;
		i = i + C(2,i) + 1;		
	end
	C(:, find(delete_mask)) = [];
	I = subpixelDT_fast(C', maxy, maxx, hx, hy);
	I(phi < 0) = -I(phi < 0);
end
