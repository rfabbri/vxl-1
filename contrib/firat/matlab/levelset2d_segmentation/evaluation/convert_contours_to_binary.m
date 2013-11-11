% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/convert_gt_to_binary.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 12, 2011

function convert_contours_to_binary(infolder, outfolder, phi_nrows, phi_ncols, hx, hy, s_index, e_index)
	num_slices = e_index - s_index + 1;
	first_time = true;
	%load seg	
	for i = s_index:e_index
		if first_time
			first_time = false;
			if ~exist(outfolder,'dir')
				mkdir(outfolder);
			end
		end
		contours = load_contours1([infolder '/' num2str(i) '.txt']);
		BW = zeros(phi_nrows, phi_ncols);
		for  j = 1:length(contours)
			BW = BW | poly2mask((contours{j}(:,1)-1)/hx+1, (contours{j}(:,2)-1)/hy+1, phi_nrows, phi_ncols);
		end
		L = bwlabel(BW == 0);
		props1 = regionprops(L, 'Area');
		[Max, ind] = max(cat(1,props1.Area));
		BW(BW == 0 & L ~= ind) = 1;
		save([outfolder '/' num2str(i) '.mat'], 'BW');
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
