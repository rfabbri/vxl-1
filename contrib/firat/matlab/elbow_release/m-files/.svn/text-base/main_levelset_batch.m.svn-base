function main_levelset_batch(V, edg, objectname, origin, spacing)
	global start_index end_index;
	[nrows,ncols,nslices] = size(V);
	levelset_params = load_parameters(fullfile('..','parameters','levelset_parameters.txt'));
	hx = str2num(levelset_params.hx);
	hy = str2num(levelset_params.hy);
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	num_slices = end_index - start_index + 1;
	x = edg(:,1); y = edg(:,2); z = edg(:,3);
	nx = edg(:,4); ny = edg(:,5); nz = edg(:,6);
	str = edg(:,7);
	dthresh =  str2num(levelset_params.slice_distance_thresh);
	sthresh =  str2num(levelset_params.edge_thresh);	
	beta0 =  str2num(levelset_params.beta0);
	beta1 =  str2num(levelset_params.beta1);
	alpha =  str2num(levelset_params.alpha);
	max_num_iter =  str2num(levelset_params.max_iter);	
	narrow_band =  str2num(levelset_params.narrow_band);
	edge_length =  str2num(levelset_params.edge_length);
	phi = zeros(size(gridx,1), size(gridx,2), num_slices);
	for i = start_index:end_index
		slices = find(abs(z  - i) < dthresh & str > sthresh);
		x1 = x(slices);
		y1 = y(slices);
		z1 = z(slices);
		nx1 = nx(slices);
		ny1 = ny(slices);
		nz1 = nz(slices);
		edg1 = [x1 y1 z1 nx1 ny1 nz1];
		phi(:,:,i-start_index+1) = autoinit_reaction_geodesic_graddt_edge_stop_bubble_with_sussman(V(:,:,i), i, beta0, alpha, 1, max_num_iter, hx, hy, narrow_band, edg1, edge_length, 0);		
		%clf;show_segmentation_matlab(V(:,:,i), phi(:,:,i-start_index+1), gridx, gridy, 0);		
	end
	%seg = phi >= 0;	
	%save(fullfile('..','binary_segmentations',[objectname '_segmentation_' datestr(now,30) '.mat']), 'phi', 'seg', 'start_index', 'end_index');
	[gridx, gridy, gridz] = meshgrid(1:hx:ncols, 1:hy:nrows, start_index:end_index);
	values = interp3(gridx, gridy, gridz, phi, x, y, z, 'linear');
	selected_index = find(str >= sthresh & values > -narrow_band);
	selected = edg(selected_index, 1:6);	
	clusters = cluster_edges(selected, sthresh);
	selected = get_largest_cluster(clusters);
	selected_file = fullfile('..','oriented_points', [objectname '_levelset_' datestr(now,30) '.xyz']);
	selected_coordinates = ((selected(:,1:3)-1).*repmat(spacing, size(selected, 1), 1))+repmat(origin, size(selected, 1), 1);
	dlmwrite(selected_file, [selected_coordinates selected(:,4:6)], 'delimiter', ' ');		
end
