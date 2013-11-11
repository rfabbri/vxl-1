% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/polygon_distance_transform.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 15, 2011

function phi = polygon_distance_transform(contours, height, width, hx, hy)
	alphabet = char('a' + (0:25));
	rand_in = ['/tmp/' alphabet(unidrnd(26,1,10)) '.ctr'];
	rand_out = ['/tmp/' alphabet(unidrnd(26,1,10)) '.phi'];
	num_poly = length(contours);
	in_cell = cell(2*num_poly,1);
	for i = 1:num_poly
		contour1 = contours{i};
		l = size(contour1, 1);
		in_cell{2*i-1} = [l 0];
		in_cell{2*i} = contour1-1;
	end
	in_mat = cell2mat(in_cell);
	dlmwrite(rand_in, in_mat, 'delimiter', ' ');
	cmd = sprintf('!/home/firat/lemsvxl/bin/contrib/firat/polygon_distance_transform/compute_polygon_distance_transform %s %d %d %f %f %s', rand_in, height, width, hx, hy, rand_out);
	eval(cmd);
	phi = load(rand_out);
	cmd2 = sprintf('!rm -f %s %s', rand_in, rand_out);
	eval(cmd2);
end

