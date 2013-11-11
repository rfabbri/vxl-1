% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/subpixelDT_fast3.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

function [DT,idx] = subpixelDT_fast3(points, z, maxy, maxx, hx, hy)
	[ygrid,xgrid] = meshgrid(1:hy:maxy, 1:hx:maxx);
	X = xgrid(:);
	Y = ygrid(:);
	Z = z*ones(size(X));
	query_pts = [X, Y, Z];
	[idx, D] = knnsearch(points, query_pts);	
	N = (maxx-1)/hx + 1;
    %DT = vec2mat(D, N);
    len = (maxy-1)/hy + 1;	
	DT = reshape(D, [N len])';
	%idx = vec2mat(idx, N);	
	idx = reshape(idx, [N len])';
end
