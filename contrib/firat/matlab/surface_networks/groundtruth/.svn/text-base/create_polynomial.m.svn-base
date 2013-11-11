% This is create_polynomial.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 23, 2012

function I = create_polynomial(M, N, d)
	[xgrid, ygrid] = meshgrid(-(N-1)/2:(N-1)/2,-(M-1)/2:(M-1)/2);
	I = zeros(M,N);
	num_terms = unidrnd(d)+1;
	for j = 1:num_terms
		I = I + (rand-.5)*(xgrid.^(unidrnd(d+1)-1)).*(ygrid.^(unidrnd(d+1)-1));
	end
	imagesc(I); colormap gray; axis image; axis off
end
