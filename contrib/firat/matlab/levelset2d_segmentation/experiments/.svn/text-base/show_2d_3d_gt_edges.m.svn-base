% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/experiments/show_2d_3d_gt_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 30, 2011

function show_2d_3d_gt_edges(V, cx, cy, cz, radius, slice_index, x, y, z, nx, ny, nz, str, dthresh, sthresh, show_lines)
	figure; imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
	phi = subpixelDT_fast([cx cy], size(V,1), size(V,2), 1, 1) - sqrt(max(radius^2 - (slice_index - cz)^2,0));
	
	hold on;
	
	
	[edg, edgemap] = third_order_edge_detector_wrapper(V(:,:,slice_index), 1, sthresh, 1);
	disp_edg(edg, 'g', 1);
	hold on
	slices = find(abs(z  - slice_index) < dthresh & str > sthresh);
	x = x(slices);
	y = y(slices);
	z = z(slices);
	nx = nx(slices);
	ny = ny(slices);
	nz = nz(slices);
	edg = [x y z nx ny nz];
	projected_edges = project_edge_to_plane([x y z], [nx ny nz], [1 1 slice_index], [0 0 1]);
	x = projected_edges(:,1);
	y = projected_edges(:,2);
	z = projected_edges(:,3);
	u = unique(x);
	%a1 = find(x == u(1));
	%x(a1) = [];
	%y(a1) = [];
	%z(a1) = [];
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
	
	contour(phi, [0,0], 'b', 'LineWidth', 2);
	hold off
	%title(sprintf('Blue: ground truth / Green: 2d edges / Red: Projected 3d edges / Slice: %d', slice_index))
	
	
	

	
	
	if 0
	
	[gridx, gridy] = meshgrid(1:.5:size(V,2), 1:.5:size(V,1));	
	phi_seg = autoinit_reaction_geodesic_graddt_edge_stop_bubble3(V(:,:,slice_index), slice_index, -2, .15, 1, 1000, .5, .5, 2, edg, 0, 0);
	figure; imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
	hold on;
	contour(phi, [0,0], 'b');
	contour(gridx, gridy,phi_seg, [0,0], 'r');
	hold off;
	
		[gridx, gridy] = meshgrid(1:.5:size(V,2), 1:.5:size(V,1));	
		phi_seg = autoinit_reaction_geodesic_graddt_edge_stop_bubble2(V(:,:,slice_index), -2, .15, 1, 1000, .5, .5, 2, [x y z], 0, 0);
		figure; imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
		hold on;
		contour(phi, [0,0], 'b');
		contour(gridx, gridy,phi_seg, [0,0], 'r');
		hold off;	
	
		[gridx, gridy] = meshgrid(1:.5:size(V,2), 1:.5:size(V,1));	
		phi_seg = autoinit_reaction_geodesic_graddt_edge_stop_bubble2(V(:,:,slice_index), -2, .15, 1, 1000, .5, .5, 2, edg, 0, 0);
		figure; imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
		hold on;
		contour(phi, [0,0], 'b');
		contour(gridx, gridy,phi_seg, [0,0], 'r');
		hold off;
	
	
	
	
		DT = subpixelDT_fast3([x y z], slice_index, size(V,1), size(V,2), 1, 1);
		DTx = (circshift(DT, [0 -1]) - circshift(DT, [0 1]))/(2);
		DTy = (circshift(DT, [-1 0]) - circshift(DT, [1 0]))/(2);
		figure; imagesc(DT); colormap gray; axis image; axis off
		hold on
		contour(phi, [0,0], 'b');
		hold off
	
		figure; imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
		hold on;
		contour(phi, [0,0], 'b');
		quiver(-DTx, -DTy,'r');
		hold off
		
		
	end
end
	
