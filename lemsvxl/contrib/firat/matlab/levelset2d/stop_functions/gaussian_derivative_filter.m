% This is /lemsvxl/contrib/firat/matlab/levelset/gaussian_derivative_filter.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 19, 2011

function [Hx, Hy] = gaussian_derivative_filter(N, sigma)
	assert(mod(N,2) == 1)
	G = fspecial('gaussian', N, sigma);
	L = (N-1)/2;
	X = repmat(-L:L, N, 1);
	Hx = G .* -X / (sigma^2);
	Hy = G .* -(X') / (sigma^2);
end

