% This is /lemsvxl/contrib/firat/matlab/levelset2d/stop_functions/gaussian_second_derivative_filter.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function [Gxx, Gyy, Gzz, Gxy, Gxz, Gyz] = gaussian_second_derivative_filter(sigma, hx, hy, hz)
	M = ceil(10*sigma);
	if ~mod(M,2)
		M = M + 1;
	end	
	L = (M-1)/2;
	[X,Y,Z] = meshgrid(-L:hx:L, -L:hy:L, -L:hz:L);	
	sigma2 = sigma ^ 2;
	sigma4 = sigma ^ 4;
	G = exp(-(X.^2 + Y.^2 + Z.^2)/(2*sigma2));
	G = G / (sum(G(:))) / hx / hy / hz;	
	Gxx = ((X.^2 / sigma4) - (1/sigma2)) .* G;
	Gyy = ((Y.^2 / sigma4) - (1/sigma2)) .* G;
	Gzz = ((Z.^2 / sigma4) - (1/sigma2)) .* G;
    Gxy = (X .* Y / sigma4) .* G;
    Gxz = (X .* Z / sigma4) .* G;
    Gyz = (Z .* Y / sigma4) .* G;
end
