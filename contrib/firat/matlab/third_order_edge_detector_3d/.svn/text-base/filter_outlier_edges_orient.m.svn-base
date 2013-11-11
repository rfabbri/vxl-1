% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/filter_outlier_edges_orient.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 8, 2011

function edg = filter_outlier_edges_orient(edg, nrows, ncols)
	x = edg(:,1); y = edg(:,2); z = edg(:,3); nx = edg(:,4); ny = edg(:,5); nz = edg(:,6);
	pixel_grid = cell(nrows, ncols);
	for i = 1:size(edg,1)		
		pixel_r = round(y(i));
		pixel_c = round(x(i));
		if pixel_r > 0 && pixel_c > 0 && pixel_r <= nrows && pixel_c <= ncols
			pixel_grid{pixel_r, pixel_c} = [pixel_grid{pixel_r, pixel_c}; x(i) y(i) z(i) nx(i) ny(i) nz(i)];
		end
	end
	inliers = cell(nrows, ncols); 
	for i = 1:nrows
		for j = 1:ncols
			if size(pixel_grid{i,j},1) > 2
				inliers{i,j} = get_inliers(pixel_grid{i,j});
			end
		end
	end	
	edg = cell2mat(inliers(:));
	
end

function inlier = get_inliers(edg)	
	
	mux = mean(edg(:,4));
	muy = mean(edg(:,5));
	sigmax = std(edg(:,4),1);
	sigmay = std(edg(:,5),1);
	
	%[min_val, min_index] = min(min(abs((edg(:,4) - mux)/sigmax), abs((edg(:,5) - muy)/sigmay)));
	%inlier = edg(min_index,:);
	inlier = edg(abs((edg(:,4) - mux)/sigmax) < 1 & abs((edg(:,5) - muy)/sigmay) < 1, :);	
	%theta = atan(-edg(:,4)./edg(:,5));
	%mu = mean(theta);
	%sigma = std(theta,1);
	%inlier = edg(abs((theta - mu)/sigma) < 1, :);
end
