% This is detect_saddles_circle.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 17, 2012

function saddles = detect_saddles_circle(I, sigma)
	[nrows,ncols] = size(I);
	G = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
	S = imfilter(I,G,'replicate');
	t1 = [linspace(0, 2*pi, 20)']; 
	t1 = [t1; t1(2)];	
	p1 = .75*[cos(t1) sin(t1)];
	
	mask = zeros(nrows, ncols);
	for i = 3:nrows-2
		for j = 3:ncols-2
			A = repmat([j i], length(t1), 1) + p1;
			V = interp2(S, A(:,1), A(:,2), '*cubic');
			[pks,maxind] = findpeaks(V);
			[pks,minind] = findpeaks(-V);
			if length(maxind) == 2 && length(minind) == 2 && (maxind(1) - minind(2) < 0 || minind(1) - maxind(2) < 0)
				mask(i,j) = 1;			
			end
		end
	end
	[r,c] = find(mask);
	saddles = [c, r];
end
