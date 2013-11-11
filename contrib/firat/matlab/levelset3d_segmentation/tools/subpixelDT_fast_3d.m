% This is /lemsvxl/contrib/firat/matlab/levelset2d/subpixelDT_fast.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 7, 2011

function [DT,idx] = subpixelDT_fast_3d(points, maxy, maxx, maxz, hx, hy, hz)
	[xgrid,ygrid,zgrid] = meshgrid(1:hx:maxx, 1:hx:maxy, 1:hz:maxz);
	query_pts = [xgrid(:), ygrid(:), zgrid(:)];
	[idx, D] = knnsearch(points, query_pts);	
	Nx = (maxx-1)/hx + 1;
    Ny = (maxy-1)/hy + 1;
    Nz = (maxz-1)/hz + 1;
    DT = reshape(D, [Ny Nx Nz]);    
	idx = reshape(idx, [Ny Nx Nz]);	
end

