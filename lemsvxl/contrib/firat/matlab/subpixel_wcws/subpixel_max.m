% This is subpixel_max.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 12, 2012

function max_loc = subpixel_max(f)
	M = imregionalmax(f);
	L = bwlabel(M);
	maxL = max(L(:));
	max_loc = zeros(maxL,2);
	[H,W] = size(f);
	for i = 1:maxL
		neig = imdilate(L==i, strel('square',3));		
		[y,x] = find(neig);		
		ind = sub2ind([H,W], y, x);
		z = f(ind);
		p = polyfitn([x,y],z,2);
		max_loc(i,:) = ([2*p.Coefficients(1) p.Coefficients(2); p.Coefficients(2) 2*p.Coefficients(4)]\[-p.Coefficients(3); -p.Coefficients(5)])';
	end
end
