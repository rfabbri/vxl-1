% This is /lemsvxl/contrib/firat/matlab/levelset2d/stop_functions/stop_edge_detection.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 27, 2011

function S = stop_edge_detection(I, N, sigma, T)
	[Gx, Gy] = gaussian_derivative_filter(N, sigma);
	[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(N, sigma);
	Ix = conv2(I, Gx, 'same');
	Iy = conv2(I, Gy, 'same');
	Ixx = conv2(I, Gxx, 'same');
	Iyy = conv2(I, Gyy, 'same');
	Ixy = conv2(I, Gxy, 'same');	
	A = ((Ix.^2) .* Ixx) + (2 .* Ix .* Iy .* Ixy) + ((Iy.^2) .* Iyy);
	%S = exp(-A/T);
	S = 1 ./ (1 + (A/T));
end

