% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/filter_edges_strength.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 29, 2011

function [x y z nx ny nz]=filter_edges_strength(x,y,z,nx,ny,nz,str,sthresh)
    slices = find(str > sthresh);
    x = x(slices);
	y = y(slices);
	z = z(slices);
	nx = nx(slices);
	ny = ny(slices);
	nz = nz(slices);
	
end
