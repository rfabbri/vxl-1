% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/experiments/compare_2d_3d_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 1, 2011

function compare_2d_3d_edges(V, slice_index, x, y, z, nx, ny, nz, str, dthresh, sthresh, show_lines)
	figure; imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
	hold on;
	
	[edg, edgemap] = third_order_edge_detector_wrapper(V(:,:,slice_index), 1, sthresh, 1);
	disp_edg(edg, 'g', 1);
	hold on	
	[nrows,ncols,nbands] = size(V);
	projected_edges = filter_and_project_edges(x,y,z,nx,ny,nz,slice_index,str,dthresh,sthresh);
	%projected_edges = filter_outlier_edges_orient(projected_edges, nrows, ncols);
	
	x = projected_edges(:,1);
	y = projected_edges(:,2);
	z = projected_edges(:,3);
	nx = projected_edges(:,4);
	ny = projected_edges(:,5);
	magnxy = sqrt(nx.^2 + ny.^2);
	nx = nx ./ magnxy;
	ny = ny ./ magnxy;
	dx = ny;
	dy = -nx;
	m = length(x);	
	
	if show_lines
		for i = 1:m 
			plot([x(i)+0.5*dx(i), x(i)-0.5*dx(i)], [y(i)+0.5*dy(i), y(i)-0.5*dy(i)], 'r-')
		end
	else
		plot(x,y,'r.');
	end
	hold off
	
	%title(sprintf('Green: 2d edges / Red: Projected 3d edges / Slice: %d', slice_index))

end
