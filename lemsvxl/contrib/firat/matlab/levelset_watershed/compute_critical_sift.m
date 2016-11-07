% This is compute_critical_sift.m.

% Created on: Apr 3, 2013
%     Author: firat

%computes sift descriptors in the vlfeat format

function [min_fra, min_sift, max_fra, max_sift, saddle_fra, saddle_sift] = compute_critical_sift(I, sigma, mins, maxs, saddles)
	[min_fra,min_sift] = compute_desc(I, sigma, mins);
	[max_fra, max_sift] = compute_desc(I, sigma, maxs);
	[saddle_fra,saddle_sift] = compute_desc(I, sigma, saddles);	
end

function [fra,desc] = compute_desc(I, sigma, pts)
	f = [pts'; sigma*ones(1, size(pts,1)); zeros(1, size(pts,1))];
	[fra, desc] = vl_sift(single(I), 'frames', f, 'orientations');
end
