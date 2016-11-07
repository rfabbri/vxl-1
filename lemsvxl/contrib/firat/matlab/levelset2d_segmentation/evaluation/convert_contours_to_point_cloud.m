% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/convert_contours_to_point_cloud.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 29, 2011

function convert_contours_to_point_cloud(infolder, outfile, s_index, e_index)
    num_slices = e_index - s_index + 1;	
	%load seg	
	for i = s_index:e_index		
		contours = load_contours1([infolder '/' num2str(i) '.txt']);
		C = cell2mat(contours);
		C = [C, repmat(i, size(C,1), 1)];
		dlmwrite(outfile, C, '-append', 'delimiter', ' ');
	end
end

function contours = load_contours1(filename)
    [x,y] = textread(filename,'%f%f', 'delimiter',' ');
    contours = {};
    while length(x) > 0
        l = x(1);
        cx = x(2:l+1);
        cy = y(2:l+1);
        x(1:l+1) = [];
        y(1:l+1) = [];
        contours = [contours; [cx cy]];           
    end     
end
