% This is synthetic.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 17, 2012

function I = synthetic(M, N, num, points, values)
	[xgrid,ygrid] = meshgrid(-(N-1)/2:(N-1)/2, -(M-1)/2:(M-1)/2);
	if ~exist('points','var')
		points = zeros(num, 2);
		points(:,1) = unidrnd(N, num, 1);
		points(:,2) = unidrnd(M, num, 1);
		values = 2*rand(num, 1)-1;
	end
	points(:,1) = points(:,1) - (N-1)/2 - 1;
	points(:,2) = points(:,2) - (M-1)/2 - 1;
	I = zeros(M, N);
	for i = 1:size(points, 1)
		G = gaussian(xgrid, ygrid, points(i,1), points(i,2), values(i));
		%[ign, Ma] = max([abs(I(:)'); abs(G(:)')]);
		%Ma = reshape(Ma, [M N]);
		%I = I.*(Ma == 1) + G.*(Ma == 2);
		I = I + G;
	end
	%imagesc(I); colormap gray; axis image; axis off
end

function G = gaussian(xgrid, ygrid, x0, y0, v)
	sigma = diag(1*rand(1,2)+5).^2;
	sigma(2:3) = 4*rand - 2;	
	invsigma = inv(sigma)/2;
	a = invsigma(1); c = invsigma(2); b = invsigma(3); d = invsigma(4);
	T = (a*(xgrid - x0) + c*(ygrid - y0)).*(xgrid - x0) + (b*(xgrid - x0) + d*(ygrid - y0)).*(ygrid - y0);		
	G = exp(-T);
	G = G ./ max(G(:)) * v;
end
