% This is gaussian_third_derivative_filter.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function [Gxxx, Gyyy, Gzzz, Gxxy, Gxxz, Gxyy, Gyyz, Gxzz, Gyzz, Gxyz] = gaussian_third_derivative_filter(sigma, hx, hy, hz)
	M = ceil(10*sigma);
	if ~mod(M,2)
		M = M + 1;
	end	
	L = (M-1)/2;
	[X,Y,Z] = meshgrid(-L:hx:L, -L:hy:L, -L:hz:L);	
	sigma2 = sigma ^ 2;
	sigma4 = sigma ^ 4;
    sigma6 = sigma ^ 6;
	G = exp(-(X.^2 + Y.^2 + Z.^2)/(2*sigma2));
	G = G / (sum(G(:))) / hx / hy / hz;		    
    Gxxx = ((3*X/sigma4) - (X.^3/sigma6)) .* G;
    Gyyy = ((3*Y/sigma4) - (Y.^3/sigma6)) .* G;
    Gzzz = ((3*Z/sigma4) - (Z.^3/sigma6)) .* G;
    Gxxy = ((Y/sigma4) - ((X.^2).*Y/sigma6)) .* G;
    Gxxz = ((Z/sigma4) - ((X.^2).*Z/sigma6)) .* G;
    Gxyy = ((X/sigma4) - ((Y.^2).*X/sigma6)) .* G;    
    Gyyz = ((Z/sigma4) - ((Y.^2).*Z/sigma6)) .* G;
    Gxzz = ((X/sigma4) - ((Z.^2).*X/sigma6)) .* G;
    Gyzz = ((Y/sigma4) - ((Z.^2).*Y/sigma6)) .* G;
    Gxyz = (-X.*Y.*Z / sigma6) .* G;   
end
