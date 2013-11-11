% This is /lemsvxl/contrib/firat/matlab/levelset/gaussian_derivative_filter.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function [Gx, Gy] = gaussian_derivative_filter(sigma, hx, hy)	
	M = ceil(10*sigma);
	if ~mod(M,2)
		M = M + 1;
	end	
	L = (M-1)/2;
	[X,Y] = meshgrid(-L:hx:L, -L:hy:L);	
	sigma2 = sigma ^ 2;
	G = exp(-(X.^2 + Y.^2)/(2*sigma2));
	G = G / (sum(G(:))) / hx / hy;		
	Gx = G .* -X / sigma2;
	Gy = G .* -Y / sigma2;
end

