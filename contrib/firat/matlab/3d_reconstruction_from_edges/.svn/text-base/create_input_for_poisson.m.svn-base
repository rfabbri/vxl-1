% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/create_input_for_poisson.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 16, 2011

function opts = create_input_for_poisson(vtk_file, edg_mat_file, stresh, ctresh, outfile)
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/Max;
	[nrows,ncols,nbands] = size(V);
	load(edg_mat_file);
	slices = find(str > stresh & x >= 1 & x <= ncols & y >= 1 & y <= nrows & z >= 1 & z <= nbands);
	x = x(slices);
	y = y(slices);
	z = z(slices);
	nx = nx(slices);
	ny = ny(slices);
	nz = nz(slices);
	[edg_BG, edg_FG] = classify_edges_3d(V, [x y z nx ny nz], ctresh);
	N = edg_FG(:,4:6);
	N_mag = sqrt(sum(N.^2,2));
	N = -N ./ repmat(N_mag, 1, 3); 
	opts = [edg_FG(:,1:3) N];
	%dlmwrite(outfile, [edg_FG(:,1:3) N], 'delimiter',' ');
end
