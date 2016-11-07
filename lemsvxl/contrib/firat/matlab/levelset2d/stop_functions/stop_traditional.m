% This is /lemsvxl/contrib/firat/matlab/levelset2d/stop_functions/stop_traditional.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

function S = stop_traditional(I, N, sigma, T, m)
	[Gx, Gy] = gaussian_derivative_filter(N, sigma);
	Ix = conv2(I, Gx, 'same');
	Iy = conv2(I, Gy, 'same');
	J = sqrt(Ix .^2 + Iy .^2);
	S = 1 ./ (1 + (J/T).^m);	
	
end

