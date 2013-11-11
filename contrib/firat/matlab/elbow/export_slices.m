% This is /lemsvxl/contrib/firat/matlab/elbow/export_slices.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 1, 2011

function export_slices(V, s, outfolder)
	if ~exist(outfolder, 'dir')
		mkdir(outfolder);
	end
	V = double(V);
	Max = max(V(:));
	Min = min(V(:));
	V = uint8(255 * (V - Min)/Max);
	for i = 1:size(V,3)
		if s == 1
			imwrite(V(:,:,i), [outfolder '/' num2str(i) '.png']);
		else
			imwrite(imresize(V(:,:,i),s,'bicubic'), [outfolder '/' num2str(i) '.png']);
		end
	end
end

