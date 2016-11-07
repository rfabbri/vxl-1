% This is /lemsvxl/contrib/firat/matlab/contour_related/save_contours.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 20, 2011

function save_contours(contours, out_file)
	num_poly = length(contours);
	in_cell = cell(2*num_poly,1);
	for i = 1:num_poly
		contour1 = contours{i}(:,1:2);
		l = size(contour1, 1);
		in_cell{2*i-1} = [l 0];
		in_cell{2*i} = contour1;
	end
	in_mat = cell2mat(in_cell);
	dlmwrite(out_file, in_mat, 'delimiter', ' ');
end
