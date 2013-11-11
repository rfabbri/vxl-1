% This is hexsaddle.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 18, 2012

function [saddles, S] = hexsaddle(I, sigma)
	[nrows,ncols] = size(I);
	if sigma > 0
		G = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		S = imfilter(I,G,'replicate');
	else
		S = I;
	end
	mask = zeros(nrows, ncols);
	for i = 2:nrows-1
		for j = 2:ncols-1
			P = S(i-1:i+1, j-1:j+1);
			if mod(i,2) == 1				
				v = sign(P([2 6 9 8 7 4 2])-P(5));				
			else			
				v = sign(P([1 2 3 6 8 4 1])-P(5));
			end
			count  = 0;
			for k = 1:6
				count = count + (v(k)~=v(k+1));
			end
			if count == 4 || count == 6
				mask(i,j) = 1;
			end
		end
	end
	[r,c] = find(mask);
	saddles = [c r];
	%figure; imagesc(S); colormap gray; axis image; axis off; hold on;
	%plot(c,r,'ro');
	%hold off;
end
