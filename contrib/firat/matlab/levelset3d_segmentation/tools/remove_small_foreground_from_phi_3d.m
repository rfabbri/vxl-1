% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/remove_small_foreground_from_phi.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 21, 2011

function phi = remove_small_foreground_from_phi_3d(phi, relAreaThresh, fill_value)
	totalArea = prod(size(phi));
	CC = bwconncomp(phi <= 0, 6);	
	N = CC.NumObjects;
	props1 = regionprops(CC,'Area');
	for i = 1:N
		if props1(i).Area/totalArea < relAreaThresh
			phi(CC.PixelIdxList{i}) = fill_value;
		end
	end
end
