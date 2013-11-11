% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/choose_boundary_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 29, 2011

function opts = choose_boundary_edges(seg_folder, edg_mat_file, s_index, e_index, hx, hy, neg, pos, sthresh)
    num_slices = e_index - s_index + 1;
	first_time = true;	
	%load seg	
	for i = s_index:e_index
		Seg = load([seg_folder '/' num2str(i) '.mat']);
		if first_time
			first_time = false;
			[nrows,ncols] = size(Seg.phi);	
			phi3d = zeros(nrows, ncols, num_slices);		
		end
		phi3d(:,:,i) = Seg.phi;
	end
	load(edg_mat_file);
	[gridx, gridy, gridz] = meshgrid(1:hx:(ncols-1)*hx+1, 1:hy:(nrows-1)*hy+1, s_index:e_index);
	values = interp3(gridx,gridy,gridz,phi3d,x,y,z);
	slices = find(values >= neg & values <= pos & str > sthresh);
	x = x(slices);
	y = y(slices);
	z = z(slices);
	nx = nx(slices);
	ny = ny(slices);
	nz = nz(slices);
	N = [nx ny nz];
	N_mag = sqrt(sum(N.^2,2));
	N = -N ./ repmat(N_mag, 1, 3);
	opts = [x y z N];
end
