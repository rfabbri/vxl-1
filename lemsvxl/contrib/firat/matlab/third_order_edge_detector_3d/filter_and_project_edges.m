% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/filter_and_project_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 1, 2011

function edg = filter_and_project_edges(x,y,z,nx,ny,nz,slice_index,str,dtresh,stresh)
	slices = find(abs(z  - slice_index) < dtresh & str > stresh);
	x = x(slices);
	y = y(slices);
	z = z(slices);
	nx = nx(slices);
	ny = ny(slices);
	nz = nz(slices);
	projected_edges = project_edge_to_plane([x y z], [nx ny nz], [1 1 slice_index], [0 0 1]);
	edg = [projected_edges, nx ny nz];
end
