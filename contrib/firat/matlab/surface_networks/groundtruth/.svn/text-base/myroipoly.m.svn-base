% This is myroipoly.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 23, 2012

function I = myroipoly(M,N,coeff)
	K = length(coeff);
	I = zeros(M, N);
	for i = 1:K
		bw = roipoly(I);
		I = I + coeff(i)*bw;
	end
	clf;
	imagesc(I); colormap gray; axis image; axis off
end
