% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/remove_small_foreground_from_phi.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 21, 2011

function phi = remove_small_foreground_from_phi(phi, relAreaThresh, fill_value)
	totalArea = prod(size(phi));
	[L, N] = bwlabel(phi <= 0, 4);	
	props1 = regionprops(L,'Area');
	for i = 1:N
		if props1(i).Area/totalArea < relAreaThresh
			phi(L == i) = fill_value;
		end
	end
end
