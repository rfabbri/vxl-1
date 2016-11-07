% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/gaussian_1d_derivative_filters.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 2, 2011

function [GX0, GY0, GZ0, GX1, GY1, GZ1, GX2, GY2, GZ2, GX3, GY3, GZ3] = gaussian_1d_derivative_filters(sigma, hx, hy, hz)
	M = ceil(10*sigma);
	if ~mod(M,2)
		M = M + 1;
	end	
	L = (M-1)/2;
	[X,Y,Z] = meshgrid(-L:hx:L, -L:hy:L, -L:hz:L);
	X = X(1,:,1);
	Y = Y(:,1,1);
	Z = Z(1,1,:); 
	sigma2 = sigma ^ 2;
	sigma4 = sigma ^ 4;
	sigma6 = sigma ^ 6;
	GX0 = exp(-(X.^2)/(2*sigma2)); GX0 = GX0 / (sum(GX0(:))) / hx;
	GY0 = exp(-(Y.^2)/(2*sigma2)); GY0 = GY0 / (sum(GY0(:))) / hy;
	GZ0 = exp(-(Z.^2)/(2*sigma2)); GZ0 = GZ0 / (sum(GZ0(:))) / hz;	
	GX1 = (-X / sigma2) .* GX0;
	GY1 = (-Y / sigma2) .* GY0;	
	GZ1 = (-Z / sigma2) .* GZ0;	
	GX2 = ((X.^2 / sigma4) - (1/sigma2)) .* GX0;
	GY2 = ((Y.^2 / sigma4) - (1/sigma2)) .* GY0;
	GZ2 = ((Z.^2 / sigma4) - (1/sigma2)) .* GZ0; 
	GX3 = ((3*X/sigma4) - (X.^3/sigma6)) .* GX0;
	GY3 = ((3*Y/sigma4) - (Y.^3/sigma6)) .* GY0;
	GZ3 = ((3*Z/sigma4) - (Z.^3/sigma6)) .* GZ0;
end
