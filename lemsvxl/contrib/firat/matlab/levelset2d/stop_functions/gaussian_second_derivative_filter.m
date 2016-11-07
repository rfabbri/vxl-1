% This is /lemsvxl/contrib/firat/matlab/levelset2d/stop_functions/gaussian_second_derivative_filter.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

function [Hxx, Hyy, Hxy] = gaussian_second_derivative_filter(N, sigma)
	assert(mod(N,2) == 1)
	G = fspecial('gaussian', N, sigma);
	L = (N-1)/2;
	X = repmat(-L:L, N, 1);
	Y =  X';
	Hxx = ((X.^2 / sigma^4) - (1/sigma^2)) .* G;
	Hyy = ((Y.^2 / sigma^4) - (1/sigma^2)) .* G;
	Hxy = (X .* Y / sigma^4) .* G;
end
