% This is erf_recons_1d.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 11, 2012

function erf_recons_1d(x, y, ind, val, sigma, hx)
	M = ceil(10*sigma);
	if ~mod(M,2)
		M = M + 1;
	end	
	L = (M-1)/2;
	X = -L:hx:L;
	sigma2 = sigma ^ 2;
	sigma4 = sigma ^ 4;
	sigma6 = sigma ^ 6;
	GX0 = exp(-(X.^2)/(2*sigma2)); GX0 = GX0 / (sum(GX0(:))) / hx;
	GX1 = (-X / sigma2) .* GX0;
	GX3 = ((3*X/sigma4) - (X.^3/sigma6)) .* GX0;
	z = zeros(size(x));
	z(ind) = val;
	lambda = 1;
	
	for i = 1:5000
		A = imfilter(z, GX1, 'replicate', 'conv')*hx;
		B = imfilter(log(abs(A)+100), GX3, 'replicate', 'conv')*(hx*hx*hx);
		%B = imfilter(z, GX3, 'replicate', 'conv')*(hx*hx*hx);
		z = z + lambda*B; 
		z(ind) = val;
		%clf;
		%plot(x, z);
		%drawnow
	end
	plot(x, z);
end
