function [oriented_points, phi3d, points]  = generate_oriented_point_cloud(seg_folder, start_index, end_index, hx, hy)

	addpath ../curve_descriptors_and_matching

	for k = start_index:end_index
		load([seg_folder '/' num2str(k) '.mat']);
		if ~exist('height','var')
			[t1, t2] = size(phi);
			height = (t1-1)*hy+1;
			width = (t2-1)*hx+1;
			phi3d = zeros(t1, t2, end_index-start_index+1);
			points = cell(end_index-start_index+1,1);
			[gridx, gridy] = meshgrid(1:hx:width, 1:hy:height);
		end
		C = contour(gridx, gridy, phi, [0,0]);
		close all;
		polygons = {};
		orig_points = {};
		L = size(C,2);		
		i = 1;
		while i < L		
			vertices = C(:,i+1:i+C(2,i)-1)';
			sampled_curve_coords = sample_curve(vertices, 81, 0);
			orig_points = [orig_points; vertices];
			polygons = [polygons; sampled_curve_coords];
			i = i + C(2,i) + 1;			
		end	
		temp_mat = cell2mat(orig_points);
		points{k-start_index+1} = [temp_mat, repmat(k,size(temp_mat,1),1)];
		phi3d(:,:,k-start_index+1) = polygon_distance_transform(polygons, height, width, hx, hy);
	end

	phi_x = (circshift(phi3d, [0 -1 0]) - circshift(phi3d, [0 1 0]))/(2*hx);
	phi_y = (circshift(phi3d, [-1 0 0]) - circshift(phi3d, [1 0 0]))/(2*hy);
	phi_z = (circshift(phi3d, [0 0 -1]) - circshift(phi3d, [0 0 1]))/2;

	phi_x(:,1,:) = (phi3d(:,2,:) - phi3d(:,1,:))/hx;
	phi_y(1,:,:) = (phi3d(2,:,:) - phi3d(1,:,:))/hy; 
	phi_z(:,:,1) = (phi3d(:,:,2) - phi3d(:,:,1));

	phi_x(:,end,:) = (phi3d(:,end,:) - phi3d(:,end-1,:))/hx;
	phi_y(end,:,:) = (phi3d(end,:,:) - phi3d(end-1,:,:))/hy; 
	phi_z(:,:,end) = (phi3d(:,:,end) - phi3d(:,:,end-1));


	points = cell2mat(points);

	[X,Y,Z] = meshgrid(1:hx:width, 1:hy:height, start_index:end_index);

	nx = interp3(X,Y,Z, phi_x, points(:,1), points(:,2), points(:,3));
	ny = interp3(X,Y,Z, phi_y, points(:,1), points(:,2), points(:,3));
	nz = interp3(X,Y,Z, phi_z, points(:,1), points(:,2), points(:,3));

	N = [nx ny nz];
	N_mag = sqrt(sum(N.^2,2));
	N = N ./ repmat(N_mag, 1, 3);

	oriented_points = [points N];

end


