% This is trace_eno_watershed.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 26, 2012

function trace_eno_watershed(M)
	phi = bwdist(M > 0);
	figure; imagesc(M); colormap gray; axis image; axis off; hold on
	contour(phi, [1,1], 'r')
	
end


