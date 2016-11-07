% This is /lemsvxl/contrib/firat/matlab/elbow/show_groundtruth.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 18, 2011

function show_groundtruth(file1, file2)
	V = readVTK(file1);
	W = readVTK(file2);
	i = 1;
	while 1
		subplot(1,2,1);
		imagesc(V(:,:,i)); colormap gray; axis image; axis off
		subplot(1,2,2);
		imagesc(W(:,:,i)); colormap gray; axis image; axis off	
		[x,y,b] = ginput(1);
		if b == 3 && i > 1
			i = i - 1;
		elseif	b == 2
			return
		elseif b ==1 && i < size(V,3)
			i = i + 1;
		end
	end
end

