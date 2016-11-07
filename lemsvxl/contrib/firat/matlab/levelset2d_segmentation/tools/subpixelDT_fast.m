% This is /lemsvxl/contrib/firat/matlab/levelset2d/subpixelDT_fast.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 7, 2011

function [DT,idx] = subpixelDT_fast(points, maxy, maxx, hx, hy)
	[ygrid,xgrid] = meshgrid(1:hy:maxy, 1:hx:maxx);
	query_pts = [xgrid(:), ygrid(:)];
	[idx, D] = knnsearch(points, query_pts);	
	N = (maxx-1)/hx + 1;
        DT = vec2mat(D, N);	
	idx = vec2mat(idx, N);	
end

